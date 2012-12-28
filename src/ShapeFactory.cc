#include "ShapeFactory.h"

#include "Solid.h"
#include "Face.h"
#include "Util.h"



Handle<v8::Value> ShapeFactory::makeBox(const v8::Arguments& args)
{
    // could be :
    //    3 numbers dx,dy,dz
    //    2 points  p1,p2
    //TODO   1 point + 3 numbers dx,dy,dz
    //TODO    1 object with { x: 1,y: 2,z: 3, dw:
    HandleScope scope;
    Handle<v8::Value> pJhis = Solid::NewInstance();
    Solid* pThis = node::ObjectWrap::Unwrap<Solid>(pJhis->ToObject());

    double dx = 10;
    double dy = 10;
    double dz = 10;

    try {

        if (args.Length() == 3 && args[0]->IsNumber() && args[1]->IsNumber()  && args[2]->IsNumber() ) {

            dx  = args[0]->ToNumber()->Value();
            dy  = args[1]->ToNumber()->Value();
            dz  = args[2]->ToNumber()->Value();

            pThis->setShape(BRepPrimAPI_MakeBox(dx, dy, dz).Shape());

        } else if (args.Length() == 2) {

            gp_Pnt p1;
            ReadPoint(args[0],&p1);

            gp_Pnt p2;
            ReadPoint(args[1],&p2);

            pThis->setShape(BRepPrimAPI_MakeBox(p1,p2).Shape());

        } else if (args.Length() == 3) {

            gp_Pnt p1;
            ReadPoint(args[0],&p1);

            dx  = args[2]->ToNumber()->Value();
            dy  = args[3]->ToNumber()->Value();
            dz  = args[4]->ToNumber()->Value();

            pThis->setShape(BRepPrimAPI_MakeBox(p1,dx, dy, dz).Shape());


        }
    } catch(Standard_Failure&) {
        ThrowException(Exception::Error(String::New("cannot build box")));
    }
    return scope.Close(pJhis);

}


Handle<Value> ShapeFactory::makePrism(const Arguments& args)
{
    HandleScope scope;
    Handle<v8::Value> pJhis = Solid::NewInstance();
    Solid* pThis = node::ObjectWrap::Unwrap<Solid>(pJhis->ToObject());

    // <FACE> [x,y,z]
    // <FACE> [x,y,z] [x,y,z]
    // OCCBase *shape, OCCStruct3d p1, OCCStruct3d p2)
    Face* pFace = DynamicCast<Face>(args[0]);

    gp_Vec direction(0,0,10);
    ReadVector(args[1],&direction);

    if (!pFace || direction.IsEqual(gp_Vec(0,0,0),1E-7,1E-8)) {
        ThrowException(Exception::TypeError(String::New("invalid arguments : expecting <FACE>,<VECTOR>")));
    }

    try {
        const TopoDS_Shape& face = pFace->face();
        //// Only accept Edge or Wire
        //TopAbs_ShapeEnum type = shp.ShapeType();
        //if (type != TopAbs_EDGE && type != TopAbs_WIRE) {
        //    StdFail_NotDone::Raise("expected Edge or Wire");
        //}

        BRepPrimAPI_MakePrism prismMaker(face, direction);

        //xx prismMaker.Check();

        pThis->setShape(prismMaker.Shape());

        // possible fix shape
        if (!pThis->fixShape())   {
            StdFail_NotDone::Raise("Shapes not valid");
        }

    }
    CATCH_AND_RETHROW("Failed to create prism ");

    return scope.Close(pJhis);
}


Handle<Value> ShapeFactory::makeSphere(const Arguments& args)
{
    HandleScope scope;
    Handle<v8::Value> pJhis = Solid::NewInstance();
    Solid* pThis = node::ObjectWrap::Unwrap<Solid>(pJhis->ToObject());


    gp_Pnt center(0,0,0);
    ReadPoint(args[0],&center);
    double radius = args[1]->ToNumber()->Value();
    if (radius < 1E-7) {
        ThrowException(Exception::Error(String::New("invalid radius")));
        return scope.Close(pJhis);
    }
    try {

        pThis->setShape(BRepPrimAPI_MakeSphere(center, radius).Shape());

    }
    CATCH_AND_RETHROW("Failed to create sphere ");

    return scope.Close(pJhis);
}

