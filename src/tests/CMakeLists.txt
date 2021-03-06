SET( EXECUTABLE_OUTPUT_PATH ${CMAKE_CURRENT_BINARY_DIR} )

# formattest includes fieldformat that include core/tellico_config.h
# which is generated in the build directory
INCLUDE_DIRECTORIES( ${Tellico_BINARY_DIR}/src )

KDE4_ADD_UNIT_TEST(entitytest NOGUI entitytest.cpp ../tellico_utils.cpp)
TARGET_LINK_LIBRARIES(entitytest ${KDE4_KDECORE_LIBS}  ${KDE4_KDEUI_LIBS} ${QT_QTTEST_LIBRARY} )

KDE4_ADD_UNIT_TEST(cuecattest NOGUI cuecattest.cpp)
TARGET_LINK_LIBRARIES(cuecattest utils ${KDE4_KDECORE_LIBS} ${KDE4_KDEUI_LIBS} ${QT_QTTEST_LIBRARY} )

KDE4_ADD_UNIT_TEST(isbntest NOGUI isbntest.cpp)
TARGET_LINK_LIBRARIES(isbntest utils ${QT_AND_KDECORE_LIBS} ${KDE4_KDEUI_LIBS} ${QT_QTTEST_LIBRARY} )

KDE4_ADD_UNIT_TEST(lccntest NOGUI lccntest.cpp)
TARGET_LINK_LIBRARIES(lccntest utils ${QT_AND_KDECORE_LIBS} ${KDE4_KDEUI_LIBS} ${QT_QTTEST_LIBRARY} )

KDE4_ADD_UNIT_TEST(lcctest NOGUI lcctest.cpp ../field.cpp ../fieldformat.cpp ../tellico_utils.cpp)
TARGET_LINK_LIBRARIES(lcctest utils core ${QT_AND_KDECORE_LIBS} ${KDE4_KDEUI_LIBS} ${QT_QTTEST_LIBRARY} )

KDE4_ADD_UNIT_TEST(formattest NOGUI formattest.cpp ../fieldformat.cpp)
TARGET_LINK_LIBRARIES(formattest core ${QT_AND_KDECORE_LIBS} ${KDE4_KDEUI_LIBS} ${QT_QTTEST_LIBRARY} )

KDE4_ADD_UNIT_TEST(fieldtest NOGUI fieldtest.cpp ../field.cpp ../fieldformat.cpp ../tellico_utils.cpp)
TARGET_LINK_LIBRARIES(fieldtest core ${QT_AND_KDECORE_LIBS} ${KDE4_KDEUI_LIBS} ${QT_QTTEST_LIBRARY} )

KDE4_ADD_UNIT_TEST(comparisontest NOGUI comparisontest.cpp ../field.cpp ../fieldformat.cpp ../tellico_utils.cpp)
TARGET_LINK_LIBRARIES(comparisontest utils core ${QT_AND_KDECORE_LIBS} ${KDE4_KDEUI_LIBS} ${QT_QTTEST_LIBRARY} )

KDE4_ADD_UNIT_TEST(imagetest NOGUI imagetest.cpp ../tellico_utils.cpp)
TARGET_LINK_LIBRARIES(imagetest images core ${QT_AND_KDECORE_LIBS} ${KDE4_KDEUI_LIBS} ${QT_QTTEST_LIBRARY} )

SET(tellicotest_SRCS
   ../collection.cpp
   ../entry.cpp
   ../entrygroup.cpp
   ../entrycomparison.cpp
   ../field.cpp
   ../fieldformat.cpp
   ../filter.cpp
   ../borrower.cpp
   ../collectionfactory.cpp
   ../tellico_utils.cpp
   ../derivedvalue.cpp
   ../document.cpp
   ../progressmanager.cpp
   )

KDE4_ADD_LIBRARY(tellicotest STATIC ${tellicotest_SRCS})

ADD_DEPENDENCIES(tellicotest tellico_config)

set(TELLICO_TEST_LIBS
   ${QT_AND_KDECORE_LIBS}
   ${QT_QTTEST_LIBRARY}
   ${KDE4_KDEUI_LIBS}
   ${KDE4_KIO_LIBS}
   tellicotest
   collections
   translators
   utils
   core
   images
   core
   gui
   tellicotest
   )

