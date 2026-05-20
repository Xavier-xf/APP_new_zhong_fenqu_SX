cmake_minimum_required(VERSION 3.5.1)

# set(CMAKE_C_COMPILER "/home/xiaole/smb/MEIO_project/AK37D_SDK_V1.06/tools/arm-anykav500-linux-uclibcgnueabi/bin/arm-anykav500-linux-uclibcgnueabi-gcc")
# set(CMAKE_CXX_COMPILER "/home/xiaole/smb/MEIO_project/AK37D_SDK_V1.06/tools/arm-anykav500-linux-uclibcgnueabi/bin/arm-anykav500-linux-uclibcgnueabi-g++")
set(CMAKE_C_COMPILER "arm-anykav500-linux-uclibcgnueabi-gcc")
set(CMAKE_CXX_COMPILER "arm-anykav500-linux-uclibcgnueabi-g++")
 
# 设置编译器，此处为ARM平台，如果你安装了编译链就用名称，没安装请用绝对路径
set(PROJECT_PATH ${CMAKE_CURRENT_SOURCE_DIR}/../)

SET(leo_proj CDV1004QT.BIN)
project(${leo_proj})

# 工程名
SET(CMAKE_BUILD_TYPE "Release")
SET(CMAKE_C_FLAGS_RELEASE "$ENV{CXXFLAGS} -O3 -s -Wall -std=gnu11  -Werror")
SET(CMAKE_CXX_FLAGS_RELEASE "$ENV{CXXFLAGS} -O3 -s -Wall -std=c++1y -Werror")

# ELSEIF(CMAKE_BUILD_TYPE STREQUAL Debug)
	# message("================================================ BUILD TYPE : Debug")
	# SET(CMAKE_C_FLAGS_DEBUG    "$ENV{CXXFLAGS} -std=gnu11   -Wall  -g3 ") 
	# SET(CMAKE_CXX_FLAGS_DEBUG  "$ENV{CXXFLAGS} -std=c++1y -Wall  -g3 ") 
	
	# add_compile_options(-g3)	# 加上编译选项
	# -g0表示不生成调试信息
	# -g3表示生成最多的调试信息

include_directories(
	${PROJECT_PATH}live555/include/BasicUsageEnvironment
	${PROJECT_PATH}live555/include/groupsock
	${PROJECT_PATH}live555/include/liveMedia
	${PROJECT_PATH}live555/include/UsageEnvironment
	${PROJECT_PATH}onvif/include
	${PROJECT_PATH}onvif/
	${PROJECT_PATH}include/
	${PROJECT_PATH}include/anyka37d
	${PROJECT_PATH}include/common
	${PROJECT_PATH}include/tuya
	${PROJECT_PATH}share/
	${PROJECT_PATH}share/include
	${PROJECT_PATH}share/lvgl
	${PROJECT_PATH}common/
	${PROJECT_PATH}layout/
	${PROJECT_PATH}res/

)

# 添加src目录
aux_source_directory(${PROJECT_PATH}layout/ DIR_SRCS)

add_subdirectory(${PROJECT_PATH}common ${CMAKE_CURRENT_SOURCE_DIR}/common/)
add_subdirectory(${PROJECT_PATH}share ${CMAKE_CURRENT_SOURCE_DIR}/share/)


set(VAR ${VAR} lvgl)
set(VAR ${VAR} common)
set(VAR ${VAR} freetype)
set(VAR ${VAR} mad)

set(VAR ${VAR} jpeg)
set(VAR ${VAR} samplerate)

# set(VAR ${VAR} xls)
# set(VAR ${VAR} tuya)
set(VAR ${VAR} zlib)
add_executable(${leo_proj} ${DIR_SRCS})
message("VAR : ${VAR}")
target_link_libraries(${leo_proj} ${VAR})

