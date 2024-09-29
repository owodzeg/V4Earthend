cmake_minimum_required(VERSION 3.19)
set(VCPKG_TARGET_TRIPLET "x64-windows-static" CACHE STRING "Vcpkg target triplet")

project(V4Earthend VERSION 1.1)
add_definitions(-DPATAFOUR_VERSION="${CMAKE_PROJECT_VERSION}")

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake")

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# c++20 handled differently on msvc
if(!MSVC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++20")
endif()

# make sure everything is handled statically on msvc
if(MSVC)
    set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
endif()

include(FetchContent)

# sfml and curl
set(SFML_STATIC_LIBRARIES TRUE)

if(MSVC)
    find_package(SFML REQUIRED COMPONENTS graphics network)
else()
    FetchContent_Declare(
            SFML
            GIT_REPOSITORY https://github.com/SFML/SFML.git
            GIT_TAG 2.6.1)
    FetchContent_MakeAvailable(SFML)

    FetchContent_Declare(curl
            GIT_REPOSITORY https://github.com/curl/curl.git
            DOWNLOAD_EXTRACT_TIMESTAMP true
            OVERRIDE_FIND_PACKAGE
    )

    FetchContent_MakeAvailable(curl)
endif()

# curl through find_package on all arch
find_package(curl)

# get json
FetchContent_Declare(json
        GIT_REPOSITORY https://github.com/nlohmann/json
        GIT_TAG v3.11.2)
FetchContent_MakeAvailable(json)

# get spdlog
FetchContent_Declare(spdlog
        GIT_REPOSITORY https://github.com/gabime/spdlog
        GIT_TAG v1.14.1)
FetchContent_MakeAvailable(spdlog)

FetchContent_Declare(libzippp
        GIT_REPOSITORY https://github.com/ctabin/libzippp
        GIT_TAG libzippp-v7.1-1.10.1)
FetchContent_MakeAvailable(libzippp)

# get nfd (for dialogs)
FetchContent_Declare(nfd
        GIT_REPOSITORY https://github.com/mlabbe/nativefiledialog.git
        GIT_TAG release_116)
FetchContent_MakeAvailable(nfd)

include_directories(${nfd_SOURCE_DIR}/src/include)

# platform exclusives
if(WIN32)
    set(NFD_SRC ${nfd_SOURCE_DIR}/src/nfd_common.c ${nfd_SOURCE_DIR}/src/nfd_win.cpp)
elseif(APPLE)
    set(NFD_SRC ${nfd_SOURCE_DIR}/src/nfd_common.c ${nfd_SOURCE_DIR}/src/nfd_cocoa.m)
elseif(UNIX AND NOT APPLE)
    set(NFD_SRC ${nfd_SOURCE_DIR}/src/nfd_common.c ${nfd_SOURCE_DIR}/src/nfd_gtk.c)
    # gtk for linux is required
    find_package(PkgConfig REQUIRED)
    pkg_check_modules(GTK3 REQUIRED gtk+-3.0)
    include_directories(${GTK3_INCLUDE_DIRS})
    link_directories(${GTK3_LIBRARY_DIRS})
    add_definitions(${GTK3_CFLAGS_OTHER})
endif()

set(ENGINE_FILES
        Engine/V4Earthend.cpp
        Engine/CoreManager.cpp
        Engine/ResourceManager.cpp
        Engine/TextureManager.cpp
        Engine/StateManager.cpp
        Engine/StringRepository.cpp
        Engine/Config.cpp
        Engine/Func.cpp
        Engine/Input/InputController.cpp
        Engine/Graphics/PSprite.cpp
        Engine/Graphics/SpriteWrapper.cpp
        Engine/Menu/FirstRun.cpp
        Engine/Menu/Entry.cpp
        Engine/Worker.cpp
        Engine/Dialog/DialogBox.cpp
        Engine/Dialog/MessageCloud.cpp
        Engine/Dialog/RoundedRect.cpp
        Engine/Graphics/PText.cpp
        Engine/Graphics/RichText.cpp
        Engine/Input/MouseController.cpp
        Engine/Cert.cpp
        Engine/Mechanics/Background.cpp
        Engine/Mechanics/Camera.cpp
        Engine/Graphics/PNGAnimation.cpp
        Engine/Mechanics/AnimatedObject.cpp
        Engine/Mechanics/Globals.cpp
        Engine/Input/TextInputController.cpp
)

add_executable(V4Earthend ${ENGINE_FILES} ${NFD_SRC} main.cpp)

if (CMAKE_BUILD_TYPE STREQUAL "Release")
    message("Enabling Unity build for Release build")
    set_target_properties(V4Earthend PROPERTIES
            UNITY_BUILD ON
            UNITY_BUILD_BATCH_SIZE 5)
endif()

if(MSVC)
    # fix sfml rect
    add_definitions(-DLEAN_AND_MEAN)
    add_definitions(-DNOMINMAX)
    target_compile_options(V4Earthend PRIVATE /std:c++20)
    target_compile_definitions(V4Earthend PRIVATE _USE_MATH_DEFINES=1)
else()
    target_compile_options(V4Earthend PRIVATE -Wall -Wextra)
endif()

target_include_directories(V4Earthend PRIVATE Engine/)
target_include_directories(V4Earthend PRIVATE DiscordSDK)

target_link_libraries(V4Earthend PRIVATE
        sfml-graphics
        sfml-network
        nlohmann_json::nlohmann_json
        spdlog::spdlog
        CURL::libcurl
        libzippp)

# Link GTK3 for Linux
if (UNIX AND NOT APPLE)
    target_link_libraries(V4Earthend PRIVATE ${GTK3_LIBRARIES})
endif()

if (OS STREQUAL "Linux")
    find_package(X11 REQUIRED)
    target_link_libraries(V4Earthend PRIVATE X11::X11)
endif (OS STREQUAL "Linux")