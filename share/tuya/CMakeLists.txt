
include_directories(
    .

    ${CMAKE_CURRENT_SOURCE_DIR}/../../include/tuya/
    ${CMAKE_CURRENT_SOURCE_DIR}/../include/tuya/
    ${CMAKE_CURRENT_SOURCE_DIR}/../include/libxls/
)
    
file(GLOB_RECURSE SOURCES ./*.c )
add_library(tuya STATIC ${SOURCES})