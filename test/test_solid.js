var assert = require("assert");
var should = require("should");

var occ = require("../build/Release/occ");


describe("testing solid construction",function(){


	describe("empty solid", function() {
	    var solid;
		before(function() {
			solid = new occ.Solid();		
		});
		it("should have no faces", function() { 
			solid.numFaces.should.equal(0);	
		});
		it("should have no solid", function() { 
			solid.numSolids.should.equal(0);
		});
	});
	describe("makeBox", function() {
	    var solid;
		before(function() {
			solid = new occ.Solid();		
			solid.makeBox([10,20,30],[20,40,60]);
		});
		it("should have 6 faces", function() { 
			solid.numFaces.should.equal(6);
		});
		it("should have 1 solid", function() { 
			solid.numSolids.should.equal(1);
		});
	});

	describe("fuse 2 overlapping boxes", function() {
	    var solid1;
	    var solid2;
		before(function() {
			solid1 = new occ.Solid();		
			solid1.makeBox([10,20,30],[20,40,60]);
			solid2 = new occ.Solid();		
			solid2.makeBox([15,25,35],[-20,-40,-60]);
			
			solid1.fuse(solid2);

		});
		it("should have 12 faces", function() { 
			solid1.numFaces.should.equal(12);
		});
		it("should have 1 solid", function() { 
			solid1.numSolids.should.equal(1);
		});
	});

});
