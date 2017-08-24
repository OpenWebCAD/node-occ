const should = require("should");

const occ = require("../lib/occ");

const DEG2RAD = Math.PI / 180;

// see https://npmjs.org/package/should

describe("testing solid construction", function () {


    describe("empty solid", function () {
        let solid;
        before(function () {
            solid = new occ.Solid();
        });
        it("should have no faces", function () {
            solid.numFaces.should.equal(0);
            Object.keys(solid.faces).length.should.equal(0);
        });
        it("should have no solid", function () {
            solid.numSolids.should.equal(0);
        });
        it("should have no shell", function () {
            solid.numShells.should.equal(0);
        });
        it("should have no outerShell", function () {
            should.not.exist(solid.outerShell);
        });
    });
    describe("makeBox with 2 points", function () {
        let solid;
        before(function () {
            solid = occ.makeBox([10, 20, 30], [20, 40, 60]);
        });
        it("should be a SOLID", function () {
            solid.shapeType.should.equal("SOLID");
        });
        it("should have 1 solid", function () {
            solid.numSolids.should.equal(1);
        });
        it("should have 1 shell", function () {
            solid.numShells.should.equal(1);
        });
        it("should have 6 faces", function () {
            solid.numFaces.should.equal(6);
            Object.keys(solid.faces).length.should.equal(6);
        });
        it("should have an outerShell with 6 faces", function () {
            should.exist(solid.getOuterShell());
            solid.getOuterShell().numFaces.should.equal(6);
            // Object.keys(solid.getOuterShell().faces).length.should.equal(6);
        });
        it("should have an outerShell with a forward orientation", function () {
            solid.getOuterShell().orientation.should.equal("FORWARD");
        });
        it("should have (20-10)*(40-20)*(60-30) as a volume", function () {
            solid.volume.should.equal((20 - 10) * (40 - 20) * (60 - 30));
        });

        it("should have ~ 2*((20-10)*(40-20)+(20-10)*(60-30)+(40-20)*(60-30)) as a area", function () {
            let expectedArea = 2 * ((20 - 10) * (40 - 20) + (20 - 10) * (60 - 30) + (40 - 20) * (60 - 30));
            let eps = 0.001;
            solid.area.should.be.within(expectedArea - eps, expectedArea + eps);
        });

        it("should have the sum(face area) ===  area of solid ", function () {

            let epsilon = 1E-3;

            let shapeIt = new occ.ShapeIterator(solid, "FACE");
            let cumulated_face_area = 0;
            while (shapeIt.more) {
                cumulated_face_area += shapeIt.next().area;
            }
            let expectedArea = solid.area;
            cumulated_face_area.should.be.within(expectedArea - epsilon, expectedArea + epsilon);
        });
    });
    describe("makeBox with invalid arguments", function () {

        it("should raise an exception when invalid arguments are passed to makeBox", function () {
            (function failing_func() {
                let solid = occ.makeBox([10, 20, 30], 10, 10, 10);
            }).should.throwError();
        });
    });
    describe("fuse 2 overlapping boxes", function () {
        let solid1;
        let solid2;
        before(function () {
            solid1 = occ.makeBox([10, 20, 30], [20, 40, 60]);
            solid2 = occ.makeBox([15, 25, 35], [-20, -40, -60]);
            solid1 = occ.fuse(solid1, solid2);
        });
        it("should be a SOLID", function () {
            solid1.shapeType.should.equal("SOLID");
        });
        it("should have 1 solid", function () {
            solid1.numSolids.should.equal(1);
        });
        it("should have 1 shell", function () {
            solid1.numShells.should.equal(1);
        });
        it("should have 12 faces", function () {
            solid1.numFaces.should.equal(12);
            Object.keys(solid1.faces).length.should.equal(12);
        });
        it("should have an outerShell with 12 faces", function () {
            should.exist(solid1.getOuterShell());
            solid1.getOuterShell().numFaces.should.equal(12);
        });
    });
    describe("cut a corner of a box", function () {
        let solid1;
        let solid2;
        before(function () {
            //
            //            +------+
            //      +-----|`.     `.                       +------+
            //      |`.   +  `+-----`+                     |`+--+  `.
            //      |  `+--`  |      |                     | |  |`+--`+
            //      |   |  +  |      |           =>        | +--+ |   |
            //      +   |   ` +------+                     +  `+-`+   |
            //       `. |      |                            `. |      |
            //         `+------+                              `+------+
            solid1 = occ.makeBox([10, 20, 30], [20, 40, 60]);
            solid2 = occ.makeBox([15, 25, 35], [-20, -40, -60]);
            solid1 = occ.cut(solid1, solid2);
        });
        it("should be a SOLID", function () {
            solid1.shapeType.should.equal("SOLID");
        });
        it("should have 9 faces", function () {
            solid1.numFaces.should.equal(9);
            Object.keys(solid1.faces).length.should.equal(9);
        });
        it("should have 1 solid", function () {
            solid1.numSolids.should.equal(1);
        });
        it("should have 1 shell", function () {
            solid1.numShells.should.equal(1);
        });


    });
    describe("Hollow  box  ( 1 solid with 2 shells )", function () {
        let solid1;
        let solid2;
        before(function () {
            solid1 = occ.makeBox([0, 0, 0], [20, 20, 20]);
            solid2 = occ.makeBox([10, 10, 10], [15, 15, 15]);

            solid1 = occ.cut(solid1, solid2);

        });
        it("should be a SOLID", function () {
            solid1.shapeType.should.equal("SOLID");
        });
        it("should have 12 faces", function () {
            solid1.numFaces.should.equal(12);
            Object.keys(solid1.faces).length.should.equal(12);

        });
        it("should have 1 solid", function () {
            solid1.numSolids.should.equal(1);
        });
        it("should have 2 shells", function () {
            solid1.numShells.should.equal(2);
        });
        it("should have an outer shell with 6 faces", function () {
            let outerShell = solid1.getOuterShell();
            outerShell.numFaces.should.equal(6);
        });
        it("should have an outer shell with 6 faces", function () {
            let outerShell = solid1.getOuterShell();
            outerShell.orientation.should.equal("FORWARD");
        });
        it("should expose 2 shells (getOuterShell)", function () {

            let shells = solid1.getShells();

            let outerShell = solid1.getOuterShell();
            should.exist(outerShell);

            shells.length.should.equal(2);
            for (let i in shells) {
                let shell = shells[i];
                if (outerShell.hashCode !== shell.hashCode) {
                    shell.orientation.should.equal("FORWARD");
                }
            }
        });
    });
    describe("split boxes", function () {

        let solid1;
        let solid2;
        let splitBoxes;
        before(function () {
            // cutting a square box in 2 boxes
            solid1 = occ.makeBox([0, 0, 0], [20, 20, 20]);
            solid2 = occ.makeBox([-100, -100, 10], [100, 100, 15]);

            splitBoxes = occ.cut(solid1, solid2);

        });
        it("should be a COMPOUND", function () {
            splitBoxes.shapeType.should.equal("COMPOUND");
        });
        it("should have 12 faces", function () {
            //console.log( splitBoxes.faces);
            splitBoxes.numFaces.should.equal(12);
            Object.keys(splitBoxes.faces).length.should.equal(12);
        });
        it("should have 2 solids", function () {
            splitBoxes.numSolids.should.equal(2);
        });
        it("should have 2 shells", function () {
            splitBoxes.numShells.should.equal(2);
        });
        it("should have an outer shell with 6 faces", function () {
            let solids = splitBoxes.getSolids();

            let outerShell1 = solids[0].getOuterShell();
            outerShell1.numFaces.should.equal(6);

            let outerShell2 = solids[1].getOuterShell();
            outerShell2.numFaces.should.equal(6);

        });

    });

    describe("creating a compound", function () {
        let compound;
        before(function () {
            let solids = [];
            let solid1 = occ.makeBox(10, 20, 30);
            for (let i = 0; i < 10; i++) {
                let s = solid1.rotate([0, 0, 0], [0, 0, 1], i * 15);
                s.numFaces.should.equal(6);
                solids.push(s);
            }
            compound = occ.compound(solids);
        });

        it("should be a compound", function () {
            compound.shapeType.should.equal("COMPOUND");
        });
        it("should have 10 solids", function () {
            console.log(compound);
            compound.numSolids.should.equal(10);
        });
    });

    describe("Meshing a simple solid", function () {
        describe("Meshing a box", function () {
            let solid;
            before(function () {
                solid = occ.makeBox([10, 20, 30], [20, 30, 40]);
            });
            it("should have a mesh with 8 vertices", function () {
                solid.mesh.numVertices.should.equal(8);
            });
            it("should have a mesh with  4+4+4=12 edges", function () {
                solid.mesh.numEdges.should.equal(12);
            });
            it("should have a mesh with 2*6 triangles", function () {
                solid.mesh.numTriangles.should.equal(12);
            });
        });
    });
    describe("Testing  Shape __prototype", function () {
        let solid;
        before(function () {
            solid = occ.makeBox([10, 20, 30], [20, 30, 40]);
        });
        it("should expose the expected properties ", function () {
            let expected = ["shapeType", "numFaces", "isNull", "isValid"];
            let actual = [];
            for (let j in occ.Solid.prototype) {
                actual.push(j.toString());
            }
            let missing = [];
            for (j in expected) {
                if (actual.indexOf(expected[j]) == -1) {
                    missing.push(expected[j]);
                }
            }
            missing.should.have.lengthOf(0);

        });
    });
    describe("exporting a solid to STEP ", function () {

        let step_filename1 = "toto1.step";
        let step_filename2 = "toto2.step";
        let solid1, solid2;
        before(function () {
            solid1 = occ.makeBox([10, 20, 30], [20, 30, 40]);
            solid1 = occ.makeBox([20, 30, 50], [110, 40, 0]);
        });
        it("should export a single solid to STEP", function () {
            occ.writeSTEP(step_filename1, solid1);
        });
        it("should export many solids to STEP", function () {
            occ.writeSTEP(step_filename2, solid1, solid2);
        });
    });
    describe("testing ShapeIterator on solid", function () {
        let solid;
        let shapeIt;
        before(function () {
            solid = occ.makeBox([10, 20, 30], [20, 40, 60]);
        });
        it("should iterate on 6 faces", function () {

            shapeIt = new occ.ShapeIterator(solid, "FACE");
            shapeIt.more.should.be.equal(true);
            should.not.exist(shapeIt.current);
            let counter = 0;
            while (shapeIt.more) {
                shapeIt.more.should.be.equal(true);
                shapeIt.next();
                should.exists(shapeIt.current);
                counter += 1;
            }
            counter.should.equal(6);
            shapeIt.more.should.be.equal(false);
            should.exists(shapeIt.current);

        });
        it("should iterate on 24 edges ( 4 on each of the 6 faces", function () {
            shapeIt = new occ.ShapeIterator(solid, "EDGE");
            shapeIt.more.should.be.equal(true);
            should.not.exist(shapeIt.current);
            let counter = 0;
            while (shapeIt.more) {
                shapeIt.more.should.be.equal(true);
                shapeIt.next();
                should.exists(shapeIt.current);
                counter += 1;
            }
            counter.should.equal(24);
            shapeIt.more.should.be.equal(false);
            should.exists(shapeIt.current);

        });

    });
    describe("testing fillet on a box..", function () {

        let solid;

        before(function () {
            solid = occ.makeBox([10, 20, 30], [30, 40, 50]);
            solid.numFaces.should.equal(6);

            solid = occ.makeFillet(solid, solid.getEdges(), 2.0);

        });
        it("should be possible to round the corner...", function () {

            //    6 flat surfaces       -> 6*4  edges
            // + 12 rounded corners     -> shared
            // + 8  corners             -> 8*3   edges
            //==> 26 faces
            solid.numFaces.should.be.equal(26);
            solid.getEdges().length.should.be.equal(6 * 4 + 8 * 3);

        });

    });
    describe("makeCylinder (variation 1)", function () {
        let solid;
        before(function () {
            let radius = 50;
            let height = 100;
            solid = occ.makeCylinder(radius, height);
        });
        it("should have 3 faces", function () {
            solid.numFaces.should.equal(3);
        });
    });

    describe("makeCylinder (variation 2)", function () {
        let solid;
        before(function () {
            let position = [[0, 0, 1], [0, 1, 0]];
            let radius = 50;
            let height = 100;
            solid = occ.makeCylinder(position, radius, height);
        });
        it("should have 3 faces", function () {
            solid.numFaces.should.equal(3);
        });
    });
    describe("makeCylinder (variation 3 : with 2 points and a radius)", function () {
        let solid;
        let bbox;
        before(function () {
            let startPoint = [-100, 20, 40];
            let endPoint = [100, 20, 40];
            let radius = 20;
            solid = occ.makeCylinder(startPoint, endPoint, radius);
            bbox = solid.getBoundingBox();

        });
        it("should have 3 faces", function () {
            solid.numFaces.should.equal(3);
        });
        it("should have a bounding box that includes X=-100,20,40", function () {
            bbox.nearPt.y.should.be.within(-2, 0);
            bbox.farPt.y.should.be.within(40, 42);

            bbox.nearPt.x.should.be.within(-101, -100);
            bbox.farPt.x.should.be.within(100, 101);
        });
    });
    describe("makeCone - variation 1", function () {
        let solid;
        before(function () {
            let radius1 = 50;
            let radius2 = 70;
            let height = 30;
            solid = occ.makeCone(radius1, radius2, height);
        });
        it("should have 3 faces", function () {
            solid.numFaces.should.equal(3);
        });
    });
    describe("makeCone - variation 2 ( point,R1, point, R2 )", function () {
        let solid;
        let radius1 = 50;
        let radius2 = 70;
        before(function () {
            let height = 30;
            solid = occ.makeCone([0, 0, 0], radius1, [0, 0, height], radius2);
        });
        it("should have 3 faces", function () {
            solid.numFaces.should.equal(3);
            should.exist(solid.faces.top);
            should.exist(solid.faces.lateral);
            should.exist(solid.faces.bottom);
        });
        it("top face should have a area of radius**2*pi", function () {
            let expectedArea = radius2 * radius2 * Math.PI;
            let eps = 1.0;
            solid.faces.top.area.should.be.within(expectedArea - eps, expectedArea + eps);
        });
        it("bottom face should have a area of radius**2*pi", function () {
            let expectedArea = radius1 * radius1 * Math.PI;
            let eps = 1.0;
            solid.faces.bottom.area.should.be.within(expectedArea - eps, expectedArea + eps);
        });
    });
    describe("makeCone - variation 3 ( axpex,dir, half_angle, height )", function () {
        let solid;
        let radius = 50;
        let height = 30;
        before(function () {
            let angle = Math.atan(radius / height);
            solid = occ.makeCone([0, 0, 0], [0, 0, 1], angle, height);
        });
        it("should have 2 faces", function () {
            solid.numFaces.should.equal(2);
            should.exist(solid.faces.top);
            should.exist(solid.faces.lateral);
            should.not.exist(solid.faces.bottom);
        });
        it("top face should have a area of radius**2*pi", function () {
            let expectedArea = radius * radius * Math.PI;
            let eps = 1.0;
            solid.faces.top.area.should.be.within(expectedArea - eps, expectedArea + eps);
        });

    });

    describe("makeSphere", function () {
        let solid;
        let radius = 10;
        let epsilon = radius * 1E-1;
        before(function () {
            let center = [10, 20, 30];
            solid = occ.makeSphere(center, radius);
        });
        it("should have 1 face and one egde", function () {
            solid.numFaces.should.equal(1);
            solid.getEdges().length.should.equal(1);
            let edges = solid.getEdges();
            for (let edge in edges) {
                // todo : do some investigation
            }
        });
        it("should have a area of 4*Pi*R", function () {
            let expected_area = 4 * 3.14159265 * radius * radius;
            solid.area.should.be.within(expected_area - epsilon, expected_area + epsilon);
        });
        it("should have a volume of 4/3*Pi*R*2", function () {

            let expected_volume = 4.0 / 3.0 * 3.14159265 * radius * radius * radius;
            solid.volume.should.be.within(expected_volume - epsilon, expected_volume + epsilon);

        });
    });
    describe("makeTorus", function () {
        let solid;
        before(function () {
            solid = occ.makeTorus([0, 0, 0], [0, 0, 1], 100, 10);
        });
        it("should have one single face", function () {
            //console.log(solid.faces);
            solid.numFaces.should.equal(1);
            should.exist(solid.faces.lateral);
        });

    });
    describe("makeTorus with invalid arguments", function () {

        it("should not crash if torus is created with invalid arguments", function () {
            should(function () {
                const solid = occ.makeTorus([0, 0, 0], [0, 0, 0], 10, 100);
            }).throwError();
        });
    });
    describe("makeCylinder with invalid arguments", function () {

        it("should not crash if Cylinder is created with invalid arguments", function () {
            should(function () {
                const solid = occ.makeCylinder([0, 0, 0], [0, 0, 0], 10);
            }).throwError();
        });
    });

    describe("makeCone with invalid arguments", function () {

        it("should not crash if Cone is created with invalid arguments", function () {
            should(function () {
                const solid = occ.makeCone([0, 0, 0], 0 [0, 0, 0], 10);
            }).throwError();

            should(function () {
                const solid = occ.makeCone([0, 0, 0], 0 [0, 0, 0], 0);
            }).throwError();
        });
    });


    describe("rotate apply on a solid", function () {
        let solid;
        before(function () {
            solid = occ.makeBox([10, 10, 0], [20, 20, 10]);

        });
        it("should expose a rotated box", function () {

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
    describe(" making a illegal solid ( with bad arguments) shall raise exception", function () {
        it("should raise exception when trying to build a box with illegal arguments", function () {
            (function () {
                let solid = makebox("illegal");
            }).should.throwError();

        });
    });
    describe("test adjacent faces", function () {
        let solid;
        before(function () {
            solid = occ.makeBox([0, 0, 0], [100, 100, 100]);
        });
        it("should have back/front/left/right faces adjacent to face 'top'", function () {
            let adjFaces = solid.getAdjacentFaces(solid.faces.top);

            adjFaces.length.should.equal(4);

            let names = adjFaces.map(function (f) {
                return solid.getShapeName(f);
            }).sort();

            names.join("/").should.equal("back/front/left/right");

        });
        it("should have back/front/left/right faces adjacent to face 'bottom'", function () {

            let adjFaces = solid.getAdjacentFaces(solid.faces.bottom);

            adjFaces.length.should.equal(4);

            let names = adjFaces.map(function (f) {
                return solid.getShapeName(f);
            }).sort();

            names.join("/").should.equal("back/front/left/right");
        });

        it("should have bottom/left/right/top faces adjacent to face 'back'", function () {

            let adjFaces = solid.getAdjacentFaces(solid.faces.back);

            adjFaces.length.should.equal(4);

            let names = adjFaces.map(function (f) {
                return solid.getShapeName(f);
            }).sort();

            names.join("/").should.equal("bottom/left/right/top");
        });

        it("should have bottom/left/right/top faces adjacent to face 'front'", function () {

            let adjFaces = solid.getAdjacentFaces(solid.faces.front);

            adjFaces.length.should.equal(4);

            let names = adjFaces.map(function (f) {
                return solid.getShapeName(f);
            }).sort();

            names.join("/").should.equal("bottom/left/right/top");
        });
        it("should have back/bottom/front/top faces adjacent to face 'left'", function () {

            let adjFaces = solid.getAdjacentFaces(solid.faces.left);

            adjFaces.length.should.equal(4);

            let names = adjFaces.map(function (f) {
                return solid.getShapeName(f);
            }).sort();

            names.join("/").should.equal("back/bottom/front/top");
        });

        it("should have back/bottom/front/top faces adjacent to face 'right'", function () {

            let adjFaces = solid.getAdjacentFaces(solid.faces.right);

            adjFaces.length.should.equal(4);

            let names = adjFaces.map(function (f) {
                return solid.getShapeName(f);
            }).sort();

            names.join("/").should.equal("back/bottom/front/top");
        });
    });

    describe("makeThickSolid (external ) on box", function () {
        let initialBox;
        let thickSolid;
        before(function () {
            initialBox = occ.makeBox(100, 200, 300);
            thickSolid = occ.makeThickSolid(initialBox, initialBox.faces.top, 10);
        });
        it("should have 23 (6 + 4 vertical faces + 4 vertical fillets + 1 horizontal face + 4 horizontal fillets + 4 rounded corners) faces", function () {
            console.log(Object.keys(thickSolid.getFaces().map(function (el) {
                return thickSolid.getShapeName(el);
            })).join(" "));
            //xx console.log( Object.keys(thickSolid.faces).join(" "));
            initialBox.numFaces.should.equal(6);
            thickSolid.numFaces.should.equal(23);
        });
    });
    describe("makeThickSolid (internal) on box", function () {
        let initialBox;
        let thickSolid;
        before(function () {
            initialBox = occ.makeBox(100, 200, 300);
            thickSolid = occ.makeThickSolid(initialBox, initialBox.faces.top, -10);
        });
        it("should have 1 (1 top face modified + 5 old + 5 new) faces", function () {
            console.log(Object.keys(thickSolid.getFaces().map(function (el) {
                return thickSolid.getShapeName(el);
            })).join(" "));
            //xx console.log( Object.keys(thickSolid.faces).join(" "));
            initialBox.numFaces.should.equal(6);
            thickSolid.numFaces.should.equal(11);
        });
    });
    describe("finding common edge of 2 faces", function () {
        let box;
        before(function () {
            box = occ.makeBox(100, 200, 300);
        });
        it("should find a common edge between 'top' face and 'left' face", function () {
            let edges = box.getCommonEdges(box.faces.top, box.faces.left);
            edges.length.should.be.equal(1);

        });
        it("should not find a common edge between 'top' face and 'bottom' face", function () {
            let edges = box.getCommonEdges(box.faces.top, box.faces.bottom);
            edges.length.should.be.equal(0);
        });
    });
    describe("makeDraftAngle", function () {
        let box;
        let boxWithDraftFace;
        before(function () {
            box = occ.makeBox(100, 200, 300);
            boxWithDraftFace = occ.makeDraftAngle(box, box.faces.right, 20 * DEG2RAD, box.faces.bottom);
        });
        it("should have 6 faces", function () {
            boxWithDraftFace.numFaces.should.equal(6);
        });
        it("should have a smaller volume", function () {
            boxWithDraftFace.volume.should.be.lessThan(box.volume);

        });
    });

    describe("makeDraftAngle on a box with a rounded corner", function () {
        let box;
        let boxWithDraftFace;
        before(function () {
            box = occ.makeBox(100, 200, 300);
            let edges = box.getCommonEdges(box.faces.left, box.faces.front)[0];
            // console.log("edge = ",edges);
            box = occ.makeFillet(box, edges, 10);

            // note: left , front , top and bottom faces have been modified by the fillet
            // operation.;

            let faceToDraft = box.faces["mleft:0"];
            let neutralFace = box.faces["mbottom:0"];

            console.log(Object.keys(box.faces).join(" "));
            should.exist(faceToDraft);
            should.exist(neutralFace);
            boxWithDraftFace = occ.makeDraftAngle(box, faceToDraft, 5 * DEG2RAD, neutralFace);

        });
        it("should have 7 faces", function () {
            boxWithDraftFace.numFaces.should.equal(7);
        });
        it("should have a smaller volume", function () {
            boxWithDraftFace.volume.should.be.lessThan(box.volume);

        });
    });
});
