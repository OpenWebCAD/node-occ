const assert = require("assert");
const should = require("should");

const occ = require("../lib/occ");

// see https://npmjs.org/package/should

describe("testing face mesh ",function() {

    it("should not have a mesh unless the parent solid has been meshed", function(){

        let solid = occ.makeBox([0,0,0],[10,10,10]);
        solid.faces.should.have.property("top");
        let topFace = solid.faces.top;
        should.exist(topFace);

        //xx topFace.area.should.equal(100.0);

        topFace.hasMesh.should.equal(false);
        // now mesh the solid
        let m = solid.mesh;
        topFace.hasMesh.should.equal(true);

        m.vertices.length.should.eql(72);
        m.normals.length.should.eql(72);
        m.edgeindices.length.should.eql(24);
        m.triangles.length.should.eql(36);

        console.log(topFace.mesh.toJSON());

        let faceMesh = topFace.mesh;
        faceMesh.vertices.length.should.eql(3 * 4);
        faceMesh.normals.length.should.eql(3 * 4);
        faceMesh.edgeindices.length.should.eql(2 * 4);
        faceMesh.triangles.length.should.eql(2 * 3);

    });
});

describe("testing face#getWire ", function () {


    it("#getWire", function () {

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
