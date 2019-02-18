import c4d
from c4d import utils

import random

import math
    
def main():
    source = op.GetDown()
    source_spline = op[c4d.ID_USERDATA,8].GetRealSpline().GetClone()
    if source_spline:
    
        random.seed(op[c4d.ID_USERDATA,4])
        initial_points = source_spline.GetAllPoints()
        
        if len(initial_points)>1:
        
            duplicates = [initial_points] * (op[c4d.ID_USERDATA,5]+1)
            
            bolts = [dict(points = bolt_points, iteration = 0) for bolt_points in duplicates]
        
            
            max_offset = op[c4d.ID_USERDATA,3]
            for i in xrange(0, op[c4d.ID_USERDATA,6]):
                new_branches = []
                for bolt_data in bolts:
                    bolt = bolt_data['points']
                    new_bolt = [bolt[0]]
                    
                    for pid in xrange(0, len(bolt)-1):
                        sp = bolt[pid]
                        ep = bolt[pid+1]
                        np = (sp+ep)*.5
                        
                        line_dir = (ep-sp).GetNormalized()
                        
                        up = c4d.Vector(random.uniform(-1, 1), random.uniform(-1, 1), random.uniform(-1, 1)).GetNormalized()
                        
                        offset_dist = random.uniform(-1, 1) * max_offset
                        offset = line_dir.Cross(up).GetNormalized() * offset_dist
                                        
                        np += offset
                                        
                        new_bolt.extend([np, ep])
                        
                        if random.random()>op[c4d.ID_USERDATA,7]:
                            new_branches.append([np, np + np - sp + offset])
                    
                    bolt_data['points'] = new_bolt
                new_branches_data = [dict(points = bolt_points, iteration = i) for bolt_points in new_branches]    
                bolts.extend(new_branches_data)
                max_offset *= .5
            
            null = c4d.BaseObject(c4d.Onull)
            
            for bolt_data in bolts:
                bolt = bolt_data['points']
                iteration = bolt_data['iteration']
                spline = c4d.SplineObject(len(bolt), c4d.SPLINETYPE_LINEAR)
                spline.SetAllPoints(bolt)
                
                sweep = c4d.BaseObject(c4d.Osweep)
                spline.InsertUnder(sweep)
                crossection = source.GetClone()
                crossection.SetRelScale(c4d.Vector(1.0/math.pow((iteration+1),(1-op[c4d.ID_USERDATA,1])*3)))
                crossection.InsertUnder(sweep)
                sweep.InsertUnder(null)
                sweep.SetPhong(True, True, 80)
                
            return null
