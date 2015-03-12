cmake_minimum_required(VERSION 2.8.4)
project(worlds_of_teeworlds)

SET(OS_WINDOWS FALSE)
SET(OS_BSD_FAMILY FALSE)
SET(OS_FREEBSD FALSE)
SET(OS_OPENBSD FALSE)
SET(OS_NETBSD FALSE)
SET(OS_LINUX FALSE)
SET(OS_MACOS FALSE)

IF(${CMAKE_SYSTEM} MATCHES "Windows")
    SET(OS_WINDOWS TRUE)
ENDIF(${CMAKE_SYSTEM} MATCHES "Windows")

IF(${CMAKE_SYSTEM} MATCHES "FreeBSD")
    SET(OS_FREEBSD TRUE)
    SET(OS_BSD_FAMILY TRUE)
ENDIF(${CMAKE_SYSTEM} MATCHES "FreeBSD")

IF (${CMAKE_SYSTEM} MATCHES "OpenBSD")
    SET(OS_OPENBSD TRUE)
    SET(OS_BSD_FAMILY TRUE)
ENDIF (${CMAKE_SYSTEM} MATCHES "OpenBSD")

IF (${CMAKE_SYSTEM} MATCHES "NetBSD")
    SET(OS_NETBSD TRUE)
    SET(OS_BSD_FAMILY TRUE)
ENDIF (${CMAKE_SYSTEM} MATCHES "NetBSD")

IF (${CMAKE_SYSTEM} MATCHES "Linux")
    SET(OS_LINUX TRUE)
ENDIF (${CMAKE_SYSTEM} MATCHES "Linux")

