cmake_minimum_required(VERSION 3.22)

#
# This file is generated only once,
# and is not re-generated if converter is called multiple times.
#
# User is free to modify the file as much as necessary
#

# Setup compiler settings
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS ON)


# Define the build type
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Debug")
endif()

# Set the project name
set(CMAKE_PROJECT_NAME vscodestm32watch)

# Enable compile command to ease indexing with e.g. clangd
set(CMAKE_EXPORT_COMPILE_COMMANDS TRUE)

# Core project settings
project(${CMAKE_PROJECT_NAME})
message("Build type: " ${CMAKE_BUILD_TYPE})

# Enable CMake support for ASM and C languages
enable_language(C ASM)

# Create an executable object type
add_executable(${CMAKE_PROJECT_NAME})

# Add STM32CubeMX generated sources
add_subdirectory(cmake/stm32cubemx)

# Link directories setup
target_link_directories(${CMAKE_PROJECT_NAME} PRIVATE
    # Add user defined library search paths
)

# Add sources to executable
target_sources(${CMAKE_PROJECT_NAME} PRIVATE
    Core/Src/delay.c
    Core/Src/key.c
    Core/Src/led.c
    Core/Src/menu.c
    Core/Src/MyRTC.c
    Core/Src/settime.c
    Core/Src/OLED.c
    Core/Src/OLED_Data.c
    Core/Src/Timer.c
    Core/Src/MPU6050.c
    Core/Src/dino.c
    Core/Src/MyI2C.c
)

# Add include paths
target_include_directories(${CMAKE_PROJECT_NAME} PRIVATE
    # Add user defined include paths
)

# Add project symbols (macros)
target_compile_definitions(${CMAKE_PROJECT_NAME} PRIVATE
    # Add user defined symbols
)

# Remove wrong libob.a library dependency when using cpp files
list(REMOVE_ITEM CMAKE_C_IMPLICIT_LINK_LIBRARIES ob)

# Add linked libraries
target_link_libraries(${CMAKE_PROJECT_NAME}
    stm32cubemx

    # Add user defined libraries
)

# Auto flash via ST-LINK after successful build.
option(AUTO_FLASH_ON_BUILD "Automatically flash target after build" ON)
if(AUTO_FLASH_ON_BUILD AND CMAKE_HOST_WIN32)
    file(GLOB STM32_PROGRAMMER_CLI
        "$ENV{LOCALAPPDATA}/stm32cube/bundles/programmer/*/bin/STM32_Programmer_CLI.exe")

    list(LENGTH STM32_PROGRAMMER_CLI STM32_PROGRAMMER_CLI_COUNT)
    if(STM32_PROGRAMMER_CLI_COUNT GREATER 0)
        list(GET STM32_PROGRAMMER_CLI 0 STM32_PROGRAMMER_CLI_EXE)
        message(STATUS "Auto flash enabled: ${STM32_PROGRAMMER_CLI_EXE}")

        add_custom_target(flash ALL
            COMMAND "${STM32_PROGRAMMER_CLI_EXE}" -c port=SWD -w $<TARGET_FILE:${CMAKE_PROJECT_NAME}> -v -rst
            DEPENDS ${CMAKE_PROJECT_NAME}
            COMMENT "Flashing firmware to STM32 via ST-LINK"
            VERBATIM)
    else()
        message(WARNING "AUTO_FLASH_ON_BUILD is ON but STM32_Programmer_CLI.exe was not found.")
    endif()
endif()
