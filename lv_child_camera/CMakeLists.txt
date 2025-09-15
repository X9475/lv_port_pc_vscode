file(GLOB_RECURSE SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/*.c)
add_library(lv_child_camera STATIC ${SOURCES})