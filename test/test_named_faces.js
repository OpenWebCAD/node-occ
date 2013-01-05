var assert = require("assert");
var should = require("should");

var occ = require("../lib/occ");

// see https://npmjs.org/package/should

describe("testing face naming on simple box",function() {

    var solid;
    before(function(){
        solid = occ.makeBox([0,0,0],[10,20,30]);
        // console.log( solid.faces);
    });
    it("should have only six named faces",function(){
        Object.keys(solid.faces).length.should.equal(6);
    });
    it("should have a face called 'top'",function(){
        solid.faces.should.have.property("top");
    });
    it("should have a face called 'bottom'",function(){
        solid.faces.should.have.property("bottom");
    });
    it("should have a face called 'front'",function(){
        solid.faces.should.have.property("front");
    });
    it("should have a face called 'back'",function(){
        solid.faces.should.have.property("back");
    });
    it("should have a face called 'left'",function(){
        solid.faces.should.have.property("left");
    });
    it("should have a face called 'right'",function(){
        solid.faces.should.have.property("right");
    });
    it("should provide a service to retrieve the name of its shapes",function(){
        solid.getShapeName(solid.faces.top).should.equal("top");
        solid.getShapeName(solid.faces.bottom).should.equal("bottom");
        solid.getShapeName(solid.faces.back).should.equal("back");
        solid.getShapeName(solid.faces.front).should.equal("front");
        solid.getShapeName(solid.faces.left).should.equal("left");
        solid.getShapeName(solid.faces.right).should.equal("right");
    });
    it("should return undefined when shape cannot be found",function(){
        var solid2 = occ.makeBox(10,20,30);
        should.exist(solid.getShapeName(solid.faces.right));
        should.not.exist(solid.getShapeName(solid2.faces.right));

    });

    it("should have 'top'    face planar and at z=30",function(){
        solid.faces.top.isPlanar.should.equal(true);
        solid.faces.top.centreOfMass.z.should.equal(30);

        solid.faces.top.centreOfMass.x.should.equal(5);
        solid.faces.top.centreOfMass.y.should.equal(10);
    });
    it("should have 'bottom' face planar and at z=0",function(){
        solid.faces.bottom.isPlanar.should.equal(true);
        solid.faces.bottom.centreOfMass.z.should.equal(0);

        solid.faces.bottom.centreOfMass.x.should.equal(5);
        solid.faces.bottom.centreOfMass.y.should.equal(10);
    });
    it("should have 'right'  face planar and at y=20",function(){
        solid.faces.right.isPlanar.should.equal(true);
        solid.faces.right.centreOfMass.y.should.equal(20);

        solid.faces.right.centreOfMass.z.should.equal(15);
        solid.faces.right.centreOfMass.x.should.equal(5);
    });
    it("should have 'left'   face planar and at y=0",function(){
        solid.faces.left.isPlanar.should.equal(true);
        solid.faces.left.centreOfMass.y.should.equal(0);

        solid.faces.left.centreOfMass.z.should.equal(15);
        solid.faces.left.centreOfMass.x.should.equal(5);
    });
    it("should have 'front'  face planar and at x=10",function(){
        solid.faces.front.isPlanar.should.equal(true);
        solid.faces.front.centreOfMass.x.should.equal(10);

        solid.faces.front.centreOfMass.y.should.equal(10);
        solid.faces.front.centreOfMass.z.should.equal(15);
    });
    it("should have 'back'   face planar and at x=0",function(){
        solid.faces.back.isPlanar.should.equal(true);
        solid.faces.back.centreOfMass.x.should.equal(0);

        solid.faces.back.centreOfMass.y.should.equal(10);
        solid.faces.back.centreOfMass.z.should.equal(15);
    });
});
describe("testing face naming on simple sphere",function() {
    var solid;
    before(function(){
        solid = occ.makeSphere([0,0,0],30);
        // console.log( "faces = ",solid.faces);
    });
    it("should have only one named face",function(){
        Object.keys(solid.faces).length.should.equal(1);
    });
    it("should have a face called 'lateral'",function(){
        solid.faces.should.have.property("lateral");
        solid.getShapeName(solid.faces.lateral).should.equal("lateral");
    });
    it("should have not have a face called 'top' ",function(){
        solid.faces.should.not.have.property("top");
    });
});
describe("testing face naming on combined boxes",function() {
   var solid,box1,box2;
   before(function(){
      box1 = occ.makeBox([0,0,0],[10,10,10]);
      box2 = occ.makeBox([5,5,5],[15,15,15]);
      solid = occ.fuse(box1,box2);
       console.log(solid.faces);
   });
   it(" should expose 12 named faces", function() {
        Object.keys(solid.faces).length.should.equal(12);
    });
    it("should preserve bottom/left/back faces of solid1",function() {
        should.exist(solid.getShapeName(box1.faces.bottom));
        should.exist(solid.getShapeName(box1.faces.left));
        should.exist(solid.getShapeName(box1.faces.back));
    });
    it("should replace top/right/front faces of solid1",function() {
        should.not.exist(solid.getShapeName(box1.faces.top));
        should.not.exist(solid.getShapeName(box1.faces.right));
        should.not.exist(solid.getShapeName(box1.faces.front));
    });
    it("should replace bottom/left/back faces of solid2",function() {
        should.not.exist(solid.getShapeName(box2.faces.bottom));
        should.not.exist(solid.getShapeName(box2.faces.left));
        should.not.exist(solid.getShapeName(box2.faces.back));
    });
    it("should preserve top/right/front faces of solid2",function() {
        should.exist(solid.getShapeName(box2.faces.top));
        should.exist(solid.getShapeName(box2.faces.right));
        should.exist(solid.getShapeName(box2.faces.front));
    });
});
describe("testing face naming on a box whose top/right/front corner is carved with a other box",function() {
    var solid,box1,box2;
    before(function(){
        box1 = occ.makeBox([0,0,0],[10,10,10]);
        box2 = occ.makeBox([5,5,5],[15,15,15]);
        solid = occ.cut(box1,box2);
        console.log(solid.faces);
    });
    it(" should expose 9 named faces", function() {
        Object.keys(solid.faces).length.should.equal(9);
    });
    it("should preserve bottom/left/back faces of original box",function() {
        should.exist(solid.getShapeName(box1.faces.bottom));
        should.exist(solid.getShapeName(box1.faces.left));
        should.exist(solid.getShapeName(box1.faces.back));
    });
    it("should replace top/right/front faces of original box",function() {
        should.not.exist(solid.getShapeName(box1.faces.top));
        should.not.exist(solid.getShapeName(box1.faces.right));
        should.not.exist(solid.getShapeName(box1.faces.front));
    });
    it("should replace bottom/left/back faces of cutting box",function() {
        should.not.exist(solid.getShapeName(box2.faces.bottom));
        should.not.exist(solid.getShapeName(box2.faces.left));
        should.not.exist(solid.getShapeName(box2.faces.back));
    });
    it("should delete top/right/front faces of cutting box",function() {
        should.not.exist(solid.getShapeName(box2.faces.top));
        should.not.exist(solid.getShapeName(box2.faces.right));
        should.not.exist(solid.getShapeName(box2.faces.front));
    });
});
