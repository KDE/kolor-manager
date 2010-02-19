
INCLUDE(UsePkgConfig)
PKGCONFIG(oyranos _OyranosIncDir _OyranosLinkDir _OyranosLinkFlags _OyranosCflags)

set(OYRANOS_DEFINITIONS ${_OyranosCflags})
set(OYRANOS_LIBRARIES ${_OyranosLinkFlags})
set(OYRANOS_INCLUDE_DIR ${_OyranosIncDir})

if(OYRANOS_DEFINITIONS AND OYRANOS_LIBRARIES)

  # query pkg-config asking for Oyranos >= 0.1.7
  EXEC_PROGRAM(${PKGCONFIG_EXECUTABLE} ARGS --atleast-version=0.1.7 oyranos RETURN_VALUE _return_VALUE OUTPUT_VARIABLE _pkgconfigDevNull )

  if(_return_VALUE STREQUAL "0")
    set(OYRANOS_FOUND TRUE)
    set(HAVE_OYRANOS TRUE)
  else(_return_VALUE STREQUAL "0")
    message(STATUS "Oyranos >= 0.1.7 was found")
  endif(_return_VALUE STREQUAL "0")
endif(OYRANOS_DEFINITIONS AND OYRANOS_LIBRARIES)

if (OYRANOS_FOUND)
    if (NOT Oyranos_FIND_QUIETLY)
        message(STATUS "Found OYRANOS: ${OYRANOS_LIBRARIES} ${OYRANOS_INCLUDE_DIR}")
    endif (NOT Oyranos_FIND_QUIETLY)
else (OYRANOS_FOUND)
    if (NOT Oyranos_FIND_QUIETLY)
        message(STATUS "Oyranos was NOT found.")
    endif (NOT Oyranos_FIND_QUIETLY)
    if (Oyranos_FIND_REQUIRED)
        message(FATAL_ERROR "Could NOT find Oyranos")
    endif (Oyranos_FIND_REQUIRED)
endif (OYRANOS_FOUND)
