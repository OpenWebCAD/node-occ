var assert = require("assert");
var should = require("should");

var occ = require("../lib/occ");





describe("testing Edges ",function(){

    describe("constructing an empty Edge",function(){
        var edge;
        before(function(){
          edge = new occ.Edge();
        });
        it("should be valid", function(){
            edge.isNull.should.equal(true);
        });
        it("should have a zero length", function(){
            edge.length.should.equal(0);
        });
        it("should have a zero vertices", function(){
            edge.numVertices.should.equal(0);
        });
        it("should be degenerated", function(){
            edge.isDegenerated.should.equal(true);
        });
        it("shouldn't be closed", function(){
            edge.isClosed.should.equal(false);
        });
        it("should provide a bounding box", function () {
            edge.getBoundingBox().should.be.instanceOf(occ.BoundingBox);
            edge.getBoundingBox().isVoid.should.eql(true);
        });
    });
    describe("an  Edge constructed as  as a linear Segment between (10,20,30) and (-30,20,30) ",function(){
        var edge;
        before(function(){
            var v1 = new occ.Vertex(10,20,30);
            var v2 = new occ.Vertex(-30,20,30);
            edge = occ.createLine(v1, v2);
            edge.should.be.instanceOf(occ.Edge);

        });
        it("should have a length of 40.0 ", function() {
            edge.length.should.equal(40.0);
        });
        it("should have two vertices ", function() {
            edge.numVertices.should.equal(2.0);
        });
        it("shouldn't be closed", function() {
            edge.isClosed.should.equal(false);
        });
        it("shouldn't be degenerated", function() {
            edge.isDegenerated.should.equal(false);
        });
        it("should provide a bounding box", function () {
            edge.getBoundingBox().should.be.instanceOf(occ.BoundingBox);
            edge.getBoundingBox().isVoid.should.eql(false);

            edge.getBoundingBox().nearPt.equals(new occ.Point3D(-30, 20, 30)).should.eql(true);
            edge.getBoundingBox().nearPt.equals([-30, 20, 30]).should.eql(true);
            edge.getBoundingBox().farPt.equals([10, 20, 30]).should.eql(true);

            var extra = 0.000000000001;
            edge.getBoundingBox().farPt.asArray().should.eql([10 + extra, 20 + extra, 30 + extra]);
        });
        it("should polygonize a segment with two points", function () {

            var polyline = edge.polygonize();
            polyline.length.should.eql(3 * 2);
            [polyline[0], polyline[1], polyline[2]].should.eql([10, 20, 30]);
            var l = polyline.length;

            (l % 3).should.eql(0);
            [polyline[l - 3], polyline[l - 2], polyline[l - 1]].should.eql([-30, 20, 30]);
        });
    });

    describe("an Edge constructed as  as a linear Segment between (10,20,30) and (-30,20,30)  and translated by [1,2,3]", function () {
        var edge;
        before(function () {
            var v1 = new occ.Vertex(10, 20, 30);
            var v2 = new occ.Vertex(-30, 20, 30);
            edge = occ.createLine(v1, v2);
            edge = edge.translate([1, 2, 3]);
        });
        it("should have a length of 40.0 ", function () {
            edge.length.should.equal(40.0);
        });
        it("should provide a bounding box", function () {
            edge.getBoundingBox().nearPt.equals([-29, 22, 33]).should.eql(true);
            edge.getBoundingBox().farPt.equals([11, 22, 33]).should.eql(true);
        });
        it("should polygonize a segment with two points", function () {
            var polyline = edge.polygonize();
            polyline.length.should.eql(3 * 2);
            [polyline[0], polyline[1], polyline[2]].should.eql([11, 22, 33]);
            var l = polyline.length;
            (l % 3).should.eql(0);
            [polyline[l - 3], polyline[l - 2], polyline[l - 1]].should.eql([-29, 22, 33]);
        });

    });
    describe("an Edge constructed as a Circle on the Z+ plan with a radius of 20", function () {
        var edge;
        before(function(){
            edge = occ.createCircle([10, 10, 10], [0, 0, 1], 20);
        });

        it("should have a length of 2*PI*20.0 ", function(){
            var epsilon = 1E-2;
            var PI = 3.1415;
            edge.length.should.be.within(2*PI*20.0-epsilon,2*PI*20.0+epsilon);
        });
        it("should have a unique vertex ", function(){
            edge.numVertices.should.equal(1);
        });
        it("should be closed", function(){
            edge.isClosed.should.equal(true);
        });
        it("shouldn't be degenerated", function(){
            edge.isDegenerated.should.equal(false);
        });

        it("should provide a bounding box", function () {
            edge.getBoundingBox().should.be.instanceOf(occ.BoundingBox);
            edge.getBoundingBox().isVoid.should.eql(false);
            //xx console.log(JSON.stringify(edge.getBoundingBox()));//.toString());
            edge.getBoundingBox().nearPt.equals([-11.647844, -11.647844, 10]).should.eql(true);
            edge.getBoundingBox().farPt.equals([31.647844, 31.647844, 10]).should.eql(true);
        });

        it("should polygonize a edge", function () {

            var a = edge.polygonize();

            [a[0], a[1], a[2]].should.eql([30, 10, 10]);
            var l = a.length;

            (l % 3).should.eql(0);
            [a[l - 3], a[l - 2], a[l - 1]].should.eql([30, 10, 10]);
            //xx console.log(a);

        });
    });
    describe("an  Edge constructed as  as a linear Segment between (10,20,30) and (-30,20,30) ", function () {

    });
});
