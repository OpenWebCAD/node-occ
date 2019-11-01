#pragma once
#include "NodeV8.h"
#include "OCC.h"

#include "Base.h"


class ShapeIterator : public Nan::ObjectWrap {
public:
    TopExp_Explorer ex;
    TopAbs_ShapeEnum m_toFind;

    ShapeIterator(Base *arg,TopAbs_ShapeEnum type) {
        m_toFind = type;
        ex.Init(arg->shape(), m_toFind);
    }

    bool more();

    void reset();
    static  NAN_METHOD(reset);

    v8::Local<v8::Value> next();
    static NAN_METHOD(next);

    // Methods exposed to JavaScripts
    static void Init(v8::Local<v8::Object> target);

    static NAN_METHOD(NewInstance);
    static NAN_METHOD(New);

    static Nan::Persistent<v8::FunctionTemplate> _template;

};
