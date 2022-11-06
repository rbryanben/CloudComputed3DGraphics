#include <iostream>
#include <string.h>
#include <math.h>
#include <stdlib.h>
#include "sample_cube.h"
#include "common.h"
#include "../helpers/string_helper.hpp"
#include "matrix_operations.h"
#include "graphics.h"
#include <fstream>
#include <strstream>

//Server3dRender
class ServerRenderer{
    
    private:
        float windowWidth, windowHeight;
        Mesh mesh;
        string platform;
        Matrix4x4 projectionMatrix;
        int argc;
        char **argv;
        bool showGraphics;
        Vect3d vCamera;


        string append(float x, float y){
            x += 0.1;
            y += 0.1;
            return to_string(x) + " " + to_string(y);
        }


    public:
        // Constructor
        ServerRenderer(float windowWidth, float windowHeight, string platform, bool showGraphics=false){   
            this->windowWidth = windowWidth;
            this->windowHeight = windowHeight;
            this->platform = platform;
            this->showGraphics = showGraphics;
            
            //Load from file 
            this->loadFromObjectFile("./objs/teapot.obj");
    
            //onCreate
            this->onCreate();

            //Show Graphics 
            if (showGraphics){
                this->setupGraphics();
            }
        }

        // Shows graphics on server using graphics.h
        void setupGraphics(){
            int gd=DETECT,gm; 
            initgraph(&gd,&gm,NULL);
        }

        //OnCreate
        void onCreate(){
            //projection matrix
            this->projectionMatrix = Make_Projection(this->windowHeight/this->windowWidth,90.0f,1000.0f,0.1f);

        }
        
        // Test Method To Test Logic
        void test(){
            Matrix4x4 trans = Matrix_MakeTranslation(1,2,3);

        }

        //OnRender
        string onRender(float rotationAngle){
            //rotationAngle = 0.0f; // Prevent Rotation

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

                    float dt = VectorDotProduct(normal,lightSource);
                    Color color = {abs(dt*255),abs(dt*255),abs(dt*255)};


                    //
                    // If z is negetive
                    MultiplyMatrixVector(triTransformed.p[0],triProjected.p[0],this->projectionMatrix);
                    MultiplyMatrixVector(triTransformed.p[1],triProjected.p[1],this->projectionMatrix);
                    MultiplyMatrixVector(triTransformed.p[2],triProjected.p[2],this->projectionMatrix);

                    //Offset
                    Vect3d vOffsetView = {1,1,0};
                    triProjected.p[0] = triProjected.p[0] + vOffsetView;
                    triProjected.p[1] = triProjected.p[1] + vOffsetView;
                    triProjected.p[2] = triProjected.p[2] + vOffsetView;
                    
                    VectorMultiplyFloat(triProjected.p[0],0.4f * this->windowWidth);
                    VectorMultiplyFloat(triProjected.p[1],0.4f * this->windowWidth);
                    VectorMultiplyFloat(triProjected.p[2],0.4f * this->windowWidth);
                
                    // Add to the final project triangles
                    triProjected.color = color;
                    projectedTriangles.push_back(triProjected);
                }
            }

            // Raster the triangles
            projectedTriangles = sortTriangleList(projectedTriangles);

            // Draw the triangle on the server side if showGraphics is true 
            
            if (this->showGraphics){
                cleardevice();
                for (Triangle tri : projectedTriangles){
                    line(tri.p[0].x,tri.p[0].y,tri.p[1].x,tri.p[1].y);
                    line(tri.p[1].x,tri.p[1].y,tri.p[2].x,tri.p[2].y);
                    line(tri.p[2].x,tri.p[2].y,tri.p[0].x,tri.p[0].y);
                }
            }
            return listTrianglesToString(projectedTriangles);
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
};