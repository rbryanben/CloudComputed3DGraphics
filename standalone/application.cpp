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
        void texturedTriangle(Triangle tri,bool colorRed=false){
            // GL_BEGIN
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

            float w1 = tri.t[0].w ;
            float w2 = tri.t[1].w ;
            float w3 = tri.t[2].w ;


            // Sort Points 
            if (y2 < y1){
                swap(y2,y1);
                swap(x2,x1);
                swap(u2,u1);
                swap(v2,v1);
                swap(w2,w1);
            }

            if (y3 < y1){
                swap(y3,y1);
                swap(x3,x1);
                swap(u3,u1);
                swap(v3,v1);
                swap(w3,w1);
            }

            if (y3 < y2){
                swap(y3,y2);
                swap(x3,x2);
                swap(u3,u2);
                swap(v3,v2);
                swap(w3,w2);
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
                for (int i = y1; i <= y2;i++){
                    int ax = x1 + (float)(i - y1) * dax_step;
                    int bx = x1 + (float)(i - y1) * dbx_step;

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
                    }

                    tex_u = tex_su;
                    tex_v = tex_sv;
                    tex_w = tex_sw;

                    float tstep = 1.0f / ((float)(bx - ax));
                    float t = 0.0f;

                    for (int j = ax; j <bx; j++){
                        tex_u = (1.0f - t) * tex_su + t * tex_eu;
                        tex_v = (1.0f - t) * tex_sv + t * tex_ev;
                        tex_w = (1.0f - t) * tex_sw + t * tex_ew;

                    
                        RGB pixel = texture.getPixelAt((tex_v/tex_w) * (this->texture.getHeight() - 1),(tex_u/tex_w) * (this->texture.getWidth() - 1));
                        glColor3f(pixel.r,pixel.g,pixel.b);
                        
                        if ( i >= 0 && j >= 0 && i * this->window_width + j < this->window_height * this->window_width 
                            && tex_w > depthBuffer[i * this->window_width + j]){
                            glVertex2i(j,i);
                            depthBuffer[i * this->window_width + j] = tex_w;
                        }

                        t += tstep;
                    }

                }
            }

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


            if (dy1)
            {
                for (int i = y2; i <= y3; i++)
                {
                    int ax = x2 + (float)(i - y2) * dax_step;
                    int bx = x1 + (float)(i - y1) * dbx_step;

                    float tex_su = u2 + (float)(i - y2) * du1_step;
                    float tex_sv = v2 + (float)(i - y2) * dv1_step;
                    float tex_sw = w2 + (float)(i - y2) * dw1_step;

                    float tex_eu = u1 + (float)(i - y1) * du2_step;
                    float tex_ev = v1 + (float)(i - y1) * dv2_step;
                    float tex_ew = w1 + (float)(i - y1) * dw2_step;


                    if (ax > bx)
                    {
                        swap(ax, bx);
                        swap(tex_su, tex_eu);
                        swap(tex_sv, tex_ev);
                        swap(tex_sw, tex_ew);

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


                        RGB pixel = texture.getPixelAt((tex_v/tex_w) * (this->texture.getHeight() - 1),(tex_u/tex_w) * (this->texture.getWidth() - 1));
                        glColor3f(pixel.r,pixel.g,pixel.b);
                        
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
        Mesh mesh;

        // W3Camera
        W3Camera sceneCamera; 

        //illumination - basic 
        Vect3d lightSource = {0,0,-1};
        
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

            // Load the texture 
            this->texture = readPPM("./assets/objs/crate/crate.ppm");
            this->mesh.LoadFromObjectFile("./assets/objs/crate/Crate1.obj",true);
            
            // Configure camera
            this->sceneCamera = W3Camera({0,0,-5,1});
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
        
        // Called every time the window updates     
        void onWindowUpdate(){
            // Render object
            // Final projected colors and triangles 
            vector<Triangle> projectedTriangles;
            
            //  Geometry (Rotation and Translation)
            Matrix4x4 matRotZ = getMatrixRotationZ(rotationAngle);

            // Camera Matrix - this is here so that we do not retrieve it on every render triangle
            Matrix4x4 matViewd = this->sceneCamera.get4x4MatrixInverse(); 
        
            //Offset to the center of the screen 
            Vect3d vOffsetView = {1,1,0};

            // Geometric Transformations 
            Matrix4x4 matWorld = matRotZ * Matrix_MakeTranslation(0.0f,0.0f,0.0f);
            
            //draw triangles 
            for (Triangle tri : this->mesh.triangles){
                Triangle triProjected,triTransformed;
                Matrix4x4 matRotZ, matRotX;
            

                // Apply geometric transformation to each triangle
                //   rotation, scaling and or translation
                triTransformed.p[0] = MatrixMultiplyVector(matWorld,tri.p[0]);
                triTransformed.p[1] = MatrixMultiplyVector(matWorld,tri.p[1]);
                triTransformed.p[2] = MatrixMultiplyVector(matWorld,tri.p[2]); 

                //Copy texture
                triTransformed.t[0] = tri.t[0];
                triTransformed.t[1] = tri.t[1];
                triTransformed.t[2] = tri.t[2];
                

                // Determine the normal so that we do not draw triangles that are not facing us
                Vect3d normal,line1,line2 ;
                line1 = triTransformed.p[1] - triTransformed.p[0];
                line2 = triTransformed.p[2] - triTransformed.p[0]; 
                normal = VectorCrossProduct(line1,line2);
                NormalizeVector(normal);


                // Camera Ray Which is the difference between triTransformed.p[0] and vCamera
                //  is the vector from the camera to the triangle. We wish to calculate the angle inbetween to see if its visible 
                Vect3d vCameraRay = triTransformed.p[0] - this->sceneCamera.getTranslation();

                if (VectorDotProduct(vCameraRay,normal) < 0
                    ){
                    // Light source is placed in the scene space, before the camera
                    float dt = f_max(VectorDotProduct(normal,lightSource),0);
                    Color color = {f_max(abs(dt*1),0.2f),f_max(abs(dt*1),0.2f),f_max(abs(dt*1),0.2f)};
                
                    //
                    // Camera - From here the entire view is inversed to the camera's view 
                    Triangle triViewd; 
                    MultiplyMatrixVector(triTransformed.p[0],triViewd.p[0],matViewd);
                    MultiplyMatrixVector(triTransformed.p[1],triViewd.p[1],matViewd);
                    MultiplyMatrixVector(triTransformed.p[2],triViewd.p[2],matViewd);

                    triViewd.color = color;
                    
                    triViewd.t[0] = triTransformed.t[0];
                    triViewd.t[1] = triTransformed.t[1];
                    triViewd.t[2] = triTransformed.t[2];
                    // Clip the triangles against the axis
                    vector<Triangle> clippedZ = clipTriangleAgainstPlane({0,0,1.1f,1},{0,0,1,1},triViewd); 

                    for (auto tri : clippedZ){  
                        //
                        // Project the polygons(Some normalization takes place here)
                        //      At this point we project the 3D co-ordinates on to a 3D scene
                        float w0 = MultiplyMatrixVector(tri.p[0],triProjected.p[0],this->projectionMatrix);
                        float w1 = MultiplyMatrixVector(tri.p[1],triProjected.p[1],this->projectionMatrix);
                        float w2 = MultiplyMatrixVector(tri.p[2],triProjected.p[2],this->projectionMatrix);

                        // Copy the texture information to triProjected from triTransformed
                        triProjected.t[0] = tri.t[0];
                        triProjected.t[1] = tri.t[1];
                        triProjected.t[2] = tri.t[2];

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

                        triProjected.color = tri.color;

                        // Add to vertex_list 
                        projectedTriangles.push_back(triProjected);
                    }
                }
            }

            // Has Texture Data
            if (this->texture.hasData()){
                // Clear deptjh buffer 
                this->clearDepthBuffer();
                // Triangle clipped against screen edges 
                //
                vector<Triangle> clippedTrianglesList;
                // Clip against the top of the screen
                vector<Triangle> topClipped = clipTriangleAgainstPlane({0,0.1,0,1},{0,1,0,1},projectedTriangles);
                //// Clip against bottom of the screen
                vector<Triangle> bottomClipped =  clipTriangleAgainstPlane({0,(float)this->window_height - 1,0,1},{0,-1,0},topClipped);
                ////Clip against
                vector<Triangle> leftClipped = clipTriangleAgainstPlane({1,0,0,1},{1,0,0,1},bottomClipped);
                //// Clip against right of the screen
                vector<Triangle> rightClipped = clipTriangleAgainstPlane({(float)this->window_width - 1,0,0,1},{-1,0,0,1},leftClipped);
                // Draw
                for (Triangle tri: rightClipped){
                    this->texturedTriangle(tri);
                }
            
            }
            // No texture
            else {
                projectedTriangles = sortTriangleList(projectedTriangles);

                // Triangle clipped against screen edges 
                //
                vector<Triangle> clippedTrianglesList;

                // Clip against the top of the screen
                vector<Triangle> topClipped = clipTriangleAgainstPlane({0,0.1,0,1},{0,1,0,1},projectedTriangles);
                // Clip against bottom of the screen
                vector<Triangle> bottomClipped = clipTriangleAgainstPlane({0,(float)this->window_height - 1,0,1},{0,-1,0},topClipped);
                //Clip against
                vector<Triangle> leftClipped = clipTriangleAgainstPlane({1,0,0,1},{1,0,0,1},bottomClipped);
                // Clip against right of the screen
                vector<Triangle> rightClipped = clipTriangleAgainstPlane({(float)this->window_width - 1,0,0,1},{-1,0,0,1},leftClipped);

                for (Triangle tri : rightClipped){
                    this->drawTriangle(tri);
                }
            }
        }

};

// Test With UI
int main(int argc, char **argv)
{
    // Engine instance
    W3DGraphics graphicsEngine = W3DGraphics(800,800);
    graphicsEngine.init(argc,argv);

    return 0;
}