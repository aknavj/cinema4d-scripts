import c4d, random
import c4d.utils

from c4d import Vector as v

""" ---------------------------- Basics --------------------------- """
basetime = doc.GetTime() 
Fps = doc.GetFps()
cF = doc.GetTime() .GetFrame(doc.GetFps())

""" ---------------------------- Vars --------------------------- """

global tV
tV = [0,0,0] # Temp null vector

vX = tV[0] #Vector X
vY = tV[1] #Vector Y
vZ = tV[2] #Vector Z

rM = range(1, 100) #Range of movement

nTimes = 50 # Number of times to run the loop/generate points on splines/generate vectors


def main():
    
    a = doc.SearchObject("Null")
   
    if cF == 0: # only runs on frame 0
        
        l = [] # loop iteration store list
    
        
        for x in xrange(nTimes):
                
    
                randindextv = random.randint(0,(len(tV)-1)) # Generates a random [i] for rM
                randrM = random.randint(0,(len(rM)-1)) # a random value is returned from rM list
                
                
                global X, Y, Z
                
                X = rM[randrM] # X vector is generated
                Y = rM[randrM] # Y vector is generated
                Z = rM[randrM] # Z vector is generated
                
                spline = c4d.SplineObject((nTimes),c4d.SPLINETYPE_LINEAR) # A spline with nTimes divisions is created
                
                if randindextv == 0:
                    tV[0] += X # should these be globals? working in console though
                    tV[1] += 0
                    tV[2] += 0
                    #print x, tV
            
                if randindextv == 1: #Y
                    tV[0] += 0
                    tV[1] += Y
                    tV[2] += 0
                    #print x, tV
                    
                if randindextv == 2:
                    tV[0] += 0
                    tV[1] += 0
                    tV[2] += Z
                    #print x, tV
                    
                l2 = [tV[0], tV[1], tV[2]]
                l.append(l2)
                
                
        for y in xrange(nTimes):
        
            spline.SetPoint( y, v(l[y][0], l[y][1], l[y][2]))
            spline[c4d.SPLINEOBJECT_CLOSED] = False
        
  
        if op:
                spline.InsertUnder(a)
        else:
                doc.InsertObject(spline)
        
        random.shuffle(l)
        
    else: 
        return