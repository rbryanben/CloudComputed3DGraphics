#include "../opencl_kernels/cl_common.hpp"
#include <math.h>
#include <stdlib.h>
#include <iostream>


// Returns the min of three float
float cl_minf(float* a,float* b, float* c){
    float min = *a;
    if (*b < min) min = *b;
    if (*c < min) min = *c;
    return min;
}

// Returns the max of three float
float cl_maxf(float* a,float* b, float* c){
    float max = *a;
    if (*b > max) max = *b;
    if (*c > max) max = *c;
    return max;
}

/*
    Given position an (x,y) co-ordinate and grid details object - returns the location if the co-ordinate in the grid  
*/
cl_Vect2d getGridPosition(float x,float y,cl_GridDetails* gridDetails){
    cl_Vect2d res;
    res.u = (int) x / gridDetails->square_width;
    res.v = (int) y / gridDetails->square_width;
    return res;
}



/*
    Given a cl_Triangle and grid_details. Returns the squares in the grid a triangle passes through 
*/
int* cl_TriangleGetGridSquares(cl_GridDetails* gridDetails,cl_Triangle* triangle){
    // Get furthest vertices 
    float farLeft = cl_minf(&triangle->p[0].x,&triangle->p[1].x,&triangle->p[2].x);
    float farRight = cl_maxf(&triangle->p[0].x,&triangle->p[1].x,&triangle->p[2].x);
    float farTop = cl_minf(&triangle->p[0].y,&triangle->p[1].y,&triangle->p[2].y);
    float farBottom = cl_maxf(&triangle->p[0].y,&triangle->p[1].y,&triangle->p[2].y);

    // Get squares for far's 
    cl_Vect2d s_topLeft = getGridPosition(farLeft,farTop,gridDetails);
    cl_Vect2d s_topRight = getGridPosition(farRight,farTop,gridDetails);
    cl_Vect2d s_bottomLeft = getGridPosition(farLeft,farBottom,gridDetails);
    cl_Vect2d s_bottomRight = getGridPosition(farRight,farBottom,gridDetails);

    // Print all values from the top left to the bottom right 
    for (int row = s_topLeft.v; row <= s_bottomRight.v; row++){
        for (int col = s_topLeft.u; col <= s_bottomRight.u; col++){
            std::cout << row << "," << col << std::endl;
        }
    }
}



int main(){
    cl_GridDetails details = createGridDetails(800,800,48);

    cl_Triangle e = {
                512.529968f,714.341614f,390.215881f,0.000000f, 
                512.529968f,517.878113f,390.215881f,0.000000f, 
                351.327972f,576.248718f,385.351196f,0.000000f
            };

    cl_TriangleGetGridSquares(&details,&e);
    
    return 1;
}