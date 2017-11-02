IF (Polem_INCLUDE_DIR AND Polem_LIBRARY)
    SET(Polem_FOUND TRUE)
ELSE (Polem_INCLUDE_DIR AND Polem_LIBRARY)
    FIND_PATH(Polem_INCLUDE_DIR ../../CascadeLemmatizer.h /usr/include /usr/local/include)

    FIND_LIBRARY(Polem_LIBRARY polem-dev /usr/lib /usr/local/lib)

    MARK_AS_ADVANCED(Polem_LIBRARY)
    MARK_AS_ADVANCED(Polem_INCLUDE_DIR)

    IF (Polem_INCLUDE_DIR AND Polem_LIBRARY)
        SET(Polem_FOUND TRUE)
    ENDIF (Polem_INCLUDE_DIR AND Polem_LIBRARY)

ENDIF (Polem_INCLUDE_DIR AND Polem_LIBRARY)

IF (Polem_FOUND)

    IF (NOT Polem_FIND_QUIETLY)
        MESSAGE(STATUS "Found libpolem-dev : ${Polem_LIBRARY}")
    ENDIF (NOT Polem_FIND_QUIETLY)
ELSE (Polem_FOUND)
    IF (Polem_FIND_REQUIRED)
        MESSAGE(FATAL_ERROR "Could not find libpolem-dev")
    ELSE (Polem_FIND_REQUIRED)
        MESSAGE(STATUS "libpolem-dev not found")
    ENDIF (Polem_FIND_REQUIRED)
ENDIF (Polem_FOUND)