# for some of these, the libraries seem to require being repeated so
# that the order of linking works...no idea why...
KDE4_ADD_UNIT_TEST(collectiontest NOGUI collectiontest.cpp)
TARGET_LINK_LIBRARIES(collectiontest ${TELLICO_TEST_LIBS})

KDE4_ADD_UNIT_TEST(filtertest NOGUI filtertest.cpp)
TARGET_LINK_LIBRARIES(filtertest ${TELLICO_TEST_LIBS})

KDE4_ADD_UNIT_TEST(adstest NOGUI adstest.cpp)
TARGET_LINK_LIBRARIES(adstest ${TELLICO_TEST_LIBS})

KDE4_ADD_UNIT_TEST(alexandriatest NOGUI alexandriatest.cpp ../progressmanager.cpp)
TARGET_LINK_LIBRARIES(alexandriatest ${TELLICO_TEST_LIBS})

KDE4_ADD_UNIT_TEST(amctest NOGUI amctest.cpp)
TARGET_LINK_LIBRARIES(amctest ${TELLICO_TEST_LIBS})

KDE4_ADD_UNIT_TEST(bibtextest NOGUI bibtextest.cpp)
TARGET_LINK_LIBRARIES(bibtextest ${TELLICO_TEST_LIBS})

KDE4_ADD_UNIT_TEST(bibtexmltest NOGUI bibtexmltest.cpp)
TARGET_LINK_LIBRARIES(bibtexmltest ${TELLICO_TEST_LIBS})

KDE4_ADD_UNIT_TEST(ciwtest NOGUI ciwtest.cpp)
TARGET_LINK_LIBRARIES(ciwtest ${TELLICO_TEST_LIBS})

KDE4_ADD_UNIT_TEST(csvtest NOGUI csvtest.cpp)
TARGET_LINK_LIBRARIES(csvtest ${TELLICO_CSV_LIBS} ${TELLICO_TEST_LIBS})

KDE4_ADD_UNIT_TEST(delicioustest NOGUI delicioustest.cpp)
TARGET_LINK_LIBRARIES(delicioustest ${TELLICO_TEST_LIBS})

KDE4_ADD_UNIT_TEST(gcstartest NOGUI gcstartest.cpp)
TARGET_LINK_LIBRARIES(gcstartest ${TELLICO_TEST_LIBS})

KDE4_ADD_UNIT_TEST(griffithtest NOGUI griffithtest.cpp)
TARGET_LINK_LIBRARIES(griffithtest ${TELLICO_TEST_LIBS})

KDE4_ADD_UNIT_TEST(referencertest NOGUI referencertest.cpp)
TARGET_LINK_LIBRARIES(referencertest ${TELLICO_TEST_LIBS})

KDE4_ADD_UNIT_TEST(ristest NOGUI ristest.cpp)
TARGET_LINK_LIBRARIES(ristest ${TELLICO_TEST_LIBS})

KDE4_ADD_UNIT_TEST(tellicoreadtest NOGUI tellicoreadtest.cpp)
TARGET_LINK_LIBRARIES(tellicoreadtest tellicomodels ${TELLICO_TEST_LIBS})

KDE4_ADD_UNIT_TEST(vinoxmltest NOGUI vinoxmltest.cpp)
TARGET_LINK_LIBRARIES(vinoxmltest ${TELLICO_TEST_LIBS})

KDE4_ADD_UNIT_TEST(entryupdatejobtest NOGUI entryupdatejobtest.cpp ../entryupdatejob.cpp)
TARGET_LINK_LIBRARIES(entryupdatejobtest fetch translators ${TELLICO_TEST_LIBS})

# fetcher tests from here down
IF(BUILD_FETCHER_TESTS)

KDE4_ADD_UNIT_TEST(allocinefetchertest NOGUI allocinefetchertest.cpp abstractfetchertest.cpp)
TARGET_LINK_LIBRARIES(allocinefetchertest fetch ${TELLICO_TEST_LIBS})

KDE4_ADD_UNIT_TEST(amazonfetchertest NOGUI amazonfetchertest.cpp abstractfetchertest.cpp)
TARGET_LINK_LIBRARIES(amazonfetchertest fetch ${TELLICO_TEST_LIBS})

KDE4_ADD_UNIT_TEST(animenfofetchertest NOGUI animenfofetchertest.cpp abstractfetchertest.cpp)
TARGET_LINK_LIBRARIES(animenfofetchertest fetch ${TELLICO_TEST_LIBS})

