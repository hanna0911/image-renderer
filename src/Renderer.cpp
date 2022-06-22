#include "Renderer.hpp"
#include "scene_parser.hpp"
#include "image.hpp"
#include "camera.hpp"
#include "group.hpp"
#include "light.hpp"
#include "camera_controller.hpp"
#include <iostream>

#include <limits>

// KDTree *root = NULL;

RayCasting::RayCasting(char* &input_file, char* &output_file) {
    _input = input_file;
    _output = output_file;
}

void
RayCasting::Render() {
    /*
    int w = _args.width;
    int h = _args.height;


    Image image(w, h);
    Image nimage(w, h);
    Image dimage(w, h);
    */

    // loop through all the pixels in the image
    // generate all the samples

    // This look generates camera rays and callse traceRay.
    // It also write to the color, normal, and depth images.
    // You should understand what this code does.

    // TODO: Main RayCasting Logic
    // First, parse the scene using SceneParser.
    // Then loop over each pixel in the image, shooting a ray
    // through that pixel and finding its intersection with
    // the scene.  Write the color at the intersection to that
    // pixel in your output image.
    SceneParser sceneParser(_input);
    Camera* camera = sceneParser.getCamera();
    Image renderedImg(camera->getWidth(), camera->getHeight());

    // 循环屏幕空间的像素
    for (int x = 0; x < camera->getWidth(); ++x) {
        for (int y = 0; y < camera->getHeight(); ++y) {
            // 计算当前像素(x,y)处相机出射光线camRay
            Ray camRay = camera->generateRay(Vector2f(x, y)); 
            Group* baseGroup = sceneParser.getGroup();
            Hit hit;
            // 判断camRay是否和场景有交点，并返回最近交点的数据，存储在hit中
            bool isIntersect = baseGroup->intersect(camRay, hit, 0);
            if (isIntersect) {
                Vector3f finalColor = Vector3f::ZERO;
                // 找到交点之后，累加来自所有光源的光强影响
                for (int li = 0; li < sceneParser.getNumLights(); ++li) {
                    Light* light = sceneParser.getLight(li);
                    Vector3f L, lightColor;
                    // 获得光照强度
                    light->getIllumination(camRay.pointAtParameter(hit.getT()), L, lightColor); // 计算局部光强
                    finalColor += hit.getMaterial()->Shade(camRay, hit, L, lightColor);
                }
                renderedImg.SetPixel(x, y, finalColor);
            } else {
            // 不存在交点，返回背景色
                renderedImg.SetPixel(x, y, sceneParser.getBackgroundColor());
            }
        }
    }
    renderedImg.SaveImage(_output);
    

    /*
    for (int y = 0; y < h; ++y) {
        float ndcy = 2 * (y / (h - 1.0f)) - 1.0f;
        for (int x = 0; x < w; ++x) {
            float ndcx = 2 * (x / (w - 1.0f)) - 1.0f;
            // Use PerspectiveCamera to generate a ray.
            // You should understand what generateRay() does.
            Ray r = cam->generateRay(Vector2f(ndcx, ndcy));

            Hit h;
            Vector3f color = traceRay(r, cam->getTMin(), _args.bounces, h);

            image.SetPixel(x, y, color);
            nimage.SetPixel(x, y, (h.getNormal() + 1.0f) / 2.0f);
            float range = (_args.depth_max - _args.depth_min);
            if (range) {
                dimage.SetPixel(x, y, Vector3f((h.getT() - _args.depth_min) / range));
            }
        }
    }
    // END SOLN

    // save the files
    if (_args.output_file.size()) {
        image.SaveImage(_args.output_file);
    }
    if (_args.depth_file.size()) {
        dimage.SaveImage(_args.depth_file);
    }
    if (_args.normals_file.size()) {
        nimage.SaveImage(_args.normals_file);
    }
    */
}

/*
Vector3f
Renderer::traceRay(const Ray &r,
                   float tmin,
                   int bounces,
                   Hit &h) const {
    // The starter code only implements basic drawing of sphere primitives.
    // You will implement phong shading, recursive ray tracing, and shadow rays.
    if (_scene.getGroup()->intersect(r, tmin, h)) {
        Vector3f I = _scene.getAmbientLight() * h.getMaterial()->getDiffuseColor();
        Vector3f p = r.pointAtParameter(h.getT());
        for (int i = 0; i < _scene.getNumLights(); ++i) {
            Vector3f tolight;
            Vector3f intensity;
            float distToLight;
            _scene.getLight(i)->getIllumination(p, tolight, intensity, distToLight);
            Vector3f ILight = h.getMaterial()->Shade(r, h, tolight, intensity);
            // To compute cast shadows, you will send rays from the surface point to each
            // light source. If an intersection is reported, and the intersection is closer
            // than the distance to the light source, the current surface point is in shadow
            // and direct illumination from that light source is ignored. Note that shadow
            // rays must be sent to all light sources.
            if (_args.shadows) {
                Vector3f shadowRayOrigin = p + 0.05 * tolight;
                Ray shadowRay(shadowRayOrigin, tolight);
                Hit shadowHit = Hit();
                Vector3f shadowTrace = traceRay(shadowRay, 0, 0, shadowHit);
                bool shadowIntersectedSomething = shadowHit.getT() < std::numeric_limits<float>::max();
                float distToIntersection = (shadowRay.pointAtParameter(shadowHit.getT()) - shadowRayOrigin).abs();
                if (
                    shadowIntersectedSomething && distToIntersection < distToLight
                ) {
                    ILight = Vector3f(0); // Object in shadow from this light, discount light.
                }
            }
            I += ILight;
        }
        // Reflections.
        if (bounces > 0) {
            // Recursive call.
            Vector3f V = r.getDirection();
            Vector3f N = h.getNormal().normalized();
            Vector3f R = (V - (2 * Vector3f::dot(V, N) * N)).normalized();
            Hit hPrime = Hit();
            // Add a little epsilon to avoid noise.
            Ray rPrime(p + 0.01 * R, R);
            Vector3f IIndirect = traceRay(rPrime, 0.0f, bounces - 1, hPrime);
            I += (h.getMaterial()->getSpecularColor() * IIndirect);
        }
        return I;
    } else {
        return _scene.getBackgroundColor(r.getDirection());
    };
}
*/

