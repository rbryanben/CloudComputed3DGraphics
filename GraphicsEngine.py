from copy import deepcopy
from random import randint
from re import X
from tkinter.messagebox import RETRY
from turtle import window_width
import pygame
from math import sqrt, tan,cos,sin
from numba import njit

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

                    
        


class olcEngine3d:
    
    def __init__(self,window_height=800,window_width=800):
        self.window_height = window_height 
        self.window_width = window_width
        self.window = pygame.display.set_mode((self.window_height,self.window_width))
        self.clock = pygame.time.Clock()
        self.black = (0,0,0)
        self.white = (255,255,255)
        self.red = (255,0,0)
        self.fYaw = 0 
        self.onUserCreate()

        # start rendering 
        while True:
            self.clock.tick(60)
            self.onUserUpdate()
            pygame.display.update()
            
            #check close window 
            for event in pygame.event.get():
                if event.type == pygame.QUIT:
                    pygame.quit()
                    return
                if event.type == pygame.KEYDOWN:
                    if event.key == pygame.K_LEFT:
                        self.vCamera.x -= 0.02
                    if event.key == pygame.K_RIGHT:
                        self.vCamera.x += 0.02
                    if event.key == pygame.K_UP:
                        self.vCamera.y += 0.02
                    if event.key == pygame.K_DOWN:
                        self.vCamera.y -= 0.02
                    
                    # I want the camera to travel along the look direction
                    vForward = Vector_Mul(self.vLookDir,0.05) # change later to 8 * Elapsed time
                    
                    if event.key == pygame.K_w:
                        self.vCamera = Vector_Add(self.vCamera,vForward)
                    if event.key == pygame.K_s:
                        self.vCamera = Vector_Sub(self.vCamera,vForward)

                    
                    if event.key == pygame.K_a:
                        self.fYaw -= 0.01
                    if event.key == pygame.K_d:
                        self.fYaw += 0.01 

    
    def onUserCreate(self):
        self.angle = 0 # for ration changing - elapsed time
        self.vCamera = vec3d()
        self.vLookDir = vec3d()
        pygame.key.set_repeat(20,20)
        
        # projection matrix 
        self.projectionMarix = Matrix_MakeProjection(self.window_height/ self.window_width,
            1000,0.1,90)

        #load mesh 
        self.meshCube = mesh()
        self.meshCube.LoadFromObjectFile("./objs/mountains.obj")


    def drawTriangle(self,tri: triangle,color):
        
        pygame.draw.polygon(self.window,color,[(tri.p[0].x,tri.p[0].y),(tri.p[1].x,tri.p[1].y),(tri.p[2].x,tri.p[2].y)])
        #pygame.draw.line(self.window,self.red,(tri.p[0].x,tri.p[0].y),(tri.p[1].x,tri.p[1].y),1)
        #pygame.draw.line(self.window,self.red,(tri.p[1].x,tri.p[1].y),(tri.p[2].x,tri.p[2].y),1)
        #pygame.draw.line(self.window,self.red,(tri.p[2].x,tri.p[2].y),(tri.p[1].x,tri.p[1].y),1)
        
    def onUserUpdate(self):
        self.window.fill(self.black)
        self.angle = 3.142

        # Rotations 
        rotationZ = Matrix_MakeRotationZ(self.angle)
        
        #translate Mat World by z + 5
        matTrans = Matrix_MakeTranslation(0,0,5)
        
        # Combine Translation and Rotation
        matWorld = Matrix_MakeIdentity()
        matWorld = Matrix_MultiplyMatrix(matWorld,matTrans)
        matWorld = Matrix_MultiplyMatrix(matWorld,rotationZ)

        # Hard code look direction
        self.vLookDir.z = 1
        matCameraRot = Matrix_MakeRotationY(self.fYaw)
       

        # Camera config 
        vUp = vec3d([[0,1,0]])
        vTarget = vec3d([[0,0,1]])
        self.vLookDir = Matrix_MultiplyVector(matCameraRot,vTarget)
        vTarget = Vector_Add(self.vCamera,self.vLookDir)

    
        matCamera = getEmpty4x4Matrix()
        matCamera = Matrix_PointAt(self.vCamera,vTarget,vUp)
        matView = Matrix_QuickInverse(matCamera)
        
        # Raster List
        vecTrianglesToRaster = [] 

        # Translate Project Triangles
        for tri in self.meshCube.tris:

            # Projection holders
            triProjected = triangle()
            triTransformed = triangle()
            triViewd = triangle()

            # Transform
            MultiplyMatrix(tri.p[0],triTransformed.p[0],matWorld)
            MultiplyMatrix(tri.p[1],triTransformed.p[1],matWorld)
            MultiplyMatrix(tri.p[2],triTransformed.p[2],matWorld)

                        
            # Normal            
            line1 =  Vector_Sub(triTransformed.p[1],triTransformed.p[0])
            line2 = Vector_Sub(triTransformed.p[2],triTransformed.p[0])
            normal = Vector_CrossProduct(line1,line2)
            normal = Vector_Normalise(normal)

            vCameraRay = Vector_Sub(triTransformed.p[0],self.vCamera)
            if Vector_DotProduct(normal,vCameraRay) < 0:

                # Illumination
                light_direction = vec3d([[0,0,-1]])
                light_direction = Vector_Normalise(light_direction)

                dp = max(0.1,Vector_DotProduct(light_direction,normal))
                color = (abs(255*dp),abs(255*dp),abs(255*dp))

                #convert to world space
                triViewd.p[0] = Matrix_MultiplyVector(matView,triTransformed.p[0])
                triViewd.p[1] = Matrix_MultiplyVector(matView,triTransformed.p[1])
                triViewd.p[2] = Matrix_MultiplyVector(matView,triTransformed.p[2])
                

                #clipping
                clipped = [triangle(),triangle()] 
                nClippedTriangle = Tiangle_ClipAgainstPlane(vec3d([[0,0,0.1]]),vec3d([[0,0,1]]),triViewd,clipped[0],clipped[1])

                for i in range(nClippedTriangle):
                    # Projection 
                    triProjected.p[1] = Matrix_MultiplyVector(self.projectionMarix,clipped[i].p[1])
                    triProjected.p[2] = Matrix_MultiplyVector(self.projectionMarix,clipped[i].p[2])
                    triProjected.p[0] = Matrix_MultiplyVector(self.projectionMarix,clipped[i].p[0])
                    
                    #Normalize
                    triProjected.p[0] = Vector_Div(triProjected.p[0],triProjected.p[0].w)
                    triProjected.p[1] = Vector_Div(triProjected.p[1],triProjected.p[1].w)
                    triProjected.p[2] = Vector_Div(triProjected.p[2],triProjected.p[2].w)   
                    
                    triProjected.p[0].x += 1
                    triProjected.p[0].y += 1
                    triProjected.p[1].x += 1
                    triProjected.p[1].y += 1
                    triProjected.p[2].x += 1
                    triProjected.p[2].y += 1

                    triProjected.p[0].x *= 0.5 * self.window_width 
                    triProjected.p[0].y *= 0.5 * self.window_width 
                    triProjected.p[1].x *= 0.5 * self.window_width 
                    triProjected.p[1].y *= 0.5 * self.window_width 
                    triProjected.p[2].x *= 0.5 * self.window_width 
                    triProjected.p[2].y *= 0.5 * self.window_width 
                    

                    triProjected.color = color
                    if nClippedTriangle == 2:
                        triProjected.setColor((0,255,0))
                    
                    if i == 1:
                        triProjected.setColor((0,0,255))

                    vecTrianglesToRaster.append(deepcopy(triProjected))

        vecTrianglesToRaster.sort(key= lambda x : x.getMid(),reverse=True)
        
        # Clip top 
        topClipped = [] 
        for triToClip in vecTrianglesToRaster:
            clipped = [triangle(),triangle()]
            nClippedTriangle = Tiangle_ClipAgainstPlane(vec3d([[0,0,0]]),vec3d([[0,1,0]]),triToClip,clipped[0],clipped[1])
            for i in range(nClippedTriangle):
                topClipped.append(deepcopy(clipped[i]))
        
        # Another Clip
        leftClipped = [] 
        for triToClip in topClipped:
            clipped = [triangle(),triangle()]
            nClippedTriangle = Tiangle_ClipAgainstPlane(vec3d([[0,self.window_height - 1,0]]),vec3d([[0,-1,0]]),triToClip,clipped[0],clipped[1])
            for i in range(nClippedTriangle):
                leftClipped.append((clipped[i]))

        #Another clip 
        anotherClip = []
        for triToClip in leftClipped:
            clipped = [triangle(),triangle()]
            nClippedTriangle = Tiangle_ClipAgainstPlane(vec3d([[0,0,0]]),vec3d([[1,0,0]]),triToClip,clipped[0],clipped[1])
            for i in range(nClippedTriangle):
                anotherClip.append((clipped[i]))
        
        #Another clip 
        lastClip = []
        for triToClip in anotherClip:
            clipped = [triangle(),triangle()]
            nClippedTriangle = Tiangle_ClipAgainstPlane(vec3d([[self.window_width - 1,0,0]]),vec3d([[-1,0,0]]),triToClip,clipped[0],clipped[1])
            for i in range(nClippedTriangle):
                lastClip.append(deepcopy(clipped[i]))


        for t in lastClip:
            self.drawTriangle(t,t.color)


