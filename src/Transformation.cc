#include "Transformation.h"
#include "Util.h"




Handle<Value> Transformation::makeTranslation(const Arguments& args)
{
    HandleScope scope;
    Transformation* pThis = Transformation::Unwrap<Transformation>(args.This());
    if( args.Length()!=1) {
        ThrowException(Exception::TypeError(String::New("Wrong arguments")));
        return scope.Close(Undefined());
    }
    double x=0,y=0,z=0;
    ReadPoint(args[0],&x,&y,&z);

    pThis->m_trsf.SetTranslation(gp_Vec(x,y,z));
    return args.This();
}

Handle<Value> Transformation::makePlaneMirror(const Arguments& args)
{
    HandleScope scope;
    Transformation* pThis = Transformation::Unwrap<Transformation>(args.This());
    if( args.Length()!=2) {
        ThrowException(Exception::TypeError(String::New("Wrong arguments")));
        return scope.Close(Undefined());
    }

    double x=0,y=0,z=0;
    ReadPoint(args[0],&x,&y,&z);

    double u=0,v=0,w=0;
    ReadPoint(args[1],&u,&v,&w);

    Standard_Real D = sqrt (u * u + v * v + w * w);
    if (D <= gp::Resolution()) {
        ThrowException(Exception::TypeError(String::New("Plane Axis direction is null")));
        return scope.Close(Undefined());
    }

    pThis->m_trsf.SetMirror(gp_Ax2(gp_Pnt(x, y, z), gp_Dir(u, v, w)));

    return args.This();
}


Handle<Value> Transformation::makeAxisMirror(const Arguments& args)
{
    HandleScope scope;
    Transformation* pThis = Transformation::Unwrap<Transformation>(args.This());
    if( args.Length()!=2) {
        ThrowException(Exception::TypeError(String::New("Wrong arguments")));
        return scope.Close(Undefined());
    }

    double x=0,y=0,z=0;
    ReadPoint(args[0],&x,&y,&z);

    double u=0,v=0,w=0;
    ReadPoint(args[1],&u,&v,&w);

    Standard_Real D = sqrt (u * u + v * v + w * w);
    if (D <= gp::Resolution()) {
        ThrowException(Exception::TypeError(String::New("Lirror Axis direction is null")));
        return scope.Close(Undefined());
    }

    pThis->m_trsf.SetMirror(gp_Ax1(gp_Pnt(x, y, z), gp_Dir(u, v, w)));

    return args.This();
}

Handle<Value> Transformation::makeScale(const Arguments& args)
{
    HandleScope scope;
    Transformation* pThis = Transformation::Unwrap<Transformation>(args.This());

    if( args.Length()!=2) {
        ThrowException(Exception::TypeError(String::New("Wrong arguments")));
        return scope.Close(Undefined());
    }

    double factor=args[0]->NumberValue();

    double x=0,y=0,z=0;
    ReadPoint(args[1],&x,&y,&z);

    pThis->m_trsf.SetScale(gp_Pnt(x, y, z), factor);

    return args.This();
}

Handle<Value> Transformation::makeRotation(const Arguments& args)
{
    HandleScope scope;
    Transformation* pThis = Transformation::Unwrap<Transformation>(args.This());
    if( args.Length()!=3) {
        ThrowException(Exception::TypeError(String::New("Wrong arguments")));
        return scope.Close(Undefined());
    }

    double x=0,y=0,z=0;
    ReadPoint(args[0],&x,&y,&z);

    double u=0,v=0,w=0;
    ReadPoint(args[1],&u,&v,&w);

    double angle=args[2]->NumberValue();

    pThis->m_trsf.SetRotation(gp_Ax1(gp_Pnt(x,y,z), gp_Dir(u,v,w)),angle/180.0*M_PI);

    return args.This();
}

// Methods exposed to JavaScripts
Persistent<FunctionTemplate> Transformation::constructor;

void Transformation::Init(Handle<Object> target)
{
    // Prepare constructor template
    constructor = Persistent<FunctionTemplate>::New(FunctionTemplate::New(Transformation::New));
    constructor->SetClassName(String::NewSymbol("Transformation"));

    // object has one internal filed ( the C++ object)
    constructor->InstanceTemplate()->SetInternalFieldCount(1);

    // Prototype
    Local<ObjectTemplate> proto = constructor->PrototypeTemplate();

    EXPOSE_METHOD(Transformation,makeRotation);
    EXPOSE_METHOD(Transformation,makeTranslation);
    EXPOSE_METHOD(Transformation,makePlaneMirror);
    EXPOSE_METHOD(Transformation,makeAxisMirror);
    EXPOSE_METHOD(Transformation,makeScale);
    EXPOSE_READ_ONLY_PROPERTY_DOUBLE(Transformation,scaleFactor);

    target->Set(String::NewSymbol("Transformation"), constructor->GetFunction());
}

Handle<Value> Transformation::New(const Arguments& args)
{
    HandleScope scope;

    if (!args.IsConstructCall()) {
        ThrowException(Exception::TypeError(String::New(" use new occ.Transformation() to construct a transformation")));
        return scope.Close(Undefined());
        // return FromConstructorTemplate(constructor, args);
    }

    Transformation* obj = new Transformation();
    obj->Wrap(args.This());
    // return scope.Close(args.This());
    return args.This();
}

Handle<Value> Transformation::NewInstance(const Arguments& args)
{
    HandleScope scope;

    // const unsigned argc = 1;
    // Handle<Value> argv[argc] = { args[0] };
    Local<Object> instance = constructor->GetFunction()->NewInstance(0,0);
    return scope.Close(instance);
}