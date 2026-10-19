cmake_minimum_required(VERSION 3.21)

project(lenovo-control VERSION 0.4.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 6.4 REQUIRED COMPONENTS Quick)

qt_standard_project_setup(REQUIRES 6.4)

qt_add_executable(lenovo-control-native
    src/main.cpp
    src/HardwareController.cpp
    src/HardwareController.h
)

qt_add_qml_module(lenovo-control-native
    URI LenovoControl
    VERSION 1.0
    QML_FILES
        qml/Main.qml
)

target_link_libraries(lenovo-control-native
    PRIVATE Qt6::Quick
)

install(TARGETS lenovo-control-native
    RUNTIME DESTINATION bin
)
