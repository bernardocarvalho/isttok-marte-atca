/*
	ivoc - ivoc@ipfn.ist.utl.pt
	
	Description: this set of functions can be used in sequence (or in separate) to draw an automatic SVG Graphic given any x/y set of point in two separate vectors
	first use DualVectorSort to sort the values, then use RemoveRepeatedValues to remove the repeated points. 
	After this use OptimalDisplayFromVectors for obtaining the optimal zoom parameters for GetGraphSVG, wich writes a FString containing SVG code for the grafic
	reminder: there is no < and > at the begining and by the end on the code becaus it is intended to be used in a ::ProcessHttpMessage(HttpStream &hStream) with hmStream.SSPrintf(HtmlTagStreamMode, temp_string_1.Buffer()); that contains the <> tags
*/
#include "SVGGraphicSupport.h"

//	SVG graph properties
#define MAJOR_TICK_LIMIT_NUMBER 20
#define MINOR_TICK_LIMIT_NUMBER 10
#define MINIMUM_WIDTH 100
#define MINIMUM_HEIGHT 100
#define MINIMUM_VECTOR_DIM 2

SVGGraphicSupport::SVGGraphicSupport(){
	
}
SVGGraphicSupport::~SVGGraphicSupport(){
	
}


bool SVGGraphicSupport::GetGraphSVG(FString * SVG_FString,char instance_name[],int vector_dim,float * vector_x,float * vector_y, int width,int height,float x_min,float y_min,float x_max,float y_max,float major_tick_x,float major_tick_y,int number_of_sub_ticks_x,int number_of_sub_ticks_y,bool legend_on_major_tick_x,bool legend_on_major_tick_y) {

	
	if(vector_dim < MINIMUM_VECTOR_DIM) {
		CStaticAssertErrorCondition(InitialisationError,"SVGGraphicSupport.h:: %s !!! GetGraphSVG -> vector_dim < MINIMUM_VECTOR_DIM, vector_dim =%d",instance_name,vector_dim);
		return False;
	}
	if(x_min >= x_max) {
			CStaticAssertErrorCondition(InitialisationError,"SVGGraphicSupport.h:: %s !!! GetGraphSVG -> x_min >= x_max",instance_name);
		return False;
	}
	if(y_min >= y_max) {
			CStaticAssertErrorCondition(InitialisationError,"SVGGraphicSupport.h:: %s !!! GetGraphSVG -> y_min >= y_max",instance_name);
		return False;
	}

	float number_of_major_ticks_x = (x_max - x_min)/major_tick_x;
	float number_of_major_ticks_y = (y_max - y_min)/major_tick_y;

	if(number_of_major_ticks_x > MAJOR_TICK_LIMIT_NUMBER) {
			CStaticAssertErrorCondition(InitialisationError,"SVGGraphicSupport.h:: %s !!! GetGraphSVG -> major_tick_x > MAJOR_TICK_LIMIT_NUMBER, number_of_major_ticks_x = %f",instance_name,number_of_major_ticks_x);
		return False;
	}
	if(number_of_major_ticks_y > MAJOR_TICK_LIMIT_NUMBER) {
			CStaticAssertErrorCondition(InitialisationError,"SVGGraphicSupport.h:: %s !!! GetGraphSVG -> major_tick_y > MAJOR_TICK_LIMIT_NUMBER,number_of_major_ticks_y = %f",instance_name,number_of_major_ticks_y);
		return False;
	}
	if(number_of_sub_ticks_x > MINOR_TICK_LIMIT_NUMBER) {
			CStaticAssertErrorCondition(InitialisationError,"SVGGraphicSupport.h:: %s !!! GetGraphSVG -> number_of_sub_ticks_x > MINOR_TICK_LIMIT_NUMBER, number_of_sub_ticks_x = %d",instance_name,number_of_sub_ticks_x);
		return False;
	}
	if(number_of_sub_ticks_y > MINOR_TICK_LIMIT_NUMBER) {
			CStaticAssertErrorCondition(InitialisationError,"SVGGraphicSupport.h:: %s !!! GetGraphSVG -> number_of_sub_ticks_y > MINOR_TICK_LIMIT_NUMBER, number_of_sub_ticks_y = %d",instance_name,number_of_sub_ticks_y);
		return False;
	}
	if(width < MINIMUM_WIDTH && width != -1) {
			CStaticAssertErrorCondition(InitialisationError,"SVGGraphicSupport.h:: %s !!! GetGraphSVG -> width < MINIMUM_WIDTH || width != -1, width = %d",instance_name, width);
		return False;
	}
	if(height < MINIMUM_HEIGHT) {
			CStaticAssertErrorCondition(InitialisationError,"SVGGraphicSupport.h:: %s !!! GetGraphSVG -> height < MINIMUM_HEIGHT, height = %d",instance_name,height);
		return False;
	}

	FString temp_string_2;
	FString buffer_fstring;

	float temp_cursor;
	float small_tick;
	float position;
	int j;
	int k;

	float pos_x;
	float pos_xx;
	float pos_y;
	float pos_yy;

	if (width == -1) {
		// width = 100%	-> html code for % is &#37;


		temp_string_2.Printf("svg width=\"100&#37;\" height=\"%d\">\n", height);

		//	if not  x_min <= 0 <= x_max
		if( x_min*x_max > 0) {

			temp_cursor = x_min;		//first grey line to be drawn
			small_tick = major_tick_x / (number_of_sub_ticks_x+1);
			while (temp_cursor < x_max) {

				position = 2+((temp_cursor-x_min)*((100-2)/(x_max-x_min)));
				buffer_fstring.Printf("<line x1=\"%f&#37;\" y1=\"0\" x2=\"%f&#37;\" y2=\"%d\" style=\"stroke:rgb(150,150,150);stroke-width:1\"/>\n", position, position, (height-10));

				//	text with the major ticks legend
				if(legend_on_major_tick_x) {
					buffer_fstring.Printf("<text x=\"%f&#37;\"  y=\"%d\" style=\"font-family:Arial;font-size:10\">%.2f</text>\n",position, (height-1),temp_cursor);
				}
				for (k=0; k < number_of_sub_ticks_x; k++) {
					position = 2+((temp_cursor+(k+1)*small_tick-x_min)*((100-2)/(x_max-x_min)));
					buffer_fstring.Printf("<line x1=\"%f&#37;\" y1=\"0\" x2=\"%f&#37;\" y2=\"%d\" style=\"stroke:rgb(220,220,220);stroke-width:1\"/>\n", position, position, (height-10));
				}
				temp_cursor += major_tick_x;
			}
		} else {

			temp_cursor = 0;		//first grey line to be drawn
			small_tick = major_tick_x / (number_of_sub_ticks_x+1);
			while (temp_cursor < x_max) {

				position = 2+((temp_cursor-x_min)*((100-2)/(x_max-x_min)));
				buffer_fstring.Printf("<line x1=\"%f&#37;\" y1=\"0\" x2=\"%f&#37;\" y2=\"%d\" style=\"stroke:rgb(150,150,150);stroke-width:1\"/>\n", position, position, (height-10));

				//	text with the major ticks legend
				if(legend_on_major_tick_x) {
					buffer_fstring.Printf("<text x=\"%f&#37;\"  y=\"%d\" style=\"font-family:Arial;font-size:10\">%.2f</text>\n",position, (height-1),temp_cursor);
				}
				for (k=0; k < number_of_sub_ticks_x; k++) {
					position = 2+((temp_cursor+(k+1)*small_tick-x_min)*((100-2)/(x_max-x_min)));
					buffer_fstring.Printf("<line x1=\"%f&#37;\" y1=\"0\" x2=\"%f&#37;\" y2=\"%d\" style=\"stroke:rgb(220,220,220);stroke-width:1\"/>\n", position, position, (height-10));
				}
				temp_cursor += major_tick_x;
			}
			temp_cursor = 0;
			while (temp_cursor > x_min) {

				position = 2+((temp_cursor-x_min)*((100-2)/(x_max-x_min)));
				buffer_fstring.Printf("<line x1=\"%f&#37;\" y1=\"0\" x2=\"%f&#37;\" y2=\"%d\" style=\"stroke:rgb(150,150,150);stroke-width:1\"/>\n", position, position, (height-10));

				//	text with the major ticks legend
				if(legend_on_major_tick_x) {
					buffer_fstring.Printf("<text x=\"%f&#37;\"  y=\"%d\" style=\"font-family:Arial;font-size:10\">%.2f</text>\n",position, (height-1),temp_cursor);
				}
				for (k=0; k < number_of_sub_ticks_x; k++) {
					position = 2+((temp_cursor-(k+1)*small_tick-x_min)*((100-2)/(x_max-x_min)));
					if ((temp_cursor-(k+1)*small_tick) > x_min) {
						buffer_fstring.Printf("<line x1=\"%f&#37;\" y1=\"0\" x2=\"%f&#37;\" y2=\"%d\" style=\"stroke:rgb(220,220,220);stroke-width:1\"/>\n", position, position, (height-10));
					}
				}
				temp_cursor -= major_tick_x;
			}
			//dark xx axes
			temp_cursor = 0;
			position = 2+((temp_cursor-x_min)*((100-2)/(x_max-x_min)));
			buffer_fstring.Printf("<line x1=\"%f&#37;\" y1=\"0\" x2=\"%f&#37;\" y2=\"%d\" style=\"stroke:rgb(0,0,100);stroke-width:1\"/>\n", position, position, (height-10));

		}


		//	if not  y_min <= 0 <= y_max
		if( y_min*y_max > 0) {

			temp_cursor = y_min;		//first grey line to be drawn
			small_tick = major_tick_y / (number_of_sub_ticks_y+1);
			while (temp_cursor < y_max) {

				position = (y_max-temp_cursor)*((height-10)/(y_max-y_min));
				buffer_fstring.Printf("<line x1=\"2&#37;\" y1=\"%f\" x2=\"100&#37;\" y2=\"%f\" style=\"stroke:rgb(150,150,150);stroke-width:1\"/>\n", position, position);

				if(legend_on_major_tick_y) {
					//	text with the major ticks legend
					buffer_fstring.Printf("<text x=\"%f\"  y=\"0\" transform=\"rotate(90 0,0)\" style=\"font-family:Arial;font-size:10\">%.1f</text>\n",(position-4),temp_cursor);
				}
				for (k=0; k < number_of_sub_ticks_x; k++) {
					position = (y_max-temp_cursor-(k+1)*small_tick)*((height-10)/(y_max-y_min));
					buffer_fstring.Printf("<line x1=\"2&#37;\" y1=\"%f\" x2=\"100&#37;\" y2=\"%f\" style=\"stroke:rgb(220,220,220);stroke-width:1\"/>\n", position, position);
				}
				temp_cursor += major_tick_y;
			}
		} else {
			temp_cursor = 0;		//first grey line to be drawn
			small_tick = major_tick_y / (number_of_sub_ticks_y+1);
			while (temp_cursor < y_max) {

				position = (y_max-temp_cursor)*((height-10)/(y_max-y_min));
				buffer_fstring.Printf("<line x1=\"2&#37;\" y1=\"%f\" x2=\"100&#37;\" y2=\"%f\" style=\"stroke:rgb(150,150,150);stroke-width:1\"/>\n", position, position);

				if(legend_on_major_tick_y) {
					//	text with the major ticks legend
					buffer_fstring.Printf("<text x=\"%f\"  y=\"0\" transform=\"rotate(90 0,0)\" style=\"font-family:Arial;font-size:10\">%.1f</text>\n",(position-4),temp_cursor);
				}
				for (k=0; k < number_of_sub_ticks_x; k++) {
					position = (y_max-temp_cursor-(k+1)*small_tick)*((height-10)/(y_max-y_min));
					buffer_fstring.Printf("<line x1=\"2&#37;\" y1=\"%f\" x2=\"100&#37;\" y2=\"%f\" style=\"stroke:rgb(220,220,220);stroke-width:1\"/>\n", position, position);
				}
				temp_cursor += major_tick_y;
			}

			temp_cursor = 0;
			while (temp_cursor > y_min) {

				position = (y_max-temp_cursor)*((height-10)/(y_max-y_min));
				buffer_fstring.Printf("<line x1=\"2&#37;\" y1=\"%f\" x2=\"100&#37;\" y2=\"%f\" style=\"stroke:rgb(150,150,150);stroke-width:1\"/>\n", position, position);

				if(legend_on_major_tick_y) {
					//	text with the major ticks legend
					buffer_fstring.Printf("<text x=\"%f\"  y=\"0\" transform=\"rotate(90 0,0)\" style=\"font-family:Arial;font-size:10\">%.1f</text>\n",(position-4),temp_cursor);
				}
				for (k=0; k < number_of_sub_ticks_x; k++) {
					position = (y_max-temp_cursor+(k+1)*small_tick)*((height-10)/(y_max-y_min));
					if((temp_cursor-(k+1)*small_tick) > y_min) {
						buffer_fstring.Printf("<line x1=\"2&#37;\" y1=\"%f\" x2=\"100&#37;\" y2=\"%f\" style=\"stroke:rgb(220,220,220);stroke-width:1\"/>\n", position, position);
					}
				}
				temp_cursor -= major_tick_y;
			}
			//	dark yy axes
			temp_cursor = 0;
			position = (y_max-temp_cursor)*((height-10)/(y_max-y_min));
			buffer_fstring.Printf("<line x1=\"2&#37;\" y1=\"%f\" x2=\"100&#37;\" y2=\"%f\" style=\"stroke:rgb(0,0,100);stroke-width:1\"/>\n", position, position);
		}

		pos_x = 2+(((*(vector_x))-x_min)*((100-2)/(x_max-x_min)));
		pos_y =	(y_max-(*(vector_y)))*((height-10)/(y_max-y_min));

		//	Now draw the waveform
		for (j=1; j < (vector_dim); j++) {

			pos_xx = 2+(((*(vector_x + j))-x_min)*((100-2)/(x_max-x_min)));
			pos_yy = (y_max-(*(vector_y + j)))*((height-10)/(y_max-y_min));
			buffer_fstring.Printf("<line x1=\"%f&#37;\" y1=\"%f\" x2=\"%f&#37;\" y2=\"%f\" style=\"stroke:rgb(255,0,0);stroke-width:2\"/>\n",pos_x,pos_y,pos_xx,pos_yy);
			pos_x = pos_xx;
			pos_y = pos_yy;
		}
		//	Draw the outter part of the graph
		buffer_fstring.Printf("<line x1=\"2&#37;\" y1=\"1\" x2=\"100&#37;\" y2=\"1\" style=\"stroke:rgb(0,0,0);stroke-width:2\"/>\n\
			<line x1=\"2&#37;\" y1=\"%d\" x2=\"100&#37;\" y2=\"%d\" style=\"stroke:rgb(0,0,0);stroke-width:2\"/>\n\
			<line x1=\"2&#37;\" y1=\"0\" x2=\"2&#37;\" y2=\"%d\" style=\"stroke:rgb(0,0,0);stroke-width:2\"/>\n\
			<line x1=\"100&#37;\" y1=\"0;\" x2=\"100&#37;\" y2=\"%d\" style=\"stroke:rgb(0,0,0);stroke-width:3\"/>",(height-10),(height-10),(height-10),(height-10));
		temp_string_2 += buffer_fstring.Buffer();

		temp_string_2 += "</svg";
	} 
	else {

		temp_string_2.Printf("svg width=\"%d\" height=\"%d\">\n",width, height);

		//	if not  x_min <= 0 <= x_max
		if( x_min*x_max > 0) {

			temp_cursor = x_min;		//first grey line to be drawn
			small_tick = major_tick_x / (number_of_sub_ticks_x+1);
			while (temp_cursor < x_max) {

				position = 10+((temp_cursor-x_min)*((width-10)/(x_max-x_min)));
				buffer_fstring.Printf("<line x1=\"%f\" y1=\"0\" x2=\"%f\" y2=\"%d\" style=\"stroke:rgb(150,150,150);stroke-width:1\"/>\n", position, position, (height-10));

				//	text with the major ticks legend
				if(legend_on_major_tick_x) {
					buffer_fstring.Printf("<text x=\"%f\"  y=\"%d\" style=\"font-family:Arial;font-size:10\">%.2f</text>\n",position, (height-1),temp_cursor);
				}
				for (k=0; k < number_of_sub_ticks_x; k++) {
					position = 10+((temp_cursor+(k+1)*small_tick-x_min)*((width-10)/(x_max-x_min)));
					buffer_fstring.Printf("<line x1=\"%f\" y1=\"0\" x2=\"%f\" y2=\"%d\" style=\"stroke:rgb(220,220,220);stroke-width:1\"/>\n", position, position, (height-10));
				}
				temp_cursor += major_tick_x;
			}
		} 
		else {

			temp_cursor = 0;		//first grey line to be drawn
			small_tick = major_tick_x / (number_of_sub_ticks_x+1);
			while (temp_cursor < x_max) {

				position = 10+((temp_cursor-x_min)*((width-10)/(x_max-x_min)));
				buffer_fstring.Printf("<line x1=\"%f\" y1=\"0\" x2=\"%f\" y2=\"%d\" style=\"stroke:rgb(150,150,150);stroke-width:1\"/>\n", position, position, (height-10));

				//	text with the major ticks legend
				if(legend_on_major_tick_x) {
					buffer_fstring.Printf("<text x=\"%f\"  y=\"%d\" style=\"font-family:Arial;font-size:10\">%.2f</text>\n",position, (height-1),temp_cursor);
				}
				for (k=0; k < number_of_sub_ticks_x; k++) {
					position = 10+((temp_cursor+(k+1)*small_tick-x_min)*((width-10)/(x_max-x_min)));
					buffer_fstring.Printf("<line x1=\"%f\" y1=\"0\" x2=\"%f\" y2=\"%d\" style=\"stroke:rgb(220,220,220);stroke-width:1\"/>\n", position, position, (height-10));
				}
				temp_cursor += major_tick_x;
			}
			temp_cursor = 0;
			while (temp_cursor > x_min) {

				position = 10+((temp_cursor-x_min)*((width-10)/(x_max-x_min)));
				buffer_fstring.Printf("<line x1=\"%f\" y1=\"0\" x2=\"%f\" y2=\"%d\" style=\"stroke:rgb(150,150,150);stroke-width:1\"/>\n", position, position, (height-10));

				//	text with the major ticks legend
				if(legend_on_major_tick_x) {
					buffer_fstring.Printf("<text x=\"%f\"  y=\"%d\" style=\"font-family:Arial;font-size:10\">%.2f</text>\n",position, (height-1),temp_cursor);
				}
				for (k=0; k < number_of_sub_ticks_x; k++) {
					position = 10+((temp_cursor-(k+1)*small_tick-x_min)*((width-10)/(x_max-x_min)));
					if ((temp_cursor-(k+1)*small_tick) > x_min) {
						buffer_fstring.Printf("<line x1=\"%f\" y1=\"0\" x2=\"%f\" y2=\"%d\" style=\"stroke:rgb(220,220,220);stroke-width:1\"/>\n", position, position, (height-10));
					}
				}
				temp_cursor -= major_tick_x;
			}
			//dark xx axes
			temp_cursor = 0;
			position = 10+((temp_cursor-x_min)*((width-10)/(x_max-x_min)));
			buffer_fstring.Printf("<line x1=\"%f\" y1=\"0\" x2=\"%f\" y2=\"%d\" style=\"stroke:rgb(0,0,100);stroke-width:1\"/>\n", position, position, (height-10));

		}


		//	if not  y_min <= 0 <= y_max
		if( y_min*y_max > 0) {

			temp_cursor = y_min;		//first grey line to be drawn
			small_tick = major_tick_y / (number_of_sub_ticks_y+1);
			while (temp_cursor < y_max) {

				position = (y_max-temp_cursor)*((height-10)/(y_max-y_min));
				buffer_fstring.Printf("<line x1=\"10\" y1=\"%f\" x2=\"%d\" y2=\"%f\" style=\"stroke:rgb(150,150,150);stroke-width:1\"/>\n", position, width, position);

				if(legend_on_major_tick_y) {
					//	text with the major ticks legend
					buffer_fstring.Printf("<text x=\"%f\"  y=\"0\" transform=\"rotate(90 0,0)\" style=\"font-family:Arial;font-size:10\">%.1f</text>\n",(position-4),temp_cursor);
				}
				for (k=0; k < number_of_sub_ticks_x; k++) {
					position = (y_max-temp_cursor-(k+1)*small_tick)*((height-10)/(y_max-y_min));
					buffer_fstring.Printf("<line x1=\"10\" y1=\"%f\" x2=\"%d\" y2=\"%f\" style=\"stroke:rgb(220,220,220);stroke-width:1\"/>\n", position, width, position);
				}
				temp_cursor += major_tick_y;
			}
		} 
		else {
			temp_cursor = 0;		//first grey line to be drawn
			small_tick = major_tick_y / (number_of_sub_ticks_y+1);
			while (temp_cursor < y_max) {

				position = (y_max-temp_cursor)*((height-10)/(y_max-y_min));
				buffer_fstring.Printf("<line x1=\"10\" y1=\"%f\" x2=\"%d\" y2=\"%f\" style=\"stroke:rgb(150,150,150);stroke-width:1\"/>\n", position, width, position);

				if(legend_on_major_tick_y) {
					//	text with the major ticks legend
					buffer_fstring.Printf("<text x=\"%f\"  y=\"0\" transform=\"rotate(90 0,0)\" style=\"font-family:Arial;font-size:10\">%.1f</text>\n",(position-4),temp_cursor);
				}
				for (k=0; k < number_of_sub_ticks_x; k++) {
					position = (y_max-temp_cursor-(k+1)*small_tick)*((height-10)/(y_max-y_min));
					buffer_fstring.Printf("<line x1=\"10\" y1=\"%f\" x2=\"%d\" y2=\"%f\" style=\"stroke:rgb(220,220,220);stroke-width:1\"/>\n", position, width, position);
				}
				temp_cursor += major_tick_y;
			}

			temp_cursor = 0;
			while (temp_cursor > y_min) {

				position = (y_max-temp_cursor)*((height-10)/(y_max-y_min));
				buffer_fstring.Printf("<line x1=\"10\" y1=\"%f\" x2=\"%d\" y2=\"%f\" style=\"stroke:rgb(150,150,150);stroke-width:1\"/>\n", position, width, position);

				if(legend_on_major_tick_y) {
					//	text with the major ticks legend
					buffer_fstring.Printf("<text x=\"%f\"  y=\"0\" transform=\"rotate(90 0,0)\" style=\"font-family:Arial;font-size:10\">%.1f</text>\n",(position-4),temp_cursor);
				}
				for (k=0; k < number_of_sub_ticks_x; k++) {
					position = (y_max-temp_cursor+(k+1)*small_tick)*((height-10)/(y_max-y_min));
					if((temp_cursor-(k+1)*small_tick) > y_min) {
						buffer_fstring.Printf("<line x1=\"10\" y1=\"%f\" x2=\"%d\" y2=\"%f\" style=\"stroke:rgb(220,220,220);stroke-width:1\"/>\n", position, width, position);
					}
				}
				temp_cursor -= major_tick_y;
			}
			//	dark yy axes
			temp_cursor = 0;
			position = (y_max-temp_cursor)*((height-10)/(y_max-y_min));
			buffer_fstring.Printf("<line x1=\"10\" y1=\"%f\" x2=\"%d\" y2=\"%f\" style=\"stroke:rgb(0,0,100);stroke-width:1\"/>\n", position, width, position);
		}

		pos_x = 10+(((*(vector_x))-x_min)*((width-10)/(x_max-x_min)));
		pos_y =	(y_max-(*(vector_y)))*((height-10)/(y_max-y_min));
	
		//	Now draw the waveform
		for (j=1; j < (vector_dim); j++) {

			pos_xx = 10+(((*(vector_x + j))-x_min)*((width-10)/(x_max-x_min)));
			pos_yy = (y_max-(*(vector_y + j)))*((height-10)/(y_max-y_min));
			buffer_fstring.Printf("<line x1=\"%f\" y1=\"%f\" x2=\"%f\" y2=\"%f\" style=\"stroke:rgb(255,0,0);stroke-width:2\"/>\n",pos_x,pos_y,pos_xx,pos_yy);
			pos_x = pos_xx;
			pos_y = pos_yy;
		}
		//	Draw the outter part of the graph
		buffer_fstring.Printf("<line x1=\"10\" y1=\"1\" x2=\"%d\" y2=\"1\" style=\"stroke:rgb(0,0,0);stroke-width:2\"/>\n\
			<line x1=\"10\" y1=\"%d\" x2=\"%d\" y2=\"%d\" style=\"stroke:rgb(0,0,0);stroke-width:2\"/>\n\
			<line x1=\"10\" y1=\"0\" x2=\"10\" y2=\"%d\" style=\"stroke:rgb(0,0,0);stroke-width:2\"/>\n\
			<line x1=\"%d\" y1=\"0;\" x2=\"%d\" y2=\"%d\" style=\"stroke:rgb(0,0,0);stroke-width:3\"/>", width, (height-10), width,(height-10),(height-10), width, width,(height-10));
		temp_string_2 += buffer_fstring.Buffer();

		temp_string_2 += "</svg";

	}


	*SVG_FString = temp_string_2.Buffer();

	return True;
}

