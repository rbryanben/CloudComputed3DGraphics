#include "GL/freeglut.h"
#include "GL/gl.h"
#include <string.h>
#include <fstream>
#include <vector>
#include <iostream>
#include <stdlib.h>
#include <err.h>
#include <chrono>
#include <strstream>
#include <map>


// Libs
#include "utils/image/image_loader.h"
#include "utils/common/common.h"
#include "utils/common/matrix_operations.h"
#include "utils/camera/camera.h"


// Variables 
#define POSX 500
#define POSY 500 
#define BORDER 15 


// Pre Init
class W3DGraphics;
static W3DGraphics* instance;

// Graphics Engine Class
class W3DGraphics {
    private:    
        // Window Properties & Window 
        int window_width=0 , window_height=0; 
        
        // Matrice
        Matrix4x4 projectionMatrix;

        // Vars 
        float rotationAngle = 22.f/7.f;

        // Depth buffer 
        float* depthBuffer;

        // Textured Triangle
        void texturedTriangle(Triangle tri,Triangle orignal_triangle){
            // GL_BEGIN
            glBegin(GL_POINTS);
            glColor3f(1,1,0);
            
            // Points vertex and texture 
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


            // Depth valuew s 
            float w1 = tri.t[0].w ;
            float w2 = tri.t[1].w ;
            float w3 = tri.t[2].w ;

            // Original points 
            float t_x1 = orignal_triangle.p[0].x;
            float t_y1 = orignal_triangle.p[0].y;
            float t_z1 = orignal_triangle.p[0].z;
           
            float t_x2 = orignal_triangle.p[1].x;
            float t_y2 = orignal_triangle.p[1].y;
            float t_z2 = orignal_triangle.p[1].z;

            float t_x3 = orignal_triangle.p[2].x;
            float t_y3 = orignal_triangle.p[2].y;
            float t_z3 = orignal_triangle.p[2].z;            

            // Sort Points 
            if (y2 < y1){
                swap(y2,y1);
                swap(x2,x1);
                swap(u2,u1);
                swap(v2,v1);
                swap(w2,w1);

                // Swap original points 
                swap(t_y2,t_y1);
                swap(t_x2,t_x1);
                swap(t_z2,t_z1);
            }

            if (y3 < y1){
                swap(y3,y1);
                swap(x3,x1);
                swap(u3,u1);
                swap(v3,v1);
                swap(w3,w1);

                // Swap original points
                swap(t_y3,t_y1);
                swap(t_x3,t_x1);
                swap(t_z3,t_z1);
            }

            if (y3 < y2){
                swap(y3,y2);
                swap(x3,x2);
                swap(u3,u2);
                swap(v3,v2);
                swap(w3,w2);

                // Swap original points
                swap(t_y3,t_y2);
                swap(t_x3,t_x2);
                swap(t_z3,t_z2);
            }

            // Args 
            int dy1 = y2 - y1 ;
            int dx1 = x2 - x1 ;
            float dv1 = v2 - v1;
            float du1 = u2 - u1;
            float dw1 = w2 - w1 ;

            int dy2 = y3 - y1;
            int dx2 = x3 - x1;
            float dv2 = v3 - v1;
            float du2 = u3 - u1;
            float dw2 = w3 - w1;


            // Calculate dx_1 and dx_2 
            float dax_step = 0, dbx_step = 0, dw1_step= 0, dw2_step = 0,
                du1_step = 0, dv1_step = 0,
                du2_step =0, dv2_step = 0 ;

            float tex_u,tex_v, tex_w;


            // Calculate differentials 
            if (dy1) dax_step = dx1 / (float)abs(dy1); //float absolutes 
            if (dy2) dbx_step = dx2 / (float)abs(dy2); 

            if (dy1) dv1_step = dv1/  (float)abs(dy1);
            if (dy1) du1_step = du1/ (float)abs(dy1);
            if (dy1) dw1_step = dw1 / (float)abs(dy1);


            if (dy2) dv2_step = dv2/ (float)abs(dy2);
            if (dy2) du2_step = du2/ (float)abs(dy2);
            if (dy2) dw2_step = dw2/ (float)abs(dy2);

            // Draw top half 
            if (dy1){
                // Move y 
                for (int i = y1; i < y2;i++){
                    // Calculate the starting and ending x 
                    int ax = x1 + (float)(i - y1) * dax_step;
                    int bx = x1 + (float)(i - y1) * dbx_step;

                    float tex_su = u1 + (float)(i - y1) * du1_step;
                    float tex_sv = v1 + (float)(i - y1) * dv1_step;
                    float tex_sw = w1 + (float)(i - y1) * dw1_step;

                    float tex_eu = u1 + (float)(i - y1) * du2_step;
                    float tex_ev = v1 + (float)(i - y1) * dv2_step;
                    float tex_ew = w1 + (float)(i - y1) * dw2_step;
                    

                    // Starting original x and y
                    float relativeY = 0; 
                    float relative_starting_x = 0;
                    float relative_ending_x = 0;
                    float relative_starting_w = 0, relative_ending_w = 0; 

                    if (y2 - y1) relativeY = (float)(i - y1 ) / (y3 - y1); 
                    if (x2 - x1) relative_starting_x = (float) (ax - x1) / (x2 - x1);
                    if (x3 - x1) relative_ending_x = (float)(bx - x1) / (x3 - x1);
                    if (w2 - w1) relative_starting_w = (float)(tex_sw - w1)/(w2 - w1);
                    if (w3 - w1) relative_ending_w = (float)(tex_ew - w1)/ (w3 - w1);

                    float t_y = t_y1 + (relativeY * (t_y3 - t_y1)); 
                    float t_ax = t_x1 + (relative_starting_x * (t_x2 - t_x1));
                    float t_bx = t_x1 + (relative_ending_x * (t_x3 - t_x1));
                    float t_az = t_z1 + (relative_starting_w * (t_z2 - t_z1));
                    float t_bz = t_z1 + (relative_ending_w * (t_z3 - t_z1));


                    // Sort the x axis
                    if (ax > bx){
                        swap(ax,bx);
                        swap(tex_su,tex_eu);
                        swap(tex_sv,tex_ev);
                        swap(tex_sw,tex_ew);

                        // Swap the original co-ordinates
                        swap(t_ax,t_bx);
                        swap(t_az,t_bz);
                    }

                    tex_u = tex_su;
                    tex_v = tex_sv;
                    tex_w = tex_sw;

                    float tstep = 1.0f / ((float)(bx - ax));
                    float t = 0.0f;

                    for (int j = ax; j <= bx; j++){
                        tex_u = (1.0f - t) * tex_su + t * tex_eu;
                        tex_v = (1.0f - t) * tex_sv + t * tex_ev;
                        tex_w = (1.0f - t) * tex_sw + t * tex_ew;

                        // original x, y and z and put it into a point in world space  
                        float ref_x =  (1.0f - t) * t_ax + t * t_bx;
                        float ref_z =  (1.0f - t) * t_az + t * t_bz;
                        Vect3d pointInWorldSpace = {ref_x,t_y,ref_z,1};
                       
                        RGB pixel = tri.texture->getPixelAt((tex_v/tex_w) * (tri.texture->getHeight() - 1),(tex_u/tex_w) * (tri.texture->getWidth() - 1));
                        glColor3f(pixel.r * 0.2f,pixel.g * 0.2f,pixel.b * 0.2f);

                        // Check if the point is visible from the light source
                        // Prototype Light Buffer 
                        Vect3d vOffsetView = {1,1,0};
                        Matrix4x4 lightViewd = dLight.get4x4MatrixInverse();
                        Vect3d pointViewd,pointProjected;
  

                        MultiplyMatrixVector(pointInWorldSpace,pointViewd,lightViewd);

                        //
                        // Project the polygons(Some normalization takes place here)
                        //      At this point we project the 3D co-ordinates on to a 3D scene
                        float w0 = MultiplyMatrixVector(pointViewd,pointProjected,this->projectionMatrix);
                        float depth = 1.f / w0;

                        // Offset to the center
                        pointProjected = pointProjected + vOffsetView;
                        
                        // Scale to the window width  
                        //  this centeres the triangle
                        pointProjected = pointProjected * (0.5f * this->window_width);
                        int pos = ((int)pointProjected.y * this->window_width) + (int)pointProjected.x;
                        float depthInDlightDepthBuffer = dLightDepthBuffer[pos];
        

                        // If  x or why is negative then dont considerer 
                        bool lightBufferVisible = true;
                        if (pointProjected.x < 0 || pointProjected.y < 0 || pointProjected.x > this->window_width || pointProjected.y > this->window_height) lightBufferVisible = false;
                    
                        if (lightBufferVisible && areEqual(depthInDlightDepthBuffer,depth,0.002f)){
                            glColor3f(pixel.r,pixel.g,pixel.b);
                        }
                   
                  
                        if ( i >= 0 && j >= 0 && i * this->window_width + j < this->window_height * this->window_width 
                            && tex_w > depthBuffer[i * this->window_width + j]){
                            glVertex2i(j,i);
                            depthBuffer[i * this->window_width + j] = tex_w;
                        }

                        t += tstep;
                    }

                }
            }

            // Draw the bottom half

            //  Arguments 
            dy1 = y3 - y2;
            dx1 = x3 - x2;
            dv1 = v3 - v2;
            du1 = u3 - u2;
            dw1 = w3 - w2;
           

            if (dy1) dax_step = dx1 / (float)abs(dy1);
            if (dy2) dbx_step = dx2 / (float)abs(dy2);

            du1_step = 0, dv1_step = 0;
            if (dy1) du1_step = du1 / (float)abs(dy1);
            if (dy1) dv1_step = dv1 / (float)abs(dy1);
            if (dy1) dw1_step = dw1 / (float)abs(dy1);

            // Draw the bttom half  
            if (dy1)
            {
                for (int i = y2; i < y3; i++)
                {
                    // Calculate differentials 
                    int ax = x2 + (float)(i - y2) * dax_step;
                    int bx = x1 + (float)(i - y1) * dbx_step;

                    float tex_su = u2 + (float)(i - y2) * du1_step;
                    float tex_sv = v2 + (float)(i - y2) * dv1_step;
                    float tex_sw = w2 + (float)(i - y2) * dw1_step;

                    float tex_eu = u1 + (float)(i - y1) * du2_step;
                    float tex_ev = v1 + (float)(i - y1) * dv2_step;
                    float tex_ew = w1 + (float)(i - y1) * dw2_step;

                    // Starting original x and y 
                    float relativeY = 0; 
                    float relative_starting_x = 0;
                    float relative_ending_x = 0;
                    float relative_starting_w = 0, relative_ending_w = 0; 


                    if (y3 - y1) relativeY = (float)(i - y1) / (y3 - y1); 
                    if (x3 - x2) relative_starting_x = (float)(ax - x2) / (x3 - x2); 
                    if (x3 - x1) relative_ending_x = (float)(bx - x1) / (x3 - x1); // Remained the same from the top 
                    if (w3 - w2) relative_starting_w = (float)(tex_sw - w2)/(w3 - w2);
                    if (w3 - w1) relative_ending_w = (float)(tex_ew - w1) / (w3 - w1);

                    float t_y = t_y1 + (relativeY * (t_y3 - t_y1));
                    float t_ax = t_x2 + (relative_starting_x * (t_x3 - t_x2));
                    float t_bx = t_x1 + (relative_ending_x * (t_x3 - t_x1));
                    float t_az = t_z2 + (relative_starting_w * (t_z3 - t_z2));
                    float t_bz = t_z1 + (relative_ending_w * (t_z3 - t_z1));

                    if (ax > bx)
                    {
                        swap(ax, bx);
                        swap(tex_su, tex_eu);
                        swap(tex_sv, tex_ev);
                        swap(tex_sw, tex_ew);

                         // Swap the original co-ordinates
                        swap(t_ax,t_bx);
                        swap(t_az,t_bz);

                    }

                    tex_u = tex_su;
                    tex_v = tex_sv;
                    tex_w = tex_sw;


                    float tstep = 1.0f / ((float)(bx - ax));
                    float t = 0.0f;

                    for (int j = ax; j < bx; j++)
                    {
                        tex_u = (1.0f - t) * tex_su + t * tex_eu;
                        tex_v = (1.0f - t) * tex_sv + t * tex_ev;
                        tex_w = (1.0f - t) * tex_sw + t * tex_ew;

                        // original x, y and z and put it into a point in world space  
                        float ref_x =  (1.0f - t) * t_ax + t * t_bx;
                        float ref_z =  (1.0f - t) * t_az + t * t_bz;
                        Vect3d pointInWorldSpace = {ref_x,t_y,ref_z,1};


                        RGB pixel = tri.texture->getPixelAt((tex_v/tex_w) * (tri.texture->getHeight() - 1),(tex_u/tex_w) * (tri.texture->getWidth() - 1));
                        glColor3f(pixel.r * 0.2,pixel.g * 0.2,pixel.b * 0.2);

                        // Check if the point is visible from the light source
                        // Prototype Light Buffer 
                        Vect3d vOffsetView = {1,1,0};
                        Matrix4x4 lightViewd = dLight.get4x4MatrixInverse();
                        Vect3d pointViewd,pointProjected;
  

                        MultiplyMatrixVector(pointInWorldSpace,pointViewd,lightViewd);

                        //
                        // Project the polygons(Some normalization takes place here)
                        //      At this point we project the 3D co-ordinates on to a 3D scene
                        float w0 = MultiplyMatrixVector(pointViewd,pointProjected,this->projectionMatrix);
                        float depth = 1.f / w0;

                        // Offset to the center
                        pointProjected = pointProjected + vOffsetView;
                        
                        // Scale to the window width  
                        //  this centeres the triangle
                        pointProjected = pointProjected * (0.5f * this->window_width);
                        int pos = ((int)pointProjected.y * this->window_width) + (int)pointProjected.x;
                        float depthInDlightDepthBuffer = dLightDepthBuffer[pos];

          
        
                    
                        // If  x or why is negative then dont considerer 
                        bool lightBufferVisible = true;
                        if (pointProjected.x < 0 || pointProjected.y < 0 || pointProjected.x > this->window_width || pointProjected.y > this->window_height) lightBufferVisible = false;
                    
                        if (lightBufferVisible && areEqual(depthInDlightDepthBuffer,depth,0.002f)){
                            glColor3f(pixel.r,pixel.g,pixel.b);
                        }
                        
                        if (i >= 0 && j >= 0 && i * this->window_width + j < this->window_height * this->window_width 
                            && tex_w > depthBuffer[i * this->window_width + j]){
                            glVertex2i(j,i);
                            depthBuffer[i * this->window_width + j] = tex_w;
                        }

                        t += tstep;
                    }
                }	
            }		
            glEnd();
            //drawTriangleLines(tri);

        
        }
        

