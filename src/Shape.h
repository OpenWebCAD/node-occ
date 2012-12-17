#pragma once
#include "NodeV8.h"
#include "OCC.h"

class Shape : public node::ObjectWrap 
{

public:
    Shape() {};
    virtual ~Shape() {};
    
protected:
    TopoDS_Shape shape_;    
    bool isNull();
    bool isValid();
	const char* shapeType(); 

// Methods exposed to JavaScripts
  static Handle<Value> translate(const Arguments& args);
  static Handle<Value> rotate(const Arguments& args);
  static Handle<Value> applyTransform(const Arguments& args);
// static Handle<Value> ShapeType(const Arguments& args);
// constructors

public:
   const TopoDS_Shape& shape() const;
   void setShape( const TopoDS_Shape&);
   bool fixShape() { return true;}
   void setErrorMessage(const char* message){};

};


