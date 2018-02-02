kcoreaddons_add_plugin(kdiff3fileitemaction
                       SOURCES kdiff3fileitemaction.cpp
                       INSTALL_NAMESPACE "kf5/kfileitemaction")
                       
target_link_libraries(kdiff3fileitemaction
                      ${LibKDiff3_LIBRARIES}
                      KF5::I18n
                      KF5::KIOWidgets)
