#ifndef convert.hpp
#define convert.hpp
// Includes 
#include "../common/common.h";
#include "../image/image_loader.h"
#include "../../opencl_kernels/cl_common.hpp";

/***
 * @brief converts an Image object to an cl_Image
 */
cl_Image to_cl_Image(Image &image){
    cl_Image res;
    // Copy Meta Data 
    res.height = image.height;
    res.width = image.width;
   
    // Fill the color matrix
    for (int i=0; i != res.height*res.width;i++){
        res.matrix[i] = {image.matrix[i].r,image.matrix[i].g,image.matrix[i].b};
    }
    //Create the array 
    return res;

}
#endif