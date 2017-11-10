cmake_minimum_required(VERSION 3.2)

project(polem)

set(BUILD_SHARED_LIBS 1)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED YES)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${PROJECT_SOURCE_DIR}/cmake/Modules")

set(CMAKE_INSTALL_LIBDIR "lib")
set(CMAKE_INSTALL_INCLUDEDIR "include")

include(LibFindMacros)

find_package(JNI REQUIRED)

find_package(Boost COMPONENTS program_options REQUIRED)
include_directories(${Boost_INCLUDE_DIRS})
set(LIBS ${LIBS} ${Boost_LIBRARIES})

find_package(Corpus2 REQUIRED)
include_directories(${Corpus2_INCLUDE_DIRS})
set(LIBS ${LIBS} ${Corpus2_LIBRARY})

find_package(ICU REQUIRED)
include_directories(${ICU_INCLUDE_DIRS})
set(LIBS ${LIBS} ${ICU_LIBRARIES})
set(LIBS ${LIBS} ${ICU_I18N_LIBRARIES})

find_library(WCCL_LIBRARY wccl /usr/lib/ /usr/local/lib/)
find_path(WCCL_INCLUDE_DIR libwccl /usr/lib/ /usr/local/lib)
if(WCCL_LIBRARY AND WCCL_INCLUDE_DIR)
    message("-- Found WCCL libraries: " ${WCCL_LIBRARY})
    include_directories(${WCCL_INCLUDE_DIR})
    set(LIBS ${LIBS} ${WCCL_LIBRARY})
endif(WCCL_LIBRARY AND WCCL_INCLUDE_DIR)

find_package(Morfeusz2 REQUIRED)
include_directories(${MORFEUSZ2_INCLUDE_DIR})
set(LIBS ${LIBS} ${MORFEUSZ2_LIBRARY})

find_package(PwrUtils REQUIRED)
include_directories(${PwrUtils_INCLUDE_DIRS})
set(LIBS ${LIBS} ${PwrUtils_LIBRARIES})
set(LIBS ${LIBS} ${PwrUtils_LIBRARY})



set(SOURCE_FILES polem/main.cpp ${CMAKE_CURRENT_SOURCE_DIR}/pugixml-1.8/src/pugixml.cpp
        polem/CascadeLemmatizer.cpp polem/RuleLemmatizer.cpp
        polem/DictionaryLemmatizer.cpp polem/Inflection.cpp
        polem/NamLivPersonLemmatizer.cpp polem/NamLocLemmatizer.cpp
        polem/OrthLemmatizer.cpp polem/CascadeLemmatizer.h
        polem/RuleLemmatizer.h polem/DictionaryLemmatizer.h
        polem/Inflection.h polem/NamLivPersonLemmatizer.h
        polem/NamLocLemmatizer.h polem/OrthLemmatizer.h)

set(LIBRARY_FILES ${CMAKE_CURRENT_SOURCE_DIR}/pugixml-1.8/src/pugixml.cpp polem/CascadeLemmatizer.cpp
        polem/RuleLemmatizer.cpp polem/DictionaryLemmatizer.cpp
        polem/Inflection.cpp polem/NamLivPersonLemmatizer.cpp
        polem/NamLocLemmatizer.cpp polem/OrthLemmatizer.cpp
        polem/CascadeLemmatizer.h polem/RuleLemmatizer.h
        polem/DictionaryLemmatizer.h polem/Inflection.h
        polem/NamLivPersonLemmatizer.h polem/NamLocLemmatizer.h
        polem/OrthLemmatizer.h)

set(PUBLIC_HEADERS polem/CascadeLemmatizer.h polem/DictionaryLemmatizer.h
        polem/NamLivPersonLemmatizer.h polem/NamLocLemmatizer.h
        polem/RuleLemmatizer.h polem/OrthLemmatizer.h
        polem/Inflection.h)

#set(WRAPPER_FILES ${CMAKE_CURRENT_SOURCE_DIR}/JavaLem_wrap.cxx)
#set(WRAPPER_HEADERS "")

file(GLOB DEPLOY_FILES_AND_DIRS "${PROJECT_SOURCE_DIR}/dict/*")
foreach(ITEM ${DEPLOY_FILES_AND_DIRS})
    LIST( APPEND FILES_TO_DEPLOY "${ITEM}" )
endforeach()
INSTALL( FILES ${FILES_TO_DEPLOY} DESTINATION share/polem )


add_executable( polem ${SOURCE_FILES} ${LIBS})
target_link_libraries(polem ${LIBS})

install(TARGETS polem RUNTIME DESTINATION "/usr/local/bin/")


set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)

add_library(polem-dev SHARED ${LIBRARY_FILES})
target_link_libraries(polem-dev ${Corpus2_LIBRARY})
target_link_libraries(polem-dev ${LIBS})


install(TARGETS polem-dev
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})
install(FILES ${PUBLIC_HEADERS}
        DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

#add_library(polemJava-dev SHARED ${WRAPPER_FILES})
#set_property(TARGET polemJava-dev PROPERTY POSITION_INDEPENDENT_CODE ON)
#include_directories(${JNI_INCLUDE_DIRS})
#target_link_libraries(polemJava-dev polem-dev)
#target_link_libraries(polemJava-dev ${JNI_LIBRARIES})
#target_link_libraries(polemJava-dev ${Corpus2_LIBRARY})
#target_link_libraries(polemJava-dev ${LIBS})


#install(TARGETS polemJava-dev
#        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})
#install(FILES ${WRAPPER_HEADERS}
#        DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})