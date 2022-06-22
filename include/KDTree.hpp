#ifndef KDTREE_H
#define KDTREE_H

#include <iostream>
#include <vector>
#include <Vector3f.h>
#include <limits>
#include <algorithm>
#include <experimental/algorithm>
#include <random>
#include "object3d.hpp"
#include "BoundingBox.hpp"

// FINAL PROJECT
class KDTree {
public:

    // CONSTRUCTOR

    KDTree() {};

    // ATTRIBUTES

    KDTree *left, *right; // children
    int splitDimension = 0; // either X, Y, or Z axis
    float splitPosition; // from origin along split axis
    bool isLeaf = false;
    std::vector<Object3D *> triangles; // only leaves have lists of triangles
    BoundingBox box; // box partition for this node

    // FUNCTIONS

    bool traverse(const Ray &r, float tmin, Hit &h);
    bool traverse(const Ray &r, float tmin, Hit &h, float tstart, float tend);

    void sortTriangles(std::vector<Object3D *> &triangles,
                       int splitDimension, float splitPosition,
                       std::vector<Object3D *> &trianglesLeft,
                       std::vector<Object3D *> &trianglesRight);

    void splitBox(const BoundingBox &box, int splitDimension, float splitPosition,
                  BoundingBox &boxLeft, BoundingBox &boxRight);

    KDTree *buildTree(std::vector<Object3D *> triangles,
                      const BoundingBox &box,
                      int splitDimension
                      );

};

bool PRINT_DEBUG = true;

bool KDTree::traverse(const Ray &r, float tmin, Hit &h)
{
    vector<float> pathTimes = box.intersect(r);
    if (pathTimes.empty())
    {
        return false;
    }
    float tstart = pathTimes[0];
    float tend = pathTimes[1];
    return traverse(r, tmin, h, tstart, tend);
}

bool KDTree::traverse(const Ray &r, float tmin, Hit &h, float tstart, float tend)
{
    assert(tstart <= tend);
    if (isLeaf)
    {
        bool result = false;
        for (Object3D *triangle : triangles)
        {
            result |= triangle->intersect(r, h, tmin);
        }
        return result;
    }
    else
    {
        // Find the intersection with the split axis
        Vector3f orig = r.getOrigin();
        Vector3f dir = r.getDirection();
        // P = dt + O -> t = (P - O) / d
        float t = (splitPosition - orig[splitDimension]) / (dir[splitDimension]);

        KDTree *front, *back;
        front = left;
        back = right;
        int belowFirst = (orig[splitDimension] < splitPosition) ||
                         (orig[splitDimension] == splitPosition && dir[splitDimension] <= 0);
        if (not belowFirst)
            swap(front, back);

        // 3 cases to check for
        if (t <= tstart)
        {
            return back->traverse(r, tmin, h, tstart, tend);
        }
        else if (t >= tend or t < 0)
        {
            return front->traverse(r, tmin, h, tstart, tend);
        }
        else
        {
            if (front->traverse(r, tmin, h, tstart, t) and h.getT() < t)
                /*
                If front region already contains something,
                terminate the search.
                */
                return true;
            return back->traverse(r, tmin, h, t, tend);
        }
    }
}

void KDTree::splitBox(const BoundingBox &box, int splitDimension, float splitPosition,
                      BoundingBox &boxLeft, BoundingBox &boxRight)
{
    boxLeft = BoundingBox(box.minBounds(), box.maxBounds());
    boxRight = BoundingBox(box.minBounds(), box.maxBounds());
    boxLeft.max[splitDimension] = splitPosition;
    boxRight.min[splitDimension] = splitPosition;
    assert(boxLeft.max[splitDimension] <= boxRight.min[splitDimension]);
}

void KDTree::sortTriangles(vector<Object3D *> &triangles,
                           int splitDimension, float splitPosition,
                           vector<Object3D *> &trianglesLeft,
                           vector<Object3D *> &trianglesRight)
{
    for (Object3D *t : triangles)
    {
        if (t->box.min[splitDimension] <= splitPosition)
        {
            trianglesLeft.push_back(t);
        }
        if (t->box.max[splitDimension] >= splitPosition)
        {
            trianglesRight.push_back(t);
        }
    }
}

KDTree *KDTree::buildTree(std::vector<Object3D *> triangles,
                          const BoundingBox &box,
                          int splitDimension)
{
    if (!PRINT_DEBUG)
        cout.rdbuf(nullptr);

    // Base case.
    if (triangles.size() <= 21)
    {
        // cout << "Leaf node, " << triangles.size() << endl;
        KDTree *leaf = new KDTree();
        leaf->isLeaf = true;
        leaf->box = box;
        leaf->triangles = triangles;
        return leaf;
    }

    // Recursive case: build subtrees.

    assert(box.min[splitDimension] < box.max[splitDimension]); // segfault if not true!

    // SPLITTING ALGORITHM: MIDPOINT
    // Naively split at half of the current distance.
    float splitPosition = box.min[splitDimension] + (box.d(splitDimension) / 2.f);
    std::vector<Object3D *> trianglesLeft, trianglesRight;
    sortTriangles(triangles,
                  splitDimension,
                  splitPosition,
                  trianglesLeft,
                  trianglesRight);
    BoundingBox boxLeft, boxRight;
    splitBox(box,
             splitDimension,
             splitPosition,
             boxLeft,
             boxRight);

    // Cycle through dimensions.
    int nextDimension = 0;
    if (box.dy() > box.dx())
        nextDimension = 1;
    if (nextDimension == 0 and box.dz() > box.dx())
        nextDimension = 2;
    if (nextDimension == 1 and box.dz() > box.dy())
        nextDimension = 2;
    KDTree *root = new KDTree();
    root->splitDimension = splitDimension;
    root->splitPosition = splitPosition;
    root->box = box;
    root->left = buildTree(trianglesLeft, boxLeft, nextDimension);
    root->right = buildTree(trianglesRight, boxRight, nextDimension);
    return root;
}

#endif // KDTREE_H
