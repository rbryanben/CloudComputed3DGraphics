#ifndef directional_light.h
#define directional_light.h
// imports 
#include <GL/freeglut.h>
#include <GL/gl.h>
#include "../common/common.h";
#include "../common/matrix_operations.h";


// The directional light is a 4x4 Matrix 
#include "../common/common.h"
#include "../common/matrix_operations.h"

struct W3DirectionalLight {
    // Primary resolution, projection matrix and depth buffer  
    Resolution primaryResolution;
    Matrix4x4 primaryProjectionMatrix, transformationMatrix;
    Vect3d vOffsetView = {1,1,0};
    float* primaryDepthBuffer; 
    bool hasRendered = false;
    
    W3DirectionalLight(Resolution resolution,float fieldView,float zFar,float zNear);
    W3DirectionalLight(){};
    void translate(Vect3d translation);
    Vect3d getTranslation();
    float* render(vector<Triangle> &triangleList,bool render_once); 
    void renderToDepthBuffer(Triangle &tri);
    bool isPointVisible(Vect3d &point);
    void rotateY(float angle); 
    void rotateX(float angle);
    void rotateZ(float angle);  
};


// Constructor - Which requires primary resolution values, that is the height
//   and the width. 
W3DirectionalLight::W3DirectionalLight(Resolution resolution,float fieldView,float zFar,float zNear){
    // Set primary resolution
    this->primaryResolution = resolution;  
    // Create projection matrix
    this->primaryProjectionMatrix = Make_Projection(resolution.height/resolution.width,fieldView,zFar,zNear);
    // Create depth buffer 
    this->primaryDepthBuffer = new float[resolution.width * resolution.height];
    // Create the transformationMatrix
    this->transformationMatrix =  Matrix4x4_MakeIdentity();
};

// Translates the directional light to a point in 3D space 
void W3DirectionalLight::translate(Vect3d translation){
    this->transformationMatrix.m[3][0] = translation.x;
    this->transformationMatrix.m[3][1] = -translation.y;
    this->transformationMatrix.m[3][2] = translation.z;
}

Vect3d W3DirectionalLight::getTranslation(){
    return {this->transformationMatrix.m[3][0],this->transformationMatrix.m[3][1],this->transformationMatrix.m[3][2],1};
}

// Render method
float* W3DirectionalLight::render(vector<Triangle> &triangleList,bool render_once=false){
    // Since the scene is static, render once only
    if (render_once == true){
        if (hasRendered) return primaryDepthBuffer;
    }
    

    // Prototype Light Buffer 
    Matrix4x4 lightViewd = Matrix_QuickInverse(this->transformationMatrix);
    vector<Triangle> lightProjected;
    Triangle triProjected;

    // Iterate triangles 
    for (Triangle tri: triangleList){
        // Determine the normal so that we do not draw triangles that are not facing us
        Vect3d normal = getTriangleNormal(tri);
        NormalizeVector(normal);
        // Light ray to triangle 
        Vect3d vLightRay = tri.p[0] - this->getTranslation();

        // Check if the triangle is visible 
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
                float w0 = MultiplyMatrixVector(tri.p[0],triProjected.p[0],this->primaryProjectionMatrix);
                float w1 = MultiplyMatrixVector(tri.p[1],triProjected.p[1],this->primaryProjectionMatrix);
                float w2 = MultiplyMatrixVector(tri.p[2],triProjected.p[2],this->primaryProjectionMatrix);

                triProjected.t[0].w = 1.f / w0;
                triProjected.t[1].w = 1.f / w1;
                triProjected.t[2].w = 1.f / w2;  

                // Offset to the center
                triProjected.p[0] = triProjected.p[0] + vOffsetView;
                triProjected.p[1] = triProjected.p[1] + vOffsetView;
                triProjected.p[2] = triProjected.p[2] + vOffsetView;

                // Scale to the window width  
                //  this centeres the triangle
                triProjected.p[0] = triProjected.p[0] * (0.5f * this->primaryResolution.width);
                triProjected.p[1] = triProjected.p[1] * (0.5f * this->primaryResolution.width);
                triProjected.p[2] = triProjected.p[2] * (0.5f * this->primaryResolution.width);

                // Add to vertex_list 
                lightProjected.push_back(triProjected);
            }
        }

    }
    
    // Clear depth buffer 
    for (int i=0;i != this->primaryResolution.width * this->primaryResolution.height; i++){
        this->primaryDepthBuffer[i] = 0;
    }
    
    // Render 
    for (Triangle tri: lightProjected){
        this->renderToDepthBuffer(tri);
    }

    // Set mode as rendered 
    hasRendered = true;

    return this->primaryDepthBuffer;
};

