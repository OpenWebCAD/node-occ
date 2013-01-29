var crypto = require('crypto');
var fs = require('fs');
var occ = require('./occ');
var shapeFactory = require('../lib/shapeFactory');
var should = require('should');
var color = require('colors');


var proxyOCC = function()
{
    'use strict';
    this.resetCache();
};

proxyOCC.prototype.resetCache = function()
{
    'use strict';
    this.mapHit =0;
    this.mapQueryCount = 0;
    this.mapObject = {};
};

function hash(args)
{
    'use strict';
    var shasum = crypto.createHash('sha1');
    shasum.update(args);
    return shasum.digest("hex");

}

function calculateOperationHash(methodName,theArguments)
{
    'use strict';

    var toUUID = function(el) {
        return el.uuid;
    };

    var replaceObjectsByGUUID = function(arg)
    {
        if (arg.hasOwnProperty("uuid")) {

            // console.log(("arg" + arg).blue);
            return toUUID(arg);

        } else if ( arg.length >0 && typeof arg[0] ==  "number" ) {

            return arg;

        } else if ( arg.length >0 && arg[0].hasOwnProperty("uuid")) {

            console.log(" Found an array of object with guiid" ,("arg "+arg.map(function(el){return el.uuid})).yellow);
            var tmp = arg.map(toUUID);
            return tmp;

        } else if ( typeof arg == "number" ) {
            
            return arg;

        } else {
            
            // we can't use cache yet
            var tmp = arg;
            //.map(toUUID);
            console.log("####".brown , typeof arg,JSON.stringify(tmp));
            return tmp;

        }
        return arg;
    };

    // build the signature of the function + arguments
    //xx var args = theArguments.map(replaceObjectsByGUUID);
    var key;
    var args=[];
    for (key in theArguments) {
        if (theArguments.hasOwnProperty(key)) {
            args.push(replaceObjectsByGUUID(theArguments[key]));
        }
    }
    var argStr = JSON.stringify(args);
    var cmd = methodName+"(" + argStr.substr(1,argStr.length-2) +")";
    var uuid  = hash(cmd);

    return [uuid,cmd];
}



proxyOCC.prototype.replaceObjectMethod = function (obj,_methodName,_originalMethod)
{
    'use strict';
    // create closure variable
    var that = this;
    var methodName = _methodName;
    var originalMethod = _originalMethod;

    var performOperation = function()  {
        return that.__performedCachedOperation(this,originalMethod,methodName+"#"+ this.uuid,arguments);
    };

    obj[methodName] = performOperation;
};


proxyOCC.prototype.installProxyMethods = function(obj)
{
    'use strict';
    if (!obj) return;

    var functionToOverride= [ "translate", "mirror" , "rotate" , "clone" , "transformed" ,"fillet","chamfer"];
    
    var that = this;

    functionToOverride.forEach(function(func_name) {

        if (obj[func_name]) {
            that.replaceObjectMethod(obj,func_name,obj[func_name]);
        }

    });

};


proxyOCC.prototype.__performedCachedOperation = function(originalObject,originalMethod,methodName, theArguments)
{
    var h = calculateOperationHash(methodName,theArguments);
    var uuid  = h[0];
    var cmd = h[1];

    // check if object is already known
    this.mapQueryCount++;
    if (this.mapObject.hasOwnProperty(uuid)) {
        console.log(" uuid = ", uuid.green , "cmd =", cmd);
        this.mapHit++;
        return this.mapObject[uuid];
    }
    console.log(" uuid = ", uuid.red , "cmd =", cmd);

    // otherwise perform the 'costly' original operation
    //  console.log("originalObject.uuid=",originalObject.uuid ,methodName,theArguments);
    var obj = originalMethod.apply(originalObject,theArguments);

    // store the resulting object in cache
    obj.cmd = cmd;
    obj.uuid = uuid;
    this.mapObject[obj.uuid] =obj;
    // prepare the resulting object
    this.installProxyMethods(obj);
    return obj;
};

proxyOCC.prototype.__proxy = function (originalObject,methodName,theArguments)
{
    'use strict';
    return this.__performedCachedOperation(originalObject,originalObject[methodName],methodName,theArguments);
};

proxyOCC.prototype.makeBox        = function() { 'use strict';return this.__proxy(occ, "makeBox",       arguments); };
proxyOCC.prototype.makeCylinder   = function() { 'use strict';return this.__proxy(occ, "makeCylinder",  arguments); };
proxyOCC.prototype.makeCone       = function() { 'use strict';return this.__proxy(occ, "makeCone",      arguments); };
proxyOCC.prototype.makeSphere     = function() { 'use strict';return this.__proxy(occ, "makeSphere",    arguments); };
proxyOCC.prototype.makeTorus      = function() { 'use strict';return this.__proxy(occ, "makeTorus",     arguments); };
proxyOCC.prototype.makeThickSolid = function() { 'use strict';return this.__proxy(occ, "makeThickSolid",arguments); };
proxyOCC.prototype.makeDraft      = function() { 'use strict';return this.__proxy(occ, "makeDraft",     arguments); };

proxyOCC.prototype.fuse           = function() { 'use strict';return this.__proxy(occ, "fuse",          arguments); };
proxyOCC.prototype.cut            = function() { 'use strict';return this.__proxy(occ, "cut",           arguments); };
proxyOCC.prototype.common         = function() { 'use strict';return this.__proxy(occ, "common",        arguments); };
proxyOCC.prototype.compound       = function() { 'use strict';return this.__proxy(occ, "compound",      arguments); };

proxyOCC.prototype.makeBottle       = function() { 'use strict';return this.__proxy(shapeFactory, "makeBottle",      arguments); };

exports.fastBuilder = new proxyOCC();
exports.calculateOperationHash = calculateOperationHash;