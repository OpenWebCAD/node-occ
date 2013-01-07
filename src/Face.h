#pragma once
#include "OCC.h"
#include "NodeV8.h"

#include "Base.h"
#include "Point3Wrap.h"

#include <vector>
class Wire;
class Face:  public Base {

    TopoDS_Face m_face;
    Persistent<Object> m_cacheMesh;
    virtual ~Face() {
        m_cacheMesh.Dispose();
    };
public:
    int numWires();
    double area();
    bool fixShape();
    bool isPlanar();
	bool hasMesh();
    std::vector<double> inertia();

    const gp_XYZ centreOfMass() const;

    TEAROFF_POINT(Face,centreOfMass,Point3Wrap,gp_XYZ);

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


	static Handle<v8::Value> _mesh(Local<String> property,const AccessorInfo &info);
	Handle<Object> createMesh(double factor, double angle, bool qualityNormals);

    static void Init(Handle<Object> target);
    static Handle<Object> NewInstance(const TopoDS_Face& box);
    static Handle<Value> New(const Arguments& args);
    static Persistent<FunctionTemplate> constructor;
};