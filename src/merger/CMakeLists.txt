project( tellicoop )
find_package( KDE4 REQUIRED )
find_package(LibXml2 REQUIRED)
find_package(LibXslt REQUIRED)

include(KDE4Defaults)
include(FindQImageBlitz)
set(TELLICO_DATA_INSTALL_DIR ${DATA_INSTALL_DIR}/tellico)

include_directories( .. ../core ../3rdparty ${KDE4_INCLUDES} ${CMAKE_CURRENT_BINARY_DIR} ${QIMAGEBLITZ_INCLUDES} ${LIBXML2_INCLUDE_DIR})

add_subdirectory(../core "${CMAKE_CURRENT_BINARY_DIR}/core") 
add_subdirectory(../images "${CMAKE_CURRENT_BINARY_DIR}/images")
add_subdirectory(../translators "${CMAKE_CURRENT_BINARY_DIR}/translators")
add_subdirectory(../models "${CMAKE_CURRENT_BINARY_DIR}/models")
add_subdirectory(../utils "${CMAKE_CURRENT_BINARY_DIR}/utils")
add_subdirectory(../collections "${CMAKE_CURRENT_BINARY_DIR}/collections")
add_subdirectory(../gui "${CMAKE_CURRENT_BINARY_DIR}/gui")
add_subdirectory(../3rdparty "${CMAKE_CURRENT_BINARY_DIR}/3rdparty")
add_subdirectory(../newstuff "${CMAKE_CURRENT_BINARY_DIR}/newstuff")
add_subdirectory(../commands "${CMAKE_CURRENT_BINARY_DIR}/commands")
add_subdirectory(../fetch "${CMAKE_CURRENT_BINARY_DIR}/fetch")
add_subdirectory(../cite "${CMAKE_CURRENT_BINARY_DIR}/cite")

include(CheckTypeSize)
check_type_size("unsigned char" SIZEOF_UNSIGNED_CHAR)
check_type_size("unsigned short" SIZEOF_UNSIGNED_SHORT)
check_type_size("unsigned long" SIZEOF_UNSIGNED_LONG)
check_type_size("unsigned int" SIZEOF_UNSIGNED_INT)
check_type_size("short" SIZEOF_SHORT)
check_type_size("long" SIZEOF_LONG)
check_type_size("int" SIZEOF_INT)

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/config.h.cmake ${CMAKE_CURRENT_BINARY_DIR}/config.h)

set( tellicoop_sources merger.cpp ../bibtexkeydialog.cpp         ../detailedlistview.cpp  ../entryupdatejob.cpp       ../filter.cpp        ../progressmanager.cpp
../borrower.cpp                ../document.cpp          ../entryupdater.cpp         ../filterdialog.cpp  ../reportdialog.cpp
../borrowerdialog.cpp          ../entrycomparison.cpp   ../entryview.cpp            ../filterview.cpp    ../tellico_kernel.cpp
../collection.cpp              ../entry.cpp             ../exportdialog.cpp         ../groupview.cpp     ../tellico_utils.cpp
../collectionfactory.cpp       ../entryeditdialog.cpp   ../fetchdialog.cpp          ../importdialog.cpp  ../viewstack.cpp
../collectionfieldsdialog.cpp  ../entrygroup.cpp        ../fetcherconfigdialog.cpp  ../loandialog.cpp
../configdialog.cpp            ../entryiconview.cpp     ../fieldcompletion.cpp      ../loanview.cpp
../controller.cpp              ../entrymatchdialog.cpp  ../field.cpp                
../derivedvalue.cpp            ../entrymerger.cpp       ../fieldformat.cpp          ../mainwindow.cpp)

kde4_add_kcfg_files(tellicoop_sources ../core/tellico_config.kcfgc)

kde4_add_executable( tellicoop ${tellicoop_sources} )

target_link_libraries( tellicoop tellicomodels utils cite collections fetch gui core translators images newstuff commands     ${QT_AND_KDECORE_LIBS} ${KDE4_KDEUI_LIBS}     ${KDE4_KIO_LIBS}    ${KDE4_SOLID_LIBS} ${KDE4_KPARTS_LIBS}   ${KDE4_KNEWSTUFF2_LIBS}  ${KDE4_KHTML_LIBS} ${QIMAGEBLITZ_LIBS} ${LIBXML2_LIBRARIES} ${LIBXSLT_LIBRARIES} btparse-tellico csv-tellico pilotdb-tellico
)

INSTALL( TARGETS tellicoop ${INSTALL_TARGETS_DEFAULT_ARGS} )

