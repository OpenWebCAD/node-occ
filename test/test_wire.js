
const assert = require("assert");
const should = require("should");

const occ = require("../lib/occ");


describe("testing Wire ",function(){

    describe("empty wire", function() {
        let wire;
        before(function() {
            wire = new occ.Wire();
        });
        it("should have no edges", function() {
            wire.numEdges.should.equal(0);
        });
        it("should have no vertex", function() {
            wire.numVertices.should.equal(0);
        });
        it("should not be closed", function() {
            wire.isClosed.should.equal(false);
        });
    });
    describe("wire with three segments", function () {

        let edge1, edge2, edge3;
        let wire;
        before(function () {
            let v1 = new occ.Vertex(0, 0, 0);
            let v2 = new occ.Vertex(10, 10, 0);
            let v3 = new occ.Vertex(20, 0, 0);
            edge1 = occ.makeLine(v1, v2);
            edge2 = occ.makeLine(v2, v3);
            edge3 = occ.makeLine(v3, v1);
            wire = new occ.Wire(edge1, edge2, edge3);
        });

        it("should have three edges", function () {
            wire.numEdges.should.equal(3);
        });

        it("should have three vertive", function () {
            wire.numVertices.should.equal(3);
        });

        it("should  be closed", function () {
            wire.isClosed.should.equal(true);
        });
    });

});



