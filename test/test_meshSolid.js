const assert = require("assert");
const should = require("should");

const nodeocc = require("..");
const occ = nodeocc.occ;
const doDebug = false;

function debugLog() {
    arguments;
    /* implement me*/
}

describe("TBugLinux- testing mesh on a simple cone shape with radius2 = 0 returns 2 Faces (latteral+bottom)", function () {
    // cf. https://github.com/antonymarion/node-occ-csg-editor-display/runs/1517044830?check_suite_focus=true
    let shape, mesh;
    before(function () {
        shape = occ.makeCone([0, 0, 0], 2, [0, 0, 2], 0);
        mesh = shape.createMesh(0.1);
    });
    it("solid should have 2 faces", function () {
        const myFaces = shape.getFaces();
        console.log("shape", shape);
        console.log("cone Faces", myFaces);
        myFaces.length.should.eql(2);

        // meshing should work on Linux ! (was not working w/ Linux pre compiled occ.node)
        shape
            .hasMesh.should.be.eql(true);
        shape.faces["lateral"]
            .hasMesh
            .should.be.eql(true);
        shape.faces["bottom"]
            .hasMesh
            .should.be.eql(true);

    });
});

describe("T1- testing mesh on a simple box shape", function () {

    let shape, mesh;
    before(function () {
        shape = occ.makeBox(10, 40, 100);
        mesh = shape.createMesh(0.1);
    });
    it("solid should have 6 faces", function () {
        shape.getFaces().length.should.eql(6);
    });
    it("Mesh#vertices          - mesh should provide a vertex array", function () {
        // ----------------------------------- testing vertices
        mesh.vertices.length.should.eql(3 * 8);

        mesh.vertices[0].should.eql(0.0);
        mesh.vertices[1].should.eql(0.0);
        mesh.vertices[2].should.eql(0.0);

        mesh.vertices[3].should.eql(0.0);
        mesh.vertices[4].should.eql(0.0);
        mesh.vertices[5].should.eql(100.0);

        mesh.vertices[6].should.eql(0.0);
        mesh.vertices[7].should.eql(40.0);
        mesh.vertices[8].should.eql(100.0);

        mesh.vertices[9].should.eql(0.0);
        mesh.vertices[10].should.eql(40.0);
        mesh.vertices[11].should.eql(0.0);

        mesh.vertices[12].should.eql(10.0);
        mesh.vertices[13].should.eql(0.0);
        mesh.vertices[14].should.eql(0.0);

    });
    it("Mesh#triangle          - mesh should provide triangles indexes", function () {
        // --------------------------------------- triangles
        mesh.triangles.length.should.eql(6 * 2 * 3, "it should provide 2 triangles per face = 2 *4 *3");
    });
    it("Mesh#faceRanges        - mesh should provide a mechanism to easily identify triangle faces", function () {
        mesh.faceRanges.length.should.eql(6 * 2, "it should 6 pairs of index ( 6 faces, 2 values per face)");
        mesh.edgeRanges.length.should.eql(12 * 2, "it should 12 pairs of index ( 12 edges, 2 values per face)");

        // --------------------------------------- faces
        // testing face 1
        mesh.faceRanges[0].should.eql(0); // start in triangle index
        mesh.faceRanges[1].should.eql(2); // nb of triangles

        // testing face 2
        mesh.faceRanges[2].should.eql(2); // start in triangle index
        mesh.faceRanges[3].should.eql(2); // nb of triangles

        // testing face 3
        mesh.faceRanges[4].should.eql(4); // start in triangle index
        mesh.faceRanges[5].should.eql(2); // nb of triangles

        // testing face 4
        mesh.faceRanges[6].should.eql(6); // start in triangle index
        mesh.faceRanges[7].should.eql(2); // nb of triangles

        // testing face 5
        mesh.faceRanges[8].should.eql(8); // start in triangle index
        mesh.faceRanges[9].should.eql(2); // nb of triangles

        // testing face 6
        mesh.faceRanges[10].should.eql(10); // start in triangle index
        mesh.faceRanges[11].should.eql(2); // nb of triangles
    });
    it("Mesh#getFaceTriangles  - mesh should provide a mechanism to extract triangles indexes of a given shape face", function () {
        // --------------------------------------- face accessor
        let arr = mesh.getFaceTriangles(shape.getFaces()[0]);
        arr.should.eql(new Uint8Array([3, 1, 2, 0, 1, 3]));

        arr = mesh.getFaceTriangles(shape.getFaces()[1]);
        arr.should.eql(new Uint8Array([5, 7, 6, 5, 4, 7]));
        // etc...
    });
    it("Mesh#edgeRanges        - mesh should provide a mechanism to easily identify edges", function () {
        // edge index
        // --------------------------------------- edges
        // testing edge 1
        mesh.edgeRanges[0].should.eql(0); // start in triangle index
        mesh.edgeRanges[1].should.eql(2); // nb of triangles

        // testing edge 2
        mesh.edgeRanges[2].should.eql(2); // start in triangle index
        mesh.edgeRanges[3].should.eql(2); // nb of triangles

        // testing edge 3
        mesh.edgeRanges[4].should.eql(4); // start in triangle index
        mesh.edgeRanges[5].should.eql(2); // nb of triangles

        // testing edge 4
        mesh.edgeRanges[6].should.eql(6); // start in triangle index
        mesh.edgeRanges[7].should.eql(2); // nb of triangles

        // testing edge 5
        mesh.edgeRanges[8].should.eql(8); // start in triangle index
        mesh.edgeRanges[9].should.eql(2); // nb of triangles

        // testing edge 6
        mesh.edgeRanges[10].should.eql(10); // start in triangle index
        mesh.edgeRanges[11].should.eql(2); // nb of triangles

        // testing edge 7
        mesh.edgeRanges[12].should.eql(12); // start in triangle index
        mesh.edgeRanges[13].should.eql(2); // nb of triangles

        // testing edge 8
        mesh.edgeRanges[14].should.eql(14); // start in triangle index
        mesh.edgeRanges[15].should.eql(2); // nb of triangles

        // testing edge 9
        mesh.edgeRanges[16].should.eql(16); // start in triangle index
        mesh.edgeRanges[17].should.eql(2); // nb of triangles

        // testing edge 10
        mesh.edgeRanges[18].should.eql(18); // start in triangle index
        mesh.edgeRanges[19].should.eql(2); // nb of triangles

        // testing edge 11
        mesh.edgeRanges[20].should.eql(20); // start in triangle index
        mesh.edgeRanges[21].should.eql(2); // nb of triangles

        // testing edge 12
        mesh.edgeRanges[22].should.eql(22); // start in triangle index
        mesh.edgeRanges[23].should.eql(2); // nb of triangles

    });

    it("Mesh#getEdgeIndices    - mesh should provide a mechanism to extract the Polygon of a given edge", function () {
        //xx console.log(mesh);
        const arr = mesh.getEdgeIndices(shape.getEdges()[0]);
        arr.should.eql(new Uint8Array([0, 1]));

    });
    it("Mesh#triangleNormals    - mesh should provide ", function () {

        // face 0  - triangle 0
        mesh.triangleNormals[0].should.eql(0);
        mesh.triangleNormals[1].should.eql(0);
        mesh.triangleNormals[2].should.eql(0);
        // face 0  - triangle 1
        mesh.triangleNormals[3].should.eql(0);
        mesh.triangleNormals[4].should.eql(0);
        mesh.triangleNormals[5].should.eql(0);

        // face 1  - triangle 0
        mesh.triangleNormals[6].should.eql(1);
        mesh.triangleNormals[7].should.eql(1);
        mesh.triangleNormals[8].should.eql(1);
        // face 1  - triangle 1
        mesh.triangleNormals[9].should.eql(1);
        mesh.triangleNormals[10].should.eql(1);
        mesh.triangleNormals[11].should.eql(1);

        // face 2  - triangle 0=
        mesh.triangleNormals[12].should.eql(2);
        mesh.triangleNormals[13].should.eql(2);
        mesh.triangleNormals[14].should.eql(2);
        // face 2  - triangle 1
        mesh.triangleNormals[15].should.eql(2);
        mesh.triangleNormals[16].should.eql(2);
        mesh.triangleNormals[17].should.eql(2);

        // face 3  - triangle 0=
        mesh.triangleNormals[18].should.eql(3);
        mesh.triangleNormals[19].should.eql(3);
        mesh.triangleNormals[20].should.eql(3);
        // face 3  - triangle 1
        mesh.triangleNormals[21].should.eql(3);
        mesh.triangleNormals[22].should.eql(3);
        mesh.triangleNormals[23].should.eql(3);

        // etc...
    });
    it("Mesh#normals          - mesh should provide a normal array", function () {
        // in the case of a simple box we expect to have 6 different normals
        mesh.normals.length.should.eql(3 * 6);

        // first normal
        mesh.normals[0].should.eql(-1);
        mesh.normals[1].should.eql(0);
        mesh.normals[2].should.eql(0);

        mesh.normals[3].should.eql(1);
        mesh.normals[4].should.eql(0);
        mesh.normals[5].should.eql(0);

        mesh.normals[6].should.eql(0);
        mesh.normals[7].should.eql(-1);
        mesh.normals[8].should.eql(0);

        mesh.normals[9].should.eql(0);
        mesh.normals[10].should.eql(1);
        mesh.normals[11].should.eql(0);

        mesh.normals[12].should.eql(0);
        mesh.normals[13].should.eql(0);
        mesh.normals[14].should.eql(-1);

        mesh.normals[15].should.eql(0);
        mesh.normals[16].should.eql(0);
        mesh.normals[17].should.eql(1);
    });

});

