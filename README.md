# Cloud Computed 3D Graphics

Optimizing Next Generation Cloud Gaming Platforms Through Distributed Rendering (Practical Implementation)

# 1.1 Introduction
There are multiple citations on the internet of cloud gaming being the next big technology in the video game industry, a citation by Emre Can Kartal forecasting that the cloud gaming market can reach $6.3 billion in global consumer spending by 2024. And It should not come as a suprise that current leaders in that field are Nvidia, Sony and Xbox, as these companies have been making gaming devices for a while. After working on a 3D Graphics engine in python based off of the olcGameEngine by Dr David Barr, it became clear that python is not a convinient language to use when writting such a compute intensive application, as a teapot would render at less than 15fps. Which eventually led to the idea of moving some of the 3D rendering pipeline proccesses to a C++ server. In doing so there was a dramatic change in the frame rate, as the teapot was able to rnder at over 30fps in a pygame window. 
Which one would question, would this be a better approach to cloud gaming as compared to the current approach of sending inputs to a remote game server and a video is streamed back to the client?

# 1.2 Background of Study
It just so happens this solution has been proposed before by Pin-Chun Wang and others in paper titled “Optimizing Next-Generation Cloud Gaming Platforms with Planar Map Streaming and Distributed Rendering’’ (Pin-Chun, Ellis, Hart and Cheng-Hsin, n.d.).  
In their paper they adress the limitations with the current solutions and bring foward graphics streaming as a solution to next generation Cloud Gaming. They put cloud gaming platforms into three categories \n
- Video streaming
- Graphics streaming
- Hybrid streaming
    
In video streaming cloud servers are treated like black boxes as they are simply video encoders. All they do is recieve key inputs and stream the game as a video to the client. Which results in several limitations that inlcude high bandwidth, limited scalability and little room for optimization. In their words thay say it “allows cloud gaming service providers to trade gaming experience for time-to-market.”
In graphics streaming game scene data is transfarered or rendering commands are send to the cloud server which it responses with some vertex data on each frame. Which by nature will allow distributation, better compression of data and can be scale to large displays of different resolutions.  
Hybrid streaming is simply a combination of the two, but is said to be hard to implement. 

# 1.3 Literature Review
Looking at a 3D rendering pipeline, there are n major proccesses which are: 
- Proccessing Inputs, this input which contains scene data (made of triangles), textures,  light sources, camera co-ordinates e.t.c.  
- Geometry, which includes  transformations, translation, scaling, rotations if any e.t.c 
- Illuminationation, which is computing the colors of pixels based off light sources. This proccess may be complex as it includes calculations like light tracing.
- Clipping, which deals with clipping triangles that have n co-ordinates behind the camera view. Not doing so will cause serious performance issues.
- Z-Buffering, which is simply prevents a 3D object behind another object to be drawn infront of the object. 
- Projection, converting the 3D co-ordinates to 2D view space.
- Texturing, applying textures to the 2D objects to look realistic. 

Within modern graphics engines there are more proccessesses to these, but these are most fundamental. Loading inputs will run with a time complexity of O(n), where Geometry will definetly run with a linear time complexity,  Rasterization will run with a time complexity of O(nlog n) if we are sorting the triangles first. Drawing and texturing will run with a quadratic time complexity.
Mathematically moving Geometry and Rasterization to a remote server will leave a client with a lower time complexity. Which is the main concept behing graphic streaming.

# 1.4 Problem Definition
The current approach to rendering graphics in cloud gaming may not be the most optimal way to do so as it treats the cloud server in a black box manner. 

# 1.5 Aims and Objectives
The objective is to put into practice the concept of graphics streaming in rendering 3D graphics, by moving some proccesses of the 3D graphics rendering pipeline to a remote server.  This which is designing and building a 3D graphics engine that makes use of a remote server to carry out some of its proccesses. The engine should model a modern 3D graphics engine like OpenGL with the following features
- Rendering 3D objects.
- Illuminating and Texturing 3D Objects.
- A first person shooter camera.
- Loading 3D scenes 

# 1.6 Expected Results and Significance 
A 3D graphics engine capable of sever side rendering while providing a better experience than video streamed servers will be a definition of success. By better experience it is meant,lower latency, lower bandwidth and easier distribution on the server side. In doing this we wish to address if graphics streaming will provide better performance than video streaming when it comes to cloud 3D graphics.

# Early Preview (No Server GUI)
Initially I did a 3D Graphics Engine in Python which worked well for 3D objects that do not have a loads of triangles. The teapot.obj is where the drama started and everything fell apart, there are just way too many triangles in that teapot for my terrible code combined with python to handle. So I made a c++ server that takes in meta.data from a client application and returns a list of rendered triangles to draw on the client screen.

https://user-images.githubusercontent.com/63599157/191249060-8663a7a2-0b9f-4a42-95d7-c4a90b734f1e.mp4

# Server GUI

Here you can get to see what is happening on the server side before data is sent to the client side to be drawn.

https://user-images.githubusercontent.com/63599157/191249780-28ea9e1e-a6c1-4da4-9178-6649706ab364.mp4

# As of Jan 2023

https://user-images.githubusercontent.com/63599157/211518203-83cb5807-5bbb-4fe5-b136-66c8d1717201.mp4

Useful References

Pin-Chun, W., Ellis, A., Hart, J. and Cheng-Hsin, H., n.d. Optimizing Next-Generation Cloud Gaming Platforms with Planar Map Streaming and Distributed Rendering. [ebook] Illinois: University of Illinois. Available at: <https://graphics.cs.illinois.edu/wp-content/uploads/2019/09/vector-cloud-gaming.pdf> [Accessed 8 October 2022].



