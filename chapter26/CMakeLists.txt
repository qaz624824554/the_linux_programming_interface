project(chapter4)

set(COMMON_SOURCE ../lib/get_num.c ../lib/error_functions.c ../lib/ugid_functions.c ../lib/file_perms.c ../lib/signal_functions.c ../lib/curr_time.c ../lib/print_wait_status.c)

set(executes wait multi_wait waitpid child_status make_zombie multi_SIGCHLD exercise_1 exercise_2)

foreach(execute ${executes})
    add_executable(${execute} ${execute}.c ${COMMON_SOURCE})
    target_include_directories(${execute} PRIVATE ../lib)
    target_link_libraries(${execute} -lcrypt)
endforeach()
