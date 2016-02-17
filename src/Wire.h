#pragma once
#include "Shape.h"

#include "Edge.h"

class Wire : public Base {

    TopoDS_Wire m_wire;
public:
    int numVertices();
    int numEdges();
    bool isClosed();


    virtual const TopoDS_Shape& shape() const;
    const  TopoDS_Wire& wire() const {
        return m_wire;
    }
    virtual void setShape(const TopoDS_Shape&);
    virtual v8::Local<v8::Object> Clone() const ;
    virtual Base* Unwrap(v8::Local<v8::Object> obj) const {
        return node::ObjectWrap::Unwrap<Wire>(obj);
    }

    static void Init(v8::Handle<v8::Object> target);
    static NAN_METHOD(New);
    static Nan::Persistent<v8::FunctionTemplate> _template;
};