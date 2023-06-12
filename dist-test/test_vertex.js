"use strict";
var __importDefault = (this && this.__importDefault) || function (mod) {
    return (mod && mod.__esModule) ? mod : { "default": mod };
};
Object.defineProperty(exports, "__esModule", { value: true });
const should_1 = __importDefault(require("should"));
const __1 = require("..");
const __2 = require("..");
const doDebug = false;
describe("testing Vertex ", function () {
    describe("constructing a empty vertex ", function () {
        let vertex;
        before(function () {
            vertex = __1.occ.makeVertex();
        });
        it("should be (0,0,0)", function () {
            vertex.x.should.equal(0);
            vertex.y.should.equal(0);
            vertex.z.should.equal(0);
        });
    });
    describe("constructing a  vertex with a {x:..., y..,z: ...}", function () {
        let vertex;
        before(function () {
            vertex = __1.occ.makeVertex({ x: 10, y: 20, z: 30 });
        });
        it("should be (10,20,30)", function () {
            vertex.x.should.equal(10);
            vertex.y.should.equal(20);
            vertex.z.should.equal(30);
        });
    });
    describe("constructing a  vertex with  {x:..., y..,z: ...} (property in random order)", function () {
        let vertex;
        before(function () {
            vertex = __1.occ.makeVertex({ a: 10, y: 20, z: 30, x: 10 });
        });
        it("should be (10,20,30)", function () {
            vertex.x.should.equal(10);
            vertex.y.should.equal(20);
            vertex.z.should.equal(30);
        });
    });
    describe("constructing a vertex build by passing x,y,z coordinates to constructor", function () {
        let vertex;
        before(function () {
            vertex = __1.occ.makeVertex(10, 20, 30);
        });
        it("should be (10,20,30)", function () {
            vertex.x.should.equal(10);
            vertex.y.should.equal(20);
            vertex.z.should.equal(30);
        });
    });
    describe("constructing a vertex build by passing [x,y,z] coordinates to constructor", function () {
        let vertex;
        before(function () {
            vertex = __1.occ.makeVertex([10, 20, 30]);
        });
        it("should be (10,20,30)", function () {
            vertex.x.should.equal(10);
            vertex.y.should.equal(20);
            vertex.z.should.equal(30);
        });
        it("should be valid", function () {
            vertex.isValid.should.equal(true);
        });
    });
    describe("constructing a  vertex and applying a translation", function () {
        let vertex_org;
        before(function () {
            vertex_org = __1.occ.makeVertex([10, 20, 30]);
        });
        it("should be translated", function () {
            let vertex;
            vertex = vertex_org.translate([10, 20, 30]);
            vertex_org.x.should.equal(10);
            vertex_org.y.should.equal(20);
            vertex_org.z.should.equal(30);
            vertex.x.should.equal(20);
            vertex.y.should.equal(40);
            vertex.z.should.equal(60);
        });
        it("should be translated - second form ", function () {
            let vertex;
            vertex = vertex_org.translate(/*[*/ 10, 20, 30 /*]*/);
            vertex_org.x.should.equal(10);
            vertex_org.y.should.equal(20);
            vertex_org.z.should.equal(30);
            vertex.x.should.equal(20);
            vertex.y.should.equal(40);
            vertex.z.should.equal(60);
        });
        it("should be mirrored", function () {
            const trsf = __1.occ.makePlaneMirror([0, 0, 0], [0, 1, 0]);
            const vertex_dest = vertex_org.transformed(trsf);
            vertex_org.x.should.equal(10);
            vertex_org.y.should.equal(20);
            vertex_org.z.should.equal(30);
            vertex_dest.x.should.equal(10);
            vertex_dest.y.should.equal(-20);
            vertex_dest.z.should.equal(30);
        });
    });
    describe("edge cases: bad use of constructor shall not cause software to crash ", function () {
        it("Edge#constructor - should not crash if new is omitted", function () {
            (0, should_1.default)(function () {
                const tmp = /* new */ __2.Edge();
                tmp;
            }).throwError(" use new occ.Edge() to construct a Edge");
        });
        it("Vertex#constructor - should not crash if new is omitted", function () {
            (0, should_1.default)(function () {
                const tmp = /* new */ __2.Vertex(10, 20, 30);
                tmp;
            }).throwError(" use occ.makeVertex() to construct a Vertex");
        });
        it("Wire#constructor - should not crash if new is omitted", function () {
            (0, should_1.default)(function () {
                const tmp = /* new */ __2.Wire();
                tmp;
            }).throwError(" use new occ.Wire() to construct a Wire");
        });
        it("Solid#constructor - should not crash if new is omitted", function () {
            (0, should_1.default)(function () {
                const tmp = /* new */ __2.Solid();
                tmp;
            }).throwError(" use new occ.Solid() to construct a Solid");
        });
        it("BoundingBox#constructor - should not crash if new is omitted", function () {
            (0, should_1.default)(function () {
                const tmp = /* new */ __2.BoundingBox(10, 20, 30);
                tmp;
            }).throwError(" use new occ.BoundingBox() to construct a BoundingBox");
        });
        it("Vertex#constructor should not crash if wrong argument are provided", function () {
            const tmp = __1.occ.makeVertex({ x: 10, y: 20, z: 30 });
            tmp;
        });
    });
    describe("should provide a way to compare vertex", function () {
        it("should compare 2 vertices with same coordinates", function () {
            const vertex1 = __1.occ.makeVertex(10, 20, 30);
            const vertex2 = __1.occ.makeVertex(10, 20, 30);
            (0, should_1.default)(vertex1).eql(vertex2);
            if (doDebug) {
                console.log("vertex1 ", vertex1);
            }
            (0, should_1.default)(vertex1).containEql({ x: 10, y: 20, z: 30 });
        });
        it("should compare 2 vertices with different coordinates", function () {
            const vertex1 = __1.occ.makeVertex(10, 20, 30);
            const vertex2 = __1.occ.makeVertex(110, 220, 330);
            (0, should_1.default)(vertex1).not.eql(vertex2);
        });
    });
});
