
########### next target ###############

SET(translators_STAT_SRCS
   adsimporter.cpp
   alexandriaexporter.cpp
   alexandriaimporter.cpp
   amcimporter.cpp
   audiofileimporter.cpp
   bibtexexporter.cpp
   bibtexhandler.cpp
   bibteximporter.cpp
   bibtexmlexporter.cpp
   bibtexmlimporter.cpp
   boardgamegeekimporter.cpp
   ciwimporter.cpp
   csvexporter.cpp
   csvimporter.cpp
   csvparser.cpp
   dataimporter.cpp
   deliciousimporter.cpp
   exporter.cpp
   filelistingimporter.cpp
   freedb_util.cpp
   freedbimporter.cpp
   gcstarexporter.cpp
   gcstarimporter.cpp
   goodreadsimporter.cpp
   griffithimporter.cpp
   grs1importer.cpp
   htmlexporter.cpp
   importer.cpp
   onixexporter.cpp
   pdfimporter.cpp
   pilotdbexporter.cpp
   referencerimporter.cpp
   risimporter.cpp
   tellico_xml.cpp
   tellicoimporter.cpp
   tellicoxmlexporter.cpp
   tellicoxmlhandler.cpp
   tellicozipexporter.cpp
   textimporter.cpp
   vinoxmlimporter.cpp
   xmlimporter.cpp
   xmlstatehandler.cpp
   xmphandler.cpp
   xsltexporter.cpp
   xslthandler.cpp
   xsltimporter.cpp
   )

KDE4_ADD_LIBRARY(translators STATIC ${translators_STAT_SRCS})
TARGET_LINK_LIBRARIES(translators
    collections
    tellicomodels
    ${TELLICO_BTPARSE_LIBS}
    ${TELLICO_CSV_LIBS}
    rtf2html-tellico
    )

TARGET_LINK_LIBRARIES(translators
    ${LIBXML2_LIBRARIES}
    ${LIBXSLT_LIBRARIES}
    ${LIBEXSLT_LIBRARIES}
    )

IF( NEPOMUK_FOUND )
  ADD_DEFINITIONS( -DDISABLE_NEPOMUK_LEGACY )
  TARGET_LINK_LIBRARIES(translators ${NEPOMUK_LIBRARIES})
ENDIF( NEPOMUK_FOUND)

IF( KCDDB_FOUND )
  TARGET_LINK_LIBRARIES(translators ${KCDDB_LIBRARIES})
ENDIF( KCDDB_FOUND)

IF( POPPLER_QT4_FOUND )
  TARGET_LINK_LIBRARIES(translators ${POPPLER_QT4_LIBRARIES})
ENDIF( POPPLER_QT4_FOUND )

IF( LIBEXEMPI_FOUND )
  TARGET_LINK_LIBRARIES(translators ${LIBEXEMPI_LIBRARIES})
ENDIF( LIBEXEMPI_FOUND )

IF( TAGLIB_FOUND )
  TARGET_LINK_LIBRARIES(translators ${TAGLIB_LIBRARIES})
ENDIF( TAGLIB_FOUND )

IF( DISCID_FOUND )
  TARGET_LINK_LIBRARIES(translators ${DISCID_LIBRARIES})
ENDIF( DISCID_FOUND )

ADD_DEPENDENCIES(translators tellico_config)

########### install files ###############

INSTALL(FILES   bibtex-translation.xml DESTINATION ${TELLICO_DATA_INSTALL_DIR} )
