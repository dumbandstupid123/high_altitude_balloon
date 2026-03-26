cmake_minimum_required(VERSION 3.20)
project(high_altitude_balloon_firmware C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

add_executable(hab_sim
    src/hab_app.c
    src/hab_gps.c
    src/hab_math.c
    src/hab_protocol.c
    src/main.c
    src/platform_stub.c
)

target_compile_definitions(hab_sim PRIVATE HAB_HOST_SIM=1)

target_include_directories(hab_sim PRIVATE
    include
    third_party/bme68x
    third_party/fatfs/source
)

if(MSVC)
    target_compile_options(hab_sim PRIVATE /W4)
else()
    target_compile_options(hab_sim PRIVATE -Wall -Wextra -Wpedantic)
endif()

target_link_libraries(hab_sim PRIVATE m)
