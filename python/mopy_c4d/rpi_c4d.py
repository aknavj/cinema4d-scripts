import c4d, socket, time
from c4d.modules import mograph as mo
from c4d import utils


def main():

    #------------ Basic MoGraph ---------------#

    md = mo.GeGetMoData(op)

    if md==None: return False

    cnt = md.GetCount()
    marr = md.GetArray(c4d.MODATA_MATRIX)
    fall = md.GetFalloffs()
    carr = md.GetArray(c4d.MODATA_COLOR)

    #--------- Colour Threshold -------------#

    colThres = []
    for i in reversed(xrange(0, cnt)):

        if carr[i] != c4d.Vector(0,0,0): #if clones arent black add to list (colThres)
            l1 = i
            colThres.append(l1)

    #print "Number of Nnt black clones", len(colThres)
    
    #---- Update mapV list based upon input ----#

    mapV = [] #map value empty list

    for x in xrange (126): # Set list values to 0 (LED OFF) - There are 126 LEDs on sheild
        l = 0
        mapV.append(l)

    #---- Update mapV list based upon input ----#

    for x in xrange (len(colThres)): # For the clones that aren't black set mapV to 1 (on)
        mapV[colThres[x]] = 1


    #---- Convert mapV to format for LED ----# #The sheild requires the formart to start with $$$F and end with "\r"
    #see mata for debug
    mapVup = ("".join(map(str, (mapV))))

    srt = '$$$F'
    end = "\r"

    #---- Send data via UDP ----#

    data = srt + mapVup + end
    #mata = "$$$F000000000000000000000111000011111110011111110111111111111101111111101111011000110011000110000000000000000000000000000000000000\r" #Debug
    #print "data =", data

    #------------- Send UDP -------------------#

    #UDP_IP = "localhost"
    UDP_IP = "192.168.0.19" #Pi
    UDP_PORT = 2556

    sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    sock.connect((UDP_IP, UDP_PORT))
    sock.send(data)
    
    #-----------------------------------------#

    return True