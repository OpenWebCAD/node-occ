const assert = require("assert");
const occ = require(".").occ;
const ProgressBar = require('progress');
const fs = require("fs");
require("should");


const filename = process.argv[2];
console.log("filename = ",filename);
if (!fs.existsSync(filename)) {
    console.log("cannot find file ",filename);
    process.exit(1);
}
const b3_step   = filename;


let bar = new ProgressBar("reading file [:bar] :percent elapsed :elapseds ETA :etas", {
    complete: '=',
    incomplete: '-',
    width: 100,
    total: 1000
});


function read_file(callback) {
    occ.readSTEP(b3_step, function (err, shapes) {

        console.log(" nbShapes = ",shapes.length);
        callback(err,shapes);
    }, function feedback(percent) {
        bar.tick(percent);
    });
}

function performMesh(solids,callback) {
    let bar = new ProgressBar("meshing solids [:bar] :percent elapsed :elapseds  ETA :etas", {
        complete: '=',
        incomplete: '-',
        width: 100,
        total: solids.length
    });
    let nbTri = 0;
    let nbVert = 0;
    let jsonSize =0;
    for (let i  in solids) {
        bar.tick();
        let solid = solids[i];
        if (solid.numFaces <= 1) {
            console.log("solid has only one face !",solid);
        }
        solid.numFaces.should.be.greaterThan(0);
        solid.name = "solid_" + i;
        try {
            const jsObj = occ.buildSolidMeshNew(solid);
            let mesh = solid.mesh;
            solid.mesh.numVertices.should.be.greaterThan(3);
            nbVert += solid.mesh.numVertices;
            nbTri += solid.mesh.numTriangles;
            jsonSize += JSON.stringify(jsObj).length;
        }
        catch (err) {
            console.log(" Error in meshing" ,err )
        }
    }
    console.log(" nb Tri = ",nbTri," nbVertices = ",nbVert," size ",jsonSize);
}

read_file(function(err,shapes){

    performMesh(shapes,function() {
        console.log("done");
    })
});

