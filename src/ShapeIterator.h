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
    static  Handle<v8::Value> reset(const v8::Arguments& args);

    Handle<Value> next();
    static  Handle<v8::Value> next(const v8::Arguments& args);

    // Methods exposed to JavaScripts
    static void Init(Handle<Object> target);
    static Handle<v8::Value> NewInstance(const v8::Arguments& args);
    static Handle<v8::Value> New(const v8::Arguments& args);

    static Persistent<FunctionTemplate> constructor;

};
