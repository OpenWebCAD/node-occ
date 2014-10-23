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

    static v8::Persistent<v8::FunctionTemplate> _template;
    static v8::Handle<v8::Value> NewInstance(BOPAlgo_Operation op);

    static NAN_METHOD(New);
    static void Init(Handle<Object> target);
};
