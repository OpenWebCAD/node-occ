#pragma once
#include "OCC.h"
#include "NodeV8.h"
#include "Util.h"


// expose a point 
class XYZMember
{
   virtual const gp_XYZ& get() const = 0;

   double x() { return get().X(); }
   double y() { return get().Y(); }
   double z() { return get().Z(); }

};

class Transformation : public node::ObjectWrap 
{
public:
  Transformation():m_translationPart_Acc(*this){};
  static v8::Handle<v8::Value> makeTranslation(const v8::Arguments& args);
  static v8::Handle<v8::Value> makePlaneMirror(const v8::Arguments& args);
  static v8::Handle<v8::Value> makeAxisMirror(const v8::Arguments& args);
  static v8::Handle<v8::Value> makeScale(const v8::Arguments& args);
  static v8::Handle<v8::Value> makeRotation(const v8::Arguments& args);

  double scaleFactor() { return m_transformation.ScaleFactor(); }


  XYZMember& translationPart() { return m_translationPart_Acc; } 
  
  gp_Trsf m_transformation;

  class MyXYZMember : public XYZMember
  {
     Transformation& m_parent;
  public:
     MyXYZMember(Transformation& parent):m_parent(parent){};
     virtual const gp_XYZ& get() const{ return m_parent.m_transformation.TranslationPart();}
  } m_translationPart_Acc;
  friend class MyXYZMember;


  // Methods exposed to JavaScripts
  static void Init(v8::Handle<v8::Object> target);
  static v8::Handle<v8::Value> NewInstance(const v8::Arguments& args);
  static v8::Handle<v8::Value> New(const v8::Arguments& args);

  static v8::Persistent<v8::Function> constructor;

private:
  Transformation(const Transformation&);
  void operator=(const Transformation&);
};