        // Render to depth buffer 
        float* dLightDepthBuffer; 
        // Textured Triangle
        void renderToDepthBuffer(Triangle tri){
            // GL_BEGIN
            glBegin(GL_POINTS);
            
            // Points 
            int x1 = tri.p[0].x;
            int y1 = tri.p[0].y;
            
            int x2 = tri.p[1].x;
            int y2 = tri.p[1].y; 

            int x3 = tri.p[2].x;
            int y3 = tri.p[2].y;

            float w1 = tri.t[0].w ;
            float w2 = tri.t[1].w ;
            float w3 = tri.t[2].w ;


            // Sort Points 
            if (y2 < y1){
                swap(y2,y1);
                swap(x2,x1);
                swap(w2,w1);
            }

            if (y3 < y1){
                swap(y3,y1);
                swap(x3,x1);
                swap(w3,w1);
            }

            if (y3 < y2){
                swap(y3,y2);
                swap(x3,x2);
                swap(w3,w2);
            }

            // Args 
            int dy1 = y2 - y1 ;
            int dx1 = x2 - x1 ;
            float dw1 = w2 - w1 ;

            int dy2 = y3 - y1;
            int dx2 = x3 - x1;
            float dw2 = w3 - w1;


            // Calculate dx_1 and dx_2 
            float dax_step = 0, dbx_step = 0, dw1_step= 0, dw2_step = 0,
                du1_step = 0, dv1_step = 0,
                du2_step =0, dv2_step = 0 ;

            float tex_u,tex_v, tex_w;


            // Calculate differentials 
            if (dy1) dax_step = dx1 / (float)abs(dy1); //float absolutes 
            if (dy2) dbx_step = dx2 / (float)abs(dy2); 
            if (dy1) dw1_step = dw1 / (float)abs(dy1);
            if (dy2) dw2_step = dw2/ (float)abs(dy2);

            // Draw top half 
            if (dy1){
                // Move y 
                for (int i = y1; i <= y2;i++){
                    int ax = x1 + (float)(i - y1) * dax_step;
                    int bx = x1 + (float)(i - y1) * dbx_step;
                    float tex_sw = w1 + (float)(i - y1) * dw1_step;
                    float tex_ew = w1 + (float)(i - y1) * dw2_step;

                    // Sort the x axis
                    if (ax > bx){
                        swap(ax,bx);
                        swap(tex_sw,tex_ew);
                    }

                    tex_w = tex_sw;

                    float tstep = 1.0f / ((float)(bx - ax));
                    float t = 0.0f;

                    for (int j = ax; j <= bx; j++){
                        tex_w = (1.0f - t) * tex_sw + t * tex_ew;
                        // Potential Bug here 
                        if ( i >= 0 && j >= 0 && i * this->window_width + j < this->window_height * this->window_width 
                            && tex_w > dLightDepthBuffer[i * this->window_width + j]){
                            glColor3f(tex_w,tex_w,tex_w);
                            glVertex2i(j,i);
                            dLightDepthBuffer[i * this->window_width + j] = tex_w;
                        }

                        t += tstep;
                    }

                }
            }

            dy1 = y3 - y2;
            dx1 = x3 - x2;
            dw1 = w3 - w2;
           

            if (dy1) dax_step = dx1 / (float)abs(dy1);
            if (dy2) dbx_step = dx2 / (float)abs(dy2);

            du1_step = 0, dv1_step = 0;
            if (dy1) dw1_step = dw1 / (float)abs(dy1);


            if (dy1)
            {
                for (int i = y2; i <= y3; i++)
                {
                    int ax = x2 + (float)(i - y2) * dax_step;
                    int bx = x1 + (float)(i - y1) * dbx_step;
                    float tex_sw = w2 + (float)(i - y2) * dw1_step;
                    float tex_ew = w1 + (float)(i - y1) * dw2_step;


                    if (ax > bx)
                    {
                        swap(ax, bx);
                        swap(tex_sw, tex_ew);

                    }

                    tex_w = tex_sw;


                    float tstep = 1.0f / ((float)(bx - ax));
                    float t = 0.0f;

                    for (int j = ax; j < bx; j++)
                    {
                        tex_w = (1.0f - t) * tex_sw + t * tex_ew;
                        
                        if (i >= 0 && j >= 0 && i * this->window_width + j < this->window_height * this->window_width 
                            && tex_w >  dLightDepthBuffer[i * this->window_width + j]){
                            glColor3f(tex_w,tex_w,tex_w);
                            glVertex2i(j,i);
                            dLightDepthBuffer[i * this->window_width + j] = tex_w;
                        }

                        t += tstep;
                    }
                }	
            }		
            glEnd();
            

        
        }
        // Draws the texture ppm 
        void drawTexture(){
            //GL Begin
            glBegin(GL_POINTS);
                //Draw Texture 
                for (int r=0; r != this->texture.getHeight(); ++r){
                    for (int c=0; c != this->texture.getWidth(); ++c){\                        
                        // Get pixel color 
                        RGB pixelColor = this->texture.getPixelAt(r,c);
                        glColor3f(pixelColor.r,pixelColor.g,pixelColor.b);
                        glVertex2i(c,r);
                    }   
                } 
            glEnd();
        }

