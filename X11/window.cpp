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
#define WIDTH 800 
#define HEIGHT 800 
#define BORDER 15 


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
void run(){
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

                    XDrawPoint(dpy,ev.xbutton.window,simple_gc(_RGB(0,0,0)),0,0);
            
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

    // Create window and map it to the display 
    Window win = create_win(POSX,POSY,WIDTH,HEIGHT,BORDER);

    // Name Window 
    XStoreName(dpy,win,"Demonstration X11");
    XMapWindow(dpy,win);

    // Run Window
    run();
    
    // Cleanup 
    cleanUpWindow(win);
    return 0 ;

}