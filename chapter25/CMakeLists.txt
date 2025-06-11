project(chapter4)

set(COMMON_SOURCE ../lib/get_num.c ../lib/error_functions.c ../lib/ugid_functions.c ../lib/file_perms.c ../lib/signal_functions.c ../lib/curr_time.c)

set(executes atexit fork_stdio_buf)

foreach(execute ${executes})
    add_executable(${execute} ${execute}.c ${COMMON_SOURCE})
    target_include_directories(${execute} PRIVATE ../lib)
    target_link_libraries(${execute} -lcrypt)
endforeach()
