cmake_minimum_required(VERSION 3.10)
project(chapter4)

set(COMMON_SOURCE ../lib/get_num.c ../lib/error_functions.c ../lib/ugid_functions.c ../lib/file_perms.c ../lib/signal_functions.c ../lib/curr_time.c ../lib/print_wait_status.c)

set(executes test_become_daemon daemon syslog)

foreach(execute ${executes})
    add_executable(${execute} ${execute}.c ${COMMON_SOURCE} become_daemon.c)
    target_include_directories(${execute} PRIVATE ../lib)
    target_link_libraries(${execute} -lcrypt -lm)
endforeach()
