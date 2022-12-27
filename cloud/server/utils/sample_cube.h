#include <vector>
#include "common.h"

using namespace std;

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
        // WEST 		
        { 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f }, 		
        { 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f }, 		
        // TOP 		
        { 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f }, 		
        { 0.0f, 1.0f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f }, 		
        // BOTTOM 		
        { 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f }, 		
        { 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f },
    }; 
    return cube;
}