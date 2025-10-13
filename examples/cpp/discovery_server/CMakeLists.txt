# Copyright 2024 Proyectos y Sistemas de Mantenimiento SL (eProsima).
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

cmake_minimum_required(VERSION 3.20)

project(fastdds_discovery_server_example VERSION 1 LANGUAGES CXX)
set(CMAKE_PREFIX_PATH "/Volumes/Samsung990/eProsima/install")

# Find requirements
if(NOT fastcdr_FOUND)
    find_package(fastcdr 2 REQUIRED)
endif()

if(NOT fastdds_FOUND)
    find_package(fastdds 3 REQUIRED)
endif()

#Check C++11
include(CheckCXXCompilerFlag)
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    check_cxx_compiler_flag(-std=c++11 SUPPORTS_CXX11)
    if(NOT SUPPORTS_CXX11)
        message(FATAL_ERROR "Compiler doesn't support C++11")
    endif()
endif()

# Set CMAKE_BUILD_TYPE to Release by default.
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    message(STATUS "Setting build type to 'Release' as none was specified.")
    set(CMAKE_BUILD_TYPE Release CACHE STRING
        "Choose the type of build, options are: None Debug Release RelWithDebInfo MinSizeRel."
        FORCE)
    set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release" "MinSizeRel" "RelWithDebInfo")
endif()

message(STATUS "Configuring discovery server example...")

# 共享源文件（不包括 main.cpp 和 Application.cpp）
file(GLOB DISCOVERY_SERVER_SOURCES_CXX "*.cxx")
set(COMMON_SOURCES
    ClientPublisherApp.cpp
    ClientSubscriberApp.cpp
    ServerApp.cpp
    ${DISCOVERY_SERVER_SOURCES_CXX}
)

# 1. Publisher 
add_executable(ds_publisher main.cpp ${COMMON_SOURCES})
target_compile_definitions(ds_publisher PRIVATE
    ENTITY_PUBLISHER
    $<$<AND:$<NOT:$<BOOL:${WIN32}>>,$<STREQUAL:"${CMAKE_BUILD_TYPE}","Debug">>:__DEBUG>
    $<$<BOOL:${INTERNAL_DEBUG}>:__INTERNALDEBUG>
    $<$<BOOL:${SHM_TRANSPORT_DEFAULT}>:SHM_TRANSPORT_BUILTIN>
)
target_link_libraries(ds_publisher fastdds fastcdr)
install(TARGETS ds_publisher
    RUNTIME DESTINATION ${DATA_INSTALL_DIR}/fastdds/examples/cpp/discovery_server/${BIN_INSTALL_DIR})

# 2. Subscriber 
add_executable(ds_subscriber main.cpp ${COMMON_SOURCES})
target_compile_definitions(ds_subscriber PRIVATE
    ENTITY_SUBSCRIBER
    $<$<AND:$<NOT:$<BOOL:${WIN32}>>,$<STREQUAL:"${CMAKE_BUILD_TYPE}","Debug">>:__DEBUG>
    $<$<BOOL:${INTERNAL_DEBUG}>:__INTERNALDEBUG>
    $<$<BOOL:${SHM_TRANSPORT_DEFAULT}>:SHM_TRANSPORT_BUILTIN>
)
target_link_libraries(ds_subscriber fastdds fastcdr)
install(TARGETS ds_subscriber
    RUNTIME DESTINATION ${DATA_INSTALL_DIR}/fastdds/examples/cpp/discovery_server/${BIN_INSTALL_DIR})

# 3. Discovery Server 
add_executable(ds_server main.cpp ${COMMON_SOURCES})
target_compile_definitions(ds_server PRIVATE
    ENTITY_SERVER
    $<$<AND:$<NOT:$<BOOL:${WIN32}>>,$<STREQUAL:"${CMAKE_BUILD_TYPE}","Debug">>:__DEBUG>
    $<$<BOOL:${INTERNAL_DEBUG}>:__INTERNALDEBUG>
    $<$<BOOL:${SHM_TRANSPORT_DEFAULT}>:SHM_TRANSPORT_BUILTIN>
)
target_link_libraries(ds_server fastdds fastcdr)
install(TARGETS ds_server
    RUNTIME DESTINATION ${DATA_INSTALL_DIR}/fastdds/examples/cpp/discovery_server/${BIN_INSTALL_DIR})

