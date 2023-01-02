#ifndef common_h
#define common_h
#include <vector>
#include <math.h>
#include <iostream>
#include <string.h>
#include <stdlib.h>
#include <strstream>
using namespace std;

//Vect3d 
struct Vect3d{
    float x,y,z;
    float w = 1.0f;
    // Overload subtration
    Vect3d operator - (Vect3d other){
        Vect3d res;
        res.x = this->x - other.x;
        res.y = this->y - other.y;
        res.z = this->z - other.z;

        return res;
    }  

    Vect3d operator + (Vect3d other){
        Vect3d res;
        res.x = this->x + other.x;
        res.y = this->y + other.y;
        res.z = this->z + other.z;

        return res;
    }  

    Vect3d operator * (float multiplier){
        return {this->x * multiplier, this->y * multiplier, this->z * multiplier, 1};
    }
};

struct Vect2d{
    float u,v;
    float w = 1 ;
};

struct Color
{
    float red = 0.0f,green=0.0f,blue = 0.0f;
};

struct Triangle
{
    Vect3d p[3];
    Vect2d t[3];
    Color color ;
};


struct Mesh
{
    vector<Triangle> triangles;

	bool LoadFromObjectFile(string sFilename, bool bHasTexture = false)
	{
		ifstream f(sFilename);
		if (!f.is_open())
			return false;

		// Local cache of verts
		vector<Vect3d> verts;
		vector<Vect2d> texs;

		while (!f.eof())
		{
			char line[128];
			f.getline(line, 128);

			strstream s;
			s << line;

			char junk;

			if (line[0] == 'v')
			{
				if (line[1] == 't')
				{
					Vect2d  v;
					s >> junk >> junk >> v.u >> v.v;
					// A little hack for the spyro texture
					//v.u = 1.0f - v.u;
					//v.v = 1.0f - v.v;
					texs.push_back(v);
				}
				else
				{
					Vect3d v;
					s >> junk >> v.x >> v.y >> v.z;
					verts.push_back(v);
				}
			}

			if (!bHasTexture)
			{
				if (line[0] == 'f')
				{
					int f[3];
					s >> junk >> f[0] >> f[1] >> f[2];
					triangles.push_back({ verts[f[0] - 1], verts[f[1] - 1], verts[f[2] - 1] });
				}
			}
			else
			{
				if (line[0] == 'f')
				{
					s >> junk;

					string tokens[6];
					int nTokenCount = -1;


					while (!s.eof())
					{
						char c = s.get();
						if (c == ' ' || c == '/')
							nTokenCount++;
						else
							tokens[nTokenCount].append(1, c);
					}

					tokens[nTokenCount].pop_back();


					triangles.push_back({ verts[stoi(tokens[0]) - 1], verts[stoi(tokens[2]) - 1], verts[stoi(tokens[4]) - 1],
						texs[stoi(tokens[1]) - 1], texs[stoi(tokens[3]) - 1], texs[stoi(tokens[5]) - 1] });

				}

			}
		}
		return true;
	}
};

struct Matrix4x4{
    float m[4][4]= {0.0f};

    double getHash(){
        return 
            m[0][0] + m[0][1] + m[0][2] + m[0][3] +
            m[1][0] + m[1][1] + m[1][2] + m[1][3] + 
            m[2][0] + m[2][1] + m[2][2] + m[2][3] + 
            m[3][0] + m[3][1] + m[3][2] + m[3][3];  
    }

