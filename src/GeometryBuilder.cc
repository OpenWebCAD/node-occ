#pragma once

#include <v8.h>
#include <map>
#include <vector>

#include "OCC.h"
#include "json_spirit/json_spirit.h"

using namespace std;
using namespace json_spirit;

class Builder {
public:
    Builder() {};
    virtual ~Builder() {};
    
protected:
    TopoDS_Shape shape_;
    
    void ApplyOrigin(map< string, mValue > json);
    void ApplyTransform(map< string, mValue > json);
    void ApplyTransforms(map< string, mValue > json);
    void ApplyWorkplane(map< string, mValue > json);
    void ApplyReverseWorkplane(map< string, mValue > json);
    virtual void PostProcess(map< string, mValue > json) = 0;
   
public:
    TopoDS_Shape shape();
    
};

class Builder3D : public Builder {
protected:
    void Mesh();
    void PostProcess(map< string, mValue > json);
    virtual ~Builder3D() {};
public:
    Builder3D() {};
    
};

class CuboidBuilder : public Builder3D {

public:
    CuboidBuilder(map< string, mValue > json);
    virtual ~CuboidBuilder() {};
};



#include "Util.h"
#include "Transform.h"


void Builder::ApplyOrigin(map< string, mValue > json) 
{
    if (!json["origin"].is_null()) {
        map< string, mValue > origin = json["origin"].get_obj();
        double x = Util::to_d(origin["x"]);
        double y = Util::to_d(origin["y"]);
        double z = Util::to_d(origin["z"]);
        gp_Trsf transformation = gp_Trsf();
        transformation.SetTranslation(gp_Vec(x,y,z));
        
        shape_ = BRepBuilderAPI_Transform(shape_, transformation).Shape();
    }
}

void Builder::ApplyTransform(map< string, mValue > transformJson)
 {
	
    string transformType = transformJson["type"].get_str();
    map< string, mValue > origin = transformJson["origin"].get_obj();
    map< string, mValue > parameters = transformJson["parameters"].get_obj();
    
    if (transformType == "rotate") {
        auto_ptr< Transformer<Rotate> > transformer(new Transformer<Rotate>(shape_, origin, parameters));
        shape_ = transformer->transformed_shape();
        return;
    } else if (transformType == "scale") {
        auto_ptr< Transformer<Scale> > transformer(new Transformer<Scale>(shape_, origin, parameters));
        shape_ = transformer->transformed_shape();
        return;
    } else if (transformType == "translate") {
        auto_ptr< Transformer<Translate> > transformer(new Transformer<Translate>(shape_, origin, parameters));
        shape_ = transformer->transformed_shape();
        return;
    } else if ((transformType == "mirror") || (transformType == "axis_mirror")) {
        auto_ptr< Transformer<AxisMirror> > transformer(new Transformer<AxisMirror>(shape_, origin, parameters));
        shape_ = transformer->transformed_shape();
        return;
    } else if (transformType == "plane_mirror") {
        auto_ptr< Transformer<PlaneMirror> > transformer(new Transformer<PlaneMirror>(shape_, origin, parameters));
        shape_ = transformer->transformed_shape();
        return;
    } 
    throw "transform type not found";
}

void Builder::ApplyTransforms(map< string, mValue > json) {
	
    if (!json["transforms"].is_null()) {
        mArray transforms = json["transforms"].get_array();
        for (unsigned int k = 0; k < transforms.size(); ++k) {
            mValue transformJson = transforms[k];
            this->ApplyTransform(transformJson.get_obj());
        }
    } 
}

void Builder::ApplyWorkplane(map< string, mValue> json) {
	
    if (!json["workplane"].is_null()) {
        map< string, mValue > workplane_origin = json["workplane"].get_obj()["origin"].get_obj();
        double x = Util::to_d(workplane_origin["x"]);
        double y = Util::to_d(workplane_origin["y"]);
        double z = Util::to_d(workplane_origin["z"]);

        map< string, mValue > workplane_axis = json["workplane"].get_obj()["axis"].get_obj();
        double u = Util::to_d(workplane_axis["x"]);
        double v = Util::to_d(workplane_axis["y"]);
        double w = Util::to_d(workplane_axis["z"]);

        double angle = Util::to_d(json["workplane"].get_obj()["angle"]);

        gp_Trsf transformation1 = gp_Trsf();
        transformation1.SetRotation(gp_Ax1(gp_Pnt(0.0,0.0,0.0), gp_Dir(u,v,w)), angle/180*M_PI);
        shape_ = BRepBuilderAPI_Transform(shape_, transformation1).Shape();

        gp_Trsf transformation2 = gp_Trsf();
        transformation2.SetTranslation(gp_Vec(x,y,z));
        shape_ = BRepBuilderAPI_Transform(shape_, transformation2).Shape();
    }
}

void Builder::ApplyReverseWorkplane(map< string, mValue> json) {
	
    if (!json["workplane"].is_null()) {
        map< string, mValue > workplane_origin = json["workplane"].get_obj()["origin"].get_obj();
        double x = -Util::to_d(workplane_origin["x"]);
        double y = -Util::to_d(workplane_origin["y"]);
        double z = -Util::to_d(workplane_origin["z"]);
        
        map< string, mValue > workplane_axis = json["workplane"].get_obj()["axis"].get_obj();
        double u = Util::to_d(workplane_axis["x"]);
        double v = Util::to_d(workplane_axis["y"]);
        double w = Util::to_d(workplane_axis["z"]);
        
        double angle = -Util::to_d(json["workplane"].get_obj()["angle"]);
        
        gp_Trsf transformation1 = gp_Trsf();
        transformation1.SetTranslation(gp_Vec(x,y,z));
        shape_ = BRepBuilderAPI_Transform(shape_, transformation1).Shape();
        
        gp_Trsf transformation2 = gp_Trsf();
        transformation2.SetRotation(gp_Ax1(gp_Pnt(0.0,0.0,0.0), gp_Dir(u,v,w)), angle/180*M_PI);
        shape_ = BRepBuilderAPI_Transform(shape_, transformation2).Shape();
        
    }
}




TopoDS_Shape Builder::shape() {
    return shape_;
}


void Builder3D::Mesh() {
    TopExp_Explorer Ex; 
    int numFaces = 0;
    for (Ex.Init(shape_, TopAbs_FACE); Ex.More(); Ex.Next()) { 
        ++numFaces;
    }
    
    if (numFaces > 0) {
        BRepMesh().Mesh(shape_, 1.0);
    }
}

void Builder3D::PostProcess(map< string, mValue > json) {
    this->ApplyOrigin(json);
    this->ApplyTransforms(json);
    this->ApplyWorkplane(json);
    this->Mesh();
}


CuboidBuilder::CuboidBuilder(map< string, mValue > json) {
    map< string, mValue > parameters = json["parameters"].get_obj();
    double width = Util::to_d(parameters["u"]);
    double depth = Util::to_d(parameters["v"]);
    double height = Util::to_d(parameters["w"]);
         
    map< string, mValue > origin = json["origin"].get_obj();
         
    if (width < 0) {
        origin["x"] = Util::to_d(origin["x"]) + width;
        width = -width;
    }

    if (depth < 0) {
        origin["y"] = Util::to_d(origin["y"]) + depth;
        depth = -depth;
    }
         
    if (height < 0) {
        origin["z"] = Util::to_d(origin["z"]) + height;
        height = -height;
    }
    json["origin"] = origin;
         
    shape_ = BRepPrimAPI_MakeBox(width, depth, height).Shape();
    PostProcess(json);
}
