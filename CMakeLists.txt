project(kopete-scripting)

find_package(KDE4 REQUIRED)
include(KDE4Defaults)
include(MacroLibrary)
include(MacroOptionalAddSubdirectory)
find_package(Kopete)

add_definitions (${QT_DEFINITIONS} ${KDE4_DEFINITIONS})
include_directories (${CMAKE_SOURCE_DIR} ${CMAKE_BINARY_DIR} ${KDE4_INCLUDES})
include_directories(${KOPETE_INCLUDES})

add_subdirectory( scripts ) 

########### next target ###############

set(kopete_scripting_SRCS scriptingplugin.cpp scriptinginterface.cpp)
kde4_add_plugin(kopete_scripting ${kopete_scripting_SRCS})
target_link_libraries(kopete_scripting ${KDE4_KDECORE_LIBS} kopete ${KDE4_KROSSUI_LIBS})
install(TARGETS kopete_scripting DESTINATION ${PLUGIN_INSTALL_DIR})
install(FILES kopete_scripting.desktop DESTINATION ${SERVICES_INSTALL_DIR})
install(FILES scriptingui.rc scriptingchatui.rc DESTINATION ${DATA_INSTALL_DIR}/kopete_scripting)

########### next target ###############

set(kcm_kopete_scripting_SRCS scriptingpreferences.cpp)
#kde4_add_ui_files(kcm_kopete_scripting_SRCS scriptingpreferencesbase.ui )
kde4_add_plugin(kcm_kopete_scripting ${kcm_kopete_scripting_SRCS})
target_link_libraries(kcm_kopete_scripting ${KDE4_KUTILS_LIBS} ${KDE4_KNOTIFYCONFIG_LIBS} ${KDE4_KROSSUI_LIBS} kopete)
install(TARGETS kcm_kopete_scripting DESTINATION ${PLUGIN_INSTALL_DIR})
install(FILES kopete_scripting_config.desktop DESTINATION ${SERVICES_INSTALL_DIR}/kconfiguredialog)
