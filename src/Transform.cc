#include <json_spirit/json_spirit.h>
#include "OCC.h"

#include "Transform.h"
#include "Util.h"

using namespace std;
using namespace json_spirit;


Transform::Transform(TopoDS_Shape shape) {
    shape_ = shape;
}
    
TopoDS_Shape Rotate::apply(double multiplier, 
                             map< string, mValue > origin, 
                            map< string, mValue > parameters) {
    
    double x = Util::to_d(origin["x"]);
    double y = Util::to_d(origin["y"]);
    double z = Util::to_d(origin["z"]);
    double u = Util::to_d(parameters["u"]);
    double v = Util::to_d(parameters["v"]);
    double w = Util::to_d(parameters["w"]);
    double angle = Util::to_d(parameters["angle"]);
    while (angle > 360) {
        angle -= 360;
    }
    while (angle < 0) {
        angle += 360;
    }
    
    gp_Trsf transformation = gp_Trsf();
    transformation.SetRotation(gp_Ax1(gp_Pnt(x,y,z), gp_Dir(u,v,w)),
                               multiplier*Util::to_d(angle)/180*M_PI);
    
    return BRepBuilderAPI_Transform(shape_, transformation).Shape();
}

TopoDS_Shape Scale::apply(double multiplier, 
                          map< string, mValue > origin, 
                          map< string, mValue > parameters) {
    
    double x = Util::to_d(origin["x"]);
    double y = Util::to_d(origin["y"]);
    double z = Util::to_d(origin["z"]);
    double factor = Util::to_d(parameters["factor"]);
    
    gp_Trsf transformation = gp_Trsf();
    transformation.SetScale(gp_Pnt(x, y, z), factor);
    
    return BRepBuilderAPI_Transform(shape_, transformation).Shape();

}

TopoDS_Shape AxisMirror::apply(double multiplier, 
                               map< string, mValue > origin, 
                               map< string, mValue > parameters) {
    
    double x = Util::to_d(origin["x"]);
    double y = Util::to_d(origin["y"]);
    double z = Util::to_d(origin["z"]);
    double u = Util::to_d(parameters["u"]);
    double v = Util::to_d(parameters["v"]);
    double w = Util::to_d(parameters["w"]);
    
    gp_Trsf transformation = gp_Trsf();
    transformation.SetMirror(gp_Ax1(gp_Pnt(x, y, z), gp_Dir(u, v, w)));
    
    return BRepBuilderAPI_Transform(shape_, transformation).Shape();
    
}

TopoDS_Shape PlaneMirror::apply(double multiplier, 
                                map< string, mValue > origin, 
                                map< string, mValue > parameters) {
    
    double x = Util::to_d(origin["x"]);
    double y = Util::to_d(origin["y"]);
    double z = Util::to_d(origin["z"]);
    double u = Util::to_d(parameters["u"]);
    double v = Util::to_d(parameters["v"]);
    double w = Util::to_d(parameters["w"]);
    
    gp_Trsf transformation = gp_Trsf();
    transformation.SetMirror(gp_Ax2(gp_Pnt(x, y, z), gp_Dir(u, v, w)));
    
    return BRepBuilderAPI_Transform(shape_, transformation).Shape();
    
}

TopoDS_Shape Translate::apply(double multiplier, 
                              map< string, mValue > origin, 
                              map< string, mValue > parameters) {
    
    double u = Util::to_d(parameters["u"]);
    double v = Util::to_d(parameters["v"]);
    double w = Util::to_d(parameters["w"]);
    
    gp_Trsf transformation = gp_Trsf();
    transformation.SetTranslation(gp_Vec(multiplier*u, 
                                         multiplier*v, 
                                         multiplier*w));
    
    return BRepBuilderAPI_Transform(shape_, transformation).Shape();

}

