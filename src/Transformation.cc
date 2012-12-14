#include "Transformation.h"
#include "Util.h"

v8::Handle<v8::Value> Transformation::makeTranslation(const v8::Arguments& args)
{
    v8::HandleScope scope;
    Transformation* pThis = Transformation::Unwrap<Transformation>(args.This());
    if( args.Length()!=1) {
      v8::ThrowException(v8::Exception::TypeError(v8::String::New("Wrong arguments")));
      return scope.Close(v8::Undefined());
    }
    double x=0,y=0,z=0;
    ReadPoint(args[0],&x,&y,&z);

    pThis->m_transformation.SetTranslation(gp_Vec(x,y,z));
    return scope.Close(args.This());
}

v8::Handle<v8::Value> Transformation::makePlaneMirror(const v8::Arguments& args)
{
    v8::HandleScope scope;
    Transformation* pThis = Transformation::Unwrap<Transformation>(args.This());
    if( args.Length()!=2) {
      v8::ThrowException(v8::Exception::TypeError(v8::String::New("Wrong arguments")));
      return scope.Close(v8::Undefined());
    }

    double x=0,y=0,z=0;
    ReadPoint(args[0],&x,&y,&z);

    double u=0,v=0,w=0;
    ReadPoint(args[1],&u,&v,&w);
    
    pThis->m_transformation.SetMirror(gp_Ax2(gp_Pnt(x, y, z), gp_Dir(u, v, w))); 
      
    return scope.Close(args.This());
}
  
  
v8::Handle<v8::Value> Transformation::makeAxisMirror(const v8::Arguments& args)
{
    v8::HandleScope scope;
    Transformation* pThis = Transformation::Unwrap<Transformation>(args.This());
    if( args.Length()!=2) {
      v8::ThrowException(v8::Exception::TypeError(v8::String::New("Wrong arguments")));
      return scope.Close(v8::Undefined());
    }

    double x=0,y=0,z=0;
    ReadPoint(args[0],&x,&y,&z);

    double u=0,v=0,w=0;
    ReadPoint(args[1],&u,&v,&w);
    
    pThis->m_transformation.SetMirror(gp_Ax1(gp_Pnt(x, y, z), gp_Dir(u, v, w))); 
      
    return scope.Close(args.This());
}
  
v8::Handle<v8::Value> Transformation::makeScale(const v8::Arguments& args)
{
    v8::HandleScope scope;
    Transformation* pThis = Transformation::Unwrap<Transformation>(args.This());

    if( args.Length()!=2) {
      v8::ThrowException(v8::Exception::TypeError(v8::String::New("Wrong arguments")));
      return scope.Close(v8::Undefined());
    }

    double factor=args[0]->NumberValue();

    double x=0,y=0,z=0;
    ReadPoint(args[1],&x,&y,&z);
    
    pThis->m_transformation.SetScale(gp_Pnt(x, y, z), factor);
      
    return scope.Close(args.This());
}

v8::Handle<v8::Value> Transformation::makeRotation(const v8::Arguments& args)
{
    v8::HandleScope scope;
    Transformation* pThis = Transformation::Unwrap<Transformation>(args.This());
    if( args.Length()!=3) {
      v8::ThrowException(v8::Exception::TypeError(v8::String::New("Wrong arguments")));
      return scope.Close(v8::Undefined());
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
v8::Persistent<v8::Function> Transformation::constructor;

void Transformation::Init(v8::Handle<v8::Object> target)
{
  // Prepare constructor template
  v8::Local<v8::FunctionTemplate> tpl = v8::FunctionTemplate::New(Transformation::New);
  tpl->SetClassName(v8::String::NewSymbol("Transformation"));

  // object has one internal filed ( the C++ object)
  tpl->InstanceTemplate()->SetInternalFieldCount(1);


  // Prototype
  v8::Local<v8::ObjectTemplate> objTemplate = tpl->PrototypeTemplate();


  tpl->PrototypeTemplate()->Set(v8::String::NewSymbol("makeRotation"),      v8::FunctionTemplate::New(makeRotation)->GetFunction());
  tpl->PrototypeTemplate()->Set(v8::String::NewSymbol("makeTranslation"),   v8::FunctionTemplate::New(makeTranslation)->GetFunction());  
  tpl->PrototypeTemplate()->Set(v8::String::NewSymbol("makePlaneMirror"),   v8::FunctionTemplate::New(makePlaneMirror)->GetFunction());
  tpl->PrototypeTemplate()->Set(v8::String::NewSymbol("makeAxisMirror"),    v8::FunctionTemplate::New(makeAxisMirror)->GetFunction());
  tpl->PrototypeTemplate()->Set(v8::String::NewSymbol("makeScale"),         v8::FunctionTemplate::New(makeScale)->GetFunction());

  tpl->PrototypeTemplate()->SetAccessor(v8::String::NewSymbol("scaleFactor"), 
    ee<Transformation,v8::Number,double,&Transformation::scaleFactor>,  0,v8::Handle<v8::Value>(),v8::DEFAULT,v8::ReadOnly);

  constructor = v8::Persistent<v8::Function>::New(tpl->GetFunction());
  target->Set(v8::String::NewSymbol("Transformation"), constructor);
}

v8::Handle<v8::Value> Transformation::New(const v8::Arguments& args)
{
  v8::HandleScope scope;
  
  Transformation* obj = new Transformation();
  obj->Wrap(args.This());
  // return scope.Close(args.This());
  return args.This();
}
v8::Handle<v8::Value> Transformation::NewInstance(const v8::Arguments& args)
{
  v8::HandleScope scope;

  const unsigned argc = 1;
  v8::Handle<v8::Value> argv[argc] = { args[0] };
  v8::Local<v8::Object> instance = constructor->NewInstance(argc, argv);

  return scope.Close(instance);
}