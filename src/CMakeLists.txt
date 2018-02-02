########### kdiff3 KPart ###############

set(kdiff3part_PART_SRCS 
   kdiff3_part.cpp 
   kdiff3.cpp 
   directorymergewindow.cpp 
   merger.cpp 
   pdiff.cpp 
   difftextwindow.cpp 
   diff.cpp 
   optiondialog.cpp 
   mergeresultwindow.cpp 
   fileaccess.cpp 
   gnudiff_analyze.cpp 
   gnudiff_io.cpp 
   gnudiff_xmalloc.cpp 
   common.cpp 
   smalldialogs.cpp 
   progress.cpp )

########### kdiff3 executable ###############

set(kdiff3_SRCS 
   main.cpp 
   kdiff3_shell.cpp
   ${kdiff3part_PART_SRCS}
    )

add_executable(
    kdiff3
    ${kdiff3_SRCS}
)

target_link_libraries(
    kdiff3
    Qt5::Core
    Qt5::Widgets
    Qt5::PrintSupport
    KF5::CoreAddons
    KF5::Crash
    KF5::I18n
    KF5::Parts
    KF5::WidgetsAddons
    KF5::IconThemes
)

install(
    TARGETS
    kdiff3
    ${INSTALL_TARGETS_DEFAULT_ARGS}
)

########### install files ###############

install( FILES kdiff3_part.rc DESTINATION ${KXMLGUI_INSTALL_DIR}/kdiff3 )
install( FILES kdiff3_shell.rc DESTINATION ${KXMLGUI_INSTALL_DIR}/kdiff3 )
install( FILES kdiff3part.desktop DESTINATION ${KDE_INSTALL_KSERVICES5DIR} )
install( PROGRAMS org.kde.kdiff3.desktop DESTINATION ${XDG_APPS_INSTALL_DIR} )
install( FILES org.kde.kdiff3.appdata.xml DESTINATION ${SHARE_INSTALL_PREFIX}/appdata )
