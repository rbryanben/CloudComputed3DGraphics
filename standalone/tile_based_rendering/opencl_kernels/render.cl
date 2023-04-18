#pragma OPENCL EXTENSION cl_intel_printf : enable
#pragma OPENCL EXTENSION cl_khr_global_int32_base_atomics : enable
#define NULL ((void*)0)


#define LOCK(a) atom_cmpxchg(a, 0, 1)
#define UNLOCK(a) atom_xchg(a, 0)

// Vect2d
typedef struct cl_Vect2d{
    float u,v,w;
}cl_Vect2d;

// Vect3d 
typedef struct cl_Vect3d{
    float x,y,z,w;
} cl_Vect3d;

// RGB 
typedef struct cl_RGB
{
    float r,g,b;
} cl_RGB;

// Image
typedef struct cl_Image{
    int width,height;
    cl_RGB* matrix;
} cl_Image;

// Triangle 
typedef struct cl_Triangle
{
    cl_Vect3d p[3];
    cl_Vect2d t[3];
    float avg_depth;
    int texture;
    bool hasData;
} cl_Triangle;

typedef struct cl_Triangle_ArrayList{
    int size;
    cl_Triangle data[2];
}cl_Triangle_ArrayList;

typedef struct cl_Pixel_Texture_Out{
    float textureV;
    float textureU;
    int textureNumber;
    float depth;
}cl_Pixel_Texture_Out;

typedef struct cl_GridDetails{  
    int rows;
    int cols;
    int square_width;
}cl_GridDetails;

// Returns the min of three float
float cl_minf(float* a,float* b, float* c){
    float min = *a;
    if (*b < min) min = *b;
    if (*c < min) min = *c;
    return min;
}

// Returns the max of three float
float cl_maxf(float* a,float* b, float* c){
    float max = *a;
    if (*b > max) max = *b;
    if (*c > max) max = *c;
    return max;
}


// Given position an (x,y) co-ordinate and grid details object - returns the location if the co-ordinate in the grid  
cl_Vect2d getGridPosition(float x,float y,__global cl_GridDetails* gridDetails){
    cl_Vect2d res;
    res.u = (int) x / gridDetails->square_width;
    res.v = (int) y / gridDetails->square_width;
    return res;
}



// Initializes  triangle arrayList
void cl_Triangle_ArrayList_init(cl_Triangle_ArrayList* arr){
    arr->size = 0 ;
}

// Appends element to a triangle arraylist 
void cl_Triangle_ArrayList_Append(cl_Triangle_ArrayList* list,cl_Triangle tri){
    list->data[list->size] = tri;
    list->size++;
}

// cl_Vect3d ArrayList 
typedef struct cl_Vect3d_ArrayList{
    cl_Vect3d data[3];
    int size;
} cl_Vect3d_ArrayList;

// Initializes Vect3d arraylist
void cl_Vect3d_ArrayList_init(cl_Vect3d_ArrayList* arr){
    arr->size = 0;
}

// Adds element to a Vect3D arraylist
void cl_Vect3d_ArrayList_Append(cl_Vect3d_ArrayList* arr,cl_Vect3d vect){
    arr->data[arr->size] = vect;
    arr->size++;
}

// cl_Vect2d ArrayList 
typedef struct cl_Vect2d_ArrayList{
    cl_Vect2d data[3];
    int size;
}cl_Vect2d_ArrayList;


void cl_Vect2d_ArrayList_init(cl_Vect2d_ArrayList* arr){
    arr->size = 0;
}

void cl_Vect2d_ArrayList_Append(cl_Vect2d_ArrayList* arr,cl_Vect2d vect){
    arr->data[arr->size] = vect;
    arr->size++;
}

// Matrix4x4 
// 4x4 Matrix
typedef struct cl_Matrix4x4{
    float m[4][4];
} cl_Matrix4x4;

///
/// For debugging
///
void cl_TrianglePrintVertices(cl_Triangle triangle){
    int id = get_global_id(0);
    // Print the triangle 
    printf("Triangle %i P1 {%f,%f,%f,%f} \nTriangle %i P2 {%f,%f,%f,%f} \nTriangle %i P3 {%f,%f,%f,%f} hasData %i \n\n",id,triangle.p[0].x,triangle.p[0].y,triangle.p[0].z,triangle.p[0].w,
    id,triangle.p[1].x,triangle.p[1].y,triangle.p[1].z,triangle.p[1].w,
    id,triangle.p[2].x,triangle.p[2].y,triangle.p[2].z,triangle.p[2].w,
    triangle.hasData);
}

void cl_TrianglePrintTexs(cl_Triangle triangle){
    int id = get_global_id(0);
    printf("Triangle %i T1 {%f,%f,%f} \nTriangle %i T2 {%f,%f,%f} \nTriangle %i T3 {%f,%f,%f} \nTriangle %i T %i \n\n",
    id,triangle.t[0].u,triangle.t[0].v,triangle.t[0].w
    ,id,triangle.t[1].u,triangle.t[1].v,triangle.t[1].w
    ,id,triangle.t[2].u,triangle.t[2].v,triangle.t[2].w
    ,id,triangle.texture);
}

void cl_Vect3dPrint(cl_Vect3d vect){
    printf("cl_Vect3d {%f,%f,%f} \n",vect.x,vect.y,vect.z);
}


// Vector subtract 
cl_Vect3d cl_Vect3dSubtract(cl_Vect3d v1,cl_Vect3d v2){
    cl_Vect3d res;
    res.x = v1.x - v2.x;
    res.y = v1.y - v2.y;
    res.z = v1.z - v2.z;

    return res;
}


