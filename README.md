Name: Karthik Srinivasan
USC ID : 1928-0944-95
USC Email: srinivak@usc.edu

Visual Studio 2010 Service Pack 1 Used, but I tried it in non sp version, it worked

Procedure Followed for building Xsm matrix model:

1. Initialized Xsp based on screen resolution
2. Modified Xsp based on camera's field of view and pushed to stack
3. Initialized Xpi based on camera's FOV and pushed to stack (after multiplying with previous top of stack)
4. Xiw was built using camera's properties such as its position, look at point and world up coordinates
5. Xiw was pushed to matrix
	a. Xwm was pushed by application to the stack
6. Triangles coordinates are transformed using Xsm and checked if their vertices fall before the view plane and are discarded
7. Remaining triangles are passed for rasterization



Procedure Followed for building Normal stack.
1. Pushed 3 identity matrix into Xnorm (to keep the similarity between Xnorm and Ximage)
2. Pushed Xiw for stripping scaling and translation vectors
	a. Values in last column changed to 0,0,0,1
	b. Scaling factor caculated by finding square root of sum of squares of element in any row/column
	c. Normalized the matrix by dividing each element by scaling factor
3. Every other subsequent matrices are stripped to unitary rotation matrix
4. Normal coordinates are transformed using Xnorm stack and passed to rasterization



Procedure Followed for Shading and Rasterization.
1. Colour computed at normal of first vertices of triangle for Flat Shading.
2. Sort vertices and normals pair using Y coordinates of vertices.
3. Differentiate edge that need to be shaded by preference, Left over Right and Top over Bottom
4. Store the edge vertices that need to shaded
5. Sort vertices and normals pair using X Coordinates of vertices.
6. determine the edges that need to be shaded using the vertices from (4)
7. if User has choses Gouraud shading compute the colours at three vertices
8. Construct Linear Equation for each edge line
9. Construct planar equation by cross product
10. Interpolate triangle from traversing from top left to bottom right of the rectangle that encloses the trianlge
 a. check whether point lies inside the triangle or on the edge using (8)
 b. Interpolate colour at the point or normals at the point (I used Barycentric interpolation)
 c. Compute colour at this point if phong shading is chosen
 b. Interpolate Z using (9)
 c. Put the pixel into display buffer using the interpolated colour or colour computed for interpolate normals
9. Deallocate Memory.