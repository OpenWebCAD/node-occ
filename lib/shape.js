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


    occ.BoundingBox.prototype.toString = function ()
    {
        return "[ " +  this.nearPt.x.toFixed(3) +
                "," +  this.nearPt.y.toFixed(3) +
                "," +  this.nearPt.z.toFixed(3) +"]" +
            "[ " +  this.farPt.x.toFixed(3) +
            "," +  this.farPt.y.toFixed(3) +
            "," +  this.farPt.z.toFixed(3) +"]";
    }
};
