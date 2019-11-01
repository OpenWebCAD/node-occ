#pragma once
#include "Shape.h"

#include "Vertex.h"

class Edge : public Base {

public:
   TopoDS_Edge m_edge;
   std::vector<float> m_positions;

    Edge() {
        ;
    }

    bool isSeam(Base *face);

    bool isDegenerated();
    bool isClosed();
    int numVertices();
    double length();

    v8::Local<v8::Object> polygonize(double factor);

    int createLine(Vertex *start, Vertex *end);
    int createArc(Vertex *start, Vertex *end, const gp_Pnt& center);
    int createArc3P(Vertex *start, Vertex *end, const gp_Pnt& middlePoint);

    int createCircle(const gp_Pnt& center , const gp_Dir& normal, double radius);

    //int createEllipse(OCCStruct3d pnt, OCCStruct3d nor, double rMajor, double rMinor);
    //int createHelix(double pitch, double height, double radius, double angle, bool leftHanded);
    //int createBezier(OCCVertex *start, OCCVertex *end, std::vector<OCCStruct3d> points);
    //int createSpline(OCCVertex *start, OCCVertex *end, std::vector<OCCStruct3d> points,double tolerance);
    //int createNURBS(OCCVertex *start, OCCVertex *end, std::vector<OCCStruct3d> points,  DVec knots, DVec weights, IVec mult);

    bool canSetShape(const TopoDS_Shape& shape) const {
        return shape.ShapeType() == TopAbs_EDGE;
    }
    virtual const TopoDS_Shape& shape() const {
        return m_edge;
    }
    virtual const TopoDS_Edge&  edge() const {
        return m_edge;
    }
    virtual void setShape(const TopoDS_Shape& shape) {
        m_edge = TopoDS::Edge(shape);
    }
    virtual v8::Local<v8::Object>  Clone() const ;
    virtual Base* Unwrap(v8::Local<v8::Object> obj) const  {
        return Nan::ObjectWrap::Unwrap<Edge>(obj);
    }

    static NAN_PROPERTY_GETTER(getter_firstVertex);
    static NAN_PROPERTY_GETTER(getter_lastVertex);

    // Static Methods
    static NAN_METHOD(static_createLine);
    static NAN_METHOD(static_createCircle);
    static NAN_METHOD(static_createArc3P);


    static NAN_METHOD(polygonize);
    static NAN_METHOD(getVertices);

    static NAN_METHOD(New);
    static NAN_METHOD(startVertex);
    static NAN_METHOD(endVertex);

    static void Init(v8::Local<v8::Object> target);
    static Nan::Persistent<v8::FunctionTemplate> _template;
};