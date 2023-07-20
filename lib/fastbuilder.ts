"use strict";
import crypto from "crypto";
import assert from "assert";

import { OCC } from "./interfaces";

function debugLog(this: any, ...args: any[]) {
  /* implement me*/
  // console.log.apply(this, args);
}

type IObject = any;

type ObjectMethod = (this: any, ...args: any[]) => any;

class Cache {
  public mapObject: Record<string, any> = {};
  public mapQueryCount = 0;
  public mapHit = 0;
  constructor() {
    this.resetCache();
  }

  resetCache() {
    this.mapObject = {};
    this.mapQueryCount = 0;
    this.mapHit = 0;
  }

  replaceObjectMethod(obj: IObject, _methodName: string) {
    const _originalMethod = obj[_methodName];
    assert(isFunction(_originalMethod));
    // create closure variable
    const that = this;
    const methodName = _methodName;
    const originalMethod = _originalMethod;
    function performOperation(this: IObject, ...args: any[]) {
      return that.__performedCachedOperation(
        this,
        originalMethod,
        `${methodName}#${this.uuid}`,
        args,
        false
      );
    }
    Object.defineProperty(obj, methodName, {
      value: performOperation,
      enumerable: false
    });
  }

  installProxyMethods(obj: IObject) {
    // xx console.log("xxx in installProxyMethods on",obj.constructor.name);
    assert(!!obj);

    const prototype = obj.constructor.prototype;

    const functionToOverride = Object.keys(prototype).filter(
      (n) => typeof prototype[n] === "function"
    );

    // ---------------- old
    const that = this;
    functionToOverride.forEach((funcName) => {
      if (funcName === "toString") {
        return;
      }
      // xx console.log("xxx in installProxyMethods replaceObjectMethod",func_name);
      that.replaceObjectMethod(obj, funcName);
    });
  }

  prepareObject(
    obj: IObject,
    cmd: string,
    uuid: string,
    index?: number | string
  ) {
    if (obj === undefined) {
      return;
    }
    if (typeof obj === "string") {
      return;
    }
    if (typeof obj === "number") {
      return;
    }

    assert(
      !obj.hasOwnProperty("cmd"),
      ` object should not have been inserted already ( old cmd : ${obj.cmd})`
    );
    assert(
      !obj.hasOwnProperty("uuid"),
      ` object should not have been inserted already ( old uuid : ${obj.uuid})`
    );

    // store the resulting object in cache
    obj.cmd = cmd;
    obj.uuid = uuid;
    if (index) {
      obj.uuid = hash(`${uuid}[${index}]`);
      obj.uuid_index = index;
    }
    debugLog(
      " adding object to cache  ",
      uuid,
      "cmd =",
      cmd,
      " index = ",
      index
    );

    this.mapObject[obj.uuid] = obj;

    // prepare the resulting object
    this.installProxyMethods(obj);
  }

