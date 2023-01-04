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
TEST(test_matrices,matrix_matrix_multiplication){
    // matrix a
    Matrix4x4 a = {
        1.f, 2.f, 3.f, 4.f,
        5.f, 6.f, 7.f, 8.f,
        9.f, 10.f, 11.f, 12.f,
        13.f, 14.f, 15.f, 16.f
    };

    Matrix4x4 b = {
        2.2f, 2.1f, 3.f, 4.f,
        5.f, 6.3f, 7.f, 8.f,
        9.f, 110.f, 11.f, 12.f,
        13.f, 14.f, 5.4f, 2.1f
    };

    Matrix4x4 a_dot_b = {
        91.2f, 400.7f, 71.6f, 64.4f,
        208.f, 930.3f, 177.2f, 168.8f,
        324.8f, 1459.9f, 282.8f, 273.2f,
        441.6f, 1989.5f, 388.4f, 377.6f    
    };

    ASSERT_EQ((a*b).getHash(),a_dot_b.getHash());
}

// Test Clipping of triangle 
TEST(clipping,triangle_clipping_against_plane_Z){
    
    Vect3d zPoint = {0,0,0.1f,1};
    Vect3d intersection_a, intersection_b;

    // One point out
    Triangle test_1 = {3,0,-8,1, 3,2,1,1, 4,7,1,1 };
    intersection_a = vectorIntersectPlane(zPoint,{0,0,1.f,1},test_1.p[0],test_1.p[1]);
    intersection_b = vectorIntersectPlane(zPoint,{0,0,1,1},test_1.p[0],test_1.p[2]);
    vector<Triangle> clippedTriangles = clipTriangleAgainstPlane(zPoint,{0,0,1,1},test_1); 

    ASSERT_TRUE(clippedTriangles[0].p[0] == test_1.p[1]);
    ASSERT_TRUE(clippedTriangles[0].p[1] == test_1.p[2]);
    ASSERT_TRUE(clippedTriangles[0].p[2] == intersection_b);

    ASSERT_TRUE(clippedTriangles[1].p[0] == test_1.p[1]);
    ASSERT_TRUE(clippedTriangles[1].p[1] == intersection_b);
    ASSERT_TRUE(clippedTriangles[1].p[2] == intersection_a);

    // two points out
    Triangle test_2 = {3,0,-8,1, 3,2,1,1, 4,7,-1,1 };
    intersection_a = vectorIntersectPlane(zPoint,{0,0,1.f,1},test_2.p[1],test_2.p[0]);
    intersection_b = vectorIntersectPlane(zPoint,{0,0,1.f,1},test_2.p[1],test_2.p[2]);
    clippedTriangles = clipTriangleAgainstPlane(zPoint,{0,0,1,1},test_2); 

    ASSERT_TRUE(clippedTriangles[0].p[0] == test_1.p[1]);
    ASSERT_TRUE(clippedTriangles[0].p[2] == intersection_b);
    ASSERT_TRUE(clippedTriangles[0].p[1] == intersection_a);

    // All points out
    Triangle test_3 = {3,0,-8,1, 3,2,-1,1, 4,7,-1,1 };
    clippedTriangles = clipTriangleAgainstPlane(zPoint,{0,0,1,1},test_3);
    ASSERT_TRUE(clippedTriangles.size() == 0);

    // All points in
    Triangle test_4 = {3,0,8,1, 3,2,1,1, 4,7,1,1 };
    clippedTriangles = clipTriangleAgainstPlane(zPoint,{0,0,1,1},test_4);

    ASSERT_TRUE(test_4.p[0]== clippedTriangles[0].p[0]);
    ASSERT_TRUE(test_4.p[1]== clippedTriangles[0].p[1]);
    ASSERT_TRUE(test_4.p[2]== clippedTriangles[0].p[2]);
};

// Test Plane intersection
TEST(lines,line_intersect_plane){
    Vect3d point1 = {2,3,1};
    Vect3d point2 = {10,3,2};
    Vect3d ans = {8.f,3.f,1.75f,1.f};
    //intersection with plane x = 8 
    Vect3d intersection = vectorIntersectPlane({8,0,0,1},{-1,0,0,1},point1,point2);

    ASSERT_TRUE(intersection == ans);
};

int main(int argc, char **argv) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}