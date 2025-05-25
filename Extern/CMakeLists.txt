# note: 强制生成静态库
set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)

# , 导入imgui
# if(NOT TARGET imgui)
# include(imgui.cmake)

# , 配置 IDE 的路径打开时的路径
# set_target_properties(imgui PROPERTIES FOLDER ${third_party_folder}/imgui)
# endif()

# , 导入 glfw
# if(NOT TARGET glfw)
# option(GLFW_BUILD_EXAMPLES "" OFF)
# option(GLFW_BUILD_TESTS "" OFF)
# option(GLFW_BUILD_DOCS "" OFF)
# option(GLFW_INSTALL "" OFF)
# add_subdirectory(glfw)
# set_target_properties(glfw PROPERTIES FOLDER ${third_party_folder}/glfw)
# set_target_properties(update_mappings PROPERTIES FOLDER ${third_party_folder}/glfw)
# endif()