var assert = require("assert");
var should = require("should");

var occ = require("../lib/occ");





describe("testing Edges ",function(){

    describe("constructing an empty Edge",function(){
        var edge;
        before(function(){
          edge = new occ.Edge();
        })
        it("should be valid", function(){
            edge.isNull.should.be.true;
        });
        it("should have a zero length", function(){
            edge.length.should.equal(0);
        });
        it("should have a zero vertices", function(){
            edge.numVertices.should.equal(0);
        });
        it("should be degenerated", function(){
            edge.isDegenerated.should.be.true;
        });
        it("shouldn't be closed", function(){
            edge.isClosed.should.be.false;
        });
    });
    describe("an  Edge constructed as  as a linear Segment between (10,20,30) and (-30,20,30) ",function(){
        var edge;
        before(function(){
            var v1 = new occ.Vertex(10,20,30);
            var v2 = new occ.Vertex(-30,20,30);
            edge = new occ.Edge();
            edge.createLine(v1,v2);

        })
        it("should have a length of 40.0 ", function(){
            edge.length.should.equal(40.0);
        });
        it("should have two vertices ", function(){
            edge.numVertices.should.equal(2.0);
        });
        it("shouldn't be closed", function(){
            edge.isClosed.should.be.false;
        });
        it("shouldn't be degenerated", function(){
            edge.isDegenerated.should.be.false;
        });
    });
    describe("an  Edge constructed as a Circle on the Z+ plan with a radius of 20" ,function(){
        var edge;
        before(function(){

            edge = new occ.Edge();
            edge.createCircle([10,10,10],[0,0,1],20);

        })
        it("should have a length of 2*PI*20.0 ", function(){
            var epsilon = 1E-2;
            var PI = 3.1415;
            edge.length.should.be.within(2*PI*20.0-epsilon,2*PI*20.0+epsilon);
        });
        it("should have a unique vertex ", function(){
            edge.numVertices.should.equal(1);
        });
        it("should be closed", function(){
            edge.isClosed.should.be.true;
        });
        it("shouldn't be degenerated", function(){
            edge.isDegenerated.should.be.false;
        });
    });
});
