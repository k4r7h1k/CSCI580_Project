/*   CS580 HW   */
#include    "stdafx.h"  
#include	"Gz.h"
#include	"disp.h"


int GzNewFrameBuffer(char** framebuffer, int width, int height)
{
/* create a framebuffer:
 -- allocate memory for framebuffer : (sizeof)GzPixel x width x height
 -- pass back pointer 
 -- NOTE: this function is optional and not part of the API, but you may want to use it within the display function.
*/
	if(width>MAXXRES || height >MAXYRES )	//Checking boundary conditions
		return GZ_FAILURE;
	*framebuffer=new char[3*width*height];	//Initializing  frame buffer
	if(*framebuffer==NULL)
		return GZ_FAILURE;
	return GZ_SUCCESS;
}

int GzNewDisplay(GzDisplay	**display, int xRes, int yRes)
{
/* create a display:
  -- allocate memory for indicated resolution
  -- pass back pointer to GzDisplay object in display
*/
	if(xRes>MAXXRES || yRes >MAXYRES ) //Boundary Conditions
		return GZ_FAILURE;
	*display=new GzDisplay;
	(**display).xres=xRes;
	(**display).yres=yRes;
	(**display).fbuf=new GzPixel[xRes*yRes]; //Display Frame Buffer
	if(*display==NULL)
		return GZ_FAILURE;
	return GZ_SUCCESS;
}


int GzFreeDisplay(GzDisplay	*display)
{
/* clean up, free memory */
	if(display==NULL)
		return GZ_FAILURE;
	delete (*display).fbuf;//Deallocating Display buffer
	delete display;//Deallocating Display pointer
	display=NULL;
	return GZ_SUCCESS;
}



int GzGetDisplayParams(GzDisplay *display, int *xRes, int *yRes)
{
/* pass back values for a display */
	if(display==NULL)
		return GZ_FAILURE;
	*xRes=(*display).xres;
	*yRes=(*display).yres;
	return GZ_SUCCESS;
}
int GzInitBGDisplay(GzDisplay	*display,int r,int g, int b)
{
/* set everything to some default values - start a new frame */
	int x,y;
	x=(*display).xres;
	y=(*display).yres;
	for(int j=0;j<y;j=j+1)
		for(int i=0;i<x;i=i+1)
		{	
			display->fbuf[j*x+i].red=(r<<4); // Right shifting initial values while storing into display buffer
			display->fbuf[j*x+i].blue=(b<<4);
			display->fbuf[j*x+i].green=(g<<4);
			display->fbuf[j*x+i].z=INT_MAX;
			display->fbuf[j*x+i].alpha=1;
		}
	return GZ_SUCCESS;
}

int GzInitDisplay(GzDisplay	*display)
{
/* set everything to some default values - start a new frame */
	GzInitBGDisplay(display,128,112,96); return GZ_SUCCESS; //toggle this line to remove bg color
	int x,y;
	x=(*display).xres;
	y=(*display).yres;
	for(int j=0;j<y;j=j+1)
		for(int i=0;i<x;i=i+1)
		{	
			//initializing to zero (black background)
			display->fbuf[j*x+i].red=display->fbuf[j*x+i].blue=display->fbuf[j*x+i].green=0;
			display->fbuf[j*x+i].z=INT_MAX;
			display->fbuf[j*x+i].alpha=1;
		}
	return GZ_SUCCESS;
}


void intensityCorrection(GzIntensity &a){
if(a<0)a=0;
if(a>4095)a=4095;
//a=a>>4; // Bit correction takes place when flushing the data.
}
int GzPutDisplay(GzDisplay *display, int i, int j, GzIntensity r, GzIntensity g, GzIntensity b, GzIntensity a, GzDepth z)
{
/* write pixel values into the display */
	if(display==NULL)
		return GZ_FAILURE;
	if(i<0 || i>=display->xres || j<0 || j>=display->yres)
		return GZ_SUCCESS;
	if(display->fbuf[(j*display->xres)+i].z<z)
		return GZ_SUCCESS;
	
	intensityCorrection(r);
	intensityCorrection(g);
	intensityCorrection(b);
	display->fbuf[(j*display->xres)+i].red=r;
	display->fbuf[(j*display->xres)+i].blue=b;
	display->fbuf[(j*display->xres)+i].green=g;
	display->fbuf[(j*display->xres)+i].z=z;
	display->fbuf[(j*display->xres)+i].alpha=a;
	return GZ_SUCCESS;
}


int GzGetDisplay(GzDisplay *display, int i, int j, GzIntensity *r, GzIntensity *g, GzIntensity *b, GzIntensity *a, GzDepth *z)
{
	/* pass back pixel value in the display */
	if(i<0 || i>display->xres || j<0 || j>display->yres || display==NULL)
		return GZ_FAILURE;
	*r=display->fbuf[(j*display->xres)+i].red;
	*b=display->fbuf[(j*display->xres)+i].blue;
	*g=display->fbuf[(j*display->xres)+i].green;
	*z=display->fbuf[(j*display->xres)+i].z;
	*a=display->fbuf[(j*display->xres)+i].alpha;
	return GZ_SUCCESS;
}


int GzFlushDisplay2File(FILE* outfile, GzDisplay *display)
{

	/* write pixels to ppm file -- "P6 %d %d 255\r" */

	if(outfile==NULL || display==NULL)
		return GZ_FAILURE;
	fprintf(outfile,"P6 %d %d 255\r",display->xres,display->yres);
	int x,y;
	x=(*display).xres;
	y=(*display).yres;
	for(int j=0;j<y;j=j+1)
		for(int i=0;i<x;i=i+1)
		{
			fputc(display->fbuf[(j*x)+i].red>>4,outfile);    // >>4 implies Bit correction 
			fputc(display->fbuf[(j*x)+i].green>>4,outfile);
			fputc(display->fbuf[(j*x)+i].blue>>4,outfile);
		}
	
	return GZ_SUCCESS;
}

int GzFlushDisplay2FrameBuffer(char* framebuffer, GzDisplay *display)
{

	/* write pixels to framebuffer: 
		- Put the pixels into the frame buffer
		- Caution: store the pixel to the frame buffer as the order of blue, green, and red 
		- Not red, green, and blue !!!
	*/
	if(framebuffer==NULL || display==NULL)
		return GZ_FAILURE;
		int x,y;
		x=(*display).xres;
		y=(*display).yres;
		for(int j=0;j<y;j=j+1)
		for(int i=0;i<x;i=i+1)
		{
			int d=(j*x)+i;
			int e=d*3;
			*(framebuffer+e)=(char)(display->fbuf[d].blue>>4);	// >>4 implies Bit correction 
			*(framebuffer+e+1)=(char)(display->fbuf[d].green>>4);
			*(framebuffer+e+2)=(char)(display->fbuf[d].red>>4);
		}
	return GZ_SUCCESS;
}