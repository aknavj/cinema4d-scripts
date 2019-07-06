import c4d, random
from c4d import utils
from c4d import Vector as v


 #--------
 
def run(): #---- Only runs the script when userdata switch is on
    
    global GenSw
    
    GenSw = doc.SearchObject("Script") #Find the object the switch
    
    global Gsw
    
    if GenSw[c4d.ID_USERDATA,1]==True: # if the switch on...
        Gsw = True
        GenSw[c4d.ID_USERDATA,1]=False # turn it off immediately (like a click press)
    else:
        Gsw = False #ensure switch remains off

def noI(): #Number of iterations to run
    
    global noi
    
    noi = GenSw[c4d.ID_USERDATA,4]
    
    print "Number of interations to run", noi
    
def gen():
    
    global grpN
    
    grpN = c4d.BaseObject(c4d.Onull)
    grpN.SetName("Generated")
    doc.InsertObject(grpN)
        
def clear(): #---- Deletes children (old objects upon re-running script)
    
    a = doc.SearchObject("Generated")
    chld = a.GetChildren()
    print len(chld)
    
    if len(chld) == 0:
        return
    else:
        doc.SearchObject("Generated")
        
def getG(): #----- Gets geometry and adds a display tag
    
    global geoM
    
    geoM = GenSw[c4d.ID_USERDATA,2]
    
    print "Geometry =", geoM
    
    dtag = geoM.MakeTag(c4d.Tdisplay) #adds display tag to cube
    dtag[c4d.DISPLAYTAG_AFFECT_DISPLAYMODE] = True #sets display oon
    dtag[c4d.DISPLAYTAG_SDISPLAYMODE] = c4d.DISPLAYTAG_SDISPLAY_NOSHADING #sets shading mode

    
def geomD():  #----- Gets geometry data
   
    global l, geomPolyC
    iD = GenSw[c4d.ID_USERDATA,3]
    
    print " "
    geomPolyC = geoM.GetPolygonCount() #gets polygon count
    geomPntC =  geoM.GetPointCount() #gets point count
    geomP = geoM.GetAllPoints()  #gets all points from Geometry, vectors


    print "Point count =", geomPntC
   
    
    polyA =  geoM.GetPolygon(iD) #gets polygon (index)
    polyPli = polyA.a, polyA.b, polyA.c, polyA.d #gets points index of polygon
    print "Polygon id =", iD
    print "Index points of polygon", polyPli
    print " "
    
    
    l =[]
    
    for x in xrange (0, 4):
        l2 = geomP[(polyPli[x])]
        l.append(l2)
        



def check():
    
    for x in xrange (0, 4):
        tobj = c4d.BaseObject(c4d.Osplinetext) 
        tobj.SetRelPos(l[x])
        tobj[c4d.PRIM_TEXT_TEXT] = str(x)
        tobj[c4d.PRIM_TEXT_ALIGN] = c4d.PRIM_TEXT_ALIGN_MIDDLE
        tobj[c4d.PRIM_TEXT_HEIGHT] = 5
        tobj.SetName(x)
        

def calcE(): #------ calculates centre point of indexed polygon
    global edg
    
    cpX =  (l[0][0] + l[1][0] + l[2][0] + l[3][0]) / 4
    cpY =  (l[0][1] + l[1][1] + l[2][1] + l[3][1]) / 4
    cpZ =  (l[0][2] + l[1][2] + l[2][2] + l[3][2]) / 4
    
    tobj1 = c4d.BaseObject(c4d.Onull)
    tobj1.SetRelPos(v(cpX, cpY, cpZ))
    tobj1.SetName("Poly Edge")
    #tobj1.InsertUnder(grpN)
    
    edg1 = tobj1.GetAbsPos()
    edg = edg1

def calcC(): #------ calculates centre point geometry
    global cntr
    
    geomPntC =  geoM.GetPointCount() #gets point count
    geomP = geoM.GetAllPoints()
    
    cntr1 = (sum(geomP) / geomPntC)  #maintains vector 
    cntr = cntr1
    
    tobj1 = c4d.BaseObject(c4d.Onull)
    tobj1.SetRelPos(v(cntr))
    tobj1.SetName("Center")
    #tobj1.InsertUnder(grpN)
    

def dist():
   
    global dist
    
    dist = edg - cntr
    #print "distance =", dist
    strd1 = (dist.x),(dist.y),(dist.z)
    strd2 = list(strd1)
    #print "strd2 = ", strd2
    
    for i in xrange (0, 2):
        #print strd2[i]
        
        if strd2[i] == 0:
            del strd2[i]
            #print "deleted"
   
    dist = strd2

    dist = int(dist[0])*2
    
    if dist < 0:
        dist = dist * (-1)
    
    
    
    print dist
   


def dirc():
    global nV
    
    dr = range(0, geomPolyC)
    nV = []

    for x in xrange (0, int(noi)):
        #nV[x] = []
        
        dri = random.randint(0,(len(dr)+1))
        root = cntr
        #print dri
        nV1 = v(root.x,root.y,root.z)
    
        if dri == 0:
           #print "-Z"
           nV1[2] = root.z - dist
           
        if dri == 1:
           #print "+X"
           nV1[0] = root.x + dist
        
        if dri == 2:
            #print "+Z"
            nV1[2] = root.z + dist
        
        if dri == 3:
            #print "-X"
            nV1[0] = root.x - dist
        
        if dri == 4:
            #print "+Y"
            nV1[1] = root.y + dist
                
        if dri == 5:
            #print "-Y"
            nV1[1] = root.y - dist
        
        
 
        nV.append(nV1)
        

    

def cr8():
   
        for x in xrange (0, int(noi)):
            lobj = grpN.GetDown()
    
            if lobj == None:
                lobj = geoM.GetClone()
            
            a = lobj.GetClone()
            a.SetName("Clone")
            pos = a.GetAbsPos() + nV[x]
            a.SetAbsPos(pos)
            a.InsertUnder(grpN)

def main():
    
    run() #runs script
    if Gsw ==True:
        print "Running!"
        print " "
        noI()
        gen() # Creates generated null
        getG() # Gets the geometry from the script userdata
        #clear() #Should delete Generated null with its children
        if geoM == None:
            return
        else:
            geomD() #Gets geometry information from getC
            #check() #check index
            calcE() #calculates polygon centre (edge of geom)
            calcC() #calculates centre of geom
            
            if edg and cntr == None:
                return
            else:
                print edg, cntr #prints the two vectors
                dist() #calculates the distance between edge and centre x 2
                dirc() #calucates movement in 3D space
                cr8() #creates a clone with distance and dirc
    else:
        return