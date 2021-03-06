ADD_SUBDIRECTORY( scripts )

########### next target ###############

SET(fetch_STAT_SRCS
   allocinefetcher.cpp
   amazonfetcher.cpp
   amazonrequest.cpp
   animenfofetcher.cpp
   arxivfetcher.cpp
   beyazperdefetcher.cpp
   bibliosharefetcher.cpp
   bibsonomyfetcher.cpp
   boardgamegeekfetcher.cpp
   citebasefetcher.cpp
   configwidget.cpp
   crossreffetcher.cpp
   dblpfetcher.cpp
   discogsfetcher.cpp
   doubanfetcher.cpp
   dvdfrfetcher.cpp
   entrezfetcher.cpp
   execexternalfetcher.cpp
   fetcher.cpp
   fetcherinitializer.cpp
   fetcherjob.cpp
   fetchmanager.cpp
   fetchresult.cpp
   filmasterfetcher.cpp
   filmstartsfetcher.cpp
   freebasefetcher.cpp
   gcstarpluginfetcher.cpp
   gcstarthread.cpp
   giantbombfetcher.cpp
   googlebookfetcher.cpp
   googlescholarfetcher.cpp
   hathitrustfetcher.cpp
   hmac_sha2.c
   ibsfetcher.cpp
   imdbfetcher.cpp
   isbndbfetcher.cpp
   masfetcher.cpp
   messagehandler.cpp
   moviemeterfetcher.cpp
   mrlookupfetcher.cpp
   multifetcher.cpp
   musicbrainzfetcher.cpp
   openlibraryfetcher.cpp
   screenrushfetcher.cpp
   sensacinefetcher.cpp
   sha2.c
   springerfetcher.cpp
   srufetcher.cpp
   thegamesdbfetcher.cpp
   themoviedbfetcher.cpp
   vndbfetcher.cpp
   winecomfetcher.cpp
   xmlfetcher.cpp
   yahoofetcher.cpp
   z3950connection.cpp
   z3950fetcher.cpp
   )

KDE4_ADD_LIBRARY(fetch STATIC ${fetch_STAT_SRCS})

TARGET_LINK_LIBRARIES(fetch
    newstuff
    ${TELLICO_BTPARSE_LIBS}
    )

IF( YAZ_FOUND )
  TARGET_LINK_LIBRARIES(fetch ${YAZ_LIBRARIES})
ENDIF( YAZ_FOUND )

IF( QJSON_FOUND )
  TARGET_LINK_LIBRARIES(fetch ${QJSON_LIBRARIES} ${qjson_LIBRARIES})
ENDIF( QJSON_FOUND )

########### install files ###############

INSTALL(FILES   z3950-servers.cfg DESTINATION ${TELLICO_DATA_INSTALL_DIR} )
