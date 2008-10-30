# - Try to find the CUPS client libraries
# Once done this will define
#
#  CUPS_FOUND - system has libcups
#  CUPS_INCLUDE_DIR - the cups include directory
#  CUPS_LIBRARIES - libcups

if(CUPS_INCLUDE_DIR AND CUPS_LIBRARIES)
    # Already in cache, be silent
    set(Cups_FIND_QUIETLY TRUE)
endif(CUPS_INCLUDE_DIR AND CUPS_LIBRARIES)


FIND_PATH(CUPS_INCLUDE_DIR cups/cups.h)

if(APPLE)
   FIND_LIBRARY(CUPS_LIBRARIES NAMES CUPS
   	PATHS
   	/System/Library/Frameworks
   	/Library/Frameworks
   )
else(APPLE)
   FIND_LIBRARY(CUPS_LIBRARIES NAMES cups)
   
endif(APPLE)

if(CUPS_INCLUDE_DIR AND CUPS_LIBRARIES)
   set(CUPS_FOUND TRUE)
endif(CUPS_INCLUDE_DIR AND CUPS_LIBRARIES)

if(CUPS_FOUND)
   if(NOT Cups_FIND_QUIETLY)
      message(STATUS "Found cups: ${CUPS_LIBRARIES}")
   endif(NOT Cups_FIND_QUIETLY)
else(CUPS_FOUND)
   if (Cups_FIND_REQUIRED)
        message(FATAL_ERROR "Could NOT find cups")
   endif (Cups_FIND_REQUIRED)
endif(CUPS_FOUND)

MARK_AS_ADVANCED(CUPS_INCLUDE_DIR CUPS_LIBRARIES )
