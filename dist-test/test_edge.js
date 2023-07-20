"use strict";
var __importDefault = (this && this.__importDefault) || function (mod) {
    return (mod && mod.__esModule) ? mod : { "default": mod };
};
Object.defineProperty(exports, "__esModule", { value: true });
const should_1 = __importDefault(require("should"));
const __1 = require("..");
const { makeLine } = __1.occ;
describe("testing Edges ", function () {
    describe("EDGE0 - constructing an empty Edge", function () {
        let edge;
        before(function () {
            edge = new __1.Edge();
            should_1.default.exist(edge);
        });
        it("should be valid", function () {
            edge.isNull.should.equal(true);
        });
        it("should have a zero length", function () {
            edge.length.should.equal(0);
        });
        it("should have a zero vertices", function () {
            edge.numVertices.should.equal(0);
        });
        it("should be degenerated", function () {
            edge.isDegenerated.should.equal(true);
        });
        it("shouldn't be closed", function () {
            edge.isClosed.should.equal(false);
        });
        it("should provide a bounding box", function () {
            edge.getBoundingBox().should.be.instanceOf(__1.BoundingBox);
            edge.getBoundingBox().isVoid.should.eql(true);
        });
    });
    describe("Wire1 - an Wire constructed as  as a linear Segment between (10,20,30) and (-30,20,30) ", function () {
        let wire;
        before(function () {
            let v1 = new __1.Vertex(10, 20, 30);
            let v2 = new __1.Vertex(-30, 20, 30);
            wire = makeLine(v1, v2);
            wire.should.be.instanceOf(__1.Edge);
        });
        it("should have a length of 40.0 ", function () {
            wire.length.should.equal(40.0);
        });
        it("should have two vertices ", function () {
            wire.numVertices.should.equal(2.0);
        });
        it("shouldn't be closed", function () {
            wire.isClosed.should.equal(false);
        });
        it("shouldn't be degenerated", function () {
            wire.isDegenerated.should.equal(false);
        });
        it("should provide a bounding box", function () {
            wire.getBoundingBox().should.be.instanceOf(__1.BoundingBox);
            wire.getBoundingBox().isVoid.should.eql(false);
            wire.getBoundingBox().nearPt.equals(new __1.Point(-30, 20, 30)).should.eql(true);
            wire.getBoundingBox().nearPt.equals([-30, 20, 30]).should.eql(true);
            wire.getBoundingBox().farPt.equals([10, 20, 30]).should.eql(true);
            let extra = 0.000000000001;
            wire.getBoundingBox().farPt.asArray().should.eql([10 + extra, 20 + extra, 30 + extra]);
        });
        it("should polygonize a segment with two points", function () {
            console.log("polyligonize");
            let polyline = wire.polygonize();
            console.log("polyligonize2");
            polyline.length.should.eql(3 * 2);
            [polyline[0], polyline[1], polyline[2]].should.eql([10, 20, 30]);
            let l = polyline.length;
            (l % 3).should.eql(0);
            [polyline[l - 3], polyline[l - 2], polyline[l - 1]].should.eql([-30, 20, 30]);
        });
    });
    describe("EDGE2 - an Edge constructed as  as a linear Segment between (10,20,30) and (-30,20,30)  and translated by [1,2,3]", function () {
        let wire;
        before(function () {
            let v1 = __1.occ.makeVertex(10, 20, 30);
            let v2 = __1.occ.makeVertex(-30, 20, 30);
            wire = __1.occ.makeLine(v1, v2);
            wire = wire.translate([1, 2, 3]);
        });
        it("should have a length of 40.0 ", function () {
            wire.length.should.equal(40.0);
        });
        it("should provide a bounding box", function () {
            wire.getBoundingBox().nearPt.equals([-29, 22, 33]).should.eql(true);
            wire.getBoundingBox().farPt.equals([11, 22, 33]).should.eql(true);
        });
        it("should polygonize a segment with two points", function () {
            let polyline = wire.polygonize();
            polyline.length.should.eql(3 * 2);
            [polyline[0], polyline[1], polyline[2]].should.eql([11, 22, 33]);
            let l = polyline.length;
            (l % 3).should.eql(0);
            [polyline[l - 3], polyline[l - 2], polyline[l - 1]].should.eql([-29, 22, 33]);
        });
    });
    describe("Wire - an Wire constructed as a Circle on the Z+ plan with a radius of 20", function () {
        let wire;
        before(function () {
            wire = __1.occ.makeCircle([10, 10, 10], [0, 0, 1], 20);
        });
        it("should have a length of 2*PI*20.0 ", function () {
            let epsilon = 1E-2;
            let PI = 3.1415;
            wire.length.should.be.within(2 * PI * 20.0 - epsilon, 2 * PI * 20.0 + epsilon);
        });
        it("should have a unique vertex ", function () {
            wire.numVertices.should.equal(1);
        });
        it("should be closed", function () {
            wire.isClosed.should.equal(true);
        });
        it("shouldn't be degenerated", function () {
            wire.isDegenerated.should.equal(false);
        });
        it("should provide a bounding box", function () {
            wire.getBoundingBox().should.be.instanceOf(__1.BoundingBox);
            wire.getBoundingBox().isVoid.should.eql(false);
            //xx console.log(JSON.stringify(edge.getBoundingBox()));//.toString());
            wire.getBoundingBox().nearPt.equals([-11.647844, -11.647844, 10]);
            wire.getBoundingBox().farPt.equals([31.647844, 31.647844, 10]);
        });
        it("should polygonize a edge", function () {
            let a = wire.polygonize();
            [a[0], a[1], a[2]].should.eql([30, 10, 10]);
            let l = a.length;
            (l % 3).should.eql(0);
            [a[l - 3], a[l - 2], a[l - 1]].should.eql([30, 10, 10]);
            //xx console.log(a);
        });
    });
    describe("EDGE4 - an  Edge constructed as  as a linear Segment between (10,20,30) and (-30,20,30) ", function () {
    });
});
