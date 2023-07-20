"use strict";
var __importDefault = (this && this.__importDefault) || function (mod) {
    return (mod && mod.__esModule) ? mod : { "default": mod };
};
Object.defineProperty(exports, "__esModule", { value: true });
const should_1 = __importDefault(require("should"));
const __1 = require("..");
const DEG2RAD = Math.PI / 180;
// see https://npmjs.org/package/should
describe("testing solid construction", () => {
    describe("empty solid", () => {
        let solid;
        before(() => {
            solid = new __1.Solid();
        });
        it("should have no faces", () => {
            solid.numFaces.should.equal(0);
            Object.keys(solid.faces).length.should.equal(0);
        });
        it("should have no solid", () => {
            solid.numSolids.should.equal(0);
        });
        it("should have no shell", () => {
            solid.numShells.should.equal(0);
        });
        it("should have no outerShell", () => {
            should_1.default.not.exist(solid.outerShell);
        });
    });
    describe("makeBox with 2 points", () => {
        let solid;
        before(() => {
            solid = __1.occ.makeBox([10, 20, 30], [20, 40, 60]);
        });
        it("should be a SOLID", () => {
            solid.shapeType.should.equal("SOLID");
        });
        it("should have 1 solid", () => {
            solid.numSolids.should.equal(1);
        });
        it("should have 1 shell", () => {
            solid.numShells.should.equal(1);
        });
        it("should have 6 faces", () => {
            solid.numFaces.should.equal(6);
            Object.keys(solid.faces).length.should.equal(6);
        });
        it("should have an outerShell with 6 faces", () => {
            should_1.default.exist(solid.getOuterShell());
            solid.getOuterShell().numFaces.should.equal(6);
            // Object.keys(solid.getOuterShell().faces).length.should.equal(6);
        });
        it("should have an outerShell with a forward orientation", () => {
            solid.getOuterShell().orientation.should.equal("FORWARD");
        });
        it("should have (20-10)*(40-20)*(60-30) as a volume", () => {
            solid.volume.should.equal((20 - 10) * (40 - 20) * (60 - 30));
        });
        it("should have ~ 2*((20-10)*(40-20)+(20-10)*(60-30)+(40-20)*(60-30)) as a area", () => {
            let expectedArea = 2 * ((20 - 10) * (40 - 20) + (20 - 10) * (60 - 30) + (40 - 20) * (60 - 30));
            let eps = 0.001;
            solid.area.should.be.within(expectedArea - eps, expectedArea + eps);
        });
        it("should have the sum(face area) ===  area of solid ", () => {
            let epsilon = 1E-3;
            let shapeIt = new __1.ShapeIterator(solid, "FACE");
            let cumulated_face_area = 0;
            while (shapeIt.more) {
                cumulated_face_area += shapeIt.next().area;
            }
            let expectedArea = solid.area;
            cumulated_face_area.should.be.within(expectedArea - epsilon, expectedArea + epsilon);
        });
    });
    describe("makeBox with invalid arguments", () => {
        it("should raise an exception when invalid arguments are passed to makeBox", () => {
            (function failing_func() {
                let solid = __1.occ.makeBox([10, 20, 30], 10, 10, 10);
            }).should.throwError();
        });
    });
    describe("fuse 2 overlapping boxes", () => {
        let solid1;
        let solid2;
        before(() => {
            solid1 = __1.occ.makeBox([10, 20, 30], [20, 40, 60]);
            solid2 = __1.occ.makeBox([15, 25, 35], [-20, -40, -60]);
            solid1 = __1.occ.fuse(solid1, solid2);
        });
        it("should be a SOLID", () => {
            solid1.shapeType.should.equal("SOLID");
        });
        it("should have 1 solid", () => {
            solid1.numSolids.should.equal(1);
        });
        it("should have 1 shell", () => {
            solid1.numShells.should.equal(1);
        });
        it("should have 12 faces", () => {
            solid1.numFaces.should.equal(12);
            Object.keys(solid1.faces).length.should.equal(12);
        });
        it("should have an outerShell with 12 faces", () => {
            should_1.default.exist(solid1.getOuterShell());
            solid1.getOuterShell().numFaces.should.equal(12);
        });
    });
    describe("cut a corner of a box", () => {
        let solid1;
        let solid2;
        before(() => {
            //
            //            +------+
            //      +-----|`.     `.                       +------+
            //      |`.   +  `+-----`+                     |`+--+  `.
            //      |  `+--`  |      |                     | |  |`+--`+
            //      |   |  +  |      |           =>        | +--+ |   |
            //      +   |   ` +------+                     +  `+-`+   |
            //       `. |      |                            `. |      |
            //         `+------+                              `+------+
            solid1 = __1.occ.makeBox([10, 20, 30], [20, 40, 60]);
            solid2 = __1.occ.makeBox([15, 25, 35], [-20, -40, -60]);
            solid1 = __1.occ.cut(solid1, solid2);
        });
        it("should be a SOLID", () => {
            solid1.shapeType.should.equal("SOLID");
        });
        it("should have 9 faces", () => {
            solid1.numFaces.should.equal(9);
            Object.keys(solid1.faces).length.should.equal(9);
        });
        it("should have 1 solid", () => {
            solid1.numSolids.should.equal(1);
        });
        it("should have 1 shell", () => {
            solid1.numShells.should.equal(1);
        });
    });
    describe("Hollow  box  ( 1 solid with 2 shells )", () => {
        let solid1;
        let solid2;
        before(() => {
            solid1 = __1.occ.makeBox([0, 0, 0], [20, 20, 20]);
            solid2 = __1.occ.makeBox([10, 10, 10], [15, 15, 15]);
            solid1 = __1.occ.cut(solid1, solid2);
        });
        it("should be a SOLID", () => {
            solid1.shapeType.should.equal("SOLID");
        });
        it("should have 12 faces", () => {
            solid1.numFaces.should.equal(12);
            Object.keys(solid1.faces).length.should.equal(12);
        });
        it("should have 1 solid", () => {
            solid1.numSolids.should.equal(1);
        });
        it("should have 2 shells", () => {
            solid1.numShells.should.equal(2);
        });
        it("should have an outer shell with 6 faces", () => {
            let outerShell = solid1.getOuterShell();
            outerShell.numFaces.should.equal(6);
        });
        it("should have an outer shell with 6 faces", () => {
            let outerShell = solid1.getOuterShell();
            outerShell.orientation.should.equal("FORWARD");
        });
        it("should expose 2 shells (getOuterShell)", () => {
            let shells = solid1.getShells();
            let outerShell = solid1.getOuterShell();
            should_1.default.exist(outerShell);
            shells.length.should.equal(2);
            for (let i in shells) {
                let shell = shells[i];
                if (outerShell.hashCode !== shell.hashCode) {
                    shell.orientation.should.equal("FORWARD");
                }
            }
        });
    });
    describe("split boxes", () => {
        let solid1;
        let solid2;
        let splitBoxes;
        before(() => {
            // cutting a square box in 2 boxes
            solid1 = __1.occ.makeBox([0, 0, 0], [20, 20, 20]);
            solid2 = __1.occ.makeBox([-100, -100, 10], [100, 100, 15]);
            splitBoxes = __1.occ.cut(solid1, solid2);
        });
        it("should be a COMPOUND", () => {
            splitBoxes.shapeType.should.equal("COMPOUND");
        });
        it("should have 12 faces", () => {
            //console.log( splitBoxes.faces);
            splitBoxes.numFaces.should.equal(12);
            Object.keys(splitBoxes.faces).length.should.equal(12);
        });
        it("should have 2 solids", () => {
            splitBoxes.numSolids.should.equal(2);
        });
        it("should have 2 shells", () => {
            splitBoxes.numShells.should.equal(2);
        });
        it("should have an outer shell with 6 faces", () => {
            let solids = splitBoxes.getSolids();
            let outerShell1 = solids[0].getOuterShell();
            outerShell1.numFaces.should.equal(6);
            let outerShell2 = solids[1].getOuterShell();
            outerShell2.numFaces.should.equal(6);
        });
    });
    describe("creating a compound", () => {
        let compound;
        before(() => {
            let solids = [];
            let solid1 = __1.occ.makeBox(10, 20, 30);
            for (let i = 0; i < 10; i++) {
                let s = solid1.rotate([0, 0, 0], [0, 0, 1], i * 15);
                s.numFaces.should.equal(6);
                solids.push(s);
            }
            compound = __1.occ.compound(solids);
        });
        it("should be a compound", () => {
            compound.shapeType.should.equal("COMPOUND");
        });
        it("should have 10 solids", () => {
            console.log(compound);
            compound.numSolids.should.equal(10);
        });
    });
    describe("Meshing a simple solid", () => {
        describe("Meshing a box", () => {
            let solid;
            before(() => {
                solid = __1.occ.makeBox([10, 20, 30], [20, 30, 40]);
            });
            it("should have a mesh with 8 vertices", () => {
                solid.mesh.numVertices.should.equal(8);
            });
            it("should have a mesh with  4+4+4=12 edges", () => {
                solid.mesh.numEdges.should.equal(12);
            });
            it("should have a mesh with 2*6 triangles", () => {
                solid.mesh.numTriangles.should.equal(12);
            });
        });
    });
    describe("Testing  Shape __prototype", () => {
        let solid;
        before(() => {
            solid = __1.occ.makeBox([10, 20, 30], [20, 30, 40]);
        });
        it("should expose the expected properties ", () => {
            let expected = ["shapeType", "numFaces", "isNull", "isValid"];
            let actual = [];
            for (let j in __1.Solid.prototype) {
                actual.push(j.toString());
            }
            let missing = [];
            for (const e of expected) {
                if (actual.indexOf(e) == -1) {
                    missing.push(e);
                }
            }
            missing.should.have.lengthOf(0);
        });
    });
    describe("exporting a solid to STEP ", () => {
        let step_filename1 = "toto1.step";
        let step_filename2 = "toto2.step";
        let solid1, solid2;
        before(() => {
            solid1 = __1.occ.makeBox([10, 20, 30], [20, 30, 40]);
            solid1 = __1.occ.makeBox([20, 30, 50], [110, 40, 0]);
        });
        it("should export a single solid to STEP", () => {
            __1.occ.writeSTEP(step_filename1, solid1);
        });
        it("should export many solids to STEP", () => {
            __1.occ.writeSTEP(step_filename2, solid1, solid2);
        });
    });
    describe("testing ShapeIterator on solid", () => {
        let solid;
        let shapeIt;
        before(() => {
            solid = __1.occ.makeBox([10, 20, 30], [20, 40, 60]);
        });
        it("should iterate on 6 faces", () => {
            shapeIt = new __1.ShapeIterator(solid, "FACE");
            shapeIt.more.should.be.equal(true);
            should_1.default.not.exist(shapeIt.current);
            let counter = 0;
            while (shapeIt.more) {
                shapeIt.more.should.be.equal(true);
                shapeIt.next();
                should_1.default.exists(shapeIt.current);
                counter += 1;
            }
            counter.should.equal(6);
            shapeIt.more.should.be.equal(false);
            should_1.default.exists(shapeIt.current);
        });
        it("should iterate on 24 edges ( 4 on each of the 6 faces", () => {
            shapeIt = new __1.ShapeIterator(solid, "EDGE");
            shapeIt.more.should.be.equal(true);
            should_1.default.not.exist(shapeIt.current);
            let counter = 0;
            while (shapeIt.more) {
                shapeIt.more.should.be.equal(true);
                shapeIt.next();
                should_1.default.exists(shapeIt.current);
                counter += 1;
            }
            counter.should.equal(24);
            shapeIt.more.should.be.equal(false);
            should_1.default.exists(shapeIt.current);
        });
    });
    describe("testing fillet on a box..", () => {
        let solid;
        before(() => {
            solid = __1.occ.makeBox([10, 20, 30], [30, 40, 50]);
            solid.numFaces.should.equal(6);
            solid = __1.occ.makeFillet(solid, solid.getEdges(), 2.0);
        });
        it("should be possible to round the corner...", () => {
            //    6 flat surfaces       -> 6*4  edges
            // + 12 rounded corners     -> shared
            // + 8  corners             -> 8*3   edges
            //==> 26 faces
            solid.numFaces.should.be.equal(26);
            solid.getEdges().length.should.be.equal(6 * 4 + 8 * 3);
        });
    });
    describe("makeCylinder (variation 1)", () => {
        let solid;
        before(() => {
            let radius = 50;
            let height = 100;
            solid = __1.occ.makeCylinder(radius, height);
        });
        it("should have 3 faces", () => {
            solid.numFaces.should.equal(3);
        });
    });
    describe("makeCylinder (variation 2)", () => {
        let solid;
        before(() => {
            let position = [[0, 0, 1], [0, 1, 0]];
            let radius = 50;
            let height = 100;
            solid = __1.occ.makeCylinder(position, radius, height);
        });
        it("should have 3 faces", () => {
            solid.numFaces.should.equal(3);
        });
    });
    describe("makeCylinder (variation 3 : with 2 points and a radius)", () => {
        let solid;
        let bbox;
        before(() => {
            let startPoint = [-100, 20, 40];
            let endPoint = [100, 20, 40];
            let radius = 20;
            solid = __1.occ.makeCylinder(startPoint, endPoint, radius);
            bbox = solid.getBoundingBox();
        });
        it("should have 3 faces", () => {
            solid.numFaces.should.equal(3);
        });
        it("should have a bounding box that includes X=-100,20,40", () => {
            bbox.nearPt.y.should.be.within(-2, 0);
            bbox.farPt.y.should.be.within(40, 42);
            bbox.nearPt.x.should.be.within(-101, -100);
            bbox.farPt.x.should.be.within(100, 101);
        });
    });
    describe("makeCone - variation 1", () => {
        let solid;
        before(() => {
            let radius1 = 50;
            let radius2 = 70;
            let height = 30;
            solid = __1.occ.makeCone(radius1, radius2, height);
        });
        it("should have 3 faces", () => {
            solid.numFaces.should.equal(3);
        });
    });
    describe("makeCone - variation 2 ( point,R1, point, R2 )", () => {
        let solid;
        let radius1 = 50;
        let radius2 = 70;
        before(() => {
            let height = 30;
            solid = __1.occ.makeCone([0, 0, 0], radius1, [0, 0, height], radius2);
        });
        it("should have 3 faces", () => {
            solid.numFaces.should.equal(3);
            should_1.default.exist(solid.faces.top);
            should_1.default.exist(solid.faces.lateral);
            should_1.default.exist(solid.faces.bottom);
        });
        it("top face should have a area of radius**2*pi", () => {
            let expectedArea = radius2 * radius2 * Math.PI;
            let eps = 1.0;
            solid.faces.top.area.should.be.within(expectedArea - eps, expectedArea + eps);
        });
        it("bottom face should have a area of radius**2*pi", () => {
            let expectedArea = radius1 * radius1 * Math.PI;
            let eps = 1.0;
            solid.faces.bottom.area.should.be.within(expectedArea - eps, expectedArea + eps);
        });
    });
    describe("makeCone - variation 3 ( axpex,dir, half_angle, height )", () => {
        let solid;
        let radius = 50;
        let height = 30;
        before(() => {
            let angle = Math.atan(radius / height);
            solid = __1.occ.makeCone([0, 0, 0], [0, 0, 1], angle, height);
        });
        it("should have 2 faces", () => {
            solid.numFaces.should.equal(2);
            should_1.default.exist(solid.faces.top);
            should_1.default.exist(solid.faces.lateral);
            should_1.default.not.exist(solid.faces.bottom);
        });
        it("top face should have a area of radius**2*pi", () => {
            let expectedArea = radius * radius * Math.PI;
            let eps = 1.0;
            solid.faces.top.area.should.be.within(expectedArea - eps, expectedArea + eps);
        });
    });
    describe("makeSphere", () => {
        let solid;
        let radius = 10;
        let epsilon = radius * 1E-1;
        before(() => {
            let center = [10, 20, 30];
            solid = __1.occ.makeSphere(center, radius);
        });
        it("should have 1 face and one egde", () => {
            solid.numFaces.should.equal(1);
            solid.getEdges().length.should.equal(1);
            let edges = solid.getEdges();
            for (let edge in edges) {
                // todo : do some investigation
            }
        });
        it("should have a area of 4*Pi*R", () => {
            let expected_area = 4 * 3.14159265 * radius * radius;
            solid.area.should.be.within(expected_area - epsilon, expected_area + epsilon);
        });
        it("should have a volume of 4/3*Pi*R*2", () => {
            let expected_volume = 4.0 / 3.0 * 3.14159265 * radius * radius * radius;
            solid.volume.should.be.within(expected_volume - epsilon, expected_volume + epsilon);
        });
    });
    describe("makeTorus", () => {
        let solid;
        before(() => {
            solid = __1.occ.makeTorus([0, 0, 0], [0, 0, 1], 100, 10);
        });
        it("should have one single face", () => {
            //console.log(solid.faces);
            solid.numFaces.should.equal(1);
            should_1.default.exist(solid.faces.lateral);
        });
    });
    describe("makeTorus with invalid arguments", () => {
        it("should not crash if torus is created with invalid arguments", () => {
            (0, should_1.default)(() => {
                const solid = __1.occ.makeTorus([0, 0, 0], [0, 0, 0], 10, 100);
            }).throwError();
        });
    });
    describe("makeCylinder with invalid arguments", () => {
        it("should not crash if Cylinder is created with invalid arguments", () => {
            (0, should_1.default)(() => {
                const solid = __1.occ.makeCylinder([0, 0, 0], [0, 0, 0], 10);
            }).throwError();
        });
    });
    describe("makeCone with invalid arguments", () => {
        it("should not crash if Cone is created with invalid arguments", () => {
            (0, should_1.default)(() => {
                const solid = __1.occ.makeCone([0, 0, 0], [0, 0, 0], 10);
            }).throwError();
            (0, should_1.default)(() => {
                const solid = __1.occ.makeCone([0, 0, 0], [0, 0, 0], 0);
            }).throwError();
        });
    });
    describe("rotate apply on a solid", () => {
        let solid;
        before(() => {
            solid = __1.occ.makeBox([10, 10, 0], [20, 20, 10]);
        });
        it("should expose a rotated box", () => {
            let epsilon = 0.1;
            let bbox = solid.getBoundingBox();
            bbox.farPt.x.should.be.lessThan(20.0 + epsilon);
            bbox.farPt.y.should.be.lessThan(20.0 + epsilon);
            bbox.nearPt.x.should.be.greaterThan(10.0 - epsilon);
            bbox.nearPt.y.should.be.greaterThan(10.0 - epsilon);
            solid = solid.rotate([0, 0, 0], [0, 0, 1], 90);
            bbox = solid.getBoundingBox();
            bbox.farPt.x.should.be.within(-10.0 - epsilon, -10 + epsilon);
            bbox.farPt.y.should.be.within(20.0 - epsilon, 20 + epsilon);
            bbox.nearPt.x.should.be.within(-20.0 - epsilon, -20 + epsilon);
            bbox.nearPt.y.should.be.within(10.0 - epsilon, 10 + epsilon);
        });
    });
    describe(" making a illegal solid ( with bad arguments) shall raise exception", () => {
        it("should raise exception when trying to build a box with illegal arguments", () => {
            (() => {
                let solid = __1.occ.makeBox("illegal");
            }).should.throwError();
        });
    });
    describe("test adjacent faces", () => {
        let solid;
        before(() => {
            solid = __1.occ.makeBox([0, 0, 0], [100, 100, 100]);
        });
        it("should have back/front/left/right faces adjacent to face 'top'", () => {
            let adjFaces = solid.getAdjacentFaces(solid.faces.top);
            adjFaces.length.should.equal(4);
            let names = adjFaces.map(function (f) {
                return solid.getShapeName(f);
            }).sort();
            names.join("/").should.equal("back/front/left/right");
        });
        it("should have back/front/left/right faces adjacent to face 'bottom'", () => {
            let adjFaces = solid.getAdjacentFaces(solid.faces.bottom);
            adjFaces.length.should.equal(4);
            let names = adjFaces.map(function (f) {
                return solid.getShapeName(f);
            }).sort();
            names.join("/").should.equal("back/front/left/right");
        });
        it("should have bottom/left/right/top faces adjacent to face 'back'", () => {
            let adjFaces = solid.getAdjacentFaces(solid.faces.back);
            adjFaces.length.should.equal(4);
            let names = adjFaces.map(function (f) {
                return solid.getShapeName(f);
            }).sort();
            names.join("/").should.equal("bottom/left/right/top");
        });
        it("should have bottom/left/right/top faces adjacent to face 'front'", () => {
            let adjFaces = solid.getAdjacentFaces(solid.faces.front);
            adjFaces.length.should.equal(4);
            let names = adjFaces.map(function (f) {
                return solid.getShapeName(f);
            }).sort();
            names.join("/").should.equal("bottom/left/right/top");
        });
        it("should have back/bottom/front/top faces adjacent to face 'left'", () => {
            let adjFaces = solid.getAdjacentFaces(solid.faces.left);
            adjFaces.length.should.equal(4);
            let names = adjFaces.map(function (f) {
                return solid.getShapeName(f);
            }).sort();
            names.join("/").should.equal("back/bottom/front/top");
        });
        it("should have back/bottom/front/top faces adjacent to face 'right'", () => {
            let adjFaces = solid.getAdjacentFaces(solid.faces.right);
            adjFaces.length.should.equal(4);
            let names = adjFaces.map(function (f) {
                return solid.getShapeName(f);
            }).sort();
            names.join("/").should.equal("back/bottom/front/top");
        });
    });
    describe("makeThickSolid (external ) on box", () => {
        let initialBox;
        let thickSolid;
        before(() => {
            initialBox = __1.occ.makeBox(100, 200, 300);
            thickSolid = __1.occ.makeThickSolid(initialBox, initialBox.faces.top, 10);
        });
        it("should have 23 (6 + 4 vertical faces + 4 vertical fillets + 1 horizontal face + 4 horizontal fillets + 4 rounded corners) faces", () => {
            console.log(Object.keys(thickSolid.getFaces().map(function (el) {
                return thickSolid.getShapeName(el);
            })).join(" "));
            //xx console.log( Object.keys(thickSolid.faces).join(" "));
            initialBox.numFaces.should.equal(6);
            thickSolid.numFaces.should.equal(23);
        });
    });
    describe("makeThickSolid (internal) on box", () => {
        let initialBox;
        let thickSolid;
        before(() => {
            initialBox = __1.occ.makeBox(100, 200, 300);
            thickSolid = __1.occ.makeThickSolid(initialBox, initialBox.faces.top, -10);
        });
        it("should have 1 (1 top face modified + 5 old + 5 new) faces", () => {
            console.log(Object.keys(thickSolid.getFaces().map(function (el) {
                return thickSolid.getShapeName(el);
            })).join(" "));
            //xx console.log( Object.keys(thickSolid.faces).join(" "));
            initialBox.numFaces.should.equal(6);
            thickSolid.numFaces.should.equal(11);
        });
    });
    describe("finding common edge of 2 faces", () => {
        let box;
        before(() => {
            box = __1.occ.makeBox(100, 200, 300);
        });
        it("should find a common edge between 'top' face and 'left' face", () => {
            let edges = box.getCommonEdges(box.faces.top, box.faces.left);
            edges.length.should.be.equal(1);
        });
        it("should not find a common edge between 'top' face and 'bottom' face", () => {
            let edges = box.getCommonEdges(box.faces.top, box.faces.bottom);
            edges.length.should.be.equal(0);
        });
    });
    describe("makeDraftAngle", () => {
        let box;
        let boxWithDraftFace;
        before(() => {
            box = __1.occ.makeBox(100, 200, 300);
            boxWithDraftFace = __1.occ.makeDraftAngle(box, box.faces.right, 20 * DEG2RAD, box.faces.bottom);
        });
        it("should have 6 faces", () => {
            boxWithDraftFace.numFaces.should.equal(6);
        });
        it("should have a smaller volume", () => {
            boxWithDraftFace.volume.should.be.lessThan(box.volume);
        });
    });
    describe("makeDraftAngle on a box with a rounded corner", () => {
        let box;
        let boxWithDraftFace;
        before(() => {
            box = __1.occ.makeBox(100, 200, 300);
            let edges = box.getCommonEdges(box.faces.left, box.faces.front)[0];
            // console.log("edge = ",edges);
            box = __1.occ.makeFillet(box, edges, 10);
            // note: left , front , top and bottom faces have been modified by the fillet
            // operation.;
            let faceToDraft = box.faces["mleft:0"];
            let neutralFace = box.faces["mbottom:0"];
            console.log(Object.keys(box.faces).join(" "));
            should_1.default.exist(faceToDraft);
            should_1.default.exist(neutralFace);
            boxWithDraftFace = __1.occ.makeDraftAngle(box, faceToDraft, 5 * DEG2RAD, neutralFace);
        });
        it("should have 7 faces", () => {
            boxWithDraftFace.numFaces.should.equal(7);
        });
        it("should have a smaller volume", () => {
            boxWithDraftFace.volume.should.be.lessThan(box.volume);
        });
    });
});
