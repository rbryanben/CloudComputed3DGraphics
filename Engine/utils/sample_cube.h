#include <vector>
#include "common.h"

using namespace std;

/*
Mesh getCube(){
    Mesh cube;
    cube.triangles = {
        //South
        { 0.0f, 0.0f, 0.0f,   0.0f, 1.0f, 0.0f,    1.0f, 1.0f, 0.0f },
        
        { 0.0f, 0.0f, 0.0f,   1.0f, 1.0f, 0.0f,    1.0f, 0.0f, 0.0f },
        
      
        // EAST 		
        { 1.0f, 0.0f, 0.0f,     1.0f, 1.0f, 0.0f,    1.0f, 1.0f, 1.0f }, 		
        { 1.0f, 0.0f, 0.0f,     1.0f, 1.0f, 1.0f,    1.0f, 0.0f, 1.0f }, 		
        // NORTH 		
        { 1.0f, 0.0f, 1.0f,     1.0f, 1.0f, 1.0f,    0.0f, 1.0f, 1.0f }, 		
        { 1.0f, 0.0f, 1.0f,     0.0f, 1.0f, 1.0f,    0.0f, 0.0f, 1.0f }, 		
      

    }; 
    return cube;
} */


Mesh getCube(){
    Mesh cube;

    Triangle tr1 = {{{0,1,0}, {0,0,0}, {1,1,0}},{{0,0},{1,1},{0,1}}};
    Triangle tr2 = {{{0,0,0}, {1,0,0}, {1,1,0}},{{0,0},{1,1},{1,0}}};

    // North 
    Triangle tr3 = {{{1,1,0}, {1,0,1}, {1,0,0}},{{0,0},{1,1},{0,1}}};
    Triangle tr4 = {{{1,1,0}, {1,1,1}, {1,0,1}},{{0,0},{1,1},{1,0}}};

    // West 
    Triangle tr5 = {{{1,0,1}, {1,1,1}, {0,0,1}},{{0,0},{1,1},{0,1}}};
    Triangle tr6 = {{{0,0,1}, {1,1,1}, {0,1,1}},{{0,0},{1,1},{1,0}}};

    // Top 
    Triangle tr7 = {{{0,0,1}, {0,1,1}, {0,1,0}},{{0,0},{1,1},{0,1}}};
    Triangle tr8 = {{{0,0,1}, {0,1,0}, {0,0,0}},{{0,0},{1,1},{1,0}}};

    // Bottom
    Triangle tr9 = {{{0,0,0}, {0,0,1}, {1,0,0}},{{0,0},{1,1},{0,1}}};
    Triangle tr10 = {{{1,0,0}, {0,0,1}, {1,0,1}},{{0,0},{1,1},{1,0}}};

     // Bottom
    Triangle tr11 = {{{0,1,0}, {0,1,1}, {1,1,0}},{{0,0},{1,1},{0,1}}};
    Triangle tr12 = {{{1,1,0}, {0,1,1}, {1,1,1}},{{0,0},{1,1},{1,0}}};

    cube.triangles.push_back(tr1);
    cube.triangles.push_back(tr2);
    cube.triangles.push_back(tr3);
    cube.triangles.push_back(tr4);
    cube.triangles.push_back(tr5);
    cube.triangles.push_back(tr6);
    cube.triangles.push_back(tr7);

    cube.triangles.push_back(tr8);
    cube.triangles.push_back(tr9);
    cube.triangles.push_back(tr10);
    cube.triangles.push_back(tr11);
    cube.triangles.push_back(tr12);



    return cube;
}