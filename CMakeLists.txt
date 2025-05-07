cmake_minimum_required(VERSION 3.16)

project(Launcher LANGUAGES CXX)
include(version.cmake)

configure_file(templates/version.h.in ${PROJECT_SOURCE_DIR}/src/version.h)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Core Quick)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Core Quick)

add_executable(Launcher
  src/main.cpp
  src/version.h
  src/launchercore.h src/launchercore.cpp
  src/manifestfetcher.h src/manifestfetcher.cpp
  src/settings.h src/settings.cpp
  src/qjsonformat.h
)
target_link_libraries(Launcher Qt${QT_VERSION_MAJOR}::Core Qt${QT_VERSION_MAJOR}::Quick)

include(GNUInstallDirs)
install(TARGETS Launcher
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
