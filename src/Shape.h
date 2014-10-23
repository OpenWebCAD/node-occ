#pragma once
#include "NodeV8.h"
#include "OCC.h"
#include "Base.h"

class Shape : public Base {

public:
    Shape() {};
    virtual ~Shape() {};

protected:
    TopoDS_Shape shape_;



    // static NAN_METHOD(ShapeType);
   // constructors

public:
    virtual const TopoDS_Shape& shape() const;
    virtual void setShape( const TopoDS_Shape&);



    void setErrorMessage(const char* message) {};

};


