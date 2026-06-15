cmake_minimum_required(VERSION 3.16)

project(DMX VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt6 REQUIRED COMPONENTS Core Gui Qml Quick Network Test)

qt_standard_project_setup(REQUIRES 6.8)

# ========================================================
# 📱 PROGRAMME 1 : L'APPLICATION TABLETTE
# ========================================================
qt_add_executable(appDMX
    main.cpp
    networkmanager.cpp
    networkmanager.h
)

# On inclut tous les fichiers de l'interface et la logique JS ici
qt_add_qml_module(appDMX
    URI DMX
    VERSION 1.0
    QML_FILES
        Main.qml
        PageLogin.qml
        PageScenes.qml
        PageControle.qml
        dmxLogic.js
)

set_target_properties(appDMX PROPERTIES
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

target_link_libraries(appDMX PRIVATE Qt6::Core Qt6::Gui Qt6::Qml Qt6::Quick Qt6::Network)

include(GNUInstallDirs)
install(TARGETS appDMX
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

set_property(TARGET appDMX APPEND PROPERTY QT_ANDROID_PACKAGE_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/android)

# ========================================================
# 🧪 PROGRAMME 2 : LE TEST AUTOMATISÉ (Uniquement sur PC)
# ========================================================
if (NOT ANDROID)
    qt_add_executable(testDMX
        testnetworkmanager.cpp
        networkmanager.cpp
        networkmanager.h
    )

    target_link_libraries(testDMX PRIVATE Qt6::Core Qt6::Network Qt6::Test)
endif()
