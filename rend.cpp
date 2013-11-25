/* CS580 Homework 4 */

#include	"stdafx.h"
#include	"stdio.h"
#include	"math.h"
#include	"Gz.h"
#include	"rend.h"
#define degToRad(x) (x*3.14159/180) //Converts Degree to Radians
GzCoord viewVector={0,0,-1};
void unitaryRotation(GzMatrix m);
float detTriangle(GzCoord *c);
void interpolationCoefficients(float detT,GzCoord *coord,int i,int j,float &u,float &v,float &w);
void edgeCheck(GzCoord *coord,float &x1,float &y1,float &x11,float &y11,float &x2,float &y2,float &x22,float &y22);
void calculateColor(GzRender *r,GzCoord *n,GzIntensity &red,GzIntensity &green,GzIntensity &blue,int q);
void sortCoord(GzCoord *coord,GzCoord *norm,float &ymin,float &xmin,float &ymax,float &xmax); //Sorts the vertices
void sortCoordX(GzCoord *coord,GzCoord *norm,float &xmin,float &xmax); //Sorts the vertices by X
void sortCoordY(GzCoord *coord,GzCoord *norm,float &ymin,float &ymax); //Sorts the vertices by Y
void swap(GzCoord *coord,int a,int b); // Swaps the vertices
void checkEdge(GzCoord *coord,float x1,float y1,float x11,float y11,bool *edgeColor); //Check whether two edges are same
void createLE(GzCoord *edges, GzCoord *coord); //Creates Line equation for each edges
short	ctoi(float color); //Converts float color to 12bit colots
bool checkPoint(GzCoord *edges, int x, int y,bool *edgeColor); //Checks whether the point lies inside the triangle
void create3DLE(FLOAT *abcd,GzCoord *coord); //Creates 3-D line equation
int interpolateZ(FLOAT *abcd,int x, int y); //Interpolates Z
void initializeMat(GzMatrix x, int p=0); //Initializes matrix
void initializeXsp(GzMatrix Xsp,int xres,int yres); //Initializes Xsp matrix
void setDeafultCamera(GzCamera &camera); //Sets Default Camera
void copyMatrix(GzMatrix m1, GzMatrix m2); //Copies one matrix to another
void mulitplyMatrix(GzMatrix result,GzMatrix m1, GzMatrix m2); // Multiplies matrix m1*m2 and stores it in result
float dotProduct(GzCoord c1,GzCoord c2); //Finds dot product of two vectors
void crossProduct(GzCoord c1,GzCoord c2,GzCoord c3); //Finds Cross  Product and stores it in c3
void normaliseVector(GzCoord c); //Normalizes the Vector
bool transformVertices(GzCoord *c,GzMatrix m); // Transforms matrices based Xsm matrix
int GzRotXMat(float degree, GzMatrix mat)
{
// Create rotate matrix : rotate along x axis
// Pass back the matrix using mat value
	initializeMat(mat);
	mat[0][0]=1;
	mat[3][3]=1;
	degree=degToRad(degree);
	mat[1][1]=cos(degree);
	mat[1][2]=-1*sin(degree);
	mat[2][1]=sin(degree);
	mat[2][2]=cos(degree);
	return GZ_SUCCESS;
}
int GzRotYMat(float degree, GzMatrix mat)
{
// Create rotate matrix : rotate along y axis
// Pass back the matrix using mat value
	initializeMat(mat);
	degree=degToRad(degree);
	mat[0][0]=cos(degree);
	mat[0][2]=sin(degree);
	mat[1][1]=1; mat[3][3]=1;
	mat[2][0]=-1*sin(degree);
	mat[2][2]=cos(degree);
	return GZ_SUCCESS;
}
int GzRotZMat(float degree, GzMatrix mat)
{
// Create rotate matrix : rotate along z axis
// Pass back the matrix using mat value
	initializeMat(mat);
	degree=degToRad(degree);
	mat[0][0]=cos(degree);
	mat[0][1]=-1*sin(degree);
	mat[1][0]=sin(degree);
	mat[1][1]=cos(degree);
	mat[2][2]=mat[3][3]=1;

	return GZ_SUCCESS;
}
int GzTrxMat(GzCoord translate, GzMatrix mat)
{
// Create translation matrix
// Pass back the matrix using mat value
	initializeMat(mat);
	mat[0][0]=mat[1][1]=mat[2][2]=mat[3][3]=1;
	mat[0][3]=translate[0];
	mat[1][3]=translate[1];
	mat[2][3]=translate[2];
	return GZ_SUCCESS;
}
int GzScaleMat(GzCoord scale, GzMatrix mat)
{
// Create scaling matrix
// Pass back the matrix using mat value
	initializeMat(mat);
	mat[0][0]=scale[0];
	mat[1][1]=scale[1];
	mat[2][2]=scale[2];
	mat[3][3]=1;
	return GZ_SUCCESS;
}
//----------------------------------------------------------
// Begin main functions

