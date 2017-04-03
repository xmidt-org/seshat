#   Copyright 2017 Comcast Cable Communications Management, LLC
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

add_test(NAME seshat_e2e_test COMMAND simple_test)

set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wno-error=missing-field-initializers -W -g -fprofile-arcs -ftest-coverage -O0")
set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -W  -g -fprofile-arcs -ftest-coverage -O0")
set (CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -fprofile-arcs -ftest-coverage -O0")

add_executable(simple_test simple.c ../src/libseshat.c)
set_property(TARGET simple_test PROPERTY C_STANDARD 99)

target_link_libraries (simple_test  -lwrp-c -lmsgpackc -ltrower-base64 -lnanomsg -luuid -lcimplog)

target_link_libraries (simple_test gcov)
target_link_libraries (simple_test cunit)
target_link_libraries (simple_test rt)

#-------------------------------------------------------------------------------