        //Draw Triangles
        void drawTriangle(Triangle &tri){
            //glBegin
            glBegin(GL_TRIANGLES);
                glColor3f(tri.color.red,tri.color.green,tri.color.blue);
                glVertex2f(tri.p[0].x,tri.p[0].y);
                glVertex2f(tri.p[1].x,tri.p[1].y);
                glVertex2f(tri.p[2].x,tri.p[2].y);
                glVertex2f(tri.p[0].x,tri.p[0].y);
            glEnd();
        }

        // Draw Lines 
        void drawTriangleLines(Triangle &tri){
            glBegin(GL_LINES);
                glColor3f(1.f,0,0);
                glVertex2f(tri.p[0].x,tri.p[0].y);
                glVertex2f(tri.p[1].x,tri.p[1].y);
                glVertex2f(tri.p[1].x,tri.p[1].y);
                glVertex2f(tri.p[2].x,tri.p[2].y);
                glVertex2f(tri.p[2].x,tri.p[2].y);
                glVertex2f(tri.p[0].x,tri.p[0].y);
            glEnd();
        }
        
    public:
        
        // Texture Holder 
        Image texture; 
        // Objects List
        std::map<string,Mesh> meshes;

        // W3Camera
        W3Camera sceneCamera; 

        //illumination - basic 
        Vect3d lightSource = {0,0,-5};
        
