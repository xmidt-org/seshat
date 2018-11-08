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

set(PROJ_LIBSESHAT libseshat)

file(GLOB HEADERS libseshat.h)
set(SOURCES libseshat.c)

add_library(${PROJ_LIBSESHAT} STATIC ${HEADERS} ${SOURCES})
add_library(${PROJ_LIBSESHAT}.shared SHARED ${HEADERS} ${SOURCES})
set_target_properties(${PROJ_LIBSESHAT}.shared PROPERTIES OUTPUT_NAME ${PROJ_LIBSESHAT})
set_property(TARGET ${PROJ_LIBSESHAT} PROPERTY C_STANDARD 99)
set_property(TARGET ${PROJ_LIBSESHAT}.shared PROPERTY C_STANDARD 99)

# ----------------------------------------------------------------------------
# Note: This is a partial solution and only covers the case required for the
# Yocto build (ie dynamic lib). Other cases may need fixing too.
# ----------------------------------------------------------------------------
if (BUILD_YOCTO)
target_link_libraries(${PROJ_LIBSESHAT}.shared m log4c rdkloggers 
  cjson cimplog nanomsg wrp-c msgpackc trower-base64)
endif (BUILD_YOCTO)

install (TARGETS ${PROJ_LIBSESHAT} DESTINATION lib${LIB_SUFFIX})
install (TARGETS ${PROJ_LIBSESHAT}.shared DESTINATION lib${LIB_SUFFIX})
install (FILES libseshat.h DESTINATION include/${PROJ_LIBSESHAT})
