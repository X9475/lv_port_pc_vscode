file(GLOB_RECURSE SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/*.c)
add_library(lv_100ask_generic_ui STATIC ${SOURCES})
target_include_directories(lv_100ask_generic_ui PUBLIC ${PROJECT_SOURCE_DIR}/lv_lib_100ask)
target_link_libraries(lv_100ask_generic_ui lv_lib_100ask)