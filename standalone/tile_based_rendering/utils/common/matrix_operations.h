#ifndef matrix_operations_h
#include "common.h"
#include <math.h>
#define matrix_operations_h

using namespace std;

/*  Multiply 2 Matrix and Vector */
float MultiplyMatrixVector(Vect3d &i, Vect3d &o, Matrix4x4 &m){
    o.x = i.x * m.m[0][0] + i.y * m.m[1][0] + i.z * m.m[2][0] + m.m[3][0]; 		
    o.y = i.x * m.m[0][1] + i.y * m.m[1][1] + i.z * m.m[2][1] + m.m[3][1]; 		
    o.z = i.x * m.m[0][2] + i.y * m.m[1][2] + i.z * m.m[2][2] + m.m[3][2]; 		
    float w = i.x * m.m[0][3] + i.y * m.m[1][3] + i.z * m.m[2][3] + m.m[3][3]; 		// is simply z 
    o.w = w;

     // Normalize Z - as Z is larger x and y get smaller
    if (w != 0.0f) { 			
        o.x /= w; 
        o.y /= w; 
        o.z /= w; 		
    } 	

    return w;
}


// Vector Multiply Matrix & Normalize Z
Vect3d MatrixMultiplyVector(Matrix4x4 &matrix,Vect3d &vector){
    Vect3d res;
    res.x = vector.x * matrix.m[0][0] + vector.y * matrix.m[1][0] + vector.z * matrix.m[2][0] +  matrix.m[3][0];
    res.y = vector.x * matrix.m[0][1] + vector.y * matrix.m[1][1] + vector.z * matrix.m[2][1] +  matrix.m[3][1];
    res.z = vector.x * matrix.m[0][2] + vector.y * matrix.m[1][2] + vector.z * matrix.m[2][2] +  matrix.m[3][2];
    res.w = vector.x * matrix.m[0][3] + vector.y * matrix.m[1][3] + vector.z * matrix.m[2][3] + matrix.m[3][3];
    
    if (res.w != 0.0f){
        res.x /= res.w;
        res.y /= res.w;
        res.z /= res.w;
    }

    return res;
}

Vect3d MatrixMultiplyVectorWN(Matrix4x4 &matrix,Vect3d &vector){
    Vect3d res;
    res.x = vector.x * matrix.m[0][0] + vector.y * matrix.m[1][0] + vector.z * matrix.m[2][0] +  matrix.m[3][0];
    res.y = vector.x * matrix.m[0][1] + vector.y * matrix.m[1][1] + vector.z * matrix.m[2][1] +  matrix.m[3][1];
    res.z = vector.x * matrix.m[0][2] + vector.y * matrix.m[1][2] + vector.z * matrix.m[2][2] +  matrix.m[3][2];
    res.w = vector.x * matrix.m[0][3] + vector.y * matrix.m[1][3] + vector.z * matrix.m[2][3] + matrix.m[3][3];
    return res;
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

// Return a rotation matrix on axis Z
Matrix4x4 getMatrixRotationZ(float rotationAngle){
    Matrix4x4 matRotZ = Matrix4x4_MakeIdentity();

    matRotZ.m[0][0] = cosf(rotationAngle);
    matRotZ.m[0][1] = -sinf(rotationAngle);
    matRotZ.m[1][0] = sinf(rotationAngle);
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

// Rotation Y
Matrix4x4 getMatrixRotationY(float rotationAngle){
    Matrix4x4 matRotY = Matrix4x4_MakeIdentity();
    matRotY.m[0][0] = cosf(rotationAngle);
    matRotY.m[0][2] = sinf(rotationAngle);
    matRotY.m[2][0] = -sinf(rotationAngle);
    matRotY.m[2][2] = cosf(rotationAngle);

    return matRotY;
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

// Fast matrix multiplication
Matrix4x4 MatrixMultiplyMatrixDirect(Matrix4x4 &a, Matrix4x4 &b){
    Matrix4x4 res; 
    res.m[0][0] = b.m[3][0]*a.m[0][3]+a.m[0][0]*b.m[0][0]+a.m[0][1]*b.m[1][0]+a.m[0][2]*b.m[2][0]; 
    res.m[0][1] = b.m[3][1]*a.m[0][3]+a.m[0][0]*b.m[0][1]+a.m[0][1]*b.m[1][1]+a.m[0][2]*b.m[2][1]; 
    res.m[0][2] = b.m[2][2]*a.m[0][2]+b.m[3][2]*a.m[0][3]+a.m[0][0]*b.m[0][2]+a.m[0][1]*b.m[1][2]; 
    res.m[0][3] = b.m[2][3]*a.m[0][2]+b.m[3][3]*a.m[0][3]+a.m[0][0]*b.m[0][3]+a.m[0][1]*b.m[1][3]; 


    res.m[1][0] = b.m[3][0]*a.m[1][3]+a.m[1][1]*b.m[1][0]+a.m[1][2]*b.m[2][0]+b.m[0][0]*a.m[1][0];
    res.m[1][1] = b.m[3][1]*a.m[1][3]+a.m[1][1]*b.m[1][1]+a.m[1][2]*b.m[2][1]+b.m[0][1]*a.m[1][0];
    res.m[1][2] = b.m[2][2]*a.m[1][2]+b.m[3][2]*a.m[1][3]+a.m[1][1]*b.m[1][2]+b.m[0][2]*a.m[1][0];
    res.m[1][3] = b.m[2][3]*a.m[1][2]+b.m[3][3]*a.m[1][3]+a.m[1][1]*b.m[1][3]+b.m[0][3]*a.m[1][0];


    res.m[2][0] = b.m[3][0]*a.m[2][3]+a.m[2][0]*b.m[0][0]+a.m[2][1]*b.m[1][0]+a.m[2][2]*b.m[2][0]; 
    res.m[2][1] = b.m[3][1]*a.m[2][3]+a.m[2][0]*b.m[0][1]+a.m[2][1]*b.m[1][1]+a.m[2][2]*b.m[2][1]; 
    res.m[2][2] = b.m[2][2]*a.m[2][2]+b.m[3][2]*a.m[2][3]+a.m[2][0]*b.m[0][2]+a.m[2][1]*b.m[1][2]; 
    res.m[2][3] = b.m[2][3]*a.m[2][2]+b.m[3][3]*a.m[2][3]+a.m[2][0]*b.m[0][3]+a.m[2][1]*b.m[1][3];
    
    res.m[3][0] = b.m[3][0]*a.m[3][3]+a.m[3][0]*b.m[0][0]+a.m[3][1]*b.m[1][0]+a.m[3][2]*b.m[2][0]; 
    res.m[3][1] = b.m[3][1]*a.m[3][3]+a.m[3][0]*b.m[0][1]+a.m[3][1]*b.m[1][1]+a.m[3][2]*b.m[2][1]; 
    res.m[3][2] = b.m[2][2]*a.m[3][2]+b.m[3][2]*a.m[3][3]+a.m[3][0]*b.m[0][2]+a.m[3][1]*b.m[1][2]; 
    res.m[3][3] = b.m[2][3]*a.m[3][2]+b.m[3][3]*a.m[3][3]+a.m[3][0]*b.m[0][3]+a.m[3][1]*b.m[1][3];

    return res ;
}


// Inverts matrice
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

// Scaling 
Matrix4x4 getScalingMatrix(float scale){
    Matrix4x4 res = Matrix4x4_MakeIdentity();
    res.m[0][0] = scale;
    res.m[1][1] = scale;
    res.m[2][2] = scale;
    return res;
}
#endif