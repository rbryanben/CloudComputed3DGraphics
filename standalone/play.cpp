#include <stdlib.h>
#include <iostream>

using namespace std;

// Calculates if floating points are equal
bool areEqual(float a, float b, float threshold) {
    if (abs(a - b) <= threshold) return true;
    return false;
}


int main(){
    float a = 0.25f;
    float b = 0.249501005;

    bool res = areEqual(a,b,0.0005f);

    if (res) cout << "are equal";
    else cout << "not equal" ;
}