// Cross Producut
cl_Vect3d cl_Vect3dCrossProduct(cl_Vect3d line1, cl_Vect3d line2){
    cl_Vect3d res;
    res.x = (line1.y * line2.z) - (line1.z * line2.y);
    res.y = (line1.z * line2.x) - (line1.x * line2.z);
    res.z = (line1.x * line2.y) - (line1.y * line2.x);
    return res;
}

// Structure to hold 2 lists of projected and original points 
typedef struct cl_TrianglesHolder {
    cl_Triangle_ArrayList original; 
    cl_Triangle_ArrayList projected; 
} cl_TrianglesHolder;

// Initialize a Triangles holder object 
void cl_TrianglesHolder_init(cl_TrianglesHolder* holder){
    cl_Triangle_ArrayList_init(&holder->original);
    cl_Triangle_ArrayList_init(&holder->projected);
}

// Vector Length
float VectorLength(cl_Vect3d* vector){
    return sqrt(vector->x * vector->x + vector->y * vector->y + vector->z * vector->z);
}

// Normalize Vector (To Screen Space)
void NormalizeVector(cl_Vect3d* vector){
    float l = VectorLength(vector);
    vector->x = vector->x / l;
    vector->y = vector->y / l;
    vector->z = vector->z / l;
}

// Normalize Vector (To Screen Space)
cl_Vect3d NormalizeVector_NoRef(cl_Vect3d* vector){
    cl_Vect3d res;
    res.x = vector->x;
    res.y = vector->y;
    res.z = vector->z;
    res.w = vector->w;

    float l = VectorLength(&res);
    res.x = res.x / l;
    res.y = res.y / l;
    res.z = res.z / l;
    
    return res;
}

// Gets the camera translation given a cl_Matrix4x4 
cl_Vect3d getCameraTranslation(cl_Matrix4x4* cameraMatrix){
    cl_Vect3d res;
    res.y = cameraMatrix->m[3][1];
    res.z = cameraMatrix->m[3][2];
    res.x = cameraMatrix->m[3][0];
    res.w = 1;
    return res;
}

// Vector Dot Product
float VectorDotProduct(cl_Vect3d* a,cl_Vect3d* b){
    return a->x * b->x + a->y * b->y + a->z * b->z;
}


cl_Vect3d cl_Vect3dMultiplyFloat(cl_Vect3d* vec,float* t){
    cl_Vect3d res ;
    res.x = vec->x * *t;
    res.y = vec->y * *t;
    res.z = vec->z * *t;
    return res;
}

// Checks if a value is in a given range
bool inRange(float value,int start_value,int end_value){
    if (value >= start_value && value <= end_value)
        return true;
    return false;
}

// Invert Matrix 
cl_Matrix4x4 Matrix_QuickInverse(cl_Matrix4x4* m) // Only for Rotation/Translation Matrices
{
    cl_Matrix4x4 matrix;
    matrix.m[0][0] = m->m[0][0]; matrix.m[0][1] = m->m[1][0]; matrix.m[0][2] = m->m[2][0]; matrix.m[0][3] = 0.0f;
    matrix.m[1][0] = m->m[0][1]; matrix.m[1][1] = m->m[1][1]; matrix.m[1][2] = m->m[2][1]; matrix.m[1][3] = 0.0f;
    matrix.m[2][0] = m->m[0][2]; matrix.m[2][1] = m->m[1][2]; matrix.m[2][2] = m->m[2][2]; matrix.m[2][3] = 0.0f;
    matrix.m[3][0] = -(m->m[3][0] * matrix.m[0][0] + m->m[3][1] * matrix.m[1][0] + m->m[3][2] * matrix.m[2][0]);
    matrix.m[3][1] = -(m->m[3][0] * matrix.m[0][1] + m->m[3][1] * matrix.m[1][1] + m->m[3][2] * matrix.m[2][1]);
    matrix.m[3][2] = -(m->m[3][0] * matrix.m[0][2] + m->m[3][1] * matrix.m[1][2] + m->m[3][2] * matrix.m[2][2]);
    matrix.m[3][3] = 1.0f;
    return matrix;
}

/*  Multiply 2 Matrix and Vector */
float MultiplyMatrixVector(cl_Vect3d* i, cl_Vect3d* o, cl_Matrix4x4* m){
    o->x = i->x * m->m[0][0] + i->y * m->m[1][0] + i->z * m->m[2][0] + m->m[3][0]; 		
    o->y = i->x * m->m[0][1] + i->y * m->m[1][1] + i->z * m->m[2][1] + m->m[3][1]; 		
    o->z = i->x * m->m[0][2] + i->y * m->m[1][2] + i->z * m->m[2][2] + m->m[3][2]; 		
    float w = i->x * m->m[0][3] + i->y * m->m[1][3] + i->z * m->m[2][3] + m->m[3][3]; 		// is simply z 
    o->w = w;

     // Normalize Z - as Z is larger x and y get smaller
    if (w != 0.0f) { 			
        o->x /= w; 
        o->y /= w; 
        o->z /= w; 		
    } 	
    return w;
}

void cl_Matrix4x4Print(cl_Matrix4x4* m){
    printf("|%f,%f,%f,%f|\n|%f,%f,%f,%f|\n|%f,%f,%f,%f|\n|%f,%f,%f,%f|\n\n",
    m->m[0][0],m->m[0][1],m->m[0][2],m->m[0][3],
    m->m[1][0],m->m[1][1],m->m[1][2],m->m[1][3],
    m->m[2][0],m->m[2][1],m->m[2][2],m->m[2][3],
    m->m[3][0],m->m[3][1],m->m[3][2],m->m[3][3]);
}

