#pragma once
#include "OCC.h"
#include <node.h>
#include <v8.h>

class Location : public node::ObjectWrap 
{

};

class Shape : public node::ObjectWrap 
{

public:
    Shape() {};
    virtual ~Shape() {};
    
protected:
    TopoDS_Shape shape_;
    
    void ApplyOrigin(v8::Handle<v8::Object> json);
    void ApplyTransform(v8::Handle<v8::Object> json);
    void ApplyTransforms(v8::Handle<v8::Object> json);
    void ApplyWorkplane(v8::Handle<v8::Object> json);
    void ApplyReverseWorkplane(v8::Handle<v8::Object> json);

    virtual void PostProcess(v8::Handle<v8::Object> json) = 0;
   
    bool isNull();
    bool isValid();

// Methods exposed to JavaScripts
  static v8::Handle<v8::Value> translate(const v8::Arguments& args);
  static v8::Handle<v8::Value> rotate(const v8::Arguments& args);

// constructors

private:
    TopoDS_Shape shape();
    
};


class Solid : public Shape {
protected:
    Solid() {};
    virtual ~Solid() {};
    void Mesh();
    void PostProcess(v8::Handle<v8::Object> json);

 public:  
    // Methods exposed to JavaScripts
    static void Init(v8::Handle<v8::Object> target);
    static v8::Handle<v8::Value> NewInstance(const v8::Arguments& args);

    // boolean operation
    static v8::Handle<v8::Value> fuse(const v8::Arguments& args);
    static v8::Handle<v8::Value> cut(const v8::Arguments& args);
    static v8::Handle<v8::Value> common(const v8::Arguments& args);

   

    static v8::Handle<v8::Value> makeBox(const v8::Arguments& args) ;
    static v8::Handle<v8::Value> New(const v8::Arguments& args);     

    static v8::Persistent<v8::Function> constructor;

};

