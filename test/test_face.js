const assert = require("assert");
const should = require("should");

const occ = require("../lib/occ");

// see https://npmjs.org/package/should

describe("testing face mesh ",function() {

    it("Face#mesh - should not have a mesh unless the parent solid has been meshed", function () {

        // given a box solid
        let solid = occ.makeBox([0,0,0],[10,10,10]);

        solid.faces.should.have.property("top");

        let topFace = solid.faces.top;
        should.exist(topFace);

        topFace.area.should.be.within(99.99, 100.0, "face area shall be 100.00");

        topFace.hasMesh.should.equal(false);

        // now mesh the solid
        let m = solid.mesh;
        m.vertices.length.should.eql(8 * 3, "expecting 8 vertices (made of 3 floats) in solid mesh");
        m.edgeIndices.length.should.eql(24);
        m.triangles.length.should.eql(36);

        //xx console.log(m.toJSON());

        // meshing the solid should cause each of its faces to be meshed
        topFace.hasMesh.should.equal(true, "Face must have a mesh when parent solid is meshed");

        //xx m.normals.length.should.eql(72);
        //xx console.log(topFace.mesh.toJSON());
        let faceMesh = topFace.mesh;

        faceMesh.vertices.length.should.eql(3 * 4, "we expect 4 points ");
        //xx faceMesh.normals.length.should.eql(3 * 4);
        faceMesh.edgeIndices.length.should.eql(2 * 4);
        faceMesh.triangles.length.should.eql(2 * 3);

    });
});

describe("testing face#getWire ", function () {


    it("Face#getWire", function () {

        let solid = occ.makeBox([0, 0, 0], [10, 10, 10]);
        solid.faces.should.have.property("top");
        let topFace = solid.faces.top;
        should.exist(topFace);

        let wires = topFace.getWires();
        wires.length.should.eql(1);

        wires[0].getEdges().length.should.eql(4);

        wires[0].getEdges()[0].getVertices()[0].should.eql(new occ.Vertex({x: 0, y: 10, z: 10}));


    });
});