bool ReadAx2(const Handle<Value>& value,gp_Ax2* ax2)
{
    assert(ax2);
    try {
        if (value->IsArray()) {
            Handle<Array> arr= Handle<Array>::Cast(value);
            gp_Pnt origin;
            ReadPoint(arr->Get(0),&origin);
            if (arr->Length() == 2) {
                // variation 2 :  gp_Ax2(const gp_Pnt& P,const gp_Dir& V);
                gp_Dir V;
                ReadDir(arr->Get(1),&V);
                *ax2 =  gp_Ax2(origin,V);
                return true;
            }
            if (arr->Length() == 3) {
                // variation 1 : gp_Ax2(const gp_Pnt& P,const gp_Dir& N,const gp_Dir& Vx);
                gp_Dir N,Vx;
                ReadDir(arr->Get(1),&N);
                ReadDir(arr->Get(2),&Vx);
                *ax2 =  gp_Ax2(origin,N,Vx);
                return true;
            }
        }
    }
    CATCH_AND_RETHROW("Failed to extract position");
    return false;
}
Handle<Value> ShapeFactory::makeCylinder(const Arguments& args)
{
    HandleScope scope;
    Handle<v8::Value> pJhis = Solid::NewInstance();
    Solid* pThis = node::ObjectWrap::Unwrap<Solid>(pJhis->ToObject());


    //  gp_Ax2& Axes
    //  gp_Ax2(const gp_Pnt& P,const gp_Dir& N,const gp_Dir& Vx);
    // Standard_EXPORT   BRepPrimAPI_MakeCylinder(const Standard_Real R,const Standard_Real H);
    // Standard_EXPORT   BRepPrimAPI_MakeCylinder(const Standard_Real R,const Standard_Real H,const Standard_Real Angle);
    // Standard_EXPORT   BRepPrimAPI_MakeCylinder(const gp_Ax2& Axes,const Standard_Real R,const Standard_Real H);
    // Standard_EXPORT   BRepPrimAPI_MakeCylinder(const gp_Ax2& Axes,const Standard_Real R,const Standard_Real H,const Standard_Real Angle);
    const double epsilon = 1E-3;

    gp_Ax2 axis;
    if (args.Length()==2) {

        // variation 1   <R:number> <H:number>
        // a vertical cylinder of radius R starting a (0,0,0) ending at (0,0,H)
        double R  = args[0]->ToNumber()->Value();
        double H  = args[1]->ToNumber()->Value();

        if ( R < epsilon || H < epsilon ) {
            ThrowException(Exception::Error(String::New("invalid value for arguments")));
        }
        try {
            pThis->setShape(BRepPrimAPI_MakeCylinder(R,H).Shape());
        }
        CATCH_AND_RETHROW("Failed to create cylinder ");

    } else if (args.Length()==3) {

        if (args[0]->IsArray() && args[1]->IsNumber() && args[2]->IsNumber()) {

            // variation 2
            //  <[ <Origin[x,yz]>, <VZn[x,yz]>,<VXn[x,yz]>] <R:number> <H:number>
            gp_Ax2  ax2;
            bool success = ReadAx2(args[0],&ax2);

            double R  = args[1]->ToNumber()->Value();
            double H  = args[2]->ToNumber()->Value();

            if ( R < epsilon || H < epsilon ) {
                ThrowException(Exception::Error(String::New("invalid value for arguments")));
            }
            try {
                pThis->setShape(BRepPrimAPI_MakeCylinder(ax2,R,H).Shape());
            }
            CATCH_AND_RETHROW("Failed to create cylinder ");

        } else if (args[0]->IsArray() && args[1]->IsArray() && args[2]->IsNumber()) {

            // variation 3 ( 2 points and a radius  )
            gp_Pnt p1;
            ReadPoint(args[0],&p1);

            gp_Pnt p2;
            ReadPoint(args[1],&p2);

            double R  = args[2]->ToNumber()->Value();

            const double dx = p2.X() - p1.X();
            const double dy = p2.Y() - p1.Y();
            const double dz = p2.Z() - p1.Z();

            const double H = sqrt(dx*dx + dy*dy + dz*dz);
            if (H < epsilon ) {
                ThrowException(Exception::Error(String::New("cannot build a cylinder on two coincident points")));
            }
            gp_Vec aV(dx / H, dy / H, dz / H);
            gp_Ax2 ax2(p1, aV);
            try {
                pThis->setShape(BRepPrimAPI_MakeCylinder(ax2,R,H).Shape());
            }
            CATCH_AND_RETHROW("Failed to create cylinder ");

        }
    } else {
        ThrowException(Exception::Error(String::New("invalid arguments")));
    }

    return scope.Close(pJhis);
}


Handle<Value> ShapeFactory::makeCone(const Arguments& args)
{
    HandleScope scope;
    Handle<v8::Value> pJhis = Solid::NewInstance();
    Solid* pThis = node::ObjectWrap::Unwrap<Solid>(pJhis->ToObject());

    const double epsilon = 1E-3;
    // Standard_EXPORT   BRepPrimAPI_MakeCone(const Standard_Real R1,const Standard_Real R2,const Standard_Real H);
    // Standard_EXPORT   BRepPrimAPI_MakeCone(const Standard_Real R1,const Standard_Real R2,const Standard_Real H,const Standard_Real angle);
    // Standard_EXPORT   BRepPrimAPI_MakeCone(const gp_Ax2& Axes,const Standard_Real R1,const Standard_Real R2,const Standard_Real H,const Standard_Real angle);
    if (args.Length()==3) {
        double R1 = args[0]->ToNumber()->Value();
        double R2 = args[1]->ToNumber()->Value();
        double H  = args[2]->ToNumber()->Value();

        if ( R1 < epsilon || R2 < epsilon || H < epsilon ) {
            ThrowException(Exception::Error(String::New("invalid value for arguments")));
        }
        try {
            pThis->setShape(BRepPrimAPI_MakeCone(R1, R2,H).Shape());
        }
        CATCH_AND_RETHROW("Failed to create sphere ");
    }  else {
        ThrowException(Exception::Error(String::New("invalid arguments")));
    }

    return scope.Close(pJhis);
}