void copyTextureFrom(cl_Triangle* out,cl_Triangle* triangle){
    out->t[0] = triangle->t[0];
    out->t[1] = triangle->t[1];
    out->t[2] = triangle->t[2];
    
    // Copy texture reference
    out->texture = triangle->texture;
}

//Returns shortest distance from point to a plane
float dist(cl_Vect3d* p,cl_Vect3d* plane_p,cl_Vect3d* plane_n){
    cl_Vect3d n = NormalizeVector_NoRef(p);
    return (plane_n->x * p->x + plane_n->y * p->y + plane_n->z * p->z - VectorDotProduct(plane_n,plane_p));
};

// Adds two cl_Vect3d's
cl_Vect3d cl_Vect3dAdd(cl_Vect3d* a,cl_Vect3d* b){
    cl_Vect3d res;
    res.x = a->x + b->x;
    res.y = a->y + b->y;
    res.z = a->z + b->z;
    res.w = 1;
    return res;
}

// Swaps two float addresses 
void swap(float* a,float* b){
    float temp = *a;
    *a = *b;
    *b = temp;
}

// Calculates where a vector intersects a plane
cl_Vect3d vectorIntersectPlane(cl_Vect3d plane_p, cl_Vect3d plane_n, cl_Vect3d* lineStart,cl_Vect3d* lineEnd,float* t){
    NormalizeVector(&plane_n);
    float plane_d = -VectorDotProduct(&plane_n,&plane_p);
    float ad = VectorDotProduct(lineStart,&plane_n);
    float bd = VectorDotProduct(lineEnd,&plane_n);
    *t = (-plane_d - ad) / (bd - ad);
    cl_Vect3d lineStartToEnd = cl_Vect3dSubtract(*lineEnd,*lineStart);
    cl_Vect3d lineIntersect = cl_Vect3dMultiplyFloat(&lineStartToEnd,t);
    return cl_Vect3dAdd(lineStart,&lineIntersect);
}


