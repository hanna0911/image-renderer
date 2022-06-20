#ifndef SPHERE_H
#define SPHERE_H

#include "object3d.hpp"
#include <vecmath.h>
#include <cmath>
#include <glut.h>

// TODO (PA2): Copy from PA1

class Sphere : public Object3D {
public:
    Sphere() {
        // unit ball at the center
        this->center = Vector3f(0, 0, 0);
        this->radius = 0;
    }

    Sphere(const Vector3f &center, float radius, Material *material) : Object3D(material) {
        //
        this->center = center;
        this->radius = radius;
    }

    ~Sphere() override = default;

    bool intersect(const Ray &r, Hit &h, float tmin) override {
        Vector3f R0 = r.getOrigin(); // 光线起始点
        Vector3f Rd = r.getDirection(); Rd.normalize(); // 光线单位方向向量
        Vector3f l = center - R0; // 由光源指向球心的向量
        if(l.squaredLength() == radius * radius){
            if(tmin <= 0 && 0 <= h.getT()) h.set(0, material, (R0 - center).normalized());
            return true; // 光源位于球面上，算作相交（光源即相交点）
        }
        
        float tp = l.dot(l, Rd); // 球心到光线所在直线的投影点 (垂足)
        if(l.squaredLength() > radius * radius && tp < 0) return false; // 如果光源在球体外部并且 tp < 0，那么光线与球面不相交

        float d = sqrt(l.squaredLength() - tp * tp); // 球心到光线所在直线的距离
        if(d > radius) return false; // 如果 d > r，那么光线与球面不相交
        
        float t_ = sqrt(radius * radius - d * d); // 投影点到光线与球面的交点的距离
        float t;
        Vector3f N; // 相交处法向（指向球外）
        if(l.squaredLength() > radius * radius){ // 光源在球体外部
            t = tp - t_;
            N = R0 + t * Rd - center;
        }
        else{ // 光源在球体内部
            t = tp + t_;
            N = R0 + t * Rd - center;
        }
        t = t / r.getDirection().length(); // Transform
        if(tmin < t && t < h.getT()){
            h.set(t, material, N);
            return true; // 不管光源在球体内部还是外部，都必定有交点
        }
        else return false;
    }

    void drawGL() override {
        Object3D::drawGL();
        glMatrixMode(GL_MODELVIEW); glPushMatrix();
        glTranslatef(center.x(), center.y(), center.z());
        glutSolidSphere(radius, 80, 80);
        glPopMatrix();
    }

protected:
    Vector3f center;
    float radius;

};


#endif
