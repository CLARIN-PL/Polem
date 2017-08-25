IF (Corpus2_INCLUDE_DIR AND Corpus2_LIBRARY)
#Already in cache
   SET(Corpus2_FOUND TRUE)
ELSE (Corpus2_INCLUDE_DIR AND Corpus2_LIBRARY)
   FIND_PATH(Corpus2_INCLUDE_DIR libcorpus2/token.h /usr/include /usr/local/include)

   FIND_LIBRARY(Corpus2_LIBRARY corpus2 /usr/lib /usr/local/lib) 

   MARK_AS_ADVANCED(Corpus2_LIBRARY)
   MARK_AS_ADVANCED(Corpus2_INCLUDE_DIR)

   IF (Corpus2_INCLUDE_DIR AND Corpus2_LIBRARY)
      SET(Corpus2_FOUND TRUE)
   ENDIF (Corpus2_INCLUDE_DIR AND Corpus2_LIBRARY)

ENDIF (Corpus2_INCLUDE_DIR AND Corpus2_LIBRARY)


IF (Corpus2_FOUND)
   set(Corpus2_VERSION 0.0.0)
   FIND_FILE(_Corpus2_VERSION_FILE libcorpus2/version.h ${Corpus2_INCLUDE_DIR})
   MARK_AS_ADVANCED(_Corpus2_VERSION_FILE)
   IF (_Corpus2_VERSION_FILE)
      FILE(READ ${_Corpus2_VERSION_FILE} _Corpus2_VERSION_CONENTS)
      STRING(REGEX REPLACE ".*#define LIBCORPUS2_VERSION \\\"([0-9.]+)\\\".*" "\\1" Corpus2_VERSION "${_Corpus2_VERSION_CONENTS}")
   ENDIF (_Corpus2_VERSION_FILE)
   IF (Corpus2_FIND_VERSION)
      IF (Corpus2_VERSION VERSION_LESS Corpus2_FIND_VERSION)
         IF (Corpus2_FIND_REQUIRED)
            MESSAGE(${_Corpus2_VERSION_FILE})
            MESSAGE(FATAL_ERROR "Corpus2 version too old: ${Corpus2_VERSION}, requested >= ${Corpus2_FIND_VERSION}")
         ELSE (Corpus2_FIND_REQUIRED)
            IF (NOT Corpus2_FIND_QUIETLY)
               MESSAGE(STATUS "Corpus2 version too old: ${Corpus2_VERSION}, requested >= ${Corpus2_FIND_VERSION}")
            ENDIF (NOT Corpus2_FIND_QUIETLY)
         ENDIF (Corpus2_FIND_REQUIRED)
         set(Corpus2_FOUND False)
      ENDIF (Corpus2_VERSION VERSION_LESS Corpus2_FIND_VERSION)
   ENDIF (Corpus2_FIND_VERSION)
   IF (NOT Corpus2_FIND_QUIETLY)
      MESSAGE(STATUS "Found libcorpus2 ${Corpus2_VERSION}: ${Corpus2_LIBRARY}")
   ENDIF (NOT Corpus2_FIND_QUIETLY)
ELSE (Corpus2_FOUND)
   IF (Corpus2_FIND_REQUIRED)
      MESSAGE(FATAL_ERROR "Could not find libcorpus2")
   ELSE (Corpus2_FIND_REQUIRED)
      MESSAGE(STATUS "libcorpus2 not found")
   ENDIF (Corpus2_FIND_REQUIRED)
ENDIF (Corpus2_FOUND)
