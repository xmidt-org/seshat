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

set(PROJ_WRPC seshat)
set(SOURCES main.c nmsg.c listener_task.c wrp_interface.c json_interface.c)
set(FAKE_SERVER_SOURCES fake_server.c)

add_executable(seshat ${SOURCES})
add_executable(fake_server ${FAKE_SERVER_SOURCES})

target_link_libraries (seshat
    ${CMAKE_THREAD_LIBS_INIT}
    -lwrp-c
    -lmsgpackc
    -ltrower-base64
    -lm
    -lcimplog
    -lnng
    -lcjson
    -lpthread
    )
if (${CMAKE_SYSTEM_NAME} MATCHES "Linux")
target_link_libraries (seshat rt)
endif()
install (TARGETS seshat DESTINATION bin)

target_link_libraries (fake_server
    ${CMAKE_THREAD_LIBS_INIT}
    -lwrp-c
    -lmsgpackc
    -ltrower-base64
    -lm
    -lcimplog
    -lnng
    -lcjson
    -lpthread
    )
if (${CMAKE_SYSTEM_NAME} MATCHES "Linux")
target_link_libraries (fake_server rt)
endif()
