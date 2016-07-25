'use strict';
var crypto = require('crypto');
var fs = require('fs');
var occ = require('./occ');
var shapeFactory = require('../lib/shapeFactory');
var should = require('should');
var color = require('colors');
var assert = require('assert');

var verboseOutput = false;

var Cache = function () {
    this.resetCache();
};

Cache.prototype.resetCache = function () {
    this.mapHit = 0;
    this.mapQueryCount = 0;
    this.mapObject = {};
};

function hash(args) {
    var shasum = crypto.createHash('sha1');
    shasum.update(args);
    return shasum.digest("hex");
}

function calculateOperationHash(methodName, theArguments, callAsConstructor) {

    var toUUID = function (el) {
        return el.uuid;
    };
    /**
     *
     */
    var replaceObjectsByGUUID = function (arg) {
        if (!arg) {
            return null;
        }
        if (arg.hasOwnProperty("uuid")) {

            return toUUID(arg);

        } else if (arg.length > 0 && typeof arg[0] == "number") {

            return arg;

        } else if (arg.length > 0 && ( arg[0] instanceof Object ) && arg[0].hasOwnProperty("uuid")) {

            if (verboseOutput) {
                console.log(" Found an array of object with guiid", ("arg " + arg.map(function (el) {
                    return el.uuid;
                })).yellow);
            }
            return arg.map(replaceObjectsByGUUID);

        } else if (typeof arg == "number") {

            return arg;

        } else {

            if (arg.hashCode) {
                return hash("---" + arg.hashCode);
            }
            // we can't use cache yet
            var tmp = arg;
            //.map(toUUID);
            if (verboseOutput) {
                console.log(" ERROR : Cannot use cache with object ".red, typeof arg, JSON.stringify(tmp));
            }
            return tmp;
        }
    };

    // build the signature of the function + arguments

    var key;
    var args = [];
    for (key in theArguments) {
        if (theArguments.hasOwnProperty(key)) {
            args.push(replaceObjectsByGUUID(theArguments[key]));
        }
    }
    var argStr = JSON.stringify(args);
    var cmd = methodName + "(" + argStr.substr(1, argStr.length - 2) + ")";
    var uuid = hash(cmd);

    return [uuid, cmd];
}

function isFunction(a) {
    return typeof a === "function";
}

Cache.prototype.replaceObjectMethod = function (obj, _methodName) {

    var _originalMethod = obj[_methodName];
    assert(isFunction(_originalMethod));
    // create closure variable
    var that = this;
    var methodName = _methodName;
    var originalMethod = _originalMethod;

    var performOperation = function () {
        return that.__performedCachedOperation(this, originalMethod, methodName + "#" + this.uuid, arguments);
    };

    Object.defineProperty(obj, methodName, {value: performOperation, enumerable: false});

    //xx obj[methodName] = performOperation;=
};

Cache.prototype.replaceObjectProperty = function (obj, _methodName) {
    //xx obj.__defineGetter__(_methodName, function () {})
};

Cache.prototype.installProxyMethods = function (obj) {

    //xx console.log("xxx in installProxyMethods on",obj.constructor.name);
    assert(!!obj);

    var prototype = obj.constructor.prototype;

    var functionToOverride = Object.keys(prototype).filter(function (n) {
        return typeof prototype[n] == "function"
    });
    var propertyToOverwrite = Object.keys(prototype).filter(function (n) {
        return typeof prototype[n] != "function"
    });

    //xx console.log(obj.constructor.name, propertyToOverwrite.join(" "), "-", functionToOverride.join(" "));

// ---------------- old
    var that = this;
    functionToOverride.forEach(function (func_name) {
        if (func_name === "toString") return;
        //xx console.log("xxx in installProxyMethods replaceObjectMethod",func_name);
        that.replaceObjectMethod(obj, func_name);
    });
    propertyToOverwrite.forEach(function (prop_name) {
        that.replaceObjectProperty(obj, prop_name);
    });

};

Cache.prototype.prepareObject = function (obj, cmd, uuid, index) {

    if (obj === undefined) {
        return;
    }
    if (typeof obj === "string") {
        return;
    }


    assert(!obj.hasOwnProperty("cmd"), " object should not have been inserted already ( old cmd : " + obj.cmd + ")");
    assert(!obj.hasOwnProperty("uuid"), " object should not have been inserted already ( old uuid : " + obj.uuid + ")");

    // store the resulting object in cache
    obj.cmd = cmd;
    obj.uuid = uuid;
    if (index) {
        obj.uuid = hash(uuid + "[" + index + "]");
        obj.uuid_index = index;
    }
    ///xx console.log("xxx cmd = ",obj.constructor.name,cmd," => ",obj.uuid);
    if (verboseOutput) {
        console.log(" adding object to cache  ", uuid.green, "cmd =", cmd, " index = ", index);
    }
    this.mapObject[obj.uuid] = obj;

    // prepare the resulting object
    this.installProxyMethods(obj);

};


