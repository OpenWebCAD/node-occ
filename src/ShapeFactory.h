#pragma once
#include "NodeV8.h"
#include "OCC.h"
#include <vector>

typedef enum BoolOpType {
  BOOL_FUSE,
  BOOL_CUT,
  BOOL_COMMON,
} BoolOpType;

class Solid;
class ShapeFactory
{
public:
	// boolean operation
	static Handle<v8::Value> fuse(const v8::Arguments& args);
	static Handle<v8::Value> cut(const v8::Arguments& args);
	static Handle<v8::Value> common(const v8::Arguments& args);
	static Handle<v8::Value> add(const v8::Arguments& args);
	// primitive constructions
	static Handle<v8::Value> makeBox(const v8::Arguments& args) ;
 	static Handle<v8::Value> makePrism(const Arguments& arg);
 	static Handle<v8::Value> makeCylinder(const Arguments& arg);
 	static Handle<v8::Value> makeCone(const Arguments& arg);
 	static Handle<v8::Value> makeSphere(const Arguments& arg);
private:
	static Handle<v8::Value> _boolean(const v8::Arguments& args,BoolOpType op);
	static TopoDS_Shape createBoolean(const TopoDS_Shape& firstObject,const TopoDS_Shape& secondObject, BoolOpType op);
	static Handle<v8::Object> add(const std::vector<Solid*>& solids);

};