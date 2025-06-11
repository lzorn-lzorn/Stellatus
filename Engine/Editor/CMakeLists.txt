set(TARGET_NAME StellatusEditor)

file(GLOB_RECURSE
    INCLUDE
    ${Stellatus_ROOT_DIR}/Engine/Editor/*.h
    ${Stellatus_ROOT_DIR}/Engine/Editor/*.hpp
)

file(GLOB_RECURSE
    SOURCE
    ${Stellatus_ROOT_DIR}/Engine/Editor/*.c
    ${Stellatus_ROOT_DIR}/Engine/Editor/*.cc
    ${Stellatus_ROOT_DIR}/Engine/Editor/*.cpp
)

source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}" FILES ${INCLUDE} ${SOURCE})


add_executable(${TARGET_NAME} ${SOURCE} ${INCLUDE})

target_link_libraries(${TARGET_NAME} PRIVATE Test01)
target_link_libraries(${TARGET_NAME} PRIVATE SDL3::SDL3)
target_link_libraries(${TARGET_NAME} PRIVATE imgui)

if (OpenGL_FOUND)
    target_link_libraries(${TARGET_NAME} PRIVATE glad)
    target_link_libraries(${TARGET_NAME} PRIVATE OpenGL::GL)
endif()

target_include_directories(${TARGET_NAME} PUBLIC ${INCLUDE})
# target_compile_features(${TARGET_NAME} PUBLIC cxx_std_23)
# target_precompile_headers(${TARGET_NAME} PUBLIC ${Stellatus_ROOT_DIR}/Engine/Stellatus.h)