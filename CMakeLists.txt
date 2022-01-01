cmake_minimum_required(VERSION 3.21)
project(launcher)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_compile_definitions(APP_VERSION="1.3.0")

add_executable(launcher launcher/resources.qrc)
add_subdirectory(launcher)
include_directories(libs/QArchive)

if(WIN32)
    set_property(TARGET launcher PROPERTY WIN32_EXECUTABLE true)
    set(APP_ICON_RESOURCE_WINDOWS "${CMAKE_CURRENT_SOURCE_DIR}/launcher/resource.rc")
    target_sources(launcher PRIVATE ${APP_ICON_RESOURCE_WINDOWS})
endif()

find_package(Qt5 COMPONENTS Core Gui Network Widgets Concurrent REQUIRED)
find_package(LibArchive REQUIRED)
add_subdirectory(libs/QArchive)
target_link_libraries(launcher PRIVATE Qt5::Core Qt5::Gui Qt5::Network Qt5::Widgets Qt5::Concurrent
    LibArchive::LibArchive QArchive)
