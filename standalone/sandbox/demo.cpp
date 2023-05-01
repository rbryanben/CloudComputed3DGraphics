#include <stdlib.h>
#include <iostream>
#include <vector>

struct Vect3D{
    float x,y,z,w=1;
};

struct Triangle{
    Vect3D vertices[3];
};

struct Matrix4x4{
    float m[4][4] = {0};
};

Vect3D MatrixMultiplyVector(Matrix4x4* matrix,Vect3D* vector){
    Vect3D res;
    res.x = vector->x * matrix->m[0][0] + vector->y * matrix->m[1][0] + vector->z * matrix->m[2][0] +  matrix->m[3][0];
    res.y = vector->x * matrix->m[0][1] + vector->y * matrix->m[1][1] + vector->z * matrix->m[2][1] +  matrix->m[3][1];
    res.z = vector->x * matrix->m[0][2] + vector->y * matrix->m[1][2] + vector->z * matrix->m[2][2] +  matrix->m[3][2];
    res.w = vector->x * matrix->m[0][3] + vector->y * matrix->m[1][3] + vector->z * matrix->m[2][3] +  matrix->m[3][3];
    
    return res;
};

struct Mesh{
    char* name; 
    std::vector<Triangle> primitives;
};

void Vect3DTranslate(Vect3D* vertex,Vect3D translation_vector){
    vertex->x += translation_vector.x;
    vertex->y += translation_vector.y;
    vertex->z += translation_vector.z;
}

void Vect3DScale(Vect3D* vertex,float scale_factor){
    vertex->x *= scale_factor;
    vertex->y *= scale_factor;
    vertex->z *= scale_factor;

}