def MultiplyMatrix(i:vec3d,o:vec3d,m):
   
    o.x = (i.x * m[0][0]) +( i.y * m[1][0]) +( i.z * m[2][0]) + m[3][0]
    o.y = (i.x * m[0][1])+ (i.y * m[1][1] )+ (i.z * m[2][1] )+ m[3][1]
    o.z = (i.x * m[0][2])+ (i.y * m[1][2] )+ (i.z * m[2][2] )+ m[3][2]
    w = (i.x * m[0][3]) + (i.y * m[1][3]) + (i.z * m[2][3]) + m[3][3]

    if w != 0:
        o.x /= w 
        o.y /= w 
        o.z /= w 
        pass


def Vector_IntersectPlane(plane_p:vec3d,plane_n:vec3d,lineStart:vec3d,lineEnd:vec3d):
    plane_n = Vector_Normalise(plane_n)
    plane_d = -Vector_DotProduct(plane_n,plane_p)
    ad = Vector_DotProduct(lineStart,plane_n)
    bd = Vector_DotProduct(lineEnd,plane_n)
    t = (-plane_d - ad) / (bd- ad)
    lineStartToEnd = Vector_Sub(lineEnd,lineStart)
    lineIntersect = Vector_Mul(lineStartToEnd,t)
    return Vector_Add(lineStart,lineIntersect)


