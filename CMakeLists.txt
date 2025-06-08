cmake_minimum_required(VERSION 3.20)

project(
    Stellatus
    LANGUAGES CXX C
    DESCRIPTION "Stellatus!"
    VERSION 0.1.0
)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(BUILD_SHARED_LIBS OFF)

# note: 启用生成 compile_commands.json
set(EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_BUILD_TYPE Debug)

# , 项目路径
set(Stellatus_ROOT_DIR "${CMAKE_CURRENT_SOURCE_DIR}")
# , 引擎路径
set(Stellatus_ENGINE_DIR "${Stellatus_RObOT_DIR}/Engine")

# , 第三方库路径
set(EXTREN_EXTERN_DIR "${Stellatus_ROOT_DIR}/Extern")

# , 配置文件路径, 使用时读取的配置文件路径
set(CONFIG_DIR, "${Stellatus_ROOT_DIR}/Configs/UserConfigs")
# , 配置文件路径, 开发者内部的配置文件路径
# set(CMAKE_INSTALL_PREFIX "${Stellatus_ROOT_DIR}")

if (WIN32)
    # , Windows下的输出路径
    # note: Windows 下 dll 和 exe 必须在同一个目录下
    set(Stellatus_DLL_DIR "${Stellatus_ROOT_DIR}/Bin/Win")
    set(Stellatus_EXE_DIR "${Stellatus_ROOT_DIR}/Bin/Win")
    set(Stellatus_Lib_DIR "${Stellatus_ROOT_DIR}/Bin/Win/StaticLib")
elseif(UNIX)
    if(APPLE)
        # , IOS下的输出路径
        set(Stellatus_DLL_DIR "${Stellatus_ROOT_DIR}/Bin/IOS")
        set(Stellatus_EXE_DIR "${Stellatus_ROOT_DIR}/Bin/IOS")
        set(Stellatus_Lib_DIR "${Stellatus_ROOT_DIR}/Bin/IOS")
    else()
        # , Linux下的输出路径
        set(Stellatus_DLL_DIR "${Stellatus_ROOT_DIR}/Bin/Linux")
        set(Stellatus_EXE_DIR "${Stellatus_ROOT_DIR}/Bin/Linux")
        set(Stellatus_Lib_DIR "${Stellatus_ROOT_DIR}/Bin/Linux")
    endif()
else()
    message(FATAL_ERROR "Unknown Platform")
endif()

# set(CMAKE_INSTALL_PREFEIX "${Stellatus_EXE_DIR}")

include(CMakeTools/CMakeTools.cmake)


add_subdirectory(Engine)
add_subdirectory(Extern)
