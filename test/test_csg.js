var assert = require("assert");

var occ = require("../lib/occ");
var CSGbuilder =require('../lib/CSGbuilder');

var shapeFactory = require('../lib/shapeFactory');


describe("Building a csg",function() {

    describe("building a simple box",function(){
        var simple_csg = {
            "solid":  { type: "makeBox",  parameters: [[10,20,30],[40,50,70]]  }
        };
        var solid;
        before(function(){
            solid = CSGbuilder.build(simple_csg);
        });
        it("should have 6 faces",function(){
            solid.numFaces.should.equal(6);
        })
    });
    describe("building a simple cylinder",function(){
        var simple_csg = {
            "solid": { type: "makeCylinder" , parameters: [ [-20,  0, 0] ,  [ 20, 0, 0] , 4]  }
        };
        var solid;
        before(function(){
            solid = CSGbuilder.build(simple_csg);
        });
        it("should have 3 faces (top,bottom,side",function(){
            solid.numFaces.should.equal(3);
        })
    });


    describe("building the common between a box and a sphere",function() {
        var simple_csg = {
            "box": { type: "makeBox" , parameters:  [[-10,-10,-10] , [10,10,10]] },
            "sphere": { type: "makeSphere" , parameters: [[0,0,0] , 15 ] },
            "solid":  { type: "common", parameters: ["box","sphere"]  }
        };
        var solid;
        before(function(){
            solid = CSGbuilder.build(simple_csg);
        });
        it("should have  14 faces",function(){
            solid.numFaces.should.equal(14);
        })

    });

    describe("building the sample_csg_tree",function() {
        var simple_csg = CSGbuilder.sample_csg_tree ;
        var solid;
        before(function(){
            solid = CSGbuilder.build(simple_csg);
        });
        it("should have 18 faces",function(){
            solid.numFaces.should.equal(18);
        })

    });
});