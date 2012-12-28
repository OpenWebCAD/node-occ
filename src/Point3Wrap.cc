#include "Point3Wrap.h"

Persistent<FunctionTemplate> Point3Wrap::constructor;

// Methods exposed to JavaScripts
Handle<Value> Point3Wrap::New(const Arguments& args)
{
    return args.This();
}
// Methods exposed to JavaScripts
void Point3Wrap::Init(Handle<Object> target)
{
    // Prepare constructor template
    constructor = Persistent<FunctionTemplate>::New(FunctionTemplate::New(Point3Wrap::New));
    constructor->SetClassName(String::NewSymbol("Point3Wrap"));

    // object has one internal filed ( the C++ object)
    constructor->InstanceTemplate()->SetInternalFieldCount(1);

    // Prototype
    Local<ObjectTemplate> proto = constructor->PrototypeTemplate();
    EXPOSE_READ_ONLY_PROPERTY_DOUBLE(Point3Wrap,x);
    EXPOSE_READ_ONLY_PROPERTY_DOUBLE(Point3Wrap,y);
    EXPOSE_READ_ONLY_PROPERTY_DOUBLE(Point3Wrap,z);

}
