#include "Transformation.h"
#include "Util.h"


// Methods exposed to JavaScripts
void Point3Wrap::Init(Handle<Object> target)
{
   // Prepare constructor template
  Local<FunctionTemplate> tpl = FunctionTemplate::New(Transformation::New);
  tpl->SetClassName(String::NewSymbol("Point3"));

  // object has one internal filed ( the C++ object)
  tpl->InstanceTemplate()->SetInternalFieldCount(1);

  // Prototype
  Local<ObjectTemplate> objTemplate = tpl->PrototypeTemplate();


}



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

    pThis->m_transformation.SetTranslation(gp_Vec(x,y,z));
    return scope.Close(args.This());
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
    
    pThis->m_transformation.SetMirror(gp_Ax2(gp_Pnt(x, y, z), gp_Dir(u, v, w))); 
      
    return scope.Close(args.This());
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
    
    pThis->m_transformation.SetMirror(gp_Ax1(gp_Pnt(x, y, z), gp_Dir(u, v, w))); 
      
    return scope.Close(args.This());
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
    
    pThis->m_transformation.SetScale(gp_Pnt(x, y, z), factor);
      
    return scope.Close(args.This());
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
    
    pThis->m_transformation.SetRotation(gp_Ax1(gp_Pnt(x,y,z), gp_Dir(u,v,w)),angle/180.0*M_PI);
      
    return scope.Close(args.This());
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
  
  Transformation* obj = new Transformation();
  obj->Wrap(args.This());
  // return scope.Close(args.This());
  return args.This();
}

Handle<Value> Transformation::NewInstance(const Arguments& args)
{
  HandleScope scope;

  const unsigned argc = 1;
  Handle<Value> argv[argc] = { args[0] };
  Local<Object> instance = constructor->GetFunction()->NewInstance(argc, argv);

  return scope.Close(instance);
}