#pragma once
#include "OCC.h"
#include "NodeV8.h"

#include "Base.h"

#include <vector>
class Wire;
class Face:  public Base {

    TopoDS_Face m_face;
public:
    int numWires();
    double area();
    bool fixShape();

    virtual const TopoDS_Shape& shape() const;
    const  TopoDS_Face& face() const {
        return m_face;
    }
    virtual void setShape(const TopoDS_Shape&);

    bool buildFace(std::vector<Wire*>& wires);

    static Handle<Value> extrude(const Arguments& arg);

    virtual Local<Object> Clone() ;

    virtual Base* Unwrap(v8::Local<v8::Object> obj) {
        return node::ObjectWrap::Unwrap<Face>(obj);
    }
    static void Init(Handle<Object> target);
    static Handle<Value> New(const Arguments& args);
    static Persistent<FunctionTemplate> constructor;
};