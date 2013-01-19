#include "ShapeFactory.h"

#include "Solid.h"
#include "Face.h"
#include "Util.h"


#include <memory>

static void registerMakeBoxFaces(Solid* pThis,BRepPrimAPI_MakeBox& tool)
{
    pThis->_registerNamedShape("top",   tool.TopFace());
    pThis->_registerNamedShape("bottom",tool.BottomFace());
    pThis->_registerNamedShape("right", tool.RightFace());
    pThis->_registerNamedShape("left",  tool.LeftFace());
    pThis->_registerNamedShape("front", tool.FrontFace());
    pThis->_registerNamedShape("back",  tool.BackFace());
}


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

            BRepPrimAPI_MakeBox  tool(dx, dy, dz);
            pThis->setShape(tool.Shape());
            registerMakeBoxFaces(pThis,tool);



        } else if (args.Length() == 2) {

            gp_Pnt p1;
            ReadPoint(args[0],&p1);

            gp_Pnt p2;
            ReadPoint(args[1],&p2);

            BRepPrimAPI_MakeBox tool(p1,p2);
            pThis->setShape(tool.Shape());
            registerMakeBoxFaces(pThis,tool);


        } else if (args.Length() == 3) {

            gp_Pnt p1;
            ReadPoint(args[0],&p1);

            dx  = args[2]->ToNumber()->Value();
            dy  = args[3]->ToNumber()->Value();
            dz  = args[4]->ToNumber()->Value();

            BRepPrimAPI_MakeBox tool(p1,dx, dy, dz);
            pThis->setShape(tool.Shape());
            registerMakeBoxFaces(pThis,tool);


        }
    } catch(Standard_Failure&) {
        ThrowException(Exception::Error(String::New("cannot build box")));
    }
    return scope.Close(pJhis);

}


static void registerMakeBoxFaces(Solid* pThis,BRepPrimAPI_MakePrism& tool)
{
	pThis->_registerNamedShape("bottom",   tool.FirstShape());
	pThis->_registerNamedShape("top",      tool.LastShape());

    // const TopTools_ListOfShape& list = tool.Generated(S);
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

        BRepPrimAPI_MakePrism prismMaker(face, direction);


        pThis->setShape(prismMaker.Shape());
		registerMakeBoxFaces(pThis,prismMaker);

        // possible fix shape
        if (!pThis->fixShape())   {
            StdFail_NotDone::Raise("Shapes not valid");
        }

    }
    CATCH_AND_RETHROW("Failed to create prism ");

    return scope.Close(pJhis);
}

