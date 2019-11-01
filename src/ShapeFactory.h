#pragma once
#include "NodeV8.h"
#include "OCC.h"
#include <vector>



class Solid;
class Base;
class ShapeFactory {
public:
    // vertex
    static NAN_METHOD(makeVertex);
    //xx // edges
    //xx static NAN_METHOD(makeLine);
    //xx static NAN_METHOD(makeCircle);
    //xx static NAN_METHOD(makeArc3P);

    // wires
    static NAN_METHOD(makeWire);

    // faces
    static NAN_METHOD(makeFace);

    // boolean operation
    static NAN_METHOD(fuse);
    static NAN_METHOD(cut);
    static NAN_METHOD(common);
    static NAN_METHOD(compound);
    // primitive constructions
    static NAN_METHOD(makeBox);
    static NAN_METHOD(makePrism);
    static NAN_METHOD(makeCylinder);
    static NAN_METHOD(makeCone);
    static NAN_METHOD(makeSphere);
    static NAN_METHOD(makeTorus);
    static NAN_METHOD(makePipe);
    //
    static NAN_METHOD(makeThickSolid);
    static NAN_METHOD(makeDraftAngle);
    static NAN_METHOD(makeFillet);
private:
    static void _boolean(_NAN_METHOD_ARGS,BOPAlgo_Operation op);
    static v8::Local<v8::Value> add(const std::vector<Base*>& shapes);

};
