#include <cstdlib>
#include <cstdio>
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <err.h>
#include <string.h>

#include <X11/X.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/XKBlib.h>
#include <X11/Xatom.h>
#include <stdio.h>
#include <err.h>

static Display* dpy;
static int scr;
static Window root;

#define POSX 500
#define POSY 500 
#define WIDTH 1920 
#define HEIGHT 1280 
#define BORDER 15 


struct RGB
{
    float r=0,g=0,b=0;
};


class Image{
    private:
        int width=0, height=0;
        std::vector<RGB> matrix;

    public:
        Image(){};

        Image(int width, int height){
            this->width = width;
            this->height = height;
        }

        void setWidth(int width){
            this->width = width;
        }

        void setHeight(int height){
            this->height = height;
        }

        void newPixel(float r,float g, float b){
            this->matrix.push_back({r,g,b});
        }

        int getWidth(){
            return this->width;
        }

        int getHeight(){
            return this->height;
        }

        RGB getPixelAt(int r, int c){
            return this->matrix[(r * width)+c];
        }
};



Image readPPM(const char *filename){
    std::ifstream ifs;
    ifs.open(filename,std::ios::binary);

    if (ifs.fail()){
        err(1,"Can't open file");
    }


    // Image 
    Image image = Image();

    std::string header;
    int w,h,b;

    // Read the header
    ifs >> header;
    if (strcmp(header.c_str(),"P6") != 0){
        err(1,"Can't read input file");
    }

    ifs >> w >> h >> b;
    image.setWidth(w);
    image.setHeight(h);

    // Start loading data
    ifs.ignore(256,'\n');
    unsigned char pix[3];

    for (int i=0; i < w* h ; ++i){
        ifs.read(reinterpret_cast<char *>(pix),3);
        image.newPixel(pix[0] / 1.f,pix[1] / 1.f, pix[2] / 1.f);
    }
    ifs.close();
    return image;
}

/*

    Creates  a window 
*/
Window create_win(int posX,int posY,int width,int height, int border){
    Window win;
    XSetWindowAttributes xwa;
    xwa.background_pixel = WhitePixel(dpy,scr);
    xwa.border_pixel = BlackPixel(dpy,scr);
    xwa.event_mask = Button1MotionMask | ButtonPressMask | ButtonReleaseMask | KeyPressMask;

    return XCreateWindow(dpy,root,posX,posY,width,height,border,DefaultDepth(dpy,scr),InputOutput,DefaultVisual(dpy,scr),
        CWBackPixel | CWBorderPixel | CWEventMask,&xwa);
}

unsigned long _RGB(int r, int g, int b){
    return b + (g<<8) + (r<<16);
}

GC create_gc(int lineWidth){
    GC gc;
    XGCValues xgcv;
    xgcv.line_style = LineSolid;
    xgcv.line_width = lineWidth;
    xgcv.cap_style = CapButt;
    xgcv.join_style = JoinMiter;
    xgcv.fill_style = FillSolid;
    xgcv.foreground = _RGB(255,0,127);
    xgcv.background = WhitePixel(dpy,scr);

    long valuemask = GCForeground | GCBackground | GCFillStyle | GCLineStyle | GCLineWidth | GCCapStyle | GCJoinStyle;
    gc = XCreateGC(dpy,root,valuemask,&xgcv);
    return gc;
}

GC simple_gc(long color){
    GC gc;
    XGCValues xgcv;
    xgcv.line_style = LineSolid;
    xgcv.line_width = 1;
    xgcv.cap_style = CapButt;
    xgcv.join_style = JoinMiter;
    xgcv.fill_style = FillSolid;
    xgcv.foreground = color;
    xgcv.background = WhitePixel(dpy,scr);

    long valuemask = GCForeground | GCBackground | GCFillStyle | GCLineStyle | GCLineWidth | GCCapStyle | GCJoinStyle;
    gc = XCreateGC(dpy,root,valuemask,&xgcv);
    return gc;
}

