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

set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wno-error=missing-field-initializers -W -g -fprofile-arcs -ftest-coverage -O0")
set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -W  -g -fprofile-arcs -ftest-coverage -O0")
set (CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -fprofile-arcs -ftest-coverage -O0")
set (LIBSESHAT_COMMON_LIBS -lcunit)
if (${CMAKE_SYSTEM_NAME} MATCHES "Linux")
set (LIBSESHAT_LINUX_ONLY_LIBS gcov -luuid -lrt -Wl,--no-as-needed)
endif()

#-------------------------------------------------------------------------------
#   simple_test
#-------------------------------------------------------------------------------
add_test(NAME seshat_e2e_test COMMAND simple_test)
add_executable(simple_test simple.c ../src/libseshat.c)
set_property(TARGET simple_test PROPERTY C_STANDARD 99)

target_link_libraries (simple_test ${LIBSESHAT_LINUX_ONLY_LIBS} ${LIBSESHAT_COMMON_LIBS}
 -lwrp-c -lmsgpackc -ltrower-base64 -lnng -lcimplog -lpthread)

#-------------------------------------------------------------------------------
#   test_init_lib_seshat_failure-3
#------------------------------------------------------------------------------
add_test(NAME test_init_lib_seshat_failure-3 COMMAND test_init_lib_seshat_failure-3)
add_executable(test_init_lib_seshat_failure-3 test_init_lib_seshat_failure-3.c ../src/libseshat.c)
set_property(TARGET test_init_lib_seshat_failure-3 PROPERTY C_STANDARD 99)

target_link_libraries (test_init_lib_seshat_failure-3 ${LIBSESHAT_LINUX_ONLY_LIBS} ${LIBSESHAT_COMMON_LIBS}
 -lwrp-c -lmsgpackc -ltrower-base64 -lcimplog -lpthread)

#-------------------------------------------------------------------------------
#   test_init_lib_seshat_failure-4
#------------------------------------------------------------------------------
add_test(NAME test_init_lib_seshat_failure-4 COMMAND test_init_lib_seshat_failure-4)
add_executable(test_init_lib_seshat_failure-4 test_init_lib_seshat_failure-4.c ../src/libseshat.c)
set_property(TARGET test_init_lib_seshat_failure-4 PROPERTY C_STANDARD 99)

target_link_libraries (test_init_lib_seshat_failure-4 ${LIBSESHAT_LINUX_ONLY_LIBS} ${LIBSESHAT_COMMON_LIBS}
 -lwrp-c -lmsgpackc -ltrower-base64 -lcimplog -lpthread)

#-------------------------------------------------------------------------------
#   test_init_lib_seshat_failure-5
#------------------------------------------------------------------------------
add_test(NAME test_init_lib_seshat_failure-5 COMMAND test_init_lib_seshat_failure-5)
add_executable(test_init_lib_seshat_failure-5 test_init_lib_seshat_failure-5.c ../src/libseshat.c)
set_property(TARGET test_init_lib_seshat_failure-5 PROPERTY C_STANDARD 99)

target_link_libraries (test_init_lib_seshat_failure-5 ${LIBSESHAT_LINUX_ONLY_LIBS} ${LIBSESHAT_COMMON_LIBS}
 -lwrp-c -lmsgpackc -ltrower-base64 -lcimplog -lpthread)

