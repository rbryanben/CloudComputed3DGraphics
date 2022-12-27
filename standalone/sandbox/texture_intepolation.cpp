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
    glBegin(GL_POINTS);
    glColor3f(1,1,0);
    // Points 
    int x1 = tri.p[0].x;
    int y1 = tri.p[0].y;
    float u1 = tri.t[0].u;
    float v1 = tri.t[0].v;

    int x2 = tri.p[1].x;
    int y2 = tri.p[1].y; 
    float u2 = tri.t[1].u;
    float v2 = tri.t[1].v;

    int x3 = tri.p[2].x;
    int y3 = tri.p[2].y;
    float u3 = tri.t[2].u;
    float v3 = tri.t[2].v;


    // Sort Points 
    if (y2 < y1){
        swap(y2,y1);
        swap(x2,x1);
        swap(u2,u1);
        swap(v2,v1);
    }

    if (y3 < y1){
        swap(y3,y1);
        swap(x3,x1);
        swap(u3,u1);
        swap(v3,v1);
    }

    if (y3 < y2){
        swap(y3,y2);
        swap(x3,x2);
        swap(u3,u2);
        swap(v3,v2);
    }

    // Args 
    float dy1 = y2 - y1 ;
    float dx1 = x2 - x1 ;
    float dv1 = v2 - v1;
    float du1 = u2 - u1;

    float dy2 = y3 - y1;
    float dx2 = x3 - x1;
    float dv2 = v3 - v1;
    float du2 = u3 - u1;


    // Calculate dx_1 and dx_2 
    float dax_step = 0.f , dbx_step = 0.f,
        du1_step = 0.f, dv1_step = 0.f,
        du2_step =0.f, dv2_step = 0.f;

    float tex_u,tex_v;


    // Calculate differentials 
    if (dy1) dax_step = dx1/abs(dy1); //float absolutes 
    if (dy2) dbx_step = dx2/abs(dy2);

    if (dy1) dv1_step = dv1/abs(dy1);
    if (dy2) du1_step = du1/abs(dy1);

    if (dy2) dv2_step = dv2/abs(dy2);
    if (dy2) du2_step = du2/abs(dy2);


    // Draw top half 
    if (dy1){
        // Move y 
        for (int i = y1; i <= y2;i++){
            int ax = x1 + (float)(i - y1) * dax_step;
            int bx = x1 + (float)(i - y1) * dbx_step;

            float tex_su = u1 + (float)(i - y1) * du1_step;
            float tex_sv = v1 + (float)(i - y1) * dv2_step;

            float tex_eu = u1 + (float)(i - y1) * du2_step;
            float tex_ev = v1 + (float)(i - y1) * dv2_step;

            // Sort the x axis
            if (ax > bx){
                swap(ax,bx);
                swap(tex_su,tex_eu);
                swap(tex_sv,tex_ev);
            }

            tex_u = tex_su;
            tex_v = tex_sv;

            float tstep = 1.0f / ((float)(bx - ax));
            float t = 0.0f;

            for (int j = ax; j <bx; j++){
                tex_u = (1.0f - t) * tex_su + t * tex_eu;
                tex_v = (1.0f - t) * tex_sv + t * tex_ev;
                RGB pixel = texture.getPixelAt(tex_v * texture.getHeight(),tex_u * texture.getWidth());
                glColor3f(pixel.r,pixel.g,pixel.b);
                glVertex2i(j,i);

                t += tstep;
            }

        }
    }

    dy1 = y3 - y2;
		dx1 = x3 - x2;
		dv1 = v3 - v2;
		du1 = u3 - u2;


		if (dy1) dax_step = dx1 / (float)abs(dy1);
		if (dy2) dbx_step = dx2 / (float)abs(dy2);

		du1_step = 0, dv1_step = 0;
		if (dy1) du1_step = du1 / (float)abs(dy1);
		if (dy1) dv1_step = dv1 / (float)abs(dy1);


		if (dy1)
		{
			for (int i = y2; i <= y3; i++)
			{
				int ax = x2 + (float)(i - y2) * dax_step;
				int bx = x1 + (float)(i - y1) * dbx_step;

				float tex_su = u2 + (float)(i - y2) * du1_step;
				float tex_sv = v2 + (float)(i - y2) * dv1_step;

				float tex_eu = u1 + (float)(i - y1) * du2_step;
				float tex_ev = v1 + (float)(i - y1) * dv2_step;


				if (ax > bx)
				{
					swap(ax, bx);
					swap(tex_su, tex_eu);
					swap(tex_sv, tex_ev);

				}

				tex_u = tex_su;
				tex_v = tex_sv;


				float tstep = 1.0f / ((float)(bx - ax));
				float t = 0.0f;

				for (int j = ax; j < bx; j++)
				{
					tex_u = (1.0f - t) * tex_su + t * tex_eu;
					tex_v = (1.0f - t) * tex_sv + t * tex_ev;
                    RGB pixel = texture.getPixelAt(tex_v * texture.getHeight(),tex_u * texture.getWidth());
                    glColor3f(pixel.r,pixel.g,pixel.b);
                    glVertex2i(j,i);

					t += tstep;
				}
			}	
		}		
    glEnd();


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