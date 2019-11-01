#include "Shape.h"
#include "Util.h"
#include "Transformation.h"

#include <map>
#include <vector>

using namespace std;




bool operator == (v8::Local<v8::String> str,const char* value)
{
    return Nan::Equals(str,Nan::New<v8::String>(value).ToLocalChecked()).ToChecked();
}


const TopoDS_Shape& Shape::shape()  const
{
    return shape_;
}

void Shape::setShape( const TopoDS_Shape& shape)
{
    shape_ = shape;
}