describe("testing performance of meshing algorithms with various parameters", function () {


    this.timeout(30000);

    function makeUnitBox() {
        return occ.makeBox([0, 0, 0], [100, 100, 100]);
    }

    function makeSphere() {
        return occ.makeSphere([0, 0, 0], 100);
    }

    function makeLegoBrick() {
        return nodeocc.shapeFactory.makeLegoBrick(occ, 4, 2, "thin");
    }

    function installFor(makeShape) {

        let shape2;
        beforeEach(function () {
            shape2 = makeShape();
        });

        function test_with(tol, angle) {
            it(makeShape.name + " testing with parameter : deflection : " + tol + "  angle :" + angle, function () {
                const mesh1 = shape2.createMesh(tol, angle);
                debugLog("  vertices    = ", mesh1.vertices.length);
                debugLog("  triangles   = ", mesh1.triangles.length);
            });
        }

        test_with(0.01, 0.5);
        test_with(0.01, 5);
        test_with(0.01, 10);

        test_with(0.1, 0.5);
        test_with(0.1, 1);
        test_with(0.1, 5);
        test_with(0.1, 10);
        test_with(0.1, 20);

        test_with(1, 0.5);
        test_with(1, 1);
        test_with(1, 5);
        test_with(1, 10);
        test_with(1, 20);

        test_with(2, 0.5);
        test_with(2, 1);
        test_with(2, 5);
        test_with(2, 10);
        test_with(2, 20);

        describe(makeShape.name + " : comparing JSON ", function () {

            let shape2;
            beforeEach(function () {
                shape2 = makeUnitBox();
            });
            it("should create default JSON file with acceptable size", function () {
                shape2.name = "shape2";
                const obj1 = occ.buildSolidMesh(shape2);
                debugLog("json json1", JSON.stringify(obj1, null, "\t"));
                debugLog("json json1", JSON.stringify(obj1).length);

            });
            it("should create default JSON file with acceptable size", function () {
                shape2.name = "shape2";
                const obj2 = occ.buildSolidMeshNew(shape2);
                debugLog("json json1", JSON.stringify(obj2, null, "\t"));
                debugLog("json json2", JSON.stringify(obj2).length);

            });
        });

    }

    installFor(makeLegoBrick);
    installFor(makeSphere);

});
