import pygame
from common.common import *
from copy import deepcopy
from time import sleep

# Setup pygame
pygame.init()
back = (0,0,0)
gameDisplay = pygame.display.set_mode((800,800))

# Triangle
v2 = Vect3d(400,200,0)
v3 = Vect3d(700,700,0)
v1 = Vect3d(100,500,0)

# Triangle Count 
triangleCount = [0]

tri = Triangle(v1,v2,v3)

def drawTriangle(tri):
    pygame.draw.line(gameDisplay,(255,255,0),(tri.v1.x,tri.v1.y),(tri.v2.x,tri.v2.y),1)
    pygame.draw.line(gameDisplay,(255,255,0),(tri.v2.x,tri.v2.y),(tri.v3.x,tri.v3.y),1)
    pygame.draw.line(gameDisplay,(255,255,0),(tri.v3.x,tri.v3.y),(tri.v1.x,tri.v1.y),1)

# draw triangles
def drawTriangles(tris):
    for t in tris:
        drawTriangle(t)

# Triangulate 
def triangulate(tri:Triangle,depth):
    if depth == 0:
        return [tri]

    # Mid point
    mid_v1_v3 = Vect3d((tri.v1.x + tri.v3.x)/2,(tri.v1.y + tri.v3.y)/2,0)
    # triangle 1 
    t1Copy = tri.copy()
    triangle_1 = Triangle(t1Copy.v2,mid_v1_v3.copy(),t1Copy.v1)
    #triangle 2 
    t2Copy = tri.copy()
    triangle_2 = Triangle(t2Copy.v2,mid_v1_v3,t2Copy.v3)

    return triangulate(triangle_1,depth - 1) + triangulate(triangle_2,depth - 1)

#render function
def render():
    triangleCount[0] += 1
    triTriangulated = triangulate(tri,min(triangleCount[0],10))
    drawTriangles(triTriangulated)




#main
pygame.display.set_caption('Triangulate Sandbox')
gameDisplay.fill(back)
clock = pygame.time.Clock()
running = True
while running:
    render()
    pygame.display.update()
    sleep(1); 
pygame.quit()
quit()