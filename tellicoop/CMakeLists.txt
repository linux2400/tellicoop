project( tellicoop )
find_package( KDE4 REQUIRED )
find_package(LibXml2 REQUIRED)
find_package(LibXslt REQUIRED)

include(KDE4Defaults)
include(FindQImageBlitz)
set(TELLICO_DATA_INSTALL_DIR ${DATA_INSTALL_DIR}/tellico)

include_directories( ../src ../src/core ../src/3rdparty ${KDE4_INCLUDES} ${CMAKE_CURRENT_BINARY_DIR} ${QIMAGEBLITZ_INCLUDES} ${LIBXML2_INCLUDE_DIR})

add_subdirectory(../src/core "${CMAKE_CURRENT_BINARY_DIR}/core") 
add_subdirectory(../src/images "${CMAKE_CURRENT_BINARY_DIR}/images")
add_subdirectory(../src/translators "${CMAKE_CURRENT_BINARY_DIR}/translators")
add_subdirectory(../src/models "${CMAKE_CURRENT_BINARY_DIR}/models")
add_subdirectory(../src/utils "${CMAKE_CURRENT_BINARY_DIR}/utils")
add_subdirectory(../src/collections "${CMAKE_CURRENT_BINARY_DIR}/collections")
add_subdirectory(../src/gui "${CMAKE_CURRENT_BINARY_DIR}/gui")
add_subdirectory(../src/3rdparty "${CMAKE_CURRENT_BINARY_DIR}/3rdparty")
add_subdirectory(../src/newstuff "${CMAKE_CURRENT_BINARY_DIR}/newstuff")
add_subdirectory(../src/commands "${CMAKE_CURRENT_BINARY_DIR}/commands")
add_subdirectory(../src/fetch "${CMAKE_CURRENT_BINARY_DIR}/fetch")
add_subdirectory(../src/cite "${CMAKE_CURRENT_BINARY_DIR}/cite")

include(CheckTypeSize)
check_type_size("unsigned char" SIZEOF_UNSIGNED_CHAR)
check_type_size("unsigned short" SIZEOF_UNSIGNED_SHORT)
check_type_size("unsigned long" SIZEOF_UNSIGNED_LONG)
check_type_size("unsigned int" SIZEOF_UNSIGNED_INT)
check_type_size("short" SIZEOF_SHORT)
check_type_size("long" SIZEOF_LONG)
check_type_size("int" SIZEOF_INT)

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/config.h.cmake ${CMAKE_CURRENT_BINARY_DIR}/config.h)

set( tellicoop_sources main.cpp ../src/bibtexkeydialog.cpp         ../src/detailedlistview.cpp  ../src/entryupdatejob.cpp       ../src/filter.cpp        ../src/progressmanager.cpp
../src/borrower.cpp                ../src/document.cpp          ../src/entryupdater.cpp         ../src/filterdialog.cpp  ../src/reportdialog.cpp
../src/borrowerdialog.cpp          ../src/entrycomparison.cpp   ../src/entryview.cpp            ../src/filterview.cpp    ../src/tellico_kernel.cpp
../src/collection.cpp              ../src/entry.cpp             ../src/exportdialog.cpp         ../src/groupview.cpp     ../src/tellico_utils.cpp
../src/collectionfactory.cpp       ../src/entryeditdialog.cpp   ../src/fetchdialog.cpp          ../src/importdialog.cpp  ../src/viewstack.cpp
../src/collectionfieldsdialog.cpp  ../src/entrygroup.cpp        ../src/fetcherconfigdialog.cpp  ../src/loandialog.cpp
../src/configdialog.cpp            ../src/entryiconview.cpp     ../src/fieldcompletion.cpp      ../src/loanview.cpp
../src/controller.cpp              ../src/entrymatchdialog.cpp  ../src/field.cpp                
../src/derivedvalue.cpp            ../src/entrymerger.cpp       ../src/fieldformat.cpp          ../src/mainwindow.cpp)

kde4_add_kcfg_files(tellicoop_sources ../src/core/tellico_config.kcfgc)

kde4_add_executable( tellicoop ${tellicoop_sources} )

target_link_libraries( tellicoop tellicomodels utils cite collections fetch gui core translators images newstuff commands     ${QT_AND_KDECORE_LIBS} ${KDE4_KDEUI_LIBS}     ${KDE4_KIO_LIBS}    ${KDE4_SOLID_LIBS} ${KDE4_KPARTS_LIBS}   ${KDE4_KNEWSTUFF2_LIBS}  ${KDE4_KHTML_LIBS} ${QIMAGEBLITZ_LIBS} ${LIBXML2_LIBRARIES} ${LIBXSLT_LIBRARIES} btparse-tellico csv-tellico pilotdb-tellico
)

INSTALL( TARGETS tellicoop ${INSTALL_TARGETS_DEFAULT_ARGS} )

