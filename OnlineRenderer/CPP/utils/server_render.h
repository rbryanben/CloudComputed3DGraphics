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
            float x = 10, y = 10;
            int gd=DETECT,gm; 
            initgraph(&gd,&gm,NULL);
            //getch();
            //closegraph();
        }

        //OnCreate
        void onCreate(){
            //projection matrix
            float aspectRatio  = this->windowHeight / this->windowWidth;
            float fieldView = 90.0f ;
            float tangentFunction = 1.0f / tanf(fieldView * 0.5f / 180.0f * 3.14159f);
            float zfar = 1000.0f;
            float znear = 0.1f;
            float zfarFunction = (zfar/(zfar-znear)) - ((zfar*znear)/(zfar-znear));
            

            this->projectionMatrix.m[0][0] = aspectRatio * tangentFunction;
            this->projectionMatrix.m[1][1] = tangentFunction;
            this->projectionMatrix.m[2][2] =  zfar/(zfar-znear);
            this->projectionMatrix.m[3][2] = (-zfar*znear)/(zfar-znear);
            this->projectionMatrix.m[2][3] = 1.0f;
        }
        

        //OnRender
        string onRender(float rotationAngle){
            // Final projected colors and triangles 
            vector<Triangle> projectedTriangles;

            //draw triangles 
            for (Triangle tri : this->mesh.triangles){
                
                // Rotate
                Triangle rotatedZ,rotatedX, triProjected,triTranslated;
                Matrix4x4 matRotZ, matRotX;

                matRotZ = getMatrixRotationZ(rotationAngle);
                matRotX = getMatrixRotationX(rotationAngle);
            

                MultiplyMatrixVector(tri.p[0],rotatedZ.p[0],matRotZ);
                MultiplyMatrixVector(tri.p[1],rotatedZ.p[1],matRotZ);
                MultiplyMatrixVector(tri.p[2],rotatedZ.p[2],matRotZ);
                
                MultiplyMatrixVector(rotatedZ.p[0],rotatedX.p[0],matRotX);
                MultiplyMatrixVector(rotatedZ.p[1],rotatedX.p[1],matRotX);
                MultiplyMatrixVector(rotatedZ.p[2],rotatedX.p[2],matRotX);
                
                
                // Translate back
                rotatedX.p[0].z += 5.0f;
                rotatedX.p[1].z += 5.0f;
                rotatedX.p[2].z += 5.0f;


                triTranslated = rotatedX;   

                //  Determine the normal so that we do not draw triangles that are not facing us
                Vect3d normal,line1,line2 ;
                line1 = triTranslated.p[1] - triTranslated.p[0];
                line2 = triTranslated.p[2] - triTranslated.p[1]; 
                normal = VectorCrossProduct(line1,line2);
                NormalizeVector(normal);

                if (normal.x * (triTranslated.p[0].x - this->vCamera.x) +
                    normal.y * (triTranslated.p[0].y - this->vCamera.y) + 
                    normal.z * (triTranslated.p[0].z - this->vCamera.z) < 0
                    ){

                    //illumination
                    Vect3d lightSource = {0,0,-1}; 
                    NormalizeVector(lightSource);

                    float dt = VectorDotProduct(normal,lightSource);
                    Color color = {abs(dt*255),abs(dt*255),abs(dt*255)};


                    //
                    // If z is negetive
                    MultiplyMatrixVector(triTranslated.p[0],triProjected.p[0],this->projectionMatrix);
                    MultiplyMatrixVector(triTranslated.p[1],triProjected.p[1],this->projectionMatrix);
                    MultiplyMatrixVector(triTranslated.p[2],triProjected.p[2],this->projectionMatrix);

                    //Offset
                    triProjected.p[0].x += 1.0f; triProjected.p[0].y += 1.0f;
                    triProjected.p[1].x += 1.0f; triProjected.p[1].y += 1.0f;
                    triProjected.p[2].x += 1.0f; triProjected.p[2].y += 1.0f; 
                    
                    triProjected.p[0].x *= 0.4f * this->windowWidth;
                    triProjected.p[0].y *= 0.4f * this->windowHeight;
                    triProjected.p[1].x *= 0.4f * this->windowWidth;
                    triProjected.p[1].y *= 0.4f * this->windowHeight;
                    triProjected.p[2].x *= 0.4f * this->windowWidth;
                    triProjected.p[2].y *= 0.4f * this->windowHeight;
                    
                    
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