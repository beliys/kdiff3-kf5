cmake_minimum_required(VERSION 2.8.12 FATAL_ERROR)
project(kdiff3)

set(QT_MIN_VERSION "5.4.2")
set(KF5_MIN_VERSION "5.16.0")

find_package(ECM 1.7.0 REQUIRED NO_MODULE)

set(
    CMAKE_MODULE_PATH
    ${CMAKE_MODULE_PATH}
    /usr/share/apps/cmake/modules/
    ${ECM_MODULE_PATH}
    ${ECM_KDE_MODULE_DIR}
    ${CMAKE_SOURCE_DIR}/cmake/modules
)

include(KDEInstallDirs)
include(KDECompilerSettings NO_POLICY_SCOPE)
include(KDECMakeSettings)
include(ECMInstallIcons)
include(ECMAddAppIcon)
# include(GenerateExportHeader)

find_package(
    Qt5 ${QT_MIN_VERSION}
    CONFIG
    REQUIRED
    Core
    Widgets
    PrintSupport
)

find_package(
    KF5 ${KF5_MIN_VERSION}
    REQUIRED
    CoreAddons
    Crash
    I18n
    Parts
    WidgetsAddons
    IconThemes
)

add_subdirectory(src)
# add_subdirectory(po)
# add_subdirectory(doc)
add_subdirectory(icons)
add_subdirectory(kdiff3fileitemactionplugin)
