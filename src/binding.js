// This code might be run multiple times!

var engine = {};
function bindEngine() {    
    Module["_enginebinding"] = {
        // Main
        init: Module.cwrap('js_init', 'void', ['string', 'number', 'number', 'number']), //const char* context, int width, int height, float curtime
        frame: Module.cwrap('js_frame', 'void', ['number']), // float curtime

        // Asset system
        asset_preload: Module.cwrap('js_asset_preload', 'number', ['number', 'string']), // Class, Path
        asset_incref: Module.cwrap('js_asset_incref', 'void', ['number']), // asset_t* asset
        asset_decref: Module.cwrap('js_asset_decref', 'void', ['number']), // asset_t* asset
        asset_indexof: Module.cwrap('js_asset_indexof', 'number', ['number']), // asset_t* asset
        asset_atindex: Module.cwrap('js_asset_atindex', 'number', ['number']), // assetIndex_t asset
        asset_data: Module.cwrap('js_asset_data', 'number', ['number']), // asset_t* asset
        //mesh_preload: Module.cwrap('js_mesh_preload', 'number', ['string']), // Path
        //texture_preload: Module.cwrap('js_texture_preload', 'number', ['string']), // Path
        //shader_preload: Module.cwrap('js_shader_preload', 'number', ['string']), // Path

        // Camera system
        camera_setOrigin: Module.cwrap('js_camera_setOrigin', 'void', ['number', 'number', 'number']),
        camera_setRotation: Module.cwrap('js_camera_setRotation', 'void', ['number', 'number', 'number']),
        camera_apply: Module.cwrap('js_camera_apply', 'void', []),

        // Shader System
        sun_setDir: Module.cwrap('js_sun_setDir', 'void', ['number', 'number', 'number']),
        sun_setColor: Module.cwrap('js_sun_setColor', 'void', ['number', 'number', 'number', 'number']),

        // Prop System
        prop_create: Module.cwrap('js_prop_create', 'number', []),
        prop_delete: Module.cwrap('js_prop_delete', 'void', ['number']),
        prop_deleteAll: Module.cwrap('js_prop_deleteAll', 'void', []),
        prop_setInfo: Module.cwrap('js_prop_setInfo', 'void', ['number', 'number', 'number', 'number']),
        prop_setOrigin: Module.cwrap('js_prop_setOrigin', 'void', ['number', 'number', 'number', 'number']),
        prop_setRotation: Module.cwrap('js_prop_setRotation', 'void', ['number', 'number', 'number', 'number']),
        prop_setScale: Module.cwrap('js_prop_setScale', 'void', ['number', 'number', 'number', 'number']),
        prop_setVisible: Module.cwrap('js_prop_setVisible', 'void', ['number', 'number']),
        prop_getVisible: Module.cwrap('js_prop_getVisible', 'number', ['number']),
        prop_count: Module.cwrap('js_prop_count', 'number', []),
        prop_draw: Module.cwrap('js_prop_draw', 'void', ['number']),
        prop_drawAll: Module.cwrap('js_prop_drawAll', 'void', []),

        // Primitives
        cubeMesh: Module.cwrap('js_cubeMesh', 'number', []),
        skyMesh: Module.cwrap('js_skyMesh', 'number', []),

        // Texture System
        texture_incRef: Module.cwrap('js_texture_incRef', 'void', ['number']),
        texture_decRef: Module.cwrap('js_texture_decRef', 'void', ['number']),

        terrainTest: Module.cwrap('js_terrainTest', 'number', []),
        //castDown: Module.cwrap('js_castDown', 'number', ['number','number','number']),
        
        prop_sim: Module.cwrap('js_prop_sim', 'void', ['number']),
        applyForce: Module.cwrap('js_applyForce', 'void', ['number', 'number', 'number']),

        
    };
    engine = Module["_enginebinding"];
    engine.ASSET_CLASS_NONE = 0
    engine.ASSET_CLASS_TEXTURE = 1 
    engine.ASSET_CLASS_MODEL = 2
    //engine.ASSET_CLASS_SOUND = 2
    engine.ASSET_CLASS_SHADER = 3
    
    console.log("Engine Bound!")
}


//};
/*
class vec3 {
    constructor(x,y,z) {
        this.x = x;
        this.y = y;
        this.z = z;
    }
}
class transform_t {
    constructor() {
        this.origin   = new vec3(0,0,0);
        this.rotation = new vec3(0,0,0);
        this.scale    = new vec3(0,0,0);
    }
}
*/
class prop_t {
    constructor() {
        this.id = engine.prop_create();
        
        this.mesh    = undefined;
        this.texture = undefined;
        this.shader  = undefined;
    }
    setOrigin(x, y, z) {
        engine.prop_setOrigin(this.id, x, y, z);
    }
    setScale(w, h, d) {
        engine.prop_setScale(this.id, w, h, d);
    }
    setRotation(p, y, r) {
        engine.prop_setRotation(this.id, p, y, r);
    }
    setInfo(mesh, texture, shader) {
        engine.prop_setInfo(this.id, mesh.id, texture.id, shader.id);

        mesh.incRef();
        texture.incRef();
        shader.incRef();

        this.mesh = mesh;
        this.texture = texture;
        this.shader = shader;
    }
    delete()
    {
        if(!(this.mesh === undefined))
            this.mesh.decRef();
            
        if(!(this.texture === undefined))
            this.texture.decRef();
        
        if(!(this.shader === undefined))
            this.shader.decRef();

        engine.prop_delete(this.id);
    }
}

class model_t // This is technically a mesh_t for now!
{
    constructor(path) {
        var a = engine.asset_preload(engine.ASSET_CLASS_MODEL, path);
        this.id = engine.asset_data(a);
        this.asset = engine.asset_indexof(a);
    }
    incRef() { engine.asset_incref(engine.asset_atindex(this.asset)); }
    decRef() { engine.asset_decref(engine.asset_atindex(this.asset)); }
}


class texture_t
{
    constructor(path) {
        var a = engine.asset_preload(engine.ASSET_CLASS_TEXTURE, path);
        this.id = engine.asset_data(a);
        this.asset = engine.asset_indexof(a);
    }
    incRef() { engine.asset_incref(engine.asset_atindex(this.asset)); }
    decRef() { engine.asset_decref(engine.asset_atindex(this.asset)); }
}

class shader_t
{
    constructor(path) {
        var a = engine.asset_preload(engine.ASSET_CLASS_SHADER, path);
        this.id = engine.asset_data(a);
        this.asset = engine.asset_indexof(a);
    }
    incRef() { engine.asset_incref(engine.asset_atindex(this.asset)); }
    decRef() { engine.asset_decref(engine.asset_atindex(this.asset)); }
}