/*
    Clips a triangle against a plane. 
*/
cl_TrianglesHolder clipTriangleAgainstPlane(cl_Vect3d* plane_p,cl_Vect3d* plane_n,cl_Triangle* in_tri, cl_Triangle* og_in_tri){
    // Result
    cl_TrianglesHolder res_holder; 
    cl_TrianglesHolder_init(&res_holder);

    // Result vector - a vector of triangle vectors where index 0 will contained clipped of projected triangles and 1 will contain
    //      clipped of original triangles  
    cl_Triangle_ArrayList res_proj;
    cl_Triangle_ArrayList res_og;
    cl_Triangle_ArrayList_init(&res_proj);
    cl_Triangle_ArrayList_init(&res_og);

    // Out tri's 
    cl_Triangle out_tri1,out_tri2;
    cl_Triangle og_out_tri1, og_out_tri2; 
    
    // Totongogara ta coper ma texture 
    //     og pretty much doesnt care about texture 
    out_tri1.texture = in_tri->texture;
    out_tri2.texture = in_tri->texture;

    //Normalize plane normal
    NormalizeVector(plane_n);

     //Create two temporary storage arrays to classify points either side of the plane
    //if the distance is positibe, point lies inside the plane
    cl_Vect3d_ArrayList inside_points;
    cl_Vect3d_ArrayList og_inside_points; 
    cl_Vect3d_ArrayList outside_points;
    cl_Vect3d_ArrayList og_outside_points;
    cl_Vect2d_ArrayList inside_texture;
    cl_Vect2d_ArrayList outside_texture;
    cl_Vect3d_ArrayList_init(&inside_points);
    cl_Vect3d_ArrayList_init(&og_inside_points);
    cl_Vect3d_ArrayList_init(&outside_points);
    cl_Vect3d_ArrayList_init(&og_outside_points);
    cl_Vect2d_ArrayList_init(&inside_texture);
    cl_Vect2d_ArrayList_init(&outside_texture);

    // get signed distance from each point in triangle to the plane
    float d0 = dist(&in_tri->p[0],plane_p,plane_n);
    float d1 = dist(&in_tri->p[1],plane_p,plane_n);
    float d2 = dist(&in_tri->p[2],plane_p,plane_n);

    if (d0 >= 0){
        cl_Vect3d_ArrayList_Append(&inside_points,in_tri->p[0]);
        cl_Vect3d_ArrayList_Append(&og_inside_points,og_in_tri->p[0]);
        cl_Vect2d_ArrayList_Append(&inside_texture,in_tri->t[0]);
    }
    else {
        cl_Vect3d_ArrayList_Append(&outside_points,in_tri->p[0]);
        cl_Vect3d_ArrayList_Append(&og_outside_points,og_in_tri->p[0]);
        cl_Vect2d_ArrayList_Append(&outside_texture,in_tri->t[0]);
    }

    if (d1 >= 0){
        cl_Vect3d_ArrayList_Append(&inside_points,in_tri->p[1]);
        cl_Vect3d_ArrayList_Append(&og_inside_points,og_in_tri->p[1]);
        cl_Vect2d_ArrayList_Append(&inside_texture,in_tri->t[1]);
    }
    else {
        cl_Vect3d_ArrayList_Append(&outside_points,in_tri->p[1]);
        cl_Vect3d_ArrayList_Append(&og_outside_points,og_in_tri->p[1]);
        cl_Vect2d_ArrayList_Append(&outside_texture,in_tri->t[1]);
    }


    if (d2 >= 0){
        cl_Vect3d_ArrayList_Append(&inside_points,in_tri->p[2]);
        cl_Vect3d_ArrayList_Append(&og_inside_points,og_in_tri->p[2]);
        cl_Vect2d_ArrayList_Append(&inside_texture,in_tri->t[2]);
    }
    else {
        cl_Vect3d_ArrayList_Append(&outside_points,in_tri->p[2]);
        cl_Vect3d_ArrayList_Append(&og_outside_points,og_in_tri->p[2]);
        cl_Vect2d_ArrayList_Append(&outside_texture,in_tri->t[2]);
    }

    // Dont draw any 
    if (inside_points.size == 0) {
        return res_holder;
    }

    // All points are inside
    if (inside_points.size == 3){
        cl_Triangle_ArrayList_Append(&res_proj,*in_tri);
        cl_Triangle_ArrayList_Append(&res_og,*og_in_tri);
        res_holder.projected = res_proj;
        res_holder.original = res_og;
        return res_holder;
    }

    // Two points outside - form one new triangle 
    if (inside_points.size == 1 && outside_points.size == 2){
        float t1,t2,t3 = 0.f;

        //inside point is valid we keep that 
        out_tri1.p[0] = inside_points.data[0];
        og_out_tri1.p[0] = og_inside_points.data[0];


         //but the two new points are at the locations where the 
	    //original sides of the triangle (lines) intersect with the plane
        out_tri1.p[1] = vectorIntersectPlane(*plane_p,*plane_n,&inside_points.data[0],&outside_points.data[0],&t1);
        out_tri1.p[2] = vectorIntersectPlane(*plane_p,*plane_n,&inside_points.data[0],&outside_points.data[1],&t2);

        // Original points
        og_out_tri1.p[1].x = og_inside_points.data[0].x + (t1 * (og_outside_points.data[0].x - og_inside_points.data[0].x));
        og_out_tri1.p[1].y = og_inside_points.data[0].y + (t1 * (og_outside_points.data[0].y - og_inside_points.data[0].y));
        og_out_tri1.p[1].z = og_inside_points.data[0].z + (t1 * (og_outside_points.data[0].z - og_inside_points.data[0].z));
        
        og_out_tri1.p[2].x = og_inside_points.data[0].x + (t2 * (og_outside_points.data[1].x - og_inside_points.data[0].x));
        og_out_tri1.p[2].y = og_inside_points.data[0].y + (t2 * (og_outside_points.data[1].y - og_inside_points.data[0].y));
        og_out_tri1.p[2].z = og_inside_points.data[0].z + (t2 * (og_outside_points.data[1].z - og_inside_points.data[0].z));

        // Add texture 
        out_tri1.t[0] = inside_texture.data[0];
        out_tri1.t[1].u = inside_texture.data[0].u + (t1 * (outside_texture.data[0].u - inside_texture.data[0].u));
        out_tri1.t[1].v = inside_texture.data[0].v + (t1 * (outside_texture.data[0].v - inside_texture.data[0].v));
        out_tri1.t[1].w = inside_texture.data[0].w + (t1 * (outside_texture.data[0].w - inside_texture.data[0].w));
        out_tri1.t[2].u = inside_texture.data[0].u + (t2 * (outside_texture.data[1].u - inside_texture.data[0].u));
        out_tri1.t[2].v = inside_texture.data[0].v + (t2 * (outside_texture.data[1].v - inside_texture.data[0].v));
        out_tri1.t[2].w = inside_texture.data[0].w + (t2 * (outside_texture.data[1].w - inside_texture.data[0].w));

        // Create lists
        cl_Triangle_ArrayList_Append(&res_proj,out_tri1);
        cl_Triangle_ArrayList_Append(&res_og,og_out_tri1);

        // Add to result holder
        res_holder.projected = res_proj;
        res_holder.original = res_og;

        return res_holder;
    }

    // One outside point - form two new triangles 
    if (inside_points.size ==2 && outside_points.size == 1){
        float t1,t2,t3;

        //The first triangle consists of the two inside points and a new
		//point determined by the location where one side of the triangle
		//intersects with the plane
        out_tri1.p[0] = inside_points.data[0];
        og_out_tri1.p[0] = og_inside_points.data[0];

        out_tri1.p[1] = inside_points.data[1];
        og_out_tri1.p[1] = og_inside_points.data[1];

        //out_tri1.p[2] = vectorIntersectPlane(*plane_p,*plane_n,&inside_points.data[0],&outside_points.data[1],&t2);
        out_tri1.p[2] = vectorIntersectPlane(*plane_p,*plane_n,&inside_points.data[1],&outside_points.data[0],&t1);
        og_out_tri1.p[2].x = og_inside_points.data[1].x + (t1 * (og_outside_points.data[0].x - og_inside_points.data[1].x));
        og_out_tri1.p[2].y = og_inside_points.data[1].y + (t1 * (og_outside_points.data[0].y - og_inside_points.data[1].y));
        og_out_tri1.p[2].z = og_inside_points.data[1].z + (t1 * (og_outside_points.data[0].z - og_inside_points.data[1].z));

        out_tri1.t[0] = inside_texture.data[0];
        out_tri1.t[1] = inside_texture.data[1];
        out_tri1.t[2].u = inside_texture.data[1].u + (t1 * (outside_texture.data[0].u - inside_texture.data[1].u));
        out_tri1.t[2].v = inside_texture.data[1].v + (t1 * (outside_texture.data[0].v - inside_texture.data[1].v));
        out_tri1.t[2].w = inside_texture.data[1].w + (t1 * (outside_texture.data[0].w - inside_texture.data[1].w));

        //form the other triangle
        out_tri2.p[0] = inside_points.data[0];
        og_out_tri2.p[0] = og_inside_points.data[0];
        out_tri2.t[0] = inside_texture.data[0];

        out_tri2.p[1] = vectorIntersectPlane(*plane_p,*plane_n,&inside_points.data[1],&outside_points.data[0],&t2);

        og_out_tri2.p[1].x = og_inside_points.data[1].x + (t2 * (og_outside_points.data[0].x - og_inside_points.data[1].x));
        og_out_tri2.p[1].y = og_inside_points.data[1].y + (t2 * (og_outside_points.data[0].y - og_inside_points.data[1].y));
        og_out_tri2.p[1].z = og_inside_points.data[1].z + (t2 * (og_outside_points.data[0].z - og_inside_points.data[1].z));
        
        out_tri2.t[1].u = inside_texture.data[1].u + (t2 * (outside_texture.data[0].u - inside_texture.data[1].u));
        out_tri2.t[1].v = inside_texture.data[1].v + (t2 * (outside_texture.data[0].v - inside_texture.data[1].v));
        out_tri2.t[1].w = inside_texture.data[1].w + (t2 * (outside_texture.data[0].w - inside_texture.data[1].w));
        
        out_tri2.p[2] = vectorIntersectPlane(*plane_p,*plane_n,&inside_points.data[0],&outside_points.data[0],&t3);

        og_out_tri2.p[2].x = og_inside_points.data[0].x + (t3 * (og_outside_points.data[0].x - og_inside_points.data[0].x));
        og_out_tri2.p[2].y = og_inside_points.data[0].y + (t3 * (og_outside_points.data[0].y - og_inside_points.data[0].y));
        og_out_tri2.p[2].z = og_inside_points.data[0].z + (t3 * (og_outside_points.data[0].z - og_inside_points.data[0].z));

        out_tri2.t[2].u = inside_texture.data[0].u + (t3 * (outside_texture.data[0].u - inside_texture.data[0].u));
        out_tri2.t[2].v = inside_texture.data[0].v + (t3 * (outside_texture.data[0].v - inside_texture.data[0].v));
        out_tri2.t[2].w = inside_texture.data[0].w + (t3 * (outside_texture.data[0].w - inside_texture.data[0].w));
        
        cl_Triangle_ArrayList_Append(&res_proj,out_tri1);
        cl_Triangle_ArrayList_Append(&res_proj,out_tri2);
        cl_Triangle_ArrayList_Append(&res_og,og_out_tri1);
        cl_Triangle_ArrayList_Append(&res_og,og_out_tri2);


        res_holder.projected = res_proj;
        res_holder.original = res_og;

        return res_holder;

    }

}


