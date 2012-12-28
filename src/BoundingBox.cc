#include "BoundingBox.h"

#include "Util.h"
#include <limits.h>

Persistent<FunctionTemplate> BoundingBox::constructor;


Handle<Object> BoundingBox::NewInstance(const Bnd_Box& box)
{
    HandleScope scope;

    Local<Object> instance = constructor->GetFunction()->NewInstance(0,0);

    BoundingBox* pThis = ObjectWrap::Unwrap<BoundingBox>(instance);

    pThis->m_box = box;

    return scope.Close(instance);
}
Handle<Object> BoundingBox::NewInstance(const gp_Pnt& nearPt,const gp_Pnt& farPt)
{
    Bnd_Box box;
    box.Add(nearPt);
    box.Add(farPt);
    return NewInstance(box);
}

Handle<Value> BoundingBox::New(const v8::Arguments& args)
{


    HandleScope scope;
    BoundingBox* pThis = new BoundingBox();
    pThis->Wrap(args.This());

    // args could be one or several points
    // or a array of point
    for (int i=0; i<args.Length(); i++) {
        if (args[i]->IsArray()) {
            Handle<Array> arr = Handle<Array>::Cast(args[i]);
            if ( arr->Get(0)->IsArray() || arr->Get(0)->IsObject()) {
                // probably an array of point
            } else {
                // a single point
                gp_Pnt point;
                ReadPoint(args[i],&point);
                pThis->m_box.Update(point.X(),point.Y(),point.Z());
            }
        }
    }
    return args.This();
}

Handle<v8::Value> BoundingBox::addPoint(const v8::Arguments& args)
{
    HandleScope scope;

    return Undefined();
}

Handle<v8::Value> BoundingBox::isOut(const v8::Arguments& args)
{
    HandleScope scope;
    bool _itOut = false;
    // pThis->isOut(pnt);
    return scope.Close(Boolean::New(_itOut));
}


void BoundingBox::Init(Handle<Object> target)
{
    // Prepare constructor template
    constructor = Persistent<FunctionTemplate>::New(FunctionTemplate::New(BoundingBox::New));
    constructor->SetClassName(String::NewSymbol("BoundingBox"));

    // object has one internal filed ( the C++ object)
    constructor->InstanceTemplate()->SetInternalFieldCount(1);


    // Prototype
    Local<ObjectTemplate> proto = constructor->PrototypeTemplate();

    EXPOSE_METHOD(BoundingBox,addPoint);
    EXPOSE_METHOD(BoundingBox,isOut);

    EXPOSE_TEAROFF(BoundingBox,nearPt);
    EXPOSE_TEAROFF(BoundingBox,farPt);
    EXPOSE_READ_ONLY_PROPERTY_BOOLEAN(BoundingBox,isVoid);


    target->Set(String::NewSymbol("BoundingBox"), constructor->GetFunction());
}