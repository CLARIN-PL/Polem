cmake_minimum_required(VERSION 3.2)

project(polem)

set(CMAKE_CXX_STANDARD 11)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${PROJECT_SOURCE_DIR}/cpp/cmake/Modules/")

include(LibFindMacros)


find_package(Corpus2 REQUIRED)
include_directories(${Corpus2_INCLUDE_DIRS})
set(LIBS ${LIBS} ${Corpus2_LIBRARY})
#set(LIBS ${LIBS} "/usr/local/lib/libcorpus2.so")
set(LIBS ${LIBS} "/usr/local/lib/libwccl.so")
set(LIBS ${LIBS} "/usr/lib/x86_64-linux-gnu/libicui18n.so")

find_package(Morfeusz2 REQUIRED)
include_directories(${MORFEUSZ2_INCLUDE_DIR})
set(LIBS ${LIBS} ${MORFEUSZ2_LIBRARY})


find_package(Glib REQUIRED)
include_directories(${Glib_INCLUDE_DIRS})
set(LIBS ${LIBS} ${Glib_LIBRARIES})

find_package(Glibmm REQUIRED)
include_directories(${Glibmm_INCLUDE_DIRS})
set(LIBS ${LIBS} ${Glibmm_LIBRARIES})

find_package(ICU REQUIRED)
include_directories(${ICU_INCLUDE_DIRS})
set(LIBS ${LIBS} ${ICU_LIBRARIES})

find_package(Libedit REQUIRED)
include_directories(${Libedit_INCLUDE_DIRS})
set(LIBS ${LIBS} ${Libedit_LIBRARIES})

find_package(LibXML2 REQUIRED)
include_directories(${LibXML2_INCLUDE_DIRS})
set(LIBS ${LIBS} ${LibXML2_LIBRARIES})


find_package(LibXML++ REQUIRED)
include_directories(${LibXML++_INCLUDE_DIRS})
set(LIBS ${LIBS} ${LibXML++_LIBRARIES})

find_package(Loki REQUIRED)
include_directories(${Loki_INCLUDE_DIRS})
set(LIBS ${LIBS} ${Loki_LIBRARIES})

#find_package(Morfeusz REQUIRED)
#include_directories(${Morfeusz_INCLUDE_DIRS})
#set(LIBS ${LIBS} ${Morfeusz_LIBRARIES})

find_package(PwrUtils REQUIRED)
include_directories(${PwrUtils_INCLUDE_DIRS})
set(LIBS ${LIBS} ${PwrUtils_LIBRARIES})
set(LIBS ${LIBS} ${PwrUtils_LIBRARY})

find_package(SigC++ REQUIRED)
include_directories(${SigC++_INCLUDE_DIRS})
set(LIBS ${LIBS} ${SigC++_LIBRARIES})

find_package(Boost REQUIRED)
include_directories(${Boost_INCLUDE_DIRS})
set(LIBS ${LIBS} ${Boost_LIBRARIES})


set(SOURCE_FILES cpp/main.cpp cpp/pugixml-1.8/src/pugixml.cpp cpp/CascadeLemmatizer.cpp cpp/CascadeLemmatizer.h cpp/RuleLemmatizer.cpp cpp/RuleLemmatizer.h cpp/DictionaryLemmatizer.cpp cpp/DictionaryLemmatizer.h cpp/Inflection.cpp cpp/Inflection.h cpp/NamLivPersonLemmatizer.cpp cpp/NamLivPersonLemmatizer.h cpp/NamLocLemmatizer.cpp cpp/NamLocLemmatizer.h cpp/OrthLemmatizer.cpp cpp/OrthLemmatizer.h)
add_executable( polem ${SOURCE_FILES} ${LIBS})


target_link_libraries(polem ${LIBS})
