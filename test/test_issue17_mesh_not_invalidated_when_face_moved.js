"use strict";
const occ = require("../lib/occ");
const should = require("should");

const doDebug = false;
function debugLog() {
    if (doDebug) {
        console.log.apply(console, arguments);
    }
}
describe("issue#17 testing that mesh get invalidated ", function () {


    function constructFaceWithWire() {
        const aPnt1 = [0, 0.0, 0];
        const aPnt2 = [10, 1.0, 0];
        const aPnt3 = [10, 9.0, 0];
        const aPnt4 = [0, 10.0, 0];
        const segment1 = new occ.makeLine(aPnt1, aPnt2);
        const segment2 = new occ.makeLine(aPnt2, aPnt3);
        const segment3 = new occ.makeLine(aPnt3, aPnt4);
        const segment4 = new occ.makeLine(aPnt4, aPnt1);

        const wire = new occ.Wire(segment1, segment2, segment3, segment4);
        wire.isClosed.should.equal(true);
        wire.numEdges.should.equal(4);
        wire.numVertices.should.equal(4);

        // the vector to extrude the face along.
        const face = new occ.Face(wire);

        face.getWires().length.should.eql(1);

        face.getWires()[0].getVertices().length.should.eql(4);
        return face;
    }

    it("should translate a face", function (done) {
        let face = constructFaceWithWire();

        const vertices_before = face.getWires()[0].getVertices();

        face = face.translate([20, 30, 40]);
        const vertices_after = face.getWires()[0].getVertices();

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

        let face = constructFaceWithWire();
        face.hasMesh.should.equal(false);

        // now mesh the faces
        face.createMesh(0.1);
        face.hasMesh.should.equal(true);

        debugLog("face mesh vertices =", face.mesh.vertices.toString());


        const vertices_before = face.mesh.vertices;
        face = face.translate([20, 30, 40]);

        debugLog("face mesh vertices =", face.mesh.vertices.toString());

        const vertices_after = face.mesh.vertices;

        vertices_before.toString().should.eql("0,0,0,10,1,0,10,9,0,0,10,0");
        vertices_after.toString().should.eql("20,30,40,30,31,40,30,39,40,20,40,40");
        done();
    });
});


