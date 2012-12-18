#pragma once
#include "NodeV8.h"
#include "OCC.h"
#include "Util.h"


// expose a point 
class Point3Wrap	 : public node::ObjectWrap 
{
   virtual const gp_XYZ& get() const = 0;

   double x() { return get().X(); }
   double y() { return get().Y(); }
   double z() { return get().Z(); }
  // Methods exposed to JavaScripts
  static void Init(Handle<Object> target);

};

class Transformation : public node::ObjectWrap 
{
public:
  Transformation()
//xx	  :m_translationPart_Acc(*this)
  {}
  static Handle<Value> makeTranslation(const Arguments& args);
  static Handle<Value> makePlaneMirror(const Arguments& args);
  static Handle<Value> makeAxisMirror(const Arguments& args);
  static Handle<Value> makeScale(const Arguments& args);
  static Handle<Value> makeRotation(const Arguments& args);

  double scaleFactor() { return m_trsf.ScaleFactor(); }


  
  gp_Trsf m_trsf;

  //class MyXYZMember : public Point3Wrap
  //{
  //   Transformation& m_parent;
  //public:
  //   MyXYZMember(Transformation& parent):m_parent(parent){};
  //   virtual const gp_XYZ& get() const{ return m_parent.m_transformation.TranslationPart();}
  //} m_translationPart_Acc;
  //friend class MyXYZMember;
  // Point3Wrap& translationPart() { return m_translationPart_Acc; } 


  // Methods exposed to JavaScripts
  static void Init(Handle<Object> target);
  static Handle<Value> NewInstance(const Arguments& args);
  static Handle<Value> New(const Arguments& args);

  static Persistent<FunctionTemplate> constructor;

private:
  Transformation(const Transformation&);
  void operator=(const Transformation&);
};