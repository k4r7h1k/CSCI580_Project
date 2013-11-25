/*$T \TriangleMesh.h GC 1.150 2013-11-24 16:46:09 */


/*$6*/
#ifndef _TRIANGLEMESH_H
#define _TRIANGLEMESH_H


#include "Gz.h"
#include <cmath>
#include <math.h>
#include <vector>
#include "GzMatrix.h"
#include <fstream>
using namespace std;
class			TriangleVertex;
class			Triangle;
class			TriangleMesh;
class			TriangleVertex
{
	float		*weights;
	GzCoord		coordinates;
	GzCoord		vertexNormal;
	
	vector<int> adjancent_triangles;

/* */
public:

	/* */
	TriangleVertex(){
		initializeGzCoord(coordinates);
		initializeGzCoord(vertexNormal);
	}
	TriangleVertex(float a, float b, float c)
	{
		coordinates[0] = a;
		coordinates[1] = b;
		coordinates[2] = c;
	}

	/* */
	float &operator[](int i)
	{
		return coordinates[i];
	}

	/* */
	float operator[](int i) const
	{
		return coordinates[i];
	}

	/* */
	int numberOfAT()
	{
		return adjancent_triangles.size();
	}

	/* */
	void setVertexNormal(GzCoord n)
	{
		copyGzCoord(vertexNormal, n);
	}

	/* */
	void getVertexNormal(GzCoord n)
	{
		copyGzCoord(n, vertexNormal);
	}
	void getVertex(GzCoord n)
	{
		copyGzCoord(n, coordinates);
	}
	/* */
	void setWeights(int n, float *w)
	{
		//weights = new float[n];
		//for(int i = 0; i < n; i++) weights[i] = w[i];
		weights=w;

	}
	void getWeights(float **w){
		(*w)=weights;
	}
	/* */
	void addAdjancentTriangle(int a)
	{
		adjancent_triangles.push_back(a);
	}

	/* */
	int getAdjancentTriangle(int i)
	{
		return adjancent_triangles[i];
	}
	friend class TriangleMesh;
};
class	Triangle
{
	int		vertex[3];
	GzCoord triangleNormal;

/* */
public:

	/* */
	Triangle(int a, int b, int c)
	{
		vertex[0] = a;
		vertex[1] = b;
		vertex[2] = c;
	}

	/* */
	void getVertices(int &a, int &b, int &c)
	{
		a = vertex[0];
		b = vertex[1];
		c = vertex[2];
	}

	/* */
	void setTriangleNormal(GzCoord n)
	{
		copyGzCoord(triangleNormal, n);
	}

	/* */
	void getTriangleNormal(GzCoord n)
	{
		copyGzCoord(n, triangleNormal);
	}

	friend class	TriangleMesh;
};
class	TriangleMesh
{
	vector<TriangleVertex>	vertices;
	vector<Triangle>		triangles;
	int						numberOfBones, numberOfVertices, numberOfTriangles;

/* */
public:

	/* */
	void loadTriangleMesh(int bones,char *filename)
	{
		numberOfBones = bones;
		
		ifstream	f(filename);
		if(f == NULL)
		{
			cerr << "failed reading polygon data file " << filename << endl;
			exit(1);
		}

		char	buf[1024];
		char	header[100];
		float	x, y, z;
		int t=0;
		int		v1, v2, v3, n1, n2, n3;

		while(!f.eof())
		{
			f.getline(buf, sizeof(buf));
			sscanf(buf, "%s", header);
			if(strcmp(header, "v") == 0)
			{
				sscanf(buf, "%s %f %f %f", header, &x, &y, &z);
				vertices.push_back(TriangleVertex(x, y, z));
			}
			else if(strcmp(header, "f") == 0)
			{
				sscanf(buf, "%s %d %d %d", header, &v1, &v2, &v3);
				float a1,a2,a3,b1,b2,b3;
				GzCoord norm;
				a1=vertices[v3-1][0]-vertices[v1-1][0];
				a2=vertices[v3-1][1]-vertices[v1-1][1];
				a3=vertices[v3-1][2]-vertices[v1-1][2];
				b1=vertices[v2-1][0]-vertices[v1-1][0];
				b2=vertices[v2-1][1]-vertices[v1-1][1];
				b3=vertices[v2-1][2]-vertices[v1-1][2];
				norm[0]=a2*b3-a3*b2;
				norm[1]=a3*b1-a1*b3;
				norm[2]=a1*b2-a2*b1;
				normalizeGzCoord(norm);
				Triangle	trig(v1 - 1, v2 - 1, v3 - 1);
				trig.setTriangleNormal(norm);
				triangles.push_back(trig);
				vertices[v1 - 1].addAdjancentTriangle(t);
				vertices[v2 - 1].addAdjancentTriangle(t);
				vertices[v3 - 1].addAdjancentTriangle(t);
				t++;
			}
		}

		numberOfVertices = vertices.size();
		numberOfTriangles = triangles.size();
		calculateVertexNormals();
		f.close();
	}

