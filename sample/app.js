/**
 * Module dependencies.
 */

var express = require('express')
// middleware
    , compress = require("compression")
    , bodyParser = require("body-parser")
    , favicon = require("static-favicon")
    , logger = require("morgan") // logger
    , methodOverride = require("method-override")
//xx    , cookieParser = require("cookie-parser")
    , expressSession = require("express-session")
//xx    , multipart = require('connect-multiparty')
    , errorHandler = require("errorhandler")
    , routes = require('./routes')
    , user = require('./routes/user')
    , object = require('./routes/object')
    , http = require('http')
    , path = require('path')
    , fs = require("fs")
    , cors = require('cors')
    ;

var app = express();


var port = parseInt(process.env.PORT) || 3000;
app.set('port', port);
app.set('views', __dirname + '/views');
app.set('view engine', 'ejs');



// parse application/x-www-form-urlencoded
app.use(bodyParser.urlencoded({extended: true}));
app.use(bodyParser.json());

//xx multi part middleware
//xx app.use(multipart());

// parse application/json
//xx app.use(multer);

//xx app.use(methodOverride());
//xx app.use(compress());
//xx app.use(favicon);
app.use(logger("combined"));
//xx //xx app.use(cookieParser('your secret here'));
app.use(expressSession({
    secret: "your secret here",
    resave: false,
    saveUninitialized: false
}));

app.use(express.static(path.join(__dirname, 'public')));
app.use(express.static(path.join(__dirname, 'client')));
app.use(express.static(path.join(__dirname, '../lib'))); // to get CSGTree.js


var env = process.env.NODE_ENV || 'development';
if ('development' == env) {
    app.use(errorHandler());
}

app.use(cors());

app.get('/toto', function (req, res) {
    res.send('Hello World!');
});

app.get('/test', function (req, res) {
    res.render("sample", {});
});

app.get('/viewer', function (req, res) {

    console.log(" viewer requested");

    res.render("viewer", {});
});

app.get('/', routes.index);
app.post('/csg1', object.buildCSG1);
app.post('/load_cadfile', object.load_cadfile);


var format = require('util').format;

var formidable = require("formidable");

app.post('/file-upload', function (req, res) {

    var form = new formidable.IncomingForm();
    // specify that we want to allow the user to upload multiple files in a single request
    form.multiples = true;
    // store all uploads in the /uploads directory
    form.uploadDir = path.join(__dirname, "uploads");


    var jsonResult = {files: []};
    form.on('fileBegin', function (name, file) {
    });

    form.on('file', function (name, file) {
        console.log('Uploaded ' + file.name);
        jsonResult.files.push({
            "name": file.name,
            "size": file.size,
            "path": path.posix.join("/uploads", path.basename(file.name))
        });

        fs.rename(file.path, path.join(form.uploadDir, file.name));
    });

    form.on('error', function (err) {
        console.log('An error has occurred: \n' + err);
    });

    form.on('end', function () {
    });

    form.parse(req, function (err, fields, files) {

        res.send(jsonResult);
    });

});


// http.createServer(app).listen(app.get('port'), function(){
app.listen(app.get('port'), function () {
    console.log("Express server listening on port " + app.get('port'));
});
