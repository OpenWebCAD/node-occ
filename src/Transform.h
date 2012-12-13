#ifndef SS_TRANSFORM
#define SS_TRANSFORM

#include "json_spirit/json_spirit.h"
#include "OCC.h"

using namespace std;
using namespace json_spirit;

template <class T>
class Transformer {
    
public:
    Transformer(TopoDS_Shape shape, 
                map< string, mValue > origin,
                map< string, mValue > parameters) {
        transformed_shape_ = apply<T>(shape, origin, parameters);
    }
    
    TopoDS_Shape transformed_shape() { 
        return transformed_shape_;
    }

    
private: 
    TopoDS_Shape transformed_shape_;
    
    template<typename U>
    static TopoDS_Shape apply(TopoDS_Shape shape, 
                              map< string, mValue > origin,
                              map< string, mValue > parameters) {
        int n = parameters["n"].is_null() ? 0 : parameters["n"].get_int();
        
        if(n == 0) {
            auto_ptr<U> transform(new T(shape));
            return transform->apply(1.0, origin, parameters);
        }
        
        vector<TopoDS_Shape> copies(n+1); 
        copies[0] = shape;
        
        int remaining = n;
        int grouping = 1;
        float multiplier = 1.0;
        int index = 1;
        
        while (remaining > 0) {
            
            int group_index = (int)(log((double)grouping)/log(2.0));
            TopoDS_Shape shape_to_copy = copies[group_index];
            
            auto_ptr<T> transform(new T(shape_to_copy));
            TopoDS_Shape transformedShape = transform->apply(multiplier, origin, parameters);
            
            copies[index] = BRepAlgoAPI_Fuse(transformedShape, copies[index - 1]).Shape();
            
            multiplier = multiplier + grouping;
            remaining = remaining - grouping;
            ++index;
            
            if ((grouping * 2) < remaining) {
                grouping = grouping * 2;
            } else if (grouping > remaining) {
                grouping = grouping / 2;
            }
        }
        
        return copies[index - 1];
    }
    
    
};

class Transform {
protected:
    TopoDS_Shape shape_;
public:
    Transform(TopoDS_Shape shape);
    virtual ~Transform() {};

    virtual TopoDS_Shape apply(double multiplier, 
                                 map< string, mValue > origin, 
                                 map< string, mValue > parameters) = 0;
};

class Rotate : public Transform {
    
public:
    Rotate(TopoDS_Shape shape) : Transform(shape) {}
    virtual ~Rotate() {};
    
    virtual TopoDS_Shape apply(double multiplier, 
                                 map< string, mValue > origin, 
                                 map< string, mValue > parameters);
};


class Scale : public Transform {
    
public:
    Scale(TopoDS_Shape shape) : Transform(shape) {}
    virtual ~Scale() {};
    
    virtual TopoDS_Shape apply(double multiplier, 
                                 map< string, mValue > origin, 
                                 map< string, mValue > parameters);
};

class AxisMirror : public Transform {
    
public:
    AxisMirror(TopoDS_Shape shape) : Transform(shape) {}
    virtual ~AxisMirror() {};
    
    virtual TopoDS_Shape apply(double multiplier, 
                                 map< string, mValue > origin, 
                                 map< string, mValue > parameters);
};

class PlaneMirror : public Transform {
    
public:
    PlaneMirror(TopoDS_Shape shape) : Transform(shape) {}
    virtual ~PlaneMirror() {};
    
    virtual TopoDS_Shape apply(double multiplier, 
                               map< string, mValue > origin, 
                               map< string, mValue > parameters);
};

class Translate : public Transform {
    
public:
    Translate(TopoDS_Shape shape) : Transform(shape) {}
    virtual ~Translate() {};
    
    virtual TopoDS_Shape apply(double multiplier, 
                                 map< string, mValue > origin, 
                                 map< string, mValue > parameters);
};

#endif