def Tiangle_ClipAgainstPlane(plane_p:vec3d,plane_n: vec3d,in_tri:triangle,out_tri1: triangle,out_tri2:triangle):
    #make sure normal is indeed normal
    plane_n = Vector_Normalise(plane_n)

    # Returns shortest distance from point to a plane
    def dist(p:vec3d):
        n = Vector_Normalise(p)
        return (plane_n.x * p.x + plane_n.y * p.y + plane_n.z * p.z - Vector_DotProduct(plane_n,plane_p))

    # Create two temporary storage arrays to classify points either side of the plane
    #if the distance is positibe, point lies inside the plane
    inside_points = []
    outside_points = []

    # get signed distance from each point in triangle to the plane
    d0 = dist(in_tri.p[0])
    d1 = dist(in_tri.p[1])
    d2 = dist(in_tri.p[2])

    if d0 >= 0 : inside_points.append(in_tri.p[0])
    else: outside_points.append(in_tri.p[0])
    if d1 >= 0 : inside_points.append(in_tri.p[1])
    else: outside_points.append(in_tri.p[1])
    if d2 >= 0 : inside_points.append(in_tri.p[2])
    else: outside_points.append(in_tri.p[2]) 

    if len(inside_points) == 0: return 0 #dont draw any

    if len(inside_points) == 3: 
        out_tri2.color = in_tri.color
        out_tri1.color = in_tri.color
        out_tri1.p[0] = deepcopy(in_tri.p[0])
        out_tri1.p[1] = deepcopy(in_tri.p[1])
        out_tri1.p[2] = deepcopy(in_tri.p[2])
        return 1 #all are valid 

    if len(inside_points) == 1 and len(outside_points) == 2:
        out_tri2.color = in_tri.color
        out_tri1.color = in_tri.color

        #inside point is valid we keep that 
        out_tri1.p[0] = deepcopy(inside_points[0])

        #but the two new points are at the locations where the 
	    #original sides of the triangle (lines) intersect with the plane
        out_tri1.p[1] = Vector_IntersectPlane(plane_p,plane_n,inside_points[0],outside_points[0])
        out_tri1.p[2] = Vector_IntersectPlane(plane_p,plane_n,inside_points[0],outside_points[1])

        return 1
    
    if len(inside_points) == 2 and len(outside_points) == 1:
        out_tri2.color = in_tri.color
        out_tri1.color = in_tri.color

        #The first triangle consists of the two inside points and a new
		#point determined by the location where one side of the triangle
		#intersects with the plane
        out_tri1.p[0] = deepcopy(inside_points[0])
        out_tri1.p[1] = deepcopy(inside_points[1])
        out_tri1.p[2] = Vector_IntersectPlane(plane_p,plane_n,inside_points[1],outside_points[0])

        out_tri2.p[0] = deepcopy(inside_points[0])
        out_tri2.p[1] = Vector_IntersectPlane(plane_p,plane_n,inside_points[1],outside_points[0])
        out_tri2.p[2] = Vector_IntersectPlane(plane_p,plane_n,inside_points[0],outside_points[0])
        return 2


