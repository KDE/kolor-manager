
INCLUDE(UsePkgConfig)
PKGCONFIG(elektra _ElektraIncDir _ElektraLinkDir _ElektraLinkFlags _ElektraCflags)

set(ELEKTRA_DEFINITIONS ${_ElektraCflags})
set(ELEKTRA_LIBRARIES ${_ElektraLinkFlags})
set(ELEKTRA_INCLUDE_DIR ${_ElektraIncDir})

if(ELEKTRA_DEFINITIONS AND ELEKTRA_LIBRARIES)

  # query pkg-config asking for Elektra >= 0.6.4
  EXEC_PROGRAM(${PKGCONFIG_EXECUTABLE} ARGS --atleast-version=0.6.4 elektra RETURN_VALUE _return_VALUE OUTPUT_VARIABLE _pkgconfigDevNull )

  if(_return_VALUE STREQUAL "0")
    set(ELEKTRA_FOUND TRUE)
    set(HAVE_ELEKTRA TRUE)
  else(_return_VALUE STREQUAL "0")
    message(STATUS "Elektra >= 0.6.4 was found")
  endif(_return_VALUE STREQUAL "0")
endif(ELEKTRA_DEFINITIONS AND ELEKTRA_LIBRARIES)

if (ELEKTRA_FOUND)
    if (NOT Elektra_FIND_QUIETLY)
        message(STATUS "Found ELEKTRA: ${ELEKTRA_LIBRARIES}")
    endif (NOT Elektra_FIND_QUIETLY)
else (ELEKTRA_FOUND)
    if (NOT Elektra_FIND_QUIETLY)
        message(STATUS "Elektra was NOT found.")
    endif (NOT Elektra_FIND_QUIETLY)
    if (Elektra_FIND_REQUIRED)
        message(FATAL_ERROR "Could NOT find Elektra")
    endif (Elektra_FIND_REQUIRED)
endif (ELEKTRA_FOUND)
