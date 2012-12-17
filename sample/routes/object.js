

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