int GzNewRender(GzRender **render, GzDisplay	*display)
{
/*  
- malloc a renderer struct 
- setup Xsp and anything only done once 
- save the pointer to display 
- init default camera 
*/ 
	if(display==NULL)
		return GZ_FAILURE;
	*render=new GzRender;
	if(*render==NULL)
		return GZ_FAILURE;
	(**render).display=display;
	initializeXsp((**render).Xsp,display->xres,display->yres);
	setDeafultCamera((**render).camera);
	(**render).matlevel=-1;
	(**render).numlights=-1;
	(**render).flatcolor[RED]=1;
	(**render).flatcolor[BLUE]=1;
	(**render).flatcolor[GREEN]=1;
	GzMatrix m;
	initializeMat(m,1); // Identity Matrix
	GzPushMatrix(*render,m); //Initializing Stack with Identity matrix
	return GZ_SUCCESS;

}
int GzFreeRender(GzRender *render)
{
/* 
-free all renderer resources
*/
	if(render==NULL)
		return GZ_FAILURE;
	delete render;
	render=NULL;
	return GZ_SUCCESS;
}
int GzBeginRender(GzRender *render)
{
/*  
- setup for start of each frame - init frame buffer color,alpha,z
- compute Xiw and projection xform Xpi from camera definition 
- init Ximage - put Xsp at base of stack, push on Xpi and Xiw 
- now stack contains Xsw and app can push model Xforms when needed 
*/ 
	if(render==NULL)
		return GZ_FAILURE;

	GzInitDisplay(render->display);
	render->matlevel=0;
	GzMatrix m; initializeMat(m,1);
	m[3][2]=tan(degToRad(render->camera.FOV/2));
	render->Xsp[2][2]=INT_MAX*m[3][2];
	GzPushMatrix(render,render->Xsp); //Xsp
	GzPushMatrix(render,m); //Xpi

	GzCoord ZAxis,YAxis,XAxis;
	GzCamera *camera=&(render->camera);
	ZAxis[0]=camera->lookat[0]-camera->position[0];
	ZAxis[1]=camera->lookat[1]-camera->position[1];
	ZAxis[2]=camera->lookat[2]-camera->position[2];
	normaliseVector(ZAxis); //Camera ZAxis
	float dp=dotProduct(camera->worldup,ZAxis);
	YAxis[0]=camera->worldup[0]-ZAxis[0]*dp;
	YAxis[1]=camera->worldup[1]-ZAxis[1]*dp;
	YAxis[2]=camera->worldup[2]-ZAxis[2]*dp;
	normaliseVector(YAxis); //Camera YAxis
	crossProduct(YAxis,ZAxis,XAxis);
	normaliseVector(XAxis); //Camera XAxis
	m[3][0]=m[3][1]=m[3][2]=0;m[3][3]=1;
	m[0][3]=dotProduct(XAxis,camera->position)*-1;
	m[1][3]=dotProduct(YAxis,camera->position)*-1;
	m[2][3]=dotProduct(ZAxis,camera->position)*-1;
	m[0][0]=XAxis[0];m[0][1]=XAxis[1];m[0][2]=XAxis[2];
	m[1][0]=YAxis[0];m[1][1]=YAxis[1];m[1][2]=YAxis[2];
	m[2][0]=ZAxis[0];m[2][1]=ZAxis[1];m[2][2]=ZAxis[2];
	GzPushMatrix(render,m);
	return GZ_SUCCESS;
}
int GzPutCamera(GzRender *render, GzCamera *camera)
{
/*
- overwrite renderer camera structure with new camera definition
*/
	if(render==NULL || camera==NULL)
		return GZ_FAILURE;
	render->camera.FOV=camera->FOV;
	memcpy(render->camera.lookat,camera->lookat,sizeof(GzCoord));
	memcpy(render->camera.position,camera->position,sizeof(GzCoord));
	memcpy(render->camera.worldup,camera->worldup,sizeof(GzCoord));
	return GZ_SUCCESS;	
}
int GzPushMatrix(GzRender *render, GzMatrix	matrix)
{
/*
- push a matrix onto the Ximage stack
- check for stack overflow
*/	
	if(render==NULL||render->matlevel>=MATLEVELS-1 ){ //Overflow 
		return GZ_FAILURE;
	}
	if(render->matlevel==-1){
		render->matlevel++;
		copyMatrix(render->Ximage[render->matlevel],matrix);
		copyMatrix(render->Xnorm[render->matlevel],matrix);
		copyMatrix(render->Xnorm[1],matrix);//Dummy Identity matrix for Xnorm
		copyMatrix(render->Xnorm[2],matrix);//Dummy Identity matrix for Xnorm
	}
	else{
		render->matlevel++;
		mulitplyMatrix(render->Ximage[render->matlevel],render->Ximage[render->matlevel-1],matrix);
		if(render->matlevel>2){
			unitaryRotation(matrix);
			mulitplyMatrix(render->Xnorm[render->matlevel],render->Xnorm[render->matlevel-1],matrix);
		}

	}
	return GZ_SUCCESS;
}
int GzPopMatrix(GzRender *render)
{
/*
- pop a matrix off the Ximage stack
- check for stack underflow
*/
	if(render==NULL || render->matlevel==-1) //Underflow
		return GZ_FAILURE;
	else 
		render->matlevel--;
	return GZ_SUCCESS;
}
int GzPutAttribute(GzRender	*render, int numAttributes, GzToken	*nameList, 
	GzPointer	*valueList) /* void** valuelist */
{
/*
- set renderer attribute states (e.g.: GZ_RGB_COLOR default color)
- later set shaders, interpolaters, texture maps, and lights
*/
	if(render==NULL || nameList==NULL || valueList==NULL)
		return GZ_FAILURE;
	GzColor *color;
	GzLight *light;
	int *m;
	float *a;
	for(int i=0;i<numAttributes;i++)
	{
		switch (nameList[i])
		{
			case GZ_RGB_COLOR:
				color = (GzColor *)valueList[i];
				memcpy(render->flatcolor,color,sizeof(GzColor *));
				break;
			case GZ_INTERPOLATE:
				m=(int*) valueList[i];
				render->interp_mode=*m;
				break;
			case GZ_DIRECTIONAL_LIGHT:
				if(render->numlights<MAX_LIGHTS-1)
				{
					int a=++render->numlights;
					light=(GzLight *) valueList[i];
					normaliseVector(light->direction);
					memcpy(render->lights[a].color,light->color,sizeof(GzLight));
					memcpy(render->lights[a].direction,light->direction,sizeof(GzCoord));
				}
				break;
			case GZ_AMBIENT_LIGHT:
				light=(GzLight *) valueList[i];
				memcpy(render->ambientlight.color,light->color,sizeof(GzLight));
				memcpy(render->ambientlight.direction,light->direction,sizeof(GzCoord));
				break;
			case GZ_AMBIENT_COEFFICIENT:
				color=(GzColor *) valueList[i];
				memcpy(render->Ka,color,sizeof(GzColor));
				break;
			case GZ_DIFFUSE_COEFFICIENT:
				color=(GzColor *) valueList[i];
				memcpy(render->Kd,color,sizeof(GzColor));
				break;
			case GZ_SPECULAR_COEFFICIENT:
				color=(GzColor *) valueList[i];
				memcpy(render->Ks,color,sizeof(GzColor));
				break;
			case GZ_DISTRIBUTION_COEFFICIENT:
				a=(float *) valueList[i];
				render->spec=*a;
				break;
			default:break;
		}
	}
	return GZ_SUCCESS;
}
int GzPutTriangle(GzRender	*render, int numParts, GzToken *nameList, GzPointer	*valueList)
{
/*  
- pass in a triangle description with tokens and values corresponding to 
      GZ_POSITION:3 vert positions in model space 
- Xform positions of verts using matrix on top of stack 
- Clip - just discard any triangle with any vert(s) behind view plane 
       - optional: test for triangles with all three verts off-screen (trivial frustum cull)
- invoke triangle rasterizer  
*/ 	if(render==NULL || nameList==NULL || valueList==NULL)
		return GZ_FAILURE;
	GzCoord *coord,*norm;
	GzIntensity red,blue,green;
	GzIntensity red1,blue1,green1;
	GzIntensity red2,blue2,green2;
	GzIntensity red3,blue3,green3;
	for(int i=0;i<numParts;i++){
		switch (nameList[i])
		{
			case GZ_NULL_TOKEN:
				break;
			case GZ_POSITION:
				coord= (GzCoord *) valueList[i];
				break;
			case GZ_NORMAL:
				norm= (GzCoord *) valueList[i];
				break;
			default:break;
		}
	}
	if(transformVertices(coord,render->Ximage[render->matlevel])==false)
		return GZ_SUCCESS;
	float ymin,xmin,ymax,xmax;
	bool *edgeColor=new bool[3];
	// Initializing all edges condition to false
	edgeColor[0]=edgeColor[1]=edgeColor[2]=false;
	float x1=0,y1=0,x11=0,y11=0;
	float x2,y2,x22,y22;
	x2=y2=x22=y22=NULL;
	//Raterize edges of Triangle based on preference, Left over Right and Top over Bottom.
	transformVertices(norm,render->Xnorm[render->matlevel]);
	calculateColor(render,norm,red,green,blue,0);//Computing colors at each vertex, V0
	sortCoordY(coord,norm,ymin,ymax);
	edgeCheck(coord,x1,y1,x11,y11,x2,y2,x22,y22);
	sortCoordX(coord,norm,xmin,xmax);
	checkEdge(coord,x1,y1,x11,y11,edgeColor);
	if(x2!=NULL)
		checkEdge(coord,x2,y2,x22,y22,edgeColor);
	if(render->interp_mode==GZ_COLOR){
		calculateColor(render,norm,red1,green1,blue1,0);//Computing colors at each vertex, V0
		calculateColor(render,norm,red2,green2,blue2,1);//Computing colors at each vertex, V1
		calculateColor(render,norm,red3,green3,blue3,2);//Computing colors at each vertex, V2
	}
	float u,v,w;
#if 1
	GzCoord *edges=new GzCoord[3];
	FLOAT *abcd=new FLOAT[4];
	float detT=detTriangle(coord);
	createLE(edges,coord);
	create3DLE(abcd,coord);
	ymax=ceil(ymax);
	xmax=ceil(xmax);
	//Raterizeses each pixel inside the triangle without plotting the edges.
	for(int j=floor(ymin);j<=ymax;j++){
		for(int i=floor(xmin);i<=xmax;i++){
			if(checkPoint(edges,i,j,edgeColor))
			{
				interpolationCoefficients(detT,coord,i,j,u,v,w);
				if(render->interp_mode==GZ_COLOR){
					red=red1*u+red2*v+red3*w;
					green=green1*u+green2*v+green3*w;
					blue=blue1*u+blue2*v+blue3*w;
				}
				else if(render->interp_mode==GZ_NORMALS){
					GzCoord *nn=new GzCoord[1];
					nn[0][0]=norm[0][0]*u+norm[1][0]*v+norm[2][0]*w;
					nn[0][1]=norm[0][1]*u+norm[1][1]*v+norm[2][1]*w;
					nn[0][2]=norm[0][2]*u+norm[1][2]*v+norm[2][2]*w;
					calculateColor(render,nn,red,green,blue,0);
					delete[] nn;
				}
				GzPutDisplay(render->display,i,j,red,green,blue,1,interpolateZ(abcd,i,j));
			}
		}
	}
	delete[] edges;
	delete abcd;
#else
	//ScanLine implementation
#endif
	delete edgeColor;
	return GZ_SUCCESS;


}