        // Constructor 
        W3DGraphics(int windowWidth, int windowHeight){
            // Set the instance, which is used by opengl to refer to methods 
            instance = this;
            //Set window width and height
            this->window_height = windowHeight;
            window_width = windowHeight; 
            // Depth buffer 
            this->depthBuffer = new float[window_height * window_width];
        }

        // Initializer 
        void init(int &argc, char **&argv){
            // Setup Window 
            glutInit(&argc, argv);
            glutInitDisplayMode(GLUT_SINGLE);
            glutInitWindowSize(this->window_width, this->window_height);
            glutInitWindowPosition(100, 100);
            glutCreateWindow("W3Graphics Engine - Standalone");
            glutDisplayFunc(_EntryOnWindowReady);
            glutKeyboardFunc(_EntryKeyboard);
            glutSpecialFunc(_EntrySpecialKeys);
            glutIdleFunc(_EntryOnWindowUpdate);
            // Set Black Background of window
            glClearColor(0,0, 0, 1); 
            glClear(GL_COLOR_BUFFER_BIT);
            // Set Window Bounds
            glOrtho(0.0,this->window_width,this->window_height,0.0,0,1000);
            glutMainLoop();    
        }

        // Add Mesh to scene and returns a reference to the mesh
        Mesh* addToScene(Mesh &mesh){
            this->meshes[mesh.name] = mesh;
            return &meshes[mesh.name];
        }
        
