cmake_minimum_required(VERSION 2.8)

project(polem)

if(UNIX)
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -std=gnu++0x")
endif()

set(BUILD_SHARED_LIBS 1)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED YES)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${PROJECT_SOURCE_DIR}/cmake/Modules")

set(CMAKE_INSTALL_LIBDIR "lib")
set(CMAKE_INSTALL_INCLUDEDIR "include")

include(LibFindMacros)

find_package(JNI)

find_package(Java)

include(UseJava)

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
        polem/NamLocLemmatizer.h polem/OrthLemmatizer.h
        polem/InflectionRule.h polem/InflectionRule.cpp)

set(LIBRARY_FILES ${CMAKE_CURRENT_SOURCE_DIR}/pugixml-1.8/src/pugixml.cpp ${CMAKE_CURRENT_SOURCE_DIR}/polem/CascadeLemmatizer.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/polem/RuleLemmatizer.cpp ${CMAKE_CURRENT_SOURCE_DIR}/polem/DictionaryLemmatizer.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/polem/Inflection.cpp ${CMAKE_CURRENT_SOURCE_DIR}/polem/NamLivPersonLemmatizer.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/polem/NamLocLemmatizer.cpp ${CMAKE_CURRENT_SOURCE_DIR}/polem/OrthLemmatizer.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/polem/CascadeLemmatizer.h ${CMAKE_CURRENT_SOURCE_DIR}/polem/RuleLemmatizer.h
        ${CMAKE_CURRENT_SOURCE_DIR}/polem/DictionaryLemmatizer.h ${CMAKE_CURRENT_SOURCE_DIR}/polem/Inflection.h
        ${CMAKE_CURRENT_SOURCE_DIR}/polem/NamLivPersonLemmatizer.h ${CMAKE_CURRENT_SOURCE_DIR}/polem/NamLocLemmatizer.h
        ${CMAKE_CURRENT_SOURCE_DIR}/polem/OrthLemmatizer.h ${CMAKE_CURRENT_SOURCE_DIR}/polem/InflectionRule.h
        ${CMAKE_CURRENT_SOURCE_DIR}/polem/InflectionRule.cpp)


set(PUBLIC_HEADERS ${CMAKE_CURRENT_SOURCE_DIR}/polem/CascadeLemmatizer.h ${CMAKE_CURRENT_SOURCE_DIR}/polem/DictionaryLemmatizer.h
        ${CMAKE_CURRENT_SOURCE_DIR}/polem/NamLivPersonLemmatizer.h ${CMAKE_CURRENT_SOURCE_DIR}/polem/NamLocLemmatizer.h
        ${CMAKE_CURRENT_SOURCE_DIR}/polem/RuleLemmatizer.h ${CMAKE_CURRENT_SOURCE_DIR}/polem/OrthLemmatizer.h
        ${CMAKE_CURRENT_SOURCE_DIR}/polem/Inflection.h)

set(WRAPPER_FILES ${CMAKE_CURRENT_SOURCE_DIR}/javawrap/WrapLem_wrap.cxx)
#set(WRAPPER_HEADERS "")

set(PY_FILES ${CMAKE_CURRENT_SOURCE_DIR}/pythonwrap/WrapLem_wrap.cxx)
set(PY_HEADERS ${CMAKE_CURRENT_SOURCE_DIR}/pythonwrap/WrapLem.py)

file(GLOB DEPLOY_FILES_AND_DIRS "${PROJECT_SOURCE_DIR}/dict/*")
foreach(ITEM ${DEPLOY_FILES_AND_DIRS})
    LIST( APPEND FILES_TO_DEPLOY "${ITEM}" )
endforeach()
INSTALL( FILES ${FILES_TO_DEPLOY} DESTINATION share/polem )


add_executable( polem ${SOURCE_FILES} ${LIBS})
target_link_libraries(polem ${LIBS})

install(TARGETS polem RUNTIME DESTINATION "/usr/local/bin/")


#set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)


add_library(polem-dev SHARED ${LIBRARY_FILES})
set_target_properties(polem-dev PROPERTIES
        PUBLIC_HEADER ${PUBLIC_HEADERS})