    Matrix4x4 operator * (Matrix4x4 b){
        Matrix4x4 res; 

        res.m[0][0] = b.m[3][0]*m[0][3]+m[0][0]*b.m[0][0]+m[0][1]*b.m[1][0]+m[0][2]*b.m[2][0]; 
        res.m[0][1] = b.m[3][1]*m[0][3]+m[0][0]*b.m[0][1]+m[0][1]*b.m[1][1]+m[0][2]*b.m[2][1]; 
        res.m[0][2] = b.m[2][2]*m[0][2]+b.m[3][2]*m[0][3]+m[0][0]*b.m[0][2]+m[0][1]*b.m[1][2]; 
        res.m[0][3] = b.m[2][3]*m[0][2]+b.m[3][3]*m[0][3]+m[0][0]*b.m[0][3]+m[0][1]*b.m[1][3]; 


        res.m[1][0] = b.m[3][0]*m[1][3]+m[1][1]*b.m[1][0]+m[1][2]*b.m[2][0]+b.m[0][0]*m[1][0];
        res.m[1][1] = b.m[3][1]*m[1][3]+m[1][1]*b.m[1][1]+m[1][2]*b.m[2][1]+b.m[0][1]*m[1][0];
        res.m[1][2] = b.m[2][2]*m[1][2]+b.m[3][2]*m[1][3]+m[1][1]*b.m[1][2]+b.m[0][2]*m[1][0];
        res.m[1][3] = b.m[2][3]*m[1][2]+b.m[3][3]*m[1][3]+m[1][1]*b.m[1][3]+b.m[0][3]*m[1][0];


        res.m[2][0] = b.m[3][0]*m[2][3]+m[2][0]*b.m[0][0]+m[2][1]*b.m[1][0]+m[2][2]*b.m[2][0]; 
        res.m[2][1] = b.m[3][1]*m[2][3]+m[2][0]*b.m[0][1]+m[2][1]*b.m[1][1]+m[2][2]*b.m[2][1]; 
        res.m[2][2] = b.m[2][2]*m[2][2]+b.m[3][2]*m[2][3]+m[2][0]*b.m[0][2]+m[2][1]*b.m[1][2]; 
        res.m[2][3] = b.m[2][3]*m[2][2]+b.m[3][3]*m[2][3]+m[2][0]*b.m[0][3]+m[2][1]*b.m[1][3];
        
        res.m[3][0] = b.m[3][0]*m[3][3]+m[3][0]*b.m[0][0]+m[3][1]*b.m[1][0]+m[3][2]*b.m[2][0]; 
        res.m[3][1] = b.m[3][1]*m[3][3]+m[3][0]*b.m[0][1]+m[3][1]*b.m[1][1]+m[3][2]*b.m[2][1]; 
        res.m[3][2] = b.m[2][2]*m[3][2]+b.m[3][2]*m[3][3]+m[3][0]*b.m[0][2]+m[3][1]*b.m[1][2]; 
        res.m[3][3] = b.m[2][3]*m[3][2]+b.m[3][3]*m[3][3]+m[3][0]*b.m[0][3]+m[3][1]*b.m[1][3];

        return res ;
    } 
};


string vectorToString(Vect3d vector){
    return "v " +  to_string(vector.x) + " " + to_string(vector.y) + " " + to_string(vector.z); 
}

string triangleToString(Triangle triangle){
    return "t " + vectorToString(triangle.p[0]) + " " +  vectorToString(triangle.p[1]) + " " + vectorToString(triangle.p[2]);
}

string colorToString(Color color){
    return to_string(color.red)+" "+to_string(color.green)+" "+to_string(color.blue);
}

string listTrianglesToString(vector<Triangle> triangleList){
    string res = "";
    for (Triangle triangle: triangleList){
        res += (triangleToString(triangle) + " c " + colorToString(triangle.color) + "/n");
    }

    return res;
}


float f_max(float a,float b){
    if (a > b) return a ;
    return b;
}

float f_min(float a, float b){
    if (a < b) return a;
    return b;
}


// Cross Producut
Vect3d VectorCrossProduct(Vect3d line1, Vect3d line2){
    Vect3d res;
    res.x = (line1.y * line2.z) - (line1.z * line2.y);
    res.y = (line1.z * line2.x) - (line1.x * line2.z);
    res.z = (line1.x * line2.y) - (line1.y * line2.x);

    return res;
}

// Vector Length
float VectorLength(Vect3d vector){
    return sqrt(vector.x * vector.x + vector.y * vector.y + vector.z * vector.z);
}


// Normalize Vector (To Screen Space)
void NormalizeVector(Vect3d &vector){
    float l = VectorLength(vector);
    vector.x /= l;
    vector.y /= l;
    vector.z /= l;
}

// Vector Dot Product
float VectorDotProduct(Vect3d a,Vect3d b){
    return a.x * b.x + a.y * b.y + a.z * b.z;
}

// Vector Multiply (For Scaling)
void VectorMultiplyFloat(Vect3d &vector,float f){
    vector.x *= f ;
    vector.y *= f ;
    vector.z *= f ;
}


// Vector multiply float return new vector
Vect3d VectorMultiplyFloat_Return(Vect3d &vector,float f){
    Vect3d res;
    res.x *= f ;
    res.y *= f ;
    res.z *= f ;

    return res;
}


// Vector Devide (For Scaling)
void VectorDevideFloat(Vect3d &vector,float f){
    vector.x /= f ;
    vector.y /= f ;
    vector.z /= f ;
}


// Triangle Depth
float getTriangleDepth(Triangle &triangle){
    return (triangle.p[0].z + triangle.p[1].z + triangle.p[2].z) / 3 ;
}

