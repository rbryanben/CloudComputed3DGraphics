# Cloud Computed 3D Graphics

Well, this is an attempt to Cloud 3D Graphics computation!. The idea is to move the Geometry and Rasterizing computation to the some remote server and relieve a client of that workload. It could be a better formular to Cloud Gaming, who knows ?

# Early Preview (No Server GUI)

Initially I did a 3D Graphics Engine in Python which worked well for 3D objects that do not have a loads of triangles. The teapot.obj is where the drama started and everything fell apart, there are just way too many triangles in that teapot for my terrible code combined with python to handle. So I made a c++ server that takes in meta.data from a client application and returns a list of rendered triangles to draw on the client screen.

https://user-images.githubusercontent.com/63599157/191249060-8663a7a2-0b9f-4a42-95d7-c4a90b734f1e.mp4

# Server GUI

Here you can get to see what is happening on the server side before data is sent to the client side to be drawn.

https://user-images.githubusercontent.com/63599157/191249780-28ea9e1e-a6c1-4da4-9178-6649706ab364.mp4

# More Stuff

Still Working on It 🥴🥴
