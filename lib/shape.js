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

};
