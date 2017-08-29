cmake_minimum_required(VERSION 3.5)

project(polem)

set(CMAKE_CXX_STANDARD 11)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/Modules/")

include(LibFindMacros)

find_package(Corpus2 REQUIRED)
include_directories(${Corpus2_INCLUDE_DIRS})
set(LIBS ${LIBS} ${Corpus2_LIBRARIES})
set(LIBS ${LIBS} ${Corpus2_LIBRARY})
set(LIBS ${LIBS} "/usr/local/lib/libcorpus2.so")
set(LIBS ${LIBS} "/usr/local/lib/libwccl.so")
set(LIBS ${LIBS} "/usr/lib/x86_64-linux-gnu/libicui18n.so")

find_package(Morfeusz2 REQUIRED)
include_directories(${Morfeusz2_INCLUDE_DIRS})
set(LIBS ${LIBS} ${Morfeusz2_LIBRARIES})
#set(LIBS ${LIBS} "/usr/local/lib/libmaca_morfeusz.so")
#set(LIBS ${LIBS} "/usr/local/lib/libmaca_morfeusz.so.1")
#set(LIBS ${LIBS} "/usr/local/lib/libmaca_morfeusz.so.1.0")
#set(LIBS ${LIBS} "/usr/local/lib/libmorfeusz.so")
#set(LIBS ${LIBS} "/usr/local/lib/libmorfeusz.so.0")
set(LIBS ${LIBS} "/usr/local/lib/libmorfeusz2.so")
#set(LIBS ${LIBS} "/usr/local/lib/libmorfeuszSGJP.so.0.7")
#-- Up-to-date: /usr/local/include/morfeusz2.h
#-- Up-to-date: /usr/local/include/morfeusz2_c.h
#-- Installing: /usr/local/lib/libmorfeusz2.so
#-- Installing: /usr/local/bin/morfeusz_analyzer
#-- Installing: /usr/local/bin/morfeusz_generator


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

find_package(SigC++ REQUIRED)
include_directories(${SigC++_INCLUDE_DIRS})
set(LIBS ${LIBS} ${SigC++_LIBRARIES})

find_package(Boost REQUIRED)
include_directories(${Boost_INCLUDE_DIRS})
set(LIBS ${LIBS} ${Boost_LIBRARIES})



set(SOURCE_FILES data/kpwr-1.2-ne-tune-fix.tsv data/kpwr-1.2-ne-train-fix.tsv data/kpwr-1.2-ne-test-fix.tsv data/kpwr-1.1-fixed-keywords-train.txt data/kpwr-1.1-fixed-keywords-test.txt cpp/main.cpp cpp/pugixml-1.8/src/pugixml.cpp cpp/CascadeLemmatizer.cpp cpp/CascadeLemmatizer.h cpp/RuleLemmatizer.cpp cpp/RuleLemmatizer.h cpp/DictionaryLemmatizer.cpp cpp/DictionaryLemmatizer.h cpp/Inflection.cpp cpp/Inflection.h cpp/NamLivPersonLemmatizer.cpp cpp/NamLivPersonLemmatizer.h cpp/NamLocLemmatizer.cpp cpp/NamLocLemmatizer.h cpp/OrthLemmatizer.cpp cpp/OrthLemmatizer.h)
add_executable( polem ${SOURCE_FILES} ${LIBS})


target_link_libraries(polem ${LIBS})