        //Entry Window Ready 
        static void _EntryOnWindowReady(){
            //Call instance window ready
            instance->onWindowReady();
        }

        // Entry Window Update
        static void _EntryOnWindowUpdate(){
            // Clear Screen
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
                // Update Display
                instance->onWindowUpdate();
            // Flush Screen
            glFlush();
        }
        
        // Entry Keyboard
        static void _EntryKeyboard(unsigned char key, int x, int y){
            instance->onUserInput(key,x,y);
        }

        static void _EntrySpecialKeys( int key, int x, int y )
        {
            switch( key )
            {
                case GLUT_KEY_UP:
                    
                    break;
        
                case GLUT_KEY_DOWN:
                    // handle down arrow
                    break;
        
                case GLUT_KEY_LEFT:
                    instance->sceneCamera.moveLeft(0.2f);
                    // handle left arrow
                    break;
        
                case GLUT_KEY_RIGHT:
                    // handle right arrow
                    instance->sceneCamera.moveRight(0.2f);
                    break;
            }
        }

        // onWindowReady
        void onWindowReady(){
            //Setup projection matrix
            this->projectionMatrix = Make_Projection(
                this->window_height/this->window_width,
                90.0f,
                1000.f,
                0.1f);
            
            // Configure camera
            this->sceneCamera = W3Camera({0,0,-5,1});

            // Directional light prototype - translate light 
            this->dLightDepthBuffer = new float[this->window_width * this->window_height];
            this->dLight.setTranslation({0,0,-5});
        }
        
