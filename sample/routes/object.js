var occ  = require('../../lib/occ')
  , CSGBuilder = require('../../lib/CSGBuilder');

exports.get = function(req, res) {

    var obj = database.findObject(req.params.id);

    res.format({
        text: function(){
            res.send('hey : ' + JSON.stringify(obj,null," "));
        },

        html: function(){
            res.send('<p>hey</p>'  + JSON.stringify(obj,null," ") + " params = " +  req.params.id + " " +JSON.stringify(req.params) + " query = " +  JSON.stringify(req.query)  );
        },

        json: function(){
            res.send(obj);
        }
    });
  // res.send('user ' + req.params.id);
}
exports.list = function(req, res) {


    res.format({
        text: function(){
            res.send('hey : ' + JSON.stringify(database.solidMap,null," "));
        },

        html: function(){
            res.send('<p>hey</p>'  + JSON.stringify(database.solidMap,null," "));
        },

        json: function(){
            res.send(database.solidMap);
        }
    });
}


exports.buildCSG = function(req,res)
{
    var jsonStr = ""
    var csgStr = CSGBuilder.sample_csg_tree;
    csgStr = req.body;

    try {
        var solid  = CSGBuilder.build(csgStr);
        var mesh   = solid.mesh;
        jsonStr = mesh.toJSON();
    }
    catch(err) {
        res.send(501,"Error building solid : "+ err.message + err.toString());
        return;
    }
    res.send(jsonStr);

}