def Matrix_PointAt(pos:vec3d,target:vec3d,up: vec3d):
    newForward = Vector_Sub(target,pos)
    newForward = Vector_Normalise(newForward) 
    
    # calculate the new up 
    a = Vector_Mul(newForward,Vector_DotProduct(up,newForward))
    newUp = Vector_Sub(up,a)
    newUp = Vector_Normalise(newUp)

    # New Right direction is easy, its just cross product
    newRight = Vector_CrossProduct(newUp,newForward)

    matrix = [
        [newRight.x,newRight.y,newRight.z,0],
        [newUp.x,newUp.y,newUp.z,0],
        [newForward.x,newForward.y,newForward.z,0],
        [pos.x,pos.y,pos.z,0]
    ]

    return matrix


def Matrix_QuickInverse(m):
    matrix = getEmpty4x4Matrix()
    matrix[0][0] = m[0][0]; matrix[0][1] = m[1][0]; matrix[0][2] = m[2][0]
    matrix[1][0] = m[0][1]; matrix[1][1] = m[1][1]; matrix[1][2] = m[2][1]
    matrix[2][0] = m[0][2]; matrix[2][1] = m[1][2]; matrix[2][2] = m[2][2]
    matrix[3][0] = -(m[3][0] * matrix[0][0] + m[3][1] * matrix[1][0] + m[3][2] * matrix[2][0])
    matrix[3][1] = -(m[3][0] * matrix[0][1] + m[3][1] * matrix[1][1] + m[3][2] * matrix[2][1])
    matrix[3][2] = -(m[3][0] * matrix[0][2] + m[3][1] * matrix[1][2] + m[3][2] * matrix[2][2])
    matrix[3][3] = 1 

    return matrix 


def Vector_Add(a:vec3d,b:vec3d):
    return vec3d([[a.x + b.x,a.y + b.y,a.z + b.z]])


def getMatrix():
    return [
        [0,0,0,0],
        [0,0,0,0],
        [0,0,0,0],
        [0,0,0,0]
    ]


