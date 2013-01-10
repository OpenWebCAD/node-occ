#include "Wire.h"




int Wire::numVertices()
{
    if (this->wire().IsNull()) return 0;
    TopTools_IndexedMapOfShape anIndices;
    TopExp::MapShapes(this->wire(), TopAbs_VERTEX, anIndices);
    return anIndices.Extent();
}

int Wire::numEdges()
{
    if (this->wire().IsNull()) return 0;
    TopTools_IndexedMapOfShape anIndices;
    TopExp::MapShapes(this->wire(), TopAbs_EDGE, anIndices);
    return anIndices.Extent();
}

bool Wire::isClosed()
{
    if (this->wire().IsNull()) return false;
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

    m_wire = TopoDS::Wire(shape);
}

Persistent<FunctionTemplate> Wire::constructor;


const char* toString(BRepBuilderAPI_WireError err)
{
    switch(err) {
    case BRepBuilderAPI_WireDone:
        return "the wire is done";
        break;
    case BRepBuilderAPI_EmptyWire:
        return "the wire is empty";
        break;
    case BRepBuilderAPI_DisconnectedWire:
        return "the wire is disconnected";
        break;
    case BRepBuilderAPI_NonManifoldWire:
        return "the wire is non-manifold";
        break;
    }
    return "";
}
Handle<Value> Wire::New(const Arguments& args)
{
    HandleScope scope;
    if (!args.IsConstructCall()) {
        ThrowException(Exception::TypeError(String::New(" use new occ.Wire() to construct a wire")));
        return scope.Close(Undefined());
    }

    Wire* pThis = new Wire();
    pThis->Wrap(args.This());

    if (args.Length()==0) {
        // this is a empty wire
        return args.This();
    }
    BRepBuilderAPI_MakeWire mkWire;

    Standard_Boolean status = false;

    BRepBuilderAPI_WireError err= BRepBuilderAPI_WireDone;

    for( int i =0; i< args.Length(); i++) {

        if (Edge::constructor->HasInstance(args[i]->ToObject())) {

            Edge* edge = node::ObjectWrap::Unwrap<Edge>(args[i]->ToObject());
            mkWire.Add(edge->edge());

            status = mkWire.IsDone();
            err = mkWire.Error();
        }     else if (Wire::constructor->HasInstance(args[i]->ToObject())) {

            Wire* wire = node::ObjectWrap::Unwrap<Wire>(args[i]->ToObject());
            mkWire.Add(wire->wire());
            status = mkWire.IsDone();
            err = mkWire.Error();
        }
    }

    err = mkWire.Error();
    if (BRepBuilderAPI_WireDone == err) {
        pThis->setShape(mkWire.Wire());
    }    else {
        ThrowException(Exception::Error(String::New((std::string("Invalid Wire err:=")+toString(mkWire.Error()) ).c_str())));
        return scope.Close(Undefined());
    }
    return args.This();
}

Local<Object>  Wire::Clone() const
{
    HandleScope scope;
    Wire* obj = new Wire();
    Local<Object> instance = constructor->GetFunction()->NewInstance();
    obj->Wrap(instance);
    obj->setShape(this->shape());
    return scope.Close(instance);
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