KDE4_ADD_UNIT_TEST(arxivfetchertest NOGUI arxivfetchertest.cpp abstractfetchertest.cpp)
TARGET_LINK_LIBRARIES(arxivfetchertest fetch ${TELLICO_TEST_LIBS})

KDE4_ADD_UNIT_TEST(bibliosharefetchertest NOGUI bibliosharefetchertest.cpp abstractfetchertest.cpp)
TARGET_LINK_LIBRARIES(bibliosharefetchertest fetch ${TELLICO_TEST_LIBS})

KDE4_ADD_UNIT_TEST(boardgamegeekfetchertest NOGUI boardgamegeekfetchertest.cpp abstractfetchertest.cpp)
TARGET_LINK_LIBRARIES(boardgamegeekfetchertest fetch ${TELLICO_TEST_LIBS})

KDE4_ADD_UNIT_TEST(citebasefetchertest NOGUI citebasefetchertest.cpp abstractfetchertest.cpp)
TARGET_LINK_LIBRARIES(citebasefetchertest fetch ${TELLICO_TEST_LIBS})

KDE4_ADD_UNIT_TEST(darkhorsefetchertest NOGUI darkhorsefetchertest.cpp abstractfetchertest.cpp)
TARGET_LINK_LIBRARIES(darkhorsefetchertest fetch ${TELLICO_TEST_LIBS})

KDE4_ADD_UNIT_TEST(dblpfetchertest NOGUI dblpfetchertest.cpp abstractfetchertest.cpp)
TARGET_LINK_LIBRARIES(dblpfetchertest fetch ${TELLICO_TEST_LIBS})

KDE4_ADD_UNIT_TEST(discogsfetchertest NOGUI discogsfetchertest.cpp abstractfetchertest.cpp)
TARGET_LINK_LIBRARIES(discogsfetchertest fetch ${TELLICO_TEST_LIBS})

KDE4_ADD_UNIT_TEST(doubanfetchertest NOGUI doubanfetchertest.cpp abstractfetchertest.cpp)
TARGET_LINK_LIBRARIES(doubanfetchertest fetch ${TELLICO_TEST_LIBS})

KDE4_ADD_UNIT_TEST(dvdfrfetchertest NOGUI dvdfrfetchertest.cpp abstractfetchertest.cpp)
TARGET_LINK_LIBRARIES(dvdfrfetchertest fetch ${TELLICO_TEST_LIBS})

KDE4_ADD_UNIT_TEST(entrezfetchertest NOGUI entrezfetchertest.cpp abstractfetchertest.cpp)
TARGET_LINK_LIBRARIES(entrezfetchertest fetch ${TELLICO_TEST_LIBS})

IF( QJSON_FOUND )
    KDE4_ADD_UNIT_TEST(freebasefetchertest NOGUI freebasefetchertest.cpp abstractfetchertest.cpp)
    TARGET_LINK_LIBRARIES(freebasefetchertest fetch ${TELLICO_TEST_LIBS})

    KDE4_ADD_UNIT_TEST(filmasterfetchertest NOGUI filmasterfetchertest.cpp abstractfetchertest.cpp)
    TARGET_LINK_LIBRARIES(filmasterfetchertest fetch ${TELLICO_TEST_LIBS})

    KDE4_ADD_UNIT_TEST(masfetchertest NOGUI masfetchertest.cpp abstractfetchertest.cpp)
    TARGET_LINK_LIBRARIES(masfetchertest fetch ${TELLICO_TEST_LIBS})

    KDE4_ADD_UNIT_TEST(googlebookfetchertest NOGUI googlebookfetchertest.cpp abstractfetchertest.cpp)
    TARGET_LINK_LIBRARIES(googlebookfetchertest fetch ${TELLICO_TEST_LIBS})

ENDIF( QJSON_FOUND )

# running gcstar in the fetcher is really unreliable
#KDE4_ADD_UNIT_TEST(gcstarfetchertest NOGUI gcstarfetchertest.cpp)
#TARGET_LINK_LIBRARIES(gcstarfetchertest fetch ${TELLICO_TEST_LIBS})

KDE4_ADD_UNIT_TEST(giantbombfetchertest NOGUI giantbombfetchertest.cpp abstractfetchertest.cpp)
TARGET_LINK_LIBRARIES(giantbombfetchertest fetch ${TELLICO_TEST_LIBS})

