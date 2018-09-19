import math
import random
import numpy as np

from scipy.spatial import Delaunay

h_all=[0.2]#,0.1,0.05,0.025,0.0125,0.00625]

height = 1.0
width  = 1.0
variation = .02 # as a decimal for a percent


for key, h in enumerate(h_all):
    content=[]
    
    content.append("# vtk DataFile Version 3.0")
    content.append("vtk output")
    content.append("ASCII")
    content.append("DATASET UNSTRUCTURED_GRID")
    
    x_layer = int(width / h)
    y_layer = int(height / h)

    flagged_layers_left_and_right = .02*x_layer
    flagged_layers_top_and_bottom = .02*y_layer
    
    
    
    points = np.zeros([x_layer*y_layer,2])

    for vertical in range(y_layer):
        for horizontal in range(x_layer):
            rand_pert_x = random.uniform(-variation, variation)
            rand_pert_y = random.uniform(-variation, variation)
    
            x = horizontal*h
            y = vertical*h
     
            if (horizontal==0 or horizontal==x_layer-1) or (vertical==0 or vertical==y_layer-1):
                points[x_layer*vertical + horizontal,:] = np.array([x,y])
            else:
                points[x_layer*vertical + horizontal,:] = np.array([x+h*rand_pert_x,y+h*rand_pert_y])
    
    
    content.append("POINTS %d float"%(points.shape[0]))
    for i in range(points.shape[0]):
        content.append("%f %f %f"%(points[i,0],points[i,1],0))

    tri = Delaunay(points, qhull_options="QJ")

    # visualization
    #import matplotlib.pyplot as plt
    #plt.triplot(points[:,0], points[:,1], tri.simplices.copy())
    #plt.plot(points[:,0], points[:,1], 'o')
    #plt.show()
    
    content.append("CELLS  %d 3"%(tri.simplices.shape[0]))
    for i in range(tri.simplices.shape[0]):
        content.append("%d %d %d"%tuple(simplex for simplex in tri.simplices[i,:]))
    content.append("CELLS_TYPES 1")
    content.append("5") # 2d triangles

#['%d'%simplex for simplex in tri.simplices[i,:]])
#(['%d'%simplex for simplex in tri.simplices[i,:]]))
    #content.append("POINT_DATA %d"%(z_layer*circumference_points))
    #content.append("SCALARS FLAG float 1")
    #content.append("LOOKUP_TABLE default")
    
    # between each two rows, fill in all cells
    # given n pts in the row, there will be n-1 spaces between, with 2 triangles each
    
    
    
    #
    #for point in points:
    #    content.append("%f"%point[3])
    #
    ## fake grid_area
    ##content.append("POINT_DATA %d"%(z_layer*circumference_points))
    #content.append("SCALARS grid_area float 1")
    #content.append("LOOKUP_TABLE default")
    #
    #for point in points:
    #    content.append("%f"%(1./(z_layer*circumference_points)))
    
    with open("rt0_%d.vtk"%key,'w') as file:
        for line in content:
            file.write(line+'\n')


# Built on the following ASCII VTK example at https://www.visitusers.org/index.php?title=ASCII_VTK_Files
## vtk DataFile Version 3.0
#vtk output
#ASCII
#DATASET UNSTRUCTURED_GRID
#POINTS 4 float
#0 0 0
#1 0 0
#0 1 0
#1.1 1.1 0
#CELLS 1 5
#4 0 1 3 2
#CELL_TYPES 1
#9
#CELL_DATA 1
#POINT_DATA 4
#FIELD FieldData 1
#nodal 1 4 float
#0 1 1.1 2
