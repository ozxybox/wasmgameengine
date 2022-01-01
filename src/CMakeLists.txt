set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
SET(CMAKE_EXE_LINKER_FLAGS  "${CMAKE_EXE_LINKER_FLAGS} -pthread -s USE_PTHREADS=1 -s ALLOW_MEMORY_GROWTH=1 -s FETCH=1 -s PTHREAD_POOL_SIZE=8 -s ASSERTIONS=1 -s MAX_WEBGL_VERSION=2 -s EXPORTED_RUNTIME_METHODS=['ccall','cwrap']")
#SET(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -pthread")
add_definitions("-pthread")
#add_definitions("-s EXPORTED_RUNTIME_METHODS='[\"ccall\",\"cwrap\"]'")

set( SOURCES 
	

# Utils
	log.h
	log.c
	linkedlist.h
	linkedlist.c
	refpool.h
	refpool.c
	stringutils.h
	stringutils.c
	hash.h
	hash.c
	hashmap.h
	hashmap.c
	map.h
	map.c
	
# Math Utils
	linalg.h
	linalg.c
	transform.h
	transform.c

# Decoders
	objimport.h
	objimport.c
	assetdecode.h
	assetdecode.c

# Asset Systems
	asset.h
	asset.c
	model.h
	#	model.c
	primitives.h
	primitives.c
	prop.h
	prop.c
	errorassets.c
	
# Rendering
	renderer.h
	renderer.c
	shader.h
	shader.c
	texture.h
	texture.c
	mesh.h
	mesh.c
	camera.h
	camera.c

# Environment
	terrain.h
	terrain.c
	physics.h
	physics.c

# Javascript things
	binding.c

)

include_directories( ${CMAKE_CURRENT_SOURCE_DIR} )
add_executable( wasmtest ${SOURCES} )
target_include_directories( wasmtest PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries( wasmtest lodepng )
em_link_post_js(wasmtest binding.js)