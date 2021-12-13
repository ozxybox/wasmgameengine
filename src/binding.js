// This code might be run multiple times!

var engine = {};
function bindEngine() {    
    Module["_enginebinding"] = {
        // Main
        init: Module.cwrap('js_init', 'void', ['string', 'number', 'number']),
        frame: Module.cwrap('js_frame', 'void', []),

        // Asset system
        preloadMesh: Module.cwrap('js_preloadMesh', 'number', ['string']), // Path
        preloadTexture: Module.cwrap('js_preloadTexture', 'number', ['string']), // Path
        preloadShader: Module.cwrap('js_preloadShader', 'number', ['string']), // Path

        // Camera system
        setCameraOrigin: Module.cwrap('js_setCameraOrigin', 'void', ['number', 'number', 'number']),
        setCameraRotation: Module.cwrap('js_setCameraRotation', 'void', ['number', 'number', 'number']),
        applyCamera: Module.cwrap('js_applyCamera', 'void', []),

        // Shader System
        setSunDir: Module.cwrap('js_setSunDir', 'void', ['number', 'number', 'number']),
        setSunColor: Module.cwrap('js_setSunColor', 'void', ['number', 'number', 'number', 'number']),

        // Prop System
        createProp: Module.cwrap('js_createProp', 'number', []),
        deleteProp: Module.cwrap('js_deleteProp', 'void', ['number']),
        deleteAllProps: Module.cwrap('js_deleteAllProps', 'void', []),
        setPropInfo: Module.cwrap('js_setPropInfo', 'void', ['number', 'number', 'number', 'number']),
        setPropOrigin: Module.cwrap('js_setPropOrigin', 'void', ['number', 'number', 'number', 'number']),
        setPropRotation: Module.cwrap('js_setPropRotation', 'void', ['number', 'number', 'number', 'number']),
        setPropScale: Module.cwrap('js_setPropScale', 'void', ['number', 'number', 'number', 'number']),
        setPropVisible: Module.cwrap('js_setPropVisible', 'void', ['number', 'number']),
        getPropVisible: Module.cwrap('js_getPropVisible', 'number', ['number']),
        propCount: Module.cwrap('js_propCount', 'number', []),
        drawProp: Module.cwrap('js_drawProp', 'void', ['number']),
        drawAllProps: Module.cwrap('js_drawAllProps', 'void', []),

        // Primitives
        cubeMesh: Module.cwrap('js_cubeMesh', 'number', []),
        skyMesh: Module.cwrap('js_skyMesh', 'number', []),
    };
    engine = Module["_enginebinding"];
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
        this.id = engine.createProp();
    }
    setOrigin(x, y, z) {
        engine.setPropOrigin(this.id, x, y, z);
    }
    setScale(w, h, d) {
        engine.setPropScale(this.id, w, h, d);
    }
    setRotation(p, y, r) {
        engine.setPropRotation(this.id, p, y, r);
    }
    setInfo(mesh, texture, shader) {
        engine.setPropInfo(this.id, mesh, texture, shader);
    }
}
