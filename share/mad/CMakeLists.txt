
add_definitions(
    -DFPM_ARM
    -DHAVE_ASSERT_H
)

include_directories(
    .
    ${CMAKE_CURRENT_SOURCE_DIR}/../include/mad/
)
    
file(GLOB_RECURSE SOURCES ./*.c )
add_library(mad STATIC ${SOURCES})