        // onUserInput
        void onUserInput(unsigned char key,int mouseX,int mouseY){
            switch (key)
            {
                //w - move foward
                case 'w':
                    this->sceneCamera.moveFoward(3);
                    break;
                //s - move back
                case 's':
                    this->sceneCamera.moveBack();
                    break;
                //a - rotate right
                case 'a':
                    this->sceneCamera.rotateY(-0.02);
                    break;
                //a - rotate left
                case 'd':
                    this->sceneCamera.rotateY(0.02);
                    break;
                case 'o':
                    this->sceneCamera.moveUp(0.2);
                    break;
                case 'l':
                    this->sceneCamera.moveUp(-0.2);
                    break;
                case '[':
                    this->lightSource.x -= 5 ;
                    break;
                case ']':
                    this->lightSource.x += 5 ;
                    break;
                case 'p':
                    this->lightSource.z += 5 ;
                    break;
                case 't':
                    break;
                case ';':
                    this->lightSource.z -= 5 ;
                    break;

                // Dlight keys 
                //y - move foward
                case 'y':
                    this->dLight.moveFoward(3);
                    break;
                //h - move back
                case 'h':
                    this->dLight.moveBack();
                    break;
                //g - rotate right
                case 'g':
                    this->dLight.rotateY(-0.02);
                    break;
                //j - rotate left
                case 'j':
                    this->dLight.rotateY(0.02);
                    break;
                //k move left 
                case 'k':
                    this->dLight.moveLeft(0.5);
                    break;
                      
                default:
                    break;
            }
        }

        // Clears the depth buffer 
        void clearDepthBuffer(){
            // Clear depth buffer 
            for (u_int i = 0; i != this->window_height * window_width; i++){
                this->depthBuffer[i] = 0 ; 
            }
        }
       
        // Directional light protoype 
        W3Camera dLight = W3Camera(0.5f);

