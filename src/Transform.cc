//#include "NodeV8.h"
//
//#include "Transform.h"
//#include "Util.h"
//
//using namespace std;
//
//
////
//Transform::Transform(TopoDS_Shape shape)
//{
//    shape_ = shape;
//}
//
//
//TopoDS_Shape RotateTransform::apply(
//    double multiplier,
//    Handle<Object> origin,
//    Handle<Object> parameters)
//{
//    double x=0,y=0,z=0;
//    ReadXYZ(origin,&x,&y,&z);
//
//    double u=0,v=0,w=0;
//    ReadUVW(parameters,&u,&v,&w);
//
//    double angle = ReadDouble(parameters,"angle",0.0);
//
//    while (angle > 360) {
//        angle -= 360;
//    }
//    while (angle < 0) {
//        angle += 360;
//    }
//
//    gp_Trsf transformation = gp_Trsf();
//
//    transformation.SetRotation(gp_Ax1(gp_Pnt(x,y,z), gp_Dir(u,v,w)),
//                               multiplier*angle/180*M_PI);
//
//    return BRepBuilderAPI_Transform(shape_, transformation).Shape();
//}

//TopoDS_Shape Scale::apply(double multiplier,
//                          Handle<Object> origin,
//                          Handle<Object> parameters) {
//
//    double x=0,y=0,z=0;
//    ReadXYZ(origin,&x,&y,&z);
//
//    double factor = ReadDouble(parameters,"factor");
//
//    gp_Trsf transformation = gp_Trsf();
//    transformation.SetScale(gp_Pnt(x, y, z), factor);
//
//    return BRepBuilderAPI_Transform(shape_, transformation).Shape();
//
//}

//TopoDS_Shape AxisMirror::apply(double multiplier,
//                               Handle<Object> origin,
//                               Handle<Object> parameters) {
//
//    double x=0,y=0,z=0;
//    ReadXYZ(origin,&x,&y,&z);
//
//    double u=0,v=0,w=0;
//    ReadUVW(parameters,&u,&v,&w);
//
//    gp_Trsf transformation = gp_Trsf();
//    transformation.SetMirror(gp_Ax1(gp_Pnt(x, y, z), gp_Dir(u, v, w)));
//
//    return BRepBuilderAPI_Transform(shape_, transformation).Shape();
//
//}
//TopoDS_Shape PlaneMirror::apply(double multiplier,
//                                Handle<Object> origin,
//                                Handle<Object> parameters) {
//
//    double x=0,y=0,z=0;
//    ReadXYZ(origin,&x,&y,&z);
//
//    double u=0,v=0,w=0;
//    ReadUVW(parameters,&u,&v,&w);
//
//    gp_Trsf transformation = gp_Trsf();
//    transformation.SetMirror(gp_Ax2(gp_Pnt(x, y, z), gp_Dir(u, v, w)));
//
//    return BRepBuilderAPI_Transform(shape_, transformation).Shape();
//
//}

//TopoDS_Shape TranslateTransform::apply(double multiplier,
//                              Handle<Object> origin,
//                              Handle<Object> parameters) {
//
//    double u=0,v=0,w=0;
//    ReadUVW(parameters,&u,&v,&w);
//
//    gp_Trsf transformation = gp_Trsf();
//    transformation.SetTranslation(gp_Vec(multiplier*u,
//                                         multiplier*v,
//                                         multiplier*w));
//
//    return BRepBuilderAPI_Transform(shape_, transformation).Shape();
//
//}
//