TopoDS_Shape ShapeFactory::createBoolean(const TopoDS_Shape& firstObject,const TopoDS_Shape& secondObject, BoolOpType op)
{
    TopoDS_Shape shape;
    try {
        switch (op) {
        case BOOL_FUSE: {
            BRepAlgoAPI_Fuse tool(firstObject, secondObject);
            if (!tool.IsDone())
                Standard_ConstructionError::Raise("operation failed");
            shape = tool.Shape();
            break;
        }
        case BOOL_CUT: {
            BRepAlgoAPI_Cut tool(firstObject, secondObject);
            if (!tool.IsDone())
                Standard_ConstructionError::Raise("operation failed");
            shape = tool.Shape();
            break;
        }
        case BOOL_COMMON: {
            BRepAlgoAPI_Common tool(firstObject, secondObject);
            if (!tool.IsDone())
                Standard_ConstructionError::Raise("operation failed");
            shape = tool.Shape();
            break;
        }
        default:
            Standard_ConstructionError::Raise("unknown operation");
            break;
        }

        // check for empty compund shape
        TopoDS_Iterator It (shape, Standard_True, Standard_True);
        int found = 0;
        for (; It.More(); It.Next()) {
            found++;
        }
        if (found == 0) {
            Standard_ConstructionError::Raise("result object is empty compound");
        }

        // simplify compound with one solid into a Solid
        if (shape.ShapeType() == TopAbs_COMPOUND) {
            TopTools_IndexedMapOfShape shapeMap;
            TopExp::MapShapes(shape, TopAbs_SOLID, shapeMap);
            if (shapeMap.Extent() == 1) {
                shape = shapeMap(1);
            }
        }

    }
    CATCH_AND_RETHROW("Failed in boolean operation");
    return shape;
}



Handle<v8::Object> ShapeFactory::add(const std::vector<Solid*>& solids)
{
    HandleScope scope;
    TopoDS_Compound compound;
    BRep_Builder builder;
    try {

        builder.MakeCompound(compound);
        for (size_t i = 0; i < solids.size(); i++) {
            builder.Add(compound, solids[i]->shape());
        }

    }
    CATCH_AND_RETHROW("Failed in boolean operation");
    return scope.Close(Solid::NewInstance(compound)->ToObject());

}
Handle<v8::Value> ShapeFactory::add(const v8::Arguments& args)
{
    HandleScope scope;
    std::vector<Solid*> solids;
    for (int i=0; i<args.Length(); i++) {
        if (Solid::constructor->HasInstance(args[i]->ToObject())) {
            Solid* pSolid = node::ObjectWrap::Unwrap<Solid>(args[i]->ToObject());
            solids.push_back(pSolid);
        }
    }
    return add(solids);
}
Handle<v8::Value> ShapeFactory::_boolean(const v8::Arguments& args,BoolOpType op)
{
    HandleScope scope;

    if (!Solid::constructor->HasInstance(args[0]) || !Solid::constructor->HasInstance(args[1])) {
        ThrowException(Exception::TypeError(String::New("Wrong arguments for boolean operation : expecting two solids")));
        return scope.Close(Undefined());
    }

    Solid* pSolid1 = node::ObjectWrap::Unwrap<Solid>(args[0]->ToObject());

    std::vector<Solid*> other_solids;
    for (int i=1; i<args.Length(); i++) {
        if(Solid::constructor->HasInstance(args[i])) {
            Solid* pSolid2 = node::ObjectWrap::Unwrap<Solid>(args[i]->ToObject());
            other_solids.push_back(pSolid2);
        }
    }
    Handle<Object> compound = ShapeFactory::add(other_solids);
    Solid* pSolid2 = node::ObjectWrap::Unwrap<Solid>(compound);

    return scope.Close(Solid::NewInstance(createBoolean(pSolid1->shape(), pSolid2->shape(),op) ) );

}
Handle<v8::Value> ShapeFactory::fuse(const v8::Arguments& args)
{
    return _boolean(args,BOOL_FUSE);
}
Handle<v8::Value> ShapeFactory::cut(const v8::Arguments& args)
{
    return _boolean(args,BOOL_CUT);
}
Handle<v8::Value> ShapeFactory::common(const v8::Arguments& args)
{
    return _boolean(args,BOOL_COMMON);
}

