#pragma once
#include "Shape.h"
#include "Mesh.h"

// a multi body shape
class Solid : public Shape 
{

protected:
    Solid() {};
    virtual ~Solid() {};

 public:  
	Persistent<Object> m_cacheMesh;

	int numSolids();
	int numFaces();

	double volume();
	double area();
   
	Handle<Object>  mesh();

	Handle<Object> createMesh(double factor, double angle, bool qualityNormals = true);

	typedef enum BoolOpType {
		BOOL_FUSE,
		BOOL_CUT,
		BOOL_COMMON,
	} BoolOpType;

	int boolean(Solid *tool, BoolOpType op);

	// boolean operation
	static Handle<v8::Value> _boolean(const v8::Arguments& args,BoolOpType op);
	static Handle<v8::Value> fuse(const v8::Arguments& args);
	static Handle<v8::Value> cut(const v8::Arguments& args);
	static Handle<v8::Value> common(const v8::Arguments& args);

	// default mesh
	static Handle<v8::Value> _mesh(Local<String> property,const AccessorInfo &info);
	static Handle<v8::Value> createMesh(const v8::Arguments& args); // custom mesh
   
	// primitive constructions
	static Handle<v8::Value> makeBox(const v8::Arguments& args) ;

	// Methods exposed to JavaScripts
	static void Init(Handle<Object> target);
	static Handle<v8::Value> NewInstance(const v8::Arguments& args);
	static Handle<v8::Value> New(const v8::Arguments& args);     

	static Persistent<FunctionTemplate> constructor;
};

