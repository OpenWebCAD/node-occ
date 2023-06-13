const util = require('util');
const assert = require('assert');

const { occ, shapeFactory, ZDir } = require('./');


function testOuterShell() {
    let solid1 = occ.makeBox([10, 20, 30], [20, 40, 60]);
    let solid2 = occ.makeBox([15, 25, 35], [-20, -40, -60]);
    let solid = occ.fuse(solid1, solid2);


    solid1 = occ.makeBox([0, 0, 0], [20, 20, 20]);
    solid2 = occ.makeBox([10, 10, 10], [15, 15, 15]);
    solid = occ.cut(solid1, solid2);

    solid.getOuterShell();

    const shells = solid.getShells();
    for (const i in shells) {
        const shell = shells[i];
        console.log(shell.shapeType, shell.numFaces);
    }
}
testOuterShell();

console.log("-------------------");

function testCyl() {
    const cyl = occ.makeCylinder([-100, 20, 30], [100, 20, 30], 40);
}
testCyl();



function testCSG1() {
    const solid = shapeFactory.makePan(occ);
}
testCSG1();


const bbox = new occ.BoundingBox();
console.log(bbox.nearPt);
console.log(bbox.nearPt.x);

//const bottle = shapeFactory.makeBottle();

function test3() {
    let solid = occ.makeBox([10, 20, 30], [20, 40, 60]);
    solid = occ.makeFillet(solid, solid.getEdges(), 2);

}
test3();

function test2() {
    const trsf = new occ.Transformation();
    trsf.makePlaneMirror([10, 20, 30], ZDir);
    console.log(" Scale Factor=", trsf.scaleFactor);

}
test2();

function test1() {
    const solid = occ.makeBox([10, 20, 30], [20, 40, 60]);
    const shapeIt = new occ.ShapeIterator(solid, "FACE");
    assert(shapeIt.more);
    assert(shapeIt.current === undefined);
}
test1();


const wire = new occ.Wire();
console.log(wire.numEdges);
console.log(wire.numVertices);

const trsf = new occ.Transformation();
trsf.makePlaneMirror([10, 20, 30], ZDir);

const v = occ.makeVertex();
const e = new occ.Edge().createLine([10, 20, 30], [10, 14, 15]);
console.log(e.length)

edge = new occ.Edge();
edge.createCircle([10, 10, 10], [0, 0, 1], 20);


console.log("hello");

const box1 = new occ.Solid();

assert(box1.isNull);
assert(!box1.isValid);

box1 = occ.makeBox([10, 20, 30], [30, 40, 50]);

//xx for (const i in box1.mesh) {
//xx     console.log( " i=",i,"  => ", box1.mesh[i]);
//xx }
console.log("---------------!!!");

console.log(" i=", i, "  => ", box1.mesh.vertices);

assert(!box1.isNull);
assert(box1.isValid);

const box2 = occ.makeBox([10, 20, 30], [100, 100, 100]);
const box3 = occ.makeBox([10, 20, 30], 100, 100, 100);

//xx console.log("box1.x",box1.location.x);
//xx console.log("box1.x",box1.location.y);
//xx console.log("box1.x",box1.location.z);
//xx console.log("cuboid",JSON.stringify(box1));


const solid = occ.makeBox([10, 20, 30], [20, 30, 40]);


for (const i in solid) {
    console.log(" i = ", i, " => ", solid[i]);
}

const expected = ["isNull", "area", "volume", "numFaces", "numSolids", "isValid", "rotate", "fuse"];

for (const j in occ.Solid.prototype) {
    console.log(" j = ", j, " => ", occ.Solid.prototype[j]);
}


console.log("------------------");



console.log("JSON =", solid.mesh.toJSON());


occ.writeSTEP("toto.STEP", solid);

console.log("Done");