//	provides an html header including the SVG top image
bool SVGGraphicSupport::HeaderSVG(FString * HeaderFString,char title[], int SVG_height,int image_x,int image_y,int image_width,int image_height,char image_link[],int text_x, int text_y, char text[], char bg_colour[]) {

	FString temp_string_3;

	temp_string_3.Printf("html>\n\
			<head>\n\
			<title>%s</title>\n\
			</head>\n\
			<body>\n\
			<svg width=\"100&#37;\" height=\"%d\" style=\"background-color: %s;\">\n\
			<image x=\"%d\" y=\"%d\" width=\"%d\" height=\"%d\" xlink:href=\"%s\" />\n\
			<text x=\"%d\"  y=\"%d\" style=\"font-family:Arial;font-size:46\">%s</text>\n\
			</svg", title, SVG_height, bg_colour ,image_x, image_y, image_width, image_height, image_link, text_x, text_y, text);

	*HeaderFString = temp_string_3.Buffer();
	return True;
}

//	provides an html footer including the SVG bottom image
bool SVGGraphicSupport::FooterSVG(FString * FooterFString,int SVG_height,int image_x,int image_y,int image_width,int image_height,char image_link[],int n_images,int text_x1,int text_x2,char gam_name[],char version[],char lastdate[],char author[],char description[]) {

	FString footer_str;
	float text_y[5];
	int footer_i;

	for (footer_i = 0; footer_i < 5; footer_i++) {
		text_y[footer_i] = (SVG_height/7)*(footer_i+2);
	}

	footer_str.Printf("svg width=\"100&#37;\" height=\"%d\">\n", SVG_height);

	for (footer_i = 0; footer_i < n_images; footer_i++) {
		footer_str.Printf("<image x=\"%d\" y=\"%d\" width=\"%d\" height=\"%d\" xlink:href=\"%s\" />\n",(image_x+image_width*footer_i), image_y,image_width, image_height, image_link);
	}
	footer_str.Printf("<text x=\"%d\"  y=\"%f\" style=\"font-family:Arial;font-size:14\">Name:</text>\n\
			<text x=\"%d\"  y=\"%f\" style=\"font-family:Arial;font-size:14\">Version:</text>\n\
			<text x=\"%d\"  y=\"%f\" style=\"font-family:Arial;font-size:14\">Date:</text>\n\
			<text x=\"%d\"  y=\"%f\" style=\"font-family:Arial;font-size:14\">Author:</text>\n\
			<text x=\"%d\"  y=\"%f\" style=\"font-family:Arial;font-size:14\">Description:</text>\n\
			<text x=\"%d\"  y=\"%f\" style=\"font-family:Arial;font-size:14\">%s</text>\n\
			<text x=\"%d\"  y=\"%f\" style=\"font-family:Arial;font-size:14\">%s</text>\n\
			<text x=\"%d\"  y=\"%f\" style=\"font-family:Arial;font-size:14\">%s</text>\n\
			<text x=\"%d\"  y=\"%f\" style=\"font-family:Arial;font-size:14\">%s</text>\n\
			<text x=\"%d\"  y=\"%f\" style=\"font-family:Arial;font-size:14\">%s</text>\n\
			</svg>\n", text_x1, text_y[0],text_x1, text_y[1],text_x1, text_y[2],text_x1, text_y[3],text_x1, text_y[4], text_x2, text_y[0],gam_name,text_x2, text_y[1],version,text_x2, text_y[2],lastdate,text_x2, text_y[3],author,text_x2, text_y[4], description);

	footer_str.Printf("</body>\n\
			</html");

	*FooterFString = footer_str.Buffer();
	return True;
}

