include_directories(
    .
    ${PROJECT_SOURCE_DIR}/../include/anyka37d
    ${PROJECT_SOURCE_DIR}/../share/include
    ${PROJECT_SOURCE_DIR}/../share/include/libxls
)
    
file(GLOB_RECURSE SOURCES ./*.c )
add_library(common STATIC ${SOURCES})


