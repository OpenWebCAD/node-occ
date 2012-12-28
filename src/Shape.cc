#include "Shape.h"
#include "Util.h"
#include "Transformation.h"

#include <map>
#include <vector>

using namespace std;




bool operator == (Local<String> str,const char* value)
{
    return str->Equals(String::New(value));
}





const TopoDS_Shape& Shape::shape()  const
{
    return shape_;
}

void Shape::setShape( const TopoDS_Shape& shape)
{
    shape_ = shape;
}



