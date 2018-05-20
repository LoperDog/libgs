# Copyright 2018 inkeun.kim
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
#


cmake_minimum_required(VERSION 3.0.2)

project(libgs C CXX)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14")

include_directories(${CMAKE_SOURCE_DIR})

find_package(Boost REQUIRED COMPONENTS system filesystem unit_test_framework)
find_package(spdlog CONFIG REQUIRED)

include_directories(spdlog)

set(HEADER
  ${CMAKE_SOURCE_DIR}/libgsCore/include/common/error.h
  ${CMAKE_SOURCE_DIR}/libgsCore/include/common/logging.h
  ${CMAKE_SOURCE_DIR}/libgsCore/include/common/peer.h
  ${CMAKE_SOURCE_DIR}/libgsCore/include/common/uuid.h
  ${CMAKE_SOURCE_DIR}/libgsCore/include/grouping/grouping.h
  ${CMAKE_SOURCE_DIR}/libgsCore/include/redis/redis_connection.h
  ${CMAKE_SOURCE_DIR}/libgsCore/include/redis/redis_parameter.h
  ${CMAKE_SOURCE_DIR}/libgsCore/include/system/thread_pool.h
  ${CMAKE_SOURCE_DIR}/libgsCore/include/stdafx.h
  ${CMAKE_SOURCE_DIR}/libgsCore/include/targetver.h
)

set(CC
  ${CMAKE_SOURCE_DIR}/libgsCore/src/common/logging.cpp
  ${CMAKE_SOURCE_DIR}/libgsCore/src/common/uuid.cpp
  ${CMAKE_SOURCE_DIR}/libgsCore/src/common/peer.cpp
  ${CMAKE_SOURCE_DIR}/libgsCore/src/grouping/grouping.cpp
  ${CMAKE_SOURCE_DIR}/libgsCore/src/redis/redis_connection.cpp
  ${CMAKE_SOURCE_DIR}/libgsCore/src/system/thread_pool.cpp
  ${CMAKE_SOURCE_DIR}/libgsCore/src/stdafx.cpp
)

set(SOURCES ${HEADER} ${CC})

include_directories(${HEADER})
include_directories(${CMAKE_SOURCE_DIR}/libgsCore/include)

# TODO(inkeun): 추후에 third-party library 빌드용
#               cmake 를 별도로 만든다.
include_directories(${CMAKE_SOURCE_DIR}/Vendor/redisclient-0.5/src)


###########################################################################
# CORE
###########################################################################

add_executable(libgs
  ${HEADER}
  ${SOURCES}
  ${CMAKE_SOURCE_DIR}/libgsCore/src/main.cpp)

target_link_libraries(libgs PUBLIC
  ${Boost_LIBRARIES}
  ${spdlog}
  pthread
)
