#pragma once

#include "Shape.h"
#include "Point3Wrap.h"

#include <limits>



class BooleanOperation : public Nan::ObjectWrap {
    BRepAlgoAPI_BooleanOperation* m_bop;
    BooleanOperation();
    ~BooleanOperation();
public:
    typedef class BooleanOperation _ThisType;

    static Nan::Persistent<v8::FunctionTemplate> _template;
    static v8::Handle<v8::Value> NewInstance(BOPAlgo_Operation op);

    static NAN_METHOD(New);
    static void Init(v8::Handle<v8::Object> target);
};
