#include "GL/freeglut.h"
#include "GL/gl.h"
#include <string.h>
#include <fstream>
#include <vector>
#include <iostream>
#include <stdlib.h>
#include <err.h>
#include <chrono>
#include <strstream>


// Libs
#include "../utils/image/image_loader.h"
#include "../utils/common/common.h"
#include "../utils/common/matrix_operations.h"
#include "../utils/camera/camera.h"

// Test Intersection of line and plane
void testIntersection(){
    Vect3d plane_n = {0,0,1,1};
    Vect3d plane_p = {0,0,0.1f,1};
    Vect3d s = {3,2,4,1};
    Vect3d e = {8,7,-3,1};
    float t;
    Vect3d res = vectorIntersectPlane(plane_p,plane_n,s,e,t);

}


int main(int argc,char** argv){
    testIntersection();
    return 0;
}