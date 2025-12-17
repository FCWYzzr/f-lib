option(BUILD_TEST "build test" OFF)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

function(fetch_files name)
    file(GLOB_RECURSE cpp
        src/*.cpp)
    file(GLOB_RECURSE ixx
        src/*.ixx)
    file(GLOB_RECURSE test
        src/*.test.cpp)
    list(REMOVE_ITEM cpp ${test})
    set(${name}_cpp ${cpp} PARENT_SCOPE)
    set(${name}_ixx ${ixx} PARENT_SCOPE)
    set(${name}_test ${test} PARENT_SCOPE)
endfunction()

function(configure_target target)
    fetch_files(${target})

    target_sources(${target}
        PRIVATE ${${target}_cpp}
        PUBLIC
            FILE_SET cxx_modules
            TYPE CXX_MODULES
            FILES ${${target}_ixx})
    if (BUILD_TEST)
        foreach (file ${${target}_test})
            get_filename_component(name ${file} NAME_WLE)
            get_filename_component(name ${name} NAME_WLE)
            add_executable(test.${name} ${file})
            target_link_libraries(test.${name} PRIVATE ${target})
        endforeach ()
    endif ()
endfunction()
