cmake_minimum_required(VERSION 3.28)
project(SfmlPuste LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 26)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include(FetchContent)
FetchContent_Declare(SFML
    GIT_REPOSITORY https://github.com/SFML/SFML.git
    GIT_TAG 3.1.0
    GIT_SHALLOW ON
    EXCLUDE_FROM_ALL
    SYSTEM
)
FetchContent_MakeAvailable(SFML)

add_executable(main main.cpp
    player.cpp
    enemy.cpp
    projectile.cpp
    )
target_compile_features(main PRIVATE cxx_std_26)
target_link_libraries(main PRIVATE SFML::Graphics SFML::Audio)
