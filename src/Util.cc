#include "Util.h"


void ReadPropertyPointFromArray( v8::Handle<v8::Array>& arr,double* x,double* y, double*z ) 
{
//    v8::Local<v8::Object> obj = value->ToObject();
//    v8::Handle<v8::Array> art = v8::Handle<v8::Array>::Cast(obj);
    int length = arr->Length();
    // int length = obj->Get(String::New("length"))->ToObject()->Uint32Value();

     if (length>=1) {  *x = arr->Get(0)->NumberValue();  }
     if (length>=2) {  *y = arr->Get(1)->NumberValue();  }
     if (length>=3) {  *z = arr->Get(2)->NumberValue();  }
  
}


//void ReadPropertyPoint( v8::Handle<v8::Object>& obj,const char* name,double* x,double* y, double*z ) 
//{
//  if (!obj.IsEmpty()) {
//    // for exemple a THREE.Vector3 
//    // ( we try to read the "x","y","z" property )
//    v8::Handle<v8::Value> v = obj->Get(v8::String::New(name));
//    return ReadPropertyPointFromArray(v->ToObject(),x,y,z);
//  }
//  //xx if (obj->IsArray()) {
//  //xx   ReadPropertyPointFromArray(value,x,y,z);
//  //xx }
//}


double ReadDouble(v8::Handle<v8::Object>& value,const char* name,double defaultValue)
{
   v8::Local<v8::Value> _v = value->ToObject()->Get(v8::String::New(name));
   return _v->ToNumber()->Value();
}

int ReadInt(v8::Handle<v8::Object>& value,const char* name,int defaultValue)
{
   v8::Local<v8::Value> _v = value->ToObject()->Get(v8::String::New(name));
   return _v->ToInteger()->ToInt32()->Value();
}

void ReadXYZ(v8::Handle<v8::Object>& obj,double* x,double* y,double* z)
{
    * x = ReadDouble(obj,"x",0.0);
    * y = ReadDouble(obj,"y",0.0);
    * z = ReadDouble(obj,"z",0.0);
}

void ReadPoint(v8::Local<v8::Value>& value,double* x,double* y, double*z)
{
  if (value->IsArray()) {
    v8::Handle<v8::Array> arr = v8::Handle<v8::Array>::Cast(value);
    ReadPropertyPointFromArray(arr,x,y,z);
       return;
  }
  if (value->IsObject()) {
    // object must have x,y,z property set
    v8::Handle<v8::Object> obj = v8::Handle<v8::Object>::Cast(value);
    ReadXYZ(obj,x,y,z);
   return;
  }
}

void ReadUVW(v8::Handle<v8::Object>& obj,double* x,double* y,double* z)
{
    * x = ReadDouble(obj,"u",0.0);
    * y = ReadDouble(obj,"v",0.0);
    * z = ReadDouble(obj,"w",0.0);
}
/*
void __cdecl boost::throw_exception(class std::exception const &)
{

}
*/