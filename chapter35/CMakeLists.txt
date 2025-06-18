cmake_minimum_required(VERSION 3.10)
project(chapter4)

set(COMMON_SOURCE ../lib/get_num.c ../lib/error_functions.c ../lib/ugid_functions.c ../lib/file_perms.c ../lib/signal_functions.c ../lib/curr_time.c ../lib/print_wait_status.c)

set(executes getpriority setpriority sched_get_priority_min_max sched_setscheduler sched_yield sched_rr_get_interval sched_affinity pipe_cpu_affinity)

foreach(execute ${executes})
    add_executable(${execute} ${execute}.c ${COMMON_SOURCE})
    target_include_directories(${execute} PRIVATE ../lib)
    target_link_libraries(${execute} -lcrypt)
endforeach()