/*
    Given a cl_Triangle and grid_details. Returns the squares in the grid a triangle passes through 
*/
void cl_TriangleSetGridSquares(__global cl_GridDetails* gridDetails,cl_Triangle* triangle,int posInSharedBuffer,__global int* subGridRasterMax,__global int* vertex_shader_to_rasterizer){
    // Get furthest vertices 
    float farLeft = cl_minf(&triangle->p[0].x,&triangle->p[1].x,&triangle->p[2].x);
    float farRight = cl_maxf(&triangle->p[0].x,&triangle->p[1].x,&triangle->p[2].x);
    float farTop = cl_minf(&triangle->p[0].y,&triangle->p[1].y,&triangle->p[2].y);
    float farBottom = cl_maxf(&triangle->p[0].y,&triangle->p[1].y,&triangle->p[2].y);

    // Get squares for far's 
    cl_Vect2d s_topLeft = getGridPosition(farLeft,farTop,gridDetails);
    cl_Vect2d s_topRight = getGridPosition(farRight,farTop,gridDetails);
    cl_Vect2d s_bottomLeft = getGridPosition(farLeft,farBottom,gridDetails);
    cl_Vect2d s_bottomRight = getGridPosition(farRight,farBottom,gridDetails);


    // For each possible square the triangle covers 
    for (int row = (int)s_topLeft.v; row <= (int) s_bottomRight.v; row++){
        for (int col = (int) s_topLeft.u; col <= (int) s_bottomRight.u; col++){

            // Start index always contains the number of triangles in the sub grid to raster
            int startIndex = ((gridDetails->rows * row) + col)  * *subGridRasterMax; 
            
            // check if the limit of tringles per subgrid has been reached (but the number of items will keep incrementing)
            if (vertex_shader_to_rasterizer[startIndex] >=  *subGridRasterMax - 1) continue;

            // increment start index to indicate a new triangle being added
            int items = atomic_inc(&vertex_shader_to_rasterizer[startIndex]);            

            // add triangle to the rasterizer
            vertex_shader_to_rasterizer[startIndex + items + 1] = posInSharedBuffer;
        }
    }
}

