cmake_minimum_required(VERSION 3.16)

project(launcher LANGUAGES CXX)
include(version.cmake)

configure_file(templates/version.h.in ${PROJECT_SOURCE_DIR}/src/version.h)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt6 6.5 REQUIRED COMPONENTS Core Qml Quick)

qt_standard_project_setup(REQUIRES 6.5)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

qt_add_executable(launcher
  src/main.cpp
  src/version.h
  src/launchercore.h src/launchercore.cpp
  src/settings.h src/settings.cpp
  src/qjsonformat.h
  src/webaccess.h src/webaccess.cpp
  src/datatypes.h
  src/launcherui.h src/launcherui.cpp
  src/launcherqmlinterface.h src/launcherqmlinterface.cpp
)

qt_add_qml_module(launcher
  URI launcher
  QML_FILES
    data/main.qml
    data/settings.qml
)

target_link_libraries(launcher PRIVATE Qt6::Core Qt6::Qml Qt6::Quick)

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
