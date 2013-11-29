/*$T \Quaternion.h GC 1.150 2013-11-28 23:29:17 */


/*$6*/


#ifndef _Quaternion_H_
#define _Quaternion_H_
#include <math.h>
#include "Gz.h"

float SIGN(float x) {return (x >= 0.0f) ? +1.0f : -1.0f;}
float NORM(float a, float b, float c, float d) {return sqrt(a * a + b * b + c * c + d * d);}

typedef float	GzQuaternion[4];
typedef float	GzDualQuaternion[2][4];
/* */

void Quat2DualQuat(GzQuaternion q, GzCoord t, GzDualQuaternion dQ)
{
	for(int i = 0; i < 4; i++) dQ[0][i] = q[i];

	// dual part:
	dQ[1][0] = -0.5 * (t[0] * q[1] + t[1] * q[2] + t[2] * q[3]);
	dQ[1][1] = 0.5 * (t[0] * q[0] + t[1] * q[3] - t[2] * q[2]);
	dQ[1][2] = 0.5 * (-t[0] * q[3] + t[1] * q[0] + t[2] * q[1]);
	dQ[1][3] = 0.5 * (t[0] * q[2] - t[1] * q[1] + t[2] * q[0]);
}

/* */
void scalarDualQuaternion(float s,GzDualQuaternion q,GzDualQuaternion r){
	for(int i=0;i<4;i++)
	{
		r[0][i]=q[0][i]*s;
		r[0][i]=q[1][i]*s;
	}
}
void addDualQuaternion(GzDualQuaternion q1,GzDualQuaternion q2,GzDualQuaternion result){
	for(int i=0;i<4;i++)
	{
		result[0][i]=q1[0][i]+q2[0][i];
		result[1][i]=q1[1][i]+q2[1][i];
	}
}
void addQuaternion(GzQuaternion q1,GzQuaternion q2,GzQuaternion result){
	for(int i=0;i<4;i++)
	{
		result[i]=q1[i]+q2[i];
	}
}
void multiplyQuaternion(GzQuaternion q1,GzQuaternion q2,GzQuaternion result){
	result[0]=(q1[0]*q2[0])-(q1[1]*q2[1]+q1[2]*q2[2]+q1[3]*q2[3]);
	result[1]=q1[0]*q2[1];
	result[2]=q1[0]*q2[2];
	result[3]=q1[0]*q2[3];
	result[1]+=q2[0]*q1[1];
	result[2]+=q2[0]*q1[2];
	result[3]+=q2[0]*q1[3];
	result[1]+=q1[2]*q2[3]-q1[3]*q2[2];
	result[2]+=q1[3]*q2[1]-q1[1]*q2[3];
	result[3]+=q1[1]*q2[2]-q1[2]*q2[1];
}
void conjugateQuaternion(GzQuaternion q1,GzQuaternion r){
	r[0]=q1[0];
	r[1]=q1[1]*-1;
	r[2]=q1[2]*-1;
	r[3]=q1[3]*-1;
}
float magnitudeQuaternion(GzQuaternion q){
	return q[0]*q[0]+q[1]*q[1]+q[2]*q[2]+q[3]*q[3];
}
void conjugateDualQuaternion(GzDualQuaternion q1,GzDualQuaternion r){
	conjugateQuaternion(q1[0],r[0]);
	conjugateQuaternion(q1[1],r[1]);
}
void multiplyDualQuaternion(GzDualQuaternion q1,GzDualQuaternion q2,GzDualQuaternion result){
	GzQuaternion r,r1,r2;
	multiplyQuaternion(q1[0],q2[0],r);
	result[0][0]=r[0];
	result[0][1]=r[1];
	result[0][2]=r[2];
	result[0][3]=r[3];
	multiplyQuaternion(q1[0],q2[1],r);
	multiplyQuaternion(q1[1],q2[0],r1);
	addQuaternion(r,r1,r2);
	result[1][0]=r2[0];
	result[1][1]=r2[1];
	result[1][2]=r2[2];
	result[1][3]=r2[3];
}
void dualQuaternionTransform(GzDualQuaternion *q, GzCoord v, GzCoord n, GzCoord mv, GzCoord mn, float *weights,int numberOfBones)
{
	GzDualQuaternion b={0,0,0,0,0,0,0,0};
	for(int i=0;i<numberOfBones;i++){
		GzDualQuaternion r;
		scalarDualQuaternion(weights[i],q[i],r);
		b[0][0]+=r[0][0];b[0][1]+=r[0][1];b[0][2]+=r[0][2];b[0][3]+=r[0][3];
		b[1][0]+=r[1][0];b[1][1]+=r[1][1];b[1][2]+=r[1][2];b[1][3]+=r[1][3];
	}
	float magB=magnitudeQuaternion(b[0]);
	GzDualQuaternion c;
	scalarDualQuaternion(1/magB,b,c);
	float t0=2*(-c[1][0]*c[0][1]+c[1][1]*c[0][0]-c[1][2]*c[0][3]+c[1][3]*c[0][2]);
	float t1=2*(-c[1][0]*c[0][2]+c[1][1]*c[0][3]+c[1][2]*c[0][0]-c[1][3]*c[0][1]);
	float t2=2*(-c[1][0]*c[0][3]-c[1][1]*c[0][2]+c[1][2]*c[0][1]+c[1][3]*c[0][0]);
	float m[3][4];
	m[0][0]=1-(2*c[0][2]*c[0][2])-(2*c[0][3]*c[0][3]);
	m[0][1]=(2*c[0][1]*c[0][2])-(2*c[0][0]*c[0][3]);
	m[0][2]=(2*c[0][1]*c[0][3])+(2*c[0][0]*c[0][2]);
	m[0][3]=t0;
	m[1][0]=(2*c[0][1]*c[0][2])+(2*c[0][0]*c[0][3]);
	m[1][1]=1-(2*c[0][1]*c[0][1])-(2*c[0][3]*c[0][3]);
	m[1][2]=(2*c[0][2]*c[0][3])-(2*c[0][0]*c[0][1]);
	m[1][3]=t1;
	m[2][0]=(2*c[0][1]*c[0][3])-(2*c[0][0]*c[0][2]);
	m[2][1]=(2*c[0][2]*c[0][3])-(2*c[0][0]*c[0][1]);
	m[2][2]=1-(2*c[0][1]*c[0][1])-(2*c[0][2]*c[0][2]);
	m[2][3]=t2;
	for(int i=0;i<3;i++)
	{
		for(int j=0;j<1;j++)
		{
			mv[i]=0;
			mn[i]=0;
			for(int k=0;k<4;k++)
			{
				if(k!=3)
				{mv[i]+=m[i][k]*v[k];
				mn[i]+=m[i][k]*v[k];}
				else{mv[i]+=m[i][k];
				}
			}
		}
	}
}

