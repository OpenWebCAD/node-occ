var util   = require('util');
var assert = require('assert');
var occ = require('./');

var shapeFactory = require('./lib/shapeFactory.js');

var bbox = new occ.BoundingBox();
console.log(bbox.nearPt);
console.log(bbox.nearPt.x);

//var bottle = shapeFactory.makeBottle();

function test3()
{
    solid = new occ.Solid();
    solid.makeBox([10,20,30],[20,40,60]);
    solid.fillet(solid.getEdges(),5);

}
test3();

function test2(){
    var trsf ;
    trsf=new occ.Transformation();
    trsf.makePlaneMirror([10,20,30],occ.ZDir);

    console.log(" Scale Factor=",trsf.scaleFactor);

}
test2();

function test1() {
var solid;
var shapeIt;

solid = new occ.Solid();
solid.makeBox([10,20,30],[20,40,60]);
shapeIt = new occ.ShapeIterator(solid,"FACE");
    shapeIt.more.should.be.true;
    assert(shapeIt.current === undefined);
    for (var i=0;i<12;i++) {
        shapeIt.more.should.be.true;
        shapeIt.next();
        shapeIt.current.should.not.be.undefined;
    }
    shapeIt.more.should.be.false;
    shapeIt.current.should.not.be.undefined;

}
test1();



var wire = new occ.Wire();
console.log(wire.numEdges);
console.log(wire.numVertices);

var trsf;
trsf = new occ.Transformation();
trsf.makePlaneMirror([10,20,30],occ.ZDir);




var v = new occ.Vertex();
var e = new occ.Edge().createLine([10,20,30],[10,14,15]);
console.log(e.length)

edge = new occ.Edge();
edge.createCircle([10,10,10],[0,0,1],20);


console.log("hello");

var box1 = new occ.Solid();

assert(box1.isNull);
assert(!box1.isValid);

box1.makeBox([10,20,30],[30,40,50]);

//xx for (var i in box1.mesh) {
//xx     console.log( " i=",i,"  => ", box1.mesh[i]);
//xx }
console.log("---------------!!!");

console.log( " i=",i,"  => ", box1.mesh.vertices);

assert(!box1.isNull);
assert(box1.isValid);

var box2 = new occ.Solid();
box1.makeBox([10,20,30],[100,100,100]);
var box3 = new occ.Solid();
box3.makeBox([10,20,30],100,100,100);

//xx console.log("box1.x",box1.location.x);
//xx console.log("box1.x",box1.location.y);
//xx console.log("box1.x",box1.location.z);
//xx console.log("cuboid",JSON.stringify(box1));


var solid = new occ.Solid();
solid.makeBox([10,20,30],[20,30,40]);


for ( var i in solid) {
	console.log(" i = ", i, " => " , solid[i]);
}

var expected = ["isNull","area","volume","numFaces","numSolids","isValid","rotate","fuse"];

for ( var j in occ.Solid.prototype) {
	console.log(" j = ", j, " => " ,  occ.Solid.prototype[j]);
}	


console.log("------------------");



console.log("JSON =",solid.mesh.toJSON());


occ.writeSTEP("toto.STEP",solid);

console.log("Done");
