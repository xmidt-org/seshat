#   Copyright 2016 Comcast Cable Communications Management, LLC
#
#   Licensed under the Apache License, Version 2.0 (the "License");
#   you may not use this file except in compliance with the License.
#   You may obtain a copy of the License at
#
#       http://www.apache.org/licenses/LICENSE-2.0
#
#   Unless required by applicable law or agreed to in writing, software
#   distributed under the License is distributed on an "AS IS" BASIS,
#   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
#   See the License for the specific language governing permissions and
#   limitations under the License.

set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wno-error=missing-field-initializers -W -g -fprofile-arcs -ftest-coverage -O0")
set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -W  -g -fprofile-arcs -ftest-coverage -O0")
set (CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -fprofile-arcs -ftest-coverage -O0")

#-------------------------------------------------------------------------------
#   test_cjson_interface
#-------------------------------------------------------------------------------
add_test(NAME test_cjson_interface COMMAND test_cjson_interface)
add_executable(test_cjson_interface test_cjson_interface.c ../src/cjson_interface.c)
set_property(TARGET test_cjson_interface PROPERTY C_STANDARD 99)

target_link_libraries (test_cjson_interface -lpthread -lmsgpackc -ltrower-base64 -lcjson -lcimplog -lwrp-c -lnanomsg)
target_link_libraries (test_cjson_interface gcov)
target_link_libraries (test_cjson_interface cunit)

add_custom_target(coverage
                  COMMAND lcov -q --capture --directory ${CMAKE_CURRENT_BINARY_DIR}/CMakeFiles/test_cjson_interface.dir/__/src --output-file coverage.info
                  COMMAND genhtml coverage.info
                  WORKING_DIRECTORY ${CMAKE_BINARY_DIR})
