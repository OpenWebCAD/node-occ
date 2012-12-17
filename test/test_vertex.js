var assert = require("assert");
var should = require("should");

var occ = require("../lib/occ");


describe("testing Vertex ",function(){


	describe("constructing a empty vertex " , function() {
        var vertex;
        before(function () {
            vertex = new occ.Vertex();
        });
        it("should be (0,0,0)", function() {
            vertex.x.should.equal(0);
            vertex.y.should.equal(0);
            vertex.z.should.equal(0);
        });

    });
    describe("constructing a vertex build by passing x,y,z coordinates to constructor" , function() {
        var vertex;
        before(function() {
            vertex = new occ.Vertex(10,20,30);
        });
        it("should be (10,20,30)", function() {
            vertex.x.should.equal(10);
            vertex.y.should.equal(20);
            vertex.z.should.equal(30);
        });

    });
    describe("constructing a vertex build by passing [x,y,z] coordinates to constructor" , function() {
        var vertex;
        before(function() {
            vertex = new occ.Vertex([10,20,30]);
        });
        it("should be (10,20,30)", function() {
            vertex.x.should.equal(10);
            vertex.y.should.equal(20);
            vertex.z.should.equal(30);
        });
        it("should be valid", function() {
            vertex.isValid.should.equal(true)
        });
    });
    describe("constructing a  vertex and applying a translation",function(){
        var vertex;
        before(function() {
            vertex = new occ.Vertex([10,20,30]);
            vertex.translate([10,20,30])
        });
        it("should be translated",function(){
            vertex.x.should.equal(20);
            vertex.y.should.equal(40);
            vertex.z.should.equal(60);
       });
    });
});
