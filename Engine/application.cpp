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
#include "PPA/image_loader.h"
#include "utils/common.h"
#include "utils/matrix_operations.h"



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
        float rotationAngle = 0.f;
        // Time
        std::chrono::steady_clock::time_point timeBegin;


        // Setup Window
        void setupWindow(int &argc, char **&argv){
            // Start Clock
            this->timeBegin = std::chrono::steady_clock::now();

            // Clock
            //std::chrono::steady_clock::time_point currentTime = std::chrono::steady_clock::now();
            //int seconds_passed = std::chrono::duration_cast<std::chrono::seconds> (currentTime- this->timeBegin).count();
            
            // Setup Window 
            glutInit(&argc, argv);
            glutInitDisplayMode(GLUT_SINGLE);
            glutInitWindowSize(this->window_width, this->window_height);
            glutInitWindowPosition(100, 100);
            glutCreateWindow("OpenGL - Creating a triangle");
            glutDisplayFunc(_EntryOnWindowReady);
            glutMainLoop();            
        }

        // Given two vectors an the y, returns the x 
        float lineEquationFindX(Vect3d line1, Vect3d line2,float y){
            float m = (line1.y - line2.y) / (line1.x - line2.x);
            return y/m;
        }

        // Texture Triangle
        void textureTriangle(Triangle &tri){
            //GLBEGIN
            glBegin(GL_POINTS);
                float y = tri.p[0].y ;
                while (y <= tri.p[2].y){
                    float x = tri.p[0].y;
                    float xEnd = lineEquationFindX(tri.p[0],tri.p[1],y); 
                    while(x <= xEnd){
                        
                        // Normalize x and y 
                        float xNorm = x / (tri.p[0].x + tri.p[1].x);
                        float yNorm = y / (tri.p[0].y + tri.p[2].y);
                        RGB pixel = this->texture.getPixelAt(this->texture.getHeight() * yNorm,this->texture.getWidth() * xNorm);
                        glColor3f(pixel.r,pixel.g,pixel.b);
                        glVertex2f(x,y);

                        // Increment x 
                        x += 1;
                    }

                    // increment y
                    y += 1 ;
                }
            glEnd();
        }

        // Texture Triangle(2)
        void texturedTriangle(Triangle &tri){
            
            // Points 
            int x1 = tri.p[0].x;
            int y1 = tri.p[0].y;
            int x2 = tri.p[1].x;
            int y2 = tri.p[1].y; 
            int x3 = tri.p[2].x;
            int y3 = tri.p[2].y;


            // Sort Points 
            if (y2 < y1){
                swap(y2,y1);
                swap(x2,x1);
            }

            if (y3 < y1){
                swap(y3,y1);
                swap(x3,x1);
            }

            if (y3 < y2){
                swap(y3,y2);
                swap(x3,x2);
            }

            // Texture Triangle 
            glBegin(GL_POINTS);
            glColor3f(1,0,0);
            // Top half 
                // Move from y1 to y2 
                int ySteps = 0 ;
                float y = y1;
                int xStart = 0, xs_temp;
                int xEnd = 0, xe_temp;

                // dx/dy 
                float A1_dx_dy = (float) (x1 - x3) / (y1 - y3);
                float A2_dx_dy = (float) (x1 - x2) / (y1 - y2);
                
                if (!(y1 - y3) || !(x1 - x3)) A1_dx_dy = 0;
                if (!(x1 - x2) || !(y1 - y2)) A2_dx_dy = 0;
                while (y <= y2){
                    // xStart is the min between the 2 x's, this is because if A2 is on the left side we wont draw anythins
                    xs_temp = x1 + (ySteps * A1_dx_dy);
                    xe_temp = x1 + (ySteps * A2_dx_dy);
                    
                    xStart = min(xs_temp,xe_temp);
                    xEnd = max(xe_temp,xs_temp);
                    
                    while (xStart <= xEnd){
                        // Texture
                        RGB pixel = this->texture.getPixelAt(y,xStart);
                        glColor3f(pixel.r,pixel.g,pixel.b);
                        glVertex2i(xStart,y);
                        xStart++;
                    }
                    //move down y 
                    ySteps++;
                    y++;
                }
            
            // Bottom half 
                int x3Steps = 0 ;
                float A3_dx_dy = (float)(x2 - x3)/(y2 - y3);
                if (!(x2 - x3) || !(y2 - y3)) A3_dx_dy = 0;

                while (y <= y3){
                    xs_temp = x1 + (ySteps * A1_dx_dy);
                    xe_temp = x2 + (x3Steps * A3_dx_dy);

                    xStart = min(xs_temp,xe_temp);
                    xEnd = max(xe_temp,xs_temp);
                    
                    glColor3f(0,1,0);
                    while (xStart <= xEnd){
                        // Texture
                        RGB pixel = this->texture.getPixelAt(y,xStart);
                        glColor3f(pixel.r,pixel.g,pixel.b);
                        glVertex2i(xStart,y);
                        xStart++;
                    }
                    //move down y 
                    ySteps++;
                    x3Steps++;
                    y++;
                }


            glEnd();
            glBegin(GL_LINES);
                glColor3f(1.f,0.f,0.f);
                glVertex2i(tri.p[0].x,tri.p[0].y);
                glVertex2i(tri.p[1].x,tri.p[1].y);
                glVertex2i(tri.p[2].x,tri.p[2].y);
                glVertex2i(tri.p[0].x,tri.p[0].y);
            glEnd();
        }
        // Draws the texture ppm 
        void drawTexture(){
            //GL Begin
            glBegin(GL_POINTS);
                //Draw Texture 
                for (int r=0; r != this->texture.getHeight(); ++r){
                    for (int c=0; c != this->texture.getWidth(); ++c){
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
        //Load from file 
        bool loadFromObjectFile(string sFilename){
            ifstream f(sFilename);
            if (!f.is_open())
                return false;

            vector<Vect3d> verts;
            vector<Triangle> tris;

            while(!f.eof()){
                char line[128];
                f.getline(line,128);

                strstream s;
                s << line ;

                char junk;
                if (line[0] == 'v'){
                    Vect3d v;
                    s >> junk >> v.x >> v.y >> v.z;
                    verts.push_back(v);
                }

                if (line[0] == 'f'){
                    int f[3];
                    s >> junk >> f[0] >> f[1] >> f[2];
                    mesh.triangles.push_back({verts[f[0] - 1],verts[f[1] - 1],verts[f[2] - 1]});
                }
            }

            return true;
        }
    public:
        
        // Texture Holder 
        Image texture; 
        // Objects List
        Mesh mesh;
        // Camera 
        Vect3d vCamera;

        // Constructor 
        W3DGraphics(int windowWidth, int windowHeight){
            instance = this;
            //Set window width and height
            this->window_height = windowHeight;
            window_width = windowHeight; 
        }

        // Initializer 
        void init(int &argc, char **&argv){
            // Setup Window 
            this->setupWindow(argc,argv);
        }

   
        //Return render function 
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

            // Load the texture 
            this->texture = readPPM("./assets/ppm/wood.ppm");
            this->loadFromObjectFile("./assets/objs/cube.obj");
            // Set Black Background
            glClearColor(0,0, 0, 1); 
            glClear(GL_COLOR_BUFFER_BIT);
            // Set Window Bounds
            glOrtho(0.0,this->window_width,this->window_height,0.0,0,1000);
            
            // Start Updating
            bool closeWindow = false;
            while (!closeWindow)
            {
                // Clear Screen
                glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
                    // Window Update 
                    closeWindow = this->onWindowUpdate();
                // Flush Screen
                glFlush();
            }
        }

        // Called every time the window updates     
        bool onWindowUpdate(){
            this->rotationAngle += 0.001f;
            // Render object
            // Final projected colors and triangles 
            vector<Triangle> projectedTriangles;

            //draw triangles 
            for (Triangle tri : this->mesh.triangles){
                Triangle triProjected,triTransformed;
                Matrix4x4 matRotZ, matRotX;

                //  Geometry (Rotation and Translation)
                matRotZ = getMatrixRotationZ(rotationAngle);
                matRotX = getMatrixRotationX(rotationAngle);
                Matrix4x4 matTrans = Matrix_MakeTranslation(0.0f,0.0f,5.0f);
                
                Matrix4x4 matWorld = MatrixMultiplyMatrix(matRotZ,matRotX);
                matWorld  = MatrixMultiplyMatrix(matWorld,matTrans);

                triTransformed.p[0] = MatrixMultiplyVector(matWorld,tri.p[0]);
                triTransformed.p[1] = MatrixMultiplyVector(matWorld,tri.p[1]);
                triTransformed.p[2] = MatrixMultiplyVector(matWorld,tri.p[2]); 
                //Copy texture as well 
                triTransformed.t[0] = tri.t[0];
                triTransformed.t[1] = tri.t[1];
                triTransformed.t[2] = tri.t[2];
                

                //  Determine the normal so that we do not draw triangles that are not facing us
                Vect3d normal,line1,line2 ;
                line1 = triTransformed.p[1] - triTransformed.p[0];
                line2 = triTransformed.p[2] - triTransformed.p[1]; 
                normal = VectorCrossProduct(line1,line2);
                NormalizeVector(normal);


                // Camera Ray Which is the difference between triTransformed.p[0] and vCamera
                Vect3d vCameraRay = triTransformed.p[0] - vCamera;

                if (VectorDotProduct(vCameraRay,normal) < 0
                    ){

                    //illumination
                    Vect3d lightSource = {0,0,-1}; 
                    NormalizeVector(lightSource);

                    float dt = f_max(VectorDotProduct(normal,lightSource),0);
                    Color color = {abs(dt*1),abs(dt*1),abs(dt*1)};


                    //
                    // If z is negetive
                    MultiplyMatrixVector(triTransformed.p[0],triProjected.p[0],this->projectionMatrix);
                    MultiplyMatrixVector(triTransformed.p[1],triProjected.p[1],this->projectionMatrix);
                    MultiplyMatrixVector(triTransformed.p[2],triProjected.p[2],this->projectionMatrix);
                    triProjected.t[0] = triTransformed.t[0];
                    triProjected.t[1] = triTransformed.t[1];
                    triProjected.t[2] = triTransformed.t[2];

                    //Offset
                    Vect3d vOffsetView = {1,1,0};
                    triProjected.p[0] = triProjected.p[0] + vOffsetView;
                    triProjected.p[1] = triProjected.p[1] + vOffsetView;
                    triProjected.p[2] = triProjected.p[2] + vOffsetView;
                    
                    VectorMultiplyFloat(triProjected.p[0],0.4f * this->window_width);
                    VectorMultiplyFloat(triProjected.p[1],0.4f * this->window_width);
                    VectorMultiplyFloat(triProjected.p[2],0.4f * this->window_width);
                
                    // Add to the final project triangles
                    triProjected.color = color;
                    projectedTriangles.push_back(triProjected);
                }
            }

            // Raster the triangles
            projectedTriangles = sortTriangleList(projectedTriangles);


            Triangle sample;
                sample.p[0] = {20,20,0};
                sample.p[1] = {180,180,0};
                sample.p[2] = {20,180,0};
            // Draw the triangle on the server side if showGraphics is true 
            for (Triangle tri : projectedTriangles){
                this->texturedTriangle(tri);
            }

            // Do not close window 
            return false;
        }
};



// Test 
int main(int argc, char **argv)
{
    // Engine instance
    W3DGraphics graphicsEngine = W3DGraphics(800,800);
    graphicsEngine.init(argc,argv);
    
    return 0;
}