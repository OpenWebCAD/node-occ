'use strict';
const crypto = require('crypto');
const occ = require('./occ');
const shapeFactory = require('../lib/shapeFactory');
require('should');
require('colors');
const assert = require('assert');

const verboseOutput = false;

class Cache {

    constructor() {

        this.resetCache();

    }

    resetCache() {

        this.mapHit = 0;
        this.mapQueryCount = 0;
        this.mapObject = {};

    }

    replaceObjectMethod(obj, _methodName) {

        const _originalMethod = obj[_methodName];
        assert(isFunction(_originalMethod));
        // create closure variable
        const that = this;
        const methodName = _methodName;
        const originalMethod = _originalMethod;

        function performOperation() {

            return that.__performedCachedOperation(this, originalMethod, `${methodName}#${this.uuid}`, arguments);

        }

        Object.defineProperty(obj, methodName, { value: performOperation, enumerable: false });

        // xx obj[methodName] = performOperation;=

    }

    replaceObjectProperty(obj, methodName) {
        // xx obj.__defineGetter__(_methodName, function () {})
    }

    installProxyMethods(obj) {

        // xx console.log("xxx in installProxyMethods on",obj.constructor.name);
        assert(!!obj);

        const prototype = obj.constructor.prototype;

        const functionToOverride = Object.keys(prototype).filter(n => typeof prototype[n] === 'function');
        const propertyToOverwrite = Object.keys(prototype).filter(n => typeof prototype[n] !== 'function');

        // xx console.log(obj.constructor.name, propertyToOverwrite.join(" "), "-", functionToOverride.join(" "));

        // ---------------- old
        const that = this;
        functionToOverride.forEach((funcName) => {

            if (funcName === 'toString') return;
            // xx console.log("xxx in installProxyMethods replaceObjectMethod",func_name);
            that.replaceObjectMethod(obj, funcName);

        });
        propertyToOverwrite.forEach((propName) => {

            that.replaceObjectProperty(obj, propName);

        });

    }

    prepareObject(obj, cmd, uuid, index) {

        if (obj === undefined) {

            return;

        }
        if (typeof obj === 'string') {

            return;

        }

        assert(!obj.hasOwnProperty('cmd'), ` object should not have been inserted already ( old cmd : ${obj.cmd})`);
        assert(!obj.hasOwnProperty('uuid'), ` object should not have been inserted already ( old uuid : ${obj.uuid})`);

        // store the resulting object in cache
        obj.cmd = cmd;
        obj.uuid = uuid;
        if (index) {

            obj.uuid = hash(`${uuid}[${index}]`);
            obj.uuid_index = index;

        }
        // /xx console.log("xxx cmd = ",obj.constructor.name,cmd," => ",obj.uuid);
        if (verboseOutput) {

            console.log(' adding object to cache  ', uuid.green, 'cmd =', cmd, ' index = ', index);

        }
        this.mapObject[obj.uuid] = obj;

        // prepare the resulting object
        this.installProxyMethods(obj);

    }

    __performedCachedOperation(originalObject,
                               originalMethod,
                               methodName,
                               theArguments,
                               callAsConstructor) {

        assert(theArguments.hasOwnProperty('length'));

        const h = calculateOperationHash(methodName, theArguments, callAsConstructor);
        const uuid = h[0];
        const cmd = h[1];

        if (verboseOutput) {

            console.log(' executing method ', cmd, uuid);

        }
        // xx console.log("xxx executing method ", cmd, uuid);

        // check if object is already known
        this.mapQueryCount++;
        if (this.mapObject.hasOwnProperty(uuid)) {

            const obj = this.mapObject[uuid];
            assert(obj.uuid === uuid, ' checking object uuid consistency');
            if (verboseOutput) {

                console.log(' using object from cache ', obj.uuid.cyan, obj.cmd);

            }
            this.mapHit++;
            return obj;

        }

        //  otherwise perform the 'costly' original operation
        let result;
        try {

            if (callAsConstructor) {

                // New object must be unique....
                assert(false);
                const args = [null].concat(Object.keys(theArguments).map(f => theArguments[f]));
                const FactoryFunction = originalMethod.bind(args);
                result = new FactoryFunction();

            } else {

                const args = Object.keys(theArguments).map(f => theArguments[f]);
                result = originalMethod.apply(originalObject, args);

            }

        } catch (err) {

            if (verboseOutput) {

                console.log(err);
                console.log(` FAILING to call ${methodName}(`, `${Object.keys(theArguments).map(e => JSON.stringify(theArguments[e])).join(' , ')})`);

            }

            throw err; // re-throw execption above

        }

        const _this = this;

        if (result instanceof Array) {

            if (verboseOutput) {

                console.log('result is an array with  ', result.length, ' elements');

            }
            result.forEach((el, index) => {

                _this.prepareObject(el, cmd, uuid, index);

            });

            _this.prepareObject(result, cmd, uuid);

        } else {

            // xx console.log("xxx resylt ) ",methodName,result.constructor.name,result);
            _this.prepareObject(result, cmd, uuid);

        }

        return result;

    }

