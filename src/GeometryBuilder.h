#pragma once
#include "OCC.h"
#include "NodeV8.h"

class Location : public node::ObjectWrap 
{

};




class Shape : public node::ObjectWrap 
{

public:
    Shape() {};
    virtual ~Shape() {};
    
protected:
    TopoDS_Shape shape_;    
    bool isNull();
    bool isValid();

// Methods exposed to JavaScripts
  static v8::Handle<v8::Value> translate(const v8::Arguments& args);
  static v8::Handle<v8::Value> rotate(const v8::Arguments& args);
  static v8::Handle<v8::Value> applyTransform(const v8::Arguments& args);

// constructors

public:
   const TopoDS_Shape& shape() const;
   void setShape( const TopoDS_Shape&);
   bool fixShape() { return true;}
   void setErrorMessage(const char* message){};

};


