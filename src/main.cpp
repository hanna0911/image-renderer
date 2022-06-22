#include <cassert>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cmath>
#include <iostream>

// #include "scene_parser.hpp"
// #include "image.hpp"
// #include "camera.hpp"
// #include "group.hpp"
// #include "light.hpp"
// #include "camera_controller.hpp"
#include "Renderer.hpp"

#include <string>

using namespace std;

// Global variables used by UI handlers.
// Of course you can use partial lambda functions to make things more graceful.
// SceneParser *sceneParser;
// CameraController *cameraController;
// int imgW, imgH;
// string savePicturePath;

int main(int argc, char *argv[]) {
    for (int argNum = 1; argNum < argc; ++argNum) {
        std::cout << "Argument " << argNum << " is: " << argv[argNum] << std::endl;
    }

    if (!strcmp(argv[3], "rc")) {
        RayCasting rc(argv[1], argv[2]);
        rc.Render();
    } 
    /* else if (!strcmp(argv[3], "pt")) {
        int samps = atoi(argv[4]);
        PathTracing pt(argv[1], samps, argv[2]);
        pt.render();
    } else if (!strcmp(argv[3], "sppm")) {
        int numRounds = atoi(argv[4]), numPhotons = atoi(argv[5]),
            ckpt = atoi(argv[6]);
        SPPM sppm(argv[1], numRounds, numPhotons, ckpt, argv[2]);
        sppm.render();
    } */
    else {
        cout << "Unknown method: " << argv[4] << endl;
        return 1;
    }

    return 0;
}

