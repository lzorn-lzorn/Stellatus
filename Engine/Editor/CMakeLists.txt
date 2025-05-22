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
add_executable(${PROJECT_NAME} ${SOURCE} ${INCLUDE})

target_include_directories(${PROJECT_NAME} PUBLIC ${INCLUDE})
target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_23)
target_precompile_headers(${PROJECT_NAME} PUBLIC ${Stellatus_ROOT_DIR}/Engine/Stellatus.h)