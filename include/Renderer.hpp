#ifndef RENDERER_H
#define RENDERER_H

#include <string>
#include <iostream>
#include <limits>

class Hit;
class Vector3f;
class Ray;

class RayCasting
{
  public:
    // Instantiates a renderer for the given scene.
    RayCasting(char* &input_file, char* &output_file);
    void Render();
  private:
    // Vector3f traceRay(const Ray &ray, float tmin, int bounces, Hit &hit) const;
    // ArgParser _args;
    char* _output;
    char* _input;
};

#endif // RENDERER_H
