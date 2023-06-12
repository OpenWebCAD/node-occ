import should from "should";
import { IVertex, occ } from "..";
import { Vertex, Edge, Wire, Solid, BoundingBox, makeVertex } from "..";
const doDebug = false;


describe("testing Vertex ", function () {

    describe("constructing a empty vertex ", function () {
        let vertex: IVertex;
        before(function () {
            vertex = occ.makeVertex();
        });
        it("should be (0,0,0)", function () {
            vertex.x.should.equal(0);
            vertex.y.should.equal(0);
            vertex.z.should.equal(0);
        });
    });
    describe("constructing a  vertex with a {x:..., y..,z: ...}", function () {
        let vertex: IVertex;
        before(function () {
            vertex = occ.makeVertex({ x: 10, y: 20, z: 30 });
        });
        it("should be (10,20,30)", function () {
            vertex.x.should.equal(10);
            vertex.y.should.equal(20);
            vertex.z.should.equal(30);
        });
    });
    describe("constructing a  vertex with  {x:..., y..,z: ...} (property in random order)", function () {
        let vertex: IVertex;
        before(function () {
            vertex = occ.makeVertex({ a: 10, y: 20, z: 30, x: 10 } as any);
        });
        it("should be (10,20,30)", function () {
            vertex.x.should.equal(10);
            vertex.y.should.equal(20);
            vertex.z.should.equal(30);
        });
    });

    describe("constructing a vertex build by passing x,y,z coordinates to constructor", function () {
        let vertex: IVertex;
        before(function () {
            vertex = occ.makeVertex(10, 20, 30);
        });
        it("should be (10,20,30)", function () {
            vertex.x.should.equal(10);
            vertex.y.should.equal(20);
            vertex.z.should.equal(30);
        });

    });
    describe("constructing a vertex build by passing [x,y,z] coordinates to constructor", function () {
        let vertex: IVertex;
        before(function () {
            vertex = occ.makeVertex([10, 20, 30]);
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
        let vertex_org: IVertex;
        before(function () {
            vertex_org = occ.makeVertex([10, 20, 30]);
        });

        it("should be translated", function () {
            let vertex: IVertex;
            vertex = vertex_org.translate([10, 20, 30]);

            vertex_org.x.should.equal(10);
            vertex_org.y.should.equal(20);
            vertex_org.z.should.equal(30);

            vertex.x.should.equal(20);
            vertex.y.should.equal(40);
            vertex.z.should.equal(60);
        });
        it("should be translated - second form ", function () {
            let vertex: IVertex;
            vertex = vertex_org.translate(/*[*/10, 20, 30/*]*/);

            vertex_org.x.should.equal(10);
            vertex_org.y.should.equal(20);
            vertex_org.z.should.equal(30);

            vertex.x.should.equal(20);
            vertex.y.should.equal(40);
            vertex.z.should.equal(60);
        });
        it("should be mirrored", function () {

            const trsf = occ.makePlaneMirror([0, 0, 0], [0, 1, 0]);

            const vertex_dest = vertex_org.transformed(trsf);
            vertex_org.x.should.equal(10);
            vertex_org.y.should.equal(20);
            vertex_org.z.should.equal(30);

            vertex_dest.x.should.equal(10);
            vertex_dest.y.should.equal(-20);
            vertex_dest.z.should.equal(30);

        })
    });


    describe("edge cases: bad use of constructor shall not cause software to crash ", function () {

        it("Edge#constructor - should not crash if new is omitted", function () {
            should(function () {
                const tmp = /* new */ (Edge as any)();
                tmp;
            }).throwError(" use new occ.Edge() to construct a Edge");
        });
        it("Vertex#constructor - should not crash if new is omitted", function () {
            should(function () {
                const tmp = /* new */ (Vertex as any)(10, 20, 30);
                tmp;
            }).throwError(" use occ.makeVertex() to construct a Vertex");
        });
        it("Wire#constructor - should not crash if new is omitted", function () {
            should(function () {
                const tmp = /* new */ (Wire as any)();
                tmp;
            }).throwError(" use new occ.Wire() to construct a Wire");
        });
        it("Solid#constructor - should not crash if new is omitted", function () {
            should(function () {
                const tmp = /* new */ (Solid as any)();
                tmp;
            }).throwError(" use new occ.Solid() to construct a Solid");
        });
        it("BoundingBox#constructor - should not crash if new is omitted", function () {
            should(function () {
                const tmp = /* new */ (BoundingBox as any)(10, 20, 30);
                tmp;
            }).throwError(" use new occ.BoundingBox() to construct a BoundingBox");
        });

        it("Vertex#constructor should not crash if wrong argument are provided", function () {
            const tmp = makeVertex({ x: 10, y: 20, z: 30 });
            tmp;
        });
    });


    describe("should provide a way to compare vertex", function () {
        it("should compare 2 vertices with same coordinates", function () {
            const vertex1 = occ.makeVertex(10, 20, 30);
            const vertex2 = occ.makeVertex(10, 20, 30);
            should(vertex1).eql(vertex2);
            if (doDebug) {
                console.log("vertex1 ", vertex1);
            }
            should(vertex1).containEql({ x: 10, y: 20, z: 30 });

        });
        it("should compare 2 vertices with different coordinates", function () {
            const vertex1 = occ.makeVertex(10, 20, 30);
            const vertex2 = occ.makeVertex(110, 220, 330);
            should(vertex1).not.eql(vertex2);
        });
    });
});
