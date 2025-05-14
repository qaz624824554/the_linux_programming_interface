project(chapter4)

set(COMMON_SOURCE ../lib/get_num.c ../lib/error_functions.c)

add_executable(copy copy.c ${COMMON_SOURCE})

target_include_directories(copy PRIVATE ../lib)

add_executable(seek_io seek_io.c ${COMMON_SOURCE})

target_include_directories(seek_io PRIVATE ../lib)

add_executable(tee tee.c ${COMMON_SOURCE})

target_include_directories(tee PRIVATE ../lib)

add_executable(cp cp.c ${COMMON_SOURCE})

target_include_directories(cp PRIVATE ../lib)