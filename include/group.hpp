#ifndef GROUP_H
#define GROUP_H


#include "object3d.hpp"
#include "ray.hpp"
#include "hit.hpp"
#include <iostream>
#include <vector>


// TODO (PA2): Implement Group - copy from PA1
class Group : public Object3D {

public:

    Group() {
        this->capacity = 0;
    }

    explicit Group (int num_objects) {
        this->capacity = num_objects;
    }

    ~Group() override {

    }

    bool intersect(const Ray &r, Hit &h, float tmin) override {
        bool True = false;
        for(int i = 0; i < capacity; i++){
            if(data[i]->intersect(r, h, tmin)) True = true;
        }
        if(True) return true;
        return false;
    }

    void drawGL() override {
        // drawGL for each obj
        for(int i = 0; i < capacity; i++){
            data[i]->drawGL();
        }
    }

    void addObject(int index, Object3D *obj) {
        data.push_back(obj);
    }

    int getGroupSize() {
        return capacity;
    }

private:
    std::vector<Object3D*> data;
    int capacity;
};

#endif
	