// Sort Triangle List
vector<Triangle> sortTriangleList(vector<Triangle> triangleList){
    if (triangleList.size() <= 1) return triangleList;
    
    // Pop back as pivot 
    Triangle pivot = triangleList.back();
    triangleList.pop_back();

    // Lesser and greater
    vector<Triangle> lesser,greater;

    //find lesser and greater
    while (triangleList.size() > 0){
        // Pop back
        Triangle tri = triangleList.back();
        triangleList.pop_back();

        //check if it is greater(deeper)
        if (getTriangleDepth(tri) > getTriangleDepth(pivot)){
            greater.push_back(tri);
        }
        else {
            lesser.push_back(tri);
        }
    }

    //sort greater
    greater = sortTriangleList(greater);
    lesser = sortTriangleList(lesser);

    //Join the lists
    greater.push_back(pivot);
    greater.insert(greater.end(),lesser.begin(),lesser.end());
    return greater;
}

Vect3d VectorNormalize(Vect3d &v1){
    float l = VectorLength(v1);
    if (l != 0){
        return {v1.x / l , v1.y / l, v1.z / l, 1};
    }
    return {v1.x,v1.y,v1.z,1};
}


// Calculates where a vector intersects a plane
Vect3d vectorIntersectPlane(Vect3d plane_p, Vect3d plane_n, Vect3d lineStart,Vect3d lineEnd){
    plane_n = VectorNormalize(plane_p);
    float plane_d = -VectorDotProduct(plane_n,plane_p);
    float ad = VectorDotProduct(lineStart,plane_n);
    float bd = VectorDotProduct(lineEnd,plane_n);
    float t = (-plane_d - ad) / (bd - ad);
    Vect3d lineStartToEnd = lineEnd - lineStart;
    Vect3d lineIntersect = lineStartToEnd * t ;
    return lineStart + lineIntersect;
}

// Tirangle clip against plane 
int Triangle_ClipAgainstPlane(Vect3d plane_p,Vect3d plane_n,Triangle &in_tri,Triangle &out_tri1,Triangle &out_tri2){
    //make sure normal is indeed normal
    plane_n = VectorNormalize(plane_n);

    //Returns shortest distance from point to a plane
    auto dist = [&](Vect3d &p){
        Vect3d n = VectorNormalize(p);
        return (plane_n.x * p.x + plane_n.y * p.y + plane_n.z * p.z - VectorDotProduct(plane_n,plane_p));
    };

    //Create two temporary storage arrays to classify points either side of the plane
    //if the distance is positibe, point lies inside the plane
    vector<Vect3d> inside_points;
    vector<Vect3d> outside_points;

    // get signed distance from each point in triangle to the plane
    float d0 = dist(in_tri.p[0]);
    float d1 = dist(in_tri.p[1]);
    float d2 = dist(in_tri.p[2]);

    if (d0 >= 0) inside_points.push_back(in_tri.p[0]);
    else outside_points.push_back(in_tri.p[0]);
    if (d1 >= 0) inside_points.push_back(in_tri.p[1]);
    else outside_points.push_back(in_tri.p[1]);
    if (d2 >= 0) inside_points.push_back(in_tri.p[2]);
    else outside_points.push_back(in_tri.p[2]); 

    if (inside_points.size() == 0) return 0; //dont draw any

    if (inside_points.size() == 3){ 
        out_tri1.color = in_tri.color;
        out_tri1.p[0] = in_tri.p[0];
        out_tri1.p[1] = in_tri.p[1];
        out_tri1.p[2] = in_tri.p[2];
        return 1;
    }

    // 2 points are outside
    if (inside_points.size() == 1 && outside_points.size() == 2){
        out_tri1.color = {0,0,1};

        //inside point is valid we keep that 
        out_tri1.p[0] = inside_points[0];

        //but the two new points are at the locations where the 
	    //original sides of the triangle (lines) intersect with the plane
        out_tri1.p[1] = vectorIntersectPlane(plane_p,plane_n,inside_points[0],outside_points[0]);
        out_tri1.p[2] = vectorIntersectPlane(plane_p,plane_n,inside_points[0],outside_points[1]);
        return 1;
    }
    
    if (inside_points.size() == 2 && outside_points.size() == 1){
        out_tri2.color = in_tri.color;
        out_tri1.color = in_tri.color;

        //The first triangle consists of the two inside points and a new
		//point determined by the location where one side of the triangle
		//intersects with the plane
        out_tri1.p[0] = inside_points[0];
        out_tri1.p[1] = inside_points[1];
        out_tri1.p[2] = vectorIntersectPlane(plane_p,plane_n,inside_points[1],outside_points[0]);

        out_tri2.p[0] = inside_points[0];
        out_tri2.p[1] = vectorIntersectPlane(plane_p,plane_n,inside_points[1],outside_points[0]);
        out_tri2.p[2] = vectorIntersectPlane(plane_p,plane_n,inside_points[0],outside_points[0]);

        out_tri1.color = {1,0,0};
        out_tri2.color = {0,1,0};
        return 2;
    }
}



#endif
