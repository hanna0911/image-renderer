#ifndef TRIANGLE_H
#define TRIANGLE_H

#include "object3d.hpp"
#include <vecmath.h>
#include <cmath>
#include <iostream>

using namespace std;

// TODO (PA2): Copy from PA1
class Triangle: public Object3D
{

public:
	Triangle() = delete;
        ///@param a b c are three vertex positions of the triangle

	Triangle( const Vector3f& a, const Vector3f& b, const Vector3f& c, Material* m) : Object3D(m) {
		this->vertices[0] = a; this->vertices[1] = b; this->vertices[2] = c;
		this->normal = Vector3f::cross(b - a, c - a);    
	}

	bool intersect( const Ray& ray,  Hit& hit , float tmin) override {
		Vector3f P0 = vertices[0], P1 = vertices[1], P2 = vertices[2]; // 三角形顶点
		Vector3f R0 = ray.getOrigin(); // 光线起始点
        Vector3f Rd = ray.getDirection(); Rd.normalize(); // 光线单位方向向量
		Vector3f E1 = P0 - P1, E2 = P0 - P2, S = P0 - R0;
        Matrix3f denominator(Rd, E1, E2, true);
		Matrix3f t_(S, E1, E2, true);
		Matrix3f b_(Rd, S, E2, true);
		Matrix3f r_(Rd, E1, S, true);
		float det_denominator = denominator.determinant();
		float t = t_.determinant() / det_denominator, b = b_.determinant() / det_denominator, r = r_.determinant() / det_denominator;
		t = t / ray.getDirection().length(); // Transform
		if(t > 0 && 0 <= b && b <= 1 && 0 <= r && r <= 1 && b + r <= 1 && tmin < t && t < hit.getT()){
            hit.set(t, material, normal);
			return true;
		}
		return false;
	}
	Vector3f normal;
	Vector3f vertices[3];

    void drawGL() override {
        Object3D::drawGL();
        glBegin(GL_TRIANGLES);
        glNormal3fv(normal);
        glVertex3fv(vertices[0]); glVertex3fv(vertices[1]); glVertex3fv(vertices[2]);
        glEnd();
    }

protected:
};

#endif //TRIANGLE_H
