
########### next target ###############

SET(collections_STAT_SRCS
   bibtexcollection.cpp
   boardgamecollection.cpp
   bookcollection.cpp
   cardcollection.cpp
   coincollection.cpp
   collectioninitializer.cpp
   comicbookcollection.cpp
   filecatalog.cpp
   gamecollection.cpp
   musiccollection.cpp
   stampcollection.cpp
   videocollection.cpp
   winecollection.cpp
   )

KDE4_ADD_LIBRARY(collections STATIC ${collections_STAT_SRCS})
TARGET_LINK_LIBRARIES(collections translators)
