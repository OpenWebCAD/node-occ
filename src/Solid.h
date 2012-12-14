#pragma once
#include "GeometryBuilder.h"


class Solid : public Shape {
protected:
    Solid() {};
    virtual ~Solid() {};
    void Mesh();


 public:  
   int numSolids();
   int numFaces();

   double volume();
   double area();

   typedef enum BoolOpType {
      BOOL_FUSE,
      BOOL_CUT,
      BOOL_COMMON,
   } BoolOpType;
   int boolean(Solid *tool, BoolOpType op);

    // boolean operation
    static v8::Handle<v8::Value> _boolean(const v8::Arguments& args,BoolOpType op);
    static v8::Handle<v8::Value> fuse(const v8::Arguments& args);
    static v8::Handle<v8::Value> cut(const v8::Arguments& args);
    static v8::Handle<v8::Value> common(const v8::Arguments& args);

   

    static v8::Handle<v8::Value> makeBox(const v8::Arguments& args) ;

    // Methods exposed to JavaScripts
    static void Init(v8::Handle<v8::Object> target);
    static v8::Handle<v8::Value> NewInstance(const v8::Arguments& args);
    static v8::Handle<v8::Value> New(const v8::Arguments& args);     

    static v8::Persistent<v8::Function> constructor;

};

