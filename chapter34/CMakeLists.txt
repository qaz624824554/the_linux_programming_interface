project(chapter4)

set(COMMON_SOURCE ../lib/get_num.c ../lib/error_functions.c ../lib/ugid_functions.c ../lib/file_perms.c ../lib/signal_functions.c ../lib/curr_time.c ../lib/print_wait_status.c)

set(executes getpgrp setpgrp getsid setsid ctermid tcgetsid tcgetpgrp tcsetpgrp handle_sighup orphaned_pgrp_SIGHUP exercise_2 exercise_3)

foreach(execute ${executes})
    add_executable(${execute} ${execute}.c ${COMMON_SOURCE})
    target_include_directories(${execute} PRIVATE ../lib)
    target_link_libraries(${execute} -lcrypt)
endforeach()
