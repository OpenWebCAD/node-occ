var crypto = require('crypto');
var fs = require('fs');
var occ = require('./occ');
var should = require('should');
var color = require('colors');


var proxyOCC = function()
{
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

    // build the signature of the function+arguments
    var args = [];
    var key;
    for (key in theArguments) {
        if (theArguments.hasOwnProperty(key)) {
            var arg = theArguments[key];
            if (arg.hasOwnProperty("uuid")) {
                arg = arg.uuid;
            }
            args.push(arg);
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
    var t;
    for ( t in functionToOverride) {
        var func_name = functionToOverride[t];
        if (obj[func_name]) {
            this.replaceObjectMethod(obj,func_name,obj[func_name]);
        }
    }

}


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

proxyOCC.prototype.__proxy = function (orignalMethod,methodName,theArguments)
{
    'use strict';
    return this.__performedCachedOperation(occ,orignalMethod,methodName,theArguments);
};

proxyOCC.prototype.makeBox      = function() { 'use strict';return this.__proxy(occ.makeBox,"occ.makeBox",arguments);};
proxyOCC.prototype.makeCylinder = function() { 'use strict';return this.__proxy(occ.makeCylinder,"occ.makeCylinder",arguments); };
proxyOCC.prototype.makeCone     = function() { 'use strict';return this.__proxy(occ.makeCone,"occ.makeCone",arguments); };
proxyOCC.prototype.makeSphere   = function() { 'use strict';return this.__proxy(occ.makeSphere,"occ.makeSphere",arguments); };
proxyOCC.prototype.fuse         = function() { 'use strict';return this.__proxy(occ.fuse,"occ.fuse",arguments); };
proxyOCC.prototype.cut          = function() { 'use strict';return this.__proxy(occ.cut,"occ.cut",arguments); };
proxyOCC.prototype.common       = function() { 'use strict';return this.__proxy(occ.common,"occ.common",arguments); };

exports.fastBuilder = new proxyOCC();