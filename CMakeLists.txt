cmake_minimum_required(VERSION 3.16)
project(ro-gallery VERSION 0.1.0 LANGUAGES CXX)

include(GNUInstallDirs)
include(CTest)
set(RO_GALLERY_VERSION "${PROJECT_VERSION}" CACHE STRING "Ro-Gallery release version")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)

find_package(Qt6 REQUIRED COMPONENTS Widgets)

qt_add_executable(ro-gallery
    src/main.cpp
    src/theme.h
    src/theme.cpp
    src/icons.h
    src/icons.cpp
    src/settings.h
    src/settings.cpp
    src/exifdate.h
    src/exifdate.cpp
    src/library.h
    src/library.cpp
    src/thumbs.h
    src/thumbs.cpp
    src/grid.h
    src/grid.cpp
    src/sidebar.h
    src/sidebar.cpp
    src/topbar.h
    src/topbar.cpp
    src/viewer.h
    src/viewer.cpp
    src/mainwindow.h
    src/mainwindow.cpp
)

target_link_libraries(ro-gallery PRIVATE Qt6::Widgets)
target_compile_definitions(ro-gallery PRIVATE RO_GALLERY_VERSION="${RO_GALLERY_VERSION}")

install(TARGETS ro-gallery RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
install(FILES ro-gallery.desktop DESTINATION ${CMAKE_INSTALL_DATADIR}/applications)
install(FILES assets/ro-gallery.svg DESTINATION ${CMAKE_INSTALL_DATADIR}/icons/hicolor/scalable/apps)
install(FILES data/org.roasd.RoGallery.metainfo.xml DESTINATION ${CMAKE_INSTALL_DATADIR}/metainfo)

if(BUILD_TESTING)
    find_package(Python3 REQUIRED COMPONENTS Interpreter)
    add_test(NAME application-smoke
        COMMAND ${Python3_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/tests/smoke.py $<TARGET_FILE:ro-gallery>)
    set_tests_properties(application-smoke PROPERTIES TIMEOUT 30)
endif()