static void registerOneAxisFaces(Solid* pThis,BRepPrim_OneAxis& tool)
{
    pThis->_registerNamedShape("lateral",tool.LateralFace());
    if (tool.HasSides())   {
        pThis->_registerNamedShape("start",  tool.StartFace());
        pThis->_registerNamedShape("end",    tool.EndFace());
    }
    if (tool.HasTop())     {
        pThis->_registerNamedShape("top",    tool.TopFace());
    }
    if (tool.HasBottom())  {
        pThis->_registerNamedShape("bottom", tool.BottomFace());
    }
    /*
         TopoDS_Wire& AxisStartWire() ;
         TopoDS_Wire& EndWire() ;
         TopoDS_Wire& AxisEndWire() ;
         TopoDS_Edge& AxisEdge() ;
         TopoDS_Edge& StartEdge() ;
         TopoDS_Edge& EndEdge() ;
         TopoDS_Edge& StartTopEdge() ;
         TopoDS_Edge& StartBottomEdge() ;
         TopoDS_Edge& EndTopEdge() ;
         TopoDS_Edge& EndBottomEdge() ;
         TopoDS_Edge& TopEdge() ;
         TopoDS_Edge& BottomEdge() ;
         TopoDS_Vertex& AxisTopVertex() ;
         TopoDS_Vertex& AxisBottomVertex() ;
         TopoDS_Vertex& TopStartVertex() ;
         TopoDS_Vertex& TopEndVertex() ;
         TopoDS_Vertex& BottomStartVertex() ;
         TopoDS_Vertex& BottomEndVertex() ;
    */
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
        BRepPrimAPI_MakeSphere tool(center, radius);
        pThis->setShape(tool.Shape());
        registerOneAxisFaces(pThis,tool.Sphere());
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
            bool success = ReadAx2(args[0],&ax2); (void)success;

            double R  = args[1]->ToNumber()->Value();
            double H  = args[2]->ToNumber()->Value();

            if ( R < epsilon || H < epsilon ) {
                ThrowException(Exception::Error(String::New("invalid value for arguments")));
            }
            try {
				BRepPrimAPI_MakeCylinder tool(ax2,R,H);
                pThis->setShape(tool.Shape());
				registerOneAxisFaces(pThis,tool.Cylinder());
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
				BRepPrimAPI_MakeCylinder tool(ax2,R,H);
                pThis->setShape(tool.Shape());
				registerOneAxisFaces(pThis,tool.Cylinder());
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
    if (args.Length()==3 && args[0]->IsNumber() && args[1]->IsNumber() && args[2]->IsNumber()) {

        double R1 = args[0]->ToNumber()->Value();
        double R2 = args[1]->ToNumber()->Value();
        double H  = args[2]->ToNumber()->Value();

        if ( R1 < epsilon || R2 < epsilon || H < epsilon ) {
            ThrowException(Exception::Error(String::New("invalid value for arguments")));
			return scope.Close(Undefined());
        }
        try {
			BRepPrimAPI_MakeCone tool(R1, R2,H);
            pThis->setShape(tool.Shape());
			registerOneAxisFaces(pThis,tool.Cone());
        }
        CATCH_AND_RETHROW("Failed to create Cone ");
	} else if (args.Length()==3 && args[0]->IsArray() && args[1]->IsNumber() && args[2]->IsNumber()) {
		
		gp_Dir axis;
		ReadDir(args[0],&axis);

		double angleInRadian = 0;
		ReadDouble(args[1],angleInRadian);

		double height = 0;
		ReadDouble(args[1],height);

		ThrowException(Exception::Error(String::New("Cone with [u,v,w],angle,height not implemented yet")));
		return scope.Close(Undefined());

    } else if (args.Length()==4 && args[0]->IsArray() && args[1]->IsNumber() && args[2]->IsArray() && args[3]->IsNumber()) {
        // Point, point , R1,R2);
        // variation 3 ( 2 points and a radius  )
        gp_Pnt p1;
        ReadPoint(args[0],&p1);

        double R1  = 10;
        ReadDouble(args[1],R1);


        gp_Pnt p2;
        ReadPoint(args[2],&p2);

        double R2  = 11;
        ReadDouble(args[3],R2);

        const double dx = p2.X() - p1.X();
        const double dy = p2.Y() - p1.Y();
        const double dz = p2.Z() - p1.Z();

        const double H = sqrt(dx*dx + dy*dy + dz*dz);
        if (H < epsilon ) {
            ThrowException(Exception::Error(String::New("cannot build a cone on two coincident points")));
          return scope.Close(Undefined());
        }
        gp_Vec aV(dx / H, dy / H, dz / H);
        gp_Ax2 ax2(p1, aV);
        try {
            BRepPrimAPI_MakeCone tool(ax2,R1,R2,H);
            pThis->setShape(tool.Shape());  
            registerOneAxisFaces(pThis,tool.Cone());
        }
        CATCH_AND_RETHROW("Failed to create cone ");

    } else if (args.Length()==4 && args[0]->IsArray() && args[1]->IsArray() && args[2]->IsNumber() && args[3]->IsNumber()) {
      // cone with a sharp apex 
      // apex, direction ,  half_angle, height
      gp_Pnt apex;
      ReadPoint(args[0],&apex);

      gp_Dir innerDir;
      ReadDir(args[1],&innerDir);

      double half_angle_in_radian=atan(1.0); // default : 45° 
      ReadDouble(args[2],half_angle_in_radian);

      double height = 100;
      ReadDouble(args[3],height);
      // r/h= tan(a);

      try {
        gp_Ax2 ax2(apex, innerDir);
        BRepPrimAPI_MakeCone tool(ax2,0,height*tan(half_angle_in_radian),height);
        pThis->setShape(tool.Shape());  
        registerOneAxisFaces(pThis,tool.Cone());
      }
      CATCH_AND_RETHROW("Failed to create cone ");
          
          
      
     } else {
        ThrowException(Exception::Error(String::New("invalid arguments (cone)")));
    }

    return scope.Close(pJhis);
}

Handle<Value> ShapeFactory::makeTorus(const Arguments& args)
{
    HandleScope scope;
    Handle<v8::Value> pJhis = Solid::NewInstance();
    Solid* pThis = node::ObjectWrap::Unwrap<Solid>(pJhis->ToObject());

	// variation 1
	//  
	//  Center as <x,y,z>
	//  axis   as <u,v,w>
	//  bigRadius
	//  smallRadius
    if (args.Length()==4 && args[2]->IsNumber() && args[3]->IsNumber()) {
		//
        gp_Pnt center;
        ReadPoint(args[0],&center);
		//
		gp_Dir axis;
		ReadDir(args[1],&axis);
		
		double bigR = 100;
		ReadDouble(args[2],bigR);
        double smallR = 10;
		ReadDouble(args[3],smallR);

        try {
		    BRepPrimAPI_MakeTorus tool(gp_Ax2(center,axis),bigR,smallR);
			pThis->setShape(tool.Shape());
			registerOneAxisFaces(pThis,tool.Torus());
			return scope.Close(pJhis);
        }
        CATCH_AND_RETHROW("Failed to create Torus ");
	}
	ThrowException(Exception::Error(String::New("invalid arguments (makeTorus)")));
	return scope.Close(Undefined());
}



#include <TopTools_ListOfShape.hxx>
#include <TopTools_ListIteratorOfListOfShape.hxx>


static void registeredShape(Solid* result,Solid* pSolid,const char* op, int operand,int counter,const TopoDS_Shape& originalShape,TopoDS_Shape& modifiedShape)
{
    std::string originalName = pSolid->_getShapeName(originalShape);
    std::stringstream s ;
    s << op << operand << ":"  << originalName;
    if (counter >=0) {
        s << ":" << counter ;
    }
    s << std::ends;
    result->_registerNamedShape(s.str().c_str(),modifiedShape);
}


static void registerShapes(BRepAlgoAPI_BooleanOperation* pTool,Solid* result,Solid* pSolid,int operand)
{
    const TopoDS_Shape& shape = pSolid->shape();

    TopTools_MapOfShape alreadyProcessedShapes;


    TopExp_Explorer shapeExplorer(shape,TopAbs_FACE);
    for (; shapeExplorer.More(); shapeExplorer.Next ()) {
        const TopoDS_Shape& current = shapeExplorer.Current();
        if (alreadyProcessedShapes.Contains(current))
            continue;
        alreadyProcessedShapes.Add(current);

        if (pTool->IsDeleted(current)) {
            continue; // skipping deleted shapes
        }

        int actionCounter = 0;
        {
            const TopTools_ListOfShape& generatedShapes = pTool->Generated(current);
            int counter =0;
            TopTools_ListIteratorOfListOfShape it(generatedShapes);
            for (; it.More (); it.Next ()) {
                TopoDS_Shape& newShape = it.Value();
                if (current.IsSame(newShape)) {
                    registeredShape(result,pSolid,"S",operand,counter++,current,newShape);
                    actionCounter++;
                } else {
                    registeredShape(result,pSolid,"g",operand,counter++,current,newShape);
                    actionCounter++;
                }
                //xx Builder.Generated (root,newShape );
                //xx registerGeneratedShape(pSolid,shape,current,newShape);
            }
        }

        {
            int counter =0;
            const TopTools_ListOfShape& modifiedShapes = pTool->Modified2(current);
            TopTools_ListIteratorOfListOfShape it(modifiedShapes);
            for (; it.More (); it.Next ()) {
                TopoDS_Shape& newShape = it.Value();
                if (current.IsSame(newShape)) {
                    // same !
                    registeredShape(result,pSolid,"s",operand,counter++,current,newShape);
                    actionCounter++;
                } else {
                    registeredShape(result,pSolid,"m",operand,counter++,current,newShape);
                    actionCounter++;
                }
                //xx Builder.Generated (root,newShape );
                //xx registerGeneratedShape(pSolid,shape,current,newShape);
            }
        }
        if (actionCounter == 0 ) {
            // the entity is not deleted, not modified , not generated ...
            // it must be unmodified in resulting shape
            registeredShape(result,pSolid,"i",operand,-1,current,const_cast<TopoDS_Shape&>(current));
        }
    }
}
static Handle<v8::Value>  ShapeFactory_createBoolean(Solid* pSolid1, Solid* pSolid2, BOP_Operation op)
{
    HandleScope scope;

    const TopoDS_Shape& firstObject= pSolid1->shape();
    const TopoDS_Shape& secondObject= pSolid2->shape();

    std::auto_ptr<BRepAlgoAPI_BooleanOperation> pTool;




    TopoDS_Shape shape;
    try {
        switch (op) {
        case BOP_FUSE:
            pTool = std::auto_ptr<BRepAlgoAPI_BooleanOperation>(new BRepAlgoAPI_Fuse(firstObject, secondObject));
            break;
        case BOP_CUT:
            pTool = std::auto_ptr<BRepAlgoAPI_BooleanOperation>(new BRepAlgoAPI_Cut(firstObject, secondObject));
            break;
        case BOP_COMMON:
            pTool = std::auto_ptr<BRepAlgoAPI_BooleanOperation>(new BRepAlgoAPI_Common(firstObject, secondObject));
            break;
        default:
            Standard_ConstructionError::Raise("unknown operation");
            break;
        }
        if (!pTool->IsDone()) {
            Standard_ConstructionError::Raise("operation failed");
        }
        shape = pTool->Shape();

        Handle<Value> result(Solid::NewInstance(shape));

        Solid* pResult = node::ObjectWrap::Unwrap<Solid>(result->ToObject());

        registerShapes(pTool.get(),pResult,pSolid1,1);
        registerShapes(pTool.get(),pResult,pSolid2,2);

        if (pTool->HasDeleted())  {
            // the boolean operation causes some shape from s1 or s2 to be deleted
        }
        if (pTool->HasGenerated()) {
            // the boolean operation causes some shape from s1 or s2 to be created

        }
        if (pTool->HasModified()) {
            // the boolean operation causes some shape from s1 or s2 to be created
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
                pResult->setShape(shapeMap(1));
            }
        }
        return scope.Close(result);

    }
    CATCH_AND_RETHROW("Failed in boolean operation");
    return scope.Close(Undefined());
}


