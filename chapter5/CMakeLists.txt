project(chapter4)

set(COMMON_SOURCE ../lib/get_num.c ../lib/error_functions.c)

add_executable(bad_exclusive_open bad_exclusive_open.c ${COMMON_SOURCE})

target_include_directories(bad_exclusive_open PRIVATE ../lib)