/* */
void convertRotation2Quaternion(GzMatrix r, GzQuaternion q)
{
	q[0] = (r[0][0] + r[1][1] + r[2][2] + 1.0f) / 4.0f;
	q[1] = (r[0][0] - r[1][1] - r[2][2] + 1.0f) / 4.0f;
	q[2] = (-r[0][0] + r[1][1] - r[2][2] + 1.0f) / 4.0f;
	q[3] = (-r[0][0] - r[1][1] + r[2][2] + 1.0f) / 4.0f;
	if(q[0] < 0.0f) q[0] = 0.0f;
	if(q[1] < 0.0f) q[1] = 0.0f;
	if(q[2] < 0.0f) q[2] = 0.0f;
	if(q[3] < 0.0f) q[3] = 0.0f;
	q[0] = sqrt(q[0]);
	q[1] = sqrt(q[1]);
	q[2] = sqrt(q[2]);
	q[3] = sqrt(q[3]);
	
if(q[0] >= q[1] && q[0] >= q[2] && q[0] >= q[3]) {
    q[0] *= +1.0f;
    q[1] *= SIGN(r[2][1] - r[1][2]);
    q[2] *= SIGN(r[0][2] - r[2][0]);
    q[3] *= SIGN(r[1][0] - r[0][1]);
} else if(q[1] >= q[0] && q[1] >= q[2] && q[1] >= q[3]) {
    q[0] *= SIGN(r[2][1] - r[1][2]);
    q[1] *= +1.0f;
    q[2] *= SIGN(r[1][0] + r[0][1]);
    q[3] *= SIGN(r[0][2] + r[2][0]);
} else if(q[2] >= q[0] && q[2] >= q[1] && q[2] >= q[3]) {
    q[0] *= SIGN(r[0][2] - r[2][0]);
    q[1] *= SIGN(r[1][0] + r[0][1]);
    q[2] *= +1.0f;
    q[3] *= SIGN(r[2][1] + r[1][2]);
} else if(q[3] >= q[0] && q[3] >= q[1] && q[3] >= q[2]) {
    q[0] *= SIGN(r[1][0] - r[0][1]);
    q[1] *= SIGN(r[2][0] + r[0][2]);
    q[2] *= SIGN(r[2][1] + r[1][2]);
    q[3] *= +1.0f;
} else {
    //cout<<"coding error\n");
}
float r1 = NORM(q[0], q[1], q[2], q[3]);
q[0] /= r1;
q[1] /= r1;
q[2] /= r1;
q[3] /= r1;
}
#endif