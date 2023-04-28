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
#include <CL/opencl.hpp>


// Libs
#include "utils/image/image_loader.h"
#include "utils/common/common.h"
#include "utils/common/matrix_operations.h"
#include "utils/camera/camera.h"
#include "utils/lights/directional_light.h"
#include "utils/text/writer.h"
#include "utils/opencl/opencl_helper.hpp"
#include "opencl_kernels/cl_common.hpp"

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

        // GPU core count 
        int gpuCoreCount;

        // Render to depth buffer 
        float* dLightDepthBuffer; 
        
        // OpenCl grid details 
        cl_GridDetails gridDetails;

        // Matrice
        Matrix4x4 projectionMatrix;

        // Depth buffer 
        float* depthBuffer;

        // Hold the number of triangles in the scene
        int trianglesInScene = 0;

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

        // Draw Lines 
        void drawTriangleLines(cl_Triangle &tri){
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
        // Maximum triangles rastered by a sub grid
        int subGridRasterMax =  30000;

        // Meshes in the scene
        std::map<string,Mesh> meshes;
        
        // OpenCL command queue 
        cl::CommandQueue clqueue;

        // OpenCL Kernels 
        cl::Kernel renderKernel;
        cl::Kernel textureKernel;
        
        // OpenCL context and device 
        cl::Context context;
        cl::Device device;
     
        // This is a list of all the textures in the scene 
        vector<Image> textureList;

        // Time
        std::chrono::steady_clock::time_point timeBegin;
        // Frames Rendered 
        long framesRendered = 0;

        // W3Camera
        W3Camera sceneCamera; 

        // Light sources
        W3DirectionalLight light;

        // Text Writter Protoype
        W3Writer writer;

        // Value to update when scene has  changed 
        bool changesToScene = true;

        // Combined objects list 
        vector<Triangle> combinedMeshesTriangles;

        // Program
        cl::Program gpuProgram;

        // Pixel Textures out
        cl_Pixel_Texture_Out* h_pixelTexturesOut;

        // Value determines if buffers have been created.
        // In the event new buffer sizes have to be assigned set to false 
        bool buffersCreated;

        // Value determines if the textures have been transfaered to the shader execution device
        bool texturesTransfared;

        // Vertex-Geometry Shader buffers 
        cl::Buffer buffer_triangles_in;
        cl::Buffer buffer_camera_matrix;
        cl::Buffer buffer_projection_matrix;
        cl::Buffer buffer_window_width;
        cl::Buffer buffer_window_height;
        
        // Rasterization Shader buffer
        cl::Buffer buffer_textures;
        cl::Buffer buffer_texture_maps;
        cl::Buffer buffer_frame;
        cl::Buffer buffer_depth;

        // Shared Buffers 
        cl::Buffer buffer_grid_details;
        cl::Buffer buffer_sub_grid_raster_max;
        cl::Buffer buffer_triangles_count;
        cl::Buffer buffer_triangles_out;
        cl::Buffer buffer_vertex_shader_to_rasterizer;

        // Frame Buffer
        cl_Pixel_Texture_Out* h_outTiles;
        
        // Constructor 
        W3DGraphics(int windowWidth, int windowHeight){
            // Set the instance, which is used by opengl to refer to methods 
            instance = this;
            //Set window width and height
            this->window_height = windowHeight;
            window_width = windowHeight; 
            // Depth buffer 
            this->depthBuffer = new float[window_height * window_width];

            // Setup OpenCL
            this->setupOpenCL();
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

        //Entry Window Ready 
        static void _EntryOnWindowReady(){
            //Call instance window ready
            instance->onWindowReady();
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

            // For debug purposes - entry camera location
            sceneCamera.cameraMatrix = {
                0.696707547f,0,-0.717356741f,0,
                0,1,0,0,
                0.717356741f,0,0.696707547f,0,
                -1.29124212f,-1.20000005f,-5.25407219f,1
            };

            // light Source
            light = W3DirectionalLight({8000,8000},90.f,1000.f,0.1f);
            light.translate({0,5,-8,1});

            // Start timing for frame rate
            timeBegin = std::chrono::steady_clock::now();
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

        // Entry Special Keys
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
        
        // Add Mesh to scene and returns a reference to the mesh
        Mesh* addToScene(Mesh &mesh){
            this->meshes[mesh.name] = mesh;

            // Set the texture number for the object in scene
            // We will need this value to reference the texture in the shader
            this->meshes[mesh.name].textureNumber = this->textureList.size();
            
            // Add the mesh texture to the texturesList
            // The textures list is going to be used to transfarer textures to the shader execution device
            this->textureList.push_back(mesh.texture);

            // Increment the triangles in the scene
            this->trianglesInScene += this->meshes[mesh.name].triangles.size();

            // Return a pointer to the mesh in the scene 
            return &meshes[mesh.name];
        }
        
        // Returns the length of all textures. Which is the sum of all textures * their resolutions
        int getLengthOfAllTextures(){
            int sum = 0;
            for (auto texture : this->textureList){
                sum += texture.getHeight() * texture.getWidth();
            }
            return sum;
        }

        // Clears the depth buffer 
        void clearDepthBuffer(){
            // Clear depth buffer 
            for (u_int i = 0; i != this->window_height * window_width; i++){
                this->depthBuffer[i] = 0 ; 
            }
        }

        
        // OpenCL setup 
        void setupOpenCL(){
            // Program
            this->gpuProgram = createClProgram("opencl_kernels/render.cl");
            
            // Create the queue 
            this->context = this->gpuProgram.getInfo<CL_PROGRAM_CONTEXT>();
            this->device =  this->context.getInfo<CL_CONTEXT_DEVICES>().front();
            cl::CommandQueue queue(this->context,this->device);
            this->clqueue = queue;

            // Set the GPU core count
            this->gpuCoreCount = this->device.getInfo<CL_DEVICE_MAX_COMPUTE_UNITS>();
        
            // Render Kernel 
            cl::Kernel renderKernel(this->gpuProgram,"render");
            this->renderKernel = renderKernel;
            
            // Texture Kernel 
            cl::Kernel textureKernel(this->gpuProgram,"texture");
            this->textureKernel = textureKernel;

            // Setup grid details 
            this->gridDetails = createGridDetails(this->window_width,this->window_height,500);    

            // Create the out frame 
            this->h_outTiles = new cl_Pixel_Texture_Out[this->window_width * this->window_height];
            
        }

        // Create buffers for OpenCL 
        void createOpenCLBuffers(){
            // For any errors 
            cl_int err;

            // Vertex-Geometry shader buffers 
            this->buffer_triangles_in = clCreateBuffer(this->context(),CL_MEM_READ_WRITE,sizeof(cl_Triangle)* this->trianglesInScene,NULL,&err);
            this->buffer_camera_matrix = clCreateBuffer(this->context(),CL_MEM_READ_WRITE,sizeof(cl_Matrix4x4),NULL,&err);
            this->buffer_projection_matrix = clCreateBuffer(this->context(),CL_MEM_READ_ONLY,sizeof(cl_Matrix4x4),NULL,&err);
            this->buffer_triangles_count = clCreateBuffer(this->context(),CL_MEM_READ_WRITE,sizeof(int),NULL,&err);

            // Rasterazation shader buffers 
            int textureSize = this->getLengthOfAllTextures();
            this->buffer_textures = clCreateBuffer(this->context(),CL_MEM_READ_ONLY,sizeof(RGB) * this->getLengthOfAllTextures(),NULL,&err);
            this->buffer_texture_maps = clCreateBuffer(this->context(),CL_MEM_READ_ONLY,sizeof(cl_TextureDetail) * this->textureList.size(),NULL,&err);
            this->buffer_depth = clCreateBuffer(this->context(),CL_MEM_READ_WRITE,sizeof(float) * this->window_width * this->window_height,NULL,&err);

            // Shared Buffers
            this->buffer_window_width = clCreateBuffer(this->context(),CL_MEM_READ_ONLY,sizeof(int),NULL,&err);
            this->buffer_window_height = clCreateBuffer(this->context(),CL_MEM_READ_ONLY,sizeof(int),NULL,&err);
            this->buffer_grid_details = clCreateBuffer(this->context(),CL_MEM_READ_ONLY,sizeof(cl_GridDetails),NULL,&err);
            this->buffer_sub_grid_raster_max = clCreateBuffer(this->context(),CL_MEM_READ_ONLY,sizeof(int),NULL,&err);
            this->buffer_triangles_out = clCreateBuffer(this->context(),CL_MEM_READ_WRITE,sizeof(cl_Triangle) * 31 * this->trianglesInScene,NULL,&err);
            this->buffer_vertex_shader_to_rasterizer = clCreateBuffer(this->context(),CL_MEM_READ_WRITE,sizeof(int) * this->gridDetails.cols * this->gridDetails.rows * this->subGridRasterMax,NULL,&err);
            
            // Out Buffers 
            this->buffer_frame = clCreateBuffer(this->context(),CL_MEM_READ_WRITE,sizeof(cl_Pixel_Texture_Out) * this->window_width*this->window_height,NULL,&err);
        }

        // Return the number of pixels required to store all textures
        int getTexturesLength(){
            int sum = 0 ;
            for (auto texture: this->textureList){
                sum += texture.getHeight() * texture.getWidth();
            }
            return sum;
        }

        // Transfarer textures to the GPU
        void transfarerTextures(){
            // texture offset
            int offset = 0;

            // cl_Details array 
            cl_TextureDetail textureDetails[this->textureList.size()]; 

            // Create a texture array 
            int textureLength = getTexturesLength();
            RGB* tex = new RGB[textureLength];
            
            //iterate textures
            for (int i = 0; i != this->textureList.size(); i++){
                //texture
                Image* texture = &this->textureList[i];
                
                // Add texture details to the array 
                textureDetails[i] = {texture->getWidth(), texture->getHeight(),offset};

                // resolution 
                int resolution = texture->getHeight() * texture->getWidth();

                // Write the textures to the buffer
                //this->clqueue.enqueueWriteBuffer(this->buffer_textures,CL_TRUE,offset,sizeof(RGB) * resolution,texture->matrix);

                // Copy textures to the array 
                for (int i=0; i != resolution; i++){
                    tex[offset + i] = texture->matrix[i];
                }

                // Finish task 
                this->clqueue.finish();

                // Increment the offset
                offset += resolution;
            }

            // Write textures
            this->clqueue.enqueueWriteBuffer(this->buffer_textures,CL_TRUE,0,sizeof(RGB) * textureLength,tex);

            // Write details to the buffer 
            this->clqueue.enqueueWriteBuffer(this->buffer_texture_maps,CL_TRUE,0,sizeof(cl_TextureDetail) * this->textureList.size(),&textureDetails);

        }

        //Called every time the window updates     
        void onWindowUpdate(){
       
            // Create OpenCL buffers
            if (!this->buffersCreated){
                this->createOpenCLBuffers();
                this->buffersCreated = true;
            }

            // Transfarer textures
            if (!this->texturesTransfared){
                this->transfarerTextures();
                this->texturesTransfared = true;
            }

            // Final projected colors and triangles 
            vector<Triangle> projectedTriangles;
            vector<Triangle> triangleReferences; 
        
            // Camera Matrix - this is here so that we do not retrieve it on every render triangle
            Matrix4x4 matViewd = this->sceneCamera.get4x4MatrixInverse(); 
        
            //Offset to the center of the screen 
            Vect3d vOffsetView = {1,1,0};

        
            // Combine meshes 
            if (this->changesToScene){

                // Clear the array first 
                this->combinedMeshesTriangles.clear();
    
                // Iterate meshes and combine into one vector  
                for (auto i= this->meshes.begin(); i != this->meshes.end(); ++i){

                    // Mesh in iteration
                    Mesh mesh = i->second;

                    // Copy vertex data to the combined vector 
                    for (Triangle tri: mesh.triangles){
                        Triangle triTransformed;;
                        
                        // Apply geometric transformation to each triangle
                        triTransformed.p[0] = MatrixMultiplyVector(mesh.geometryMatrix,tri.p[0]);
                        triTransformed.p[1] = MatrixMultiplyVector(mesh.geometryMatrix,tri.p[1]);
                        triTransformed.p[2] = MatrixMultiplyVector(mesh.geometryMatrix,tri.p[2]);
                        
                        //Copy texture
                        triTransformed.copyTextureFrom(tri);

                        // Set texture number
                        triTransformed.textureNumber = mesh.textureNumber;

                        // Add triangle to the combined mesh triangles 
                        this->combinedMeshesTriangles.push_back(triTransformed); 
                    }

                }

                // Set scene as combined 
                this->changesToScene = false;
            }
            
            // Light 
            light.render(combinedMeshesTriangles,true);

            // Add combined triangles to an array for opencl 
            cl_Triangle h_combinedMeshesTriangleArray[combinedMeshesTriangles.size()];
            size_t combinedMeshesTriangleCount = combinedMeshesTriangles.size();
            for (size_t i=0; i!=combinedMeshesTriangles.size();i++){
                Triangle* h_tri = &combinedMeshesTriangles[i];
                h_combinedMeshesTriangleArray[i] = {
                    /*point 1*/ h_tri->p[0].x,h_tri->p[0].y,h_tri->p[0].z,h_tri->p[0].w,
                    /*point 2*/ h_tri->p[1].x,h_tri->p[1].y,h_tri->p[1].z,h_tri->p[1].w,
                    /*point 3*/ h_tri->p[2].x,h_tri->p[2].y,h_tri->p[2].z,h_tri->p[2].w,
                    /*tex 1*/   h_tri->t[0].u,h_tri->t[0].v,h_tri->t[0].w,
                    /*tex 2*/   h_tri->t[1].u,h_tri->t[1].v,h_tri->t[1].w, 
                    /*tex 3*/   h_tri->t[2].u,h_tri->t[2].v,h_tri->t[2].w,
                    0,
                    /*Texture */h_tri->textureNumber};
            }


            // Camera Matrix - copy the current camera matrix data to the cl Matrix 
            cl_Matrix4x4 h_CameraMatrix;
            h_CameraMatrix.m[0][0] = this->sceneCamera.cameraMatrix.m[0][0];
            h_CameraMatrix.m[0][1] = this->sceneCamera.cameraMatrix.m[0][1];
            h_CameraMatrix.m[0][2] = this->sceneCamera.cameraMatrix.m[0][2];
            h_CameraMatrix.m[0][3] = this->sceneCamera.cameraMatrix.m[0][3];

            h_CameraMatrix.m[1][0] = this->sceneCamera.cameraMatrix.m[1][0];
            h_CameraMatrix.m[1][1] = this->sceneCamera.cameraMatrix.m[1][1];
            h_CameraMatrix.m[1][2] = this->sceneCamera.cameraMatrix.m[1][2];
            h_CameraMatrix.m[1][3] = this->sceneCamera.cameraMatrix.m[1][3];

            h_CameraMatrix.m[2][0] = this->sceneCamera.cameraMatrix.m[2][0];
            h_CameraMatrix.m[2][1] = this->sceneCamera.cameraMatrix.m[2][1];
            h_CameraMatrix.m[2][2] = this->sceneCamera.cameraMatrix.m[2][2];
            h_CameraMatrix.m[2][3] = this->sceneCamera.cameraMatrix.m[2][3];
            
            h_CameraMatrix.m[3][0] = this->sceneCamera.cameraMatrix.m[3][0];
            h_CameraMatrix.m[3][1] = this->sceneCamera.cameraMatrix.m[3][1];
            h_CameraMatrix.m[3][2] = this->sceneCamera.cameraMatrix.m[3][2];
            h_CameraMatrix.m[3][3] = this->sceneCamera.cameraMatrix.m[3][3];

            cl_Matrix4x4 h_projectionMatrix;
            h_projectionMatrix.m[0][0] = this->projectionMatrix.m[0][0];
            h_projectionMatrix.m[0][1] = this->projectionMatrix.m[0][1];
            h_projectionMatrix.m[0][2] = this->projectionMatrix.m[0][2];
            h_projectionMatrix.m[0][3] = this->projectionMatrix.m[0][3];

            h_projectionMatrix.m[1][0] = this->projectionMatrix.m[1][0];
            h_projectionMatrix.m[1][1] = this->projectionMatrix.m[1][1];
            h_projectionMatrix.m[1][2] = this->projectionMatrix.m[1][2];
            h_projectionMatrix.m[1][3] = this->projectionMatrix.m[1][3];

            h_projectionMatrix.m[2][0] = this->projectionMatrix.m[2][0];
            h_projectionMatrix.m[2][1] = this->projectionMatrix.m[2][1];
            h_projectionMatrix.m[2][2] = this->projectionMatrix.m[2][2];
            h_projectionMatrix.m[2][3] = this->projectionMatrix.m[2][3];
            
            h_projectionMatrix.m[3][0] = this->projectionMatrix.m[3][0];
            h_projectionMatrix.m[3][1] = this->projectionMatrix.m[3][1];
            h_projectionMatrix.m[3][2] = this->projectionMatrix.m[3][2];
            h_projectionMatrix.m[3][3] = this->projectionMatrix.m[3][3];
        
            // Write buffers
            this->clqueue.enqueueWriteBuffer(this->buffer_triangles_in, CL_TRUE, 0, sizeof(cl_Triangle) * combinedMeshesTriangleCount, &h_combinedMeshesTriangleArray);
            this->clqueue.enqueueWriteBuffer(this->buffer_camera_matrix,CL_TRUE,0,sizeof(cl_Matrix4x4),&h_CameraMatrix);
            this->clqueue.enqueueWriteBuffer(this->buffer_projection_matrix,CL_TRUE,0,sizeof(cl_Matrix4x4),&h_projectionMatrix);
            this->clqueue.enqueueWriteBuffer(this->buffer_window_width,CL_TRUE,0,sizeof(int),&this->window_width);
            this->clqueue.enqueueWriteBuffer(this->buffer_window_height,CL_TRUE,0,sizeof(int),&this->window_height);
            this->clqueue.enqueueWriteBuffer(this->buffer_grid_details,CL_TRUE,0,sizeof(cl_GridDetails),&this->gridDetails);
            this->clqueue.enqueueWriteBuffer(this->buffer_sub_grid_raster_max,CL_TRUE,0,sizeof(int),&this->subGridRasterMax);

            // Reset the triangle count
            int zero = 0;
            this->clqueue.enqueueWriteBuffer(this->buffer_triangles_count,CL_TRUE,0,sizeof(int),&zero);

            // Reset vertex_shader_to_rasterizer buffer
            cl_int pattern = 0;
            cl_int err;
            clEnqueueFillBuffer(this->clqueue(), buffer_vertex_shader_to_rasterizer(), &pattern, sizeof(pattern), 0, sizeof(int) * this->gridDetails.cols * this->gridDetails.rows * this->subGridRasterMax, 0, NULL, NULL);

            // Reset frame buffer;
            cl_Pixel_Texture_Out blank_pixel = {0.0f, 0.0f, 0, 0.0f};
            clEnqueueFillBuffer(this->clqueue(),this->buffer_frame(), &blank_pixel, sizeof(cl_Pixel_Texture_Out), 0, sizeof(cl_Pixel_Texture_Out) * this->window_width * this->window_height, 0, NULL, NULL);
            

            // Reset depth buffer 
            float zero_f = 0.f;
            clEnqueueFillBuffer(this->clqueue(),this->buffer_depth(),&zero_f,sizeof(float),0,sizeof(float) * this->window_width * this->window_height,0,NULL,NULL);

            // Vertex Shader - Set arguments
            this->renderKernel.setArg(0,this->buffer_triangles_in);
            this->renderKernel.setArg(1,this->buffer_camera_matrix);
            this->renderKernel.setArg(2,this->buffer_projection_matrix);
            this->renderKernel.setArg(3,this->buffer_window_width);
            this->renderKernel.setArg(4,this->buffer_window_height);
            this->renderKernel.setArg(5,this->buffer_triangles_out);
            this->renderKernel.setArg(6,this->buffer_triangles_count);
            this->renderKernel.setArg(7,this->buffer_grid_details);
            this->renderKernel.setArg(8,this->buffer_vertex_shader_to_rasterizer);
            this->renderKernel.setArg(9,this->buffer_sub_grid_raster_max);
    

            // Execute Vertex and Geometry Shader 
            this->clqueue.enqueueNDRangeKernel(renderKernel,cl::NullRange,cl::NDRange(combinedMeshesTriangleCount));
            this->clqueue.finish();

            cl::Buffer buffer_image;
            buffer_image = clCreateBuffer(this->context(),CL_MEM_READ_WRITE,sizeof(RGB) * this->window_width * this->window_height,NULL,NULL);

            // Set the Rasterizer Shader Values
            this->textureKernel.setArg(0,this->buffer_vertex_shader_to_rasterizer);
            this->textureKernel.setArg(1,this->buffer_sub_grid_raster_max);
            this->textureKernel.setArg(2,this->buffer_triangles_out);
            this->textureKernel.setArg(3,this->buffer_grid_details);
            this->textureKernel.setArg(4,this->buffer_frame);
            this->textureKernel.setArg(5,this->buffer_textures);
            this->textureKernel.setArg(6,this->buffer_texture_maps);
            this->textureKernel.setArg(7,this->buffer_depth);
            this->textureKernel.setArg(8,buffer_image);

            // Execute Rasterizer Shader 
            this->clqueue.enqueueNDRangeKernel(this->textureKernel,0,this->gridDetails.rows * this->gridDetails.cols,1);
            this->clqueue.finish();
        

            // Read Out Tiles 
            RGB* im = new RGB[800*800];
            this->clqueue.enqueueReadBuffer(buffer_frame,CL_TRUE,0,sizeof(cl_Pixel_Texture_Out) * this->window_width * this->window_height,this->h_outTiles);
            this->clqueue.enqueueReadBuffer(buffer_image,CL_TRUE,0,sizeof(RGB)*this->window_width * this->window_height,im);

            glBegin(GL_POINTS);
                // Iterate the output buffer 
                for (int pos = 0 ; pos != this->window_width * this->window_height; pos++){
                    int y = pos / this->window_width;
                    int x = pos - (y * this->window_width);
                    
                    RGB pixel = im[pos];
                    
                    if (!pixel.hasData){
                        continue;
                    }

                    glColor3f(pixel.r,pixel.g,pixel.b);
                    glVertex2i(x,y);
                }
            glEnd();
            
    
            std::free(im);

            // Draw Grid Lines 
            //for (int col = 0; col <= this->gridDetails.square_width * this->gridDetails.cols; col += this->gridDetails.square_width ){
            //    glBegin(GL_LINES);
            //        glColor3f(1,1,0);
            //        glVertex2i(col,0);
            //        glVertex2i(col,this->gridDetails.square_width * this->gridDetails.rows);
            //        glVertex2i(0,col);
            //        glVertex2i(this->gridDetails.square_width * this->gridDetails.cols,col);
            //    glEnd();
            //} 

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
            //// POLYGON COUNT
            string polygons = "POLYGONS " + to_string(combinedMeshesTriangles.size());
            writer.print(20,65,polygons,9);
                  

            if (framesRendered >= 2000){
                framesRendered = 0 ;
                timeBegin = std::chrono::steady_clock::now();
            }            

            return;          
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
    //graphicsEngine.addToScene(crate);


    Mesh crate2 = Mesh("crate2");
    crate2.LoadFromObjectFile("./assets/objs/crate/Crate1.obj",readPPM("./assets/objs/crate/crate.ppm"));
    crate2.scale(5);
    crate2.translate({0,0,10});
    //graphicsEngine.addToScene(crate2);

    // Basement Entry 
    Mesh basement = Mesh("basement");
    basement.LoadFromObjectFile("./assets/objs/barn/barn.obj",readPPM("./assets/objs/barn/texture.ppm"));
    basement.rotateZ(180);
    basement.translate({0,0,5});
    //graphicsEngine.addToScene(basement);
    

    // Pathwalk 
    Mesh pathwalk = Mesh();
    pathwalk.LoadFromObjectFile("./assets/objs/pathwalk/pathwalk.obj",readPPM("./assets/objs/pathwalk/texture.ppm"));
    pathwalk.rotateZ(180);
    pathwalk.translate({0,0,-10});
    graphicsEngine.addToScene(pathwalk);

    // Pathwalk 
    Mesh pathwalk2 = Mesh();
    pathwalk2.LoadFromObjectFile("./assets/objs/pathwalk/pathwalk.obj",readPPM("./assets/objs/pathwalk/texture.ppm"));
    pathwalk2.rotateZ(180);
    pathwalk2.translate({0,10,-10});
    //graphicsEngine.addToScene(pathwalk2);


     // Pathwalk 
    Mesh pathwalk3 = Mesh();
    pathwalk3.LoadFromObjectFile("./assets/objs/pathwalk/pathwalk.obj",readPPM("./assets/objs/pathwalk/texture.ppm"));
    pathwalk3.rotateZ(180);
    pathwalk3.translate({0,5,-10});
    ///graphicsEngine.addToScene(pathwalk3);

    // Drum 
    Mesh drum = Mesh("drum");
    drum.LoadFromObjectFile("./assets/objs/drum/drum.obj",readPPM("./assets/objs/drum/texture.ppm"));
    drum.rotateZ(180);
    //graphicsEngine.addToScene(drum);

    // Initialize engine
    graphicsEngine.init(argc,argv);

    return 0;
}