/*
    Texture cl_Triangle
*/
void texturedTriangle(__private cl_Triangle* tri,__private cl_Triangle* orignal_triangle,int windowWidth,int windowHeight,cl_Vect2d topLeftPoint,__global cl_GridDetails* grid_details
    ,__global cl_Pixel_Texture_Out* out_pixels){

    // Points vertex and texture 
    int x1 = tri->p[0].x;
    int y1 = tri->p[0].y;
    float u1 = tri->t[0].u;
    float v1 = tri->t[0].v;

    int x2 = tri->p[1].x;
    int y2 = tri->p[1].y; 
    float u2 = tri->t[1].u;
    float v2 = tri->t[1].v;

    int x3 = tri->p[2].x;
    int y3 = tri->p[2].y;
    float u3 = tri->t[2].u;
    float v3 = tri->t[2].v;


    // Depth valuew s 
    float w1 = tri->t[0].w ;
    float w2 = tri->t[1].w ;
    float w3 = tri->t[2].w ;

    // Original points 
    float t_x1 = orignal_triangle->p[0].x;
    float t_y1 = orignal_triangle->p[0].y;
    float t_z1 = orignal_triangle->p[0].z;
    
    float t_x2 = orignal_triangle->p[1].x;
    float t_y2 = orignal_triangle->p[1].y;
    float t_z2 = orignal_triangle->p[1].z;

    float t_x3 = orignal_triangle->p[2].x;
    float t_y3 = orignal_triangle->p[2].y;
    float t_z3 = orignal_triangle->p[2].z;            

    
    // Sort Points 
    if (y2 < y1){
        swap(&y2,&y1);
        swap(&x2,&x1);
        swap(&u2,&u1);
        swap(&v2,&v1);
        swap(&w2,&w1);

        // Swap original points 
        swap(&t_y2,&t_y1);
        swap(&t_x2,&t_x1);
        swap(&t_z2,&t_z1);
    }
    

    if (y3 < y1){
        swap(&y3,&y1);
        swap(&x3,&x1);
        swap(&u3,&u1);
        swap(&v3,&v1);
        swap(&w3,&w1);

        // Swap original points
        swap(&t_y3,&t_y1);
        swap(&t_x3,&t_x1);
        swap(&t_z3,&t_z1);
    }
    
    if (y3 < y2){
        swap(&y3,&y2);
        swap(&x3,&x2);
        swap(&u3,&u2);
        swap(&v3,&v2);
        swap(&w3,&w2);

        // Swap original points
        swap(&t_y3,&t_y2);
        swap(&t_x3,&t_x2);
        swap(&t_z3,&t_z2);
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
        // Interpolate from the top to the bottom
        for (int i = y1; i < y2;i++){
            // Check if y is in the grid range 
            if (!inRange(i,topLeftPoint.v,topLeftPoint.v + grid_details->square_width)){
                continue;
            }


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
                swap(&ax,&bx);
                swap(&tex_su,&tex_eu);
                swap(&tex_sv,&tex_ev);
                swap(&tex_sw,&tex_ew);

                // Swap the original co-ordinates
                swap(&t_ax,&t_bx);
                swap(&t_az,&t_bz);
                swap(&t_ay,&t_by);
            }

            // For texture co-ordinate calculations
            tex_u = tex_su;
            tex_v = tex_sv;
            tex_w = tex_sw;

            float tstep = 1.0f / ((float)(bx - ax));
            float t = 0.0f;

            // Interpolate from the the left to the right
            for (int j = ax; j <= bx; j++){
                // Check if j is in range of the grid x 
                if (!inRange(j,topLeftPoint.u,topLeftPoint.u + grid_details->square_width)){
                    t += tstep; 
                    continue;
                } 

                // Do not consider the position (x,y) -> (j,i) if it is outside the frame 

                tex_u = (1.0f - t) * tex_su + t * tex_eu;
                tex_v = (1.0f - t) * tex_sv + t * tex_ev;
                tex_w = (1.0f - t) * tex_sw + t * tex_ew;

                float tx = (1.0f - t) * t_ax + t * t_bx;
                float tz = (1.0f - t) * t_az + t * t_bz;
                float ty = (1.0f - t) * t_ay + t * t_by;

                // Point in world space 
                cl_Vect3d pointInWorldSpace = {tx,ty,tz,1};
            
                // Get the frame position 
                int framePosition = i * windowWidth + j;

                // Create the pixel to assign 
                cl_Pixel_Texture_Out pixelOut = {(tex_v / tex_w),(tex_u / tex_w),tri->texture,tex_w};

                // Check if the depth is greater than that in the frame
                if (tex_w > out_pixels[framePosition].depth){
                    out_pixels[framePosition] = pixelOut;
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
            // Check if y is in the grid range 
            if (!inRange(i,topLeftPoint.v,topLeftPoint.v + grid_details->square_width)){
                continue;
            }

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
                swap(&ax, &bx);
                swap(&tex_su, &tex_eu);
                swap(&tex_sv, &tex_ev);
                swap(&tex_sw, &tex_ew);

                // Swap the original co-ordinates
                swap(&t_ax,&t_bx);
                swap(&t_az,&t_bz);
                swap(&t_ay,&t_by);

            }

            tex_u = tex_su;
            tex_v = tex_sv;
            tex_w = tex_sw;


            float tstep = 1.0f / ((float)(bx - ax));
            float t = 0.0f;

            for (int j = ax; j <= bx; j++)
            {
                // Check if j is in range of the grid x 
                if (!inRange(j,topLeftPoint.u,topLeftPoint.u + grid_details->square_width)) {
                    t += tstep;
                    continue;
                }

                tex_u = (1.0f - t) * tex_su + t * tex_eu;
                tex_v = (1.0f - t) * tex_sv + t * tex_ev;
                tex_w = (1.0f - t) * tex_sw + t * tex_ew;

                float tx = (1.0f - t) * t_ax + t * t_bx;
                float tz = (1.0f - t) * t_az + t * t_bz;
                float ty = (1.0f - t) * t_ay + t * t_by;

                cl_Vect3d pointInWorldSpace = {tx,ty,tz,1};

                /** Write to image */
                int pixelPosition = i * windowWidth + j;
                cl_Pixel_Texture_Out pixelOut = {(tex_v / tex_w),(tex_u / tex_w),tri->texture,tex_w};

                // Check if there is a lock applied 
                if (tex_w > out_pixels[pixelPosition].depth){
                    out_pixels[pixelPosition] = pixelOut;
                }

                t += tstep;
            }
        }	
    }		

}

