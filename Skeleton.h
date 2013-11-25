/*$T \Skeleton.h GC 1.150 2013-11-24 14:10:06 */


/*$6*/

#ifndef _SKELETON_H
#define _SKELETON_H
#include "stdafx.h"
#include "Gz.h"
#include "TriangleMesh.h"
#include <iostream>
#include <fstream>
#include "GzMatrix.h"
using namespace std;
class			Skeleton
{
	GzMatrix	*translation_Matrix;
	GzMatrix	*rotation_Matrix;
	GzMatrix	*inverse_Matrix;
	GzMatrix	*motion_Matrix;
	GzMatrix	*motionInverse;
	int			*previousBone;
	int			numberOfBones;

/* */
public:

	/* */
	void loadSkeletonFile(char *filename)
	{
		GzCoord		*b = new GzCoord[25];
		GzCoord		*tv = new GzCoord[25];
		GzMatrix	*TM = new GzMatrix[25];
		previousBone	= new int[25];
		int			bn, bp;
		float		bx, by, bz;
		ifstream	out(filename);
		if(out == NULL)
		{
			cerr << "failed reading polygon data file " << filename << endl;
			exit(1);
		}
		char		a[1024];
		int			bonecounter = 0;
		while(!out.eof())
		{
			out.getline(a, 1024);
			sscanf(a, "%d %f %f %f %d", &bn, &bx, &by, &bz, &bp);
			b[bn][0] = bx;
			b[bn][1] = by;
			b[bn][2] = bz;
			previousBone[bn]=bp;
			setIdentityMatrix(TM[bn]);
			if(bp == -1)
			{
				tv[bn][0] = b[bn][0] - b[bn][0];
				tv[bn][1] = b[bn][1] - b[bn][1];
				tv[bn][2] = b[bn][2] - b[bn][2];
			}
			else
			{
				bonecounter++;
				tv[bn][0] = b[bn][0] - b[bp][0];
				tv[bn][1] = b[bn][1] - b[bp][1];
				tv[bn][2] = b[bn][2] - b[bp][2];
				if(bp != 0) copyMatrices(TM[bn - 1], TM[bp - 1]);
				addTranslationMatrix(TM[bn - 1], tv[bn]);
			}
		}

		numberOfBones = bn;
		inverse_Matrix = new GzMatrix[bonecounter];
		translation_Matrix = new GzMatrix[bonecounter];
		rotation_Matrix = new GzMatrix[bonecounter];
		for(int i = 0; i < numberOfBones; i++)
		{
			setIdentityMatrix(rotation_Matrix[i]);
			setIdentityMatrix(translation_Matrix[i]);
			addTranslationMatrix(translation_Matrix[i], tv[i + 1]);
			inverseTranslateCopyMatrix(inverse_Matrix[i], TM[i]);
		}

		delete[] TM;
		delete[] tv;
		delete[] b;
	}
	void calculateMotionMatrix(){
		motion_Matrix=new GzMatrix[numberOfBones];
		for(int i=0;i<numberOfBones;i++)
		{
			if(previousBone[i+1]==0)
				matrixMultiplication(rotation_Matrix[i],translation_Matrix[i],motion_Matrix[i]);
			else
				matrixMultiplication(motion_Matrix[previousBone[i+1]-1],rotation_Matrix[i],translation_Matrix[i],motion_Matrix[i]);
		}
	}
	/* */
	void printSkeletonConfiguration()
	{
		cout << "Number of Bones: " << numberOfBones;
		cout << "Translation Matrix\n";
		for(int i = 0; i < numberOfBones; i++)
		{
			printMatrix(translation_Matrix[i]);
		}

		cout << "Rotation Matrix\n";
		for(int i = 0; i < numberOfBones; i++)
		{
			printMatrix(rotation_Matrix[i]);
		}

		cout << "Inverse Matrix\n";
		for(int i = 0; i < numberOfBones; i++)
		{
			printMatrix(inverse_Matrix[i]);
		}
	}
	int getNumberOfBones(){
		return numberOfBones;
	}
	bool checkSkeleton(){
		calculateMotionMatrix();
		for(int i=0;i<numberOfBones;i++){
			GzMatrix temp;
			matrixMultiplication(motion_Matrix[i],inverse_Matrix[i],temp);
			printMatrix(temp);
			if(isIdentityMatrix(temp)==false)
				return false;
		}
		return true;
	}
	void transformVertex(int triangleNumber,float *weight,GzCoord v){
		motionInverse=new GzMatrix[numberOfBones];
		GzCoord temp,neww;
		initializeGzCoord(neww);
		for(int i=0;i<numberOfBones;i++){
			matrixMultiplication(motion_Matrix[i],inverse_Matrix[i],motionInverse[i]);
		}
		for(int i=0;i<numberOfBones;i++){
			copyGzCoord(temp,v);
			transformVertices(temp,motionInverse[i]);
			if(weight==NULL)
				i=i;
			scalarGzCoord(temp,weight[i]);
			addGzCoords(neww,temp);
		}
		copyGzCoord(v,neww);
	}
	void moveHands(){
		if(isIdentityMatrix(rotation_Matrix[9]))
		{
			rotation_Matrix[9][0][0]=cos(-20*3.143/180);
			rotation_Matrix[9][0][2]=sin(-20*3.143/180);
			rotation_Matrix[9][2][0]=-sin(-20*3.143/180);
			rotation_Matrix[9][2][2]=cos(-20*3.143/180);
		}
		else setIdentityMatrix(rotation_Matrix[9]);
	}
};
#endif