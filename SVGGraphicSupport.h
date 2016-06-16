/*
	ivoc - ivoc@ipfn.ist.utl.pt
	
	Description: this set of functions can be used in sequence (or in separate) to draw an automatic SVG Graphic given any x/y set of point in two separate vectors
	first use DualVectorSort to sort the values, then use RemoveRepeatedValues to remove the repeated points. 
	After this use OptimalDisplayFromVectors for obtaining the optimal zoom parameters for GetGraphSVG, wich writes a FString containing SVG code for the grafic
	reminder: there is no < and > at the begining and by the end on the code becaus it is intended to be used in a ::ProcessHttpMessage(HttpStream &hStream) with hmStream.SSPrintf(HtmlTagStreamMode, temp_string_1.Buffer()); that contains the <> tags
*/

#if !defined (SVGGRAPHICSUPPORT_H)
#define SVGGRAPHICSUPPORT_H

#include "Level0.h"
#include "Level1.h"
#include "Level2.h"
//#include "FString.h"

class SVGGraphicSupport {

public:	
	
	SVGGraphicSupport();
	
	~SVGGraphicSupport();
	
	bool GetGraphSVG(FString * SVG_FString,char instance_name[],int vector_dim,float * vector_x,float * vector_y, int width,int height,float x_min,float y_min,float x_max,float y_max,float major_tick_x,float major_tick_y,int number_of_sub_ticks_x,int number_of_sub_ticks_y,bool legend_on_major_tick_x,bool legend_on_major_tick_y); 

	bool HeaderSVG(FString * HeaderFString,char title[], int SVG_height,int image_x,int image_y,int image_width,int image_height,char image_link[],int text_x, int text_y, char text[], char bg_colour[]); 

	bool FooterSVG(FString * FooterFString,int SVG_height,int image_x,int image_y,int image_width,int image_height,char image_link[],int n_images,int text_x1,int text_x2,char gam_name[],char version[],char lastdate[],char author[],char description[]); 

	bool OptimalDisplayFromVectors(int vector_dim, float * vector_x, float * vector_y,float * x_min,float * y_min,float * x_max,float * y_max,float * major_tick_x,float * major_tick_y ,int * number_of_sub_ticks_x,int * number_of_sub_ticks_y); 
	
	bool HorizontalBarSVG ( FString * SVGFString);
	
	bool TrianglePlusBarSVG(FString * SVGFString, float triangle_position);

};

#endif