/* NOT part of API - just for general assistance */

short	ctoi(float color)		/* convert float color to GzIntensity short */
{
  return(short)((int)(color * ((1 << 12) - 1)));
}
void sortCoordY(GzCoord *coord,GzCoord *norm,float &ymin,float &ymax){
	ymin=coord[0][1];
	int a=0,i=1;
	for(i=1;i<3;i++)
	{
		if(coord[i][1]<ymin)
		{
			ymin=coord[i][1];
			a=i;
		}
	}
	swap(coord,a,0);
	swap(norm,a,0);
	ymax=coord[1][1];
	a=1;
	if(coord[2][1]>ymax)
	{
			ymax=coord[2][1];
			a=2;
	}
	swap(coord,2,a);
	swap(norm,2,a);
}
void sortCoordX(GzCoord *coord,GzCoord *norm,float &xmin,float &xmax)
{
	int a=0,i=1;
	xmin=coord[0][0];
	a=0;
	for(i=1;i<3;i++)
	{
		if(coord[i][0]<xmin)
		{
			xmin=coord[i][0];
			a=i;
		}
	}
	swap(coord,0,a);
	swap(norm,0,a);
	xmax=coord[1][0];
	a=1;
	if(coord[2][0]>xmax)
	{
		xmax=coord[2][0];
		a=2;
	}
	swap(coord,a,2);
	swap(norm,a,2);
}
void sortCoord(GzCoord *coord,GzCoord *norm,float &ymin,float &xmin,float &ymax,float &xmax){
	sortCoordY(coord,norm,ymin,ymax);
	sortCoordX(coord,norm,xmin,xmax);
}
void swap(GzCoord *coord,int a,int b){
	if(a==b)
		return;
	float x,y,z;
	x=coord[a][0];	y=coord[a][1];	z=coord[a][2];
	coord[a][0]=coord[b][0];coord[b][0]=x;
	coord[a][1]=coord[b][1];coord[b][1]=y;
	coord[a][2]=coord[b][2];coord[b][2]=z;
}
void createLE(GzCoord *edges, GzCoord *coord){
	edges[0][0]=coord[1][1]-coord[0][1];	//A Value
	edges[1][0]=coord[2][1]-coord[1][1];
	edges[2][0]=coord[0][1]-coord[2][1];
	edges[0][1]=coord[1][0]-coord[0][0];	//B Value
	edges[1][1]=coord[2][0]-coord[1][0];
	edges[2][1]=coord[0][0]-coord[2][0];
	edges[0][2]=edges[0][1]*coord[0][1]-edges[0][0]*coord[0][0]; //C Value
	edges[1][2]=edges[1][1]*coord[1][1]-edges[1][0]*coord[1][0];
	edges[2][2]=edges[2][1]*coord[2][1]-edges[2][0]*coord[2][0];
	edges[0][1]*=-1;
	edges[1][1]*=-1;
	edges[2][1]*=-1;
}
bool checkPoint(GzCoord *edges, int x, int y,bool *edgeColor){
	float a=edges[0][0]*x+edges[0][1]*y+edges[0][2];
	float b=edges[1][0]*x+edges[1][1]*y+edges[1][2];
	float c=edges[2][0]*x+edges[2][1]*y+edges[2][2];
	if((a>0 || (a==0 && edgeColor[0])) && (b>0 || (b==0 && edgeColor[1])) && (c>0 || (c==0 && edgeColor[2])) )
		return true;
	else if((a<0 || (a==0 && edgeColor[0])) && (b<0 || (b==0 && edgeColor[1])) && (c<0 || (c==0 && edgeColor[2])) )
		return true;
	return false;
}
void create3DLE(FLOAT *abcd,GzCoord *coord){
	float x1,y1,z1,x2,y2,z2;
	x1=coord[1][0]-coord[0][0];
	x2=coord[2][0]-coord[0][0];
	y1=coord[1][1]-coord[0][1];
	y2=coord[2][1]-coord[0][1];
	z1=coord[1][2]-coord[0][2];
	z2=coord[2][2]-coord[0][2];
	abcd[0]=y1*z2-z1*y2;	//A Value
	abcd[1]=z1*x2-x1*z2;	//B Value
	abcd[2]=x1*y2-y1*x2;	//C Value
	abcd[3]=abcd[0]*coord[0][0]+abcd[1]*coord[0][1]+abcd[2]*coord[0][2];	
	abcd[3]*=-1;			//D Value
}
int interpolateZ(FLOAT *abcd,int x,int y){
	return (int)(((((abcd[0]*x)+(abcd[1]*y)+abcd[3])*-1)/abcd[2])+0.5);
}
void checkEdge(GzCoord *coord,float x1,float y1,float x11,float y11,bool *edgeColor){
	for(int i=0;i<3;i++)
	{
		for(int j=0;j<3;j++){
			if(coord[i][0]==x1 && coord[i][1]==y1 && coord[j][0]==x11 && coord[j][1]==y11){
				if((i==0 && j==1) || (i==1 && j==0))
				{edgeColor[0]=true;break;}
				if((i==0 && j==2) || (i==2 && j==0))
				{edgeColor[2]=true;break;}
				if((i==1 && j==2) || (i==2 && j==1))
				{edgeColor[1]=true;break;}
			}
		}
	}
}
void initializeMat(GzMatrix x, int p){
	for(int i=0;i<4;i++)
		for(int j=0;j<4;j++)
		{
			x[i][j]=0;
			if(i==j)x[i][j]=p;
		}
}
void initializeXsp(GzMatrix Xsp,int xres, int yres){
	initializeMat(Xsp);
	Xsp[2][2]=Xsp[3][3]=1;
	int a=xres/2;
	int b=yres/2;
	Xsp[0][0]=Xsp[0][3]=a;
	Xsp[1][3]=b;
	Xsp[1][1]=b*-1;
}
void setDeafultCamera(GzCamera &camera){
	camera.FOV=DEFAULT_FOV;
	camera.lookat[0]=camera.lookat[1]=camera.lookat[2]=0;
	camera.worldup[0]=camera.worldup[2]=0;
	camera.worldup[1]=1;
	camera.position[0]=DEFAULT_IM_X;
	camera.position[1]=DEFAULT_IM_Y;
	camera.position[2]=DEFAULT_IM_Z;

}
void copyMatrix(GzMatrix m1, GzMatrix m2){
	for(int i=0;i<4;i++)
		for(int j=0;j<4;j++)
			m1[i][j]=m2[i][j];
}
void mulitplyMatrix(GzMatrix result,GzMatrix m1, GzMatrix m2){
	
	for(int i=0;i<4;i++){
		for(int j=0;j<4;j++){
			result[i][j]=0;
			for(int k=0;k<4;k++){
				result[i][j]=result[i][j]+(m1[i][k] * m2[k][j]);
			}
		}
	}
}
float dotProduct(GzCoord c1,GzCoord c2){
	return c1[0]*c2[0]+c1[1]*c2[1]+c1[2]*c2[2];
}
void crossProduct(GzCoord c1,GzCoord c2,GzCoord c3){
	c3[0]=c1[1]*c2[2]-c1[2]*c2[1];
	c3[1]=c1[2]*c2[0]-c1[0]*c2[2];
	c3[2]=c1[0]*c2[1]-c1[1]*c2[0];
}
void normaliseVector(GzCoord c){
	float d=sqrt (c[0]*c[0]+c[1]*c[1]+c[2]*c[2]);
	c[0]/=d;
	c[1]/=d;
	c[2]/=d;
}
bool transformVertices(GzCoord *c,GzMatrix m){
	float b[4];
	bool flag=true;
	for(int i=0;i<3;i++)
	{
		float a[4];a[0]=c[i][0];a[1]=c[i][1];a[2]=c[i][2];a[3]=1;
		for(int j=0;j<4;j++)
		{
			b[j]=0;
			for(int k=0;k<4;k++)
			{
				b[j]+=a[k]*m[j][k];
			}
			
		}
		
		c[i][0]=b[0]/b[3];
		c[i][1]=b[1]/b[3];
		if(b[2]<0)
			flag=false;
		c[i][2]=b[2]/b[3];
	}
	

	return flag;
}
void edgeCheck(GzCoord *coord,float &x1,float &y1,float &x11,float &y11,float &x2,float &y2,float &x22,float &y22){
	if(coord[0][1]==coord[1][1])
	{	
		//Draw Top Edge of the Triangle V1-V2
		x1=coord[0][0];y1=coord[0][1];x11=coord[1][0];y11=coord[1][1];
		//Draw (Bottom) Left Edge V2-V3
		x2=coord[2][0];y2=coord[2][1];x22=coord[1][0];y22=coord[1][1];
	}
	else if(coord[1][1]==coord[2][1])
	{
		//Draw (Top) Left Edge V1-V3
		x1=coord[0][0];y1=coord[0][1];x11=coord[2][0];y11=coord[2][1];
	}
	else
	{
		float x=0;
		//Solve intercepting x point
		x= ((coord[1][1]-coord[0][1])/(coord[2][1]-coord[0][1]))*(coord[2][0]-coord[0][0])+coord[0][0];
		if(x<coord[1][0]){
			//Draw V1-V2 & V2-V3
			x1=coord[0][0];y1=coord[0][1];x11=coord[1][0];y11=coord[1][1];
			x2=coord[2][0];y2=coord[2][1];x22=coord[1][0];y22=coord[1][1];
		}
		else{
			//Draw V3-V1
			x1=coord[0][0];y1=coord[0][1];x11=coord[2][0];y11=coord[2][1];
		}
	}
}
void unitaryRotation(GzMatrix m){
	m[0][3]=0;m[1][3]=0;m[2][3]=0;
	m[3][0]=m[3][1]=m[3][2]=0;m[3][3]=1;
	float k=sqrt(m[0][0]*m[0][0]+m[1][0]*m[1][0]+m[2][0]*m[2][0]);
	m[0][0]/=k;m[0][1]/=k;m[0][2]/=k;
	m[1][0]/=k;m[1][1]/=k;m[1][2]/=k;
	m[2][0]/=k;m[2][1]/=k;m[2][2]/=k;
}
float detTriangle(GzCoord *c){
	return (c[1][1]-c[2][1])*(c[0][0]-c[2][0])+(c[2][0]-c[1][0])*(c[0][1]-c[2][1]);
}
void interpolationCoefficients(float detT,GzCoord *coord,int i,int j,float &u,float &v,float &w){
	u=(coord[1][1]-coord[2][1])*(i-coord[2][0])+(coord[2][0]-coord[1][0])*(j-coord[2][1]);
	v=(coord[2][1]-coord[0][1])*(i-coord[2][0])+(coord[0][0]-coord[2][0])*(j-coord[2][1]);
	u/=detT;
	v/=detT;
	w=1-u-v;
}
void calculateColor(GzRender *r,GzCoord *n,GzIntensity &red,GzIntensity &green,GzIntensity &blue,int q){
	GzColor col;
	col[RED]=r->Ka[RED]*r->ambientlight.color[RED];
	col[GREEN]=r->Ka[GREEN]*r->ambientlight.color[GREEN];
	col[BLUE]=r->Ka[BLUE]*r->ambientlight.color[BLUE];
	GzCoord nn,ref;
	for(int i=0;i<=r->numlights;i++){
		nn[0]=n[q][0];
		nn[1]=n[q][1];
		nn[2]=n[q][2];
		normaliseVector(nn);
		float dp1=dotProduct(nn,viewVector);
		float dp2=dotProduct(nn,r->lights[i].direction);
		if(dp1<0 && dp2<0){
			nn[0]*=-1;
			nn[1]*=-1;
			nn[2]*=-1;
			dp2*=-1; 
		}
		else if((dp1>=0 && dp2<0)||(dp1<0 && dp2>=0))
			continue;
		ref[0]=((2*dp2*nn[0])-r->lights[i].direction[0]);	
		ref[1]=((2*dp2*nn[1])-r->lights[i].direction[1]);	
		ref[2]=((2*dp2*nn[2])-r->lights[i].direction[2]);
		normaliseVector(ref);
		dp1=dotProduct(ref,viewVector);
		if(dp1<0)dp1=0;
		if(dp1>1)dp1=1;
		if(dp2>1)dp2=1;
		dp1=pow(dp1,r->spec);
		col[RED]+=r->Ks[RED]*r->lights[i].color[RED]*dp1;
		col[GREEN]+=r->Ks[GREEN]*r->lights[i].color[GREEN]*dp1;
		col[BLUE]+=r->Ks[BLUE]*r->lights[i].color[BLUE]*dp1;
		col[RED]+=r->Kd[RED]*r->lights[i].color[RED]*dp2;
		col[GREEN]+=r->Kd[GREEN]*r->lights[i].color[GREEN]*dp2;
		col[BLUE]+=r->Kd[BLUE]*r->lights[i].color[BLUE]*dp2; 
	}
	red=ctoi(col[RED]);
	green=ctoi(col[GREEN]);
	blue=ctoi(col[BLUE]);
}