Handle<v8::Value> ShapeFactory::add(const std::vector<Base*>& shapes)
{
    TopoDS_Compound compound;
    BRep_Builder builder;

	Handle<v8::Value> pJhis(Solid::NewInstance());
    Solid* pThis = node::ObjectWrap::Unwrap<Solid>(pJhis->ToObject());
    
	try {

        builder.MakeCompound(compound);
        
		for (size_t i = 0; i < shapes.size(); i++) {

		   const TopoDS_Shape&  shape = shapes[i]->shape();
           builder.Add(compound, shape);
        }

		pThis->setShape(compound);

		std::cout << " num solid " << pThis->numSolids() << std::endl;

    }
    CATCH_AND_RETHROW("Failed in compound operation");

	return pJhis;

}
Handle<v8::Value> ShapeFactory::compound(const v8::Arguments& args)
{
    HandleScope scope;
    std::vector<Base*> shapes;
    for (int i=0; i<args.Length(); i++) {
		Handle<Object> obj = args[i]->ToObject();
        if (Solid::constructor->HasInstance(obj)) {
            Base* pShape = node::ObjectWrap::Unwrap<Solid>(obj);
            shapes.push_back(pShape);
        } else if (args[i]->IsArray()) {
			Handle<Array> arr = Handle<Array>::Cast(args[i]);
			int length = arr->Length();
			for(int j=0;j<length;j++) {
				 Handle<Object> obj1 = arr->Get(j)->ToObject();
				 if (Solid::constructor->HasInstance(obj1)) {
					Base* pShape = node::ObjectWrap::Unwrap<Solid>(obj1);
					shapes.push_back(pShape);
				 }
			}
		}
    }
    return scope.Close(add(shapes));
}
Handle<v8::Value> ShapeFactory::_boolean(const v8::Arguments& args,BOP_Operation op)
{
    HandleScope scope;

    if (!Solid::constructor->HasInstance(args[0]) || !Solid::constructor->HasInstance(args[1])) {
        ThrowException(Exception::TypeError(String::New("Wrong arguments for boolean operation : expecting two solids")));
        return scope.Close(Undefined());
    }

    Solid* pSolid1 = node::ObjectWrap::Unwrap<Solid>(args[0]->ToObject());

    Solid* pSolid2 = node::ObjectWrap::Unwrap<Solid>(args[1]->ToObject());
    /*
        std::vector<Solid*> other_solids;
        for (int i=1; i<args.Length(); i++) {
            if(Solid::constructor->HasInstance(args[i])) {
                Solid* pSolid2 = node::ObjectWrap::Unwrap<Solid>(args[i]->ToObject());
                other_solids.push_back(pSolid2);
            }
        }
        Handle<Object> compound = ShapeFactory::add(other_solids);
        Solid* pSolid2 = node::ObjectWrap::Unwrap<Solid>(compound);
    */
    return scope.Close(ShapeFactory_createBoolean(pSolid1,pSolid2,op));

}
Handle<v8::Value> ShapeFactory::fuse(const v8::Arguments& args)
{
    return _boolean(args,BOP_FUSE);
}
Handle<v8::Value> ShapeFactory::cut(const v8::Arguments& args)
{
    return _boolean(args,BOP_CUT);
}
Handle<v8::Value> ShapeFactory::common(const v8::Arguments& args)
{
    return _boolean(args,BOP_COMMON);
}



