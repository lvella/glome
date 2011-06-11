SCRIPT=$(top_srcdir)/scripts/navigna_export.py

BUILT_SOURCES = hunter.wire destroyer.wire

hunter.wire: hunter.blend ${SCRIPT}
	${BLENDER} -b hunter.blend -P ${SCRIPT}

destroyer.wire: destroyer.blend ${SCRIPT}
	${BLENDER} -b destroyer.blend -P ${SCRIPT}

CLEANFILES = ${BUILT_SOURCES}
