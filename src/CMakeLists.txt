ADD_SUBDIRECTORY( core )
ADD_SUBDIRECTORY( gui )
ADD_SUBDIRECTORY( collections )
ADD_SUBDIRECTORY( translators )
ADD_SUBDIRECTORY( fetch )
ADD_SUBDIRECTORY( commands )
ADD_SUBDIRECTORY( cite )
ADD_SUBDIRECTORY( newstuff )
ADD_SUBDIRECTORY( models )
ADD_SUBDIRECTORY( 3rdparty )
ADD_SUBDIRECTORY( tests )

IF( ENABLE_WEBCAM )
  ADD_SUBDIRECTORY( barcode )
ENDIF( ENABLE_WEBCAM )

########### next target ###############

SET(tellico_SRCS
   borrower.cpp
   borrowerdialog.cpp
   calendarhandler.cpp
   collection.cpp
   collectionfactory.cpp
   collectionfieldsdialog.cpp
   configdialog.cpp
   controller.cpp
   detailedlistview.cpp
   document.cpp
   entry.cpp
   entryeditdialog.cpp
   entryiconview.cpp
   entrycomparison.cpp
   entrymerger.cpp
   entryupdater.cpp
   entryview.cpp
   exportdialog.cpp
   fetchdialog.cpp
   fetcherconfigdialog.cpp
   field.cpp
   fieldcompletion.cpp
   fieldformat.cpp
   filehandler.cpp
   filter.cpp
   filterdialog.cpp
   filterview.cpp
   groupiterator.cpp
   groupview.cpp
   image.cpp
   imagefactory.cpp
   importdialog.cpp
   isbnvalidator.cpp
   iso5426converter.cpp
   iso6937converter.cpp
   lccnvalidator.cpp
   listviewcomparison.cpp
   loandialog.cpp
   loanview.cpp
   main.cpp
   mainwindow.cpp
   progressmanager.cpp
   reportdialog.cpp
   statusbar.cpp
   tellico_kernel.cpp
   tellico_strings.cpp
   tellico_utils.cpp
   upcvalidator.cpp
   viewstack.cpp
   xmphandler.cpp
   )

KDE4_ADD_EXECUTABLE(tellico ${tellico_SRCS})

TARGET_LINK_LIBRARIES(tellico
    core
    cite
    fetch
    collections
    translators
    gui
    commands
    models
    newstuff
    pilotdb
    rtf2html
    btparse
    csv
    )

IF( ENABLE_WEBCAM )
  TARGET_LINK_LIBRARIES(tellico barcode )
ENDIF( ENABLE_WEBCAM )

TARGET_LINK_LIBRARIES(tellico
    ${QT_AND_KDECORE_LIBS}
    ${KDE4_KDEUI_LIBS}
    ${KDE4_KABC_LIBRARY}
    ${KDE4_KIO_LIBS}
    ${KDE4_KHTML_LIBS}
    ${KDE4_KABC_LIBS}
    ${KDE4_KNEWSTUFF2_LIBS}
    ${QIMAGEBLITZ_LIBRARIES}
    ${TAGLIB_LIBRARIES}
    ${LIBXML2_LIBRARIES}
    ${LIBXSLT_LIBRARIES}
    ${LIBEXSLT_LIBRARIES}
    ${POPPLER_QT4_LIBRARIES}
    ${EXEMPI_LIBRARIES}
    ${YAZ_LIBRARIES}
    )

IF( KDEPIMLIBS_FOUND )
TARGET_LINK_LIBRARIES(tellico ${KDE4_KCAL_LIBRARY})
ENDIF( KDEPIMLIBS_FOUND )

IF( KDEMULTIMEDIA_FOUND )
TARGET_LINK_LIBRARIES(tellico ${KCDDB_LIBRARY})
ENDIF( KDEMULTIMEDIA_FOUND)

#TARGET_LINK_LIBRARIES(tellico ${QT_QT3SUPPORT_LIBRARY} )

INSTALL( TARGETS tellico ${INSTALL_TARGETS_DEFAULT_ARGS} )

########### install files ###############

INSTALL(FILES   tellicorc DESTINATION ${CONFIG_INSTALL_DIR} )
INSTALL(FILES   tellicoui.rc DESTINATION ${TELLICO_DATA_INSTALL_DIR} )