IF(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
    SET(OS_MACOS TRUE)
    SET(OS_BSD_FAMILY TRUE)
ENDIF(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")

#--------------GENERATED----------------

    FIND_PACKAGE(PythonInterp REQUIRED)
    IF(NOT PYTHON_EXECUTABLE)
        MESSAGE(SEND_ERROR "You need Python to build Python binding")
    ENDIF(NOT PYTHON_EXECUTABLE)


    #protocol.cpp
    execute_process(COMMAND ${PYTHON_EXECUTABLE} "${CMAKE_SOURCE_DIR}/datasrc/compile.py" "network_source"
                    OUTPUT_FILE ${CMAKE_SOURCE_DIR}/src/game/generated/protocol.cpp)

    #protocol.h
    execute_process(COMMAND ${PYTHON_EXECUTABLE} "${CMAKE_SOURCE_DIR}/datasrc/compile.py" "network_header"
                    OUTPUT_FILE ${CMAKE_SOURCE_DIR}/src/game/generated/protocol.h)

    #client_data.cpp
    execute_process(COMMAND ${PYTHON_EXECUTABLE} "${CMAKE_SOURCE_DIR}/datasrc/compile.py" "client_content_source"
                    OUTPUT_FILE ${CMAKE_SOURCE_DIR}/src/game/generated/client_data.cpp)

    #client_data.h
    execute_process(COMMAND ${PYTHON_EXECUTABLE} "${CMAKE_SOURCE_DIR}/datasrc/compile.py" "client_content_header"
                    OUTPUT_FILE ${CMAKE_SOURCE_DIR}/src/game/generated/client_data.h)

    #server_data.cpp
    execute_process(COMMAND ${PYTHON_EXECUTABLE} "${CMAKE_SOURCE_DIR}/datasrc/compile.py" "server_content_source"
                    OUTPUT_FILE ${CMAKE_SOURCE_DIR}/src/game/generated/server_data.cpp)

    #server_data.h
    execute_process(COMMAND ${PYTHON_EXECUTABLE} "${CMAKE_SOURCE_DIR}/datasrc/compile.py" "server_content_header"
                    OUTPUT_FILE ${CMAKE_SOURCE_DIR}/src/game/generated/server_data.h)

    #protocol_tdtw.cpp
    execute_process(COMMAND ${PYTHON_EXECUTABLE} "${CMAKE_SOURCE_DIR}/datasrc/compile_tdtw.py" "network_tdtw_source"
                    OUTPUT_FILE ${CMAKE_SOURCE_DIR}/src/game/generated/protocol_tdtw.cpp)

    #protocol_tdtw.h
    execute_process(COMMAND ${PYTHON_EXECUTABLE} "${CMAKE_SOURCE_DIR}/datasrc/compile_tdtw.py" "network_tdtw_header"
                    OUTPUT_FILE ${CMAKE_SOURCE_DIR}/src/game/generated/protocol_tdtw.h)

    #nethash.cpp
    execute_process(COMMAND ${PYTHON_EXECUTABLE}    "${CMAKE_SOURCE_DIR}/scripts/cmd5.py"
                                                    "${CMAKE_SOURCE_DIR}/src/engine/shared/protocol.h"
                                                    "${CMAKE_SOURCE_DIR}/src/game/generated/protocol.h"
                                                    "${CMAKE_SOURCE_DIR}/src/game/tuning.h"
                                                    "${CMAKE_SOURCE_DIR}/src/game/gamecore.h"
                                                    "${CMAKE_SOURCE_DIR}/src/game/generated/protocol.h"
                    OUTPUT_FILE ${CMAKE_SOURCE_DIR}/src/game/generated/nethash.cpp)

#--------------GENERATED----------------

include_directories(src)
include_directories(src/engine/external/zlib)

IF(OS_LINUX)
    find_package(SDL REQUIRED)
    if(NOT SDL_FOUND)
        message(ERROR: SDL not Found)
    endif()
    include_directories(${SDL_INCLUDE_DIR})


    find_package(Threads REQUIRED)


    find_package(Freetype REQUIRED)
    if(NOT FREETYPE_FOUND)
        message(ERROR: Freetype not found)
    endif()
    include_directories(${FREETYPE_INCLUDE_DIRS})
elseif(OS_WINDOWS)
    include_directories(other/sdl/include)
    include_directories(other/freetype/include)
    include_directories(other/sqlite/include)
    link_directories(other/freetype/lib32)
    link_directories(other/sdl/lib32)
    link_directories(other/sdl/lib32)
    link_directories(other/sqlite/lib)
ENDIF(OS_LINUX)

set(SOURCE_FILES
    src/base/tl/algorithm.h
    src/base/tl/allocator.h
    src/base/tl/array.h
    src/base/tl/base.h
    src/base/tl/range.h
    src/base/tl/sorted_array.h
    src/base/tl/string.h
    src/base/tl/threading.h
    src/base/detect.h
    src/base/math.h
    src/base/system.c
    src/base/system.h
    src/base/vmath.h
    src/engine/client/backend_sdl.cpp
    src/engine/client/backend_sdl.h
    src/engine/client/client.cpp
    src/engine/client/client.h
    src/engine/client/friends.cpp
    src/engine/client/friends.h
    src/engine/client/graphics.cpp
    src/engine/client/graphics.h
    src/engine/client/graphics_threaded.cpp
    src/engine/client/graphics_threaded.h
    src/engine/client/input.cpp
    src/engine/client/input.h
    src/engine/client/keynames.h
    src/engine/client/serverbrowser.cpp
    src/engine/client/serverbrowser.h
    src/engine/client/sound.cpp
    src/engine/client/sound.h
    src/engine/client/text.cpp
    src/engine/external/pnglite/pnglite.c
    src/engine/external/pnglite/pnglite.h
    src/engine/external/wavpack/bits.c
    src/engine/external/wavpack/float.c
    src/engine/external/wavpack/metadata.c
    src/engine/external/wavpack/unpack.c
    src/engine/external/wavpack/wavpack.h
    src/engine/external/wavpack/words.c
    src/engine/external/wavpack/wputils.c
    src/engine/external/zlib/adler32.c
    src/engine/external/zlib/compress.c
    src/engine/external/zlib/crc32.c
    src/engine/external/zlib/crc32.h
    src/engine/external/zlib/deflate.c
    src/engine/external/zlib/deflate.h
    src/engine/external/zlib/infback.c
    src/engine/external/zlib/inffast.c
    src/engine/external/zlib/inffast.h
    src/engine/external/zlib/inffixed.h
    src/engine/external/zlib/inflate.c
    src/engine/external/zlib/inflate.h
    src/engine/external/zlib/inftrees.c
    src/engine/external/zlib/inftrees.h
    src/engine/external/zlib/trees.c
    src/engine/external/zlib/trees.h
    src/engine/external/zlib/uncompr.c
    src/engine/external/zlib/zconf.h
    src/engine/external/zlib/zlib.h
    src/engine/external/zlib/zutil.c
    src/engine/external/zlib/zutil.h
    src/engine/server/register.cpp
    src/engine/server/register.h
    src/engine/server/server.cpp
    src/engine/server/server.h
    src/engine/shared/compression.cpp
    src/engine/shared/compression.h
    src/engine/shared/config.cpp
    src/engine/shared/config.h
    src/engine/shared/config_variables.h
    src/engine/shared/console.cpp
    src/engine/shared/console.h
    src/engine/shared/datafile.cpp
    src/engine/shared/datafile.h
    src/engine/shared/demo.cpp
    src/engine/shared/demo.h
    src/engine/shared/econ.cpp
    src/engine/shared/econ.h
    src/engine/shared/engine.cpp
    src/engine/shared/filecollection.cpp
    src/engine/shared/filecollection.h
    src/engine/shared/huffman.cpp
    src/engine/shared/huffman.h
    src/engine/shared/jobs.cpp
    src/engine/shared/jobs.h
    src/engine/shared/kernel.cpp
    src/engine/shared/linereader.cpp
    src/engine/shared/linereader.h
    src/engine/shared/map.cpp
    src/engine/shared/mapchecker.cpp
    src/engine/shared/mapchecker.h
    src/engine/shared/masterserver.cpp
    src/engine/shared/memheap.cpp
    src/engine/shared/memheap.h
    src/engine/shared/message.h
    src/engine/shared/netban.cpp
    src/engine/shared/netban.h
    src/engine/shared/network.cpp
    src/engine/shared/network.h
    src/engine/shared/network_client.cpp
    src/engine/shared/network_conn.cpp
    src/engine/shared/network_console.cpp
    src/engine/shared/network_console_conn.cpp
    src/engine/shared/network_server.cpp
    src/engine/shared/packer.cpp
    src/engine/shared/packer.h
    src/engine/shared/protocol.h
    src/engine/shared/ringbuffer.cpp
    src/engine/shared/ringbuffer.h
    src/engine/shared/snapshot.cpp
    src/engine/shared/snapshot.h
    src/engine/shared/storage.cpp
    src/engine/client.h
    src/engine/config.h
    src/engine/console.h
    src/engine/demo.h
    src/engine/editor.h
    src/engine/engine.h
    src/engine/friends.h
    src/engine/graphics.h
    src/engine/input.h
    src/engine/kernel.h
    src/engine/keys.h
    src/engine/map.h
    src/engine/masterserver.h
    src/engine/message.h
    src/engine/server.h
    src/engine/serverbrowser.h
    src/engine/sound.h
    src/engine/storage.h
    src/engine/textrender.h
    src/game/client/components/binds.cpp
    src/game/client/components/binds.h
    src/game/client/components/broadcast.cpp
    src/game/client/components/broadcast.h
    src/game/client/components/camera.cpp
    src/game/client/components/camera.h
    src/game/client/components/chat.cpp
    src/game/client/components/chat.h
    src/game/client/components/console.cpp
    src/game/client/components/console.h
    src/game/client/components/controls.cpp
    src/game/client/components/controls.h
    src/game/client/components/countryflags.cpp
    src/game/client/components/countryflags.h
    src/game/client/components/damageind.cpp
    src/game/client/components/damageind.h
    src/game/client/components/debughud.cpp
    src/game/client/components/debughud.h
    src/game/client/components/effects.cpp
    src/game/client/components/effects.h
    src/game/client/components/emoticon.cpp
    src/game/client/components/emoticon.h
    src/game/client/components/flow.cpp
    src/game/client/components/flow.h
    src/game/client/components/hud.cpp
    src/game/client/components/hud.h
    src/game/client/components/items.cpp
    src/game/client/components/items.h
    src/game/client/components/killmessages.cpp
    src/game/client/components/killmessages.h
    src/game/client/components/mapimages.cpp
    src/game/client/components/mapimages.h
    src/game/client/components/maplayers.cpp
    src/game/client/components/maplayers.h
    src/game/client/components/menus.cpp
    src/game/client/components/menus.h
    src/game/client/components/menus_browser.cpp
    src/game/client/components/menus_demo.cpp
    src/game/client/components/menus_ingame.cpp
    src/game/client/components/menus_settings.cpp
    src/game/client/components/motd.cpp
    src/game/client/components/motd.h
    src/game/client/components/nameplates.cpp
    src/game/client/components/nameplates.h
    src/game/client/components/particles.cpp
    src/game/client/components/particles.h
    src/game/client/components/players.cpp
    src/game/client/components/players.h
    src/game/client/components/scoreboard.cpp
    src/game/client/components/scoreboard.h
    src/game/client/components/skins.cpp
    src/game/client/components/skins.h
    src/game/client/components/sounds.cpp
    src/game/client/components/sounds.h
    src/game/client/components/spectator.cpp
    src/game/client/components/spectator.h
    src/game/client/components/voting.cpp
    src/game/client/components/voting.h
    src/game/client/animstate.cpp
    src/game/client/animstate.h
    src/game/client/component.h
    src/game/client/gameclient.cpp
    src/game/client/gameclient.h
    src/game/client/lineinput.cpp
    src/game/client/lineinput.h
    src/game/client/render.cpp
    src/game/client/render.h
    src/game/client/render_map.cpp
    src/game/client/ui.cpp
    src/game/client/ui.h
    src/game/editor/auto_map.cpp
    src/game/editor/auto_map.h
    src/game/editor/editor.cpp
    src/game/editor/editor.h
    src/game/editor/io.cpp
    src/game/editor/layer_game.cpp
    src/game/editor/layer_quads.cpp
    src/game/editor/layer_tiles.cpp
    src/game/editor/popups.cpp
    src/game/server/entities/character.cpp
    src/game/server/entities/character.h
    src/game/server/entities/flag.cpp
    src/game/server/entities/flag.h
    src/game/server/entities/laser.cpp
    src/game/server/entities/laser.h
    src/game/server/entities/pickup.cpp
    src/game/server/entities/pickup.h
    src/game/server/entities/projectile.cpp
    src/game/server/entities/projectile.h
    src/game/server/gamemodes/ctf.cpp
    src/game/server/gamemodes/ctf.h
    src/game/server/gamemodes/dm.cpp
    src/game/server/gamemodes/dm.h
    src/game/server/gamemodes/mod.cpp
    src/game/server/gamemodes/mod.h
    src/game/server/gamemodes/tdm.cpp
    src/game/server/gamemodes/tdm.h
    src/game/server/entity.cpp
    src/game/server/entity.h
    src/game/server/eventhandler.cpp
    src/game/server/eventhandler.h
    src/game/server/gamecontext.cpp
    src/game/server/gamecontext.h
    src/game/server/gamecontroller.cpp
    src/game/server/gamecontroller.h
    src/game/server/gameworld.cpp
    src/game/server/gameworld.h
    src/game/server/player.cpp
    src/game/server/player.h
    src/game/collision.cpp
    src/game/collision.h
    src/game/gamecore.cpp
    src/game/gamecore.h
    src/game/layers.cpp
    src/game/layers.h
    src/game/localization.cpp
    src/game/localization.h
    src/game/mapitems.h
    src/game/tuning.h
    src/game/variables.h
    src/game/version.h
    src/game/voting.h
    src/mastersrv/mastersrv.cpp
    src/mastersrv/mastersrv.h
    src/osxlaunch/client.h
    src/tdtwsrv/tdtwsrv.h
    src/tdtwsrv/game.h
    src/tdtwsrv/server/tdtwsrv.h
    src/tdtwsrv/server/tdtwsrv.cpp
    src/tdtwsrv/server/protocol.h
    src/tdtwsrv/server/protocol.cpp
    src/tdtwsrv/game/game.h
    src/tdtwsrv/game/game.cpp
    src/tools/crapnet.cpp
    src/tools/dilate.cpp
    src/tools/fake_server.cpp
    src/tools/map_resave.cpp
    src/tools/map_version.cpp
    src/tools/packetgen.cpp
    src/tools/tileset_borderadd.cpp
    src/tools/tileset_borderfix.cpp
    src/tools/tileset_borderrem.cpp
    src/tools/tileset_borderset.cpp
    src/versionsrv/mapversions.h
    src/versionsrv/versionsrv.cpp
    src/versionsrv/versionsrv.h)


file(GLOB ENGINE src/engine/shared/*.cpp src/base/*.c)
file(GLOB SERVER src/engine/server/*.cpp)
file(GLOB CLIENT src/engine/client/*.cpp)
file(GLOB VERSION_SERVER src/versionsrc/*.cpp)
file(GLOB MASTER_SERVER src/mastersrv/*.cpp)
file(GLOB_RECURSE TDTW_SERVER src/tdtwsrv/*.cpp)
file(GLOB GAME_SHARED src/game/*.cpp src/game/generated/nethash.cpp src/game/generated/protocol.cpp src/game/generated/protocol_tdtw.cpp)
file(GLOB_RECURSE GAME_CLIENT src/game/client/*.cpp src/game/generated/client_data.cpp)
file(GLOB_RECURSE GAME_SERVER src/game/server/*.cpp src/game/generated/server_data.cpp)
file(GLOB GAME_EDITOR src/game/editor/*.cpp)

file(GLOB TOOLS src/tools/*.cpp src/tools/*.c)

file(GLOB ZLIB src/engine/external/zlib/*.c)
file(GLOB WAVPACK src/engine/external/wavpack/*.c)
file(GLOB PNGLITE src/engine/external/pnglite/*.c)


set(EXECUTABLE_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/)

if(OS_WINDOWS)
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -static-libgcc -static-libstdc++")
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -static-libgcc -static-libstdc++")
endif(OS_WINDOWS)

#SERVER_DEBUG
add_executable(server_debug ${ENGINE} ${GAME_SHARED} ${TDTW_SERVER} ${ZLIB})

if(OS_LINUX)
    target_link_libraries(server_debug ${CMAKE_THREAD_LIBS_INIT})
elseif(OS_WINDOWS)
    target_link_libraries(server_debug ws2_32 sqlite3)
endif()

#SERVER_RELEASE
add_executable(server_release ${ENGINE} ${GAME_SHARED} ${TDTW_SERVER} ${ZLIB})

if(OS_LINUX)
    target_link_libraries(server_release ${CMAKE_THREAD_LIBS_INIT})
elseif(OS_WINDOWS)
    target_link_libraries(server_release ws2_32 sqlite3)
endif()


#CLIENT_RELEASE
add_executable(client_release ${GAME_SHARED} ${GAME_CLIENT} ${ENGINE}
 ${CLIENT} ${GAME_EDITOR} ${ZLIB} ${PNGLITE} ${WAVPACK})

if(OS_LINUX)
    target_link_libraries(client_release ${SDL_LIBRARY} ${FREETYPE_LIBRARY} GL GLU)
elseif(OS_WINDOWS)
    target_link_libraries(client_release freetype SDL SDLmain gdi32 ws2_32 user32 ole32 shell32 opengl32 glu32 winmm)
    execute_process(COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_SOURCE_DIR}/other/freetype/lib32/freetype.dll ${CMAKE_SOURCE_DIR}/freetype.dll)
    execute_process(COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_SOURCE_DIR}/other/sdl/lib32/sdl.dll ${CMAKE_SOURCE_DIR}/sdl.dll)
endif()
#CLIENT_DEBUG

add_executable(client_debug ${GAME_SHARED} ${GAME_CLIENT} ${ENGINE}
 ${CLIENT} ${GAME_EDITOR} ${ZLIB} ${PNGLITE} ${WAVPACK})

if(OS_LINUX)
    target_link_libraries(client_debug ${SDL_LIBRARY} ${FREETYPE_LIBRARY} GL GLU)
elseif(OS_WINDOWS)
    target_link_libraries(client_debug freetype SDL SDLmain gdi32 ws2_32 user32 ole32 shell32 opengl32 glu32 winmm)
    execute_process(COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_SOURCE_DIR}/other/freetype/lib32/freetype.dll ${CMAKE_SOURCE_DIR}/freetype.dll)
    execute_process(COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_SOURCE_DIR}/other/sdl/lib32/sdl.dll ${CMAKE_SOURCE_DIR}/sdl.dll)
endif()