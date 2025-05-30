/*************************************************************************\
*                  Copyright (C) Michael Kerrisk, 2025.                   *
*                                                                         *
* This program is free software. You may use, modify, and redistribute it *
* under the terms of the GNU General Public License as published by the   *
* Free Software Foundation, either version 3 or (at your option) any      *
* later version. This program is distributed without any warranty.  See   *
* the file COPYING.gpl-v3 for details.                                    *
\*************************************************************************/

/* Supplementary program for Chapter 17 */

/* acl_update.c

   Perform updates on the access control lists (ACLs) of files named in the
   command line.  This program provides a subset of the functionality of the
   setfacl(1) command. For usage, see usageError() below.

   This program is Linux-specific. ACLs are supported since Linux 2.6.
   To build this program, you must have the ACL library (libacl) installed
   on your system.
*/
#include <sys/acl.h>
#include <stdbool.h>
#include "ugid_functions.h"
#include "tlpi_hdr.h"

#define MAX_ENTRIES 10000   /* Maximum entries that we can handle in an ACL */

struct AccessControlEntry {     /* Represent a single ACL entry */
    acl_tag_t   tag;            /* Tag type */
    id_t        qual;           /* Optional tag qualifier (UID or GID) */
    int         perms;          /* Permissions bit mask */
};

enum Operation {
    unspecifiedOp, modifyEntries, removeEntries, removeDefaultACL,
    checkValidACL
};

static void
usageError(char *progName, char *msg, bool shortUsage)
{
    if (msg != NULL)
        fprintf(stderr, "%s\n", msg);

    if (shortUsage) {
        fprintf(stderr, "Type '%s --help' for usage information\n", progName);
        exit(EXIT_FAILURE);
    }

    fprintf(stderr, "Usage: %s -m acl [-d] [-n] file...\n", progName);
    fprintf(stderr, "   or: %s -x acl [-d] [-n] file...\n", progName);
    fprintf(stderr, "   or: %s -k file...\n\n", progName);
    fprintf(stderr, "   or: %s -V acl\n\n", progName);
#define fpe(msg) fprintf(stderr, "      " msg);
    fpe("-m  Modify/create ACL entries\n");
    fpe("-x  Remove ACL entries\n");
    fpe("-k  Remove default ACL\n\n");
    fpe("-V  Check validity of 'acl'\n\n");
    fpe("'acl' consists of one or more comma-separated entries of the form:\n");
    fpe("\n            tag:[qualifier][:[perms]]\n\n");
    fpe("      'perms' are specified only for -m\n\n");
    fpe("-d  Apply operation to default ACL\n");
    fpe("-n  Don't recalculate mask entry\n");
    fpe("      NOTE: if you specify this option and have specified\n");
    fpe("      the -m option, then you may encounter errors if the\n");
    fpe("      file does not already have a mask entry.\n");
    exit(EXIT_FAILURE);
}

/* Parse an ACL entry specification (the null-terminated string
   provided in 'entryStr') of the form:

        tag:[qualifier][:[permissions]]

   returning the parsed information in the structure 'ace'.

   If 'permsReqd' is set, then the ACL entries must contain
   permission specifications (i.e., a colon followed by at least
   one of [-rwx], otherwise they must not.

   Return true if the specification parsed okay, or false otherwise. */