    __proxy(originalObject, methodName, theArguments, callAsConstructor) {

        return this.__performedCachedOperation(originalObject, originalObject[methodName], methodName, theArguments, callAsConstructor);

    }
}

function hash(args) {

    const shasum = crypto.createHash('sha1');
    shasum.update(args);
    return shasum.digest('hex');

}

function calculateOperationHash(methodName, theArguments, callAsConstructor) {

    const toUUID = el => el.uuid;
    /**
     *
     */
    const replaceObjectsByGUUID = (arg) => {

        if (!arg) {

            return null;

        }
        if (arg.hasOwnProperty('uuid')) {

            return toUUID(arg);

        } else if (arg.length > 0 && typeof arg[0] === 'number') {

            return arg;

        } else if (arg.length > 0 && (arg[0] instanceof Object) && arg[0].hasOwnProperty('uuid')) {

            if (verboseOutput) {

                console.log(' Found an array of object with guiid', (`arg ${arg.map(el => el.uuid)}`).yellow);

            }
            return arg.map(replaceObjectsByGUUID);

        } else if (typeof arg === 'number') {

            return arg;

        } else {

            if (arg.hashCode) {

                return hash(`---${arg.hashCode}`);

            }
            // we can't use cache yet
            const tmp = arg;
            // .map(toUUID);
            if (verboseOutput) {

                console.log(' ERROR : Cannot use cache with object '.red, typeof arg, JSON.stringify(tmp));

            }
            return tmp;

        }

    };

    // build the signature of the function + arguments

    let key;
    const args = [];
    for (key in theArguments) {

        if (theArguments.hasOwnProperty(key)) {

            args.push(replaceObjectsByGUUID(theArguments[key]));

        }

    }
    const argStr = JSON.stringify(args);
    const cmd = `${methodName}(${argStr.substr(1, argStr.length - 2)})`;
    const uuid = hash(cmd);

    return [uuid, cmd];

}

function isFunction(a) {

    return typeof a === 'function';

}

const cache = new Cache();

function replaceFunc(proxyObj, Obj, methodName) {

    assert(proxyObj);
    if (verboseOutput) {

        console.log(` installing proxy method  ${methodName}`);

    }

    if (isFunction(Obj[methodName])) {

        assert(isFunction(Obj[methodName]), `methodName = ${methodName} ${typeof Obj[methodName]}`);
        const fn = function() {

            // http://stackoverflow.com/questions/367768/how-to-detect-if-a-function-is-called-as-constructor
            // Xx console.log(this.constructor.name == methodName, (this.constructor === fn), this.constructor.name);
            return cache.__proxy(Obj, methodName, arguments, (this.constructor === fn));

        };
        proxyObj[methodName] = fn;
        // http://stackoverflow.com/questions/5871040/how-to-dynamically-set-a-function-object-name-in-javascript-as-it-is-displayed-i
        Object.defineProperty(fn, 'name', { value: methodName });

    }

}

occ.makeBottle = shapeFactory.makeBottle;

function createProxyObject(originalObject) {

    const proxyObject = new Object();
    const properties = Object.keys(originalObject);
    properties.forEach((funcName) => {

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
