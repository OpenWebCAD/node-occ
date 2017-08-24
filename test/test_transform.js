const should = require("should");
const occ = require("../lib/occ");


describe("testing transformation object", function () {

    let trsf;
    before(function () {
        trsf = new occ.Transformation();
        should.exist(trsf);
    });
    describe("a empty transformation", function () {
        it("should have a scale factor of 1.0", function () {
            trsf.scaleFactor.should.equal(1.0);
        });
        it("should have a rotation axis of 0,0,1", function () {
            trsf.scaleFactor.should.equal(1.0);
        });
    });

    describe("testing translation [10,20,30]", function () {
        before(function () {
            trsf.makeTranslation([10, 20, 30]);
        });
        it("should have a scale factor of 1.0", function () {
            trsf.scaleFactor.should.equal(1.0);
        });
        /*
        it("should have a rotation axis of 0,0,1", function() {
            trsf.rotationAxis.i.should.equal(0.0);
            trsf.rotationAxis.j.should.equal(0.0);
            trsf.rotationAxis.k.should.equal(1.0);
        });
        it("should have a translationPart at [10,20,30]", function() {
            trsf.translationPart.x.should.equal(10.0);
            trsf.translationPart.y.should.equal(20.0);
            trsf.translationPart.z.should.equal(30.0);
        });
        */
    });
    describe("testing planeMirror o=[10,20,30] dir=[0,0,1", function () {

        before(function () {

            occ.ZDir.should.eql([0, 0, 1]);

            trsf.makePlaneMirror([10, 20, 30], occ.ZDir);

        });

        it("should have a scale factor of -1.0", function () {
            trsf.scaleFactor.should.eql(-1);
        });

        it("should flip coord on the Z axis", function () {
            let v = occ.makeVertex(10, 10, 40);
            let v2 = v.transformed(trsf);
            v2.x.should.eql(v.x);
        })
    });
});
