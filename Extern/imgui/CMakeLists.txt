set(TARGET_NAME imgui)
file(GLOB_RECURSE INCLUDE *.h)
set(SOURCES
    imgui_demo.cpp
    imgui_draw.cpp
    imgui_widgets.cpp
    imgui_tables.cpp
    imgui.cpp
    backends/imgui_impl_sdl3.cpp
    backends/imgui_impl_opengl3.cpp
)
add_library(${TARGET_NAME} STATIC ${SOURCES} ${INCLUDE})
target_link_libraries(${TARGET_NAME} PUBLIC SDL3::SDL3)
target_include_directories(${TARGET_NAME} PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR} 
    ${CMAKE_CURRENT_SOURCE_DIR}/backends 
)