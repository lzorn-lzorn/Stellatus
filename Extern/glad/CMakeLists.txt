
set(TARGET_NAME glad)
file(GLOB_RECURSE INCLUDE *.h)
file(GLOB_RECURSE SOURCES *.c)
add_library(${TARGET_NAME} ${INCLUDE} ${SOURCES})

target_include_directories(${TARGET_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