  __performedCachedOperation(
    originalObject: IObject,
    originalMethod: ObjectMethod,
    methodName: string,
    theArguments: any[],
    callAsConstructor: boolean
  ) {
    assert(theArguments.hasOwnProperty("length"));

    const h = calculateOperationHash(
      methodName,
      theArguments,
      callAsConstructor
    );
    const uuid = h[0];
    const cmd = h[1];

    debugLog(" executing method ", cmd, uuid);

    // check if object is already known
    this.mapQueryCount++;
    if (this.mapObject.hasOwnProperty(uuid)) {
      const obj = this.mapObject[uuid];
      assert(obj.uuid === uuid, " checking object uuid consistency");
      debugLog(" using object from cache ", obj.uuid, obj.cmd);

      this.mapHit++;
      return obj;
    }

    //  otherwise perform the "costly" original operation
    let result;
    try {
      if (callAsConstructor) {
        // New object must be unique....
        assert(false);
        // const args = [null].concat(Object.keys(theArguments).map(f => theArguments[f]));
        // const FactoryFunction = originalMethod.bind(...args);
        // result = new FactoryFunction();
      } else {
        // const args = Object.keys(theArguments).map((f) => theArguments[f]);
        result = originalMethod.apply(originalObject, theArguments);
      }
    } catch (err) {
      debugLog(
        ` FAILING to call ${methodName}(`,
        `${theArguments
          .map((e) => JSON.stringify(theArguments[e]))
          .join(" , ")})`
      );
      throw err; // re-throw execption above
    }

    const _this = this;

    if (result instanceof Array) {
      debugLog("result is an array with  ", result.length, " elements");

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

  __proxy(
    originalObject: IObject,
    methodName: string,
    theArguments: any[],
    callAsConstructor: boolean
  ) {
    return this.__performedCachedOperation(
      originalObject,
      originalObject[methodName],
      methodName,
      theArguments,
      callAsConstructor
    );
  }
}

function hash(...args: any[]) {
  const shasum = crypto.createHash("sha1");
  shasum.update(args.map((a) => a.toString()).join(","));
  return shasum.digest("hex");
}

/**
 *
 */
function replaceObjectsByGUUID(arg: any) {
  const toUUID = (el: { uuid: string }) => el.uuid;

  if (!arg) {
    return null;
  }
  if (arg.hasOwnProperty("uuid")) {
    return toUUID(arg);
  } else if (
    (arg.length > 0 && typeof arg[0] === "number") ||
    typeof arg === "number"
  ) {
    return arg;
  } else if (
    (arg.length > 0 && typeof arg[0] === "string") ||
    typeof arg === "string"
  ) {
    return arg;
  } else if (
    arg.length > 0 &&
    arg[0] instanceof Object &&
    arg[0].hasOwnProperty("uuid")
  ) {
    debugLog(" Found an array of object with guiid", `arg ${arg.map(toUUID)}`);
    return arg.map(replaceObjectsByGUUID);
  } else {
    if (arg.hashCode) {
      return hash(`---${arg.hashCode}`);
    }
    // we can"t use cache yet
    const tmp = arg;
    // .map(toUUID);
    debugLog(
      " ERROR : Cannot use cache with object ",
      typeof arg,
      JSON.stringify(tmp)
    );

    return tmp;
  }
}
/**
 *
 * @private
 */
export function calculateOperationHash(
  methodName: string,
  theArguments: any[],
  _callAsConstructor: boolean
) {
  // build the signature of the function + arguments

  let key;
  const args = [];
  for (key in theArguments) {
    if (theArguments.hasOwnProperty(key)) {
      args.push(replaceObjectsByGUUID(theArguments[key]));
    }
  }
  const argStr = JSON.stringify(args);
  const cmd = `${methodName}(${argStr.substring(1, argStr.length - 1)})`;
  const uuid = hash(cmd);

  return [uuid, cmd];
}

function isFunction(a: any) {
  return typeof a === "function";
}

function replaceFunc(
  cache: Cache,
  proxyObj: IObject,
  Obj: IObject,
  methodName: string
) {
  assert(proxyObj);
  debugLog(` installing proxy method  ${methodName}`);

  const method = (Obj as any)[methodName];
  if (isFunction(method)) {
    assert(isFunction(method), `methodName = ${methodName} ${typeof method}`);
    const fn = function (this: any, ...args: any[]) {
      // http://stackoverflow.com/questions/367768/how-to-detect-if-a-function-is-called-as-constructor
      // Xx console.log(this.constructor.name == methodName, (this.constructor === fn), this.constructor.name);
      return cache.__proxy(Obj, methodName, args, this.constructor === fn);
    };
    (proxyObj as any)[methodName] = fn;
    // http://stackoverflow.com/questions/5871040/how-to-dynamically-set-a-function-object-name-in-javascript-as-it-is-displayed-i
    Object.defineProperty(fn, "name", { value: methodName });
  }
}

export function createProxyObject<IObject>(originalObject: IObject): IObject {
  const proxyObject: Record<string, any> = {};

  const cache = new Cache();
  proxyObject.$ = cache;
  const properties = Object.keys(originalObject as Record<string, any>);
  properties.forEach((funcName) =>
    replaceFunc(cache, proxyObject, originalObject, funcName)
  );
  return proxyObject as IObject;
}

/*
 * occ.makeBottle = shapeFactory.makeBottle;
 * [
 * "makeBox", "makeCylinder", "makeCone", "makeSphere",
 * "makeTorus", "makeThickSolid", "makeDraftAngle",
 * "makeFillet",
 * "fuse", "cut", "common", "compound",
 * "makeVertex", "makeEdge", "makeWire",
 * // constructors
 * "Vertex", "Edge", "Wire", "Transformation","BoundingBox"
 *
 * ].forEach(function (funcName) {
 * replaceFunc(ProxyOCC, occ, funcName);
 * });
 *
 * ["makeBottle"].forEach(function (funcName) {
 *    replaceFunc(ProxyOCC, shapeFactory, funcName);
 *  });
 */

import { occ as occOriginal } from "./occ";

export interface OCC2 extends OCC {
  $: {
    resetCache(): void;
    mapQueryCount: number;
    mapHit: number;
  };
}
//xx export const _cache: OCC2 = cache as any as OCC2;

export const fastBuilder = createProxyObject(occOriginal) as OCC2;
(fastBuilder as any).buildSolidMesh = occOriginal.buildSolidMesh;
