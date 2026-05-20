# Install script for directory: /home/xiaoxiao/workspace/meiou/APP_new_zhong_fenqu_SX/share

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "/usr/local")
endif()
string(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
if(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  if(BUILD_TYPE)
    string(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  else()
    set(CMAKE_INSTALL_CONFIG_NAME "Release")
  endif()
  message(STATUS "Install configuration: \"${CMAKE_INSTALL_CONFIG_NAME}\"")
endif()

# Set the component getting installed.
if(NOT CMAKE_INSTALL_COMPONENT)
  if(COMPONENT)
    message(STATUS "Install component: \"${COMPONENT}\"")
    set(CMAKE_INSTALL_COMPONENT "${COMPONENT}")
  else()
    set(CMAKE_INSTALL_COMPONENT)
  endif()
endif()

# Install shared libraries without execute permission?
if(NOT DEFINED CMAKE_INSTALL_SO_NO_EXE)
  set(CMAKE_INSTALL_SO_NO_EXE "1")
endif()

# Is this installation the result of a crosscompile?
if(NOT DEFINED CMAKE_CROSSCOMPILING)
  set(CMAKE_CROSSCOMPILING "FALSE")
endif()

# Set default install directory permissions.
if(NOT DEFINED CMAKE_OBJDUMP)
  set(CMAKE_OBJDUMP "/opt/arm-anykav500-linux-uclibcgnueabi/bin/arm-anykav500-linux-uclibcgnueabi-objdump")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for each subdirectory.
  include("/home/xiaoxiao/workspace/meiou/APP_new_zhong_fenqu_SX/build/share/mad/cmake_install.cmake")
  include("/home/xiaoxiao/workspace/meiou/APP_new_zhong_fenqu_SX/build/share/lvgl/cmake_install.cmake")
  include("/home/xiaoxiao/workspace/meiou/APP_new_zhong_fenqu_SX/build/share/freetype/cmake_install.cmake")
  include("/home/xiaoxiao/workspace/meiou/APP_new_zhong_fenqu_SX/build/share/tuya/cmake_install.cmake")
  include("/home/xiaoxiao/workspace/meiou/APP_new_zhong_fenqu_SX/build/share/xls/cmake_install.cmake")
  include("/home/xiaoxiao/workspace/meiou/APP_new_zhong_fenqu_SX/build/share/jpeg/cmake_install.cmake")
  include("/home/xiaoxiao/workspace/meiou/APP_new_zhong_fenqu_SX/build/share/samplerate/cmake_install.cmake")
  include("/home/xiaoxiao/workspace/meiou/APP_new_zhong_fenqu_SX/build/share/zlib/cmake_install.cmake")

endif()

