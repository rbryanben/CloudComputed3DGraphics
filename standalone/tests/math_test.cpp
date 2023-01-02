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

int main(int argc, char **argv) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}