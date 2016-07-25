"use strict";
var occ = require("../lib/occ");

console.log("v ", (new occ.Vertex()).toString());
var should = require("should");


describe("issue#17 testing that mesh get invalidated ", function () {


    function constructFaceWithWire() {
        var aPnt1 = [0, 0.0, 0];
        var aPnt2 = [10, 1.0, 0];
        var aPnt3 = [10, 9.0, 0];
        var aPnt4 = [0, 10.0, 0];
        var segment1 = new occ.createLine(aPnt1, aPnt2);
        var segment2 = new occ.createLine(aPnt2, aPnt3);
        var segment3 = new occ.createLine(aPnt3, aPnt4);
        var segment4 = new occ.createLine(aPnt4, aPnt1);

        var wire = new occ.Wire(segment1, segment2, segment3, segment4);
        wire.isClosed.should.equal(true);
        wire.numEdges.should.equal(4);
        wire.numVertices.should.equal(4);

        // the vector to extrude the face along.
        var face = new occ.Face(wire);

        face.getWires().length.should.eql(1);

        face.getWires()[0].getVertices().length.should.eql(4);
        return face;
    }

    it("should translate a face", function (done) {
        var face = constructFaceWithWire();

        var vertices_before = face.getWires()[0].getVertices();

        face = face.translate([20, 30, 40]);
        var vertices_after = face.getWires()[0].getVertices();

        vertices_after[0].x.should.eql(vertices_before[0].x + 20);
        vertices_after[0].y.should.eql(vertices_before[0].y + 30);
        vertices_after[0].z.should.eql(vertices_before[0].z + 40);

        vertices_after[1].x.should.eql(vertices_before[1].x + 20);
        vertices_after[1].y.should.eql(vertices_before[1].y + 30);
        vertices_after[1].z.should.eql(vertices_before[1].z + 40);

        vertices_after[2].x.should.eql(vertices_before[2].x + 20);
        vertices_after[2].y.should.eql(vertices_before[2].y + 30);
        vertices_after[2].z.should.eql(vertices_before[2].z + 40);

        vertices_after[3].x.should.eql(vertices_before[3].x + 20);
        vertices_after[3].y.should.eql(vertices_before[3].y + 30);
        vertices_after[3].z.should.eql(vertices_before[3].z + 40);

        done();
    });

    it("#17-B should provide a translated mesh when face is translated", function (done) {

        var face = constructFaceWithWire();
        face.hasMesh.should.equal(false);

        // now mesh the faces
        face.mesh
        face.hasMesh.should.equal(true);

        console.log("face mesh vertices =", face.mesh.vertices.toString());

        var vertices_before = face.mesh.vertices;
        face = face.translate([20, 30, 40]);

        console.log("face mesh vertices =", face.mesh.vertices.toString());
        var vertices_after = face.mesh.vertices;

        vertices_before.toString().should.eql("0,0,0,10,1,0,10,9,0,0,10,0");
        vertices_after.toString().should.eql("20,30,40,30,31,40,30,39,40,20,40,40");
        done();
    });
});