// Returns the average depth of a cl_Triangle
float cl_TriangleGetAvgDepth(cl_Triangle* tri){
    return (tri->p[0].z + tri->p[1].z + tri->p[2].z) / 3;
};

void swapi(__global int* a,__global int *b){
    int temp = *a;
    *a = *b;
    *b = temp;
}

void quickSort(__global int* arr,int start, int end,__global cl_Triangle* triangles){
    if (start >= end) return;

    //pivot
    int pivot = arr[start];
    float pivotDepth = triangles[pivot].avg_depth;
    
    // Set left and right pointer 
    int l = start + 1;
    int r = end;

    //partion items 
    while (l <= r){
        // move left pointer till we find a depth value greater than the pivot Depth
        while (l <= r && triangles[arr[l]].avg_depth <= pivotDepth){
            l++;
        }

        //move right pointer till we find a value less than the pivot 
        while (l <= r && triangles[arr[r]].avg_depth > pivotDepth){
            r--;
        }

        if (l <= r){
            swapi(&arr[l++],&arr[r--]);
        }

    }

    swapi(&arr[start],&arr[r]);

    quickSort(arr,r+1,end,triangles);
    quickSort(arr,start,r-1,triangles);
    
}


/* 
    Texture Method 
*/
__kernel void texture(__global int* vertex_shader_to_rasterizer,__global int* sub_grid_raster_max,__global cl_Triangle* triangles,__global cl_GridDetails* grid_details,__global cl_Pixel_Texture_Out* out_tiles){
    // Max Sub Grid 
    private int subGridRasterMax = *sub_grid_raster_max; 

    // Get the id 
    int workID = get_global_id(0);

    // Create a vect 3d to hold the top left corner of the grid
    int row = workID / grid_details->rows;
    int col = workID % grid_details->rows;

    // Top left corner of grid 
    private cl_Vect2d topLeftPoint = {col * grid_details->square_width,row * grid_details->square_width,1};

    // Get the start index of triangles to render.
    // The start index contains the number of triangles that are in the next <sub_grid_raster_max - 1> index's.
    int startIndex = workID * subGridRasterMax;
    int items = vertex_shader_to_rasterizer[startIndex];

    // Sort the triangles first 
    int end = startIndex + items;
    int limit= startIndex + subGridRasterMax;
    end = min(end,limit);
    
    quickSort(vertex_shader_to_rasterizer,startIndex + 1,end,triangles);

    // Raster triangles assigned
    for (int i=startIndex + 1; i <= end; i++){
      

        // Get the position of the triangle in the triangles buffer. This is because <vertex_shader_to_rasterize> stores the positions
        //  of triangles in the triangles buffer 
        int trianglePosInBuffer = vertex_shader_to_rasterizer[i];

        // Make the triangle private 
        private cl_Triangle triangleProjected = triangles[trianglePosInBuffer];
  
        texturedTriangle(&triangleProjected,&triangleProjected,800,800,topLeftPoint,grid_details,out_tiles);    
    }    
}



/*
    Render method is respnsible for rendering a triangle 
 */
