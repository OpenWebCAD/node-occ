
/**
 * Module dependencies.
 */

var express = require('express')
  , routes = require('./routes')
  , user = require('./routes/user')
    , object = require('./routes/object')
  , http = require('http')
  , path = require('path')
  , occ  = require('../build/Release/occ');

var app = express();


var OCEDatabase = function() {

    this.solidMap = {}

    this.findObject = function(id) {
        return this.solidMap[id];
    }

    this.addSolid = function(id)
    {
        var s = new occ.Solid();
        s.makeBox([10,20,30],[10,40,60]);
        this.solidMap[id] = s;
    }
    this.solidMap[1] = { hello: "world"};
    this.addSolid(2);

}
database = new OCEDatabase()

app.configure(function(){
  app.set('port', process.env.PORT || 3000);
  app.set('views', __dirname + '/views');
  app.set('view engine', 'ejs');
  app.use(express.favicon());
  app.use(express.logger('dev'));
  app.use(express.bodyParser());
  app.use(express.methodOverride());
  app.use(express.cookieParser('your secret here'));
  app.use(express.session());
  app.use(app.router);
  app.use(express.static(path.join(__dirname, 'public')));
});

app.configure('development', function(){
  app.use(express.errorHandler());
});

app.get('/', routes.index);
app.get('/objects(""|\.json|\.html)', object.list);
app.get('/object/:id', object.get);
app.post('/object', object.get);

http.createServer(app).listen(app.get('port'), function(){
  console.log("Express server listening on port " + app.get('port'));
});
