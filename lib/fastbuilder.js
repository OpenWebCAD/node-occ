var crypto = require('crypto');
var fs = require('fs');
var occ = require('./occ');
var shapeFactory = require('../lib/shapeFactory');
var should = require('should');
var color = require('colors');
var assert = require('assert');

var Cache = function()
{
    'use strict';
    this.resetCache();
};

Cache.prototype.resetCache = function()
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

    /**
     * 
     */
    var replaceObjectsByGUUID = function(arg)
    {
        if (!arg) {
            return null;
        }
        if (arg.hasOwnProperty("uuid")) {

            return toUUID(arg);

        } else if ( arg.length >0 && typeof arg[0] ==  "number" ) {

            return arg;

        } else if ( arg.length >0 && ( arg[0] instanceof Object ) && arg[0].hasOwnProperty("uuid")) {

            console.log(" Found an array of object with guiid" ,("arg "+arg.map(function(el){return el.uuid})).yellow);

            return arg.map(replaceObjectsByGUUID);

        } else if ( typeof arg == "number" ) {
            
            return arg;

        } else {
            
            if (arg.hashCode) {
                return hash("---"+arg.hashCode);
            }
            // we can't use cache yet
            var tmp = arg;
            //.map(toUUID);
            console.log(" ERROR : Cannot use cache with object ".red , typeof arg,JSON.stringify(tmp));
            return tmp;

        }
        return arg;
    };

    // build the signature of the function + arguments

    var key;
    var args=[];
    for (key in theArguments) {
        if (theArguments.hasOwnProperty(key)) {
            args.push(replaceObjectsByGUUID(theArguments[key]));
        }
    }
    var argStr = JSON.stringify(args);
    var cmd   = methodName+"(" + argStr.substr(1,argStr.length-2) +")";
    var uuid  = hash(cmd);

    return [uuid,cmd];
}



Cache.prototype.replaceObjectMethod = function (obj,_methodName,_originalMethod) {
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


Cache.prototype.installProxyMethods = function(obj) {
    'use strict';
    if (!obj) return;

    var functionToOverride= [ "translate", "mirror" , "rotate" , "clone" , "transformed" ,"getCommonEdges"];
    
    var that = this;

    functionToOverride.forEach(function(func_name) {

        if (obj[func_name]) {
            that.replaceObjectMethod(obj,func_name,obj[func_name]);
        }

    });

};

Cache.prototype.prepareObject = function(obj,cmd,uuid,index) {

    if (obj === undefined ) { return; }

    assert(!obj.hasOwnProperty("cmd"), " object should not have been inserted already ( old cmd : " + obj.cmd +")");
    assert(!obj.hasOwnProperty("uuid"), " object should not have been inserted already ( old uuid : " + obj.uuid +")");

    // store the resulting object in cache
    obj.cmd  = cmd
    obj.uuid = uuid;
    if (index) {
        obj.uuid = hash(uuid + "[" + index + "]" );
        obj.uuid_index = index;
    }
    console.log(" adding object to cache  ", uuid.green , "cmd =", cmd , " index = " , index);
    this.mapObject[obj.uuid] =obj;

    // prepare the resulting object
    this.installProxyMethods(obj);
    
};

Cache.prototype.__performedCachedOperation = function(originalObject,originalMethod,methodName, theArguments) {


    var h = calculateOperationHash(methodName,theArguments);

    var uuid  = h[0];
    
    var cmd = h[1];

    console.log(" executing method ", cmd, uuid);

    // check if object is already known
    this.mapQueryCount++;
    if (this.mapObject.hasOwnProperty(uuid)) {
        var obj = this.mapObject[uuid];
        assert(obj.uuid == uuid," checking object uuid consistency");
        console.log(" using object from cache ", obj.uuid.cyan, obj.cmd);
        this.mapHit++;
        return obj;
    }
    

    //  otherwise perform the 'costly' original operation
    try  {
        var result = originalMethod.apply(originalObject,theArguments);
    }
    catch (err) {
        console.log(err);
        console.log( " FAILING to call " + methodName + "(" , Object.keys(theArguments).map(function(e){ return JSON.stringify(theArguments[e]);}).join(" , ") + ")");

        throw err; // re-throw execption above
    }


    var _this = this;

    if (result instanceof Array) {
        
        console.log("result is an array with  ",result.length , " elements");
        result.forEach(function(el,index){ _this.prepareObject(el,cmd,uuid,index); });

        _this.prepareObject(result,cmd,uuid);

    } else {
        
        _this.prepareObject(result,cmd,uuid);
    }


    return result;
};


Cache.prototype.__proxy = function(originalObject,methodName,theArguments) {
    'use strict';
    return this.__performedCachedOperation(originalObject,originalObject[methodName],methodName,theArguments);
};






var cache = new Cache();

var proxyOCC = function() {

};

function replaceFunc(proxyObj,Obj,methodName) {       
    console.log(" installing proxy method  " + methodName);
    assert(typeof Obj[methodName] === "function", "methodName = " +  methodName + " " +  typeof Obj[methodName] ); 
    proxyObj.prototype[methodName] = function() {  'use strict';return cache.__proxy(Obj, methodName, arguments); };
}

["makeBox"   ,"makeCylinder"  ,"makeCone"      ,"makeSphere",
 "makeTorus","makeThickSolid", "makeDraftAngle",
 "makeFillet",
 "fuse"     ,"cut"           ,"common"        ,"compound"].forEach(function(funcName) {

    replaceFunc(proxyOCC,occ,funcName);
});

["makeBottle"].forEach(function(funcName) {
    replaceFunc(proxyOCC,shapeFactory,funcName);
});

exports.fastBuilder= cache;
exports.occ   = new proxyOCC();

exports.occ.buildSolidMesh = occ.buildSolidMesh;


exports.calculateOperationHash = calculateOperationHash;