#include <graphics.h>
#include <iostream>
#include <thread>
#include <chrono>

float x = 10, y = 10;

void drawCirlce(){
    x += 0.1f;
    y += 0.1f;
    circle(x,y,10);
    
}

int main(){
    int gd=DETECT,gm; 
    initgraph(&gd,&gm,NULL);
    
    while (true){
        cleardevice();
        drawCirlce();
        std::this_thread::sleep_for(std::chrono::milliseconds(16));
        
    }

    // closegraph function closes
    // the graphics mode and
    // deallocates all memory
    // allocated by graphics system
    getch();
  
    // Close the initialized gdriver
    closegraph();
}