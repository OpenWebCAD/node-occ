var assert = require("assert");
var should = require("should");

var occ = require("../lib/occ");

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
			solid = occ.makeBox([10,20,30],[20,40,60]);
		});
		it("should have 6 faces", function() { 
			solid.numFaces.should.equal(6);
		});
		it("should have 1 solid", function() { 
			solid.numSolids.should.equal(1);
		});
		it("should have a shapeType beeing 'SOLID' ", function() { 
			solid.shapeType.should.equal("SOLID");
		});
		it("should have (20-10)*(40-20)*(60-30) as a volume", function() { 
			solid.volume.should.equal( (20-10)*(40-20)*(60-30));
		});

		it("should have ~ 2*((20-10)*(40-20)+(20-10)*(60-30)+(40-20)*(60-30)) as a area", function() {
			var expectedArea = 2*((20-10)*(40-20)+(20-10)*(60-30)+(40-20)*(60-30));
			var eps = 0.001;
			solid.area.should.be.within( expectedArea - eps, expectedArea+eps );
		});

        it("should have the sum(face area) ===  area of solid ",function() {

            var epsilon = 1E-3;

            var shapeIt = new occ.ShapeIterator(solid,"FACE");
            var cumulated_face_area = 0;
            while(shapeIt.more) {
                cumulated_face_area += shapeIt.next().area
            }
            var expectedArea = solid.area;
            cumulated_face_area.should.be.within(expectedArea -epsilon,expectedArea+epsilon )
        })
	});
	describe("fuse 2 overlapping boxes", function() {
	    var solid1;
	    var solid2;
		before(function() {
			solid1 = occ.makeBox([10,20,30],[20,40,60]);
			solid2 = occ.makeBox([15,25,35],[-20,-40,-60]);
			solid1 = occ.fuse(solid1,solid2);
		});
		it("should have 12 faces", function() { 
			solid1.numFaces.should.equal(12);
		});
		it("should have 1 solid", function() { 
			solid1.numSolids.should.equal(1);
		});
	});
	describe("cut a corner of a box", function() {
	    var solid1;
	    var solid2;
		before(function() {
			solid1 = occ.makeBox([10,20,30],[20,40,60]);
			solid2 = occ.makeBox([15,25,35],[-20,-40,-60]);
			
			solid1 = occ.cut(solid1,solid2);

		});
		it("should have 9 faces", function() { 
			solid1.numFaces.should.equal(9);
		});
		it("should have 1 solid", function() { 
			solid1.numSolids.should.equal(1);
		});
	});
    describe("Meshing a simple solid", function() {
        describe("Meshing a box", function() {
            var solid;
            before(function(){
                solid = occ.makeBox([10,20,30],[20,30,40]);
            });
            it("should have a mesh with 4*6 vertices", function() {
                solid.mesh.numVertices.should.equal(24);
            });
            it("should have a mesh with (2*3)*4 edges", function() {
                solid.mesh.numEdges.should.equal(24);
            });
            it("should have a mesh with 2*6 triangles", function() {
                solid.mesh.numTriangles.should.equal(12);
            });
        });

    });
	describe("Testing  Shape __prototype", function() {
		 var solid;
            before(function(){
                solid = occ.makeBox([10,20,30],[20,30,40]);
            });
			it("should expose the expected properties ", function() {
			   var expected = ["shapeType","numFaces","isNull","isValid"];
			   var actual = []
               for ( var j in occ.Solid.prototype) {
			      actual.push(j.toString())	
			   }
			   // console.log(actual);
			   var missing = []
               for ( var j in expected) {			   
			       if (actual.indexOf(expected[j]) == -1) {
				    missing.push(expected[j]);
				   }
			   }
			   missing.should.be.empty;

            });
	});
    describe("exporting a solid to STEP ", function() {
        var solid1,solid2;
        before(function(){
            solid1 = occ.makeBox([10,20,30],[20,30,40]);
            solid1 = occ.makeBox([20,30,50],[110,40,00]);
        });
        it("should export a single solid to STEP", function() {
            occ.writeSTEP("toto1.step",solid1);
        });
        it("should export many solids to STEP", function() {
            occ.writeSTEP("toto2.step",solid1,solid2);
        });
    });
    describe("testing ShapeIterator on solid", function() {
        var solid;
        var shapeIt;
        before(function() {
            solid = occ.makeBox([10,20,30],[20,40,60]);
        });
        it("should iterate on 6 faces", function() {

            shapeIt = new occ.ShapeIterator(solid,"FACE");
            shapeIt.more.should.be.true;
            assert(shapeIt.current === undefined);
            var counter =0;
            while (shapeIt.more) {
                shapeIt.more.should.be.true;
                shapeIt.next();
                shapeIt.current.should.not.be.undefined;
                counter+=1;
            }
            counter.should.equal(6);
            shapeIt.more.should.be.false;
            shapeIt.current.should.not.be.undefined;

        });
        it("should iterate on 24 edges ( 4 on each of the 6 faces", function() {
            shapeIt = new occ.ShapeIterator(solid,"EDGE");
            shapeIt.more.should.be.true;
            assert(shapeIt.current === undefined);
            var counter =0;
            while (shapeIt.more) {
                shapeIt.more.should.be.true;
                shapeIt.next();
                shapeIt.current.should.not.be.undefined;
                counter+=1;
            }
            counter.should.equal(24);
            shapeIt.more.should.be.false;
            shapeIt.current.should.not.be.undefined;

        });

    });
    describe("testing fillet on a box..",function(){
        var solid;
        before(function(){
            solid = occ.makeBox([10,20,30],[30,40,50]);

        });
        it("should be possible to round the corner...",function(){
            solid.numFaces.should.equal(6);
            solid.fillet(solid.getEdges(),2.0);
            //    6 flat surfaces       -> 6*4  edges
            // + 12 rounded corners     -> shared
            // + 8  corners             -> 8*3   edges
            //==> 26 faces
            solid.numFaces.should.be.equal(26);
            solid.getEdges().length.should.be.equal(6*4+8*3);

        });
    });
    describe("makeCylinder (variation 1)",function(){
        var solid;
        before(function(){
            var radius = 50;
            var height = 100;
            solid = occ.makeCylinder(radius,height);
        })
        it("should have 3 faces", function() {
            solid.numFaces.should.equal(3);
        })
    })

    describe("makeCylinder (variation 2)",function(){
        var solid;
        before(function(){
            var position = [ [0,0,1] , [0,1,0] ];
            var radius = 50;
            var height = 100;
            solid = occ.makeCylinder(position ,radius,height);
        })
        it("should have 3 faces", function() {
            solid.numFaces.should.equal(3);
        })
    })
    describe("makeCylinder (variation 3 : with 2 points and a height)",function(){
        var solid;
        before(function(){
            var startPoint = [10,20,40];
            var endPoint   = [20,30,40];
            var height = 100;
            solid = occ.makeCylinder(startPoint ,endPoint,height);
        })
        it("should have 3 faces", function() {
            solid.numFaces.should.equal(3);
        })
    })
    describe("makeCone",function(){
        var solid;
        before(function(){
            var radius1 = 50;
            var radius2 = 70;
            var height =  30;
            solid = occ.makeCone(radius1,radius2,height);
        })
        it("should have 3 faces", function() {
            solid.numFaces.should.equal(3);
        })
    })
    describe("makeSphere",function(){
        var solid;
        before(function(){
            var center = [10,20,30];
            var radius = 70;
            solid = occ.makeSphere(center,radius);
        })
        it("should have 1 face and one egde", function() {
            solid.numFaces.should.equal(1);
            solid.getEdges().length.should.equal(1)
            var edges = solid.getEdges()
            for ( var edge in edges) {
                // todo : do some investigation
            }
        })
    })
});