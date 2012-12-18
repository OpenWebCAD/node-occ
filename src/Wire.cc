#include "Wire.h"




int Wire::numVertices()
{
    TopTools_IndexedMapOfShape anIndices;
    TopExp::MapShapes(this->wire(), TopAbs_VERTEX, anIndices);
    return anIndices.Extent();
}

int Wire::numEdges()
{
    TopTools_IndexedMapOfShape anIndices;
    TopExp::MapShapes(this->wire(), TopAbs_EDGE, anIndices);
    return anIndices.Extent();
}

bool Wire::isClosed()
{
    TopoDS_Vertex aV1, aV2;
    TopExp::Vertices(this->wire(), aV1, aV2);
    if (!aV1.IsNull() && !aV2.IsNull() && aV1.IsSame(aV2))
        return true;
    return false;
}


const TopoDS_Shape& Wire::shape() const 
{
	return m_wire;
}
void Wire::setShape( const TopoDS_Shape& shape)
{
	TopoDS::Wire(shape);
}

Persistent<FunctionTemplate> Wire::constructor;

Handle<Value> Wire::New(const Arguments& args)
{
  HandleScope scope;

  Wire* pThis = new Wire();

  BRepBuilderAPI_MakeWire mkWire;

  for( uint32_t i =0;i< args.Length();i++) {
    
	  if (Edge::constructor->HasInstance(args[i]->ToObject())) {

		  Edge* edge = Unwrap<Edge>(args[i]->ToObject());
		  mkWire.Add(edge->edge());
	  }

  }	
  
  pThis->setShape(mkWire.Wire());
  return args.This();
}

void Wire::Init(Handle<Object> target)
{
  // Prepare constructor template
  constructor = Persistent<FunctionTemplate>::New(FunctionTemplate::New(Wire::New));
  constructor->SetClassName(String::NewSymbol("Wire"));

  // object has one internal filed ( the C++ object)
  constructor->InstanceTemplate()->SetInternalFieldCount(1);

  // Prototype
  Local<ObjectTemplate> proto = constructor->PrototypeTemplate();

  Base::InitProto(proto);

  EXPOSE_READ_ONLY_PROPERTY_INTEGER(Wire,numVertices);
  EXPOSE_READ_ONLY_PROPERTY_INTEGER(Wire,numEdges);
  EXPOSE_READ_ONLY_PROPERTY_BOOLEAN(Wire,isClosed);

  target->Set(String::NewSymbol("Wire"), constructor->GetFunction());
}

