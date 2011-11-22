if HAVE_BLENDER

MESH_SCRIPT=$(top_srcdir)/scripts/mesh_export.py
GUN_SCRIPT=$(top_srcdir)/scripts/gun_export.py

BUILT_SOURCES = hunter.wire destroyer.wire ufo.wire hunter.gun destroyer.gun ufo.gun

#MESHS
hunter.wire: hunter.blend
	${BLENDER} -b hunter.blend -P ${MESH_SCRIPT} -noaudio

destroyer.wire: destroyer.blend
	${BLENDER} -b destroyer.blend -P ${MESH_SCRIPT} -noaudio

ufo.wire: ufo.blend
	${BLENDER} -b ufo.blend -P ${MESH_SCRIPT} -noaudio


#GUNS
hunter.gun: hunter.blend
	${BLENDER} -b hunter.blend -P ${GUN_SCRIPT} -noaudio

destroyer.gun: destroyer.blend
	${BLENDER} -b destroyer.blend -P ${GUN_SCRIPT} -noaudio

ufo.gun: ufo.blend
	${BLENDER} -b ufo.blend -P ${GUN_SCRIPT} -noaudio

CLEANFILES = ${BUILT_SOURCES}

endif
