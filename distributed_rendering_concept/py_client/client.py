from math import sqrt
from lib2to3 import pygram
import pygame
from pygame.time import Clock
import socket


class vec3d:
    def __init__(self,matrix=None):
        self.w = 1
        if matrix == None:
            self.x = 0
            self.y = 0
            self.z = 0
            return

        self.x = matrix[0][0]
        self.y = matrix[0][1]
        self.z = matrix[0][2]

    def __sub__(self,other):
        res = vec3d()
        res.x = self.x - other.x
        res.y = self.y - other.y
        res.z = self.z - other.z 
        return res 
    

        

class triangle:
    def __init__(self,vectors3d_three=None):
        self.color = (50,50,50)
        if vectors3d_three == None:
            self.p = [vec3d([[0,0,0]]) for i in range(3)]
        else:
            self.p = vectors3d_three

    def getMid(self):
        return (self.p[0].z + self.p[1].z + self.p[2].z) / 3 
    
    def setColor(self,color):
        self.color =  color

class mesh:
    def __init__(self):
        self.tris = [] 
    
    def LoadFromObjectFile(self,filename):
        verts = []

        with open(filename,"r") as file:
            for line in file.read().splitlines():
                    
                #vertex
                if len(line) > 0 and line[0] == "v":
                    # new vertex 
                    v = vec3d()
                    tokens = line[2:].split(" ")
                    v.x = float(tokens[0])
                    v.y = float(tokens[1])
                    v.z = float(tokens[2])
                    verts.append(v)

                #triangle
                if len(line) > 0 and line[0] == "f":
                    f = []
                    tokens = line[2:].split(" ")
                    
                    self.tris.append(
                        triangle(vectors3d_three=[
                            verts[int(tokens[0])-1],
                            verts[int(tokens[1])-1],
                            verts[int(tokens[2])-1]
                        ])
                    )


# Cloud Gaming Engine
class Cloud3DRender:
    # Constructor
    def __init__(self,cloudServerHost="localhost",cloudServerPort=8085, windowWidth=800,windowHeight=800,windowBackgroundColor=(0,0,0)):
        self.cloudServerHost = cloudServerHost
        self.cloudServerPort = cloudServerPort
        self.windowWidth = windowWidth
        self.windowHeight = windowHeight
        self.windowBackgroundColor = windowBackgroundColor

        #setup 
        if self.setup():
            self.onReady()
            while True:
                # Tick at 60fps 
                self.clock.tick(60)
                # Clear screen
                self.window.fill(self.windowBackgroundColor)
                # Update display
                self.onUpdate()
                pygame.display.update()
                
                #check close window 
                for event in pygame.event.get():
                    if event.type == pygame.QUIT:
                        pygame.quit()
                        return
        else:
            print(f"Failed to connect to {cloudServerHost}:{cloudServerPort}")
        
    # Setup Variables and Connections
    def setup(self):
        #setup clock and window
        self.clock = Clock()
        self.window = pygame.display.set_mode((self.windowWidth,self.windowHeight))

        #setup socket
        try:
            self.socketConnection = socket.socket(socket.AF_INET,socket.SOCK_STREAM)
            self.socketConnection.connect((self.cloudServerHost,self.cloudServerPort))
            pygame.display.set_caption(f"Cloud Server - {self.cloudServerHost}:{self.cloudServerPort}")
        except:
            return False

        # All Set
        return True
    
    # Send Message To Socket
    def sendToSocket(self,data:str):
        self.socketConnection.send(bytes(data,"utf-8"))
        return self.socketConnection.recv(2000000).decode("utf-8")

    # Convert String To List Of Triangles
    def stringToTriangles(self,s):
        res = []
        #split into lines
        lines = s.split("/n")
        lines.pop() #pop at the end empty string

        #split each line into 3 string triangles
        for line in lines:
            vectors_color = line[2:].split("c")
            tristr = vectors_color[0].split("v")
            
            if (len(vectors_color) != 2): continue
            #color 
            rgbstr = vectors_color[1].split(" ")
            color = (float(rgbstr[1]),float(rgbstr[2]),float(rgbstr[3]))

            if (len(tristr) != 4) : continue
            #vectors string
            vectstr1 = tristr[1] 
            vectstr2 = tristr[2]
            vectstr3 = tristr[3]

            #Build triangle
            vectstr1 = tristr[1].split(" ")
            vectstr2 = tristr[2].split(" ")
            vectstr3 = tristr[3].split(" ")

            tri = triangle(vectors3d_three=[
                vec3d([[float(vectstr1[1]),float(vectstr1[2]),float(vectstr1[3])]]),
                vec3d([[float(vectstr2[1]),float(vectstr2[2]),float(vectstr2[3])]]),
                vec3d([[float(vectstr3[1]),float(vectstr3[2]),float(vectstr3[3])]])
            ])
            tri.color = color
            res.append(tri)
        return res

    # Object Ready 
    def onReady(self):
        self.angle = 0
        pass  
    
    # On Update
    def onUpdate(self):
        self.angle += 0.1
        # Compute Data
        data = f"a {self.angle}"
        
        # Compute Online
        finalComputed = self.sendToSocket(data)

        # Draw Triangles
        tris = self.stringToTriangles(finalComputed)
        for tri in tris:
            # Draw triangle
            self.drawTriangle(tri)

    def drawTriangle(self,tri: triangle):
        pygame.draw.polygon(self.window,tri.color,[(tri.p[0].x,tri.p[0].y),(tri.p[1].x,tri.p[1].y),(tri.p[2].x,tri.p[2].y)])
        #pygame.draw.line(self.window,(255,255,255),(tri.p[0].x,tri.p[0].y),(tri.p[1].x,tri.p[1].y),1)
        #pygame.draw.line(self.window,(255,255,255),(tri.p[1].x,tri.p[1].y),(tri.p[2].x,tri.p[2].y),1)
        #pygame.draw.line(self.window,(255,255,255),(tri.p[2].x,tri.p[2].y),(tri.p[0].x,tri.p[0].y),1)


# main function
def main():
    cube = Cloud3DRender()

# program start 
main()
