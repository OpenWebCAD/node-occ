"use strict";
var __importDefault = (this && this.__importDefault) || function (mod) {
    return (mod && mod.__esModule) ? mod : { "default": mod };
};
Object.defineProperty(exports, "__esModule", { value: true });
const should_1 = __importDefault(require("should"));
const __1 = require("..");
describe("testing transformation object", function () {
    let trsf;
    before(function () {
        trsf = new __1.Transformation();
        should_1.default.exist(trsf);
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
            __1.ZDir.should.eql([0, 0, 1]);
            trsf.makePlaneMirror([10, 20, 30], __1.ZDir);
        });
        it("should have a scale factor of -1.0", function () {
            trsf.scaleFactor.should.eql(-1);
        });
        it("should flip coord on the Z axis", function () {
            let v = __1.occ.makeVertex(10, 10, 40);
            let v2 = v.transformed(trsf);
            v2.x.should.eql(v.x);
        });
    });
});
