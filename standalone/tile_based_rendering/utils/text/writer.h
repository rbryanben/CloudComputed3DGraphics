// writter.cpp 
// This structure is responsible for writting 2D text to the screen
#ifndef writer.h
#define writter.h 

// imports 
#include <GL/freeglut.h>
#include <GL/gl.h>
#include <string.h>
#include "../image/image_loader.h"

struct W3Writer
{
    // Textures 
    vector<Image> textures;

    // Constructor 
    W3Writer(){
        //load textures 
        textures.push_back(readPPM("assets/fonts/cap_a.ppm"));
        textures.push_back(readPPM("assets/fonts/cap_b.ppm"));
        textures.push_back(readPPM("assets/fonts/cap_c.ppm"));
        textures.push_back(readPPM("assets/fonts/cap_d.ppm"));
        textures.push_back(readPPM("assets/fonts/cap_e.ppm"));
        textures.push_back(readPPM("assets/fonts/cap_f.ppm"));
        textures.push_back(readPPM("assets/fonts/cap_g.ppm"));
        textures.push_back(readPPM("assets/fonts/cap_h.ppm"));
        textures.push_back(readPPM("assets/fonts/cap_i.ppm"));
        textures.push_back(readPPM("assets/fonts/cap_j.ppm"));
        textures.push_back(readPPM("assets/fonts/cap_k.ppm"));
        textures.push_back(readPPM("assets/fonts/cap_l.ppm"));
        textures.push_back(readPPM("assets/fonts/cap_m.ppm"));
        textures.push_back(readPPM("assets/fonts/cap_n.ppm"));
        textures.push_back(readPPM("assets/fonts/cap_o.ppm"));
        textures.push_back(readPPM("assets/fonts/cap_p.ppm"));
        textures.push_back(readPPM("assets/fonts/cap_q.ppm"));
        textures.push_back(readPPM("assets/fonts/cap_r.ppm"));
        textures.push_back(readPPM("assets/fonts/cap_s.ppm"));
        textures.push_back(readPPM("assets/fonts/cap_t.ppm"));
        textures.push_back(readPPM("assets/fonts/cap_u.ppm"));
        textures.push_back(readPPM("assets/fonts/cap_v.ppm"));
        textures.push_back(readPPM("assets/fonts/cap_w.ppm"));
        textures.push_back(readPPM("assets/fonts/cap_x.ppm"));
        textures.push_back(readPPM("assets/fonts/cap_y.ppm"));
        textures.push_back(readPPM("assets/fonts/cap_z.ppm"));
        textures.push_back(readPPM("assets/fonts/zero.ppm"));
        textures.push_back(readPPM("assets/fonts/one.ppm"));
        textures.push_back(readPPM("assets/fonts/two.ppm"));
        textures.push_back(readPPM("assets/fonts/three.ppm"));
        textures.push_back(readPPM("assets/fonts/four.ppm"));
        textures.push_back(readPPM("assets/fonts/five.ppm"));
        textures.push_back(readPPM("assets/fonts/six.ppm"));
        textures.push_back(readPPM("assets/fonts/seven.ppm"));
        textures.push_back(readPPM("assets/fonts/eight.ppm"));
        textures.push_back(readPPM("assets/fonts/nine.ppm"));
        textures.push_back(readPPM("assets/fonts/semi.ppm"));
        textures.push_back(readPPM("assets/fonts/dot.ppm"));
    }


    void drawTextureAtPos(int x, int y,int pixels, Image &texture){
        glBegin(GL_POINTS);
        for (int r =0 ; r != pixels - 1; r++){
            for (int c=0; c != pixels - 1; c++){
                // Sample  
                float tex_x = f_min(((float) c / (float) (pixels - 1)),1) * texture.getWidth();  
                float tex_y = f_min(((float) r / (float) (pixels - 1)),1) * texture.getHeight();
                RGB pixel = texture.getPixelAt(tex_y,tex_x);
                if (!(pixel.r < 0.3f && pixel.g < 0.3f && pixel.b < 0.3f)){
                    glColor3f(pixel.r,pixel.g,pixel.b);
                    glVertex2i(c + x,r + y);
                }
            }
        }
        glEnd();
    }

    void print(double x,double y,string text,int pixels=15){
        for (int i=0; i != text.length(); i++){
            // get the ascci number 
            int ascii_number = (int)text[i];
            
            // skip spaces
            if (text[i] == ' '){ 
                continue;
            }

            // dot
            if (text[i] == '.'){
              drawTextureAtPos(x + (i * pixels),y,pixels,textures[37]);
              continue;
            }

            // Number
            if (ascii_number < 59){
              int pos = (ascii_number - 48) + 26;
              drawTextureAtPos(x + (i * pixels),y,pixels,textures[pos]);
              continue;
            }
            
            // Chars
            drawTextureAtPos(x + (i * pixels),y,pixels,textures[ascii_number - 65]);
        }
    }
};

#endif