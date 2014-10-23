#pragma once
#include "Shape.h"
#include "Mesh.h"

class Edge;
// a multi body shape
class Solid : public Shape {

protected:
    Solid() {};
    virtual ~Solid() {
        NanDisposePersistent(m_cacheMesh);
    };

public:
    virtual Local<Object>  Clone() const;
    virtual Base* Unwrap(v8::Local<v8::Object> obj) const { return node::ObjectWrap::Unwrap<Solid>(obj); }

    Persistent<Object> m_cacheMesh;

    const  TopoDS_Solid& solid() const {
        return TopoDS::Solid(shape());
    }

    int numSolids();
    int numFaces();
    int numShells();

    double volume();
    double area();

	virtual void InitNew(_NAN_METHOD_ARGS);

    Handle<Object> createMesh(double factor, double angle, bool qualityNormals = true);

    typedef enum BoolOpType {
        BOOL_FUSE,
        BOOL_CUT,
        BOOL_COMMON,
    } BoolOpType;

    int boolean(Solid *tool, BoolOpType op);
    //xx int chamfer(const std::vector<Edge*>& edges, const std::vector<double>& distances);
    //xx int fillet(const std::vector<Edge*>& edges, const std::vector<double>& distances);
   
	// static Handle<v8::Value> fillet(const v8::Arguments& args);
    // static Handle<v8::Value> chamfer(const v8::Arguments& args);

    // default mesh
    static NAN_PROPERTY_GETTER(_mesh);
    
    static NAN_METHOD(createMesh); // custom mesh

    static NAN_METHOD(getEdges);
    static NAN_METHOD(getVertices);
    static NAN_METHOD(getFaces);
    static NAN_METHOD(getShells);
    static NAN_METHOD(getSolids);
    static NAN_METHOD(getOuterShell);
    static NAN_METHOD(getShapeName);
	  static NAN_METHOD(getAdjacentFaces);
	  static NAN_METHOD(getCommonEdges);
	  static NAN_METHOD(getCommonVertices);

    // Methods exposed to JavaScripts
    static void Init(Handle<Object> target);

    static NAN_METHOD(New);
    static NAN_METHOD(NewInstance);

    static Handle<Value>     NewInstance(TopoDS_Shape shape);
    static Handle<Value>     NewInstance();

    static v8::Persistent<v8::FunctionTemplate> _template;


    void _registerNamedShape(const char* name,const TopoDS_Shape& shape);
    std::string _getShapeName(const TopoDS_Shape& shape);


};

