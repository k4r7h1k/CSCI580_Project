#include "Gz.h"
#include "Skeleton.h"
#include "TriangleMesh.h"
class CharacterModel{
	Skeleton characterSkeleton;
	TriangleMesh characterMesh;
public:
	CharacterModel(char *characterFile,char *skeletonFile,char *boneWeights){
		characterSkeleton.loadSkeletonFile(skeletonFile);
		characterMesh.loadTriangleMesh(characterSkeleton.getNumberOfBones(),characterFile);
		characterMesh.loadWeights(boneWeights);
	}
	void printCharacterConfiguration(){
		cout<<"Number of Vertices : "<< characterMesh.getNumberOfVertices()<<endl;
		cout<<"Number of Triangles : "<< characterMesh.getNumberOfTriangles()<<endl;
		cout<<"Number of Bones : "<<characterSkeleton.getNumberOfBones()<<endl; 
	}
	bool checkSkeleton(){
		return characterSkeleton.checkSkeleton();
	}
	int getTriangleNumber(){return characterMesh.getNumberOfTriangles();}
	void calculateMotion(){
		characterSkeleton.calculateMotionMatrix();
	}
	void getVertexes(int triangleNumber,GzCoord C1,GzCoord C2,GzCoord C3,GzCoord N1,GzCoord N2,GzCoord N3){
		TriangleVertex v1,v2,v3;
		characterMesh.getTriangleVertices(triangleNumber,v1,v2,v3);

		v1.getVertex(C1);	v1.getVertexNormal(N1);
		v2.getVertex(C2);	v2.getVertexNormal(N2);
		v3.getVertex(C3);	v3.getVertexNormal(N3);
		float *weights;
		v1.getWeights(&weights);
		characterSkeleton.transformVertex(triangleNumber, weights,C1);
		v2.getWeights(&weights);
		characterSkeleton.transformVertex(triangleNumber,weights,C2);
		v3.getWeights(&weights);
		characterSkeleton.transformVertex(triangleNumber,weights,C3);
		
	}
	void characterMoveHands(){
		characterSkeleton.moveHands(2,2);
		
		characterSkeleton.moveHands(14,2);
		characterSkeleton.moveHands(11,2);
		//characterSkeleton.moveHands(13,15);
		//characterSkeleton.moveHands(3,2);
	}

};