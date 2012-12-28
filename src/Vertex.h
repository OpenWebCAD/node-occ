#pragma once
#include "Shape.h"

class Vertex : public Shape {
private:
    TopoDS_Vertex m_vertex;
public:

    double x() ;
    double y() ;
    double z() ;

    gp_Pnt point() const;

    virtual const TopoDS_Shape& shape() const;
    const  TopoDS_Vertex& vertex() const {
        return m_vertex;
    }
    virtual void setShape( const TopoDS_Shape&);
    virtual Local<Object>  Clone() ;
    virtual Base* Unwrap(v8::Local<v8::Object> obj) {
        return node::ObjectWrap::Unwrap<Vertex>(obj);
    }

    static void Init(Handle<Object> target);
    static Handle<Value> New(const Arguments& args);
    static Persistent<FunctionTemplate> constructor;
};