	/* */
	void loadWeights(char *filename)
	{
		
		ifstream	f(filename);
		if(f == NULL)
		{
			cerr << "failed reading polygon data file " << filename << endl;
			exit(1);
		}
		char		buf[1024];
		char		header[100];
		int			i = 0;
		float		*weights= new float[numberOfBones];
		int			j = 0;
		while(!f.eof())
		{
			
			f.getline(buf, sizeof(buf), ' ');
			sscanf(buf, "%f", &weights[j]);
			j++;
			if(j == numberOfBones)
			{
				j = 0;
				vertices[i].setWeights(numberOfBones,weights);
				weights= new float[numberOfBones];
				i++;
			}
		}

		//delete[] weights;
	}

	/* */
	void setWeight(int vertexNumber, float *w)
	{
		vertices[vertexNumber].setWeights(numberOfBones, w);
	}

	/* */
	int getNumberOfTriangles()
	{
		return numberOfTriangles;
	}

	/* */
	int getNumberOfVertices()
	{
		return numberOfVertices;
	}

	/* */
	void getTriangleVertices(int triangleNumber, TriangleVertex &v1, TriangleVertex &v2, TriangleVertex &v3)
	{
		vertices[triangles[triangleNumber].vertex[0]].getVertex(v1.coordinates);
		vertices[triangles[triangleNumber].vertex[1]].getVertex(v2.coordinates);
		vertices[triangles[triangleNumber].vertex[2]].getVertex(v3.coordinates);;
		vertices[triangles[triangleNumber].vertex[0]].getVertexNormal(v1.vertexNormal);
		vertices[triangles[triangleNumber].vertex[1]].getVertexNormal(v2.vertexNormal);
		vertices[triangles[triangleNumber].vertex[2]].getVertexNormal(v3.vertexNormal);
		vertices[triangles[triangleNumber].vertex[0]].getWeights(&v1.weights);
		vertices[triangles[triangleNumber].vertex[1]].getWeights(&v2.weights);
		vertices[triangles[triangleNumber].vertex[2]].getWeights(&v3.weights);
	}

	/* */
	void getTriangleVertexNumber(int triangleNumber, int &v1, int &v2, int &v3)
	{
		v1 = triangles[triangleNumber].vertex[0];
		v2 = triangles[triangleNumber].vertex[1];
		v3 = triangles[triangleNumber].vertex[2];
	}

	/* */
	void setTriangleNormal(int triangleNumber, GzCoord n)
	{
		triangles[triangleNumber].setTriangleNormal(n);
	}

	/* */
	void getTriangleNormal(int triangleNumber, GzCoord n)
	{
		triangles[triangleNumber].getTriangleNormal(n);
	}

	/* */
	void getVertexNormal(int vertexNumber, GzCoord n)
	{
		vertices[vertexNumber].getVertexNormal(n);
	}

	/* */
	void calculateVertexNormals()
	{
		GzCoord normal;
		for(int i = 0; i < numberOfVertices; i++)
		{
			initializeGzCoord(normal);

			int numberOfAdjancentTriangles = vertices[i].numberOfAT();
			for(int j = 0; j < numberOfAdjancentTriangles; j++)
			{
				GzCoord triangleNormal;
				triangles[vertices[i].getAdjancentTriangle(j)].getTriangleNormal(triangleNormal);
				addGzCoords(normal, triangleNormal);
			}

			scalarGzCoord(normal, ((float)1.0 / numberOfAdjancentTriangles));
			normalizeGzCoord(normal);
			vertices[i].setVertexNormal(normal);
			vertices[i].adjancent_triangles.clear();
		}
		
	}
};
#endif