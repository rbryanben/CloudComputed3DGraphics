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

// Test that all triangles of a mesh have the same parent
TEST(test_triangles,triangles_in_a_mesh_have_the_same_parent){
    // Meshs 
    Mesh pathwalk = Mesh();
    Mesh drum = Mesh();
    // load objects 
    pathwalk.LoadFromObjectFile("../assets/objs/pathwalk/pathwalk.obj",readPPM("../assets/objs/pathwalk/texture.ppm"));
    drum.LoadFromObjectFile("../assets/objs/drum/drum.obj",readPPM("../assets/objs/drum/texture.ppm"));


    // test the same parents
    ASSERT_TRUE(pathwalk.triangles[0].parent == pathwalk.triangles[1].parent 
        && pathwalk.triangles[pathwalk.triangles.size() - 1].parent == pathwalk.triangles[1].parent);

    // test the same parents
    ASSERT_TRUE(drum.triangles[0].parent == drum.triangles[1].parent 
        && drum.triangles[drum.triangles.size() - 1].parent == drum.triangles[1].parent);

    // test parents are not the same
    ASSERT_TRUE(drum.triangles[0].parent != pathwalk.triangles[1].parent 
        && pathwalk.triangles[pathwalk.triangles.size() - 1].parent != drum.triangles[1].parent);
}

int main(int argc, char **argv) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}