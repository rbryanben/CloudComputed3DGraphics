
class Vect3d:
    def __init__(self,x=None,y=None,z=None):
        self.x = x 
        self.y = y 
        self.z = z 

    def copy(self):
        res = Vect3d()
        res = self
        return res


class Triangle:
    def __init__(self,v1=None,v2=None,v3=None):
        self.v1 = v1 
        self.v2 = v2
        self.v3 = v3

    def copy(self):
        res = Triangle()
        res.v1 = self.v1.copy()
        res.v2 = self.v2.copy()
        res.v3 = self.v3.copy()
        return res 


    