void W3DirectionalLight::renderToDepthBuffer(Triangle &tri){
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
                if ( i >= 0 && j >= 0 && i * this->primaryResolution.width + j < this->primaryResolution.height * this->primaryResolution.width 
                    && tex_w > this->primaryDepthBuffer[i * this->primaryResolution.width + j]){
                    this->primaryDepthBuffer[i * this->primaryResolution.width + j] = tex_w;
                    glColor3f(tex_w,tex_w,tex_w);
                    glVertex2i(j,i);
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
                
                if (i >= 0 && j >= 0 && i * this->primaryResolution.width + j < this->primaryResolution.height * this->primaryResolution.width 
                    && tex_w >  this->primaryDepthBuffer[i * this->primaryResolution.width + j]){
                    this->primaryDepthBuffer[i * this->primaryResolution.width + j] = tex_w;
                    glColor3f(tex_w,tex_w,tex_w);
                    glVertex2i(j,i);
                }

                t += tstep;
            }
        }	
    }	
    glEnd();
}

bool W3DirectionalLight::isPointVisible(Vect3d &point){

    // Check if the point is visible from the light source
    // Prototype Light Buffer 
    Vect3d vOffsetView = {1,1,0};
    Matrix4x4 lightViewd = Matrix_QuickInverse(this->transformationMatrix);
    Vect3d pointViewd,pointProjected;

    MultiplyMatrixVector(point,pointViewd,lightViewd);
    //
    // Project the polygons(Some normalization takes place here)
    //      At this point we project the 3D co-ordinates on to a 3D scene
    float w0 = MultiplyMatrixVector(pointViewd,pointProjected,this->primaryProjectionMatrix);
    float depth = 1.f / w0;

    // Offset to the center
    pointProjected = pointProjected + vOffsetView;
    
    // Scale to the window width  
    //  this centeres the triangle
    pointProjected = pointProjected * (0.5f * this->primaryResolution.width);
    int pos = ((int)pointProjected.y * this->primaryResolution.width) + (int)pointProjected.x;
    
    // If  x or why is negative then dont considerer 
    if (pointProjected.x < 0 || pointProjected.y < 0 || 
        pointProjected.x > this->primaryResolution.width || pointProjected.y > this->primaryResolution.height)
    {
        return false;
    }
    
    float depthInDlightDepthBuffer = this->primaryDepthBuffer[pos];
    
    if (areEqual(depthInDlightDepthBuffer,depth,0.0002f)){
        return true;
    }

    return false;
}


void W3DirectionalLight::rotateY(float angle_deg){
    // Rotation mat
    Matrix4x4 rot = getMatrixRotationY(((22.f/7.f) * angle_deg) / 180);
    // Original Translation
    Vect3d originalTranslation = this->getTranslation();
    // Rotate 
    this->transformationMatrix = MatrixMultiplyMatrix(this->transformationMatrix,rot);
    // Restore transalation
    this->translate(originalTranslation);
    this->hasRendered = false;
}

void W3DirectionalLight::rotateX(float angle_deg){
    // Rotation mat
    Matrix4x4 rot = getMatrixRotationX(((22.f/7.f) * angle_deg) / 180);
    // Original Translation
    Vect3d originalTranslation = this->getTranslation();
    // Rotate 
    this->transformationMatrix = MatrixMultiplyMatrix(this->transformationMatrix,rot);
    // Restore transalation
    this->translate(originalTranslation);
    this->hasRendered = false;
}

void W3DirectionalLight::rotateZ(float angle_deg){
    // Rotation mat
    Matrix4x4 rot = getMatrixRotationZ(((22.f/7.f) * angle_deg) / 180);
    // Original Translation
    Vect3d originalTranslation = this->getTranslation();
    // Rotate 
    this->transformationMatrix = MatrixMultiplyMatrix(this->transformationMatrix,rot);
    // Restore transalation
    this->translate(originalTranslation);
    this->hasRendered = false;
}



#endif