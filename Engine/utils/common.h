#ifndef common_h
#define common_h
#include <vector>
#include <math.h>
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

#endif
