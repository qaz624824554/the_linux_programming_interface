project(chapter4)

set(COMMON_SOURCE ../lib/get_num.c ../lib/error_functions.c)

set(executes mem_segments necho display_env modify_env longjmp setjmp_vars exercise_2 exercise_3)

foreach(execute ${executes})
    add_executable(${execute} ${execute}.c ${COMMON_SOURCE})
    target_include_directories(${execute} PRIVATE ../lib)
endforeach()
