project(chapter4)

set(COMMON_SOURCE ../lib/get_num.c ../lib/error_functions.c ../lib/ugid_functions.c ../lib/file_perms.c)

set(executes link unlink t_unlink rename symlink readlink mkdir rmdir dir nftw nftw_dir_tree getcwd chdir fchdir openat chroot realpath parse_path)

foreach(execute ${executes})
    add_executable(${execute} ${execute}.c ${COMMON_SOURCE})
    target_include_directories(${execute} PRIVATE ../lib)
endforeach()
