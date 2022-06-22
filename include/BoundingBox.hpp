#ifndef BOUNDINGBOX_H
#define BOUNDINGBOX_H

#include "ray.hpp"
#include <iostream>
#include <vector>

using namespace std;

class BoundingBox
{

public:
    Vector3f min;
    Vector3f max;

    BoundingBox(const Vector3f min, const Vector3f max) : min(min), max(max){};
    BoundingBox(){};

    Vector3f minBounds() const
    {
        return min;
    }
    Vector3f maxBounds() const
    {
        return max;
    }

    Vector3f bounds(int type) const
    {
        if (type == 0)
            return min;
        return max;
    }

    // dx/y/z functions are all correct

    float dx() const
    {
        return abs(max.x() - min.x());
    };
    float dy() const
    {
        return abs(max.y() - min.y());
    };
    float dz() const
    {
        return abs(max.z() - min.z());
    };

    float d(int axis) const
    {
        return max[axis] - min[axis];
    }

    float isPlanar()
    {
        return dx() <= 0.01 || dy() <= 0.01 || dz() <= 0.01;
    }

    // Code taken mostly from
    // https://www.scratchapixel.com/lessons/3d-basic-rendering/
    // minimal-ray-tracer-rendering-simple-shapes/ray-box-intersection.
    vector<float> intersect(const Ray &r)
    {
        float tmin, tmax, tymin, tymax, tzmin, tzmax;
        
        Vector3f dir = r.getDirection();
        Vector3f invdir = Vector3f(1.f / dir[0], 1.f / dir[1], 1.f / dir[2]); 
        int sign[3];
        sign[0] = (invdir.x() < 0); 
        sign[1] = (invdir.y() < 0); 
        sign[2] = (invdir.z() < 0); 

        tmin = (bounds(sign[0]).x() - r.getOrigin().x()) * invdir.x();
        tmax = (bounds(1 - sign[0]).x() - r.getOrigin().x()) * invdir.x();
        tymin = (bounds(sign[1]).y() - r.getOrigin().y()) * invdir.y();
        tymax = (bounds(1 - sign[1]).y() - r.getOrigin().y()) * invdir.y();

        if ((tmin > tymax) || (tymin > tmax))
            return vector<float>{};
        if (tymin > tmin)
            tmin = tymin;
        if (tymax < tmax)
            tmax = tymax;

        tzmin = (bounds(sign[2]).z() - r.getOrigin().z()) * invdir.z();
        tzmax = (bounds(1 - sign[2]).z() - r.getOrigin().z()) * invdir.z();

        if ((tmin > tzmax) || (tzmin > tmax))
            return vector<float>{};
        if (tzmin > tmin)
            tmin = tzmin;
        if (tzmax < tmax)
            tmax = tzmax;
        return vector<float>{tmin, tmax};
    }
};

#endif //BOUNDINGBOX_H
