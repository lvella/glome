SCRIPT=$(top_srcdir)/scripts/navigna_export.py

BUILT_SOURCES = hunter.wire

hunter.wire: hunter.blend ${SCRIPT}
	${BLENDER} -b hunter.blend -P ${SCRIPT}

CLEANFILES = ${BUILT_SOURCES}