def Vector_Sub(a:vec3d,b:vec3d):
    return vec3d([[a.x - b.x,a.y - b.y,a.z - b.z]])


def Vector_Mul(a:vec3d,b:float):
    return vec3d([[a.x * b,a.y * b,a.z * b]])


def Vector_Div(a:vec3d,b):
    return vec3d([[a.x / b,a.y / b,a.z / b]])


def Vector_DotProduct(a:vec3d,b:vec3d):
    return a.x*b.x + a.y*b.y + a.z * b.z


def Vector_Length(v:vec3d):
    return sqrt(Vector_DotProduct(v,v))


def Vector_Normalise(a:vec3d):
    l = Vector_Length(a)
    return vec3d([[a.x / l if l != 0 else 1, a.y / l  if l != 0 else 1 , a.z / l  if l != 0 else 1]])


def Vector_CrossProduct(v1:vec3d,v2:vec3d):
    v = vec3d() 
    v.x = v1.y * v2.z - v1.z * v2.y
    v.y = v1.z * v2.x - v1.x * v2.z
    v.z = v1.x * v2.y - v1.y * v2.x
    return v

def Matrix_MultiplyVector(m,i:vec3d):
    v = vec3d()
    v.x = (i.x * m[0][0] )+ (i.y * m[1][0] )+ (i.z * m[2][0] )+ (i.w * m[3][0])
    v.y = (i.x * m[0][1] )+ (i.y * m[1][1] )+ (i.z * m[2][1] )+ (i.w * m[3][1])
    v.z = (i.x * m[0][2] )+ (i.y * m[1][2] )+ (i.z * m[2][2] )+ (i.w * m[3][2])
    v.w = (i.x * m[0][3] )+ (i.y * m[1][3] )+ (i.z * m[2][3] )+ (i.w * m[3][3])

    return v


def getEmpty4x4Matrix():
    return [
        [0,0,0,0],
        [0,0,0,0],
        [0,0,0,0],
        [0,0,0,0]
    ]


def Matrix_MakeIdentity():
    return [
        [1,0,0,0],
        [0,1,0,0],
        [0,0,1,0],
        [0,0,0,1]
    ]


def Matrix_MakeRotationZ(angle):
    return [
            [cos(angle),-sin(angle),0,0],
            [sin(angle),cos(angle),0,0],
            [0,0,1,0],
            [0,0,0,0]
        ]


def Matrix_MakeRotationY(angle):
    return [
            [cos(angle),0,sin(angle),0],
            [0,1,0,0],
            [-sin(angle),0,cos(angle),0],
            [0,0,0,0]
        ]


def Matrix_MakeRotationX(angle):
    return [
            [1,0,0,0],
            [0,cos(angle),-sin(angle),0],
            [0,sin(angle),cos(angle),0],
            [0,0,0,0]
        ]


def Matrix_MakeTranslation(x,y,z):
    return [
        [1,0,0,0],
        [0,1,0,0],
        [0,0,1,0],
        [x,y,z,1]
    ]


def Matrix_MakeProjection(aspectRatio,zFar,zNear,fieldview):
    fTangent = 1/tan(fieldview*0.5/180*3.14159)
    return [
            [aspectRatio*fTangent,0,0,0],
            [0,fTangent,0,0],
            [0,0,zFar/(zFar-zNear),1],
            [0,0,(-zFar*zNear)/(zFar-zNear),0]
        ]


def Matrix_MultiplyMatrix(a,b):
    # Get meta 
    a_rows = len(a)
    a_cols = len(a[0])

    b_rows = len(b)
    b_cols = len(b[0])
    
    # Incompatable matrix
    if a_cols != b_rows:
        return None
    else:
        # Result matrix
        res = [[0]*b_cols for r in range(a_rows)]

        # fill in the res 
        for r in range(a_rows):
            for c in range(b_cols):
                # r represents the r in a 
                # c represents the col in b 
                for k in range(a_cols):
                    
                    res[r][c] += (a[r][k]*b[k][c])
        return res


# Main Function

def main():
    demo = olcEngine3d()

main()

