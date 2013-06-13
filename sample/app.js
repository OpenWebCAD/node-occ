
/**
 * Module dependencies.
 */

var express = require('express')
  , routes = require('./routes')
  , user = require('./routes/user')
  , object = require('./routes/object')
  , http = require('http')
  , path = require('path')
  , occ  = require('../')
  , CSGBuilder = require('../lib/CSGBuilder');

var app = express();


app.configure(function(){
  app.set('port', process.env.PORT || 3000);
  app.set('views', __dirname + '/views');
  app.set('view engine', 'ejs');
  app.use(express.compress());
  app.use(express.favicon());
  app.use(express.logger('dev'));
  app.use(express.bodyParser());
  app.use(express.methodOverride());
  app.use(express.cookieParser('your secret here'));
  app.use(express.session());
  app.use(app.router);
  app.use(express.static(path.join(__dirname, 'public')));
  app.use(express.static(path.join(__dirname, 'client')));
  app.use(express.static(path.join(__dirname, '../lib'))); // to get CSGTree.js
});

app.configure('development', function(){
  app.use(express.errorHandler());
});

app.get('/test', function(req,res) {
   res.render("sample",{});
});

app.get('/viewer', function(req,res) {
   res.render("viewer",{});
});

app.get('/', routes.index);
app.post('/csg', object.buildCSG);
app.post('/csg1', object.buildCSG1);
app.post('/load_cadfile',object.load_cadfile);

var format = require('util').format;
app.post('/file-upload',function(req, res,next) {

    var jsonResult = { "files": [ ]};

    for (var f in req.files ) {
      jsonResult.files.push(
      {
        "name": req.files.upload.name,
        "size": req.files.upload.size,
        "path": req.files.upload.path,
        "url": "http:\/\/example.org\/files\/" + req.files.upload.name,
        "thumbnail_url": "http:\/\/example.org\/files\/thumbnail\/picture1.jpg",
        "delete_url": "http:\/\/example.org\/files\/picture1.jpg",
        "delete_type": "DELETE"
      });
    }


    console.log("done", format('\nuploaded %s (%d Kb) to %s as %s'
      , req.files.upload.name
      , req.files.upload.size / 1024 | 0 
      , req.files.upload.path
      , req.body.title));

  res.send(jsonResult);

});


http.createServer(app).listen(app.get('port'), function(){
  console.log("Express server listening on port " + app.get('port'));
});