static bool
parseEntrySpec(char *entryStr, struct AccessControlEntry *ace,
               bool permsReqd)
{
    char *colon1 = strchr(entryStr, ':');
    if (colon1 == NULL) {
        fprintf(stderr, "Missing initial colon in ACL entry: %s\n", entryStr);
        return false;
    }

    bool hasQual = *(colon1 + 1) != '\0' && *(colon1 + 1) != ':';

    *colon1 = '\0';     /* Add terminator to tag type */

    char *colon2 = strchr(colon1 + 1, ':');
    bool hasPerms = colon2 != NULL && *(colon2 + 1) != '\0';

    if (hasPerms && !permsReqd) {
        fprintf(stderr, "Cannot specify permissions here\n");
        return false;
    }

    if (!hasPerms && permsReqd) {
        fprintf(stderr, "Must specify permissions\n");
        return false;
    }

    /* Determine tag type, depending on tag string and presence
       of qualifier after the first ':' */

    if (strcmp(entryStr, "u") == 0 || strcmp(entryStr, "user") == 0)
        ace->tag = hasQual ? ACL_USER : ACL_USER_OBJ;
    else if (strcmp(entryStr, "g") == 0 || strcmp(entryStr, "group") == 0)
        ace->tag = hasQual ? ACL_GROUP : ACL_GROUP_OBJ;
    else if (strcmp(entryStr, "o") == 0 || strcmp(entryStr, "other") == 0)
        ace->tag = ACL_OTHER;
    else if (strcmp(entryStr, "m") == 0 || strcmp(entryStr, "mask") == 0)
        ace->tag = ACL_MASK;
    else {
        fprintf(stderr, "Bad tag: %s\n", entryStr);
        return false;
    }

    /* For ACL_USER and ACL_GROUP tags, extract a UID / GID from qualifier */

    if (colon2 != NULL)
        *colon2 = '\0';         /* Add terminator to qualifier */

    ace->qual = 0;

    if (ace->tag == ACL_USER) {
        ace->qual = userIdFromName(colon1 + 1);
        if (ace->qual == -1) {
            fprintf(stderr, "Bad user ID: %s\n", colon1 + 1);
            return false;
        }
    } else if (ace->tag == ACL_GROUP) {
        ace->qual = groupIdFromName(colon1 + 1);
        if (ace->qual == -1) {
            fprintf(stderr, "Bad group ID: %s\n", colon1 + 1);
            return false;
        }
    }

    /* If a permissions string was present, return it as a bit mask */

    if (hasPerms) {
        ace->perms = 0;

        /* We're not too thorough here -- we don't check for multiple
           instances of [-rwx], or check if the permissions string is
           longer than three characters... */

        for (char *p = colon2 + 1; *p != '\0'; p++) {
            if (*p == 'r')
                ace->perms |= ACL_READ;
            else if (*p == 'w')
                ace->perms |= ACL_WRITE;
            else if (*p == 'x')
                ace->perms |= ACL_EXECUTE;
            else if (*p != '-') {
                fprintf(stderr, "Bad character in permissions "
                        "string: %c\n", *p);
                return false;
            }
        }
    }

    return true;
}

/* Parse a text form ACL, returning information about the entries in
   'aclist'. On success, return the number of ACL entries found; on
   error return -1. */

static int
parseACL(char *aclStr, struct AccessControlEntry aclist[],
        bool permsReqd)
{
    char *comma;
    int n = 0;

    for (char *nextEntry = aclStr; ; nextEntry = comma + 1) {

        if (n >= MAX_ENTRIES) {
            fprintf(stderr, "Too many entries in ACL\n");
            return -1;
        }

        comma = strchr(nextEntry, ',');
        if (comma != NULL)
            *comma = '\0';

        if (!parseEntrySpec(nextEntry, &aclist[n], permsReqd))
            return -1;

        if (comma == NULL)              /* This was the last entry */
            break;

        n++;
    }

    return n + 1;
}

/* Find the the ACL entry in 'acl' corresponding to the tag type and
   qualifier in 'tag' and 'qual'. Return the matching entry, or NULL
   if no entry was found. */

static acl_entry_t
findEntry(acl_t acl, acl_tag_t tag, id_t qaul)
{
    acl_entry_t entry;
    acl_tag_t entryTag;

    for (int ent = ACL_FIRST_ENTRY; ; ent = ACL_NEXT_ENTRY) {
        int s = acl_get_entry(acl, ent, &entry);
        if (s == -1)
            errExit("acl_get_entry");

        if (s == 0)
            return NULL;

        if (acl_get_tag_type(entry, &entryTag) == -1)
            errExit("acl_get_tag_type");

        if (tag == entryTag) {
            if (tag == ACL_USER) {
                uid_t *uidp = acl_get_qualifier(entry);
                if (uidp == NULL)
                    errExit("acl_get_qualifier");

                if (qaul == *uidp) {
                    if (acl_free(uidp) == -1)
                        errExit("acl_free");
                    return entry;
                } else {
                    if (acl_free(uidp) == -1)
                        errExit("acl_free");
                }

            } else if (tag == ACL_GROUP) {
                gid_t *gidp = acl_get_qualifier(entry);
                if (gidp == NULL)
                    errExit("acl_get_qualifier");

                if (qaul == *gidp) {
                    if (acl_free(gidp) == -1)
                        errExit("acl_free");
                    return entry;
                } else {
                    if (acl_free(gidp) == -1)
                        errExit("acl_free");
                }

            } else {
                return entry;
            }
        }
    }
}

/* Set the permissions in 'perms' in the ACL entry 'entry' */

static void
setPerms(acl_entry_t entry, int perms)
{
    acl_permset_t permset;

    if (acl_get_permset(entry, &permset) == -1)
        errExit("acl_get_permset");

    if (acl_clear_perms(permset) == -1)
        errExit("acl_clear_perms");

    if (perms & ACL_READ)
        if (acl_add_perm(permset, ACL_READ) == -1)
            errExit("acl_add_perm");
    if (perms & ACL_WRITE)
        if (acl_add_perm(permset, ACL_WRITE) == -1)
            errExit("acl_add_perm");
    if (perms & ACL_EXECUTE)
        if (acl_add_perm(permset, ACL_EXECUTE) == -1)
            errExit("acl_add_perm");

    if (acl_set_permset(entry, permset) == -1)
        errExit("acl_set_permset");
}

