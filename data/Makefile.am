SCRIPT=$(top_srcdir)/scripts/navigna_export.py

BUILT_SOURCES = destroyer.wire hunter.wire ufo.wire

ufo.wire: ufo.blend ${SCRIPT}
	${BLENDER} -b ufo.blend -P ${SCRIPT}

destroyer.wire: destroyer.blend ${SCRIPT}
	${BLENDER} -b destroyer.blend -P ${SCRIPT}

hunter.wire: hunter.blend ${SCRIPT}
	${BLENDER} -b hunter.blend -P ${SCRIPT}

CLEANFILES = ${BUILT_SOURCES}
