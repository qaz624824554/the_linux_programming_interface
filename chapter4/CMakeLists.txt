project(chapter4)

set(COMMON_SOURCE ../lib/get_num.c ../lib/error_functions.c)

add_executable(copy copy.c ${COMMON_SOURCE})

target_include_directories(copy PRIVATE ../lib)