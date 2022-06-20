#ifndef CURVE_HPP
#define CURVE_HPP

#include "object3d.hpp"
#include <vecmath.h>
#include <vector>
#include <utility>
#include <math.h>

#include <algorithm>

// TODO (PA2): Implement Bernstein class to compute spline basis function.
//       You may refer to the python-script for implementation.

// The CurvePoint object stores information about a point on a curve
// after it has been tesselated: the vertex (V) and the tangent (T)
// It is the responsiblility of functions that create these objects to fill in all the data.
struct CurvePoint {
    Vector3f V; // Vertex
    Vector3f T; // Tangent  (unit)
};

class Curve : public Object3D {
protected:
    std::vector<Vector3f> controls; // 控制点数组
    int n, k; // 样条曲线的控制点个数n，样条曲线的次数k
    std::vector<double> knot, knotpad; // Knot Vector

    int get_bpos(double mu){
        // lower bound
        if(mu == knot[0]) return upper_bound(knot.begin(), knot.end(), mu) - knot.begin() - 1;
        int bpos = lower_bound(knot.begin(), knot.end(), mu) - knot.begin() - 1;
        return bpos > 0 ? bpos : 0;
    }
    void evaluate(double mu, std::vector<double>& s, std::vector<double>& ds, int &lsk){
        int bpos = get_bpos(mu);
        s[k] = 1;
        for(int p = 1; p < k + 1; p++){
            for(int ii = k - p; ii < k + 1; ii++){
                int i = ii + bpos - k;
                double w1, dw1, w2, dw2;
                if(knotpad[i + p] == knotpad[i]){
                    w1 = mu;
                    dw1 = 1;
                }
                else{
                    w1 = (mu - knotpad[i]) / (knotpad[i + p] - knotpad[i]);
                    dw1 = 1 / (knotpad[i + p] - knotpad[i]);
                }
                if(knotpad[i + p + 1] == knotpad[i + 1]){
                    w2 = 1 - mu;
                    dw2 = -1;
                }
                else{
                    w2 = (knotpad[i + p + 1] - mu) / (knotpad[i + p + 1] - knotpad[i + 1]);
                    dw2 = - 1 / (knotpad[i + p + 1] - knotpad[i + 1]);
                }
                if(p == k) ds[ii] = (dw1 * s[ii] + dw2 * s[ii + 1]) * p;
                s[ii] = w1 * s[ii] + w2 * s[ii + 1];
            }
        }

        s.pop_back();
        lsk = bpos - k;
        int rsk = n - bpos - 1;
        if(lsk < 0){
            if(-lsk < s.size()){
                for(int i = 0; i < -lsk; i++){
                    s.erase(s.begin());
                    ds.erase(ds.begin());
                }
            }
            else{
                s.clear();
                ds.clear();
            }
            lsk = 0;
        }
        if(rsk < 0){
            if(-rsk < s.size()){
                for(int i = 0; i < -rsk; i++){
                    s.pop_back();
                    ds.pop_back();
                }
            }
            else{
                s.clear();
                ds.clear();
            }
        }
    }

public:
    explicit Curve(std::vector<Vector3f> points) : controls(std::move(points)) {}

    bool intersect(const Ray &r, Hit &h, float tmin) override {
        return false;
    }

    std::vector<Vector3f> &getControls() {
        return controls;
    }

    virtual void discretize(int resolution, std::vector<CurvePoint>& data) = 0;

    void drawGL() override {
        Object3D::drawGL();
        glPushAttrib(GL_ALL_ATTRIB_BITS);
        glDisable(GL_LIGHTING);
        glColor3f(1, 1, 0);
        glBegin(GL_LINE_STRIP);
        for (auto & control : controls) { glVertex3fv(control); }
        glEnd();
        glPointSize(4);
        glBegin(GL_POINTS);
        for (auto & control : controls) { glVertex3fv(control); }
        glEnd();
        std::vector<CurvePoint> sampledPoints;
        discretize(30, sampledPoints);
        glColor3f(1, 1, 1);
        glBegin(GL_LINE_STRIP);
        for (auto & cp : sampledPoints) { glVertex3fv(cp.V); }
        glEnd();
        glPopAttrib();
    }
};

class BezierCurve : public Curve {
public:
    explicit BezierCurve(const std::vector<Vector3f> &points) : Curve(points) {
        if (points.size() < 4 || points.size() % 3 != 1) {
            printf("Number of control points of BezierCurve must be 3n+1!\n");
            exit(0);
        }

        n = controls.size(); // 样条曲线的控制点个数
        k = n - 1; // 样条曲线的次数
        for(int i = 0; i < n; i++) knot.push_back(0);
        for(int i = 0; i < n; i++) knot.push_back(1);

        knotpad = knot;
        for(int i = 0; i < k; i++) knotpad.push_back(knot.back());
    }

    void discretize(int resolution, std::vector<CurvePoint>& data) override {
        data.clear();
        // TODO (PA2): fill in data vector
        for(double t = 0; t <= 1; t += (double)1 / (resolution * n / k)){ // t ∈ I = [0, 1]，[0, 1]分成resolution * n / k份
            std::vector<double> pt(k + 2, 0), dpt(k + 1, 1); 
            int lsk;
            evaluate(t, pt, dpt, lsk);

            CurvePoint temp = {Vector3f::ZERO, Vector3f::ZERO};
            for (int i = 0; i < pt.size(); i++) {
                temp.V += controls[i + lsk] * pt[i];
                temp.T += controls[i + lsk] * dpt[i];
            }
            data.push_back(temp);
        }
    }

protected:

};

class BsplineCurve : public Curve {
public:
    BsplineCurve(const std::vector<Vector3f> &points) : Curve(points) {
        if (points.size() < 4) {
            printf("Number of control points of BspineCurve must be more than 4!\n");
            exit(0);
        }
        
        n = controls.size(); // 样条曲线的控制点个数
        k = 3; // 样条曲线的次数
        for(int i = 0; i < n + k + 1; i++) knot.push_back((double)i / (n + k));

        knotpad = knot;
        for(int i = 0; i < k; i++) knotpad.push_back(knot.back());
    }

    void discretize(int resolution, std::vector<CurvePoint>& data) override {
        data.clear();
        // TODO (PA2): fill in data vector
        for(double t = knot[k]; t <= knot[n]; t += (double)(knot[n] - knot[k]) / (resolution * n / k)){ // t ∈ I = [tk, tn]，[tk, tn]分成resolution * n / k份
            std::vector<double> pt(k + 2, 0), dpt(k + 1, 1); 
            int lsk;
            evaluate(t, pt, dpt, lsk);

            CurvePoint temp = {Vector3f::ZERO, Vector3f::ZERO};
            for (int i = 0; i < pt.size(); i++) {
                temp.V += controls[i + lsk] * pt[i];
                temp.T += controls[i + lsk] * dpt[i];
            }
            data.push_back(temp);
        }
    }

protected:

};

#endif // CURVE_HPP
