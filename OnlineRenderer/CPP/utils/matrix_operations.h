#ifndef matrix_operations_h
#include "common.h"
#define matrix_operations_h

using namespace std;

/*  Multiply 2 Matrix and Vector */
void MultiplyMatrixVector(Vect3d &i, Vect3d &o, Matrix4x4 &m){
    o.x = i.x * m.m[0][0] + i.y * m.m[1][0] + i.z * m.m[2][0] + m.m[3][0]; 		
    o.y = i.x * m.m[0][1] + i.y * m.m[1][1] + i.z * m.m[2][1] + m.m[3][1]; 		
    o.z = i.x * m.m[0][2] + i.y * m.m[1][2] + i.z * m.m[2][2] + m.m[3][2]; 		
    float w = i.x * m.m[0][3] + i.y * m.m[1][3] + i.z * m.m[2][3] + m.m[3][3]; 		
    
    // Normalize Z - as Z is larger x and y get smaller
    if (w != 0.0f) { 			
        o.x /= w; 
        o.y /= w; 
        o.z /= w; 		
    } 	
}
#endif