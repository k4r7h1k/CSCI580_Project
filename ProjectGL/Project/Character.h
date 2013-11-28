#include "Gz.h"
#include "Skeleton.h"
#include "TriangleMesh.h"
int angle=360,angle1=0;
class CharacterModel{
public:
	Skeleton characterSkeleton;
	TriangleMesh characterMesh;
	void loadModel(char *characterFile,char *skeletonFile,char *boneWeights){
		cout<<"Loading Character";
		characterSkeleton.loadSkeletonFile(skeletonFile);
		characterMesh.loadTriangleMesh(characterSkeleton.getNumberOfBones(),characterFile);
		characterMesh.loadWeights(boneWeights);
	}
	CharacterModel(){
	}
	CharacterModel(char *characterFile,char *skeletonFile,char *boneWeights){
		loadModel(characterFile,skeletonFile,boneWeights);
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
	void calculateMotionInverse(){
		characterSkeleton.calculateMotionInverse();
		modifyVertices();
	}
	void modifyVertices(){
		characterMesh.modifyVertices(characterSkeleton);
	}
	void getVertexes(int triangleNumber,GzCoord C1,GzCoord C2,GzCoord C3,GzCoord N1,GzCoord N2,GzCoord N3){
		TriangleVertex v1,v2,v3;
		characterMesh.getTriangleVertices(triangleNumber,v1,v2,v3);
		v1.getMVertex(C1);	v1.getVertexNormal(N1);
		v2.getMVertex(C2);	v2.getVertexNormal(N2);
		v3.getMVertex(C3);	v3.getVertexNormal(N3);
		/*float *weights;
		v1.getWeights(&weights);
		characterSkeleton.transformVertex( weights,C1);
		v2.getWeights(&weights);
		characterSkeleton.transformVertex(weights,C2);
		v3.getWeights(&weights);
		characterSkeleton.transformVertex(weights,C3);
		*/
	}
	void moveCharacter(){
		//characterSkeleton.moveHands(2,1.5);
		//characterSkeleton.moveHands(14,1.5);
		characterSkeleton.moveBoneZ(17,angle+angle1);
		if(angle==350){
			angle1++;
			if(angle1==10)
			{
				angle1=0;
				angle=360;
			}
		}else
			angle--;
		//characterSkeleton.moveHands(13,15);
		//characterSkeleton.moveHands(3,2);

	}

};