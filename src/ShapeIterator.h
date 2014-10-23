#pragma once
#include "NodeV8.h"
#include "OCC.h"

#include "Base.h"


class ShapeIterator : public node::ObjectWrap {
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

    Handle<Value> next();
    static NAN_METHOD(next);

    // Methods exposed to JavaScripts
    static void Init(Handle<Object> target);

    static NAN_METHOD(NewInstance);
    static NAN_METHOD(New);

    static v8::Persistent<v8::FunctionTemplate> _template;

};