//	saves: x_min x, x_max, y_min, y_max, major_tick_x, major_tick_y for calling GetGraphSVG
bool SVGGraphicSupport::OptimalDisplayFromVectors(int vector_dim, float * vector_x, float * vector_y,float * x_min,float * y_min,float * x_max,float * y_max,float * major_tick_x,float * major_tick_y ,int * number_of_sub_ticks_x,int * number_of_sub_ticks_y){
	
	float xmin = *vector_x;
	float xmax = *vector_x;
	float ymin = *vector_y;
	float ymax = *vector_y;
	float diff;
	float majortickx = 1;
	float majorticky = 1;
	int i;
	
	for (i=0; i < vector_dim; i++){
		
		if (*(vector_x+i)>xmax) xmax = *(vector_x+i);
		if (*(vector_x+i)<xmin) xmin = *(vector_x+i);
		if (*(vector_y+i)>ymax) ymax = *(vector_y+i);
		if (*(vector_y+i)<ymin) ymin = *(vector_y+i);
	}
	
	if (xmax==xmin){
		xmax +=1;
		xmin -=1;
	}
	if (ymax==ymin){
		ymax += 1;
		ymin -= 1;
	}
	
	diff = xmax-xmin;
	while (diff > 12 || diff < 6){
	
		if (diff>12){
			if ((diff/10) < 6 ){
				diff = diff / 2;
				majortickx = majortickx * 2;
			}
			else {
				diff = diff / 10;
				majortickx = majortickx * 10;
			}
		}
		else if(diff < 6){
			if ((diff*10) > 12){
				diff = diff * 2;
				majortickx = majortickx / 2;
			}
			else {
				diff = diff * 10;
				majortickx = majortickx / 10;
			}
		}
	}
	*number_of_sub_ticks_x = 4;
//	if (diff < 7) 	*number_of_sub_ticks_x = 9;
		
	diff = ymax-ymin;
	while (diff > 12 || diff < 6){
	
		if (diff>12){
			if ((diff/10) < 6 ){
				diff = diff / 2;
				majorticky = majorticky * 2;
			}
			else {
				diff = diff / 10;
				majorticky = majorticky * 10;
			}
		}
		else if(diff < 6){
			if ((diff*10) > 12){
				diff = diff * 2;
				majorticky = majorticky / 2;
			}
			else {
				diff = diff * 10;
				majorticky = majorticky / 10;
			}
		}
	}

	*number_of_sub_ticks_y = 4;
//	if (diff < 7) 	*number_of_sub_ticks_y = 9;


	*x_min = xmin - 0.01*(xmax-xmin);
	*x_max = xmax + 0.01*(xmax-xmin);
	*y_min = ymin - 0.01*(ymax-ymin);
	*y_max = ymax + 0.01*(ymax-ymin);
	
	
	if((*x_min) * (*x_max) > 0 ){
		if (x_min > 0){
			*x_min = (float) (majortickx * ((int) (*x_min/majortickx)));
		}
		else {
			*x_min = (float) ((majortickx * ((int) (*x_min/majortickx)))-majortickx);
		}
	}

	if((*y_min) * (*y_max) >0 ){
		if (y_min > 0){
			*y_min = (float) (majorticky * ((int) (*y_min/majorticky)));
		}
		else {
			*y_min = (float) ((majorticky * ((int) (*y_min/majorticky)))-majorticky);
		}
	}

	
	*major_tick_x = majortickx;
	*major_tick_y = majorticky;
	
	return True;
}



bool SVGGraphicSupport::HorizontalBarSVG ( FString * SVGFString){
	
	FString temp_str;
	
	temp_str.Printf("svg width=\"100&#37;\" height=\"15\">\n\
	<rect x=\"0\" y=\"3\" rx=\"5\" ry=\"5\" width=\"100&#37;\" height=\"10\" style=\"fill:navy;opacity:1\"/>\n\
	</svg");

			
	*SVGFString = temp_str.Buffer();
	return True;
}


bool SVGGraphicSupport::TrianglePlusBarSVG(FString * SVGFString, float triangle_position){
	
	FString temp_str;
	
	temp_str.Printf("svg width=\"100&#37;\" height=\"25\">\n\
	<polygon points=\"%f,0 %f,10 %f,10\" style=\"fill:navy;\"/>\n\
	<rect x=\"0\" y=\"10\" rx=\"5\" ry=\"5\" width=\"100&#37;\" height=\"10\" style=\"fill:navy;opacity:1\"/>\n\
	</svg", triangle_position, (triangle_position-10),(triangle_position+10));

			
	*SVGFString = temp_str.Buffer();
	return True;
}


