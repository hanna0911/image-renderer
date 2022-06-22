#ifndef MATERIAL_H
#define MATERIAL_H

#include <cassert>
#include <vecmath.h>

#include "ray.hpp"
#include "hit.hpp"
#include <iostream>
#include <glut.h>
#include "texture.hpp"

// TODO (PA2): Copy from PA1.
class Material {
public:

    explicit Material(const Vector3f &d_color, const Vector3f &s_color = Vector3f::ZERO, float s = 0,
                      const Vector3f &e_color = Vector3f::ZERO, float r = 1,
                      Vector3f t = Vector3f(1, 0, 0),
                      const char *textureFile = "", const char *bumpFile = "") 
          : diffuseColor(d_color), specularColor(s_color), shininess(s),
            emission(e_color),
            refr(r),
            type(t),
            texture(textureFile),
            bump(bumpFile) {}

    virtual ~Material() = default;

    virtual Vector3f getDiffuseColor() const {
        return diffuseColor;
    }


    Vector3f Shade(const Ray &ray, const Hit &hit,
                   const Vector3f &dirToLight, const Vector3f &lightColor) {
        Vector3f shaded = Vector3f::ZERO;
        // 
        Vector3f N = hit.getNormal(); N.normalize(); // bug solved: 单位化!
        Vector3f L = dirToLight; L.normalize();
        Vector3f V = -ray.getDirection(); V.normalize();
        Vector3f R = 2 * N.dot(N, L) * N - L; R.normalize();
        float diffuse = L.dot(L, N), specular = V.dot(V, R);
        if(diffuse < 0) diffuse = 0;
        if(specular < 0) specular = 0;
        else specular = pow(specular, shininess);
        shaded = lightColor * (diffuseColor * diffuse + specularColor * specular);
        return shaded;
    }

    // For OpenGL, this is fully implemented
    void Use() {
        glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, Vector4f(diffuseColor, 1.0f));
        glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, Vector4f(specularColor, 1.0f));
        glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, Vector2f(shininess * 4.0, 1.0f));
    }

protected:
    Vector3f diffuseColor;
    Vector3f specularColor;
    float shininess;
    Vector3f color;
    Vector3f emission;
    float refr;
    Vector3f type;
    Texture texture, bump;

    float relu(float x){ 
        return std::max((float)0, x); 
    }
};


#endif // MATERIAL_H
