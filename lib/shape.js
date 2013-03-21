var assert = require('assert');
var occ = require('./bindings');

exports.init= function() {

    occ.Solid.prototype.getAllEdges = function() {
        // deprecated : use Solid.getEdgesi !
        var edges=[];
        var iterator = new occ.ShapeIterator(this,"EDGE");
        while(iterator.more) { edges.push(iterator.next());  }
        return edges;
    };
    occ.Face.prototype.getAllEdges = occ.Solid.prototype.getAllEdges;
    occ.Wire.prototype.getAllEdges = occ.Solid.prototype.getAllEdges;


    occ.BoundingBox.prototype.toString = function () {
        return "[ " +  this.nearPt.x.toFixed(3) +
                "," +  this.nearPt.y.toFixed(3) +
                "," +  this.nearPt.z.toFixed(3) +"]" +
            "[ " +  this.farPt.x.toFixed(3) +
            "," +  this.farPt.y.toFixed(3) +
            "," +  this.farPt.z.toFixed(3) +"]";
    };

    occ.buildSolidMesh = function(solid) {

        assert(solid.hasOwnProperty('name'), "occ.buildSolidMesh : the solid must have a name");

        // make sure object is meshed
        var mesh = solid.mesh;

        // produce each faces
        var faces = solid.getFaces();
        var face;

        var jsonSolidMesh = { name: solid.name, faces: [] , edges: []};
        var i,entry;

        for (i=0;i<faces.length;i++) {
            face = faces[i];

            if (! face.hasMesh) {
                continue;
            }

            entry =  {
                name: solid.getShapeName(face),
                // color:(r*255+g)*255+b,
                mesh: face.mesh.toJSON()
            };
            entry.mesh.materials[0].colorDiffuse = [ Math.random(),Math.random(),Math.random()];
            jsonSolidMesh.faces.push( entry);
        }
        // produce each edge

        var edges = solid.getEdges();
        var edge;
        for (i=0;i<edges.length;i++) {
            edge = edges[i];
            
            if (edge.isSeam) continue;

            var polygone = edge.polygonize();
            var pts = [];
            for (var j=0;j<polygone.length;j++) {
                pts.push(polygone[j]);
            }
            entry =  {
                    name: solid.getShapeName(edge),                
                    mesh: pts
            };
            //entry.mesh.materials[0].colorDiffuse = [ Math.random(),Math.random(),Math.random()];  
            jsonSolidMesh.edges.push( entry);   
        }          
        return jsonSolidMesh;
    };


};
