cmake_minimum_required(VERSION 3.20)

project(
    Stellatus
    LANGUAGES CXX C
    DESCRIPTION "Stellatus!"
    VERSION 0.1.0
)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(BUILD_SHARED_LIBS OFF)

# note: 启用生成 compile_commands.json
set(EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_BUILD_TYPE Debug)

# , 项目路径
set(Stellatus_ROOT_DIR "${CMAKE_CURRENT_SOURCE_DIR}")
# , 引擎路径
set(Stellatus_ENGINE_DIR "${Stellatus_ROOT_DIR}/Engine")
# , 第三方库路径
set(Stellatus_THIRDPARTY_DIR "${Stellatus_ENGINE_DIR}/Lib")

# , Windows下的输出路径
# note: Windows 下 dll 和 exe 必须在同一个目录下
set(Stellatus_Windows_DLL_DIR "${Stellatus_ROOT_DIR}/Bin/Win")
set(Stellatus_Windows_EXE_DIR "${Stellatus_ROOT_DIR}/Bin/Win")
set(Stellatus_Windows_Lib_DIR "${Stellatus_ROOT_DIR}/Bin/Win/StaticLib")

# , Linux下的输出路径
set(Stellatus_Linux_DLL_DIR "${Stellatus_ROOT_DIR}/Bin/Linux")
set(Stellatus_Linux_EXE_DIR "${Stellatus_ROOT_DIR}/Bin/Linux")
set(Stellatus_Linux_Lib_DIR "${Stellatus_ROOT_DIR}/Bin/Linux")

# , IOS下的输出路径
set(Stellatus_IOS_DLL_DIR "${Stellatus_ROOT_DIR}/Bin/IOS")
set(Stellatus_IOS_EXE_DIR "${Stellatus_ROOT_DIR}/Bin/IOS")
set(Stellatus_IOS_Lib_DIR "${Stellatus_ROOT_DIR}/Bin/IOS")

add_subdirectory(Engine)
