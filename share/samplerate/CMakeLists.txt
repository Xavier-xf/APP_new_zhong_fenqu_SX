add_definitions(-DHAVE_CONFIG_H)

include_directories(
	.
	${CMAKE_CURRENT_SOURCE_DIR}/../include/samplerate/
)

add_library(samplerate STATIC
	samplerate.c
	src_linear.c
	src_sinc.c
	src_zoh.c
	samplerate_api.c
)
