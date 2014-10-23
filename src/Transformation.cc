#include "Transformation.h"
#include "Util.h"




NAN_METHOD(Transformation::makeTranslation)
{
  NanScope();
  Transformation* pThis = Transformation::Unwrap<Transformation>(args.This());
  if( args.Length()!=1) {
    NanThrowError("Wrong arguments");
    NanReturnUndefined();
  }
  double x=0,y=0,z=0;
  ReadPoint(args[0],&x,&y,&z);

  pThis->m_trsf.SetTranslation(gp_Vec(x,y,z));
  NanReturnValue(args.This());
}

NAN_METHOD(Transformation::makePlaneMirror)
{
  NanScope();
  Transformation* pThis = Transformation::Unwrap<Transformation>(args.This());
  if( args.Length()!=2) {
    NanThrowError("Wrong arguments");
    NanReturnUndefined();
  }

  double x=0,y=0,z=0;
  ReadPoint(args[0],&x,&y,&z);

  double u=0,v=0,w=0;
  ReadPoint(args[1],&u,&v,&w);

  Standard_Real D = sqrt (u * u + v * v + w * w);
  if (D <= gp::Resolution()) {
    NanThrowError("Plane Axis direction is null");
    NanReturnUndefined();
  }

  pThis->m_trsf.SetMirror(gp_Ax2(gp_Pnt(x, y, z), gp_Dir(u, v, w)));

  NanReturnValue(args.This());
}


NAN_METHOD(Transformation::makeAxisMirror)
{
  NanScope();
  Transformation* pThis = Transformation::Unwrap<Transformation>(args.This());
  if( args.Length()!=2) {
    NanThrowError("Wrong arguments");
    NanReturnUndefined();
  }

  double x=0,y=0,z=0;
  ReadPoint(args[0],&x,&y,&z);

  double u=0,v=0,w=0;
  ReadPoint(args[1],&u,&v,&w);

  Standard_Real D = sqrt (u * u + v * v + w * w);
  if (D <= gp::Resolution()) {
    NanThrowError("Lirror Axis direction is null");
    NanReturnUndefined();
  }

  pThis->m_trsf.SetMirror(gp_Ax1(gp_Pnt(x, y, z), gp_Dir(u, v, w)));

  NanReturnValue(args.This());
}

NAN_METHOD(Transformation::makeScale)
{
  NanScope();
  Transformation* pThis = Transformation::Unwrap<Transformation>(args.This());

  if( args.Length()!=2) {
    NanThrowError("Wrong arguments");
    NanReturnUndefined();
  }

  double factor=args[0]->NumberValue();

  double x=0,y=0,z=0;
  ReadPoint(args[1],&x,&y,&z);

  pThis->m_trsf.SetScale(gp_Pnt(x, y, z), factor);

  NanReturnValue(args.This());
}



NAN_METHOD(Transformation::makeRotation)
{
  NanScope();

  Transformation* pThis = Transformation::Unwrap<Transformation>(args.This());

  if( args.Length()!=3) {
    NanThrowError("Wrong arguments in makeRotation");
    NanReturnUndefined();
  }

  ReadRotationFromArgs(args, pThis->m_trsf);

  NanReturnValue(args.This());
}

// Methods exposed to JavaScripts
v8::Persistent<v8::FunctionTemplate> Transformation::_template;

void Transformation::Init(Handle<Object> target)
{
  v8::Local<v8::FunctionTemplate> tpl =  NanNew<v8::FunctionTemplate>(Transformation::New);
  // Prepare constructor template
  tpl->SetClassName(NanNew("Transformation"));

  // object has one internal filed ( the C++ object)
  tpl->InstanceTemplate()->SetInternalFieldCount(1);

  NanAssignPersistent<v8::FunctionTemplate>(_template,tpl);

  // Prototype
  Local<ObjectTemplate> proto = tpl->PrototypeTemplate();

  EXPOSE_METHOD(Transformation,makeRotation);
  EXPOSE_METHOD(Transformation,makeTranslation);
  EXPOSE_METHOD(Transformation,makePlaneMirror);
  EXPOSE_METHOD(Transformation,makeAxisMirror);
  EXPOSE_METHOD(Transformation,makeScale);
  EXPOSE_READ_ONLY_PROPERTY_DOUBLE(Transformation,scaleFactor);

  target->Set(NanNew("Transformation"), tpl->GetFunction());
}

NAN_METHOD(Transformation::New)
{
  NanScope();

  if (!args.IsConstructCall()) {
    NanThrowError(" use new occ.Transformation() to construct a transformation");
    NanReturnUndefined();
    // return FromConstructorTemplate(constructor, args);
  }

  Transformation* obj = new Transformation();
  obj->Wrap(args.This());
  NanReturnValue(args.This());
}

NAN_METHOD(Transformation::NewInstance)
{
  NanScope();

  // const unsigned argc = 1;
  // Handle<Value> argv[argc] = { args[0] };
  Local<Object> instance = NanNew<v8::FunctionTemplate>(_template)->GetFunction()->NewInstance(0,0);
  NanReturnValue(instance);
}