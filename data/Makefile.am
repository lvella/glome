if HAVE_BLENDER

MESH_SCRIPT=$(top_srcdir)/scripts/mesh_export.py

BUILT_SOURCES = hunter.wire destroyer.wire ufo.wire

#MESHS
hunter.wire: hunter.blend
	${BLENDER} -b hunter.blend -P ${MESH_SCRIPT} -noaudio > h 

destroyer.wire: destroyer.blend
	${BLENDER} -b destroyer.blend -P ${MESH_SCRIPT} -noaudio > d

ufo.wire: ufo.blend
	${BLENDER} -b ufo.blend -P ${MESH_SCRIPT} -noaudio > u

CLEANFILES = ${BUILT_SOURCES}

endif
