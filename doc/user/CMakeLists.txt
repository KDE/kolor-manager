SET( GENERATOR /home/kuwe/.local/bin/oyranos-policy -vvv )
SET( DOCBOOK_BASE ${CMAKE_CURRENT_SOURCE_DIR}/po )
SET( DOCBOOK_AFTER docs/kcontrol/kcm_kmsettings )

FOREACH(lang en_US ${OYRANOS_LINGUAS_FULL})
  ADD_CUSTOM_TARGET( generate-${lang}
      COMMAND mkdir -p ${DOCBOOK_BASE}/${lang}/${DOCBOOK_AFTER}
      COMMAND echo "LANG=${lang}.UTF-8 ${GENERATOR} --docbook --doc-title KolorManager KCM --doc-version ${${PROJECT_NAME}_VERSION} > ${DOCBOOK_BASE}/${lang}/${DOCBOOK_AFTER}/index.docbook"
      COMMAND LANG=${lang}.UTF-8 ${GENERATOR} --docbook --doc-title "KolorManager KCM" --doc-version ${${PROJECT_NAME}_VERSION} > ${DOCBOOK_BASE}/${lang}/${DOCBOOK_AFTER}/index.docbook
      COMMENT generate KDE help files in docbook format
      VERBATIM
  )
  SET( GENS ${GENS} generate-${lang} )
ENDFOREACH()

ADD_CUSTOM_TARGET( generate
	COMMAND echo "generated following linguas: ${OYRANOS_LINGUAS_FULL}"
		DEPENDS ${GENS}
		COMMENT generate docbook KDE help files
		VERBATIM
		)

# install docbook for all available languages
KDOCTOOLS_INSTALL(po)

# install a single language docbook
#kdoctools_create_handbook(index.docbook INSTALL_DESTINATION ${KDE_INSTALL_DOCBUNDLEDIR}/en SUBDIR kcontrol/kcm_kmsettings)