target_include_directories(polem-dev PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/polem)
target_include_directories(polem-dev PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

install(TARGETS polem-dev
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/polem-dev)
target_link_libraries(polem-dev ${ICU_I18N_LIBRARIES})
target_link_libraries(polem-dev ${ICU_LIBRARIES})
target_link_libraries(polem-dev ${Corpus2_LIBRARY})
target_link_libraries(polem-dev ${LIBS})
install(FILES ${PUBLIC_HEADERS}
        DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/polem-dev)


#####JAVA
if(Java_FOUND)
    FIND_PACKAGE(SWIG REQUIRED)
    INCLUDE(${SWIG_USE_FILE})

    include(UseSWIG)
    include_directories(${JNI_INCLUDE_DIRS})

    SET(CMAKE_SWIG_OUTDIR ${CMAKE_CURRENT_BINARY_DIR}/java)
    SET(CMAKE_SWIG_FLAGS -package g419.polem)

    set_property(SOURCE "WrapLem.i" PROPERTY CPLUSPLUS ON)

    swig_add_module(
        PolemJava
        java
        WrapLem.i
        ${LIBRARY_FILES}
    )

    TARGET_INCLUDE_DIRECTORIES(PolemJava
        PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}
        PRIVATE ${JAVA_INCLUDE_PATH}
        PRIVATE ${JAVA_INCLUDE_PATH2}
        )

    target_link_libraries(PolemJava ${LIBS})

    FILE(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/classes)
    add_custom_command(TARGET PolemJava POST_BUILD
        COMMAND ${Java_JAVAC_EXECUTABLE} -d classes java/*.java
        COMMAND ${Java_JAR_EXECUTABLE} -cfM PolemJava.jar -C classes .
        )

    INSTALL(TARGETS PolemJava
        DESTINATION ${CMAKE_INSTALL_LIBDIR})


endif()

####PYTHON

find_package(Python3 COMPONENTS Interpreter Development)
find_package(Python2 COMPONENTS Interpreter Development)

if(${Python3_VERSION} EQUAL 3.6)

    INCLUDE(${SWIG_USE_FILE})

    include(UseSWIG)
    
    INCLUDE_DIRECTORIES(${Python3_INCLUDE_DIRS})
    INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR})
    
    SET(CMAKE_SWIG_FLAGS "")
    SET_SOURCE_FILES_PROPERTIES(${CMAKE_CURRENT_SOURCE_DIR}/WrapLem.i PROPERTIES CPLUSPLUS ON)
    
    SWIG_ADD_LIBRARY(WrapLem LANGUAGE python SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/WrapLem.i)
    SWIG_LINK_LIBRARIES(WrapLem polem-dev ${LIBS} ${Python3_LIBRARIES})

    execute_process ( COMMAND python3.6 -c "import site; print(site.getsitepackages()[0])" OUTPUT_VA$

    install(TARGETS _WrapLem
        LIBRARY DESTINATION ${PYTHON_SITE_PACKAGES})
    install(FILES ${PY_HEADERS}
        DESTINATION ${PYTHON_SITE_PACKAGES})

elseif(Python2_FOUND)
    INCLUDE_DIRECTORIES(${Python2_INCLUDE_DIRS})
    INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR})
    
    SET(CMAKE_SWIG_FLAGS "")
    SET_SOURCE_FILES_PROPERTIES(${CMAKE_CURRENT_SOURCE_DIR}/WrapLem.i PROPERTIES CPLUSPLUS ON)
    
    SWIG_ADD_LIBRARY(WrapLem LANGUAGE python SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/WrapLem.i)
    SWIG_LINK_LIBRARIES(WrapLem polem-dev ${LIBS} ${Python2_LIBRARIES})

    execute_process(COMMAND python2 -c "from distutils.sysconfig import get_python_lib; print get_python_lib()" OUTPUT_VARIABLE PYTHON2_SITE_PACKAGES OUTPUT_STRIP_TRAILING_WHITESPACE)

    install(TARGETS _WrapLem
        LIBRARY DESTINATION ${PYTHON2_SITE_PACKAGES})
    install(FILES ${PY_HEADERS}
        DESTINATION ${PYTHON2_SITE_PACKAGES})
endif()



