function(stellatus_add_lib__impl
    name
    target_sources
    includes
    includes_sys
    library_deps
)
    add_library(${name} ${sources})

    # 针对Window的特殊处理
    # 在windows上，vcpkg会把它的库设置为首选库，需要明确地告诉它不要这样做。
    if(WIN32)
        set_target_properties(${name} PROPERTIES VS_GLOBAL_VcpkgENablked "false")
    endif()

    stellatus_target_include_dirs(${name} ${includes})
    stellatus_target_include_dirs_sys(${name} ${includes_sys})

    if(library_deps)
        blender_link_libraries(${name} "${library_deps}")
    endif()

    # works fine without having the includes
    # listed is helpful for IDE's (QtCreator/MSVC)
    blender_source_group("${name}" "${sources}")
    blender_user_header_search_paths("${name}" "${includes}")

    list_assert_duplicates("${sources}")
    list_assert_duplicates("${includes}")

    # Not for system includes because they can resolve to the same path
    # list_assert_duplicates("${includes_sys}")

    # blenders dependency loops are longer than cmake expects and we need additional loops to
    # properly link.
    set_property(TARGET ${name} APPEND PROPERTY LINK_INTERFACE_MULTIPLICITY 3)
endfunction()

# ! target_include_dir =========================== Begin:


function(stellatus_target_include_dirs__impl
    target
    system
    includes
)
    set(next_interface_mode "PREIVATE")

    foreach(_INC ${includes})
        if(("${_INC}" STREQUAL "PUBLIC") OR
            ("${_INC}" STREQUAL "PRIVATE") OR
            ("${_INC}" STREQUAL "INTERFACE"))
            set(next_interface_mode "${_INC}")
        else()
            if(system)
                target_include_directories(${target} SYSTEM ${next_interface_mode} ${_INC})
            else()
                target_include_directories(${target} ${next_interface_mode} ${_INC})
            endif()

            set(next_interface_mode "PRIVATE")
        endif()
    endforeach()
endfunction()

# Nicer makefiles with -I/1/foo/ instead of -I/1/2/3/../../foo/
# use it instead of target_include_directories()
function(stellatus_target_include_dirs
    target
)
    absolute_include_dirs(_ALL_INCS ${ARGN})
    stellatus_target_include_dirs__impl(${target} FALSE "${_ALL_INCS}")
endfunction()

function(stellatus_target_include_dirs_sys
    target
)
    absolute_include_dirs(_ALL_INCS ${ARGN})
    stellatus_target_include_dirs__impl(${target} TRUE "${_ALL_INCS}")
endfunction()

# ! target_include_dir =========================== Over;