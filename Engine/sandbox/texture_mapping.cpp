// This program is meant to demostrate texture interpolation
// Authour - Ryan Ben 
#include "GL/freeglut.h"
#include "GL/gl.h"
#include <iostream>
#include <stdlib.h>
#include <err.h>
#include <strstream>
#include <fstream>

// Libraries
#include "../utils/common.h"
#include "../PPA/image_loader.h"


// Definitions
#define WINDOW_WIDTH 800;
#define WINDOW_HEIGHT 800;

// Variables
Triangle triangle_1;
Image texture;

// Texture Triangle
void textureTriangle(Triangle tri){
    //Draw lines 
    glBegin(GL_LINES);
        glColor3f(1,0,0);
        glVertex2i(tri.p[0].x,tri.p[0].y);
        glVertex2i(tri.p[1].x,tri.p[1].y);
        glVertex2i(tri.p[1].x,tri.p[1].y);
        glVertex2i(tri.p[2].x,tri.p[2].y);
        glVertex2i(tri.p[2].x,tri.p[2].y);
        glVertex2i(tri.p[0].x,tri.p[0].y);
    glEnd();
}

// Display Function 
void displayFunc(){
    // Create Triangle 
    triangle_1.p[0] = {100,100,0};
    triangle_1.p[1] = {250,250,0};
    triangle_1.p[2] = {250,50,0,0};
    triangle_1.t[0] = {0,0};
    triangle_1.t[1] = {1,1};
    triangle_1.t[2] = {1,0};

    //load texture
    texture = readPPM("../assets/objs/crate/crate.ppm");

    
    // Clear Color 
    glClearColor(0,0,0,1);
    glClear(GL_COLOR_BUFFER_BIT);
    glOrtho(0,800,800,0,0,1000);
        textureTriangle(triangle_1);
    glFlush();
}


// Program Entry
int main(int argc,char **argv){

    // Setup OpenGL 
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_SINGLE);
    glutInitWindowSize(800,800);
    glutInitWindowPosition(2000, 100);
    glutCreateWindow("OpenGL - Texture Interpolation");
    glutDisplayFunc(displayFunc);
    glutMainLoop();            
    return 1;
}