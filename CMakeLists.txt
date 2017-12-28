cmake_minimum_required(VERSION 3.4 FATAL_ERROR)
set(PROJECT test_module)
project(${PROJECT} LANGUAGES CXX)

if(NOT API_VERSION)
  message(SEND_ERROR "Please set API_VERSION")
endif()

if(NOT BUILD_NUMBER)
  message(SEND_ERROR "Please set BUILD_NUMBER")
endif()

#################################################################################
# Sources
#################################################################################
set(SOURCES
  src/test_robot_module.cpp
)

set(PUBLIC_HEADERS
)

add_library(${PROJECT} SHARED ${SOURCES})

#################################################################################
# Properties
#################################################################################

target_compile_definitions(${PROJECT} PRIVATE
  API_VERSION=${API_VERSION}
  BUILD_NUMBER=${BUILD_NUMBER}
)

set_property(TARGET ${PROJECT} PROPERTY CXX_STANDARD 11)

target_compile_options(${PROJECT} PRIVATE
  -Wall -Werror
  -fno-strict-aliasing
  $<$<BOOL:API_VERSION LESS 101>:-Wno-write-strings>
)

#if(UNIX AND (X86_64 OR X86)) need detect X86
  target_compile_options(${PROJECT} PUBLIC -m32)
  set_target_properties(${PROJECT} PROPERTIES LINK_FLAGS -m32)
#endif()

set_target_properties(${PROJECT} PROPERTIES
  PREFIX ""
  DEBUG_POSTFIX "_debug"
)

#################################################################################
# Libraries
#################################################################################

###############################
## SimpleIni
###############################
if(NOT SIMPLE_INI_PATH)
  set(SIMPLE_INI_PATH "../../../libraries/3rdparty/simpleini")
endif()

if(NOT SIMPLE_INI_PATH STREQUAL SIMPLE_INI_PATH_INTERNAL)
  unset(SIMPLE_INI_DIR)
endif()
set(SIMPLE_INI_PATH_INTERNAL ${SIMPLE_INI_PATH} CACHE INTERNAL "" FORCE)

find_path(SIMPLE_INI_DIR NAMES "SimpleIni.h" PATHS "${SIMPLE_INI_PATH}" NO_DEFAULT_PATH)
if(SIMPLE_INI_DIR)
  target_include_directories(${PROJECT} SYSTEM PRIVATE ${SIMPLE_INI_DIR})
else()
  message(SEND_ERROR "Please set SIMPLE_INI_PATH, current value: ${SIMPLE_INI_PATH}")
endif()

###############################
## Common_headers
###############################
if(NOT COMMON_HEADER_PATH)
  set(COMMON_HEADER_PATH "../../../libraries/common_headers")
endif()

if(NOT COMMON_HEADER_PATH STREQUAL COMMON_HEADER_PATH_INTERNAL)
  unset(COMMON_HEADER_CONFIG)
endif()
set(COMMON_HEADER_PATH_INTERNAL ${COMMON_HEADER_PATH} CACHE INTERNAL "" FORCE)

find_file(COMMON_HEADER_CONFIG NAMES "config/common_headers.cmake" PATHS "${COMMON_HEADER_PATH}/build/install" NO_DEFAULT_PATH)
if(COMMON_HEADER_CONFIG)
  include(${COMMON_HEADER_CONFIG})
  target_link_libraries(${PROJECT} PRIVATE common_headers)
else()
  message(SEND_ERROR "Please set COMMON_HEADER_PATH, current value: ${COMMON_HEADER_PATH}")
endif()


###############################
## Module_headers
###############################
if(NOT MODULE_HEADERS_PATH)
  set(MODULE_HEADERS_PATH "../../../libraries/public/module_headers")
endif()

if(NOT MODULE_HEADERS_PATH STREQUAL MODULE_HEADERS_PATH_INTERNAL)
  unset(MODULE_HEADERS_DIR)
endif()
set(MODULE_HEADERS_PATH_INTERNAL ${MODULE_HEADERS_PATH} CACHE INTERNAL "" FORCE)

find_path(MODULE_HEADERS_DIR NAMES "current/module.h" PATHS ${MODULE_HEADERS_PATH} NO_DEFAULT_PATH)
if(MODULE_HEADERS_DIR)
  target_include_directories(${PROJECT} PUBLIC "${MODULE_HEADERS_DIR}/${API_VERSION}")
else()
  message(SEND_ERROR "Please set MODULE_HEADERS_PATH, current value: ${MODULE_HEADERS_PATH}")
endif()

#################################################################################
# Install
#################################################################################

set(CMAKE_INSTALL_PREFIX ${CMAKE_BINARY_DIR}/install)
target_include_directories(${PROJECT} INTERFACE $<INSTALL_INTERFACE:include>)

install(TARGETS ${PROJECT} EXPORT ${PROJECT}
  RUNTIME DESTINATION lib_${API_VERSION}
  LIBRARY DESTINATION lib_${API_VERSION}
  COMPONENT runtime)