Cache.prototype.__performedCachedOperation = function (originalObject, originalMethod, methodName, theArguments, callAsConstructor) {

    assert(theArguments.hasOwnProperty("length"));

    var h = calculateOperationHash(methodName, theArguments, callAsConstructor);
    var uuid = h[0];
    var cmd = h[1];

    if (verboseOutput) {
        console.log(" executing method ", cmd, uuid);
    }
    //xx console.log("xxx executing method ", cmd, uuid);

    // check if object is already known
    this.mapQueryCount++;
    if (this.mapObject.hasOwnProperty(uuid)) {
        var obj = this.mapObject[uuid];
        assert(obj.uuid == uuid, " checking object uuid consistency");
        if (verboseOutput) {
            console.log(" using object from cache ", obj.uuid.cyan, obj.cmd);
        }
        this.mapHit++;
        return obj;
    }


    //  otherwise perform the 'costly' original operation
    var result;
    try {
        if (callAsConstructor) {
            // New object must be unique....
            assert(false);
            var args = [null].concat(Object.keys(theArguments).map(function (f) {
                return theArguments[f]
            }));
            var factoryFunction = originalMethod.bind.apply(originalMethod, args);
            result = new factoryFunction();
        } else {

            var args = Object.keys(theArguments).map(function (f) {
                return theArguments[f]
            });
            result = originalMethod.apply(originalObject, args);
        }
    }

    catch (err) {

        if (verboseOutput) {
            console.log(err);
            console.log(" FAILING to call " + methodName + "(", Object.keys(theArguments).map(function (e) {
                    return JSON.stringify(theArguments[e]);
                }).join(" , ") + ")");
        }

        throw err; // re-throw execption above
    }


    var _this = this;

    if (result instanceof Array) {

        if (verboseOutput) {
            console.log("result is an array with  ", result.length, " elements");
        }
        result.forEach(function (el, index) {
            _this.prepareObject(el, cmd, uuid, index);
        });

        _this.prepareObject(result, cmd, uuid);

    } else {
        //xx console.log("xxx resylt ) ",methodName,result.constructor.name,result);
        _this.prepareObject(result, cmd, uuid);
    }

    return result;
};


Cache.prototype.__proxy = function (originalObject, methodName, theArguments, callAsConstructor) {
    return this.__performedCachedOperation(originalObject, originalObject[methodName], methodName, theArguments, callAsConstructor);
};


var cache = new Cache();

function replaceFunc(proxyObj, Obj, methodName) {

    assert(proxyObj);
    if (verboseOutput) {
        console.log(" installing proxy method  " + methodName);
    }

    if (isFunction(Obj[methodName])) {
        assert(isFunction(Obj[methodName]), "methodName = " + methodName + " " + typeof Obj[methodName]);
        var fn = function () {
            // http://stackoverflow.com/questions/367768/how-to-detect-if-a-function-is-called-as-constructor
            //Xx console.log(this.constructor.name == methodName, (this.constructor === fn), this.constructor.name);
            return cache.__proxy(Obj, methodName, arguments, (this.constructor === fn));
        };
        proxyObj[methodName] = fn;
        // http://stackoverflow.com/questions/5871040/how-to-dynamically-set-a-function-object-name-in-javascript-as-it-is-displayed-i
        Object.defineProperty(fn, "name", {value: methodName});
    }
}


occ.makeBottle = shapeFactory.makeBottle;

function createProxyObject(originalObject) {
    var proxyObject = new Object();
    var properties = Object.keys(originalObject);
    properties.forEach(function (funcName) {
        replaceFunc(proxyObject, originalObject, funcName);
    });
    return proxyObject;
}


/*
 [
 "makeBox", "makeCylinder", "makeCone", "makeSphere",
 "makeTorus", "makeThickSolid", "makeDraftAngle",
 "makeFillet",
 "fuse", "cut", "common", "compound",
 "makeVertex", "makeEdge", "makeWire",
 // constructors
 "Vertex", "Edge", "Wire", "Transformation","BoundingBox"

 ].forEach(function (funcName) {
 replaceFunc(ProxyOCC, occ, funcName);
 });

 ["makeBottle"].forEach(function (funcName) {
 replaceFunc(ProxyOCC, shapeFactory, funcName);
 });
 */

exports.fastBuilder = cache;


exports.occ = createProxyObject(occ);


exports.occ.buildSolidMesh = occ.buildSolidMesh;


exports.calculateOperationHash = calculateOperationHash;