        //Called every time the window updates     
        void onWindowUpdate(){
            // Final projected colors and triangles 
            vector<Triangle> projectedTriangles;
            vector<Triangle> triangleReferences; 
        
            // Camera Matrix - this is here so that we do not retrieve it on every render triangle
            Matrix4x4 matViewd = this->sceneCamera.get4x4MatrixInverse(); 
        
            //Offset to the center of the screen 
            Vect3d vOffsetView = {1,1,0};

            // Combined objects list 
            vector<Triangle> combinedMeshesTriangles;

            // Combine meshes 
            for (auto i= this->meshes.begin(); i != this->meshes.end(); ++i){
                Mesh mesh = i->second;
                for (Triangle tri: mesh.triangles){
                    Triangle triTransformed;;
                    
                    // Apply geometric transformation to each triangle
                    triTransformed.p[0] = MatrixMultiplyVector(mesh.geometryMatrix,tri.p[0]);
                    triTransformed.p[1] = MatrixMultiplyVector(mesh.geometryMatrix,tri.p[1]);
                    triTransformed.p[2] = MatrixMultiplyVector(mesh.geometryMatrix,tri.p[2]);
                    
                    //Copy texture
                    triTransformed.copyTextureFrom(tri);
                    // Add triangle to the combined mesh triangles 
                    combinedMeshesTriangles.push_back(triTransformed); 
                    
                    
                }
            }

            // Prototype Light Buffer 
            Matrix4x4 lightViewd = dLight.get4x4MatrixInverse();
            vector<Triangle> lightProjected;

            for (Triangle tri: combinedMeshesTriangles){
                // Determine the normal so that we do not draw triangles that are not facing us
                Vect3d normal,line1,line2 ;
                Triangle triProjected;
                line1 = tri.p[1] - tri.p[0];
                line2 = tri.p[2] - tri.p[0]; 
                normal = VectorCrossProduct(line1,line2);
                NormalizeVector(normal);

    

                // Light ray to triangle 
                Vect3d vLightRay = tri.p[0] - this->dLight.getTranslation();

                if (VectorDotProduct(vLightRay,normal) < 0
                    ){
                    
                    //
                    // Camera - From here the entire view is inversed to the camera's view 
                    Triangle triViewd; 
                    MultiplyMatrixVector(tri.p[0],triViewd.p[0],lightViewd);
                    MultiplyMatrixVector(tri.p[1],triViewd.p[1],lightViewd);
                    MultiplyMatrixVector(tri.p[2],triViewd.p[2],lightViewd);

                    // Clip the triangles against the axis
                    vector<Triangle> clippedZ = clipTriangleAgainstPlane({0,0,0.1f,1},{0,0,1,1},triViewd); 

                    for (auto tri : clippedZ){  

                        //
                        // Project the polygons(Some normalization takes place here)
                        //      At this point we project the 3D co-ordinates on to a 3D scene
                        float w0 = MultiplyMatrixVector(tri.p[0],triProjected.p[0],this->projectionMatrix);
                        float w1 = MultiplyMatrixVector(tri.p[1],triProjected.p[1],this->projectionMatrix);
                        float w2 = MultiplyMatrixVector(tri.p[2],triProjected.p[2],this->projectionMatrix);

                        triProjected.t[0].w = 1.f / w0;
                        triProjected.t[1].w = 1.f / w1;
                        triProjected.t[2].w = 1.f / w2;  

                        // Offset to the center
                        triProjected.p[0] = triProjected.p[0] + vOffsetView;
                        triProjected.p[1] = triProjected.p[1] + vOffsetView;
                        triProjected.p[2] = triProjected.p[2] + vOffsetView;

                        // Scale to the window width  
                        //  this centeres the triangle
                        triProjected.p[0] = triProjected.p[0] * (0.5f * this->window_width);
                        triProjected.p[1] = triProjected.p[1] * (0.5f * this->window_width);
                        triProjected.p[2] = triProjected.p[2] * (0.5f * this->window_width);

                        // Add to vertex_list 
                        lightProjected.push_back(triProjected);
                    }
                }
        
            }
            
            // Clear depth buffer 
            for (int i=0;i != this->window_width * this->window_height; i++){
                dLightDepthBuffer[i]=0;
            }
            
            // Render 
            for (Triangle tri: lightProjected){
                renderToDepthBuffer(tri);
            }


            //draw triangles - scene is combined from this point 
            for (Triangle tri : combinedMeshesTriangles){
                Triangle original = tri;
                Triangle triProjected;
                Matrix4x4 matRotZ, matRotX;

                // Determine the normal so that we do not draw triangles that are not facing us
                Vect3d normal,line1,line2 ;
                line1 = tri.p[1] - tri.p[0];
                line2 = tri.p[2] - tri.p[0]; 
                normal = VectorCrossProduct(line1,line2);
                NormalizeVector(normal);


                // Camera Ray Which is the difference between triTransformed.p[0] and vCamera
                //  is the vector from the camera to the triangle. We wish to calculate the angle inbetween to see if its visible 
                Vect3d vCameraRay = tri.p[0] - this->sceneCamera.getTranslation();

                if (VectorDotProduct(vCameraRay,normal) < 0
                    ){
                    // Light source is placed in the scene space, before the camera
                    NormalizeVector(lightSource);
                    float dt = f_max(VectorDotProduct(normal,lightSource),0);
                    Color color = {f_max(abs(dt*1),0.2f),f_max(abs(dt*1),0.2f),f_max(abs(dt*1),0.2f)};
                
                    //
                    // Camera - From here the entire view is inversed to the camera's view 
                    Triangle triViewd; 
                    MultiplyMatrixVector(tri.p[0],triViewd.p[0],matViewd);
                    MultiplyMatrixVector(tri.p[1],triViewd.p[1],matViewd);
                    MultiplyMatrixVector(tri.p[2],triViewd.p[2],matViewd);

                    // Copy texture
                    tri.color = color;
                    triViewd.copyTextureFrom(tri);

                    // Clip the triangles against the axis
                    vector<Triangle> clippedZ = clipTriangleAgainstPlane({0,0,0.1f,1},{0,0,1,1},triViewd); 

                    for (Triangle tri : clippedZ){  
                        //
                        // Project the polygons(Some normalization takes place here)
                        //      At this point we project the 3D co-ordinates on to a 3D scene
                        float w0 = MultiplyMatrixVector(tri.p[0],triProjected.p[0],this->projectionMatrix);
                        float w1 = MultiplyMatrixVector(tri.p[1],triProjected.p[1],this->projectionMatrix);
                        float w2 = MultiplyMatrixVector(tri.p[2],triProjected.p[2],this->projectionMatrix);

                        // Copy the texture information to triProjected from triTransformed
                        triProjected.copyTextureFrom(tri);

                        // Also normalize u and v , as z gets larger u and v get smaller 
                        // This is to fix distortion
                        triProjected.t[0].u = triProjected.t[0].u / w0;
                        triProjected.t[1].u = triProjected.t[1].u / w1;
                        triProjected.t[2].u = triProjected.t[2].u / w2;

                        triProjected.t[0].v = triProjected.t[0].v / w0;
                        triProjected.t[1].v = triProjected.t[1].v / w1;
                        triProjected.t[2].v = triProjected.t[2].v / w2;  

                        triProjected.t[0].w = 1.f / w0;
                        triProjected.t[1].w = 1.f / w1;
                        triProjected.t[2].w = 1.f / w2;  

                        // Offset to the center
                        triProjected.p[0] = triProjected.p[0] + vOffsetView;
                        triProjected.p[1] = triProjected.p[1] + vOffsetView;
                        triProjected.p[2] = triProjected.p[2] + vOffsetView;

                        // Scale to the window width  
                        //  this centeres the triangle
                        triProjected.p[0] = triProjected.p[0] * (0.5f * this->window_width);
                        triProjected.p[1] = triProjected.p[1] * (0.5f * this->window_width);
                        triProjected.p[2] = triProjected.p[2] * (0.5f * this->window_width);


                        // Attach original triangle to the projected triangle
                        triangleReferences.push_back(original);
                        // Add to vertex_list 
                        projectedTriangles.push_back(triProjected);
                    }
                }
            }


            
            // Triangle clipped against screen edges 
            //
            //vector<Triangle> clippedTrianglesList;
            //// Clip against the top of the screen
            //vector<Triangle> topClipped = clipTriangleAgainstPlane({0,0.1,0,1},{0,1,0,1},projectedTriangles);
            ////// Clip against bottom of the screen
            //vector<Triangle> bottomClipped =  clipTriangleAgainstPlane({0,(float)this->window_height - 1,0,1},{0,-1,0},topClipped);
            //////Clip against
            //vector<Triangle> leftClipped = clipTriangleAgainstPlane({1,0,0,1},{1,0,0,1},bottomClipped);
            ////// Clip against right of the screen
            //vector<Triangle> rightClipped = clipTriangleAgainstPlane({(float)this->window_width - 1,0,0,1},{-1,0,0,1},leftClipped);
            
            // Clear depth buffer 
            this->clearDepthBuffer();

            // Render the triangles
            for (int i= 0; i != projectedTriangles.size(); i ++){
                this->texturedTriangle(projectedTriangles[i],triangleReferences[i]);
            }
            
            
        }
        
};

