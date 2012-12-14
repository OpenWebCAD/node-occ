#pragma once
#include "NodeV8.h"
 
int ReadInt(v8::Handle<v8::Object>& obj,const char* name,int defaultValue);
double ReadDouble(v8::Handle<v8::Object>& obj,const char* name,double defaultValue=0.0);

void ReadPropertyPointFromArray( v8::Handle<v8::Array>& value,double* x,double* y, double*z );
// void ReadPropertyPoint( v8::Handle<v8::Object>& value,const char* name,double* x,double* y, double*z );

void ReadXYZ(v8::Handle<v8::Object>& value,double* x,double* y,double* z);
void ReadUVW(v8::Handle<v8::Object>& value,double* x,double* y,double* z);

void ReadPoint(v8::Local<v8::Value>& value,double* x,double* y, double*z);
