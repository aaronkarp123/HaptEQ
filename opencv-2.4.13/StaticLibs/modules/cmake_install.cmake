# Install script for directory: /Users/aaronkarp/Documents/Research/Summer2016/GraphingPad/opencv-2.4.13/modules

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

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for each subdirectory.
  include("/Users/aaronkarp/Documents/Research/Summer2016/GraphingPad/opencv-2.4.13/StaticLibs/modules/androidcamera/.androidcamera/cmake_install.cmake")
  include("/Users/aaronkarp/Documents/Research/Summer2016/GraphingPad/opencv-2.4.13/StaticLibs/modules/contrib/.contrib/cmake_install.cmake")
  include("/Users/aaronkarp/Documents/Research/Summer2016/GraphingPad/opencv-2.4.13/StaticLibs/modules/core/.core/cmake_install.cmake")
  include("/Users/aaronkarp/Documents/Research/Summer2016/GraphingPad/opencv-2.4.13/StaticLibs/modules/highgui/.highgui/cmake_install.cmake")
  include("/Users/aaronkarp/Documents/Research/Summer2016/GraphingPad/opencv-2.4.13/StaticLibs/modules/imgproc/.imgproc/cmake_install.cmake")
  include("/Users/aaronkarp/Documents/Research/Summer2016/GraphingPad/opencv-2.4.13/StaticLibs/modules/java/.java/cmake_install.cmake")
  include("/Users/aaronkarp/Documents/Research/Summer2016/GraphingPad/opencv-2.4.13/StaticLibs/modules/legacy/.legacy/cmake_install.cmake")
  include("/Users/aaronkarp/Documents/Research/Summer2016/GraphingPad/opencv-2.4.13/StaticLibs/modules/python/.python/cmake_install.cmake")
  include("/Users/aaronkarp/Documents/Research/Summer2016/GraphingPad/opencv-2.4.13/StaticLibs/modules/ts/.ts/cmake_install.cmake")
  include("/Users/aaronkarp/Documents/Research/Summer2016/GraphingPad/opencv-2.4.13/StaticLibs/modules/video/.video/cmake_install.cmake")
  include("/Users/aaronkarp/Documents/Research/Summer2016/GraphingPad/opencv-2.4.13/StaticLibs/modules/world/.world/cmake_install.cmake")
  include("/Users/aaronkarp/Documents/Research/Summer2016/GraphingPad/opencv-2.4.13/StaticLibs/modules/core/cmake_install.cmake")
  include("/Users/aaronkarp/Documents/Research/Summer2016/GraphingPad/opencv-2.4.13/StaticLibs/modules/imgproc/cmake_install.cmake")
  include("/Users/aaronkarp/Documents/Research/Summer2016/GraphingPad/opencv-2.4.13/StaticLibs/modules/highgui/cmake_install.cmake")
  include("/Users/aaronkarp/Documents/Research/Summer2016/GraphingPad/opencv-2.4.13/StaticLibs/modules/video/cmake_install.cmake")

endif()