KDE4_ADD_UNIT_TEST(googlescholarfetchertest NOGUI googlescholarfetchertest.cpp abstractfetchertest.cpp)
TARGET_LINK_LIBRARIES(googlescholarfetchertest fetch ${TELLICO_TEST_LIBS})

KDE4_ADD_UNIT_TEST(ibsfetchertest NOGUI ibsfetchertest.cpp abstractfetchertest.cpp)
TARGET_LINK_LIBRARIES(ibsfetchertest fetch ${TELLICO_TEST_LIBS})

KDE4_ADD_UNIT_TEST(imdbfetchertest NOGUI imdbfetchertest.cpp abstractfetchertest.cpp)
TARGET_LINK_LIBRARIES(imdbfetchertest fetch ${TELLICO_TEST_LIBS})

KDE4_ADD_UNIT_TEST(isbndbfetchertest NOGUI isbndbfetchertest.cpp abstractfetchertest.cpp)
TARGET_LINK_LIBRARIES(isbndbfetchertest fetch ${TELLICO_TEST_LIBS})

KDE4_ADD_UNIT_TEST(moviemeterfetchertest NOGUI moviemeterfetchertest.cpp abstractfetchertest.cpp)
TARGET_LINK_LIBRARIES(moviemeterfetchertest fetch ${TELLICO_TEST_LIBS})

KDE4_ADD_UNIT_TEST(mrlookupfetchertest NOGUI mrlookupfetchertest.cpp abstractfetchertest.cpp)
TARGET_LINK_LIBRARIES(mrlookupfetchertest fetch ${TELLICO_TEST_LIBS})

KDE4_ADD_UNIT_TEST(musicbrainzfetchertest NOGUI musicbrainzfetchertest.cpp abstractfetchertest.cpp)
TARGET_LINK_LIBRARIES(musicbrainzfetchertest fetch ${TELLICO_TEST_LIBS})

IF( QJSON_FOUND )
    KDE4_ADD_UNIT_TEST(openlibraryfetchertest NOGUI openlibraryfetchertest.cpp abstractfetchertest.cpp)
    TARGET_LINK_LIBRARIES(openlibraryfetchertest fetch ${TELLICO_TEST_LIBS})
ENDIF( QJSON_FOUND )

KDE4_ADD_UNIT_TEST(springerfetchertest NOGUI springerfetchertest.cpp abstractfetchertest.cpp)
TARGET_LINK_LIBRARIES(springerfetchertest fetch ${TELLICO_TEST_LIBS})

KDE4_ADD_UNIT_TEST(srufetchertest NOGUI srufetchertest.cpp abstractfetchertest.cpp)
TARGET_LINK_LIBRARIES(srufetchertest fetch ${TELLICO_TEST_LIBS})

KDE4_ADD_UNIT_TEST(thegamesdbfetchertest NOGUI thegamesdbfetchertest.cpp abstractfetchertest.cpp)
TARGET_LINK_LIBRARIES(thegamesdbfetchertest fetch ${TELLICO_TEST_LIBS})

IF( QJSON_FOUND )
    KDE4_ADD_UNIT_TEST(themoviedbfetchertest NOGUI themoviedbfetchertest.cpp abstractfetchertest.cpp)
    TARGET_LINK_LIBRARIES(themoviedbfetchertest fetch ${TELLICO_TEST_LIBS})
ENDIF( QJSON_FOUND )

KDE4_ADD_UNIT_TEST(vndbfetchertest NOGUI vndbfetchertest.cpp abstractfetchertest.cpp)
TARGET_LINK_LIBRARIES(vndbfetchertest fetch ${TELLICO_TEST_LIBS})

KDE4_ADD_UNIT_TEST(winecomfetchertest NOGUI winecomfetchertest.cpp abstractfetchertest.cpp)
TARGET_LINK_LIBRARIES(winecomfetchertest fetch ${TELLICO_TEST_LIBS})

IF( YAZ_FOUND )
    KDE4_ADD_UNIT_TEST(z3950fetchertest NOGUI z3950fetchertest.cpp abstractfetchertest.cpp)
    TARGET_LINK_LIBRARIES(z3950fetchertest fetch ${TELLICO_TEST_LIBS})
ENDIF( YAZ_FOUND )

ENDIF(BUILD_FETCHER_TESTS)
