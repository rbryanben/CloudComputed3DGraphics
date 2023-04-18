#ifndef cl_common.hpp
#define cl_common.hpp
// Vect2d
typedef struct cl_Vect2d{
    float u,v,w;
}cl_Vect2d;

// Vect3d 
typedef struct cl_Vect3d{
    float x,y,z,w;
} cl_Vect3d;

// RGB 
typedef struct cl_RGB
{
    float r,g,b;
} cl_RGB;


// Triangle 
typedef struct cl_Triangle
{
    cl_Vect3d p[3];
    cl_Vect2d t[3];
    int texture;
    bool hasData;
} cl_Triangle;

// Matrix4x4 
// 4x4 Matrix
struct cl_Matrix4x4{
    float m[4][4];
};

/* Pixel Texture  Out */
typedef struct cl_Pixel_Texture_Out{
    float textureV;
    float textureU;
    int textureNumber;
    float depth;
}cl_Pixel_Texture_Out;

#endif