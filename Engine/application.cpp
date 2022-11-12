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
#include "utils/sample_cube.h"


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
        float rotationAngle = 22/7  * 2;
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
        void texturedTriangle(Triangle tri,bool colorRed=false){
            glBegin(GL_LINES);
                glVertex2i(tri.p[0].x,tri.p[0].y);
                glVertex2i(tri.p[1].x,tri.p[1].y);
                glVertex2i(tri.p[1].x,tri.p[1].y);
                glVertex2i(tri.p[2].x,tri.p[2].y);
                glVertex2i(tri.p[2].x,tri.p[2].y);
                glVertex2i(tri.p[0].x,tri.p[0].y);
            glEnd();

            // 
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
            int dy1 = y2 - y1 ;
            int dx1 = x2 - x1 ;
            float dv1 = v2 - v1;
            float du1 = u2 - u1;

            int dy2 = y3 - y1;
            int dx2 = x3 - x1;
            float dv2 = v3 - v1;
            float du2 = u3 - u1;


            // Calculate dx_1 and dx_2 
            float dax_step = 0, dbx_step = 0,
                du1_step = 0, dv1_step = 0,
                du2_step =0, dv2_step = 0 ;

            float tex_u,tex_v;


            // Calculate differentials 
            if (dy1) dax_step = dx1 / (float)abs(dy1); //float absolutes 
            if (dy2) dbx_step = dx2 / (float)abs(dy2);

            if (dy1) dv1_step = dv1/  (float)abs(dy1);
            if (dy2) du1_step = du1/ (float)abs(dy1);

            if (dy2) dv2_step = dv2/ (float)abs(dy2);
            if (dy2) du2_step = du2/ (float)abs(dy2);


            // Draw top half 
            if (dy1){
                // Move y 
                for (int i = y1; i <= y2;i++){
                    int ax = x1 + (float)(i - y1) * dax_step;
                    int bx = x1 + (float)(i - y1) * dbx_step;

                    float tex_su = u1 + (float)(i - y1) * du1_step;
                    float tex_sv = v1 + (float)(i - y1) * dv1_step;

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

                        tex_u = f_min(tex_u,1.0f);
                        tex_v = f_min(tex_v,1.0f);

                        RGB pixel = texture.getPixelAt(tex_v * this->texture.getHeight(),tex_u * this->texture.getWidth());
                        glColor3f(pixel.r,pixel.g,pixel.b);
                        if (colorRed) glColor3f(1,0,0);
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


                    
                        tex_u = f_min(tex_u,1.0f);
                        tex_v = f_min(tex_v,1.0f);

                        RGB pixel = texture.getPixelAt(tex_v * this->texture.getHeight(),tex_u * this->texture.getWidth());
                        glColor3f(pixel.r,pixel.g,pixel.b);
                        if (colorRed) glColor3f(1,0,0);
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
            this->texture = readPPM("./assets/objs/crate/crate.ppm");
            this->mesh.LoadFromObjectFile("./assets/objs/cube.obj");
            this->mesh.triangles[0].t[0].u = 0;
            this->mesh.triangles[0].t[0].v = 0;
            this->mesh.triangles[0].t[1].u = 1.f;
            this->mesh.triangles[0].t[1].v = 1.f;
            this->mesh.triangles[0].t[2].u = 0;
            this->mesh.triangles[0].t[2].v = 1.f;
            
            this->mesh.triangles[6].t[0].u = 0;
            this->mesh.triangles[6].t[0].v = 0;
            this->mesh.triangles[6].t[1].u = 0.f;
            this->mesh.triangles[6].t[1].v = 1.f;
            this->mesh.triangles[6].t[2].u = 1.f;
            this->mesh.triangles[6].t[2].v = 1.f;

            



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
            this->rotationAngle += 0.001;
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

                // No matworld 
                //matWorld = Matrix4x4_MakeIdentity();
                //matRotX = getMatrixRotationX(rotationAngle);
                //matWorld = MatrixMultiplyMatrix(matRotX,matTrans);

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
                line2 = triTransformed.p[2] - triTransformed.p[0]; 
                normal = VectorCrossProduct(line1,line2);
                NormalizeVector(normal);


                vCamera = {0,0,-1};
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


            // Draw the triangle on the server side if showGraphics is true 
            for (Triangle tri : projectedTriangles){
                if (tri.t[0].u == -1){
                    this->texturedTriangle(tri);
                    continue;
                }
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