cmake_minimum_required(VERSION 3.14)
set(CMAKE_C_STANDARD 11)

set(ENVSDK $ENV{PLAYDATE_SDK_PATH})


if (NOT ${ENVSDK} STREQUAL "")
	# Convert path from Windows
	file(TO_CMAKE_PATH ${ENVSDK} SDK)
else()
	execute_process(
			COMMAND bash -c "egrep '^\\s*SDKRoot' $HOME/.Playdate/config"
			COMMAND head -n 1
			COMMAND cut -c9-
			OUTPUT_VARIABLE SDK
			OUTPUT_STRIP_TRAILING_WHITESPACE
	)
endif()

if (NOT EXISTS ${SDK})
	message(FATAL_ERROR "SDK Path not found; set ENV value PLAYDATE_SDK_PATH")
	return()
endif()

if ($ENV{BUILD_DEVICE})
	include(${SDK}/C_API/buildsupport/arm.cmake)
endif()

set(CMAKE_CONFIGURATION_TYPES "Debug;Release")
set(CMAKE_XCODE_GENERATE_SCHEME TRUE)

# Game Name Customization
set(PLAYDATE_GAME_NAME GAME)
set(PLAYDATE_GAME_DEVICE GAME_DEVICE)

project(${PLAYDATE_GAME_NAME} C ASM)

file(GLOB SRC_FILES 
		engine/platform.c
		engine/render.c
		engine/utils.c
		engine/animation.c
		engine/image.c
		engine/input.c
		engine/trace.c
		engine/entity.c
		engine/engine.c
		engine/font.c
		engine/map.c
		src/scenes/game.c
		src/entities/coin.c
		src/entities/player.c
		src/main.c
)

if (TOOLCHAIN STREQUAL "armgcc")
	add_executable(${PLAYDATE_GAME_DEVICE}
	${SRC_FILES}
	)
	target_include_directories(${PLAYDATE_GAME_DEVICE} 
		PRIVATE
		src
		engine
	)
	target_compile_definitions(${PLAYDATE_GAME_DEVICE} PUBLIC PLATFORM_PLAYDATE=1 RENDER_PLAYDATE=1)
else()
	add_library(${PLAYDATE_GAME_NAME} SHARED
	${SRC_FILES}
	)
	target_include_directories(${PLAYDATE_GAME_NAME} 
		PRIVATE
		src
		engine
	)
	target_compile_definitions(${PLAYDATE_GAME_NAME} PUBLIC PLATFORM_PLAYDATE=1 RENDER_PLAYDATE=1)
endif()

include(${SDK}/C_API/buildsupport/playdate_game.cmake)

