set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
SET(CMAKE_EXE_LINKER_FLAGS  "${CMAKE_EXE_LINKER_FLAGS} -pthread -s USE_PTHREADS=1 -s ALLOW_MEMORY_GROWTH=1 -s FETCH=1 -s PTHREAD_POOL_SIZE=8 -s ASSERTIONS=1 -s MAX_WEBGL_VERSION=2 -s EXPORTED_RUNTIME_METHODS=['ccall','cwrap']")
#SET(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -pthread")
add_definitions("-pthread")
#add_definitions("-s EXPORTED_RUNTIME_METHODS='[\"ccall\",\"cwrap\"]'")

set( SOURCES 
	

# Utils
	"util/log.h"
	"util/log.c"
	"util/linkedlist.h"
	"util/linkedlist.c"
	"util/refpool.h"
	"util/refpool.c"
	"util/stringutils.h"
	"util/stringutils.c"
	"util/hash.h"
	"util/hash.c"
	"util/hashmap.h"
	"util/hashmap.c"
	"util/map.h"
	"util/map.c"
	
# Math util
	"math/linalg.h"
	"math/linalg.c"
	"math/linalg_p.h"
	"math/linalg_p.c"
	"math/mat4.c"
	"math/transform.h"
	"math/transform.c"
	"math/geometry.h"
	"math/geometry.c"

	
# Asset System
	"assetsys/asset.h"
	"assetsys/asset.c"
	"assetsys/primitives.h"
	"assetsys/primitives.c"
	"assetsys/objimport.h"
	"assetsys/objimport.c"
	"assetsys/assetdecode.h"
	"assetsys/assetdecode.c"

	
# Rendering
	"render/renderer.h"
	"render/renderer.c"
	"render/shader.h"
	"render/shader.c"
	"render/texture.h"
	"render/texture.c"
	"render/mesh.h"
	"render/mesh.c"
	"render/camera.h"
	"render/camera.c"
	"render/errorassets.c"
	"render/vertex.h"
	"render/vertex.c"

# Environment
	"environment/terrain.h"
	"environment/terrain.c"
	"environment/physics.h"
	"environment/physics.c"
	"environment/prop.h"
	"environment/prop.c"

# Javascript things
	"binding.c"

)

set(INCLUDE_DIRS

	"${CMAKE_CURRENT_SOURCE_DIR}/util/"
	"${CMAKE_CURRENT_SOURCE_DIR}/math/"
	"${CMAKE_CURRENT_SOURCE_DIR}/render/"
	"${CMAKE_CURRENT_SOURCE_DIR}/assetsys/"
	"${CMAKE_CURRENT_SOURCE_DIR}/environment/"

)

include_directories( ${CMAKE_CURRENT_SOURCE_DIR} )
add_executable( wasmtest ${SOURCES} )
target_include_directories( wasmtest PRIVATE ${CMAKE_CURRENT_SOURCE_DIR} ${INCLUDE_DIRS})
target_link_libraries( wasmtest lodepng )
em_link_post_js(wasmtest binding.js)