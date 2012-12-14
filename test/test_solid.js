var assert = require("assert");
var should = require("should");

var occ = require("../build/Release/occ");

// see https://npmjs.org/package/should

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
	describe("makeBox with 2 points", function() {
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
		it("should have (20-10)*(40-20)*(60-30) as a volume", function() { 
			solid.volume.should.equal( (20-10)*(40-20)*(60-30));
		});
		it("should have ~ 2*((20-10)*(40-20)+(20-10)*(60-30)+(40-20)*(60-30)) as a area", function() { 
			
			var expectedArea = 2*((20-10)*(40-20)+(20-10)*(60-30)+(40-20)*(60-30));
			var eps = 0.001;
			solid.area.should.be.within( expectedArea - eps, expectedArea+eps );
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
	describe("cut a corner of a boxe", function() {
	    var solid1;
	    var solid2;
		before(function() {
			solid1 = new occ.Solid();		
			solid1.makeBox([10,20,30],[20,40,60]);
			solid2 = new occ.Solid();		
			solid2.makeBox([15,25,35],[-20,-40,-60]);
			
			solid1.cut(solid2);

		});
		it("should have 9 faces", function() { 
			solid1.numFaces.should.equal(9);
		});
		it("should have 1 solid", function() { 
			solid1.numSolids.should.equal(1);
		});
	});
});
