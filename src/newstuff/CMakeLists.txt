
########### next target ###############

SET(newstuff_STAT_SRCS
   manager.cpp
   )

QT4_ADD_DBUS_ADAPTOR(newstuff_STAT_SRCS org.kde.tellico.newstuff.xml manager.h Tellico::NewStuff::Manager)
KDE4_ADD_LIBRARY(newstuff STATIC ${newstuff_STAT_SRCS})

INSTALL(FILES tellico-template.knsrc tellico-script.knsrc DESTINATION ${CONFIG_INSTALL_DIR})
