# from razor-qt/qtxdg project.
# defines
# LIBMAGIC_INCLUDE_DIR
# LIBMAGIC_FOUND
# LIBMAGIC_LIBRARY

FIND_PATH(LIBMAGIC_INCLUDE_DIR magic.h)

FIND_LIBRARY(LIBMAGIC_LIBRARY NAMES magic)

IF (LIBMAGIC_INCLUDE_DIR AND LIBMAGIC_LIBRARY)
	SET(LIBMAGIC_FOUND TRUE)
ENDIF (LIBMAGIC_INCLUDE_DIR AND LIBMAGIC_LIBRARY)

IF (LIBMAGIC_FOUND)
	IF (NOT LibMagic_FIND_QUIETLY)
		MESSAGE(STATUS "Found libmagic: ${LIBMAGIC_LIBRARY}")
		MESSAGE(STATUS " includes: ${LIBMAGIC_INCLUDE_DIR}")
	ENDIF (NOT LibMagic_FIND_QUIETLY)
ELSE (LIBMAGIC_FOUND)
	IF (LibMagic_FIND_REQUIRED)
		MESSAGE(STATUS "")
		MESSAGE(STATUS "libmagic development package cannot be found. Install it, please")
		MESSAGE(STATUS "For example in (open)SUSE it's file-devel package, in Ubuntu libmagic-dev")
		MESSAGE(STATUS "")
		MESSAGE(FATAL_ERROR "Could not find libmagic")
	ENDIF (LibMagic_FIND_REQUIRED)
ENDIF (LIBMAGIC_FOUND)