/* Modify or remove entries in the default or in the access ACL of 'file. */

static void
updateACL(char *file, bool useDefaultACL,
          int numEntries, struct AccessControlEntry *aclist,
          enum Operation operation, bool recalcMask)
{
    acl_type_t type = useDefaultACL ? ACL_TYPE_DEFAULT : ACL_TYPE_ACCESS;

    acl_t acl = acl_get_file(file, type);
    if (acl == NULL)
        errExit("acl_get_file");

    /* Apply each of the entries in 'aclist' to the current file */

    for (int en = 0; en < numEntries; en++) {
        acl_entry_t entry = findEntry(acl, aclist[en].tag, aclist[en].qual);

        if (operation == removeEntries) {
            if (entry != NULL)
                if (acl_delete_entry(acl, entry) == -1)
                    errExit("acl_delete_entry");

        } else {        /* modifyEntries */

            if (entry == NULL) {

                /* Entry didn't exist in ACL -- create a new
                   entry with required tag and qualifier */

                if (acl_create_entry(&acl, &entry) == -1)
                    errExit("acl_create_entry");
                if (acl_set_tag_type(entry, aclist[en].tag) == -1)
                    errExit("acl_set_tag_type");
                if (aclist[en].tag == ACL_USER || aclist[en].tag == ACL_GROUP)
                    if (acl_set_qualifier(entry, &aclist[en].qual) == -1)
                        errExit("acl_set_qualifier");
            }

            setPerms(entry, aclist[en].perms);
        }

        /* Recalculate the mask entry if requested */

        if (recalcMask)
            if (acl_calc_mask(&acl) == -1)
                errExit("acl_calc_mask");

        /* Update the file ACL */

        if (acl_valid(acl) == -1)
            errExit("acl_valid");

        if (acl_set_file(file, type, acl) == -1)
            errExit("acl_set_file");
    }

    if (acl_free(acl) == -1)
        errExit("acl_free");
}

int
main(int argc, char *argv[])
{
    if (argc < 2 || strcmp(argv[1], "--help") == 0)
        usageError(argv[0], NULL, false);

    /* Parse command-line options */

    enum Operation operation = unspecifiedOp;
    bool recalcMask = true;
    bool useDefaultACL = false;
    int optCnt = 0;
    char *aclSpec = NULL;
    int opt;

    while ((opt = getopt(argc, argv, "m:x:kdnV:")) != -1) {
        switch (opt) {
        case 'm':
            operation = modifyEntries;
            aclSpec = optarg;
            optCnt++;
            break;

        case 'x':
            operation = removeEntries;
            aclSpec = optarg;
            optCnt++;
            break;

        case 'k':
            operation = removeDefaultACL;
            optCnt++;
            break;

        case 'V':
            operation = checkValidACL;
            aclSpec = optarg;
            optCnt++;
            break;

        case 'd':
            useDefaultACL = true;
            break;

        case 'n':
            recalcMask = false;
            break;

        default:
            usageError(argv[0], "Bad option\n", true);
            break;
        }
    }

    if (optCnt != 1)
        usageError(argv[0], "Specify exactly one of -m, -x, -k, or -V\n", true);

    if ((operation == checkValidACL) && useDefaultACL)
        usageError(argv[0], "Can't specify -d with -V\n", true);

    struct AccessControlEntry aclist[MAX_ENTRIES];

    if (operation == checkValidACL) {
        if (parseACL(aclSpec, aclist, true) == -1) {
            fatal("Bad ACL entry specification");
        } else {
            printf("ACL is valid\n");
            exit(EXIT_SUCCESS);
        }
    }

    int numEntries = 0;
    if (operation == modifyEntries || operation == removeEntries) {
        numEntries = parseACL(aclSpec, aclist, operation == modifyEntries);
        if (numEntries == -1)
            usageError(argv[0], "Bad ACL specification\n", true);
    }

    /* Perform the operation on each file argument */

    for (int j = optind; j < argc; j++) {
        if (operation == removeDefaultACL) {
            if (acl_delete_def_file(argv[j]) == -1)
                errExit("acl_delete_def_file: %s", argv[j]);

        } else if (operation == modifyEntries || operation == removeEntries) {
            updateACL(argv[j], useDefaultACL, numEntries, aclist,
                      operation, recalcMask);

        } else {
            fatal("Bad logic!");
        }
    }

    exit(EXIT_SUCCESS);
}