# target_link_libraries(${leo_proj} m)
# target_link_libraries(${leo_proj} pthread)
# target_link_libraries(${leo_proj} dl)
set(ANYKA_LIB_PATH ${PROJECT_PATH}/lib/anyka37d/)
target_link_libraries(${leo_proj} ${ANYKA_LIB_PATH}/libplat_tde.so)
target_link_libraries(${leo_proj} ${ANYKA_LIB_PATH}/libplat_mem.so)
target_link_libraries(${leo_proj} ${ANYKA_LIB_PATH}/libplat_osal.so)
target_link_libraries(${leo_proj} ${ANYKA_LIB_PATH}/libplat_thread.so)
target_link_libraries(${leo_proj} ${ANYKA_LIB_PATH}/libplat_common.so)
target_link_libraries(${leo_proj} ${ANYKA_LIB_PATH}/libplat_log.so)
target_link_libraries(${leo_proj} ${ANYKA_LIB_PATH}/libplat_dbg.so)
target_link_libraries(${leo_proj} ${ANYKA_LIB_PATH}/libplat_ao.so)
target_link_libraries(${leo_proj} ${ANYKA_LIB_PATH}/libplat_vqe.so)
target_link_libraries(${leo_proj} ${ANYKA_LIB_PATH}/libakaudiofilter.so)
target_link_libraries(${leo_proj} ${ANYKA_LIB_PATH}/libmpi_vdec.so)
target_link_libraries(${leo_proj} ${ANYKA_LIB_PATH}/libakv_decode.so)
target_link_libraries(${leo_proj} ${ANYKA_LIB_PATH}/libplat_vi.so)
target_link_libraries(${leo_proj} ${ANYKA_LIB_PATH}/libplat_vpss.so)
target_link_libraries(${leo_proj} ${ANYKA_LIB_PATH}/libplat_isp_sdk.so)
target_link_libraries(${leo_proj} ${ANYKA_LIB_PATH}/libmpi_venc.so)
target_link_libraries(${leo_proj} ${ANYKA_LIB_PATH}/libakv_encode.so)
target_link_libraries(${leo_proj} ${ANYKA_LIB_PATH}/libplat_ai.so)

# target_link_libraries(${leo_proj} ${PROJECT_PATH}/live555/lib/libBasicUsageEnvironment.a)
# target_link_libraries(${leo_proj} ${PROJECT_PATH}/live555/lib/libgroupsock.a)
# target_link_libraries(${leo_proj} ${PROJECT_PATH}/live555/lib/libliveMedia.a)
# target_link_libraries(${leo_proj} ${PROJECT_PATH}/live555/lib/libUsageEnvironment.a)

set(LIVE555_PATH ${PROJECT_PATH}/live555/lib)
target_link_libraries(${leo_proj} -Wl,--start-group
    ${LIVE555_PATH}/libBasicUsageEnvironment.a
    ${LIVE555_PATH}/libgroupsock.a
    ${LIVE555_PATH}/libliveMedia.a
    ${LIVE555_PATH}/libUsageEnvironment.a
    -Wl,--end-group)

#target_link_libraries(${leo_proj} ${ANYKA_LIB_PATH}/libapp_its.a)
#target_link_libraries(${leo_proj} ${ANYKA_LIB_PATH}/libplat_timer.a)

set(ANYKA_LIB_PATH ${PROJECT_PATH}/lib/tuya/)
target_link_libraries(${leo_proj} -Wl,--start-group
	${ANYKA_LIB_PATH}/libtuya_ipc.a
	${ANYKA_LIB_PATH}/libmbedtls.a
	tuya
	xls
	-Wl,--end-group)

set(ONVIF_LIB_PATH ${PROJECT_PATH}/onvif/lib)
target_link_libraries(${leo_proj} -Wl,--start-group
    
        # stand lib
        m
        pthread
        dl
    
        # system lib
        ${ONVIF_LIB_PATH}/libbase64.a
        ${ONVIF_LIB_PATH}/libmxml.a
        ${ONVIF_LIB_PATH}/libonvif.a
        ${ONVIF_LIB_PATH}/libsha1.a
        ${ONVIF_LIB_PATH}/libmd5.a
        -Wl,--end-group)