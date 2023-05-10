#include "dune.hpp"

/// @brief Test Application
/// @param argc 
/// @param argv 
/// @return 
int main(int argc, char **argv)
{
    // Engine instance
    W3DGraphics graphicsEngine = W3DGraphics(800,800);

    // Meshs 
    Mesh crate = Mesh("crate");
    crate.LoadFromObjectFile("./assets/objs/crate/Crate1.obj",readPPM("./assets/objs/crate/crate.ppm"));
    graphicsEngine.addToScene(crate);


    Mesh crate2 = Mesh("crate2");
    crate2.LoadFromObjectFile("./assets/objs/crate/Crate1.obj",readPPM("./assets/objs/crate/crate.ppm"));
    crate2.scale(5);
    crate2.translate({0,0,10});
    //graphicsEngine.addToScene(crate2);

    // Basement Entry 
    Mesh basement = Mesh("basement");
    basement.LoadFromObjectFile("./assets/objs/barn/barn.obj",readPPM("./assets/objs/barn/texture.ppm"));
    basement.rotateZ(180);
    basement.translate({0,0,5});
    //graphicsEngine.addToScene(basement);
    

    // Pathwalk 
    Mesh pathwalk = Mesh();
    pathwalk.LoadFromObjectFile("./assets/objs/pathwalk/pathwalk.obj",readPPM("./assets/objs/pathwalk/texture.ppm"));
    pathwalk.rotateZ(180);
    pathwalk.translate({0,0,-10});
    graphicsEngine.addToScene(pathwalk);

    // Pathwalk 
    Mesh pathwalk2 = Mesh();
    pathwalk2.LoadFromObjectFile("./assets/objs/pathwalk/pathwalk.obj",readPPM("./assets/objs/pathwalk/texture.ppm"));
    pathwalk2.rotateZ(180);
    pathwalk2.translate({0,10,-10});
    //graphicsEngine.addToScene(pathwalk2);


     // Pathwalk 
    Mesh pathwalk3 = Mesh();
    pathwalk3.LoadFromObjectFile("./assets/objs/pathwalk/pathwalk.obj",readPPM("./assets/objs/pathwalk/texture.ppm"));
    pathwalk3.rotateZ(180);
    pathwalk3.translate({0,5,-10});
    ///graphicsEngine.addToScene(pathwalk3);

    // Drum 
    Mesh drum = Mesh("drum");
    drum.LoadFromObjectFile("./assets/objs/drum/drum.obj",readPPM("./assets/objs/drum/texture.ppm"));
    drum.rotateZ(180);
    //graphicsEngine.addToScene(drum);

    // Initialize engine
    graphicsEngine.init(argc,argv);

    return 0;
}