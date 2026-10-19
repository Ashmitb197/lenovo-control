cmake_minimum_required(VERSION 3.21)

project(lenovo-control VERSION 0.4.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 6.4 REQUIRED COMPONENTS Quick DBus)

option(LENOVO_CONTROL_TRAY
    "Enable optional system tray integration"
    ON
)

if(LENOVO_CONTROL_TRAY)
    find_package(Qt6 6.4 QUIET COMPONENTS Widgets)
endif()

qt_standard_project_setup(REQUIRES 6.4)

qt_add_executable(lenovo-control-native
    src/main.cpp
    src/HardwareController.cpp
    src/HardwareController.h
    src/HardwareDBus.h
    src/HardwareDBus.cpp
)

qt_policy(SET QTP0001 NEW)
qt_policy(SET QTP0004 NEW)

qt_add_qml_module(lenovo-control-native
    NO_IMPORT_SCAN
    URI LenovoControl
    VERSION 1.0
    QML_FILES
        qml/Main.qml
        qml/TrayPopup.qml
)

target_link_libraries(lenovo-control-native
    PRIVATE Qt6::Quick Qt6::DBus
)

if(LENOVO_CONTROL_TRAY AND TARGET Qt6::Widgets)
    target_link_libraries(lenovo-control-native
        PRIVATE Qt6::Widgets
    )

    target_compile_definitions(lenovo-control-native
        PRIVATE LENOVO_CONTROL_HAS_TRAY=1
    )

    message(STATUS "Lenovo Control: tray support enabled")
else()
    message(STATUS "Lenovo Control: building without tray support")
endif()


# Independent D-Bus hardware monitoring service
qt_add_executable(lenovo-control-service
    src/service_main.cpp
    src/HardwareController.cpp
    src/HardwareController.h
    src/HardwareDBus.cpp
    src/HardwareDBus.h
)

target_link_libraries(lenovo-control-service
    PRIVATE Qt6::Core Qt6::DBus
)

install(TARGETS lenovo-control-service
    RUNTIME DESTINATION bin
)

install(TARGETS lenovo-control-native
    RUNTIME DESTINATION bin
)

install(FILES
    assets/org.lenovocontrol.LenovoControl.desktop
    DESTINATION share/applications
)

# Optional KDE Plasma 6 integration
option(LENOVO_CONTROL_PLASMA
    "Install optional KDE Plasma 6 widget"
    OFF
)

if(LENOVO_CONTROL_PLASMA)
    install(
        DIRECTORY plasma/org.lenovo.control/
        DESTINATION share/plasma/plasmoids/org.lenovo.control
        FILES_MATCHING
        PATTERN "*.qml"
        PATTERN "*.json"
        PATTERN "*.js"
        PATTERN "*.svg"
        PATTERN "*.png"
    )
endif()

# Native D-Bus bridge for the KDE Plasma widget
qt_add_library(lenovo-control-qmlbridge SHARED)

qt_add_qml_module(lenovo-control-qmlbridge
    URI LenovoControl.Backend
    VERSION 1.0
    OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/LenovoControl/Backend"
    SOURCES
        src/qmlbridge/HardwareBridge.h
        src/qmlbridge/HardwareBridge.cpp
)

target_include_directories(lenovo-control-qmlbridge
    PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/src/qmlbridge"
)

target_link_libraries(lenovo-control-qmlbridge
    PRIVATE
        Qt6::Core
        Qt6::Qml
        Qt6::DBus
)

# Install both the QML plugin and its backing library.
install(TARGETS
    lenovo-control-qmlbridge
    lenovo-control-qmlbridgeplugin
    LIBRARY DESTINATION lib/qt6/qml/LenovoControl/Backend
)

# Install the QML module metadata.
install(FILES
    "${CMAKE_CURRENT_BINARY_DIR}/LenovoControl/Backend/qmldir"
    "${CMAKE_CURRENT_BINARY_DIR}/LenovoControl/Backend/lenovo-control-qmlbridge.qmltypes"
    DESTINATION lib/qt6/qml/LenovoControl/Backend
)

# Portable systemd user backend service
install(FILES
    packaging/systemd/lenovo-control-backend.service
    DESTINATION lib/systemd/user
)

# Lenovo Control v0.5.0 — hardware capability detection tests
include(CTest)

if(BUILD_TESTING)
    add_executable(lenovo-control-capabilities-test
        src/hardware/HardwareCapabilities.cpp
        src/hardware/capabilities_test.cpp
    )

    target_link_libraries(lenovo-control-capabilities-test
        PRIVATE Qt6::Core
    )

    add_test(
        NAME hardware-capabilities
        COMMAND lenovo-control-capabilities-test
    )
endif()
