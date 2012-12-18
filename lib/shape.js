var occ = require('./bindings');

exports.init= function() {

    occ.Solid.prototype.getEdges = function() {
        var edges=[];
        var iterator = new occ.ShapeIterator(this,"EDGE");
        while(iterator.more) { edges.push(iterator.next());  }
        return edges;
    }
    occ.Face.prototype.getEdges = occ.Solid.prototype.getEdges;
    occ.Wire.prototype.getEdges = occ.Solid.prototype.getEdges;

};
