project(chapter4)

set(COMMON_SOURCE ../lib/get_num.c ../lib/error_functions.c)

set(executes free_and_sbrk exercise_1)

foreach(execute ${executes})
    add_executable(${execute} ${execute}.c ${COMMON_SOURCE})
    target_include_directories(${execute} PRIVATE ../lib)
endforeach()
