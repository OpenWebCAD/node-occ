#pragma once
#include "NodeV8.h"
#include "OCC.h"
#include "Base.h"

class Shape : public Base
{

public:
    Shape() {};
    virtual ~Shape() {};
    
protected:
    TopoDS_Shape shape_;    



// static Handle<Value> ShapeType(const Arguments& args);
// constructors

public:
   virtual const TopoDS_Shape& shape() const;
   virtual void setShape( const TopoDS_Shape&);

   bool fixShape() { return true;}
   void setErrorMessage(const char* message){};

};


