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

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

include_directories(${CMAKE_SOURCE_DIR})

find_package(Boost COMPONENTS system unit_test_framework REQUIRED)

set(HEADER
  ${CMAKE_SOURCE_DIR}/core/include/libgs/common/uuid.h
)

set(CC
  ${CMAKE_SOURCE_DIR}/core/src/common/uuid.cc
)

set(SOURCES ${HEADER} ${CC})

add_executable(libgs ${HEADER}
${CMAKE_SOURCE_DIR}/src/main.cc
${SOURCES})

add_executable(libgs_test ${HEADER}
${SOURCES}
${CMAKE_SOURCE_DIR}/unit_test/common/uuid_test.cc)

target_link_libraries(libgs PUBLIC
  ${Boost_LIBRARIES}
  pthread
)


###########################################################################
# CORE-TEST
###########################################################################

SET(TEST_CASES uuid_test)

SET(TEST_CASE_IMPLEMENTATION
  common/uuid_test)


file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/tests)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/tests/)


add_executable(uuid_test
  ${HEADER}
  ${SOURCES}
  ${CMAKE_SOURCE_DIR}/core-test/common/uuid_test.cc)

target_link_libraries(uuid_test PUBLIC
  ${Boost_LIBRARIES}
  ${Boost_UNIT_TEST_FRAMEWORK}
  pthread
)
