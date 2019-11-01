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
    virtual v8::Local<v8::Object>  Clone() const ;
    virtual Base* Unwrap(v8::Local<v8::Object> obj) const {
        return Nan::ObjectWrap::Unwrap<Vertex>(obj);
    }


    virtual void InitNew(_NAN_METHOD_ARGS);


    static void Init(v8::Local<v8::Object> target);
    static NAN_METHOD(NewInstance);
    static NAN_METHOD(New);
    static Nan::Persistent<v8::FunctionTemplate> _template;
};
