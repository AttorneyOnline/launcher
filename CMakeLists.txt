cmake_minimum_required(VERSION 3.16)

project(launcher LANGUAGES CXX)
include(version.cmake)

configure_file(templates/version.h.in ${PROJECT_SOURCE_DIR}/src/version.h)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Core Quick)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Core Quick)

add_executable(launcher
  src/main.cpp
  src/version.h
  src/launchercore.h src/launchercore.cpp
  src/manifestfetcher.h src/manifestfetcher.cpp
  src/settings.h src/settings.cpp
  src/qjsonformat.h
)
target_link_libraries(launcher Qt${QT_VERSION_MAJOR}::Core Qt${QT_VERSION_MAJOR}::Quick)

include(GNUInstallDirs)
install(TARGETS launcher
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

set_target_properties(launcher PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/bin")

if(WIN32)
  if(CMAKE_BUILD_TYPE STREQUAL "Release")
    set_property(TARGET launcher PROPERTY WIN32_EXECUTABLE true)
    set(APP_ICON_RESOURCE_WINDOWS "${CMAKE_CURRENT_SOURCE_DIR}/data/logo-client.rc")
    target_sources(launcher PRIVATE ${APP_ICON_RESOURCE_WINDOWS})
  endif()
endif()

set_target_properties(launcher PROPERTIES
        LIBRARY_OUTPUT_DIRECTORY $<1:${CMAKE_CURRENT_LIST_DIR}/bin>
        RUNTIME_OUTPUT_DIRECTORY $<1:${CMAKE_CURRENT_LIST_DIR}/bin>)
