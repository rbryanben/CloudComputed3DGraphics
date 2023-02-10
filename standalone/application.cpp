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
#include "utils/lights/directional_light.h"
#include "utils/text/writer.h"


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

            
            // original points
            float t_dy1 = t_y2 - t_y1;
            float t_dx1 = t_x2 - t_x1;
            float t_dz1 = t_z2 - t_z1;

            float t_dy2 = t_y3 - t_y1;
            float t_dx2 = t_x3 - t_x1;
            float t_dz2 = t_z3 - t_z1; 

        
            // Calculate dx_1 and dx_2 
            float dax_step = 0, t_dax_step = 0,t_day_step = 0, t_dby_step= 0,t_dbx_step =0,t_daz_step = 0,t_dbz_step = 0,dbx_step = 0, dw1_step= 0, dw2_step = 0,
                du1_step = 0, dv1_step = 0,
                du2_step =0, dv2_step = 0 ;

            float tex_u,tex_v, tex_w;


            // Calculate differentials 
            if (dy1) dax_step = dx1 / (float)abs(dy1); //float absolutes 
            if (dy2) dbx_step = dx2 / (float)abs(dy2); 

            if (dy1) t_dax_step = t_dx1 / (float)abs(dy1);
            if (dy2) t_dbx_step = t_dx2 / (float)abs(dy2);

            if (dy1) t_day_step = t_dy1 / (float)abs(dy1);
            if (dy2) t_dby_step = t_dy2 / (float)abs(dy2);

            if (dy1) t_daz_step = t_dz1 / (float)abs(dy1);
            if (dy2) t_dbz_step = t_dz2/(float)abs(dy2);

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

                    float t_ax = t_x1 + (float)(i - y1) * t_dax_step;
                    float t_bx = t_x1 + (float)(i - y1) * t_dbx_step;

                    float t_ay = t_y1 + (float)(i - y1) * t_day_step;
                    float t_by = t_y1 + (float)(i - y1) * t_dby_step;

                    float t_az = t_z1 + (float)(i - y1) * t_daz_step;
                    float t_bz = t_z1 + (float)(i - y1) * t_dbz_step;


                    float tex_su = u1 + (float)(i - y1) * du1_step;
                    float tex_sv = v1 + (float)(i - y1) * dv1_step;
                    float tex_sw = w1 + (float)(i - y1) * dw1_step;
                   

                    float tex_eu = u1 + (float)(i - y1) * du2_step;
                    float tex_ev = v1 + (float)(i - y1) * dv2_step;
                    float tex_ew = w1 + (float)(i - y1) * dw2_step;
                    

                    // Sort the x axis
                    if (ax > bx){
                        swap(ax,bx);
                        swap(tex_su,tex_eu);
                        swap(tex_sv,tex_ev);
                        swap(tex_sw,tex_ew);

                        // Swap the original co-ordinates
                        swap(t_ax,t_bx);
                        swap(t_az,t_bz);
                        swap(t_ay,t_by);
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

                        float tx = (1.0f - t) * t_ax + t * t_bx;
                        float tz = (1.0f - t) * t_az + t * t_bz;
                        float ty = (1.0f - t) * t_ay + t * t_by;

                        Vect3d pointInWorldSpace = {tx,ty,tz,1};
                       
                
                        // Pixel Color 
                        RGB pixel = tri.texture->getPixelAt((tex_v/tex_w) * (tri.texture->getHeight() - 1),(tex_u/tex_w) * (tri.texture->getWidth() - 1));
                        
                        // Dark Pixel 
                        glColor3f(pixel.r * 0.2f,pixel.g * 0.2f,pixel.b * 0.2f);

                        if (this->light.isPointVisible(pointInWorldSpace)){
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

            // original points 
            t_dx1 = t_x3 - t_x2;
            t_dy1 = t_y3 - t_y2;
            t_dz1 = t_z3 - t_z2;
   
            dv1 = v3 - v2;
            du1 = u3 - u2;
            dw1 = w3 - w2;
            
            if (dy1) dax_step = dx1 / (float)abs(dy1);
            if (dy1) t_dax_step = t_dx1 / (float)abs(dy1);
            if (dy1) t_day_step = t_dy1 / (float)abs(dy1);
            if (dy1) t_daz_step = t_dz1 / (float)abs(dy1);
        
            if (dy2) dbx_step = dx2 / (float)abs(dy2);
            if (dy2) t_dbx_step = t_dx2 / (float)abs(dy2); 
            if (dy2) t_dby_step = t_dy2 / (float)abs(dy2);
            if (dy2) t_dbz_step = t_dz2 / (float)abs(dy2);

            du1_step = 0, dv1_step = 0;
            if (dy1) du1_step = du1 / (float)abs(dy1);
            if (dy1) dv1_step = dv1 / (float)abs(dy1);
            if (dy1) dw1_step = dw1 / (float)abs(dy1);

            // Draw the bttom half  
            if (dy1)
            {
                for (int i = y2; i <= y3; i++)
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

                    float t_ax = t_x2 + (float)(i - y2) * t_dax_step;
                    float t_bx = t_x1 + (float)(i - y1) * t_dbx_step;

                    float t_ay = t_y2 + (float)(i - y2) * t_day_step;
                    float t_by = t_y1 + (float)(i - y1) * t_dby_step;

                    float t_az = t_z2 + (float)(i - y2) * t_daz_step;
                    float t_bz = t_z1 + (float)(i - y1) * t_dbz_step;


                    if (ax > bx)
                    {
                        swap(ax, bx);
                        swap(tex_su, tex_eu);
                        swap(tex_sv, tex_ev);
                        swap(tex_sw, tex_ew);

                        // Swap the original co-ordinates
                        swap(t_ax,t_bx);
                        swap(t_az,t_bz);
                        swap(t_ay,t_by);

                    }

                    tex_u = tex_su;
                    tex_v = tex_sv;
                    tex_w = tex_sw;


                    float tstep = 1.0f / ((float)(bx - ax));
                    float t = 0.0f;

                    for (int j = ax; j <= bx; j++)
                    {
                        tex_u = (1.0f - t) * tex_su + t * tex_eu;
                        tex_v = (1.0f - t) * tex_sv + t * tex_ev;
                        tex_w = (1.0f - t) * tex_sw + t * tex_ew;

                        float tx = (1.0f - t) * t_ax + t * t_bx;
                        float tz = (1.0f - t) * t_az + t * t_bz;
                        float ty = (1.0f - t) * t_ay + t * t_by;

                        Vect3d pointInWorldSpace = {tx,ty,tz,1};

                        if (abs(pointInWorldSpace.x) > 1 || abs(pointInWorldSpace.y) > 1 || abs(pointInWorldSpace.z) > 1){
                            int a = 2 ;
                            // Break;
                       }


                        RGB pixel = tri.texture->getPixelAt((tex_v/tex_w) * (tri.texture->getHeight() - 1),(tex_u/tex_w) * (tri.texture->getWidth() - 1));
                        
                        glColor3f(pixel.r * 0.2,pixel.g * 0.2,pixel.b * 0.2);

                        if (this->light.isPointVisible(pointInWorldSpace)){
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
        }

        // Render to depth buffer 
        float* dLightDepthBuffer; 
        
        // Draws the texture ppm 
        void drawTexture(Image texture){
            //GL Begin
            glBegin(GL_POINTS);
                //Draw Texture 
                for (int r=0; r != texture.getHeight(); ++r){
                    for (int c=0; c != texture.getWidth(); ++c){\                        
                        // Get pixel color 
                        RGB pixelColor = texture.getPixelAt(r,c);
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
        // Objects List
        std::map<string,Mesh> meshes;

        // Time
        std::chrono::steady_clock::time_point timeBegin;
        long framesRendered = 0;

        // W3Camera
        W3Camera sceneCamera; 
        
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
            this->sceneCamera = W3Camera({0,0,-4,1});
            /// light
            light = W3DirectionalLight({8000,8000},90.f,1000.f,0.1f);
            light.translate({0,5,-8,1});

            // Start timing
            timeBegin = std::chrono::steady_clock::now();
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

        // Light sources
        W3DirectionalLight light;

        // Text Writter Protoype
        W3Writer writer;

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
            
            // Light 
            light.render(combinedMeshesTriangles,true);

            //draw triangles - scene is combined from this point 
            for (Triangle tri : combinedMeshesTriangles){
                Triangle original = tri; // The original triangle
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
                    
                    //
                    // Camera - From here the entire view is inversed to the camera's view 
                    Triangle triViewd; 
                    MultiplyMatrixVector(tri.p[0],triViewd.p[0],matViewd);
                    MultiplyMatrixVector(tri.p[1],triViewd.p[1],matViewd);
                    MultiplyMatrixVector(tri.p[2],triViewd.p[2],matViewd);

                    triViewd.copyTextureFrom(tri);

                    // Clip the triangles against the axis
                    TrianglesHolder clippedZ = clipTriangleAgainstPlane({0,0,0.1f,1},{0,0,1,1},triViewd,original); 

                    for (int i = 0; i != clippedZ.projected.size(); i++){
                        Triangle tri = clippedZ.projected[i];
                        Triangle og = clippedZ.original[i]; 

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
                        triangleReferences.push_back(og);
                        // Add to vertex_list 
                        projectedTriangles.push_back(triProjected);
                    }
                }
            }

            // Triangle clipped against screen edges 
            //
            //vector<Triangle> clippedTrianglesList;
            //// Clip against the top of the screen
            TrianglesHolder topClipped = clipTriangleAgainstPlane({0,0.1,0,1},{0,1,0,1},projectedTriangles,triangleReferences);
            ////// Clip against bottom of the screen
            TrianglesHolder bottomClipped =  clipTriangleAgainstPlane({0,(float)this->window_height - 1,0,1},{0,-1,0},topClipped.projected,topClipped.original);
            //////Clip against
            TrianglesHolder leftClipped = clipTriangleAgainstPlane({1,0,0,1},{1,0,0,1},bottomClipped.projected,bottomClipped.original);
            ////// Clip against right of the screen
            TrianglesHolder rightClipped = clipTriangleAgainstPlane({(float)this->window_width - 1,0,0,1},{-1,0,0,1},leftClipped.projected,leftClipped.original);
            
            // Clear depth buffer 
            this->clearDepthBuffer();

            // Render the triangles
            for (int i= 0; i != rightClipped.projected.size(); i ++){
                this->texturedTriangle(rightClipped.projected[i],rightClipped.original[i]);
            }    

            // Update frames 
            this->framesRendered++; 
            std::chrono::steady_clock::time_point timeNow = std::chrono::steady_clock::now();
            long seconds = std::chrono::duration_cast<std::chrono::seconds>(timeNow - timeBegin).count();
            float frameRate = 0;
            if (seconds > 1) frameRate = (float)this->framesRendered / (float)seconds;

            // FPS
            string fps = "FRAMES PER SECOND " + std::to_string(frameRate);
            writer.print(20,20,fps,9);   
            // SERVER
            writer.print(20,35,"SERVER NA",9); 
            // SERVER RESPONSE 
            writer.print(20,50,"RESPONSE NA",9);
            // POLYGON COUNT
            string polygons = "POLYGONS " + to_string(combinedMeshesTriangles.size());
            writer.print(20,65,polygons,9);
            // RAY TRACING   
            writer.print(20,80,"RAY TRACING OFF",9);                     
        }
        
};

// Test With UI
int main(int argc, char **argv)
{
    // Engine instance
    W3DGraphics graphicsEngine = W3DGraphics(800,750);

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
    graphicsEngine.addToScene(basement);
    

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






