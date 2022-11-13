#ifndef matrix_operations_h
#include "common.h"
#include <math.h>
#define matrix_operations_h

using namespace std;

/*  Multiply 2 Matrix and Vector */
void MultiplyMatrixVector(Vect3d &i, Vect3d &o, Matrix4x4 &m){
    o.x = i.x * m.m[0][0] + i.y * m.m[1][0] + i.z * m.m[2][0] + m.m[3][0]; 		
    o.y = i.x * m.m[0][1] + i.y * m.m[1][1] + i.z * m.m[2][1] + m.m[3][1]; 		
    o.z = i.x * m.m[0][2] + i.y * m.m[1][2] + i.z * m.m[2][2] + m.m[3][2]; 		
    float w = i.x * m.m[0][3] + i.y * m.m[1][3] + i.z * m.m[2][3] + m.m[3][3]; 		
    o.w = w;
    // Normalize Z - as Z is larger x and y get smaller
    if (w != 0.0f) { 			
        o.x /= w; 
        o.y /= w; 
        o.z /= w; 		
    } 	
}


// Vector Multiply Matrix 
Vect3d MatrixMultiplyVector(Matrix4x4 &matrix,Vect3d &vector){
    Vect3d res;
    res.x = vector.x * matrix.m[0][0] + vector.y * matrix.m[1][0] + vector.z * matrix.m[2][0] + vector.w * matrix.m[3][0];
    res.y = vector.x * matrix.m[0][1] + vector.y * matrix.m[1][1] + vector.z * matrix.m[2][1] + vector.w * matrix.m[3][1];
    res.z = vector.x * matrix.m[0][2] + vector.y * matrix.m[1][2] + vector.z * matrix.m[2][2] + vector.w * matrix.m[3][2];
    res.w = vector.x * matrix.m[0][3] + vector.y * matrix.m[1][3] + vector.z * matrix.m[2][3] + vector.w * matrix.m[3][3];

    return res;
}

// Return a rotation matrix on axis Z
Matrix4x4 getMatrixRotationZ(float rotationAngle){
    Matrix4x4 matRotZ;

    matRotZ.m[0][0] = cosf(rotationAngle);
    matRotZ.m[0][1] = sinf(rotationAngle);
    matRotZ.m[1][0] = -sinf(rotationAngle);
    matRotZ.m[1][1] = cosf(rotationAngle);
    matRotZ.m[2][2] = 1;
    matRotZ.m[3][3] = 1;

    return matRotZ;
}

// Return a rotation matrix on axis X
Matrix4x4 getMatrixRotationX(float rotationAngle){
    Matrix4x4 matRotX;
    matRotX.m[0][0] = 1;
    matRotX.m[1][1] = cosf(rotationAngle * 0.5f);
    matRotX.m[1][2] = sinf(rotationAngle * 0.5f);
    matRotX.m[2][1] = -sinf(rotationAngle * 0.5);
    matRotX.m[2][2] = cosf(rotationAngle * 0.5f);
    matRotX.m[3][3] = 1;

    return matRotX;
}

// Make Identity Matrix 
Matrix4x4 Matrix4x4_MakeIdentity(){
    Matrix4x4 res;
    res.m[0][0] = 1;
    res.m[1][1] = 1;
    res.m[2][2] = 1;
    res.m[3][3] = 1 ;
    return res;
}

// Make Translation
Matrix4x4 Matrix_MakeTranslation(float x, float y, float z){
    Matrix4x4 res = Matrix4x4_MakeIdentity();
    res.m[3][0] = x;
    res.m[3][1] = y;
    res.m[3][2] = z;
    return res;
}

// Make Projection
Matrix4x4 Make_Projection(float aspectRatio,float fieldView,float zfar,float znear){
    float tangentFunction = 1.0f / tanf(fieldView * 0.5f / 180.0f * 3.14159f);
    float zfarFunction = (zfar/(zfar-znear)) - ((zfar*znear)/(zfar-znear));
    
    Matrix4x4 projectionMatrix;
    projectionMatrix.m[0][0] = aspectRatio * tangentFunction;
    projectionMatrix.m[1][1] = tangentFunction;
    projectionMatrix.m[2][2] =  zfar/(zfar-znear);
    projectionMatrix.m[3][2] = (-zfar*znear)/(zfar-znear);
    projectionMatrix.m[2][3] = 1.0f;
    return projectionMatrix;
}

// Matrix Multiply Matrix
Matrix4x4 MatrixMultiplyMatrix(Matrix4x4 &a, Matrix4x4 &b){
    Matrix4x4 res;

    for(int r=0;r!=4;r++){
        for(int c=0; c!=4;c++){ 
            for(int k=0;k!=4;k++){
                res.m[r][c] += (a.m[r][k]*b.m[k][c]);
            }
        }
    }

    return res;
}

Matrix4x4 MatrixPointAt(Vect3d &pos,Vect3d &target,Vect3d &up){
    Matrix4x4 res;

    //calculate new forward 
    Vect3d newFoward = target - pos;
    NormalizeVector(newFoward);

    //calculate the new forward
    Vect3d a = VectorMultiplyFloat_Return(newFoward,VectorDotProduct(up,newFoward));
    Vect3d newUp = up - a ;
    NormalizeVector(newUp);


    // Orthogonal vector - derived from the cross product of the up and new Forward
    Vect3d orthorgonal = VectorCrossProduct(newFoward,newUp);
    

    // Fix here (incomplete)
    res.m[0][0] = orthorgonal.x; res.m[0][1] = orthorgonal.y ; res.m[0][2] = orthorgonal.z;
    res.m[1][0] = newUp.x; res.m[1][1] = newUp.y ; res.m[1][2] = newUp.z;
    res.m[2][0] = newFoward.x; res.m[2][1] = newFoward.y, res.m[2][2] = newFoward.z;
    res.m[3][0] = pos.x; res.m[3][1] = pos.y; res.m[3][2] = pos.z; res.m[3][3] = 1.0f;
    return res;
}

Matrix4x4 Matrix_QuickInverse(Matrix4x4 &m) // Only for Rotation/Translation Matrices
{
    Matrix4x4 matrix;
    matrix.m[0][0] = m.m[0][0]; matrix.m[0][1] = m.m[1][0]; matrix.m[0][2] = m.m[2][0]; matrix.m[0][3] = 0.0f;
    matrix.m[1][0] = m.m[0][1]; matrix.m[1][1] = m.m[1][1]; matrix.m[1][2] = m.m[2][1]; matrix.m[1][3] = 0.0f;
    matrix.m[2][0] = m.m[0][2]; matrix.m[2][1] = m.m[1][2]; matrix.m[2][2] = m.m[2][2]; matrix.m[2][3] = 0.0f;
    matrix.m[3][0] = -(m.m[3][0] * matrix.m[0][0] + m.m[3][1] * matrix.m[1][0] + m.m[3][2] * matrix.m[2][0]);
    matrix.m[3][1] = -(m.m[3][0] * matrix.m[0][1] + m.m[3][1] * matrix.m[1][1] + m.m[3][2] * matrix.m[2][1]);
    matrix.m[3][2] = -(m.m[3][0] * matrix.m[0][2] + m.m[3][1] * matrix.m[1][2] + m.m[3][2] * matrix.m[2][2]);
    matrix.m[3][3] = 1.0f;
    return matrix;
}



Matrix4x4 Inverse4x4(Matrix4x4 &matrix){
    Matrix4x4 res;

}

#endif