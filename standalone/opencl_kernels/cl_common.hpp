#ifndef cl_common.hpp
#define cl_common.hpp
#include <functional>
#include <math.h>


// Vect2d
typedef struct cl_Vect2d{
    float u,v,w;
}cl_Vect2d;

// Vect3d 
typedef struct cl_Vect3d{
    float x,y,z,w;
} cl_Vect3d;


// Triangle 
typedef struct cl_Triangle
{
    cl_Vect3d p[3];
    cl_Vect2d t[3];
    float avg_depth;
    int texture;
    bool hasData;
} cl_Triangle;

// Texture Detail
typedef struct cl_TextureDetail{
    int width;
    int height;
    int start_index;
}cl_TextureDetail;


// Matrix4x4 
// 4x4 Matrix
struct cl_Matrix4x4{
    float m[4][4] = {0.0f};
};

/* Pixel Texture  Out */
typedef struct cl_Pixel_Texture_Out{
    float textureV;
    float textureU;
    int textureNumber;
    float depth;
}cl_Pixel_Texture_Out;

// If a given integer has a float value, it will be rounded off to the next value 
int round_least(float a){
    float residue = a - (int)a;
    if (residue > 0) return (int)a + 1 ;
    return (int)a;
}

typedef struct cl_GridDetails{  
    int rows;
    int cols;
    int square_width;
}cl_GridDetails;


/* 
    Given a resolution and the number of GPU cores returns a cl_GridDetails 
*/
cl_GridDetails createGridDetails(int windowWidth,int windowHeight,int gpuCoreCount){
    cl_GridDetails res;

    // Get the number of squares each grid square should have 
    float gridSquarePixelsCount = (windowWidth * windowHeight) / gpuCoreCount;

    // Square side 
    float gridSquareWidth = sqrt(gridSquarePixelsCount);

    // Set properties 
    res.square_width = round_least(gridSquareWidth);
    res.cols =  round_least((float) windowHeight / res.square_width);
    res.rows =  round_least((float) windowWidth / res.square_width);
    return res;
}



void writeTrianglesToFile(const cl_Triangle* triangles, size_t numTriangles, const std::string& filename) {
    std::ofstream file(filename);
    if (!file.is_open()) {
        // handle error
        return;
    }

    for (size_t i = 0; i < numTriangles; i++) {
        cl_Triangle tri = triangles[i];
        file << "v" <<  tri.p[0].x << "," << tri.p[0].y << "," << tri.p[0].z;
        file << "v" <<  tri.p[1].x << "," << tri.p[1].y << "," << tri.p[1].z;
        file << "v" <<  tri.p[2].x << "," << tri.p[2].y << "," << tri.p[2].z;
        file << "t" <<  tri.t[0].u << "," << tri.t[0].v;
        file << "t" <<  tri.t[1].u << "," << tri.t[1].v;
        file << "t" <<  tri.t[2].u << "," << tri.t[2].v;
        file << "\n";
    }

    file.close();
}



#endif