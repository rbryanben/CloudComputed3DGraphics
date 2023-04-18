#ifndef image_loader.h
#define image_loader.h

#include <fstream>
#include <iostream>
#include <vector>
#include <stdlib.h>
#include <string.h>
#include <err.h>
#include <iostream>
struct RGB
{
    float r=0,g=0,b=0;
};


struct Image{
    int width=0, height=0;
    RGB* matrix;
    size_t pointer = 0; 

    
    Image(){};

    Image(long null){
    }

    Image(int width, int height){
        this->width = width;
        this->height = height;
        this->matrix = new RGB[width * height];
    }

    void setWidth(int width){
        this->width = width;
    }

    void setHeight(int height){
        this->height = height;
    }

    void newPixel(float r,float g, float b){
        matrix[pointer] = {r,g,b};
        pointer++;
    }

    int getWidth(){
        return this->width;
    }

    int getHeight(){
        return this->height;
    }

    RGB getPixelAt(int r, int c){
        return this->matrix[(r * width)+c];
    }

    bool hasData(){
        if (this->height == 0){
            return false;
        }
        return true;
    }
};



Image readPPM(const char *filename){
    std::ifstream ifs;
    ifs.open(filename,std::ios::binary);

    if (ifs.fail()){
        err(1,"Can't open file");
    }

    std::string header;
    int w,h,b;

    // Read the header
    ifs >> header;
    if (strcmp(header.c_str(),"P6") != 0){
        err(1,"Can't read input file");
    }

    ifs >> w >> h >> b;
     // Image 
    Image image = Image(w,h);


    // Start loading data
    ifs.ignore(256,'\n');
    unsigned char pix[3];

    for (int i=0; i < w* h ; ++i){
        ifs.read(reinterpret_cast<char *>(pix),3);
        image.newPixel(pix[0] / 255.f,pix[1] / 255.f, pix[2] / 255.f);
    }
    ifs.close();
    return image;
}

#endif