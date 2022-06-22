#include <cassert>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cmath>
#include <iostream>

#include "scene_parser.hpp"
#include "image.hpp"
#include "camera.hpp"
#include "group.hpp"
#include "light.hpp"
#include "camera_controller.hpp"

#include <string>

using namespace std;

// Global variables used by UI handlers.
// Of course you can use partial lambda functions to make things more graceful.
SceneParser *sceneParser;
CameraController *cameraController;
int imgW, imgH;
string savePicturePath;

void screenCapture() {
    Image openglImg(imgW, imgH);
    auto *pixels = new unsigned char[3 * imgW * imgH];
    glPixelStorei(GL_PACK_ALIGNMENT, 1);
    // glReadBuffer(GL_BACK);
    glReadBuffer(GL_FRONT);
    glReadPixels(0, 0, imgW, imgH, GL_RGB, GL_UNSIGNED_BYTE, pixels);
    for (int x = 0; x < imgW; ++x) {
        for (int y = 0; y < imgH; ++y) {
            Vector3f color(
                    pixels[3 * (y * imgW + x) + 0] / 255.0,
                    pixels[3 * (y * imgW + x) + 1] / 255.0,
                    pixels[3 * (y * imgW + x) + 2] / 255.0);
            openglImg.SetPixel(x, y, color);
        }
    }
    openglImg.SaveImage(savePicturePath.c_str());
    delete[] pixels;
    cout << "Current viewport captured at " << savePicturePath << "." << endl;
}

//  Called when mouse button is pressed.
void mouseFunc(int button, int state, int x, int y) {
    if (state == GLUT_DOWN) {

        switch (button) {
            case GLUT_LEFT_BUTTON:
                cameraController->mouseClick(CameraController::LEFT, x, y);
                break;
            case GLUT_MIDDLE_BUTTON:
                cameraController->mouseClick(CameraController::MIDDLE, x, y);
                break;
            case GLUT_RIGHT_BUTTON:
                cameraController->mouseClick(CameraController::RIGHT, x, y);
            default:
                break;
        }
    } else {
        cameraController->mouseRelease(x, y);
    }
    glutPostRedisplay();
}

// Called when mouse is moved while button pressed.
void motionFunc(int x, int y) {
    cameraController->mouseDrag(x, y);
    glutPostRedisplay();
}

// Called when the window is resized
void reshapeFunc(int w, int h) {
    sceneParser->getCamera()->resize(w, h);
    glViewport(0, 0, w, h);
    imgW = w;
    imgH = h;
}

// This function is responsible for displaying the object.
void drawScene() {
    Vector3f backGround = sceneParser->getBackgroundColor();
    glClearColor(backGround.x(), backGround.y(), backGround.z(), 1.0);

    // Clear the rendering window
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Setup MODELVIEW Matrix
    sceneParser->getCamera()->setupGLMatrix();

    // Turn On all lights.
    for (int li = 0; li < sceneParser->getNumLights(); ++li) {
        Light *light = sceneParser->getLight(li);
        light->turnOn(li);
    }
    // Draw elements.
    Group *baseGroup = sceneParser->getGroup();
    baseGroup->drawGL();

    // Dump the image to the screen.
    glutSwapBuffers();

    // Save if not in interactive mode.
    if (!savePicturePath.empty()) {
        screenCapture();
        exit(0);
    }
}

float getCenterDepth() {
    Camera *cam = sceneParser->getCamera();
    Ray centerRay = sceneParser->getCamera()->generateRay(
            Vector2f((float) cam->getWidth() / 2, (float) cam->getHeight() / 2));
    Hit hit;
    bool isHit = sceneParser->getGroup()->intersect(centerRay, hit, 0.0);
    return isHit ? hit.getT() : 10.0f;
}

int main(int argc, char *argv[]) {
    for (int argNum = 1; argNum < argc; ++argNum) {
        std::cout << "Argument " << argNum << " is: " << argv[argNum] << std::endl;
    }

    if (argc != 3) {
        // cout << "Usage: ./bin/PA1 <input scene file> <output bmp file>" << endl;
        return 1;
    }
    string inputFile = argv[1];
    string outputFile = argv[2];  // only bmp is allowed.

    // TODO: Main RayCasting Logic
    // First, parse the scene using SceneParser.
    // Then loop over each pixel in the image, shooting a ray
    // through that pixel and finding its intersection with
    // the scene.  Write the color at the intersection to that
    // pixel in your output image.
    SceneParser sceneParser(argv[1]);
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
    renderedImg.SaveImage(argv[2]);
    return 0;
}