__kernel void render(__global cl_Triangle* triangles_in,__global cl_Matrix4x4* camera_matrix,__global cl_Matrix4x4* projection_matrix,__global int* window_width,
    __global int* window_height,__global cl_Triangle* triangles_out,__global int* triangles_out_count,__global cl_GridDetails* gridDetails,__global int* vertex_shader_to_rasterizer,
    __global int* subGridRasterMax){
    
    //
    // Window width
    int windowWidth = *window_width;
    int windowHeight = *window_height;

    // 
    // Camera 
    cl_Matrix4x4 cameraMatrix = *camera_matrix; 
    //
    // Projection Matrix
    cl_Matrix4x4 projectionMatrix = *projection_matrix; 

    //Offset to the center of the screen 
    cl_Vect3d vOffsetView = {1,1,0,1};

    // Get the triangle to work on 
    int id = get_global_id(0);    
    cl_Triangle tri = triangles_in[id];

    //
    // Holds projected and triangle references 
    cl_Triangle_ArrayList projectedTriangles;
    cl_Triangle_ArrayList triangleReferences; 
    cl_Triangle_ArrayList_init(&projectedTriangles);
    cl_Triangle_ArrayList_init(&triangleReferences);

    //
    // Rendering begins 

    // Determine the normal so that we do not draw triangles that are not facing us
    cl_Vect3d normal,line1,line2 ;
    
    line1 = cl_Vect3dSubtract(tri.p[1],tri.p[0]);
    line2 = cl_Vect3dSubtract(tri.p[2],tri.p[0]);

    normal = cl_Vect3dCrossProduct(line1,line2);
    NormalizeVector(&normal);  

    // Camera Ray Which is the difference between triTransformed.p[0] and vCamera
    //  is the vector from the camera to the triangle. We wish to calculate the angle inbetween to see if its visible 
    cl_Vect3d cameraTranslation = getCameraTranslation(&cameraMatrix);
    cl_Vect3d vCameraRay = cl_Vect3dSubtract(tri.p[0],cameraTranslation);

    // Check if visible 
    // If not visible then free the unit of its work  
    if ((VectorDotProduct(&vCameraRay,&normal) < 0) == false){    
        return;          
    }
    
    // Camera Pespective 
    cl_Matrix4x4 matViewd = Matrix_QuickInverse(&cameraMatrix);

    //
    // Camera - From here the entire view is inversed to the camera's view 
    cl_Triangle triViewd; 
    MultiplyMatrixVector(&tri.p[0],&triViewd.p[0],&matViewd);
    MultiplyMatrixVector(&tri.p[1],&triViewd.p[1],&matViewd);
    MultiplyMatrixVector(&tri.p[2],&triViewd.p[2],&matViewd);

    // Copy texture co-ordinate 
    copyTextureFrom(&triViewd,&tri); 

    // Clip the triangles against the z axis
    cl_Vect3d normal_vec = {0,0,0.1f,1};
    cl_Vect3d point = {0,0,1,1};
    cl_TrianglesHolder clippedZ = clipTriangleAgainstPlane(&point,&normal_vec,&triViewd,&tri);

    // Project clippedZ points
    cl_Triangle triProjected;

    // Position to place the result 
    int pos = get_global_id(0) * 4;

    // Project
    for (int i=0;i !=clippedZ.projected.size;i++){
        cl_Triangle tri = clippedZ.projected.data[i];
        cl_Triangle og = clippedZ.original.data[i]; 

        //
        // Project the polygons(Some normalization takes place here)
        //      At this point we project the 3D co-ordinates on to a 3D scene
        float w0 = MultiplyMatrixVector(&tri.p[0],&triProjected.p[0],&projectionMatrix);
        float w1 = MultiplyMatrixVector(&tri.p[1],&triProjected.p[1],&projectionMatrix);
        float w2 = MultiplyMatrixVector(&tri.p[2],&triProjected.p[2],&projectionMatrix);

        // Copy the texture information to triProjected from triTransformed
        copyTextureFrom(&triProjected,&tri);

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
        triProjected.p[0] = cl_Vect3dAdd(&triProjected.p[0],&vOffsetView);
        triProjected.p[1] = cl_Vect3dAdd(&triProjected.p[1],&vOffsetView);
        triProjected.p[2] = cl_Vect3dAdd(&triProjected.p[2],&vOffsetView);

        // Scale to the window width  
        //  this centeres the triangle
        float scale = (0.5f * windowWidth);
        triProjected.p[0] = cl_Vect3dMultiplyFloat(&triProjected.p[0],&scale);
        triProjected.p[1] = cl_Vect3dMultiplyFloat(&triProjected.p[1],&scale);
        triProjected.p[2] = cl_Vect3dMultiplyFloat(&triProjected.p[2],&scale);

        cl_Triangle_ArrayList_Append(&triangleReferences,og);
        cl_Triangle_ArrayList_Append(&projectedTriangles,triProjected);
    }  


    // Clip against the screen edges
    for (int pj=0;pj!=projectedTriangles.size;pj++){
        // Clip against the top of the screen 
        cl_Vect3d tPoint = {0,0.1f,0,1};
        cl_Vect3d tNorm = {0,1,0,1};
        cl_TrianglesHolder topClipped = clipTriangleAgainstPlane(&tPoint,&tNorm,&projectedTriangles.data[pj],&triangleReferences.data[pj]);

        /** {Im sorry for nesting. OpenCl did not allow adding of dynamic arrays} */
        // Clip against the bottom of the screen
        // For triangle in topClipped 
        for(int tc=0;tc!=topClipped.projected.size;tc++){
            cl_Vect3d bPoint = {0,(float)windowHeight - 1,0,1};
            cl_Vect3d bNorm = {0,-1,0};
            cl_TrianglesHolder bottomClipped =  clipTriangleAgainstPlane(&bPoint,&bNorm,&topClipped.projected.data[tc],&topClipped.original.data[tc]);

            // Clip against the  left of the screen
            for (int lc=0;lc!=bottomClipped.projected.size;lc++){
                cl_Vect3d lPoint = {1,0,0,1};
                cl_Vect3d lNorm = {1,0,0,1};
                cl_TrianglesHolder leftClipped = clipTriangleAgainstPlane(&lPoint,&lNorm,&bottomClipped.projected.data[lc],&bottomClipped.original.data[lc]);

                for (int rc=0;rc != leftClipped.projected.size;rc++){
                    cl_Vect3d rPoint = {windowWidth - 1,0,0,1};
                    cl_Vect3d rNorm = {-1,0,0,1};
                    cl_TrianglesHolder rightClipped = clipTriangleAgainstPlane(&rPoint,&rNorm,&leftClipped.projected.data[rc],&leftClipped.original.data[rc]);

                    // Output final clipped items
                    for (int rClipped=0;rClipped != rightClipped.projected.size;rClipped++){
                        // Add triangle to the out buffer 
                        cl_Triangle out = rightClipped.projected.data[rClipped];

                        // increment start index to indicate a new triangle being added
                        int pos = atomic_inc(triangles_out_count); 

                        //calculate average depth
                        out.avg_depth = cl_TriangleGetAvgDepth(&rightClipped.original.data[rClipped]);
                        
                        // Insert into shared buffer 
                        triangles_out[pos] = out;

                        // Assign triangle to rasterizer grid 
                        cl_TriangleSetGridSquares(gridDetails,&out,pos,subGridRasterMax,vertex_shader_to_rasterizer);
                    }
                }
                
            }
        }
    }
}