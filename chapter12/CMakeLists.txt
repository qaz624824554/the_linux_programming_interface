project(chapter4)

set(COMMON_SOURCE ../lib/get_num.c ../lib/error_functions.c ../lib/ugid_functions.c)

set(executes procfs_pidmax t_uname exercise_1 exercise_2 exercise_3)

foreach(execute ${executes})
    add_executable(${execute} ${execute}.c ${COMMON_SOURCE})
    target_include_directories(${execute} PRIVATE ../lib)
endforeach()
