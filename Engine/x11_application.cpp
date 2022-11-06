#include <cstdlib>
#include <cstdio>
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <err.h>
#include <string.h>
#include <X11/X.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/XKBlib.h>
#include <X11/Xatom.h>
#include <stdio.h>
#include <err.h>
#include <string.h>
#include <chrono>

// Libs
#include "graphics/x11_helper.chh"
#include "PPA/image_loader.h"
#include "utils/common.h"
#include "utils/matrix_operations.h"
#include "utils/sample_cube.h"

// Variables 
#define POSX 500
#define POSY 500 
#define BORDER 15 





// Graphics Engine Class
class W3DGraphics {
    private:    
        // Window Properties & Window 
        int window_width=0 , window_height=0; 
        Window win;

        // Matrice
        Matrix4x4 projectionMatrix;
        
        // Vars 
        float rotationAngle = 0.f;
        // Setup Window
        void setupWindow(){
            // Create connection to display and get root window  
            dpy = XOpenDisplay(NULL);
            if (dpy == NULL)
                err(1,"Can't open display");

            scr = DefaultScreen(dpy);
            root = RootWindow(dpy,scr);

            // Create connection to display and get root window  
            dpy = XOpenDisplay(NULL);
            if (dpy == NULL)
                err(1,"Can't open display");

            scr = DefaultScreen(dpy);
            root = RootWindow(dpy,scr);

            // Window 
            this->win = create_win(POSX,POSY,800,800,BORDER);

            // Name Window 
            XStoreName(dpy,win,"Wapazock 3D Graphics");
            XMapWindow(dpy,win);

            // onWindowReady
            XEvent ev;
            XNextEvent(dpy,&ev);

            // Window ready 
            XFlush(dpy);

            this->onWindowReady();
            
            // onWindow Update
            bool close = false;
            float count = 1 ;

            GC white = simple_gc(_RGB(255,255,255));
            // Time start 
            std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
            while (!close)
            {   
                std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
                int seconds_passed = std::chrono::duration_cast<std::chrono::seconds> (end - begin).count();
                XClearWindow(dpy,this->win);
                count++;
                string text = "FPS " + std::to_string(count/seconds_passed);
                char const *tchar = text.c_str();
                XDrawString(dpy,this->win,white,200, 20,tchar,strlen(tchar));

                //XClearWindow(dpy,this->win);
                this->onWindowUpdate();
                
                XFlush(dpy);
            }
            

            // Cleanup 
            cleanUpWindow(win);
            
        }

        // Given two vectors an the y, returns the x 
        float lineEquationFindX(Vect3d line1, Vect3d line2,float y){
            float m = (line1.y - line2.y) / (line1.x - line2.x);
            return y/m;
        }

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

                    // Draw point
                    RGB pixel = this->texture.getPixelAt(this->texture.getHeight() * yNorm, this->texture.getWidth() * xNorm);
                    float depth = 150.0f;
                    pixel.r *= x/depth;
                    pixel.g *= x/depth;
                    pixel.b *= x/depth;
                    XSetForeground(dpy,this->gcx,_RGB(pixel.r,pixel.g,pixel.b));
                    XDrawPoint(dpy,this->win,this->gcx,x,y);

                    // Increment x 
                    x += 1;
                }

                // increment y
                y += 1 ;
            }

        }
    
        // Draw triangle
        void drawTriangle(Triangle tri){
            //List of lines
            XDrawLine(dpy,this->win,this->gcx,tri.p[0].x,tri.p[0].y,tri.p[1].x,tri.p[1].y);
            XDrawLine(dpy,this->win,this->gcx,tri.p[1].x,tri.p[1].y,tri.p[2].x,tri.p[2].y);
            XDrawLine(dpy,this->win,this->gcx,tri.p[2].x,tri.p[2].y,tri.p[0].x,tri.p[0].y);
            XFlush(dpy);
        }

        // Draws the texture ppm 
        void drawTexture(){
            //Draw Texture 
            for (int r=0; r != this->texture.getHeight(); ++r){
                for (int c=0; c != this->texture.getWidth(); ++c){
                    // Get pixel color 
                    RGB pixelColor = this->texture.getPixelAt(r,c);
                    long color = _RGB(pixelColor.r,pixelColor.g,pixelColor.b);
                    XSetForeground(dpy,gcx,color);

                    // Draw image 
                    XDrawPoint(dpy,this->win,gcx,r,c);
                }   
            } 
        }

    public:
        // GC in edit
        GC gcx;
        // Texture Holder 
        Image texture; 
        // Objects List
        vector<MeshPos> _3DObjects;
        
        // Constructor 
        W3DGraphics(int windowWidth, int windowHeight){
            //Set window width and height
            this->window_height = windowHeight;
            window_width = windowHeight; 
        }

        // Initializer 
        void init(){
            // Create Projection Matrix 
            this->projectionMatrix = Make_Projection(
                this->window_height/this->window_width,
                90.f,
                1000.f,
                0.1f);

            // Setup Window 
            this->setupWindow();
        }
        // Adds object to the scene
        void add3ObjectToScene(MeshPos &object){
            this->_3DObjects.push_back(object);
        }

        // onWindowReady
        void onWindowReady(){
            // Create the gc 
            this->gcx = simple_gc(_RGB(255,0,0));
            // Load the texture 
            this->texture = readPPM("./assets/ppm/wood.ppm");
        }

        double posx = 0;
       
        // onWindowUpdate// Draw the triangle
            Triangle tri =  { 20.0f, 20.0f, 0.0f,   180.0f, 180.0f, 0.0f,    20.0f, 180.0f, 0.0f };
            
        void onWindowUpdate(){

            // Draw the triangle
            Triangle tri;
            tri.p[0] = {20.f,20.f,0.f};
            tri.p[1] = {180.f,180.f,0.f};
            tri.p[2] = {20.f,180.f,0.f};

            XDrawPoint(dpy,this->win,this->gcx,10,10);
            this->textureTriangle(tri);
            XFlush(dpy);
        }
};


// Program Start
int main(){
    // Engine instance
    W3DGraphics graphicsEngine = W3DGraphics(500,500);
    // Add cube to the scene 
    MeshPos cube;
    cube.mesh = getCube();
    graphicsEngine.add3ObjectToScene(cube);
    graphicsEngine.init();

    return 0 ;

}