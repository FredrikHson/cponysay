cmake_minimum_required(VERSION 3.4)

project(cponysay)
set(CMAKE_EXPORT_COMPILE_COMMANDS TRUE)
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake")

file(GLOB_RECURSE PONIES
    ../external/ponysay/ponies/*.pony
    )
file(GLOB_RECURSE EXTRAPONIES
    ../external/ponysay/extraponies/*.pony
    )


set(COMMON_SRC
    "main.c"
    )

include_directories(
    "."
    )

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_C_STANDARD 11)

file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/ponies)

foreach(PONY ${PONIES})
    get_filename_component(PONY_NAME ${PONY} NAME_WE)
    get_filename_component(ABSPONY ${PONY} ABSOLUTE)
    add_custom_command(OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/ponies/${PONY_NAME}.cpp
        COMMAND ${CMAKE_CURRENT_LIST_DIR}/scripts/parsepony.sh ${ABSPONY} > ${CMAKE_CURRENT_BINARY_DIR}/ponies/${PONY_NAME}.cpp
        DEPENDS ./scripts/parsepony.sh ${ABSPONY}
        )
    list(APPEND PONY_DEPEND ${CMAKE_CURRENT_BINARY_DIR}/ponies/${PONY_NAME}.cpp)
endforeach()

add_executable(${CMAKE_PROJECT_NAME} ${COMMON_SRC} ${PONY_DEPEND})


target_link_libraries(${CMAKE_PROJECT_NAME}
    )
install(TARGETS ${CMAKE_PROJECT_NAME} RUNTIME DESTINATION bin)

# uninstall target
configure_file(
    "${CMAKE_MODULE_PATH}/cmake_uninstall.cmake"
    "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake"
    IMMEDIATE @ONLY)

add_custom_target(uninstall
    COMMAND ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake)

find_program(iwyu_path NAMES include-what-you-use iwyu)
if(iwyu_path)
    set_property(TARGET ${CMAKE_PROJECT_NAME} PROPERTY C_INCLUDE_WHAT_YOU_USE ${iwyu_path})
    set_property(TARGET ${CMAKE_PROJECT_NAME} PROPERTY CXX_INCLUDE_WHAT_YOU_USE ${iwyu_path})
endif()
