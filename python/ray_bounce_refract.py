import c4d
from c4d import utils
import math

def GetCurrentState(o):
    obj = utils.SendModelingCommand(
            command = c4d.MCOMMAND_CURRENTSTATETOOBJECT,
            list = [o],
            mode=c4d.MODELINGCOMMANDMODE_ALL,
            bc=c4d.BaseContainer(),
            doc = doc)
    return  utils.SendModelingCommand(
            command = c4d.MCOMMAND_JOIN,
            list = obj,
            mode=c4d.MODELINGCOMMANDMODE_ALL,
            bc=c4d.BaseContainer(),
            doc = doc)

def main():
    
    update = op[c4d.ID_USERDATA,4]
    if update:
                
        target = op[c4d.ID_USERDATA,2]
        max_seg = op[c4d.ID_USERDATA,3]
        
        if target != None:
            obj = op.GetObject()
            target_poly = GetCurrentState(target)[0]
            
            objM = obj.GetMg()
            targetM = target.GetMg() * target.GetUpMg()

            toTarget = objM * targetM.__invert__()
            toObj = targetM * objM.__invert__()
                
            rc = utils.GeRayCollider()
            rc.Init(target_poly)
            
            points = [c4d.Vector(0, 0, 0)]
    
            start = points[0] * toTarget
            direction = c4d.Vector(0, 0, 1) * toTarget - start
            
#            c4d.StatusSetSpin()
            
            for i in xrange(1, max_seg+1):
                if not rc.Intersect(start + direction.GetNormalized() * 0.001, direction, 1000000):
                    break
                
                ray_result = rc.GetNearestIntersection()
                pos = ray_result["hitpos"]
                start = pos
                points.append(pos * toObj)
            
                V = direction
                N = ray_result["f_normal"].GetNormalized() # without phong
                # N = ray_result["s_normal"].GetNormalized() # with phong
                N_out = ray_result["backface"]
                
                # print normal vectors – only for the first hit
                if i==1:
                    print ray_result["f_normal"].GetNormalized(), "face normal"
                    print ray_result["s_normal"].GetNormalized(), "phong normal"
                    print "-----------------------------------------"
                
                # ior –> ior difference (difference is not quite right)
                n1 = 1.0
                n2 = 1.3
                n0 = n1 / n2
                
                # check if the ray hits an object from inside
                # flipp normal direction and ior difference to allow the ray to exit the object correctly
                if N_out:
                    N = -N
                    n0 = n2 / n1       
                
                # c1 –> reflection vector
                c1 = -N.Dot(V)
                R_refl = V + (2 * N * c1 )                
                
                # c2 –> refraction vector
                c2 = 1 - n0**2 * (1 - c1**2)
                
                # check for inner total refraction
                # if yes –> return the reflection vector
                if c2 < 0:
                    c2 = 0
                    direction = R_refl
                
                # if not –> return the refraction vector
                else:
                    c2 = math.sqrt( c2 )
                    R_refr = (n0 * V) + (n0 * c1 - c2) * N
                    direction = R_refr
                
                
            obj.ResizeObject(len(points))
            obj.SetAllPoints(points)
            obj.Message(c4d.MSG_UPDATE)
#            c4d.StatusClear()
