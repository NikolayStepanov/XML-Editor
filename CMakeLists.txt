cmake_minimum_required(VERSION 3.5)

project(EditorXML LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt5 COMPONENTS Widgets LinguistTools Xml REQUIRED)



set ( SOURCES main.cpp mainwindow.cpp xmlwindow.cpp treemodel.cpp treeitem.cpp)

set ( HEADERS mainwindow.h xmlwindow.h treemodel.h treeitem.h)

set ( UIS mainwindow.ui xmlwindow.ui)

set (RES resource.qrc)


# Translation
set(FILES_TO_TRANSLATE ${SOURCES} ${HEADERS} ${UIS} )
set(TS_FILES translations/EditorXML_en_US.ts)

option(
UPDATE_TRANSLATIONS
"Update source translation (files translations/*.ts)."
ON)

if(UPDATE_TRANSLATIONS)
    message(
        STATUS "UPDATE_TRANSLATIONS option is set.")
    qt5_create_translation(
        QM_FILES ${FILES_TO_TRANSLATE} ${TS_FILES})
else()
    qt5_add_translation(
        QM_FILES ${TS_FILES})
endif()

add_custom_target(translations DEPENDS ${QM_FILES})
set_property(DIRECTORY PROPERTY CLEAN_NO_CUSTOM TRUE)

add_executable(EditorXML  ${SOURCES} ${HEADERS} ${UIS} ${RES})

target_link_libraries(EditorXML PRIVATE Qt5::Widgets Qt5::Xml)

# Installation

install(TARGETS EditorXML RUNTIME DESTINATION bin)
install(FILES ${QM_FILES} DESTINATION translations)

get_property(LIB_QT5CORE_PATH TARGET Qt5::Core PROPERTY IMPORTED_LOCATION_RELEASE)

get_filename_component(QT_LIBRARY_DIR "${LIB_QT5CORE_PATH}" DIRECTORY)
set(EXE_PATH bin/EditorXML${CMAKE_EXECUTABLE_SUFFIX})

include(DeployQt4)

install_qt4_executable(
"${EXE_PATH}"
""
""
"${QT_LIBRARY_DIR}")
