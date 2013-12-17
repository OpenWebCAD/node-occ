#pragma once

#include "Shape.h"
#include "Point3Wrap.h"

#include <limits>



class BooleanOperation : public node::ObjectWrap {
    BRepAlgoAPI_BooleanOperation* m_bop;
    BooleanOperation();
    ~BooleanOperation();
public:
    typedef class BooleanOperation _ThisType;

    static Persistent<FunctionTemplate> constructor;
    static Handle<Object> NewInstance(BOPAlgo_Operation op);

    static Handle<Value> New(const v8::Arguments& args);
    static void Init(Handle<Object> target);
};
