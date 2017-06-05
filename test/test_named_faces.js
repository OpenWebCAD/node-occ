const assert = require("assert");
const should = require("should");

const occ = require("../lib/occ");

const doDebug = false;
function dumpSolid(b) {
    if (doDebug) {
        console.log(" faces    = ", b.getFaces().map(function (e) {
            return b.getShapeName(e);
        }).join(", "));
        console.log(" edges    = ", b.getEdges().map(function (e) {
            return b.getShapeName(e);
        }).join(", "));
        console.log(" vertices = ", b.getVertices().map(function (e) {
            return b.getShapeName(e);
        }).join(", "));
    }
}
// see https://npmjs.org/package/should

describe("testing face naming on simple box", function () {

    let solid;
    before(function () {
        solid = occ.makeBox([0, 0, 0], [10, 20, 30]);
    });
    it("should have only six named faces", function () {
        Object.keys(solid.faces).length.should.equal(6);
    });
    it("should have a face called 'top'", function () {
        solid.faces.should.have.property("top");
    });
    it("should have a face called 'bottom'", function () {
        solid.faces.should.have.property("bottom");
    });
    it("should have a face called 'front'", function () {
        solid.faces.should.have.property("front");
    });
    it("should have a face called 'back'", function () {
        solid.faces.should.have.property("back");
    });
    it("should have a face called 'left'", function () {
        solid.faces.should.have.property("left");
    });
    it("should have a face called 'right'", function () {
        solid.faces.should.have.property("right");
    });
    it("should provide a service to retrieve the name of its shapes", function () {
        solid.getShapeName(solid.faces.top).should.equal("top");
        solid.getShapeName(solid.faces.bottom).should.equal("bottom");
        solid.getShapeName(solid.faces.back).should.equal("back");
        solid.getShapeName(solid.faces.front).should.equal("front");
        solid.getShapeName(solid.faces.left).should.equal("left");
        solid.getShapeName(solid.faces.right).should.equal("right");
    });
    it("should return undefined when shape cannot be found", function () {
        const solid2 = occ.makeBox(10, 20, 30);
        should.exist(solid.getShapeName(solid.faces.right));
        should.not.exist(solid.getShapeName(solid2.faces.right));

    });

    it("should have 'top'    face planar and at z=30", function () {
        solid.faces.top.isPlanar.should.equal(true);
        solid.faces.top.centreOfMass.z.should.equal(30);

        solid.faces.top.centreOfMass.x.should.equal(5);
        solid.faces.top.centreOfMass.y.should.equal(10);
    });
    it("should have 'bottom' face planar and at z=0", function () {
        solid.faces.bottom.isPlanar.should.equal(true);
        solid.faces.bottom.centreOfMass.z.should.equal(0);

        solid.faces.bottom.centreOfMass.x.should.equal(5);
        solid.faces.bottom.centreOfMass.y.should.equal(10);
    });
    it("should have 'right'  face planar and at y=20", function () {
        solid.faces.right.isPlanar.should.equal(true);
        solid.faces.right.centreOfMass.y.should.equal(20);

        solid.faces.right.centreOfMass.z.should.equal(15);
        solid.faces.right.centreOfMass.x.should.equal(5);
    });
    it("should have 'left'   face planar and at y=0", function () {
        solid.faces.left.isPlanar.should.equal(true);
        solid.faces.left.centreOfMass.y.should.equal(0);

        solid.faces.left.centreOfMass.z.should.equal(15);
        solid.faces.left.centreOfMass.x.should.equal(5);
    });
    it("should have 'front'  face planar and at x=10", function () {
        solid.faces.front.isPlanar.should.equal(true);
        solid.faces.front.centreOfMass.x.should.equal(10);

        solid.faces.front.centreOfMass.y.should.equal(10);
        solid.faces.front.centreOfMass.z.should.equal(15);
    });
    it("should have 'back'   face planar and at x=0", function () {
        solid.faces.back.isPlanar.should.equal(true);
        solid.faces.back.centreOfMass.x.should.equal(0);

        solid.faces.back.centreOfMass.y.should.equal(10);
        solid.faces.back.centreOfMass.z.should.equal(15);
    });
    it("should have named edges", function () {

        const test = solid.getEdges().map(function (e) {
            return solid.getShapeName(e);
        });
        test.sort().join(" ").should.equal("EXY EXZ EXy EXz EYZ EYz ExY ExZ Exy Exz EyZ Eyz");
    });

    it("should have named vertex", function () {
        const test = solid.getVertices().map(function (e) {
            return solid.getShapeName(e);
        });
        test.sort().join(" ").should.equal("VXYZ VXYz VXyZ VXyz VxYZ VxYz VxyZ Vxyz");
    });

});
describe("testing face naming on simple sphere", function () {
    let solid;
    before(function () {
        solid = occ.makeSphere([0, 0, 0], 30);
    });
    it("should have only one named face", function () {
        Object.keys(solid.faces).length.should.equal(1);
    });
    it("should have a face called 'lateral'", function () {
        solid.faces.should.have.property("lateral");
        solid.getShapeName(solid.faces.lateral).should.equal("lateral");
    });
    it("should have not have a face called 'top' ", function () {
        solid.faces.should.not.have.property("top");
    });
});
describe("testing face naming on combined boxes", function () {
    let solid, box1, box2;
    before(function () {
        box1 = occ.makeBox([0, 0, 0], [10, 10, 10]);
        box2 = occ.makeBox([5, 5, 5], [15, 15, 15]);
        solid = occ.fuse(box1, box2);
        dumpSolid(solid);

    });
    it(" should expose 12 named faces", function () {
        Object.keys(solid.faces).length.should.equal(12);
    });
    it("should preserve bottom/left/back faces of solid1", function () {
        should.exist(solid.getShapeName(box1.faces.bottom));
        should.exist(solid.getShapeName(box1.faces.left));
        should.exist(solid.getShapeName(box1.faces.back));
    });
    it("should replace top/right/front faces of solid1", function () {
        should.not.exist(solid.getShapeName(box1.faces.top));
        should.not.exist(solid.getShapeName(box1.faces.right));
        should.not.exist(solid.getShapeName(box1.faces.front));
    });
    it("should replace bottom/left/back faces of solid2", function () {
        should.not.exist(solid.getShapeName(box2.faces.bottom));
        should.not.exist(solid.getShapeName(box2.faces.left));
        should.not.exist(solid.getShapeName(box2.faces.back));
    });
    it("should preserve top/right/front faces of solid2", function () {
        should.exist(solid.getShapeName(box2.faces.top));
        should.exist(solid.getShapeName(box2.faces.right));
        should.exist(solid.getShapeName(box2.faces.front));
    });
});
describe("testing face naming on a box whose top/right/front corner is carved with a other box", function () {
    let solid, box1, box2;
    before(function () {
        box1 = occ.makeBox([0, 0, 0], [10, 10, 10]);
        box2 = occ.makeBox([5, 5, 5], [15, 15, 15]);
        solid = occ.cut(box1, box2);
    });
    it(" should expose 9 named faces", function () {
        Object.keys(solid.faces).length.should.equal(9);
    });
    it("should preserve bottom/left/back faces of original box", function () {
        should.exist(solid.getShapeName(box1.faces.bottom));
        should.exist(solid.getShapeName(box1.faces.left));
        should.exist(solid.getShapeName(box1.faces.back));
    });
    it("should replace top/right/front faces of original box", function () {
        should.not.exist(solid.getShapeName(box1.faces.top));
        should.not.exist(solid.getShapeName(box1.faces.right));
        should.not.exist(solid.getShapeName(box1.faces.front));
    });
    it("should replace bottom/left/back faces of cutting box", function () {
        should.not.exist(solid.getShapeName(box2.faces.bottom));
        should.not.exist(solid.getShapeName(box2.faces.left));
        should.not.exist(solid.getShapeName(box2.faces.back));
    });
    it("should delete top/right/front faces of cutting box", function () {
        should.not.exist(solid.getShapeName(box2.faces.top));
        should.not.exist(solid.getShapeName(box2.faces.right));
        should.not.exist(solid.getShapeName(box2.faces.front));
    });
});
describe("testing face naming on a box with a split face ('top' face)", function () {
    let solid, block, cutting_solid;
    before(function () {
        block = occ.makeBox([0, 0, 0], [100, 100, 25]);
        cutting_solid = occ.makeBox([40, -10, 10], [60, 110, 50]);
        solid = occ.cut(block, cutting_solid);
    });
    it(" should expose 10 named faces", function () {
        Object.keys(solid.faces).length.should.equal(10);
        solid.getFaces().length.should.equal(10);
    });
    it("should have a preserved front/bottom/back faces ", function () {
        should.exist(solid.getShapeName(block.faces.front));
        should.exist(solid.getShapeName(block.faces.bottom));
        should.exist(solid.getShapeName(block.faces.back));
    });
    it("should have modified right/top/left faces ", function () {
        should.not.exist(solid.getShapeName(block.faces.right));
        should.not.exist(solid.getShapeName(block.faces.top));
        should.not.exist(solid.getShapeName(block.faces.left));
    });


});
describe("testing face naming on a box fused with a box that have a common face , leading to 4 merging faces", function () {
    let box1;
    let box2;
    let solid;
    before(function () {
        //        +------+       +------+
        //        |`.     `.     |\      \
        //        |  `+------+   | +------+
        //        |   |      |   | |      |
        //        +   |      |   + |      |
        //         `. |      |    \|      |
        //           `+------+     +------+
        //
        box1 = occ.makeBox([0, 0, 0], [10, 10, 10]);
        box2 = occ.makeBox([10, 0, 0], [20, 10, 10]); // box2 back face is same as box1.front frace
        solid = occ.fuse(box1, box2);
    });
    it("should expose 10 faces", function () {
        //
        //   +----++----+        +----++----+
        //   |    ||    |        |          |
        //   |    ||    |   =>   |          |
        //   +----++----+        +----++----+
        //
        // Although the side surface could be merged
        //
        Object.keys(solid.faces).length.should.equal(10);
        solid.getFaces().length.should.equal(10);
        occ.gc();
        const faces = solid.getFaces();
        faces.forEach((face, i) => {
            const bbox = face.getBoundingBox();
            console.log("face i ", i, solid.getShapeName(face), bbox.toString());
        });
    });
});
describe("testing face naming on a box with a top face split twice leading to 4 isolated corners)", function () {
    //
    // // in this sample, the top face of the block will be split in two pieces
    // during the first cut operation.Then  each part will be split in two pieces again
    // during the second cut operation.
    //
    let solid, block, cutting_solid1, cutting_solid2;
    before(function () {
        block = occ.makeBox([0, 0, 0], [100, 100, 25]);
        cutting_solid1 = occ.makeBox([40, -10, 10], [60, 110, 50]);
        cutting_solid2 = occ.makeBox([-10, 40, 10], [110, 60, 50]);
        solid = occ.cut(block, cutting_solid1);
        solid = occ.cut(solid, cutting_solid2);
    });
    it("should expose 22 named faces", function () {
        Object.keys(solid.faces).length.should.equal(22);
        solid.getFaces().length.should.equal(22);
    });
    it("should have a preserved bottom face", function () {
        should.exist(solid.getShapeName(block.faces.bottom));
    });
    it("should have modified front/back/right/top/left faces ", function () {
        should.not.exist(solid.getShapeName(block.faces.front));
        should.not.exist(solid.getShapeName(block.faces.back));
        should.not.exist(solid.getShapeName(block.faces.right));
        should.not.exist(solid.getShapeName(block.faces.top));
        should.not.exist(solid.getShapeName(block.faces.left));
    });


});
describe("testing face naming on a box with a top face split by a cross shape leading to 4 isolated corners", function () {
    //
    // in this sample, the top face of the block will be split in 4 pieces
    // during the single cut operation
    //
    let solid, block, cutting_solid1, cutting_solid2, cutting_solid;
    before(function () {
        block = occ.makeBox([0, 0, 0], [100, 100, 25]);
        cutting_solid1 = occ.makeBox([40, -10, 10], [60, 110, 50]);
        cutting_solid2 = occ.makeBox([-10, 40, 10], [110, 60, 50]);
        cutting_solid = occ.fuse(cutting_solid1, cutting_solid2);
        solid = occ.cut(block, cutting_solid);
    });
    it("should expose 22 named faces", function () {
        Object.keys(solid.faces).length.should.equal(22);
        solid.getFaces().length.should.equal(22);
    });
    it("should have a preserved bottom face", function () {
        should.exist(solid.getShapeName(block.faces.bottom));
    });
    it("should have modified front/back/right/top/left faces ", function () {
        should.not.exist(solid.getShapeName(block.faces.front));
        should.not.exist(solid.getShapeName(block.faces.back));
        should.not.exist(solid.getShapeName(block.faces.right));
        should.not.exist(solid.getShapeName(block.faces.top));
        should.not.exist(solid.getShapeName(block.faces.left));

        solid.faces.should.have.property("m1:" + "front" + ":0");
        solid.faces.should.have.property("m1:" + "back" + ":0");
        solid.faces.should.have.property("m1:" + "top" + ":0");
        solid.faces.should.have.property("m1:" + "left" + ":0");

        solid.faces.should.not.have.property("m1:" + "front" + ":1");
        solid.faces.should.not.have.property("m1:" + "back" + ":1");
        solid.faces.should.not.have.property("m1:" + "left" + ":1");

    });

    it("should have 4 (and only 4) faces that have been generated from the top face of the original block", function () {
        // this could be tested using face names
        const name = block.getShapeName(block.faces.top);

        solid.faces.should.have.property("m1:" + name + ":0");
        solid.faces.should.have.property("m1:" + name + ":1");
        solid.faces.should.have.property("m1:" + name + ":2");
        solid.faces.should.have.property("m1:" + name + ":3");
        solid.faces.should.not.have.property("m1:" + name + ":4");
    });
});

describe("testing naming with makeFillet operation", function () {


    let solid;
    before(function () {
        solid = occ.makeBox([10, 20, 30], [100, 200, 300]);
        dumpSolid(solid);

        const edges = solid.getCommonEdges(solid.faces.front, solid.faces.left);
        assert(edges instanceof Array);

        solid = occ.makeFillet(solid, edges[0], 10);
        dumpSolid(solid);

    });
    it("should have numFaces with expected number of faces", function () {
        //xx console.log(Object.keys(solid.faces).join(", "));
        Object.keys(solid.faces).length.should.equal(solid.numFaces);
    });
});