// Test With UI
int main(int argc, char **argv)
{
    // Engine instance
    W3DGraphics graphicsEngine = W3DGraphics(800,800);

    // Meshs 
    Mesh crate = Mesh("crate");
    crate.LoadFromObjectFile("./assets/objs/crate/Crate1.obj",readPPM("./assets/objs/crate/crate.ppm"));
    graphicsEngine.addToScene(crate);


    Mesh crate2 = Mesh("crate2");
    crate2.LoadFromObjectFile("./assets/objs/crate/Crate1.obj",readPPM("./assets/objs/crate/crate.ppm"));
    crate2.scale(5);
    crate2.translate({0,0,10});
    graphicsEngine.addToScene(crate2);

    // Basement Entry 
    Mesh basement = Mesh("basement");
    basement.LoadFromObjectFile("./assets/objs/basement_entry/basement_entry.obj",readPPM("./assets/objs/basement_entry/Image_9.ppm"));
    basement.rotateZ(180);
    basement.translate({0,0,5});
    //graphicsEngine.addToScene(basement);
    

    // Pathwalk 
    Mesh pathwalk = Mesh();
    pathwalk.LoadFromObjectFile("./assets/objs/pathwalk/pathwalk.obj",readPPM("./assets/objs/pathwalk/texture.ppm"));
    pathwalk.rotateZ(180);
    pathwalk.translate({0,0,-10});
    //graphicsEngine.addToScene(pathwalk);

    // Drum 
    Mesh drum = Mesh("drum");
    drum.LoadFromObjectFile("./assets/objs/drum/drum.obj",readPPM("./assets/objs/drum/texture.ppm"));
    drum.rotateZ(180);
    //graphicsEngine.addToScene(drum);

    // Initialize engine
    graphicsEngine.init(argc,argv);

    return 0;
}






