#include "GL/freeglut.h"
#include "GL/gl.h"
#include <string.h>
#include <fstream>
#include <vector>
#include <iostream>
#include <stdlib.h>
#include <err.h>

#include "utils/common.h"


struct RGB
{
    float r=0,g=0,b=0;
};


class Image{
    private:
        int width=0, height=0;
        std::vector<RGB> matrix;

    public:
        Image(){};

        Image(int width, int height){
            this->width = width;
            this->height = height;
        }

        void setWidth(int width){
            this->width = width;
        }

        void setHeight(int height){
            this->height = height;
        }

        void newPixel(float r,float g, float b){
            this->matrix.push_back({r,g,b});
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
};



Image readPPM(const char *filename){
    std::ifstream ifs;
    ifs.open(filename,std::ios::binary);

    if (ifs.fail()){
        err(1,"Can't open file");
    }


    // Image 
    Image image = Image();

    std::string header;
    int w,h,b;

    // Read the header
    ifs >> header;
    if (strcmp(header.c_str(),"P6") != 0){
        err(1,"Can't read input file");
    }

    ifs >> w >> h >> b;
    image.setWidth(w);
    image.setHeight(h);

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

// Given two vectors an the y, returns the x 
float lineEquationFindX(Vect3d line1, Vect3d line2,float y){
    float m = (line1.y - line2.y) / (line1.x - line2.x);
    return y/m;
}

Image texture = readPPM("./assets/ppm/wood.ppm");

// Texture Triangle
void textureTriangle(Triangle tri){
    float y = tri.p[0].y ;
    while (y <= tri.p[2].y){
        float x = tri.p[0].y;
        float xEnd = lineEquationFindX(tri.p[0],tri.p[1],y); 
        while(x <= xEnd){
            
            // Normalize x and y 
            float xNorm = x / (tri.p[0].x + tri.p[1].x);
            float yNorm = y / (tri.p[0].y + tri.p[2].y);
            RGB pixel = texture.getPixelAt(texture.getHeight() * yNorm,texture.getWidth() * xNorm);
            glColor3f(pixel.r,pixel.g,pixel.b);
            glVertex2f(x,y);

            // Increment x 
            x += 1;
        }

        // increment y
        y += 1 ;
    }

}

void drawTriangle()
{
    glClearColor(0,0, 0, 0.4);
    glClear(GL_COLOR_BUFFER_BIT);
    glOrtho(0.0,800.0,800.0,0.0,0,1000);


    // Load image 
    Image image = readPPM("./assets/ppm/ice.ppm");
    
    Triangle tri;
    tri.p[0] = {20.f,20.f,0.f};
    tri.p[1] = {180.f,180.f,0.f};
    tri.p[2] = {20.f,180.f,0.f};
    
    tri.p[0].x += 150;
    tri.p[1].x += 150;
    tri.p[2].x += 150;

    float colorBlue;
    // Draw Texture
    while (1)
    {
        // Clear Screen
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        // Start Drawing 
        glBegin(GL_POINTS);
            // Texture triangle   
            textureTriangle(tri);
        glEnd();
        
        colorBlue += 0.0001;
        glClearColor(0,0, colorBlue, 0.4);
        // Flush Screen
        glFlush();
    }
    
    
}

int main(int argc, char **argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_SINGLE);
    glutInitWindowSize(800, 800);
    glutInitWindowPosition(100, 100);
    glutCreateWindow("OpenGL - Creating a triangle");
    glutDisplayFunc(drawTriangle);
    glutMainLoop();
    return 0;
}