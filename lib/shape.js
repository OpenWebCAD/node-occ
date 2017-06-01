const assert = require('assert');
const occ = require('./bindings');
const fs = require("fs");


function export_debug_step(solid) {

    let debug_step_file = "debug_step";
    let counter= 0;
    while( fs.existsSync(debug_step_file + counter + ".step")) {
        counter ++;
    }
    debug_step_file =debug_step_file + counter + ".step"

    console.log("The solid causing problem has been saved in ",debug_step_file);
    occ.writeSTEP(debug_step_file, solid);
}
exports.init= function() {


    occ.Solid.prototype.getAllEdges = function() {
        // deprecated : use Solid.getEdgesi !
        let edges=[];
        let iterator = new occ.ShapeIterator(this,"EDGE");
        while(iterator.more) { edges.push(iterator.next());  }
        return edges;
    };
    occ.Face.prototype.getAllEdges = occ.Solid.prototype.getAllEdges;
    occ.Wire.prototype.getAllEdges = occ.Solid.prototype.getAllEdges;


    occ.BoundingBox.prototype.toString = function () {
        return "[ " +  this.nearPt.x.toFixed(3) +
                "," +  this.nearPt.y.toFixed(3) +
                "," +  this.nearPt.z.toFixed(3) +"]" +
            "[ " +  this.farPt.x.toFixed(3) +
            "," +  this.farPt.y.toFixed(3) +
            "," +  this.farPt.z.toFixed(3) +"]";
    };

    occ.buildSolidMeshNew = function (solid) {

        function makeEdgesIndexes(solid, jsonSolidMesh) {

            // produce each edge
            let mesh = solid.mesh;
            let indexes = null;

            solid.getEdges().forEach(edge => {
                if (edge.isSeam) {
                    return;
                }
                try {
                    indexes = mesh.getEdgeIndices(edge);
                }
                catch(err) {

                    // fall back to complete edge
                    var polyline =edge.polygonize();
                    if (!polyline) {
                        console.log("mesh# failed ! cannot extract edges indexes on solid",err,solid.hasMesh,solid.name,solid.getShapeName(edge));
                        console.log(" mesh", mesh.numVertex);
                        console.log(" edge", edge.polygonize());
                        export_debug_step(solid);
                        process.exit(2);
                    }

                    let entry = {
                        name: solid.getShapeName(edge),
                        mesh: toBase64(polyline)
                    };
                    jsonSolidMesh.edges.push(entry);
                    return;
                }
                if (!indexes || indexes.length == 0) {
                    return ; // ignore empty edges
                }

                let entry = {
                    name: solid.getShapeName(edge),
                    indexes: toBase64(indexes),
                };
                jsonSolidMesh.edges.push(entry);
            });
        }

        function toBase64(typedArray) {
            return ( new Buffer(typedArray.buffer)).toString("base64");
        }

        function makeFacesIndexes(solid, jsonSolidMesh) {

            let mesh = solid.mesh;
            solid.getFaces().forEach(face => {

                let indexes = mesh.getFaceTriangles(face);
                let entry = {
                    name: solid.getShapeName(face),
                    indexes: toBase64(indexes)
                };
                //xx entry.mesh.materials[0].colorDiffuse = [ Math.random(),Math.random(),Math.random()];
                jsonSolidMesh.faces.push(entry);
            });

        }
        assert(solid.hasOwnProperty('name'), "occ.buildSolidMesh : the solid must have a name");


        // make sure object is meshed
        if (!solid.hasMesh) {
            try {
                solid.createMesh(0.5,5);
            }
            catch(err) {


            }
            assert(solid.hasMesh);
        }
        let mesh = solid.mesh;


        let jsonSolidMesh = {name: solid.name, vertices: [], faces: [], edges: []};

        jsonSolidMesh.vertices = toBase64(mesh.vertices);

        makeFacesIndexes(solid, jsonSolidMesh);

        makeEdgesIndexes(solid, jsonSolidMesh);

        return jsonSolidMesh;
    };


    occ.buildSolidMesh = function (solid) {

        assert(solid.hasOwnProperty('name'), "occ.buildSolidMesh : the solid must have a name");

        // make sure object is meshed
        let mesh = solid.mesh;

        // produce each faces
        let faces = solid.getFaces();
        let face;

        let jsonSolidMesh = {name: solid.name, faces: [], edges: []};
        let i, entry;

        for (i = 0; i < faces.length; i++) {
            face = faces[i];

            if (!face.hasMesh) {
                continue;
            }
            entry = {
                name: solid.getShapeName(face),
                // color:(r*255+g)*255+b,
                mesh: face.mesh.toJSON()
            };
            entry.mesh.materials[0].colorDiffuse = [Math.random(), Math.random(), Math.random()];
            jsonSolidMesh.faces.push(entry);
        }
        // produce each edge

        let edges = solid.getEdges();
        let edge;
        for (i = 0; i < edges.length; i++) {
            edge = edges[i];

            if (edge.isSeam) continue;
            let polygone = edge.polygonize();

            //xx console.log( "polygone.length = ",polygone.length);
            let pts = [];
            for (let j = 0; j < polygone.length; j++) {
                pts.push(polygone[j]);
            }
            entry = {
                name: solid.getShapeName(edge),
                mesh: pts
            };
            //entry.mesh.materials[0].colorDiffuse = [ Math.random(),Math.random(),Math.random()];
            jsonSolidMesh.edges.push(entry);
        }
        return jsonSolidMesh;
    };

};
