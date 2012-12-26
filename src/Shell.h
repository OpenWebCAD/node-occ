#pragma once
#include "OCC.h"
#include "NodeV8.h"

#include "Base.h"

#include <vector>
class Face;
class Shell:  public Base 
{

    TopoDS_Shell m_shell;
public:
	int numFaces();

	double area();
	
 

	virtual const TopoDS_Shape& shape() const;
	const  TopoDS_Shell& shell() const { return m_shell;}
	virtual void setShape(const TopoDS_Shape&);

	virtual Base* Unwrap(v8::Local<v8::Object> obj) { return node::ObjectWrap::Unwrap<Shell>(obj); }
    virtual Local<Object> Clone() ;
    
	static void Init(Handle<Object> target);
	static Handle<Value> New(const Arguments& args);
	static Persistent<FunctionTemplate> constructor;
};