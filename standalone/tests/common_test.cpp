#include <gtest/gtest.h>
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


// Test Matrice Multiplication
TEST(test_mesh,mesh_construction){
    Mesh test_mesh;
    for (int r=0;r != 4; r++){
        for (int c=0; c != 4; c++){
            ASSERT_TRUE(test_mesh.geometryMatrix.m[r][c] == 0);
        }
    }
}

int main(int argc, char **argv) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}