/*
    Cleanup window 
*/
void cleanUpWindow(Window &win){
    // Close Window and  Cleanup resoucerse 
    XUnmapWindow(dpy,win);
    XDestroyWindow(dpy,win);
    XCloseDisplay(dpy);

}

/*

    Run
*/
void run(Image &image){
    XEvent ev;
    int prev_x = 0 , prev_y = 0 ;
    int init = 0 ;
    GC gc = create_gc(4);

    // Catch events
    while (XNextEvent(dpy,&ev) == 0 ){
        switch (ev.type)
        {
            // When button is pressed
            case ButtonPress:
                if (ev.xbutton.button == Button1){
                    XDrawPoint(dpy,ev.xbutton.window, gc, ev.xbutton.x, ev.xbutton.y);
                }
                break;
            // On Mouse Move 
            case MotionNotify:
                if (init){
                    XDrawLine(dpy,ev.xbutton.window, gc,prev_x,prev_y, ev.xbutton.x,ev.xbutton.y);
                }
                else {
                    XDrawPoint(dpy,ev.xbutton.window, gc, ev.xbutton.x, ev.xbutton.y);
                    init = 1 ;
                }
                prev_x = ev.xbutton.x;
                prev_y = ev.xbutton.y;
                break;
            // When button is released
            case ButtonRelease:
                init = 0 ;
                break;
            // Get Single Key Press 
            case KeyPress:
                if (XkbKeycodeToKeysym(dpy,ev.xkey.keycode,0,0) == XK_q){
                    return;
                }
                else if (XkbKeycodeToKeysym(dpy,ev.xkey.keycode,0,0) == XK_t){

                    XPoint points[] = {
                        {5,5},
                        {100,100},
                        {5,100},
                        {5,5}
                    };
                    int npoints = sizeof(points)/sizeof(XPoint);

                    XSetLineAttributes(dpy,gc,1,LineSolid,CapButt,JoinMiter);
                    // Draw triangle 
                    XDrawLines(dpy,ev.xbutton.window,gc,points,npoints,CoordModeOrigin);
                    

                    //XDrawLines(dpy,ev.xbutton.window,gc,0,0,100,100,0,100);
                }
                else if (XkbKeycodeToKeysym(dpy,ev.xkey.keycode,0,0) == XK_c){

                    XClearWindow(dpy,ev.xbutton.window);
            
                }
                else if (XkbKeycodeToKeysym(dpy,ev.xkey.keycode,0,0) == XK_r){

                    XSetForeground(dpy,gc,_RGB(255,0,0));
            
                }
                else if (XkbKeycodeToKeysym(dpy,ev.xkey.keycode,0,0) == XK_g){

                    XSetForeground(dpy,gc,_RGB(0,255,0));
            
                }
                else if (XkbKeycodeToKeysym(dpy,ev.xkey.keycode,0,0) == XK_p){

                    for (int r =0 ; r != image.getHeight(); ++r){
                        for (int c=0; c != image.getWidth(); ++c){
                            // Do not draw if out of bounds 
                            if (r > HEIGHT){
                                continue;
                            }
                            if (c > WIDTH){
                                continue;
                            }
                            
                            RGB px = image.getPixelAt(r,c);
                            XDrawPoint(dpy,ev.xbutton.window,simple_gc(_RGB(px.r,px.g,px.b)),r,c);
                        }
                    }
            
                }
                break;


        }
    }
    
}


// Program Start
int main(){
    // Create connection to display and get root window  
    dpy = XOpenDisplay(NULL);
    if (dpy == NULL)
        err(1,"Can't open display");

    scr = DefaultScreen(dpy);
    root = RootWindow(dpy,scr);
    
    // Load and display image
    Image image = readPPM("./demo.ppm");
    // Create window and map it to the display 
    int win_width = std::min(image.getWidth(),WIDTH);
    int win_height = std::min(image.getHeight(),HEIGHT);
    Window win = create_win(POSX,POSY,win_width,win_height,BORDER);

    // Name Window 
    XStoreName(dpy,win,"Demonstration X11");
    XMapWindow(dpy,win);

    // Run Window
    run(image);
    
    // Cleanup 
    cleanUpWindow(win);
    return 0 ;

}