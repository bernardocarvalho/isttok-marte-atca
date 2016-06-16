//	Object properties
#define OBJECTNAME "Advanced Configurator"
/*
#define VERSION "1.01"
#define LASTDATE "2013-01-25"
#define AUTHOR "Ivo Carvalho - ivoc@ipfn.ist.utl.pt"
#define DESCRIPTION "HTML page to change and upload the configuration file parameters"
*/
#define HEADERIMAGELINK "http://www.ipfn.ist.utl.pt/ipfnPortalLayout/themes/ipfn/_img_/logoIPFN_Topo_officialColours.png"
#define FOOTERIMAGELINK "http://backoffice.ipfn.ist.utl.pt/mgallery/default.asp?obj=1"

#include "AdvancedConfigurator.h"
#include "MessageEnvelope.h"
#include "Message.h"


OBJECTLOADREGISTER(AdvancedConfigurator,"$Id: AdvancedConfigurator.cpp,v 1.0 2009/05/22 10:40:56 ivoc Exp $")


   /**  initialise an object from a set of configs */
bool AdvancedConfigurator::ObjectLoadSetup(ConfigurationDataBase & info, StreamInterface * err){
	
	int i;
	delete_selected_points_bool = False;
	vector_dim_temp = 0;
	
	already_started = False;
	
	this->graphics_support = new SVGGraphicSupport();
		
	save_filename.Printf("last.cfg");

        GCReferenceContainer::ObjectLoadSetup(info,err);

        CDBExtended &cdbx = (CDBExtended &)info;

        cdbx.ReadFString(configFileID, "configFileID", "cfgFile");

	if(!cdbx.ReadFString(Description, "Description"))
	{
		CStaticAssertErrorCondition(InitialisationError,"AdvancedConfigurator::ObjectLoadSetup: %s Could not read Description",this->Name());
		return False;
	}

// Move to the colours
	if(!cdbx->Move("colours"))
	{
		CStaticAssertErrorCondition(InitialisationError,"AdvancedConfigurator::ObjectLoadSetup: %s Could not move to \"colours\"",this->Name());
		return False;
	}
		if(!cdbx.ReadFString(header_colour, "header_colour"))
		{
			CStaticAssertErrorCondition(InitialisationError,"AdvancedConfigurator::ObjectLoadSetup: %s Could not read header_colour",this->Name());
			return False;
		}
		if(!cdbx.ReadFString(colour1, "colour1"))
		{
			CStaticAssertErrorCondition(InitialisationError,"AdvancedConfigurator::ObjectLoadSetup: %s Could not read colour1",this->Name());
			return False;
		}
		if(!cdbx.ReadFString(colour2, "colour2"))
		{
			CStaticAssertErrorCondition(InitialisationError,"AdvancedConfigurator::ObjectLoadSetup: %s Could not read colour2",this->Name());
			return False;
		}
		if(!cdbx.ReadFString(colour3, "colour3"))
		{
			CStaticAssertErrorCondition(InitialisationError,"AdvancedConfigurator::ObjectLoadSetup: %s Could not read colour3",this->Name());
			return False;
		}
		if(!cdbx.ReadFString(colour4, "colour4"))
		{
			CStaticAssertErrorCondition(InitialisationError,"AdvancedConfigurator::ObjectLoadSetup: %s Could not read colour4",this->Name());
			return False;
		}
		if(!cdbx.ReadFString(colour5, "colour5"))
		{
			CStaticAssertErrorCondition(InitialisationError,"AdvancedConfigurator::ObjectLoadSetup: %s Could not read colour5",this->Name());
			return False;
		}
		if(!cdbx.ReadFString(footer_colour, "footer_colour"))
		{
			CStaticAssertErrorCondition(InitialisationError,"AdvancedConfigurator::ObjectLoadSetup: %s Could not read footer_colour",this->Name());
			return False;
		}
	// Move back to the GAM main branch
	cdbx->MoveToFather();

// Move to the default directories
	if(!cdbx->Move("default_directories"))
	{
		CStaticAssertErrorCondition(InitialisationError,"AdvancedConfigurator::ObjectLoadSetup: %s Could not move to \"default_directories\"",this->Name());
		return False;
	}
		if(!cdbx.ReadFString(default_config_file_path, "default_config_file_path"))
		{
			CStaticAssertErrorCondition(InitialisationError,"AdvancedConfigurator::ObjectLoadSetup: %s Could not read default_config_file_path",this->Name());
			return False;
		}
		if(!cdbx.ReadFString(config_files_directory, "config_files_directory"))
		{
			CStaticAssertErrorCondition(InitialisationError,"AdvancedConfigurator::ObjectLoadSetup: %s Could not read config_files_directory",this->Name());
			return False;
		}
	// Move back to the GAM main branch
	cdbx->MoveToFather();

	graphic_select = 0;
	selected_load_directory = "..";
	selected_load_file = "last.cfg";
		
	this->ReadConfigurationFile((char *)default_config_file_path.Buffer());	
	
	if(!cdbx.ReadFString(marteLocation, "MARTeLocation", "MARTe")){
		AssertErrorCondition(Warning, "ObjectLoadSetup::MARTe location wasn not specified. Using default: %s", marteLocation.Buffer());
	}

	return HttpInterface::ObjectLoadSetup(info,err);
}
	
bool AdvancedConfigurator::PrintHTTPForm(HtmlStream &hmStream){
	
	
	FString temp_string_1;
	int i;

	hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 10px; left: 1050px;\"><b>Units used:</b></div");
	hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 30px; left: 1050px;\">Time in miliseconds</div");
	hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 50px; left: 1050px;\">Distance in milimeters</div");
	hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 70px; left: 1050px;\">Current in Ampere</div");

	hmStream.SSPrintf(HtmlTagStreamMode, "form enctype=\"multipart/form-data\" method=\"post\"");
//	Description box	
	hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 140px; left: 780px;\"");
	hmStream.SSPrintf(HtmlTagStreamMode, "TEXTAREA NAME=\"form_description\" COLS=50 ROWS=\"7\" WRAP=\"SOFT\">%s</TEXTAREA", Description.Buffer());
	hmStream.SSPrintf(HtmlTagStreamMode, "/div");
	hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 120px; left: 780px;\">File Description:</div");
	
//	hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 205px; left: 10px;\">_____________________________________________________________________________________________</div");

	hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 225px; left: 15px;\"><b>Control file to load:</b></div");
	hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 245px; left: 15px;\"><b>Tomography file to load:</b></div");
		
	hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 135px; left: 15px;\"><b>Tomography active channels (1 to 8):</b></div");
	hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 155px; left: 15px;\">top</div");
	hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 175px; left: 15px;\">outer</div");
	hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 195px; left: 15px;\">bottom</div");
	
	hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 155px; left: 70px;\"");
		if (tomography_online_channels[0]) hmStream.SSPrintf(HtmlTagStreamMode, "input type=\"checkbox\" name=\"form_tomography_online_0\" CHECKED"); 
		else hmStream.SSPrintf(HtmlTagStreamMode, "input type=\"checkbox\" name=\"form_tomography_online_0\"");
		if (tomography_online_channels[1]) hmStream.SSPrintf(HtmlTagStreamMode, "input type=\"checkbox\" name=\"form_tomography_online_1\" CHECKED"); 
		else hmStream.SSPrintf(HtmlTagStreamMode, "input type=\"checkbox\" name=\"form_tomography_online_1\"");
		if (tomography_online_channels[2]) hmStream.SSPrintf(HtmlTagStreamMode, "input type=\"checkbox\" name=\"form_tomography_online_2\" CHECKED"); 
		else hmStream.SSPrintf(HtmlTagStreamMode, "input type=\"checkbox\" name=\"form_tomography_online_2\"");
		if (tomography_online_channels[3]) hmStream.SSPrintf(HtmlTagStreamMode, "input type=\"checkbox\" name=\"form_tomography_online_3\" CHECKED"); 
		else hmStream.SSPrintf(HtmlTagStreamMode, "input type=\"checkbox\" name=\"form_tomography_online_3\"");
		if (tomography_online_channels[4]) hmStream.SSPrintf(HtmlTagStreamMode, "input type=\"checkbox\" name=\"form_tomography_online_4\" CHECKED"); 
		else hmStream.SSPrintf(HtmlTagStreamMode, "input type=\"checkbox\" name=\"form_tomography_online_4\"");
		if (tomography_online_channels[5]) hmStream.SSPrintf(HtmlTagStreamMode, "input type=\"checkbox\" name=\"form_tomography_online_5\" CHECKED"); 
		else hmStream.SSPrintf(HtmlTagStreamMode, "input type=\"checkbox\" name=\"form_tomography_online_5\"");
		if (tomography_online_channels[6]) hmStream.SSPrintf(HtmlTagStreamMode, "input type=\"checkbox\" name=\"form_tomography_online_6\" CHECKED"); 
		else hmStream.SSPrintf(HtmlTagStreamMode, "input type=\"checkbox\" name=\"form_tomography_online_6\"");
		if (tomography_online_channels[7]) hmStream.SSPrintf(HtmlTagStreamMode, "input type=\"checkbox\" name=\"form_tomography_online_7\" CHECKED"); 
		else hmStream.SSPrintf(HtmlTagStreamMode, "input type=\"checkbox\" name=\"form_tomography_online_7\"");
	hmStream.SSPrintf(HtmlTagStreamMode, "/div");
		
	hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 175px; left: 70px;\"");
		if (tomography_online_channels[8]) hmStream.SSPrintf(HtmlTagStreamMode, "input type=\"checkbox\" name=\"form_tomography_online_8\" CHECKED"); 
		else hmStream.SSPrintf(HtmlTagStreamMode, "input type=\"checkbox\" name=\"form_tomography_online_8\"");
		if (tomography_online_channels[9]) hmStream.SSPrintf(HtmlTagStreamMode, "input type=\"checkbox\" name=\"form_tomography_online_9\" CHECKED"); 
		else hmStream.SSPrintf(HtmlTagStreamMode, "input type=\"checkbox\" name=\"form_tomography_online_9\"");
		if (tomography_online_channels[10]) hmStream.SSPrintf(HtmlTagStreamMode, "input type=\"checkbox\" name=\"form_tomography_online_10\" CHECKED"); 
		else hmStream.SSPrintf(HtmlTagStreamMode, "input type=\"checkbox\" name=\"form_tomography_online_10\"");
		if (tomography_online_channels[11]) hmStream.SSPrintf(HtmlTagStreamMode, "input type=\"checkbox\" name=\"form_tomography_online_11\" CHECKED"); 
		else hmStream.SSPrintf(HtmlTagStreamMode, "input type=\"checkbox\" name=\"form_tomography_online_11\"");
		if (tomography_online_channels[12]) hmStream.SSPrintf(HtmlTagStreamMode, "input type=\"checkbox\" name=\"form_tomography_online_12\" CHECKED"); 
		else hmStream.SSPrintf(HtmlTagStreamMode, "input type=\"checkbox\" name=\"form_tomography_online_12\"");
		if (tomography_online_channels[13]) hmStream.SSPrintf(HtmlTagStreamMode, "input type=\"checkbox\" name=\"form_tomography_online_13\" CHECKED"); 
		else hmStream.SSPrintf(HtmlTagStreamMode, "input type=\"checkbox\" name=\"form_tomography_online_13\"");
		if (tomography_online_channels[14]) hmStream.SSPrintf(HtmlTagStreamMode, "input type=\"checkbox\" name=\"form_tomography_online_14\" CHECKED"); 
		else hmStream.SSPrintf(HtmlTagStreamMode, "input type=\"checkbox\" name=\"form_tomography_online_14\"");
		if (tomography_online_channels[15]) hmStream.SSPrintf(HtmlTagStreamMode, "input type=\"checkbox\" name=\"form_tomography_online_15\" CHECKED"); 
		else hmStream.SSPrintf(HtmlTagStreamMode, "input type=\"checkbox\" name=\"form_tomography_online_15\"");
	hmStream.SSPrintf(HtmlTagStreamMode, "/div");
		
	hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 195px; left: 70px;\"");
		if (tomography_online_channels[16]) hmStream.SSPrintf(HtmlTagStreamMode, "input type=\"checkbox\" name=\"form_tomography_online_16\" CHECKED"); 
		else hmStream.SSPrintf(HtmlTagStreamMode, "input type=\"checkbox\" name=\"form_tomography_online_16\"");
		if (tomography_online_channels[17]) hmStream.SSPrintf(HtmlTagStreamMode, "input type=\"checkbox\" name=\"form_tomography_online_17\" CHECKED"); 
		else hmStream.SSPrintf(HtmlTagStreamMode, "input type=\"checkbox\" name=\"form_tomography_online_17\"");
		if (tomography_online_channels[18]) hmStream.SSPrintf(HtmlTagStreamMode, "input type=\"checkbox\" name=\"form_tomography_online_18\" CHECKED"); 
		else hmStream.SSPrintf(HtmlTagStreamMode, "input type=\"checkbox\" name=\"form_tomography_online_18\"");
		if (tomography_online_channels[19]) hmStream.SSPrintf(HtmlTagStreamMode, "input type=\"checkbox\" name=\"form_tomography_online_19\" CHECKED"); 
		else hmStream.SSPrintf(HtmlTagStreamMode, "input type=\"checkbox\" name=\"form_tomography_online_19\"");
		if (tomography_online_channels[20]) hmStream.SSPrintf(HtmlTagStreamMode, "input type=\"checkbox\" name=\"form_tomography_online_20\" CHECKED"); 
		else hmStream.SSPrintf(HtmlTagStreamMode, "input type=\"checkbox\" name=\"form_tomography_online_20\"");
		if (tomography_online_channels[21]) hmStream.SSPrintf(HtmlTagStreamMode, "input type=\"checkbox\" name=\"form_tomography_online_21\" CHECKED"); 
		else hmStream.SSPrintf(HtmlTagStreamMode, "input type=\"checkbox\" name=\"form_tomography_online_21\"");
		if (tomography_online_channels[22]) hmStream.SSPrintf(HtmlTagStreamMode, "input type=\"checkbox\" name=\"form_tomography_online_22\" CHECKED"); 
		else hmStream.SSPrintf(HtmlTagStreamMode, "input type=\"checkbox\" name=\"form_tomography_online_22\"");
		if (tomography_online_channels[23]) hmStream.SSPrintf(HtmlTagStreamMode, "input type=\"checkbox\" name=\"form_tomography_online_23\" CHECKED"); 
		else hmStream.SSPrintf(HtmlTagStreamMode, "input type=\"checkbox\" name=\"form_tomography_online_23\"");
	hmStream.SSPrintf(HtmlTagStreamMode, "/div");
		
	hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 135px; left: 450px;\">Primary PS</div");
	hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 135px; left: 550px;\">Vertical PS</div");
	hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 135px; left: 650px;\">Horizontal PS</div");
	hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 155px; left: 320px;\">UART port address</div");
	hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 175px; left: 320px;\">Zero current point</div");
	hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 195px; left: 320px;\">Current step</div");
		
	if(!edit_general){
		
		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 100px; left: 190px;\"");
		hmStream.SSPrintf(HtmlTagStreamMode, "input type=\"submit\" name=\"submit_edit_general\" value=\"Edit parameters\"");
		hmStream.SSPrintf(HtmlTagStreamMode, "/div");

		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 225px; left: 155px;\">%s</div", control_file_to_load.Buffer());
		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 245px; left: 190px;\">%s</div", tomography_file_to_load.Buffer());

		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 155px; left: 460px;\">%s</div", primaryPS_UARTPortAddress.Buffer());
		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 175px; left: 460px;\">%0.2f</div", primaryPS_PointOfZeroCurrent);
		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 195px; left: 460px;\">%0.4f</div", primaryPS_CurrentStep);

		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 155px; left: 570px;\">%s</div", verticalPS_UARTPortAddress.Buffer());
		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 175px; left: 570px;\">%0.2f</div", verticalPS_PointOfZeroCurrent);
		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 195px; left: 570px;\">%0.4f</div", verticalPS_CurrentStep);

		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 155px; left: 670px;\">%s</div", horizontalPS_UARTPortAddress.Buffer());
		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 175px; left: 670px;\">%0.2f</div", horizontalPS_PointOfZeroCurrent);
		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 195px; left: 670px;\">%0.4f</div", horizontalPS_CurrentStep);


	}
	else {
		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 100px; left: 190px;\"");
		hmStream.SSPrintf(HtmlTagStreamMode, "input type=\"submit\" name=\"submit_save_general\" value=\"Save parameters\"");
		hmStream.SSPrintf(HtmlTagStreamMode, "/div");
		
		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 223px; left: 155px;\"><input type=\"text\" size=\"65\" maxlength=\"100\" name=\"form_control_file_to_load\" value=\"%s\"></div",control_file_to_load.Buffer());
		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 245px; left: 190px;\"><input type=\"text\" size=\"65\" maxlength=\"100\" name=\"form_tomography_file_to_load\" value=\"%s\"></div",tomography_file_to_load.Buffer());


		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 155px; left: 460px;\"><input type=\"text\" size=\"3\" maxlength=\"6\" name=\"form_primaryPS_UARTPortAddress\" value=\"%s\"></div",primaryPS_UARTPortAddress.Buffer());
		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 175px; left: 460px;\"><input type=\"text\" size=\"3\" maxlength=\"6\" name=\"form_primaryPS_PointOfZeroCurrent\" value=\"%0.2f\"></div",primaryPS_PointOfZeroCurrent);
		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 195px; left: 460px;\"><input type=\"text\" size=\"3\" maxlength=\"6\" name=\"form_primaryPS_CurrentStep\" value=\"%0.4f\"></div",primaryPS_CurrentStep);

		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 155px; left: 570px;\"><input type=\"text\" size=\"3\" maxlength=\"6\" name=\"form_verticalPS_UARTPortAddress\" value=\"%s\"></div",verticalPS_UARTPortAddress.Buffer());
		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 175px; left: 570px;\"><input type=\"text\" size=\"3\" maxlength=\"6\" name=\"form_verticalPS_PointOfZeroCurrent\" value=\"%0.2f\"></div",verticalPS_PointOfZeroCurrent);
		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 195px; left: 570px;\"><input type=\"text\" size=\"3\" maxlength=\"6\" name=\"form_verticalPS_CurrentStep\" value=\"%0.4f\"></div",verticalPS_CurrentStep);

		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 155px; left: 670px;\"><input type=\"text\" size=\"3\" maxlength=\"6\" name=\"form_horizontalPS_UARTPortAddress\" value=\"%s\"></div",horizontalPS_UARTPortAddress.Buffer());
		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 175px; left: 670px;\"><input type=\"text\" size=\"3\" maxlength=\"6\" name=\"form_horizontalPS_PointOfZeroCurrent\" value=\"%0.2f\"></div",horizontalPS_PointOfZeroCurrent);
		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 195px; left: 670px;\"><input type=\"text\" size=\"3\" maxlength=\"6\" name=\"form_horizontalPS_CurrentStep\" value=\"%0.4f\"></div",horizontalPS_CurrentStep);
	}
		
// control settings

	hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 322px; left: 15px;\"><b>Actuators:</b></div");
	hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 300px; left: 355px;\"><b>Processed:</b></div");
	hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 300px; left: 690px;\"><b>Gas puffing feedback:</b></div");
	hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 300px; left: 970px;\"><b>MARTe Configuration</b></div");

	hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 322px; left: 145px;\">Minimum</div");
	hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 322px; left: 235px;\">Maximum</div");
	hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 322px; left: 475px;\">Minimum</div");
	hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 322px; left: 575px;\">Maximum</div");
	
	hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 344px; left: 15px;\">Toroidal current</div");
	hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 366px; left: 15px;\">Primary current</div");
	hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 388px; left: 15px;\">Vertical current</div");
	hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 410px; left: 15px;\">Horizontal current</div");
	
	hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 344px; left: 355px;\">Plasma current</div");
	hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 366px; left: 355px;\">Radial position</div");
	hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 388px; left: 355px;\">Vertical position</div");
	hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 410px; left: 355px;\">Density & HAlpha</div");

	hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 322px; left: 690px;\">puffing duration</div");
	hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 344px; left: 690px;\">maximum idle time</div");
	hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 366px; left: 690px;\">minimum idle time</div");
	hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 388px; left: 690px;\">&#37; of cycle change</div");

	hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 410px; left: 690px;\">Feedback type:<b");
		if (puffing_feedback_mode == 1) 	hmStream.SSPrintf(HtmlTagStreamMode, "/b>&nbsp;&nbsp;&nbsp;&nbsp;<input type=\"radio\" name=\"form_puffing_feedback_mode\" value=\"1\" CHECKED /><text>Density</text><input type=\"radio\" name=\"form_puffing_feedback_mode\" value=\"2\" /><text>HAlpha</text><br");
		else  hmStream.SSPrintf(HtmlTagStreamMode, "/b>&nbsp;&nbsp;&nbsp;&nbsp;<input type=\"radio\" name=\"form_puffing_feedback_mode\" value=\"1\" /><text>Density</text><input type=\"radio\" name=\"form_puffing_feedback_mode\" value=\"2\" CHECKED /><text>HAlpha</text><br");
	hmStream.SSPrintf(HtmlTagStreamMode, "/div");

	hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 322px; left: 970px;\">Thread priority</div");
	hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 344px; left: 970px;\">RunOnCPU ( &#8721; 2<sup>n</sup> )</div");

	if(!edit_control){
		
		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 275px; left: 190px;\"");
		hmStream.SSPrintf(HtmlTagStreamMode, "input type=\"submit\" name=\"submit_edit_control\" value=\"Edit parameters\"");
		hmStream.SSPrintf(HtmlTagStreamMode, "/div");

		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 344px; left: 135px;\">%.1f A</div",toroidal_1_p_min_value);
		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 344px; left: 235px;\">%.1f A</div",toroidal_1_p_max_value);
		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 366px; left: 145px;\">%.1f A</div",primary_1_p_min_value);
		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 366px; left: 245px;\">%.1f A</div",primary_1_p_max_value);
		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 388px; left: 145px;\">%.1f A</div",vertical_1_p_min_value);
		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 388px; left: 245px;\">%.1f A</div",vertical_1_p_max_value);
		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 410px; left: 145px;\">%.1f A</div",horizontal_1_p_min_value);
		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 410px; left: 245px;\">%.1f A</div",horizontal_1_p_max_value);

		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 344px; left: 485px;\">%.1f A</div",primary_2_p_min_value);
		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 344px; left: 575px;\">%.1f A</div",primary_2_p_max_value);
		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 366px; left: 485px;\">%.1f mm</div",vertical_2_p_min_value);
		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 366px; left: 575px;\">%.1f mm</div",vertical_2_p_max_value);
		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 388px; left: 485px;\">%.1f mm</div",horizontal_2_p_min_value);
		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 388px; left: 575px;\">%.1f mm</div",horizontal_2_p_max_value);
		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 410px; left: 485px;\">%.1f au</div",puffing_2_p_min_value);
		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 410px; left: 575px;\">%.1f au</div",puffing_2_p_max_value);

		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 322px; left: 825px;\">%.2f ms</div",puffing_duration_in_puffing_feedback_in_ms);
		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 344px; left: 825px;\">%.2f ms</div",maximum_idle_time_in_puffing_feedback_in_ms);
		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 366px; left: 825px;\">%.2f ms</div",minimum_idle_time_in_puffing_feedback_in_ms);
		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 388px; left: 825px;\">%.2f &#37;</div",puffing_feedback_usec_change_percentage_by_cycle);
		
		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 322px; left: 1120px;\">%d</div",thread_priority);
		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 344px; left: 1120px;\">%d</div",run_on_cpu);

	}
	else {
		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 275px; left: 190px;\"");
		hmStream.SSPrintf(HtmlTagStreamMode, "input type=\"submit\" name=\"submit_save_control\" value=\"Save parameters\"");
		hmStream.SSPrintf(HtmlTagStreamMode, "/div");

//		control edit boxes
//	minimum and maximum values
		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 344px; left: 145px;\"><input type=\"text\" size=\"3\" maxlength=\"6\" name=\"form_toroidal_1_p_min_value\" value=\"%0.1f\"></div",toroidal_1_p_min_value);
		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 344px; left: 245px;\"><input type=\"text\" size=\"3\" maxlength=\"6\" name=\"form_toroidal_1_p_max_value\" value=\"%0.1f\"></div",toroidal_1_p_max_value);
		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 366px; left: 145px;\"><input type=\"text\" size=\"3\" maxlength=\"6\" name=\"form_primary_1_p_min_value\" value=\"%0.1f\"></div",primary_1_p_min_value);
		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 366px; left: 245px;\"><input type=\"text\" size=\"3\" maxlength=\"6\" name=\"form_primary_1_p_max_value\" value=\"%0.1f\"></div",primary_1_p_max_value);
		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 388px; left: 145px;\"><input type=\"text\" size=\"3\" maxlength=\"6\" name=\"form_vertical_1_p_min_value\" value=\"%0.1f\"></div",vertical_1_p_min_value);
		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 388px; left: 245px;\"><input type=\"text\" size=\"3\" maxlength=\"6\" name=\"form_vertical_1_p_max_value\" value=\"%0.1f\"></div",vertical_1_p_max_value);
		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 410px; left: 145px;\"><input type=\"text\" size=\"3\" maxlength=\"6\" name=\"form_horizontal_1_p_min_value\" value=\"%0.1f\"></div",horizontal_1_p_min_value);
		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 410px; left: 245px;\"><input type=\"text\" size=\"3\" maxlength=\"6\" name=\"form_horizontal_1_p_max_value\" value=\"%0.1f\"></div",horizontal_1_p_max_value);

		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 344px; left: 485px;\"><input type=\"text\" size=\"3\" maxlength=\"6\" name=\"form_primary_2_p_min_value\" value=\"%0.1f\"></div",primary_2_p_min_value);
		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 344px; left: 575px;\"><input type=\"text\" size=\"3\" maxlength=\"6\" name=\"form_primary_2_p_max_value\" value=\"%0.1f\"></div",primary_2_p_max_value);
		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 366px; left: 485px;\"><input type=\"text\" size=\"3\" maxlength=\"6\" name=\"form_vertical_2_p_min_value\" value=\"%0.1f\"></div",vertical_2_p_min_value);
		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 366px; left: 575px;\"><input type=\"text\" size=\"3\" maxlength=\"6\" name=\"form_vertical_2_p_max_value\" value=\"%0.1f\"></div",vertical_2_p_max_value);
		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 388px; left: 485px;\"><input type=\"text\" size=\"3\" maxlength=\"6\" name=\"form_horizontal_2_p_min_value\" value=\"%0.1f\"></div",horizontal_2_p_min_value);
		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 388px; left: 575px;\"><input type=\"text\" size=\"3\" maxlength=\"6\" name=\"form_horizontal_2_p_max_value\" value=\"%0.1f\"></div",horizontal_2_p_max_value);
		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 410px; left: 485px;\"><input type=\"text\" size=\"3\" maxlength=\"6\" name=\"form_puffing_2_p_min_value\" value=\"%0.1f\"></div",puffing_2_p_min_value);
		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 410px; left: 575px;\"><input type=\"text\" size=\"3\" maxlength=\"6\" name=\"form_puffing_2_p_max_value\" value=\"%0.1f\"></div",puffing_2_p_max_value);

		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 322px; left: 825px;\"><input type=\"text\" size=\"3\" maxlength=\"6\" name=\"form_puffing_duration_in_puffing_feedback_in_ms\" value=\"%0.1f\"></div",puffing_duration_in_puffing_feedback_in_ms);
		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 344px; left: 825px;\"><input type=\"text\" size=\"3\" maxlength=\"6\" name=\"form_maximum_idle_time_in_puffing_feedback_in_ms\" value=\"%0.1f\"></div",maximum_idle_time_in_puffing_feedback_in_ms);
		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 366px; left: 825px;\"><input type=\"text\" size=\"3\" maxlength=\"6\" name=\"form_minimum_idle_time_in_puffing_feedback_in_ms\" value=\"%0.1f\"></div",minimum_idle_time_in_puffing_feedback_in_ms);
		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 388px; left: 825px;\"><input type=\"text\" size=\"3\" maxlength=\"6\" name=\"form_puffing_feedback_usec_change_percentage_by_cycle\" value=\"%0.1f\"></div",puffing_feedback_usec_change_percentage_by_cycle);
		
		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 322px; left: 1120px;\"><input type=\"text\" size=\"3\" maxlength=\"6\" name=\"form_thread_priority\" value=\"%d\"></div",thread_priority);
		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 344px; left: 1120px;\"><input type=\"text\" size=\"3\" maxlength=\"6\" name=\"form_run_on_cpu\" value=\"%d\"></div",run_on_cpu);
	}
	
	hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 490px; left: 15px;\"");
	hmStream.Printf("End if unsuccessful breakdown/inversion");
	if (end_discharge_after_unsuccess_bool) hmStream.SSPrintf(HtmlTagStreamMode, "input type=\"checkbox\" name=\"form_end_discharge_after_unsuccess_bool\" CHECKED"); 
	else hmStream.SSPrintf(HtmlTagStreamMode, "input type=\"checkbox\" name=\"form_end_discharge_after_unsuccess_bool\"");
	hmStream.SSPrintf(HtmlTagStreamMode, "/div");
	
	hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 515px; left: 15px;\">saturation predictor 1 limit</div");
	hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 535px; left: 15px;\">saturation Iprim/Vloop limit</div");
	
	hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 570px; left: 15px;\"><b>Discharge Timming</b></div");
	hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 600px; left: 15px;\">online to discharge time</div");
	hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 620px; left: 15px;\">breakdown maximum time</div");
	hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 640px; left: 15px;\">inversion maximum time</div");


	hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 680px; left: 15px;\"><b>Puffing output:</b><br");
		if (puffing_mode == 1) { 
			hmStream.SSPrintf(HtmlTagStreamMode, "input type=\"radio\" name=\"form_puffing_mode\" value=\"1\" CHECKED /><text>Time windows, open percentage</text><br");
			hmStream.SSPrintf(HtmlTagStreamMode, "input type=\"radio\" name=\"form_puffing_mode\" value=\"2\" /><text>Time windows with feedback</text><br");
			hmStream.SSPrintf(HtmlTagStreamMode, "input type=\"radio\" name=\"form_puffing_mode\" value=\"3\" /><text>Absolute time, open percentage</text><br");
			hmStream.SSPrintf(HtmlTagStreamMode, "input type=\"radio\" name=\"form_puffing_mode\" value=\"4\" /><text>Preprogrammed + time windows feedback</text><br");
			hmStream.SSPrintf(HtmlTagStreamMode, "input type=\"radio\" name=\"form_puffing_mode\" value=\"0\" /><text>OFF</text><br");
		}
		else if (puffing_mode == 2) { 
			hmStream.SSPrintf(HtmlTagStreamMode, "input type=\"radio\" name=\"form_puffing_mode\" value=\"1\" /><text>Time windows, open percentage</text><br");
			hmStream.SSPrintf(HtmlTagStreamMode, "input type=\"radio\" name=\"form_puffing_mode\" value=\"2\" CHECKED /><text>Time windows with feedback</text><br");
			hmStream.SSPrintf(HtmlTagStreamMode, "input type=\"radio\" name=\"form_puffing_mode\" value=\"3\" /><text>Absolute time, open percentage</text><br");
			hmStream.SSPrintf(HtmlTagStreamMode, "input type=\"radio\" name=\"form_puffing_mode\" value=\"4\" /><text>Preprogrammed + time windows feedback</text><br");
			hmStream.SSPrintf(HtmlTagStreamMode, "input type=\"radio\" name=\"form_puffing_mode\" value=\"0\" /><text>OFF</text><br");
		}
		else if (puffing_mode == 3) { 
			hmStream.SSPrintf(HtmlTagStreamMode, "input type=\"radio\" name=\"form_puffing_mode\" value=\"1\" /><text>Time windows, open percentage</text><br");
			hmStream.SSPrintf(HtmlTagStreamMode, "input type=\"radio\" name=\"form_puffing_mode\" value=\"2\" /><text>Time windows with feedback</text><br");
			hmStream.SSPrintf(HtmlTagStreamMode, "input type=\"radio\" name=\"form_puffing_mode\" value=\"3\" CHECKED /><text>Absolute time, open percentage</text><br");
			hmStream.SSPrintf(HtmlTagStreamMode, "input type=\"radio\" name=\"form_puffing_mode\" value=\"4\" /><text>Preprogrammed + time windows feedback</text><br");
			hmStream.SSPrintf(HtmlTagStreamMode, "input type=\"radio\" name=\"form_puffing_mode\" value=\"0\" /><text>OFF</text><br");
		}
		else if (puffing_mode == 4) { 
			hmStream.SSPrintf(HtmlTagStreamMode, "input type=\"radio\" name=\"form_puffing_mode\" value=\"1\" /><text>Time windows, open percentage</text><br");
			hmStream.SSPrintf(HtmlTagStreamMode, "input type=\"radio\" name=\"form_puffing_mode\" value=\"2\" /><text>Time windows with feedback</text><br");
			hmStream.SSPrintf(HtmlTagStreamMode, "input type=\"radio\" name=\"form_puffing_mode\" value=\"3\" /><text>Absolute time, open percentage</text><br");
			hmStream.SSPrintf(HtmlTagStreamMode, "input type=\"radio\" name=\"form_puffing_mode\" value=\"4\" CHECKED /><text>Preprogrammed + time windows feedback</text><br");
			hmStream.SSPrintf(HtmlTagStreamMode, "input type=\"radio\" name=\"form_puffing_mode\" value=\"0\" /><text>OFF</text><br");
		}
		else{
			hmStream.SSPrintf(HtmlTagStreamMode, "input type=\"radio\" name=\"form_puffing_mode\" value=\"1\" /><text>Time windows, open percentage</text><br");
			hmStream.SSPrintf(HtmlTagStreamMode, "input type=\"radio\" name=\"form_puffing_mode\" value=\"2\" /><text>Time windows with feedback</text><br");
			hmStream.SSPrintf(HtmlTagStreamMode, "input type=\"radio\" name=\"form_puffing_mode\" value=\"3\" /><text>Absolute time, open percentage</text><br");
			hmStream.SSPrintf(HtmlTagStreamMode, "input type=\"radio\" name=\"form_puffing_mode\" value=\"4\" /><text>Preprogrammed + time windows feedback</text><br");
			hmStream.SSPrintf(HtmlTagStreamMode, "input type=\"radio\" name=\"form_puffing_mode\" value=\"0\" CHECKED /><text>OFF</text><br");
		}
	hmStream.SSPrintf(HtmlTagStreamMode, "/div");

	if(!edit_main){
		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 445px; left: 310px;\"");
		hmStream.SSPrintf(HtmlTagStreamMode, "input type=\"submit\" name=\"submit_edit_main\" value=\"Edit parameters\"");
		hmStream.SSPrintf(HtmlTagStreamMode, "/div");
		
		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 515px; left: 200px;\">%0.3f</div",iron_core_saturation_value);
		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 535px; left: 200px;\">%0.3f</div",iron_core_dangerous_value);

		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 600px; left: 200px;\">%d &#181;s</div",time_between_online_and_discharge);
		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 620px; left: 200px;\">%d &#181;s</div",usec_pre_pulse_time);
		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 640px; left: 200px;\">%d &#181;s</div",maximum_inversion_usectime);
	}
	else {
		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 445px; left: 310px;\"");
		hmStream.SSPrintf(HtmlTagStreamMode, "input type=\"submit\" name=\"submit_save_main\" value=\"Save parameters\"");
		hmStream.SSPrintf(HtmlTagStreamMode, "/div");
		
		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 515px; left: 200px;\"><input type=\"text\" size=\"3\" maxlength=\"6\" name=\"form_iron_core_saturation_value\" value=\"%0.3f\"></div",iron_core_saturation_value);
		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 535px; left: 200px;\"><input type=\"text\" size=\"3\" maxlength=\"6\" name=\"form_iron_core_dangerous_value\" value=\"%0.3f\"></div",iron_core_dangerous_value);

		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 600px; left: 200px;\"><input type=\"text\" size=\"3\" maxlength=\"9\" name=\"form_time_between_online_and_discharge\" value=\"%d\"></div",time_between_online_and_discharge);
		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 620px; left: 200px;\"><input type=\"text\" size=\"3\" maxlength=\"6\" name=\"form_usec_pre_pulse_time\" value=\"%d\"></div",usec_pre_pulse_time);
		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 640px; left: 200px;\"><input type=\"text\" size=\"3\" maxlength=\"6\" name=\"form_maximum_inversion_usectime\" value=\"%d\"></div",maximum_inversion_usectime);
	}
	
	hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 460px; left: 520px;\">Select waveform:  \
		<select NAME=\"form_graphic_select\" onchange='this.form.submit()'");
			hmStream.SSPrintf(HtmlTagStreamMode, "optgroup label=\"Breakdown to Ip > 0\"");
				if(graphic_select == 0) hmStream.SSPrintf(HtmlTagStreamMode, "option selected=\"selected\" value=0>Primary breakdown to Ip > 0</option");	
				else hmStream.SSPrintf(HtmlTagStreamMode, "option value=0>Primary breakdown to Ip > 0</option");	
				if(graphic_select == 1) hmStream.SSPrintf(HtmlTagStreamMode, "option selected=\"selected\" value=1>Vertical breakdown to Ip > 0</option");
				else hmStream.SSPrintf(HtmlTagStreamMode, "option value=1>Vertical breakdown to Ip > 0</option");
				if(graphic_select == 2) hmStream.SSPrintf(HtmlTagStreamMode, "option selected=\"selected\" value=2>Horizontal breakdown to Ip > 0</option");
				else hmStream.SSPrintf(HtmlTagStreamMode, "option value=2>Horizontal breakdown to Ip > 0</option");
			hmStream.SSPrintf(HtmlTagStreamMode, "/optgroup");	
			hmStream.SSPrintf(HtmlTagStreamMode, "optgroup label=\"Breakdown to Ip < 0\"");
				if(graphic_select == 3) hmStream.SSPrintf(HtmlTagStreamMode, "option selected=\"selected\" value=3>Primary breakdown to Ip < 0</option");						
				else hmStream.SSPrintf(HtmlTagStreamMode, "option value=3>Primary breakdown to Ip < 0</option");						
				if(graphic_select == 4) hmStream.SSPrintf(HtmlTagStreamMode, "option selected=\"selected\" value=4>Vertical breakdown to Ip < 0</option");
				else hmStream.SSPrintf(HtmlTagStreamMode, "option value=4>Vertical breakdown to Ip < 0</option");
				if(graphic_select == 5) hmStream.SSPrintf(HtmlTagStreamMode, "option selected=\"selected\" value=5>Horizontal breakdown to Ip < 0</option");
				else hmStream.SSPrintf(HtmlTagStreamMode, "option value=5>Horizontal breakdown to Ip < 0</option");
			hmStream.SSPrintf(HtmlTagStreamMode, "/optgroup");											
			hmStream.SSPrintf(HtmlTagStreamMode, "optgroup label=\"Inversion to Ip < 0\"");				
				if(graphic_select == 6) hmStream.SSPrintf(HtmlTagStreamMode, "option selected=\"selected\" value=6>Primary inversion to Ip < 0</option");	
				else hmStream.SSPrintf(HtmlTagStreamMode, "option value=6>Primary inversion to Ip < 0</option");	
				if(graphic_select == 7) hmStream.SSPrintf(HtmlTagStreamMode, "option selected=\"selected\" value=7>Vertical inversion to Ip < 0</option");
				else hmStream.SSPrintf(HtmlTagStreamMode, "option value=7>Vertical inversion to Ip < 0</option");
				if(graphic_select == 8) hmStream.SSPrintf(HtmlTagStreamMode, "option selected=\"selected\" value=8>Horizontal inversion to Ip < 0</option");
				else hmStream.SSPrintf(HtmlTagStreamMode, "option value=8>Horizontal inversion to Ip < 0</option");
			hmStream.SSPrintf(HtmlTagStreamMode, "/optgroup");	
			hmStream.SSPrintf(HtmlTagStreamMode, "optgroup label=\"Inversion to Ip > 0\"");	
				if(graphic_select == 9) hmStream.SSPrintf(HtmlTagStreamMode, "option selected=\"selected\" value=9>Primary inversion to Ip > 0</option");	
				else hmStream.SSPrintf(HtmlTagStreamMode, "option value=9>Primary inversion to Ip > 0</option");	
				if(graphic_select == 10) hmStream.SSPrintf(HtmlTagStreamMode, "option selected=\"selected\" value=10>Vertical inversion to Ip > 0</option");
				else hmStream.SSPrintf(HtmlTagStreamMode, "option value=10>Vertical inversion to Ip > 0</option");
				if(graphic_select == 11) hmStream.SSPrintf(HtmlTagStreamMode, "option selected=\"selected\" value=11>Horizontal inversion to Ip > 0</option");
				else hmStream.SSPrintf(HtmlTagStreamMode, "option value=11>Horizontal inversion to Ip > 0</option");
			hmStream.SSPrintf(HtmlTagStreamMode, "/optgroup");	
			hmStream.SSPrintf(HtmlTagStreamMode, "optgroup label=\"Toroidal (absolute timing)\"");	
				if(graphic_select == 12) hmStream.SSPrintf(HtmlTagStreamMode, "option selected=\"selected\" value=12>Toroidal current</option");	
				else hmStream.SSPrintf(HtmlTagStreamMode, "option value=12>Toroidal current</option");	
			hmStream.SSPrintf(HtmlTagStreamMode, "/optgroup");	
			hmStream.SSPrintf(HtmlTagStreamMode, "optgroup label=\"Puffing (timewindows time)\"");	
				if(graphic_select == 13) hmStream.SSPrintf(HtmlTagStreamMode, "option selected=\"selected\" value=13>Puffing output &#37;, Ip > 0</option");	
				else hmStream.SSPrintf(HtmlTagStreamMode, "option value=13>Puffing output &#37;, Ip > 0</option");	
				if(graphic_select == 14) hmStream.SSPrintf(HtmlTagStreamMode, "option selected=\"selected\" value=14>Puffing output &#37;, Ip < 0</option");
				else hmStream.SSPrintf(HtmlTagStreamMode, "option value=14>Puffing output &#37;, Ip < 0</option");
				if(graphic_select == 15) hmStream.SSPrintf(HtmlTagStreamMode, "option selected=\"selected\" value=15>Feedback, Ip > 0</option");
				else hmStream.SSPrintf(HtmlTagStreamMode, "option value=15>Feedback, Ip > 0</option");
				if(graphic_select == 16) hmStream.SSPrintf(HtmlTagStreamMode, "option selected=\"selected\" value=16>Feedback, Ip < 0</option");
				else hmStream.SSPrintf(HtmlTagStreamMode, "option value=16>Feedback, Ip < 0</option");
			hmStream.SSPrintf(HtmlTagStreamMode, "/optgroup");	
			hmStream.SSPrintf(HtmlTagStreamMode, "optgroup label=\"Puffing (absolute time)\"");	
				if(graphic_select == 17) hmStream.SSPrintf(HtmlTagStreamMode, "option selected=\"selected\" value=17>puffing output (absolute time)</option");	
				else hmStream.SSPrintf(HtmlTagStreamMode, "option value=17>puffing output (absolute time)</option");	
			hmStream.SSPrintf(HtmlTagStreamMode, "/optgroup");	
		hmStream.SSPrintf(HtmlTagStreamMode, "/select");	
	hmStream.SSPrintf(HtmlTagStreamMode, "/div");

	float xmin,ymin,xmax,ymax,majortickx,majorticky;
	int number_of_sub_ticks_x,number_of_sub_ticks_y;
	
	if(graphic_select == 0){
		DualVectorSort(primary_breakdown_vector_size, &primary_breakdown_index_vector[0], &primary_breakdown_data_vector[0]);
		RemoveRepeatedValues(&primary_breakdown_vector_size, &primary_breakdown_index_vector[0], &primary_breakdown_data_vector[0]);
		
		this->graphics_support->OptimalDisplayFromVectors(primary_breakdown_vector_size, &primary_breakdown_index_vector[0], &primary_breakdown_data_vector[0],&xmin,&ymin,&xmax,&ymax,&majortickx,&majorticky,&number_of_sub_ticks_x,&number_of_sub_ticks_y);
		this->graphics_support->GetGraphSVG(&temp_string_1,(char *) this->Name(),primary_breakdown_vector_size,&primary_breakdown_index_vector[0],&primary_breakdown_data_vector[0], 760, 400,xmin,ymin,xmax,ymax,majortickx,majorticky,number_of_sub_ticks_x,number_of_sub_ticks_y, True,True);
		
		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 490px; left: 320px;\"");
			hmStream.SSPrintf(HtmlTagStreamMode, temp_string_1.Buffer());
		hmStream.SSPrintf(HtmlTagStreamMode, "/div");
		
		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 540px; left: 1085px;\"");
			hmStream.SSPrintf(HtmlTagStreamMode, "select NAME=\"form_select_waveform_points_0\" size=\"14\" onchange='this.form.submit()'");
				for (i=0;i<primary_breakdown_vector_size;i++) hmStream.SSPrintf(HtmlTagStreamMode, "option value=%d>%.2f | %.2f</option",i,primary_breakdown_index_vector[i],primary_breakdown_data_vector[i]);
			hmStream.SSPrintf(HtmlTagStreamMode, "/select");	
		hmStream.SSPrintf(HtmlTagStreamMode, "/div");

		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 780px; left: 1085px;\"");
		hmStream.Printf("Delete points");
		if (delete_selected_points_bool) hmStream.SSPrintf(HtmlTagStreamMode, "input type=\"checkbox\" name=\"form_delete_selected_points_bool\" CHECKED"); 
		else hmStream.SSPrintf(HtmlTagStreamMode, "input type=\"checkbox\" name=\"form_delete_selected_points_bool\"");
		hmStream.SSPrintf(HtmlTagStreamMode, "/div");

		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 815px; left: 1105px;\">t =<input type=\"text\" size=\"2\" maxlength=\"7\" name=\"form_insert_point_x_0\" id=\"form_insert_point_x_0\" value=%.2f></div",box_display_point_x);
		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 840px; left: 1105px;\">y=<input type=\"text\" size=\"2\" maxlength=\"7\" name=\"form_insert_point_y_0\" id=\"form_insert_point_y_0\"></div");

		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 864px; left: 1105px;\"");
		hmStream.SSPrintf(HtmlTagStreamMode, "input type=\"submit\" name=\"submit_insert_point_0\" value=\"Add point\"");
		hmStream.SSPrintf(HtmlTagStreamMode, "/div");
		
		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 500px; left: 1085px;\">Max: %.2f A</div",primary_1_p_max_value);
		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 520px; left: 1085px;\">Min: %.2f A</div",primary_1_p_min_value);
	}
	if(graphic_select == 1){
		DualVectorSort(vertical_breakdown_vector_size, &vertical_breakdown_index_vector[0], &vertical_breakdown_data_vector[0]);
		RemoveRepeatedValues(&vertical_breakdown_vector_size, &vertical_breakdown_index_vector[0], &vertical_breakdown_data_vector[0]);
		
		this->graphics_support->OptimalDisplayFromVectors(vertical_breakdown_vector_size, &vertical_breakdown_index_vector[0], &vertical_breakdown_data_vector[0],&xmin,&ymin,&xmax,&ymax,&majortickx,&majorticky,&number_of_sub_ticks_x,&number_of_sub_ticks_y);
		this->graphics_support->GetGraphSVG(&temp_string_1,(char *) this->Name(),vertical_breakdown_vector_size,&vertical_breakdown_index_vector[0],&vertical_breakdown_data_vector[0], 760, 400,xmin,ymin,xmax,ymax,majortickx,majorticky,number_of_sub_ticks_x,number_of_sub_ticks_y, True,True);
		
		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 490px; left: 320px;\"");
			hmStream.SSPrintf(HtmlTagStreamMode, temp_string_1.Buffer());
		hmStream.SSPrintf(HtmlTagStreamMode, "/div");

		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 540px; left: 1085px;\"");
			hmStream.SSPrintf(HtmlTagStreamMode, "select NAME=\"form_select_waveform_points_1\" size=\"14\" onchange='this.form.submit()'");
				for (i=0;i<vertical_breakdown_vector_size;i++) hmStream.SSPrintf(HtmlTagStreamMode, "option value=%d>%.2f | %.2f</option",i,vertical_breakdown_index_vector[i],vertical_breakdown_data_vector[i]);
			hmStream.SSPrintf(HtmlTagStreamMode, "/select");	
		hmStream.SSPrintf(HtmlTagStreamMode, "/div");

		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 780px; left: 1085px;\"");
		hmStream.Printf("Delete points");
		if (delete_selected_points_bool) hmStream.SSPrintf(HtmlTagStreamMode, "input type=\"checkbox\" name=\"form_delete_selected_points_bool\" CHECKED"); 
		else hmStream.SSPrintf(HtmlTagStreamMode, "input type=\"checkbox\" name=\"form_delete_selected_points_bool\"");
		hmStream.SSPrintf(HtmlTagStreamMode, "/div");
	
		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 815px; left: 1105px;\">t =<input type=\"text\" size=\"2\" maxlength=\"7\" name=\"form_insert_point_x_1\" id=\"form_insert_point_x_1\" value=%.2f></div",box_display_point_x);
		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 840px; left: 1105px;\">y=<input type=\"text\" size=\"2\" maxlength=\"7\" name=\"form_insert_point_y_1\" id=\"form_insert_point_y_1\"></div");

		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 864px; left: 1105px;\"");
		hmStream.SSPrintf(HtmlTagStreamMode, "input type=\"submit\" name=\"submit_insert_point_1\" value=\"Add point\"");
		hmStream.SSPrintf(HtmlTagStreamMode, "/div");
		
		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 500px; left: 1085px;\">Max: %.2f A</div",vertical_1_p_max_value);
		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 520px; left: 1085px;\">Min: %.2f A</div",vertical_1_p_min_value);
	}
	if(graphic_select == 2){
		DualVectorSort(horizontal_breakdown_vector_size, &horizontal_breakdown_index_vector[0], &horizontal_breakdown_data_vector[0]);
		RemoveRepeatedValues(&horizontal_breakdown_vector_size, &horizontal_breakdown_index_vector[0], &horizontal_breakdown_data_vector[0]);
		
		this->graphics_support->OptimalDisplayFromVectors(horizontal_breakdown_vector_size, &horizontal_breakdown_index_vector[0], &horizontal_breakdown_data_vector[0],&xmin,&ymin,&xmax,&ymax,&majortickx,&majorticky,&number_of_sub_ticks_x,&number_of_sub_ticks_y);
		this->graphics_support->GetGraphSVG(&temp_string_1,(char *) this->Name(),horizontal_breakdown_vector_size,&horizontal_breakdown_index_vector[0],&horizontal_breakdown_data_vector[0], 760, 400,xmin,ymin,xmax,ymax,majortickx,majorticky,number_of_sub_ticks_x,number_of_sub_ticks_y, True,True);
		
		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 490px; left: 320px;\"");
			hmStream.SSPrintf(HtmlTagStreamMode, temp_string_1.Buffer());
		hmStream.SSPrintf(HtmlTagStreamMode, "/div");

		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 540px; left: 1085px;\"");
			hmStream.SSPrintf(HtmlTagStreamMode, "select NAME=\"form_select_waveform_points_2\" size=\"14\" onchange='this.form.submit()'");
				for (i=0;i<horizontal_breakdown_vector_size;i++) hmStream.SSPrintf(HtmlTagStreamMode, "option value=%d>%.2f | %.2f</option",i,horizontal_breakdown_index_vector[i],horizontal_breakdown_data_vector[i]);
			hmStream.SSPrintf(HtmlTagStreamMode, "/select");	
		hmStream.SSPrintf(HtmlTagStreamMode, "/div");

		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 780px; left: 1085px;\"");
		hmStream.Printf("Delete points");
		if (delete_selected_points_bool) hmStream.SSPrintf(HtmlTagStreamMode, "input type=\"checkbox\" name=\"form_delete_selected_points_bool\" CHECKED"); 
		else hmStream.SSPrintf(HtmlTagStreamMode, "input type=\"checkbox\" name=\"form_delete_selected_points_bool\"");
		hmStream.SSPrintf(HtmlTagStreamMode, "/div");

		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 815px; left: 1105px;\">t =<input type=\"text\" size=\"2\" maxlength=\"7\" name=\"form_insert_point_x_2\" id=\"form_insert_point_x_2\" value=%.2f></div",box_display_point_x);
		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 840px; left: 1105px;\">y=<input type=\"text\" size=\"2\" maxlength=\"7\" name=\"form_insert_point_y_2\" id=\"form_insert_point_y_2\"></div");

		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 864px; left: 1105px;\"");
		hmStream.SSPrintf(HtmlTagStreamMode, "input type=\"submit\" name=\"submit_insert_point_2\" value=\"Add point\"");
		hmStream.SSPrintf(HtmlTagStreamMode, "/div");
		
		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 500px; left: 1085px;\">Max: %.2f A</div",horizontal_1_p_max_value);
		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 520px; left: 1085px;\">Min: %.2f A</div",horizontal_1_p_min_value);
	}
	if(graphic_select == 3){
		DualVectorSort(primary_breakdown_negative_vector_size, &primary_breakdown_negative_index_vector[0], &primary_breakdown_negative_data_vector[0]);
		RemoveRepeatedValues(&primary_breakdown_negative_vector_size, &primary_breakdown_negative_index_vector[0], &primary_breakdown_negative_data_vector[0]);
		
		this->graphics_support->OptimalDisplayFromVectors(primary_breakdown_negative_vector_size, &primary_breakdown_negative_index_vector[0], &primary_breakdown_negative_data_vector[0],&xmin,&ymin,&xmax,&ymax,&majortickx,&majorticky,&number_of_sub_ticks_x,&number_of_sub_ticks_y);
		this->graphics_support->GetGraphSVG(&temp_string_1,(char *) this->Name(),primary_breakdown_negative_vector_size,&primary_breakdown_negative_index_vector[0],&primary_breakdown_negative_data_vector[0], 760, 400,xmin,ymin,xmax,ymax,majortickx,majorticky,number_of_sub_ticks_x,number_of_sub_ticks_y, True,True);
		
		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 490px; left: 320px;\"");
			hmStream.SSPrintf(HtmlTagStreamMode, temp_string_1.Buffer());
		hmStream.SSPrintf(HtmlTagStreamMode, "/div");

		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 540px; left: 1085px;\"");
			hmStream.SSPrintf(HtmlTagStreamMode, "select NAME=\"form_select_waveform_points_6\" size=\"14\" onchange='this.form.submit()'");
				for (i=0;i<primary_breakdown_negative_vector_size;i++) hmStream.SSPrintf(HtmlTagStreamMode, "option value=%d>%.2f | %.2f</option",i,primary_breakdown_negative_index_vector[i],primary_breakdown_negative_data_vector[i]);
			hmStream.SSPrintf(HtmlTagStreamMode, "/select");	
		hmStream.SSPrintf(HtmlTagStreamMode, "/div");

		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 780px; left: 1085px;\"");
		hmStream.Printf("Delete points");
		if (delete_selected_points_bool) hmStream.SSPrintf(HtmlTagStreamMode, "input type=\"checkbox\" name=\"form_delete_selected_points_bool\" CHECKED"); 
		else hmStream.SSPrintf(HtmlTagStreamMode, "input type=\"checkbox\" name=\"form_delete_selected_points_bool\"");
		hmStream.SSPrintf(HtmlTagStreamMode, "/div");

		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 815px; left: 1105px;\">t =<input type=\"text\" size=\"2\" maxlength=\"7\" name=\"form_insert_point_x_3\" id=\"form_insert_point_x_3\" value=%.2f></div",box_display_point_x);
		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 840px; left: 1105px;\">y=<input type=\"text\" size=\"2\" maxlength=\"7\" name=\"form_insert_point_y_3\" id=\"form_insert_point_y_3\"></div");

		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 864px; left: 1105px;\"");
		hmStream.SSPrintf(HtmlTagStreamMode, "input type=\"submit\" name=\"submit_insert_point_3\" value=\"Add point\"");
		hmStream.SSPrintf(HtmlTagStreamMode, "/div");
		
		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 500px; left: 1085px;\">Max: %.2f A</div",primary_1_p_max_value);
		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 520px; left: 1085px;\">Min: %.2f A</div",primary_1_p_min_value);
	}
	if(graphic_select == 4){
		DualVectorSort(vertical_breakdown_negative_vector_size, &vertical_breakdown_negative_index_vector[0], &vertical_breakdown_negative_data_vector[0]);
		RemoveRepeatedValues(&vertical_breakdown_negative_vector_size, &vertical_breakdown_negative_index_vector[0], &vertical_breakdown_negative_data_vector[0]);
		
		this->graphics_support->OptimalDisplayFromVectors(vertical_breakdown_negative_vector_size, &vertical_breakdown_negative_index_vector[0], &vertical_breakdown_negative_data_vector[0],&xmin,&ymin,&xmax,&ymax,&majortickx,&majorticky,&number_of_sub_ticks_x,&number_of_sub_ticks_y);
		this->graphics_support->GetGraphSVG(&temp_string_1,(char *) this->Name(),vertical_breakdown_negative_vector_size,&vertical_breakdown_negative_index_vector[0],&vertical_breakdown_negative_data_vector[0], 760, 400,xmin,ymin,xmax,ymax,majortickx,majorticky,number_of_sub_ticks_x,number_of_sub_ticks_y, True,True);
		
		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 490px; left: 320px;\"");
			hmStream.SSPrintf(HtmlTagStreamMode, temp_string_1.Buffer());
		hmStream.SSPrintf(HtmlTagStreamMode, "/div");

		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 540px; left: 1085px;\"");
			hmStream.SSPrintf(HtmlTagStreamMode, "select NAME=\"form_select_waveform_points_7\" size=\"14\" onchange='this.form.submit()'");
				for (i=0;i<vertical_breakdown_negative_vector_size;i++) hmStream.SSPrintf(HtmlTagStreamMode, "option value=%d>%.2f | %.2f</option",i,vertical_breakdown_negative_index_vector[i],vertical_breakdown_negative_data_vector[i]);
			hmStream.SSPrintf(HtmlTagStreamMode, "/select");	
		hmStream.SSPrintf(HtmlTagStreamMode, "/div");

		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 780px; left: 1085px;\"");
		hmStream.Printf("Delete points");
		if (delete_selected_points_bool) hmStream.SSPrintf(HtmlTagStreamMode, "input type=\"checkbox\" name=\"form_delete_selected_points_bool\" CHECKED"); 
		else hmStream.SSPrintf(HtmlTagStreamMode, "input type=\"checkbox\" name=\"form_delete_selected_points_bool\"");
		hmStream.SSPrintf(HtmlTagStreamMode, "/div");

		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 815px; left: 1105px;\">t =<input type=\"text\" size=\"2\" maxlength=\"7\" name=\"form_insert_point_x_4\" id=\"form_insert_point_x_4\" value=%.2f></div",box_display_point_x);
		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 840px; left: 1105px;\">y=<input type=\"text\" size=\"2\" maxlength=\"7\" name=\"form_insert_point_y_4\" id=\"form_insert_point_y_4\"></div");

		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 864px; left: 1105px;\"");
		hmStream.SSPrintf(HtmlTagStreamMode, "input type=\"submit\" name=\"submit_insert_point_4\" value=\"Add point\"");
		hmStream.SSPrintf(HtmlTagStreamMode, "/div");
		
		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 500px; left: 1085px;\">Max: %.2f A</div",vertical_1_p_max_value);
		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 520px; left: 1085px;\">Min: %.2f A</div",vertical_1_p_min_value);
	}
	if(graphic_select == 5){
		DualVectorSort(horizontal_breakdown_negative_vector_size, &horizontal_breakdown_negative_index_vector[0], &horizontal_breakdown_negative_data_vector[0]);
		RemoveRepeatedValues(&horizontal_breakdown_negative_vector_size, &horizontal_breakdown_negative_index_vector[0], &horizontal_breakdown_negative_data_vector[0]);
		
		this->graphics_support->OptimalDisplayFromVectors(horizontal_breakdown_negative_vector_size, &horizontal_breakdown_negative_index_vector[0], &horizontal_breakdown_negative_data_vector[0],&xmin,&ymin,&xmax,&ymax,&majortickx,&majorticky,&number_of_sub_ticks_x,&number_of_sub_ticks_y);
		this->graphics_support->GetGraphSVG(&temp_string_1,(char *) this->Name(),horizontal_breakdown_negative_vector_size,&horizontal_breakdown_negative_index_vector[0],&horizontal_breakdown_negative_data_vector[0], 760, 400,xmin,ymin,xmax,ymax,majortickx,majorticky,number_of_sub_ticks_x,number_of_sub_ticks_y, True,True);
		
		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 490px; left: 320px;\"");
			hmStream.SSPrintf(HtmlTagStreamMode, temp_string_1.Buffer());
		hmStream.SSPrintf(HtmlTagStreamMode, "/div");

		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 540px; left: 1085px;\"");
			hmStream.SSPrintf(HtmlTagStreamMode, "select NAME=\"form_select_waveform_points_8\" size=\"14\" onchange='this.form.submit()'");
				for (i=0;i<horizontal_breakdown_negative_vector_size;i++) hmStream.SSPrintf(HtmlTagStreamMode, "option value=%d>%.2f | %.2f</option",i,horizontal_breakdown_negative_index_vector[i],horizontal_breakdown_negative_data_vector[i]);
			hmStream.SSPrintf(HtmlTagStreamMode, "/select");	
		hmStream.SSPrintf(HtmlTagStreamMode, "/div");

		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 780px; left: 1085px;\"");
		hmStream.Printf("Delete points");
		if (delete_selected_points_bool) hmStream.SSPrintf(HtmlTagStreamMode, "input type=\"checkbox\" name=\"form_delete_selected_points_bool\" CHECKED"); 
		else hmStream.SSPrintf(HtmlTagStreamMode, "input type=\"checkbox\" name=\"form_delete_selected_points_bool\"");
		hmStream.SSPrintf(HtmlTagStreamMode, "/div");

		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 815px; left: 1105px;\">t =<input type=\"text\" size=\"2\" maxlength=\"7\" name=\"form_insert_point_x_5\" id=\"form_insert_point_x_5\" value=%.2f></div",box_display_point_x);
		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 840px; left: 1105px;\">y=<input type=\"text\" size=\"2\" maxlength=\"7\" name=\"form_insert_point_y_5\" id=\"form_insert_point_y_5\"></div");

		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 864px; left: 1105px;\"");
		hmStream.SSPrintf(HtmlTagStreamMode, "input type=\"submit\" name=\"submit_insert_point_5\" value=\"Add point\"");
		hmStream.SSPrintf(HtmlTagStreamMode, "/div");
		
		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 500px; left: 1085px;\">Max: %.2f A</div",horizontal_1_p_max_value);
		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 520px; left: 1085px;\">Min: %.2f A</div",horizontal_1_p_min_value);
	}
	if(graphic_select == 6){
		DualVectorSort(primary_inversion_positive_vector_size, &primary_inversion_positive_index_vector[0], &primary_inversion_positive_data_vector[0]);
		RemoveRepeatedValues(&primary_inversion_positive_vector_size, &primary_inversion_positive_index_vector[0], &primary_inversion_positive_data_vector[0]);
		
		this->graphics_support->OptimalDisplayFromVectors(primary_inversion_positive_vector_size, &primary_inversion_positive_index_vector[0], &primary_inversion_positive_data_vector[0],&xmin,&ymin,&xmax,&ymax,&majortickx,&majorticky,&number_of_sub_ticks_x,&number_of_sub_ticks_y);
		this->graphics_support->GetGraphSVG(&temp_string_1,(char *) this->Name(),primary_inversion_positive_vector_size,&primary_inversion_positive_index_vector[0],&primary_inversion_positive_data_vector[0], 760, 400,xmin,ymin,xmax,ymax,majortickx,majorticky,number_of_sub_ticks_x,number_of_sub_ticks_y, True,True);
		
		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 490px; left: 320px;\"");
			hmStream.SSPrintf(HtmlTagStreamMode, temp_string_1.Buffer());
		hmStream.SSPrintf(HtmlTagStreamMode, "/div");

		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 540px; left: 1085px;\"");
			hmStream.SSPrintf(HtmlTagStreamMode, "select NAME=\"form_select_waveform_points_3\" size=\"14\" onchange='this.form.submit()'");
				for (i=0;i<primary_inversion_positive_vector_size;i++) hmStream.SSPrintf(HtmlTagStreamMode, "option value=%d>%.2f | %.2f</option",i,primary_inversion_positive_index_vector[i],primary_inversion_positive_data_vector[i]);
			hmStream.SSPrintf(HtmlTagStreamMode, "/select");	
		hmStream.SSPrintf(HtmlTagStreamMode, "/div");

		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 780px; left: 1085px;\"");
		hmStream.Printf("Delete points");
		if (delete_selected_points_bool) hmStream.SSPrintf(HtmlTagStreamMode, "input type=\"checkbox\" name=\"form_delete_selected_points_bool\" CHECKED"); 
		else hmStream.SSPrintf(HtmlTagStreamMode, "input type=\"checkbox\" name=\"form_delete_selected_points_bool\"");
		hmStream.SSPrintf(HtmlTagStreamMode, "/div");

		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 815px; left: 1105px;\">t =<input type=\"text\" size=\"2\" maxlength=\"6\" name=\"form_insert_point_x_6\" id=\"form_insert_point_x_6\" value=%.2f></div",box_display_point_x);
		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 840px; left: 1105px;\">y=<input type=\"text\" size=\"2\" maxlength=\"6\" name=\"form_insert_point_y_6\" id=\"form_insert_point_y_6\"></div");

		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 864px; left: 1105px;\"");
		hmStream.SSPrintf(HtmlTagStreamMode, "input type=\"submit\" name=\"submit_insert_point_6\" value=\"Add point\"");
		hmStream.SSPrintf(HtmlTagStreamMode, "/div");
		
		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 500px; left: 1085px;\">Max: %.2f A</div",primary_1_p_max_value);
		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 520px; left: 1085px;\">Min: %.2f A</div",primary_1_p_min_value);
	}
	if(graphic_select == 7){
		DualVectorSort(vertical_inversion_positive_vector_size, &vertical_inversion_positive_index_vector[0], &vertical_inversion_positive_data_vector[0]);
		RemoveRepeatedValues(&vertical_inversion_positive_vector_size, &vertical_inversion_positive_index_vector[0], &vertical_inversion_positive_data_vector[0]);
		
		this->graphics_support->OptimalDisplayFromVectors(vertical_inversion_positive_vector_size, &vertical_inversion_positive_index_vector[0], &vertical_inversion_positive_data_vector[0],&xmin,&ymin,&xmax,&ymax,&majortickx,&majorticky,&number_of_sub_ticks_x,&number_of_sub_ticks_y);
		this->graphics_support->GetGraphSVG(&temp_string_1,(char *) this->Name(),vertical_inversion_positive_vector_size,&vertical_inversion_positive_index_vector[0],&vertical_inversion_positive_data_vector[0], 760, 400,xmin,ymin,xmax,ymax,majortickx,majorticky,number_of_sub_ticks_x,number_of_sub_ticks_y, True,True);
		
		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 490px; left: 320px;\"");
			hmStream.SSPrintf(HtmlTagStreamMode, temp_string_1.Buffer());
		hmStream.SSPrintf(HtmlTagStreamMode, "/div");

		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 540px; left: 1085px;\"");
			hmStream.SSPrintf(HtmlTagStreamMode, "select NAME=\"form_select_waveform_points_4\" size=\"14\" onchange='this.form.submit()'");
				for (i=0;i<vertical_inversion_positive_vector_size;i++) hmStream.SSPrintf(HtmlTagStreamMode, "option value=%d>%.2f | %.2f</option",i,vertical_inversion_positive_index_vector[i],vertical_inversion_positive_data_vector[i]);
			hmStream.SSPrintf(HtmlTagStreamMode, "/select");	
		hmStream.SSPrintf(HtmlTagStreamMode, "/div");

		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 780px; left: 1085px;\"");
		hmStream.Printf("Delete points");
		if (delete_selected_points_bool) hmStream.SSPrintf(HtmlTagStreamMode, "input type=\"checkbox\" name=\"form_delete_selected_points_bool\" CHECKED"); 
		else hmStream.SSPrintf(HtmlTagStreamMode, "input type=\"checkbox\" name=\"form_delete_selected_points_bool\"");
		hmStream.SSPrintf(HtmlTagStreamMode, "/div");

		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 815px; left: 1105px;\">t =<input type=\"text\" size=\"2\" maxlength=\"7\" name=\"form_insert_point_x_7\" id=\"form_insert_point_x_7\" value=%.2f></div",box_display_point_x);
		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 840px; left: 1105px;\">y=<input type=\"text\" size=\"2\" maxlength=\"7\" name=\"form_insert_point_y_7\" id=\"form_insert_point_y_7\"></div");

		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 864px; left: 1105px;\"");
		hmStream.SSPrintf(HtmlTagStreamMode, "input type=\"submit\" name=\"submit_insert_point_7\" value=\"Add point\"");
		hmStream.SSPrintf(HtmlTagStreamMode, "/div");
		
		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 500px; left: 1085px;\">Max: %.2f A</div",vertical_1_p_max_value);
		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 520px; left: 1085px;\">Min: %.2f A</div",vertical_1_p_min_value);
	}
	if(graphic_select == 8){
		DualVectorSort(horizontal_inversion_positive_vector_size, &horizontal_inversion_positive_index_vector[0], &horizontal_inversion_positive_data_vector[0]);
		RemoveRepeatedValues(&horizontal_inversion_positive_vector_size, &horizontal_inversion_positive_index_vector[0], &horizontal_inversion_positive_data_vector[0]);
		
		this->graphics_support->OptimalDisplayFromVectors(horizontal_inversion_positive_vector_size, &horizontal_inversion_positive_index_vector[0], &horizontal_inversion_positive_data_vector[0],&xmin,&ymin,&xmax,&ymax,&majortickx,&majorticky,&number_of_sub_ticks_x,&number_of_sub_ticks_y);
		this->graphics_support->GetGraphSVG(&temp_string_1,(char *) this->Name(),horizontal_inversion_positive_vector_size,&horizontal_inversion_positive_index_vector[0],&horizontal_inversion_positive_data_vector[0], 760, 400,xmin,ymin,xmax,ymax,majortickx,majorticky,number_of_sub_ticks_x,number_of_sub_ticks_y, True,True);
		
		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 490px; left: 320px;\"");
			hmStream.SSPrintf(HtmlTagStreamMode, temp_string_1.Buffer());
		hmStream.SSPrintf(HtmlTagStreamMode, "/div");

		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 540px; left: 1085px;\"");
			hmStream.SSPrintf(HtmlTagStreamMode, "select NAME=\"form_select_waveform_points_5\" size=\"14\" onchange='this.form.submit()'");
				for (i=0;i<horizontal_inversion_positive_vector_size;i++) hmStream.SSPrintf(HtmlTagStreamMode, "option value=%d>%.2f | %.2f</option",i,horizontal_inversion_positive_index_vector[i],horizontal_inversion_positive_data_vector[i]);
			hmStream.SSPrintf(HtmlTagStreamMode, "/select");	
		hmStream.SSPrintf(HtmlTagStreamMode, "/div");

		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 780px; left: 1085px;\"");
		hmStream.Printf("Delete points");
		if (delete_selected_points_bool) hmStream.SSPrintf(HtmlTagStreamMode, "input type=\"checkbox\" name=\"form_delete_selected_points_bool\" CHECKED"); 
		else hmStream.SSPrintf(HtmlTagStreamMode, "input type=\"checkbox\" name=\"form_delete_selected_points_bool\"");
		hmStream.SSPrintf(HtmlTagStreamMode, "/div");

		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 815px; left: 1105px;\">t =<input type=\"text\" size=\"2\" maxlength=\"7\" name=\"form_insert_point_x_8\" id=\"form_insert_point_x_8\" value=%.2f></div",box_display_point_x);
		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 840px; left: 1105px;\">y=<input type=\"text\" size=\"2\" maxlength=\"7\" name=\"form_insert_point_y_8\" id=\"form_insert_point_y_8\"></div");

		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 864px; left: 1105px;\"");
		hmStream.SSPrintf(HtmlTagStreamMode, "input type=\"submit\" name=\"submit_insert_point_8\" value=\"Add point\"");
		hmStream.SSPrintf(HtmlTagStreamMode, "/div");
		
		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 500px; left: 1085px;\">Max: %.2f A</div",horizontal_1_p_max_value);
		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 520px; left: 1085px;\">Min: %.2f A</div",horizontal_1_p_min_value);
	}
	if(graphic_select == 9){
		DualVectorSort(primary_inversion_negative_vector_size, &primary_inversion_negative_index_vector[0], &primary_inversion_negative_data_vector[0]);
		RemoveRepeatedValues(&primary_inversion_negative_vector_size, &primary_inversion_negative_index_vector[0], &primary_inversion_negative_data_vector[0]);
		
		this->graphics_support->OptimalDisplayFromVectors(primary_inversion_negative_vector_size, &primary_inversion_negative_index_vector[0], &primary_inversion_negative_data_vector[0],&xmin,&ymin,&xmax,&ymax,&majortickx,&majorticky,&number_of_sub_ticks_x,&number_of_sub_ticks_y);
		this->graphics_support->GetGraphSVG(&temp_string_1,(char *) this->Name(),primary_inversion_negative_vector_size,&primary_inversion_negative_index_vector[0],&primary_inversion_negative_data_vector[0], 760, 400,xmin,ymin,xmax,ymax,majortickx,majorticky,number_of_sub_ticks_x,number_of_sub_ticks_y, True,True);
		
		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 490px; left: 320px;\"");
			hmStream.SSPrintf(HtmlTagStreamMode, temp_string_1.Buffer());
		hmStream.SSPrintf(HtmlTagStreamMode, "/div");

		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 540px; left: 1085px;\"");
			hmStream.SSPrintf(HtmlTagStreamMode, "select NAME=\"form_select_waveform_points_9\" size=\"14\" onchange='this.form.submit()'");
				for (i=0;i<primary_inversion_negative_vector_size;i++) hmStream.SSPrintf(HtmlTagStreamMode, "option value=%d>%.2f | %.2f</option",i,primary_inversion_negative_index_vector[i],primary_inversion_negative_data_vector[i]);
			hmStream.SSPrintf(HtmlTagStreamMode, "/select");	
		hmStream.SSPrintf(HtmlTagStreamMode, "/div");

		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 780px; left: 1085px;\"");
		hmStream.Printf("Delete points");
		if (delete_selected_points_bool) hmStream.SSPrintf(HtmlTagStreamMode, "input type=\"checkbox\" name=\"form_delete_selected_points_bool\" CHECKED"); 
		else hmStream.SSPrintf(HtmlTagStreamMode, "input type=\"checkbox\" name=\"form_delete_selected_points_bool\"");
		hmStream.SSPrintf(HtmlTagStreamMode, "/div");

		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 815px; left: 1105px;\">t =<input type=\"text\" size=\"2\" maxlength=\"7\" name=\"form_insert_point_x_9\" id=\"form_insert_point_x_9\" value=%.2f></div",box_display_point_x);
		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 840px; left: 1105px;\">y=<input type=\"text\" size=\"2\" maxlength=\"7\" name=\"form_insert_point_y_9\" id=\"form_insert_point_y_9\"></div");

		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 864px; left: 1105px;\"");
		hmStream.SSPrintf(HtmlTagStreamMode, "input type=\"submit\" name=\"submit_insert_point_9\" value=\"Add point\"");
		hmStream.SSPrintf(HtmlTagStreamMode, "/div");
		
		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 500px; left: 1085px;\">Max: %.2f A</div",primary_1_p_max_value);
		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 520px; left: 1085px;\">Min: %.2f A</div",primary_1_p_min_value);
	}
	if(graphic_select == 10){
		DualVectorSort(vertical_inversion_negative_vector_size, &vertical_inversion_negative_index_vector[0], &vertical_inversion_negative_data_vector[0]);
		RemoveRepeatedValues(&vertical_inversion_negative_vector_size, &vertical_inversion_negative_index_vector[0], &vertical_inversion_negative_data_vector[0]);
		
		this->graphics_support->OptimalDisplayFromVectors(vertical_inversion_negative_vector_size, &vertical_inversion_negative_index_vector[0], &vertical_inversion_negative_data_vector[0],&xmin,&ymin,&xmax,&ymax,&majortickx,&majorticky,&number_of_sub_ticks_x,&number_of_sub_ticks_y);
		this->graphics_support->GetGraphSVG(&temp_string_1,(char *) this->Name(),vertical_inversion_negative_vector_size,&vertical_inversion_negative_index_vector[0],&vertical_inversion_negative_data_vector[0], 760, 400,xmin,ymin,xmax,ymax,majortickx,majorticky,number_of_sub_ticks_x,number_of_sub_ticks_y, True,True);
		
		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 490px; left: 320px;\"");
			hmStream.SSPrintf(HtmlTagStreamMode, temp_string_1.Buffer());
		hmStream.SSPrintf(HtmlTagStreamMode, "/div");

		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 540px; left: 1085px;\"");
			hmStream.SSPrintf(HtmlTagStreamMode, "select NAME=\"form_select_waveform_points_10\" size=\"14\" onchange='this.form.submit()'");
				for (i=0;i<vertical_inversion_negative_vector_size;i++) hmStream.SSPrintf(HtmlTagStreamMode, "option value=%d>%.2f | %.2f</option",i,vertical_inversion_negative_index_vector[i],vertical_inversion_negative_data_vector[i]);
			hmStream.SSPrintf(HtmlTagStreamMode, "/select");	
		hmStream.SSPrintf(HtmlTagStreamMode, "/div");

		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 780px; left: 1085px;\"");
		hmStream.Printf("Delete points");
		if (delete_selected_points_bool) hmStream.SSPrintf(HtmlTagStreamMode, "input type=\"checkbox\" name=\"form_delete_selected_points_bool\" CHECKED"); 
		else hmStream.SSPrintf(HtmlTagStreamMode, "input type=\"checkbox\" name=\"form_delete_selected_points_bool\"");
		hmStream.SSPrintf(HtmlTagStreamMode, "/div");

		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 815px; left: 1105px;\">t =<input type=\"text\" size=\"2\" maxlength=\"7\" name=\"form_insert_point_x_10\" id=\"form_insert_point_x_10\" value=%.2f></div",box_display_point_x);
		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 840px; left: 1105px;\">y=<input type=\"text\" size=\"2\" maxlength=\"7\" name=\"form_insert_point_y_10\" id=\"form_insert_point_y_10\"></div");

		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 864px; left: 1105px;\"");
		hmStream.SSPrintf(HtmlTagStreamMode, "input type=\"submit\" name=\"submit_insert_point_10\" value=\"Add point\"");
		hmStream.SSPrintf(HtmlTagStreamMode, "/div");

		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 500px; left: 1085px;\">Max: %.2f A</div",vertical_1_p_max_value);
		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 520px; left: 1085px;\">Min: %.2f A</div",vertical_1_p_min_value);
	}
	if(graphic_select == 11){
		DualVectorSort(horizontal_inversion_negative_vector_size, &horizontal_inversion_negative_index_vector[0], &horizontal_inversion_negative_data_vector[0]);
		RemoveRepeatedValues(&horizontal_inversion_negative_vector_size, &horizontal_inversion_negative_index_vector[0], &horizontal_inversion_negative_data_vector[0]);
		
		this->graphics_support->OptimalDisplayFromVectors(horizontal_inversion_negative_vector_size, &horizontal_inversion_negative_index_vector[0], &horizontal_inversion_negative_data_vector[0],&xmin,&ymin,&xmax,&ymax,&majortickx,&majorticky,&number_of_sub_ticks_x,&number_of_sub_ticks_y);
		this->graphics_support->GetGraphSVG(&temp_string_1,(char *) this->Name(),horizontal_inversion_negative_vector_size,&horizontal_inversion_negative_index_vector[0],&horizontal_inversion_negative_data_vector[0], 760, 400,xmin,ymin,xmax,ymax,majortickx,majorticky,number_of_sub_ticks_x,number_of_sub_ticks_y, True,True);
		
		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 490px; left: 320px;\"");
			hmStream.SSPrintf(HtmlTagStreamMode, temp_string_1.Buffer());
		hmStream.SSPrintf(HtmlTagStreamMode, "/div");

		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 540px; left: 1085px;\"");
			hmStream.SSPrintf(HtmlTagStreamMode, "select NAME=\"form_select_waveform_points_11\" size=\"14\" onchange='this.form.submit()'");
				for (i=0;i<horizontal_inversion_negative_vector_size;i++) hmStream.SSPrintf(HtmlTagStreamMode, "option value=%d>%.2f | %.2f</option",i,horizontal_inversion_negative_index_vector[i],horizontal_inversion_negative_data_vector[i]);
			hmStream.SSPrintf(HtmlTagStreamMode, "/select");	
		hmStream.SSPrintf(HtmlTagStreamMode, "/div");

		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 780px; left: 1085px;\"");
		hmStream.Printf("Delete points");
		if (delete_selected_points_bool) hmStream.SSPrintf(HtmlTagStreamMode, "input type=\"checkbox\" name=\"form_delete_selected_points_bool\" CHECKED"); 
		else hmStream.SSPrintf(HtmlTagStreamMode, "input type=\"checkbox\" name=\"form_delete_selected_points_bool\"");
		hmStream.SSPrintf(HtmlTagStreamMode, "/div");

		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 815px; left: 1105px;\">t =<input type=\"text\" size=\"2\" maxlength=\"7\" name=\"form_insert_point_x_11\" id=\"form_insert_point_x_11\" value=%.2f></div",box_display_point_x);
		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 840px; left: 1105px;\">y=<input type=\"text\" size=\"2\" maxlength=\"7\" name=\"form_insert_point_y_11\" id=\"form_insert_point_y_11\"></div");

		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 864px; left: 1105px;\"");
		hmStream.SSPrintf(HtmlTagStreamMode, "input type=\"submit\" name=\"submit_insert_point_11\" value=\"Add point\"");
		hmStream.SSPrintf(HtmlTagStreamMode, "/div");

		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 500px; left: 1085px;\">Max: %.2f A</div",horizontal_1_p_max_value);
		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 520px; left: 1085px;\">Min: %.2f A</div",horizontal_1_p_min_value);
	}
	if(graphic_select == 12){
		DualVectorSort(toroidal_1_p_vector_size, &toroidal_1_p_index_vector[0], &toroidal_1_p_data_vector[0]);
		RemoveRepeatedValues(&toroidal_1_p_vector_size, &toroidal_1_p_index_vector[0], &toroidal_1_p_data_vector[0]);
		
		this->graphics_support->OptimalDisplayFromVectors(toroidal_1_p_vector_size, &toroidal_1_p_index_vector[0], &toroidal_1_p_data_vector[0],&xmin,&ymin,&xmax,&ymax,&majortickx,&majorticky,&number_of_sub_ticks_x,&number_of_sub_ticks_y);
		this->graphics_support->GetGraphSVG(&temp_string_1,(char *) this->Name(),toroidal_1_p_vector_size,&toroidal_1_p_index_vector[0],&toroidal_1_p_data_vector[0], 760, 400,xmin,ymin,xmax,ymax,majortickx,majorticky,number_of_sub_ticks_x,number_of_sub_ticks_y, True,True);
		
		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 490px; left: 320px;\"");
			hmStream.SSPrintf(HtmlTagStreamMode, temp_string_1.Buffer());
		hmStream.SSPrintf(HtmlTagStreamMode, "/div");

		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 540px; left: 1085px;\"");
			hmStream.SSPrintf(HtmlTagStreamMode, "select NAME=\"form_select_waveform_points_12\" size=\"14\" onchange='this.form.submit()'");
				for (i=0;i<toroidal_1_p_vector_size;i++) hmStream.SSPrintf(HtmlTagStreamMode, "option value=%d>%.2f | %.2f</option",i,toroidal_1_p_index_vector[i],toroidal_1_p_data_vector[i]);
			hmStream.SSPrintf(HtmlTagStreamMode, "/select");	
		hmStream.SSPrintf(HtmlTagStreamMode, "/div");

		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 780px; left: 1085px;\"");
		hmStream.Printf("Delete points");
		if (delete_selected_points_bool) hmStream.SSPrintf(HtmlTagStreamMode, "input type=\"checkbox\" name=\"form_delete_selected_points_bool\" CHECKED"); 
		else hmStream.SSPrintf(HtmlTagStreamMode, "input type=\"checkbox\" name=\"form_delete_selected_points_bool\"");
		hmStream.SSPrintf(HtmlTagStreamMode, "/div");

		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 815px; left: 1105px;\">t =<input type=\"text\" size=\"2\" maxlength=\"7\" name=\"form_insert_point_x_12\" id=\"form_insert_point_x_12\" value=%.2f></div",box_display_point_x);
		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 840px; left: 1105px;\">y=<input type=\"text\" size=\"2\" maxlength=\"7\" name=\"form_insert_point_y_12\" id=\"form_insert_point_y_12\"></div");

		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 864px; left: 1105px;\"");
		hmStream.SSPrintf(HtmlTagStreamMode, "input type=\"submit\" name=\"submit_insert_point_12\" value=\"Add point\"");
		hmStream.SSPrintf(HtmlTagStreamMode, "/div");

		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 500px; left: 1085px;\">Max: %.1f A</div",toroidal_1_p_max_value);
		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 520px; left: 1085px;\">Min: %.1f A</div",toroidal_1_p_min_value);
	}
	if(graphic_select == 13){
		DualVectorSort(puffing_1_p_vector_size, &puffing_1_p_index_vector[0], &puffing_1_p_data_vector[0]);
		RemoveRepeatedValues(&puffing_1_p_vector_size, &puffing_1_p_index_vector[0], &puffing_1_p_data_vector[0]);
		
		this->graphics_support->OptimalDisplayFromVectors(puffing_1_p_vector_size, &puffing_1_p_index_vector[0], &puffing_1_p_data_vector[0],&xmin,&ymin,&xmax,&ymax,&majortickx,&majorticky,&number_of_sub_ticks_x,&number_of_sub_ticks_y);
		this->graphics_support->GetGraphSVG(&temp_string_1,(char *) this->Name(),puffing_1_p_vector_size,&puffing_1_p_index_vector[0],&puffing_1_p_data_vector[0], 760, 400,xmin,ymin,xmax,ymax,majortickx,majorticky,number_of_sub_ticks_x,number_of_sub_ticks_y, True,True);
		
		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 490px; left: 320px;\"");
			hmStream.SSPrintf(HtmlTagStreamMode, temp_string_1.Buffer());
		hmStream.SSPrintf(HtmlTagStreamMode, "/div");

		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 540px; left: 1085px;\"");
			hmStream.SSPrintf(HtmlTagStreamMode, "select NAME=\"form_select_waveform_points_13\" size=\"14\" onchange='this.form.submit()'");
				for (i=0;i<puffing_1_p_vector_size;i++) hmStream.SSPrintf(HtmlTagStreamMode, "option value=%d>%.2f | %.2f</option",i,puffing_1_p_index_vector[i],puffing_1_p_data_vector[i]);
			hmStream.SSPrintf(HtmlTagStreamMode, "/select");	
		hmStream.SSPrintf(HtmlTagStreamMode, "/div");

		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 780px; left: 1085px;\"");
		hmStream.Printf("Delete points");
		if (delete_selected_points_bool) hmStream.SSPrintf(HtmlTagStreamMode, "input type=\"checkbox\" name=\"form_delete_selected_points_bool\" CHECKED"); 
		else hmStream.SSPrintf(HtmlTagStreamMode, "input type=\"checkbox\" name=\"form_delete_selected_points_bool\"");
		hmStream.SSPrintf(HtmlTagStreamMode, "/div");

		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 815px; left: 1105px;\">t =<input type=\"text\" size=\"2\" maxlength=\"7\" name=\"form_insert_point_x_13\" id=\"form_insert_point_x_13\" value=%.2f></div",box_display_point_x);
		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 840px; left: 1105px;\">y=<input type=\"text\" size=\"2\" maxlength=\"7\" name=\"form_insert_point_y_13\" id=\"form_insert_point_y_13\"></div");

		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 864px; left: 1105px;\"");
		hmStream.SSPrintf(HtmlTagStreamMode, "input type=\"submit\" name=\"submit_insert_point_13\" value=\"Add point\"");
		hmStream.SSPrintf(HtmlTagStreamMode, "/div");

		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 500px; left: 1085px;\">Max: %.1f &#37;</div",puffing_1_p_max_value);
		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 520px; left: 1085px;\">Min: %.1f &#37;</div",puffing_1_p_min_value);
	}
	if(graphic_select == 14){
		DualVectorSort(puffing_1_n_vector_size, &puffing_1_n_index_vector[0], &puffing_1_n_data_vector[0]);
		RemoveRepeatedValues(&puffing_1_n_vector_size, &puffing_1_n_index_vector[0], &puffing_1_n_data_vector[0]);
		
		this->graphics_support->OptimalDisplayFromVectors(puffing_1_n_vector_size, &puffing_1_n_index_vector[0], &puffing_1_n_data_vector[0],&xmin,&ymin,&xmax,&ymax,&majortickx,&majorticky,&number_of_sub_ticks_x,&number_of_sub_ticks_y);
		this->graphics_support->GetGraphSVG(&temp_string_1,(char *) this->Name(),puffing_1_n_vector_size,&puffing_1_n_index_vector[0],&puffing_1_n_data_vector[0], 760, 400,xmin,ymin,xmax,ymax,majortickx,majorticky,number_of_sub_ticks_x,number_of_sub_ticks_y, True,True);
		
		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 490px; left: 320px;\"");
			hmStream.SSPrintf(HtmlTagStreamMode, temp_string_1.Buffer());
		hmStream.SSPrintf(HtmlTagStreamMode, "/div");

		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 540px; left: 1085px;\"");
			hmStream.SSPrintf(HtmlTagStreamMode, "select NAME=\"form_select_waveform_points_14\" size=\"14\" onchange='this.form.submit()'");
				for (i=0;i<puffing_1_n_vector_size;i++) hmStream.SSPrintf(HtmlTagStreamMode, "option value=%d>%.2f | %.2f</option",i,puffing_1_n_index_vector[i],puffing_1_n_data_vector[i]);
			hmStream.SSPrintf(HtmlTagStreamMode, "/select");	
		hmStream.SSPrintf(HtmlTagStreamMode, "/div");

		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 780px; left: 1085px;\"");
		hmStream.Printf("Delete points");
		if (delete_selected_points_bool) hmStream.SSPrintf(HtmlTagStreamMode, "input type=\"checkbox\" name=\"form_delete_selected_points_bool\" CHECKED"); 
		else hmStream.SSPrintf(HtmlTagStreamMode, "input type=\"checkbox\" name=\"form_delete_selected_points_bool\"");
		hmStream.SSPrintf(HtmlTagStreamMode, "/div");

		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 815px; left: 1105px;\">t =<input type=\"text\" size=\"2\" maxlength=\"7\" name=\"form_insert_point_x_14\" id=\"form_insert_point_x_14\" value=%.2f></div",box_display_point_x);
		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 840px; left: 1105px;\">y=<input type=\"text\" size=\"2\" maxlength=\"7\" name=\"form_insert_point_y_14\" id=\"form_insert_point_y_14\"></div");

		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 864px; left: 1105px;\"");
		hmStream.SSPrintf(HtmlTagStreamMode, "input type=\"submit\" name=\"submit_insert_point_14\" value=\"Add point\"");
		hmStream.SSPrintf(HtmlTagStreamMode, "/div");

		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 500px; left: 1085px;\">Max: %.1f &#37;</div",puffing_1_p_max_value);
		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 520px; left: 1085px;\">Min: %.1f &#37;</div",puffing_1_p_min_value);
	}
	if(graphic_select == 15){
		DualVectorSort(puffing_2_p_vector_size, &puffing_2_p_index_vector[0], &puffing_2_p_data_vector[0]);
		RemoveRepeatedValues(&puffing_2_p_vector_size, &puffing_2_p_index_vector[0], &puffing_2_p_data_vector[0]);
		
		this->graphics_support->OptimalDisplayFromVectors(puffing_2_p_vector_size, &puffing_2_p_index_vector[0], &puffing_2_p_data_vector[0],&xmin,&ymin,&xmax,&ymax,&majortickx,&majorticky,&number_of_sub_ticks_x,&number_of_sub_ticks_y);
		this->graphics_support->GetGraphSVG(&temp_string_1,(char *) this->Name(),puffing_2_p_vector_size,&puffing_2_p_index_vector[0],&puffing_2_p_data_vector[0], 760, 400,xmin,ymin,xmax,ymax,majortickx,majorticky,number_of_sub_ticks_x,number_of_sub_ticks_y, True,True);
		
		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 490px; left: 320px;\"");
			hmStream.SSPrintf(HtmlTagStreamMode, temp_string_1.Buffer());
		hmStream.SSPrintf(HtmlTagStreamMode, "/div");

		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 540px; left: 1085px;\"");
			hmStream.SSPrintf(HtmlTagStreamMode, "select NAME=\"form_select_waveform_points_15\" size=\"14\" onchange='this.form.submit()'");
				for (i=0;i<puffing_2_p_vector_size;i++) hmStream.SSPrintf(HtmlTagStreamMode, "option value=%d>%.2f | %.2f</option",i,puffing_2_p_index_vector[i],puffing_2_p_data_vector[i]);
			hmStream.SSPrintf(HtmlTagStreamMode, "/select");	
		hmStream.SSPrintf(HtmlTagStreamMode, "/div");

		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 780px; left: 1085px;\"");
		hmStream.Printf("Delete points");
		if (delete_selected_points_bool) hmStream.SSPrintf(HtmlTagStreamMode, "input type=\"checkbox\" name=\"form_delete_selected_points_bool\" CHECKED"); 
		else hmStream.SSPrintf(HtmlTagStreamMode, "input type=\"checkbox\" name=\"form_delete_selected_points_bool\"");
		hmStream.SSPrintf(HtmlTagStreamMode, "/div");

		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 815px; left: 1105px;\">t =<input type=\"text\" size=\"2\" maxlength=\"7\" name=\"form_insert_point_x_15\" id=\"form_insert_point_x_15\" value=%.2f></div",box_display_point_x);
		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 840px; left: 1105px;\">y=<input type=\"text\" size=\"2\" maxlength=\"7\" name=\"form_insert_point_y_15\" id=\"form_insert_point_y_15\"></div");

		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 864px; left: 1105px;\"");
		hmStream.SSPrintf(HtmlTagStreamMode, "input type=\"submit\" name=\"submit_insert_point_15\" value=\"Add point\"");
		hmStream.SSPrintf(HtmlTagStreamMode, "/div");

		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 500px; left: 1085px;\">Max: %.1f x10<sup>18</sup></div",puffing_2_p_max_value);
		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 520px; left: 1085px;\">Min: %.1f x10<sup>18</sup></div",puffing_2_p_min_value);
	}
	if(graphic_select == 16){
		DualVectorSort(puffing_2_n_vector_size, &puffing_2_n_index_vector[0], &puffing_2_n_data_vector[0]);
		RemoveRepeatedValues(&puffing_2_n_vector_size, &puffing_2_n_index_vector[0], &puffing_2_n_data_vector[0]);
		
		this->graphics_support->OptimalDisplayFromVectors(puffing_2_n_vector_size, &puffing_2_n_index_vector[0], &puffing_2_n_data_vector[0],&xmin,&ymin,&xmax,&ymax,&majortickx,&majorticky,&number_of_sub_ticks_x,&number_of_sub_ticks_y);
		this->graphics_support->GetGraphSVG(&temp_string_1,(char *) this->Name(),puffing_2_n_vector_size,&puffing_2_n_index_vector[0],&puffing_2_n_data_vector[0], 760, 400,xmin,ymin,xmax,ymax,majortickx,majorticky,number_of_sub_ticks_x,number_of_sub_ticks_y, True,True);
		
		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 490px; left: 320px;\"");
			hmStream.SSPrintf(HtmlTagStreamMode, temp_string_1.Buffer());
		hmStream.SSPrintf(HtmlTagStreamMode, "/div");

		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 540px; left: 1085px;\"");
			hmStream.SSPrintf(HtmlTagStreamMode, "select NAME=\"form_select_waveform_points_16\" size=\"14\" onchange='this.form.submit()'");
				for (i=0;i<puffing_2_n_vector_size;i++) hmStream.SSPrintf(HtmlTagStreamMode, "option value=%d>%.2f | %.2f</option",i,puffing_2_n_index_vector[i],puffing_2_n_data_vector[i]);
			hmStream.SSPrintf(HtmlTagStreamMode, "/select");	
		hmStream.SSPrintf(HtmlTagStreamMode, "/div");

		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 780px; left: 1085px;\"");
		hmStream.Printf("Delete points");
		if (delete_selected_points_bool) hmStream.SSPrintf(HtmlTagStreamMode, "input type=\"checkbox\" name=\"form_delete_selected_points_bool\" CHECKED"); 
		else hmStream.SSPrintf(HtmlTagStreamMode, "input type=\"checkbox\" name=\"form_delete_selected_points_bool\"");
		hmStream.SSPrintf(HtmlTagStreamMode, "/div");

		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 815px; left: 1105px;\">t =<input type=\"text\" size=\"2\" maxlength=\"7\" name=\"form_insert_point_x_16\" id=\"form_insert_point_x_16\" value=%.2f></div",box_display_point_x);
		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 840px; left: 1105px;\">y=<input type=\"text\" size=\"2\" maxlength=\"7\" name=\"form_insert_point_y_16\" id=\"form_insert_point_y_16\"></div");

		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 864px; left: 1105px;\"");
		hmStream.SSPrintf(HtmlTagStreamMode, "input type=\"submit\" name=\"submit_insert_point_16\" value=\"Add point\"");
		hmStream.SSPrintf(HtmlTagStreamMode, "/div");

		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 500px; left: 1085px;\">Max: %.1f x10<sup>18</sup></div",puffing_2_p_max_value);
		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 520px; left: 1085px;\">Min: %.1f x10<sup>18</sup></div",puffing_2_p_min_value);
	}
	if(graphic_select == 17){
		DualVectorSort(puffing_absolute_time_vector_size, &puffing_absolute_time_index_vector[0], &puffing_absolute_time_data_vector[0]);
		RemoveRepeatedValues(&puffing_absolute_time_vector_size, &puffing_absolute_time_index_vector[0], &puffing_absolute_time_data_vector[0]);
		
		this->graphics_support->OptimalDisplayFromVectors(puffing_absolute_time_vector_size, &puffing_absolute_time_index_vector[0], &puffing_absolute_time_data_vector[0],&xmin,&ymin,&xmax,&ymax,&majortickx,&majorticky,&number_of_sub_ticks_x,&number_of_sub_ticks_y);
		this->graphics_support->GetGraphSVG(&temp_string_1,(char *) this->Name(),puffing_absolute_time_vector_size,&puffing_absolute_time_index_vector[0],&puffing_absolute_time_data_vector[0], 760, 400,xmin,ymin,xmax,ymax,majortickx,majorticky,number_of_sub_ticks_x,number_of_sub_ticks_y, True,True);
		
		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 490px; left: 320px;\"");
			hmStream.SSPrintf(HtmlTagStreamMode, temp_string_1.Buffer());
		hmStream.SSPrintf(HtmlTagStreamMode, "/div");

		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 540px; left: 1085px;\"");
			hmStream.SSPrintf(HtmlTagStreamMode, "select NAME=\"form_select_waveform_points_17\" size=\"14\" onchange='this.form.submit()'");
				for (i=0;i<puffing_absolute_time_vector_size;i++) hmStream.SSPrintf(HtmlTagStreamMode, "option value=%d>%.2f | %.2f</option",i,puffing_absolute_time_index_vector[i],puffing_absolute_time_data_vector[i]);
			hmStream.SSPrintf(HtmlTagStreamMode, "/select");	
		hmStream.SSPrintf(HtmlTagStreamMode, "/div");

		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 780px; left: 1085px;\"");
		hmStream.Printf("Delete points");
		if (delete_selected_points_bool) hmStream.SSPrintf(HtmlTagStreamMode, "input type=\"checkbox\" name=\"form_delete_selected_points_bool\" CHECKED"); 
		else hmStream.SSPrintf(HtmlTagStreamMode, "input type=\"checkbox\" name=\"form_delete_selected_points_bool\"");
		hmStream.SSPrintf(HtmlTagStreamMode, "/div");

		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 815px; left: 1105px;\">t =<input type=\"text\" size=\"2\" maxlength=\"7\" name=\"form_insert_point_x_17\" id=\"form_insert_point_x_17\" value=%.2f></div",box_display_point_x);
		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 840px; left: 1105px;\">y=<input type=\"text\" size=\"2\" maxlength=\"7\" name=\"form_insert_point_y_17\" id=\"form_insert_point_y_17\"></div");

		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 864px; left: 1105px;\"");
		hmStream.SSPrintf(HtmlTagStreamMode, "input type=\"submit\" name=\"submit_insert_point_17\" value=\"Add point\"");
		hmStream.SSPrintf(HtmlTagStreamMode, "/div");

		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 500px; left: 1085px;\">Max: %.1f &#37;</div",puffing_1_p_max_value);
		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 520px; left: 1085px;\">Min: %.1f &#37;</div",puffing_1_p_min_value);
	}

	if (focus_on_t_form_bool){
		focus_on_t_form_bool = False;
		hmStream.SSPrintf(HtmlTagStreamMode, "script type=\"text/javascript\" language=\"javascript\"> document.getElementById(\"form_insert_point_x_%d\").focus(); </script",graphic_select);
	}
	if (focus_on_y_form_bool){
		focus_on_y_form_bool = False;
		hmStream.SSPrintf(HtmlTagStreamMode, "script type=\"text/javascript\" language=\"javascript\"> document.getElementById(\"form_insert_point_y_%d\").focus(); </script",graphic_select);
	}

//	File operation section
	hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 895px; left: 10px;\"");
	hmStream.SSPrintf(HtmlTagStreamMode, "input type=\"submit\" name=\"form_load_file\" value=\"Load File\"");

	struct dirent *entry;
	DIR *temp_directory;
	
	temp_directory = opendir(config_files_directory.Buffer());
	if (temp_directory == NULL) {
		CStaticAssertErrorCondition(InitialisationError,"MainConfigurator:: FILE!!!: %s empty directory == NULL",this->Name());
	}
	else {
		hmStream.SSPrintf(HtmlTagStreamMode, "select NAME=\"form_load_directory_select\" onchange='this.form.submit()'");
			while((entry = readdir(temp_directory))) {
			
				dummy_fstring = entry->d_name;
				
				if (  !strcmp("..",dummy_fstring.Buffer()) ){
					if(!strcmp(selected_load_directory.Buffer(),dummy_fstring.Buffer())) hmStream.SSPrintf(HtmlTagStreamMode, "option selected=\"selected\" value=%s>%s</option",entry->d_name,"Root DIR");	
					else hmStream.SSPrintf(HtmlTagStreamMode, "option value=%s>%s</option",entry->d_name,"Root DIR");	
				}
				else if (  !strcmp(".",dummy_fstring.Buffer()) ){
					if(!strcmp(selected_load_directory.Buffer(),dummy_fstring.Buffer())) hmStream.SSPrintf(HtmlTagStreamMode, "option selected=\"selected\" value=%s>%s</option",entry->d_name,"Standart DIR");	
					else hmStream.SSPrintf(HtmlTagStreamMode, "option value=%s>%s</option",entry->d_name,"Standart DIR");	
				}
				else if (entry->d_type == DT_DIR){
					if(!strcmp(selected_load_directory.Buffer(),dummy_fstring.Buffer())) hmStream.SSPrintf(HtmlTagStreamMode, "option selected=\"selected\" value=%s>%s</option",entry->d_name,entry->d_name);	
					else hmStream.SSPrintf(HtmlTagStreamMode, "option value=%s>%s</option",entry->d_name,entry->d_name);	
				}
			}
			hmStream.SSPrintf(HtmlTagStreamMode, "/select");	
		
		closedir(temp_directory);
	
	
		dummy_fstring = config_files_directory.Buffer() ;
		dummy_fstring += selected_load_directory.Buffer(); 
		dummy_fstring += "/";
		temp_directory = opendir(dummy_fstring.Buffer());
		if (temp_directory == NULL) {
			CStaticAssertErrorCondition(InitialisationError,"MainConfigurator:: %s could not open this directory = %s",this->Name(),dummy_fstring.Buffer());
		}
		else {
			hmStream.SSPrintf(HtmlTagStreamMode, "select NAME=\"form_load_file_name\" onchange='this.form.submit()'");
				while((entry = readdir(temp_directory))) {
				
					dummy_fstring = entry->d_name;
					
					FString token;
					if (dummy_fstring.GetStringToken(token, ".cfg")){
						if ((dummy_fstring.Size() - token.Size()) == 4 && entry->d_type != DT_DIR){
							if(!strcmp(selected_load_file.Buffer(),dummy_fstring.Buffer())) hmStream.SSPrintf(HtmlTagStreamMode, "option selected=\"selected\" value=%s>%s</option",entry->d_name,entry->d_name);	
							else hmStream.SSPrintf(HtmlTagStreamMode, "option value=%s>%s</option",entry->d_name,entry->d_name);	
						} 
					}	
				}
				hmStream.SSPrintf(HtmlTagStreamMode, "/select");	
		
			closedir(temp_directory);
		}
	}
	hmStream.SSPrintf(HtmlTagStreamMode, "/div");

	if (!edit_main && !edit_control && !edit_general){

		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 895px; left: 420px;\"");
		hmStream.SSPrintf(HtmlTagStreamMode, "input type=\"submit\" name=\"form_save_file_as\" value=\"Save as...\"");
		if (save_as_bool){
			hmStream.SSPrintf(HtmlTagStreamMode, "input type=\"text\" size=\"20\" maxlength=\"40\" name=\"form_save_filename\" value=\"%s\"",save_filename.Buffer());
			hmStream.SSPrintf(HtmlTagStreamMode, "input type=\"submit\" name=\"form_save_file\" value=\"Save\"");
		} 
		hmStream.SSPrintf(HtmlTagStreamMode, "/div");

		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 895px; left: 850px;\"");
		hmStream.SSPrintf(HtmlTagStreamMode, "input type=\"submit\" name=\"form_save_file_and_send\" value=\"Save on default filename and Commit\"");
		hmStream.SSPrintf(HtmlTagStreamMode, "/div");
	}
	else {
		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 898px; left: 620px;\"><b>Save and commit options only available after all parameters saved!</b></div");
	}

	hmStream.SSPrintf(HtmlTagStreamMode, "/form");
	

    return True;
}

bool AdvancedConfigurator::ProcessHttpMessage(HttpStream &hStream){    
    HtmlStream hmStream(hStream);

	FString temp_string_1;
	int i;
	
	this->graphics_support->HeaderSVG(&temp_string_1,(char *) OBJECTNAME, 90, 500, 0,422,87,(char *) HEADERIMAGELINK,10,52, (char *) this->Name(),(char *) header_colour.Buffer());
	hmStream.SSPrintf(HtmlTagStreamMode, temp_string_1.Buffer());

	hmStream.SSPrintf(HtmlTagStreamMode, "svg width=\"100&#37;\" height=\"%d\" style=\"background-color: %s;\">\n\
			</svg", 170, colour1.Buffer());

	hmStream.SSPrintf(HtmlTagStreamMode, "svg width=\"100&#37;\" height=\"%d\" style=\"background-color: %s;\">\n\
			</svg", 170, colour2.Buffer());

	hmStream.SSPrintf(HtmlTagStreamMode, "svg width=\"100&#37;\" height=\"%d\" style=\"background-color: %s;\">\n\
			</svg", 455, colour3.Buffer());

	hmStream.SSPrintf(HtmlTagStreamMode, "svg width=\"100&#37;\" height=\"%d\" style=\"background-color: %s;\">\n\
			</svg", 30, "DarkKhaki");

	hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 80px; left: 10px;\"><b><h2>General Settings</h2></b></div");
	
	hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 255px; left: 10px;\"><b><h2>Control Settings</h2></b></div");
	
	hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 425px; left: 10px;\"><b><h2>Main Settings & Waveforms</h2></b></div");
	
		
		
		
	if (this->already_started){

	//		edit/save buttons
		FString submit_save_main;
		submit_save_main.SetSize(0);
		if (hStream.Switch("InputCommands.submit_save_main")){
			hStream.Seek(0);
			hStream.GetToken(submit_save_main, "");
			hStream.Switch((uint32)0);
		}
		if(submit_save_main.Size() > 0) edit_main = False;
		
		FString submit_edit_main;
		submit_edit_main.SetSize(0);
		if (hStream.Switch("InputCommands.submit_edit_main")){
			hStream.Seek(0);
			hStream.GetToken(submit_edit_main, "");
			hStream.Switch((uint32)0);
		}
		if(submit_edit_main.Size() > 0) edit_main = True;
			
		FString submit_save_control;
		submit_save_control.SetSize(0);
		if (hStream.Switch("InputCommands.submit_save_control")){
			hStream.Seek(0);
			hStream.GetToken(submit_save_control, "");
			hStream.Switch((uint32)0);
		}
		if(submit_save_control.Size() > 0) edit_control = False;
		
		FString submit_edit_control;
		submit_edit_control.SetSize(0);
		if (hStream.Switch("InputCommands.submit_edit_control")){
			hStream.Seek(0);
			hStream.GetToken(submit_edit_control, "");
			hStream.Switch((uint32)0);
		}
		if(submit_edit_control.Size() > 0) edit_control = True;
			
		FString submit_save_general;
		submit_save_general.SetSize(0);
		if (hStream.Switch("InputCommands.submit_save_general")){
			hStream.Seek(0);
			hStream.GetToken(submit_save_general, "");
			hStream.Switch((uint32)0);
		}
		if(submit_save_general.Size() > 0) edit_general = False;
		
		FString submit_edit_general;
		submit_edit_general.SetSize(0);
		if (hStream.Switch("InputCommands.submit_edit_general")){
			hStream.Seek(0);
			hStream.GetToken(submit_edit_general, "");
			hStream.Switch((uint32)0);
		}
		if(submit_edit_general.Size() > 0) edit_general = True;

		FString form_description;
		form_description.SetSize(0);
		if (hStream.Switch("InputCommands.form_description")){
			hStream.Seek(0);
			hStream.GetToken(form_description, "");
			hStream.Switch((uint32)0);
		}
		if(form_description.Size() > 0) Description = form_description.Buffer();

//general settings		
		FString form_tomography_online_0;
		form_tomography_online_0.SetSize(0);
		if (hStream.Switch("InputCommands.form_tomography_online_0")){
			hStream.Seek(0);
			hStream.GetToken(form_tomography_online_0, "");
			hStream.Switch((uint32)0);
		}
		if(form_tomography_online_0.Size() > 0) tomography_online_channels[0] = True;
		else tomography_online_channels[0] =False;
		

		FString form_tomography_online_1;
		form_tomography_online_1.SetSize(0);
		if (hStream.Switch("InputCommands.form_tomography_online_1")){
			hStream.Seek(0);
			hStream.GetToken(form_tomography_online_1, "");
			hStream.Switch((uint32)0);
		}
		if(form_tomography_online_1.Size() > 0) tomography_online_channels[1] = True;
		else tomography_online_channels[1] =False;

		FString form_tomography_online_2;
		form_tomography_online_2.SetSize(0);
		if (hStream.Switch("InputCommands.form_tomography_online_2")){
			hStream.Seek(0);
			hStream.GetToken(form_tomography_online_2, "");
			hStream.Switch((uint32)0);
		}
		if(form_tomography_online_2.Size() > 0) tomography_online_channels[2] = True;
		else tomography_online_channels[2] =False;

		FString form_tomography_online_3;
		form_tomography_online_3.SetSize(0);
		if (hStream.Switch("InputCommands.form_tomography_online_3")){
			hStream.Seek(0);
			hStream.GetToken(form_tomography_online_3, "");
			hStream.Switch((uint32)0);
		}
		if(form_tomography_online_3.Size() > 0) tomography_online_channels[3] = True;
		else tomography_online_channels[3] =False;

		FString form_tomography_online_4;
		form_tomography_online_4.SetSize(0);
		if (hStream.Switch("InputCommands.form_tomography_online_4")){
			hStream.Seek(0);
			hStream.GetToken(form_tomography_online_4, "");
			hStream.Switch((uint32)0);
		}
		if(form_tomography_online_4.Size() > 0) tomography_online_channels[4] = True;
		else tomography_online_channels[4] =False;

		FString form_tomography_online_5;
		form_tomography_online_5.SetSize(0);
		if (hStream.Switch("InputCommands.form_tomography_online_5")){
			hStream.Seek(0);
			hStream.GetToken(form_tomography_online_5, "");
			hStream.Switch((uint32)0);
		}
		if(form_tomography_online_5.Size() > 0) tomography_online_channels[5] = True;
		else tomography_online_channels[5] =False;

		FString form_tomography_online_6;
		form_tomography_online_6.SetSize(0);
		if (hStream.Switch("InputCommands.form_tomography_online_6")){
			hStream.Seek(0);
			hStream.GetToken(form_tomography_online_6, "");
			hStream.Switch((uint32)0);
		}
		if(form_tomography_online_6.Size() > 0) tomography_online_channels[6] = True;
		else tomography_online_channels[6] =False;

		FString form_tomography_online_7;
		form_tomography_online_7.SetSize(0);
		if (hStream.Switch("InputCommands.form_tomography_online_7")){
			hStream.Seek(0);
			hStream.GetToken(form_tomography_online_7, "");
			hStream.Switch((uint32)0);
		}
		if(form_tomography_online_7.Size() > 0) tomography_online_channels[7] = True;
		else tomography_online_channels[7] =False;

		FString form_tomography_online_8;
		form_tomography_online_8.SetSize(0);
		if (hStream.Switch("InputCommands.form_tomography_online_8")){
			hStream.Seek(0);
			hStream.GetToken(form_tomography_online_8, "");
			hStream.Switch((uint32)0);
		}
		if(form_tomography_online_8.Size() > 0) tomography_online_channels[8] = True;
		else tomography_online_channels[8] =False;

		FString form_tomography_online_9;
		form_tomography_online_9.SetSize(0);
		if (hStream.Switch("InputCommands.form_tomography_online_9")){
			hStream.Seek(0);
			hStream.GetToken(form_tomography_online_9, "");
			hStream.Switch((uint32)0);
		}
		if(form_tomography_online_9.Size() > 0) tomography_online_channels[9] = True;
		else tomography_online_channels[9] =False;

		FString form_tomography_online_10;
		form_tomography_online_10.SetSize(0);
		if (hStream.Switch("InputCommands.form_tomography_online_10")){
			hStream.Seek(0);
			hStream.GetToken(form_tomography_online_10, "");
			hStream.Switch((uint32)0);
		}
		if(form_tomography_online_10.Size() > 0) tomography_online_channels[10] = True;
		else tomography_online_channels[10] =False;

		FString form_tomography_online_11;
		form_tomography_online_11.SetSize(0);
		if (hStream.Switch("InputCommands.form_tomography_online_11")){
			hStream.Seek(0);
			hStream.GetToken(form_tomography_online_11, "");
			hStream.Switch((uint32)0);
		}
		if(form_tomography_online_11.Size() > 0) tomography_online_channels[11] = True;
		else tomography_online_channels[11] =False;

		FString form_tomography_online_12;
		form_tomography_online_12.SetSize(0);
		if (hStream.Switch("InputCommands.form_tomography_online_12")){
			hStream.Seek(0);
			hStream.GetToken(form_tomography_online_12, "");
			hStream.Switch((uint32)0);
		}
		if(form_tomography_online_12.Size() > 0) tomography_online_channels[12] = True;
		else tomography_online_channels[12] =False;

		FString form_tomography_online_13;
		form_tomography_online_13.SetSize(0);
		if (hStream.Switch("InputCommands.form_tomography_online_13")){
			hStream.Seek(0);
			hStream.GetToken(form_tomography_online_13, "");
			hStream.Switch((uint32)0);
		}
		if(form_tomography_online_13.Size() > 0) tomography_online_channels[13] = True;
		else tomography_online_channels[13] =False;

		FString form_tomography_online_14;
		form_tomography_online_14.SetSize(0);
		if (hStream.Switch("InputCommands.form_tomography_online_14")){
			hStream.Seek(0);
			hStream.GetToken(form_tomography_online_14, "");
			hStream.Switch((uint32)0);
		}
		if(form_tomography_online_14.Size() > 0) tomography_online_channels[14] = True;
		else tomography_online_channels[14] =False;

		FString form_tomography_online_15;
		form_tomography_online_15.SetSize(0);
		if (hStream.Switch("InputCommands.form_tomography_online_15")){
			hStream.Seek(0);
			hStream.GetToken(form_tomography_online_15, "");
			hStream.Switch((uint32)0);
		}
		if(form_tomography_online_15.Size() > 0) tomography_online_channels[15] = True;
		else tomography_online_channels[15] =False;

		FString form_tomography_online_16;
		form_tomography_online_16.SetSize(0);
		if (hStream.Switch("InputCommands.form_tomography_online_16")){
			hStream.Seek(0);
			hStream.GetToken(form_tomography_online_16, "");
			hStream.Switch((uint32)0);
		}
		if(form_tomography_online_16.Size() > 0) tomography_online_channels[16] = True;
		else tomography_online_channels[16] =False;

		FString form_tomography_online_17;
		form_tomography_online_17.SetSize(0);
		if (hStream.Switch("InputCommands.form_tomography_online_17")){
			hStream.Seek(0);
			hStream.GetToken(form_tomography_online_17, "");
			hStream.Switch((uint32)0);
		}
		if(form_tomography_online_17.Size() > 0) tomography_online_channels[17] = True;
		else tomography_online_channels[17] =False;

		FString form_tomography_online_18;
		form_tomography_online_18.SetSize(0);
		if (hStream.Switch("InputCommands.form_tomography_online_18")){
			hStream.Seek(0);
			hStream.GetToken(form_tomography_online_18, "");
			hStream.Switch((uint32)0);
		}
		if(form_tomography_online_18.Size() > 0) tomography_online_channels[18] = True;
		else tomography_online_channels[18] =False;

		FString form_tomography_online_19;
		form_tomography_online_19.SetSize(0);
		if (hStream.Switch("InputCommands.form_tomography_online_19")){
			hStream.Seek(0);
			hStream.GetToken(form_tomography_online_19, "");
			hStream.Switch((uint32)0);
		}
		if(form_tomography_online_19.Size() > 0) tomography_online_channels[19] = True;
		else tomography_online_channels[19] =False;

		FString form_tomography_online_20;
		form_tomography_online_20.SetSize(0);
		if (hStream.Switch("InputCommands.form_tomography_online_20")){
			hStream.Seek(0);
			hStream.GetToken(form_tomography_online_20, "");
			hStream.Switch((uint32)0);
		}
		if(form_tomography_online_20.Size() > 0) tomography_online_channels[20] = True;
		else tomography_online_channels[20] =False;

		FString form_tomography_online_21;
		form_tomography_online_21.SetSize(0);
		if (hStream.Switch("InputCommands.form_tomography_online_21")){
			hStream.Seek(0);
			hStream.GetToken(form_tomography_online_21, "");
			hStream.Switch((uint32)0);
		}
		if(form_tomography_online_21.Size() > 0) tomography_online_channels[21] = True;
		else tomography_online_channels[21] =False;

		FString form_tomography_online_22;
		form_tomography_online_22.SetSize(0);
		if (hStream.Switch("InputCommands.form_tomography_online_22")){
			hStream.Seek(0);
			hStream.GetToken(form_tomography_online_22, "");
			hStream.Switch((uint32)0);
		}
		if(form_tomography_online_22.Size() > 0) tomography_online_channels[22] = True;
		else tomography_online_channels[22] =False;

		FString form_tomography_online_23;
		form_tomography_online_23.SetSize(0);
		if (hStream.Switch("InputCommands.form_tomography_online_23")){
			hStream.Seek(0);
			hStream.GetToken(form_tomography_online_23, "");
			hStream.Switch((uint32)0);
		}
		if(form_tomography_online_23.Size() > 0) tomography_online_channels[23] = True;
		else tomography_online_channels[23] =False;

		FString form_control_file_to_load;
		form_control_file_to_load.SetSize(0);
		if (hStream.Switch("InputCommands.form_control_file_to_load")){
			hStream.Seek(0);
			hStream.GetToken(form_control_file_to_load, "");
			hStream.Switch((uint32)0);
		}
		if(form_control_file_to_load.Size() > 0) control_file_to_load = form_control_file_to_load.Buffer();

		FString form_tomography_file_to_load;
		form_tomography_file_to_load.SetSize(0);
		if (hStream.Switch("InputCommands.form_tomography_file_to_load")){
			hStream.Seek(0);
			hStream.GetToken(form_tomography_file_to_load, "");
			hStream.Switch((uint32)0);
		}
		if(form_tomography_file_to_load.Size() > 0) tomography_file_to_load = form_tomography_file_to_load.Buffer();
		
		FString form_primaryPS_UARTPortAddress;
		form_primaryPS_UARTPortAddress.SetSize(0);
		if (hStream.Switch("InputCommands.form_primaryPS_UARTPortAddress")){
			hStream.Seek(0);
			hStream.GetToken(form_primaryPS_UARTPortAddress, "");
			hStream.Switch((uint32)0);
		}
		if(form_primaryPS_UARTPortAddress.Size() > 0) primaryPS_UARTPortAddress = form_primaryPS_UARTPortAddress.Buffer();
		
		FString form_verticalPS_UARTPortAddress;
		form_verticalPS_UARTPortAddress.SetSize(0);
		if (hStream.Switch("InputCommands.form_verticalPS_UARTPortAddress")){
			hStream.Seek(0);
			hStream.GetToken(form_verticalPS_UARTPortAddress, "");
			hStream.Switch((uint32)0);
		}
		if(form_verticalPS_UARTPortAddress.Size() > 0) verticalPS_UARTPortAddress = form_verticalPS_UARTPortAddress.Buffer();
		
		FString form_horizontalPS_UARTPortAddress;
		form_horizontalPS_UARTPortAddress.SetSize(0);
		if (hStream.Switch("InputCommands.form_horizontalPS_UARTPortAddress")){
			hStream.Seek(0);
			hStream.GetToken(form_horizontalPS_UARTPortAddress, "");
			hStream.Switch((uint32)0);
		}
		if(form_horizontalPS_UARTPortAddress.Size() > 0) horizontalPS_UARTPortAddress = form_horizontalPS_UARTPortAddress.Buffer();

		FString form_primaryPS_PointOfZeroCurrent;
		form_primaryPS_PointOfZeroCurrent.SetSize(0);
		if (hStream.Switch("InputCommands.form_primaryPS_PointOfZeroCurrent")){
			hStream.Seek(0);
			hStream.GetToken(form_primaryPS_PointOfZeroCurrent, "");
			hStream.Switch((uint32)0);
		}
		if(form_primaryPS_PointOfZeroCurrent.Size() > 0) if(atof(form_primaryPS_PointOfZeroCurrent.Buffer()) > 0 && atof(form_primaryPS_PointOfZeroCurrent.Buffer()) < 1023) primaryPS_PointOfZeroCurrent = (float) atof(form_primaryPS_PointOfZeroCurrent.Buffer());

		FString form_verticalPS_PointOfZeroCurrent;
		form_verticalPS_PointOfZeroCurrent.SetSize(0);
		if (hStream.Switch("InputCommands.form_verticalPS_PointOfZeroCurrent")){
			hStream.Seek(0);
			hStream.GetToken(form_verticalPS_PointOfZeroCurrent, "");
			hStream.Switch((uint32)0);
		}
		if(form_verticalPS_PointOfZeroCurrent.Size() > 0) if(atof(form_verticalPS_PointOfZeroCurrent.Buffer()) > 0 && atof(form_verticalPS_PointOfZeroCurrent.Buffer()) < 1023) verticalPS_PointOfZeroCurrent = (float) atof(form_verticalPS_PointOfZeroCurrent.Buffer());

		FString form_horizontalPS_PointOfZeroCurrent;
		form_horizontalPS_PointOfZeroCurrent.SetSize(0);
		if (hStream.Switch("InputCommands.form_horizontalPS_PointOfZeroCurrent")){
			hStream.Seek(0);
			hStream.GetToken(form_horizontalPS_PointOfZeroCurrent, "");
			hStream.Switch((uint32)0);
		}
		if(form_horizontalPS_PointOfZeroCurrent.Size() > 0) if(atof(form_horizontalPS_PointOfZeroCurrent.Buffer()) > 0 && atof(form_horizontalPS_PointOfZeroCurrent.Buffer()) < 1023) horizontalPS_PointOfZeroCurrent = (float) atof(form_horizontalPS_PointOfZeroCurrent.Buffer());
		
		FString form_primaryPS_CurrentStep;
		form_primaryPS_CurrentStep.SetSize(0);
		if (hStream.Switch("InputCommands.form_primaryPS_CurrentStep")){
			hStream.Seek(0);
			hStream.GetToken(form_primaryPS_CurrentStep, "");
			hStream.Switch((uint32)0);
		}
		if(form_primaryPS_CurrentStep.Size() > 0) if(atof(form_primaryPS_CurrentStep.Buffer()) > 0 && atof(form_primaryPS_CurrentStep.Buffer()) < 1023) primaryPS_CurrentStep = (float) atof(form_primaryPS_CurrentStep.Buffer());

		FString form_verticalPS_CurrentStep;
		form_verticalPS_CurrentStep.SetSize(0);
		if (hStream.Switch("InputCommands.form_verticalPS_CurrentStep")){
			hStream.Seek(0);
			hStream.GetToken(form_verticalPS_CurrentStep, "");
			hStream.Switch((uint32)0);
		}
		if(form_verticalPS_CurrentStep.Size() > 0) if(atof(form_verticalPS_CurrentStep.Buffer()) > 0 && atof(form_verticalPS_CurrentStep.Buffer()) < 1023) verticalPS_CurrentStep = (float) atof(form_verticalPS_CurrentStep.Buffer());

		FString form_horizontalPS_CurrentStep;
		form_horizontalPS_CurrentStep.SetSize(0);
		if (hStream.Switch("InputCommands.form_horizontalPS_CurrentStep")){
			hStream.Seek(0);
			hStream.GetToken(form_horizontalPS_CurrentStep, "");
			hStream.Switch((uint32)0);
		}
		if(form_horizontalPS_CurrentStep.Size() > 0) if(atof(form_horizontalPS_CurrentStep.Buffer()) > 0 && atof(form_horizontalPS_CurrentStep.Buffer()) < 1023) horizontalPS_CurrentStep = (float) atof(form_horizontalPS_CurrentStep.Buffer());


//control
		FString form_primaryPS_P;
		form_primaryPS_P.SetSize(0);
		if (hStream.Switch("InputCommands.form_primaryPS_P")){
			hStream.Seek(0);
			hStream.GetToken(form_primaryPS_P, "");
			hStream.Switch((uint32)0);
		}
		if(form_primaryPS_P.Size() > 0) if(atof(form_primaryPS_P.Buffer()) > 0) primaryPS_P = (float) atof(form_primaryPS_P.Buffer());

		FString form_primaryPS_I;
		form_primaryPS_I.SetSize(0);
		if (hStream.Switch("InputCommands.form_primaryPS_I")){
			hStream.Seek(0);
			hStream.GetToken(form_primaryPS_I, "");
			hStream.Switch((uint32)0);
		}
		if(form_primaryPS_I.Size() > 0) if(atof(form_primaryPS_I.Buffer()) > 0) primaryPS_I = (float) atof(form_primaryPS_I.Buffer());

		FString form_primaryPS_D;
		form_primaryPS_D.SetSize(0);
		if (hStream.Switch("InputCommands.form_primaryPS_D")){
			hStream.Seek(0);
			hStream.GetToken(form_primaryPS_D, "");
			hStream.Switch((uint32)0);
		}
		if(form_primaryPS_D.Size() > 0) if(atof(form_primaryPS_D.Buffer()) > 0) primaryPS_D = (float) atof(form_primaryPS_D.Buffer());

		FString form_verticalPS_P;
		form_verticalPS_P.SetSize(0);
		if (hStream.Switch("InputCommands.form_verticalPS_P")){
			hStream.Seek(0);
			hStream.GetToken(form_verticalPS_P, "");
			hStream.Switch((uint32)0);
		}
		if(form_verticalPS_P.Size() > 0) if(atof(form_verticalPS_P.Buffer()) > 0) verticalPS_P = (float) atof(form_verticalPS_P.Buffer());

		FString form_verticalPS_I;
		form_verticalPS_I.SetSize(0);
		if (hStream.Switch("InputCommands.form_verticalPS_I")){
			hStream.Seek(0);
			hStream.GetToken(form_verticalPS_I, "");
			hStream.Switch((uint32)0);
		}
		if(form_verticalPS_I.Size() > 0) if(atof(form_verticalPS_I.Buffer()) > 0) verticalPS_I = (float) atof(form_verticalPS_I.Buffer());

		FString form_verticalPS_D;
		form_verticalPS_D.SetSize(0);
		if (hStream.Switch("InputCommands.form_verticalPS_D")){
			hStream.Seek(0);
			hStream.GetToken(form_verticalPS_D, "");
			hStream.Switch((uint32)0);
		}
		if(form_verticalPS_D.Size() > 0) if(atof(form_verticalPS_D.Buffer()) > 0) verticalPS_D = (float) atof(form_verticalPS_D.Buffer());

		FString form_horizontalPS_P;
		form_horizontalPS_P.SetSize(0);
		if (hStream.Switch("InputCommands.form_horizontalPS_P")){
			hStream.Seek(0);
			hStream.GetToken(form_horizontalPS_P, "");
			hStream.Switch((uint32)0);
		}
		if(form_horizontalPS_P.Size() > 0) if(atof(form_horizontalPS_P.Buffer()) > 0) horizontalPS_P = (float) atof(form_horizontalPS_P.Buffer());

		FString form_horizontalPS_I;
		form_horizontalPS_I.SetSize(0);
		if (hStream.Switch("InputCommands.form_horizontalPS_I")){
			hStream.Seek(0);
			hStream.GetToken(form_horizontalPS_I, "");
			hStream.Switch((uint32)0);
		}
		if(form_horizontalPS_I.Size() > 0) if(atof(form_horizontalPS_I.Buffer()) > 0) horizontalPS_I = (float) atof(form_horizontalPS_I.Buffer());

		FString form_horizontalPS_D;
		form_horizontalPS_D.SetSize(0);
		if (hStream.Switch("InputCommands.form_horizontalPS_D")){
			hStream.Seek(0);
			hStream.GetToken(form_horizontalPS_D, "");
			hStream.Switch((uint32)0);
		}
		if(form_horizontalPS_D.Size() > 0) if(atof(form_horizontalPS_D.Buffer()) > 0) horizontalPS_D = (float) atof(form_horizontalPS_D.Buffer());

		FString form_toroidal_1_p_max_value;
		form_toroidal_1_p_max_value.SetSize(0);
		if (hStream.Switch("InputCommands.form_toroidal_1_p_max_value")){
			hStream.Seek(0);
			hStream.GetToken(form_toroidal_1_p_max_value, "");
			hStream.Switch((uint32)0);
		}
		FString form_toroidal_1_p_min_value;
		form_toroidal_1_p_min_value.SetSize(0);
		if (hStream.Switch("InputCommands.form_toroidal_1_p_min_value")){
			hStream.Seek(0);
			hStream.GetToken(form_toroidal_1_p_min_value, "");
			hStream.Switch((uint32)0);
		}
		if(form_toroidal_1_p_max_value.Size() > 0 && form_toroidal_1_p_min_value.Size() > 0){
			if( atof(form_toroidal_1_p_max_value.Buffer()) >  atof(form_toroidal_1_p_min_value.Buffer())) {
				toroidal_1_p_max_value = (float) atof(form_toroidal_1_p_max_value.Buffer());
				toroidal_1_p_min_value = (float) atof(form_toroidal_1_p_min_value.Buffer());
			}
		} 

		FString form_primary_1_p_max_value;
		form_primary_1_p_max_value.SetSize(0);
		if (hStream.Switch("InputCommands.form_primary_1_p_max_value")){
			hStream.Seek(0);
			hStream.GetToken(form_primary_1_p_max_value, "");
			hStream.Switch((uint32)0);
		}
		FString form_primary_1_p_min_value;
		form_primary_1_p_min_value.SetSize(0);
		if (hStream.Switch("InputCommands.form_primary_1_p_min_value")){
			hStream.Seek(0);
			hStream.GetToken(form_primary_1_p_min_value, "");
			hStream.Switch((uint32)0);
		}
		if(form_primary_1_p_max_value.Size() > 0 && form_primary_1_p_min_value.Size() > 0){
			if( atof(form_primary_1_p_max_value.Buffer()) >  atof(form_primary_1_p_min_value.Buffer())) {
				primary_1_p_max_value = (float) atof(form_primary_1_p_max_value.Buffer());
				primary_1_p_min_value = (float) atof(form_primary_1_p_min_value.Buffer());
			}
		} 

		FString form_vertical_1_p_max_value;
		form_vertical_1_p_max_value.SetSize(0);
		if (hStream.Switch("InputCommands.form_vertical_1_p_max_value")){
			hStream.Seek(0);
			hStream.GetToken(form_vertical_1_p_max_value, "");
			hStream.Switch((uint32)0);
		}
		FString form_vertical_1_p_min_value;
		form_vertical_1_p_min_value.SetSize(0);
		if (hStream.Switch("InputCommands.form_vertical_1_p_min_value")){
			hStream.Seek(0);
			hStream.GetToken(form_vertical_1_p_min_value, "");
			hStream.Switch((uint32)0);
		}
		if(form_vertical_1_p_max_value.Size() > 0 && form_vertical_1_p_min_value.Size() > 0){
			if( atof(form_vertical_1_p_max_value.Buffer()) >  atof(form_vertical_1_p_min_value.Buffer())) {
				vertical_1_p_max_value = (float) atof(form_vertical_1_p_max_value.Buffer());
				vertical_1_p_min_value = (float) atof(form_vertical_1_p_min_value.Buffer());
			}
		} 

		FString form_horizontal_1_p_max_value;
		form_horizontal_1_p_max_value.SetSize(0);
		if (hStream.Switch("InputCommands.form_horizontal_1_p_max_value")){
			hStream.Seek(0);
			hStream.GetToken(form_horizontal_1_p_max_value, "");
			hStream.Switch((uint32)0);
		}
		FString form_horizontal_1_p_min_value;
		form_horizontal_1_p_min_value.SetSize(0);
		if (hStream.Switch("InputCommands.form_horizontal_1_p_min_value")){
			hStream.Seek(0);
			hStream.GetToken(form_horizontal_1_p_min_value, "");
			hStream.Switch((uint32)0);
		}
		if(form_horizontal_1_p_max_value.Size() > 0 && form_horizontal_1_p_min_value.Size() > 0){
			if( atof(form_horizontal_1_p_max_value.Buffer()) >  atof(form_horizontal_1_p_min_value.Buffer())) {
				horizontal_1_p_max_value = (float) atof(form_horizontal_1_p_max_value.Buffer());
				horizontal_1_p_min_value = (float) atof(form_horizontal_1_p_min_value.Buffer());
			}
		} 

		FString form_puffing_2_p_max_value;
		form_puffing_2_p_max_value.SetSize(0);
		if (hStream.Switch("InputCommands.form_puffing_2_p_max_value")){
			hStream.Seek(0);
			hStream.GetToken(form_puffing_2_p_max_value, "");
			hStream.Switch((uint32)0);
		}
		FString form_puffing_2_p_min_value;
		form_puffing_2_p_min_value.SetSize(0);
		if (hStream.Switch("InputCommands.form_puffing_2_p_min_value")){
			hStream.Seek(0);
			hStream.GetToken(form_puffing_2_p_min_value, "");
			hStream.Switch((uint32)0);
		}
		if(form_puffing_2_p_max_value.Size() > 0 && form_puffing_2_p_min_value.Size() > 0){
			if( atof(form_puffing_2_p_max_value.Buffer()) >  atof(form_puffing_2_p_min_value.Buffer())) {
				puffing_2_p_max_value = (float) atof(form_puffing_2_p_max_value.Buffer());
				puffing_2_p_min_value = (float) atof(form_puffing_2_p_min_value.Buffer());
			}
		} 

		FString form_primary_2_p_max_value;
		form_primary_2_p_max_value.SetSize(0);
		if (hStream.Switch("InputCommands.form_primary_2_p_max_value")){
			hStream.Seek(0);
			hStream.GetToken(form_primary_2_p_max_value, "");
			hStream.Switch((uint32)0);
		}
		FString form_primary_2_p_min_value;
		form_primary_2_p_min_value.SetSize(0);
		if (hStream.Switch("InputCommands.form_primary_2_p_min_value")){
			hStream.Seek(0);
			hStream.GetToken(form_primary_2_p_min_value, "");
			hStream.Switch((uint32)0);
		}
		if(form_primary_2_p_max_value.Size() > 0 && form_primary_2_p_min_value.Size() > 0){
			if( atof(form_primary_2_p_max_value.Buffer()) >  atof(form_primary_2_p_min_value.Buffer())) {
				primary_2_p_max_value = (float) atof(form_primary_2_p_max_value.Buffer());
				primary_2_p_min_value = (float) atof(form_primary_2_p_min_value.Buffer());
			}
		} 

		FString form_vertical_2_p_max_value;
		form_vertical_2_p_max_value.SetSize(0);
		if (hStream.Switch("InputCommands.form_vertical_2_p_max_value")){
			hStream.Seek(0);
			hStream.GetToken(form_vertical_2_p_max_value, "");
			hStream.Switch((uint32)0);
		}
		FString form_vertical_2_p_min_value;
		form_vertical_2_p_min_value.SetSize(0);
		if (hStream.Switch("InputCommands.form_vertical_2_p_min_value")){
			hStream.Seek(0);
			hStream.GetToken(form_vertical_2_p_min_value, "");
			hStream.Switch((uint32)0);
		}
		if(form_vertical_2_p_max_value.Size() > 0 && form_vertical_2_p_min_value.Size() > 0){
			if( atof(form_vertical_2_p_max_value.Buffer()) >  atof(form_vertical_2_p_min_value.Buffer())) {
				vertical_2_p_max_value = (float) atof(form_vertical_2_p_max_value.Buffer());
				vertical_2_p_min_value = (float) atof(form_vertical_2_p_min_value.Buffer());
			}
		} 

		FString form_horizontal_2_p_max_value;
		form_horizontal_2_p_max_value.SetSize(0);
		if (hStream.Switch("InputCommands.form_horizontal_2_p_max_value")){
			hStream.Seek(0);
			hStream.GetToken(form_horizontal_2_p_max_value, "");
			hStream.Switch((uint32)0);
		}
		FString form_horizontal_2_p_min_value;
		form_horizontal_2_p_min_value.SetSize(0);
		if (hStream.Switch("InputCommands.form_horizontal_2_p_min_value")){
			hStream.Seek(0);
			hStream.GetToken(form_horizontal_2_p_min_value, "");
			hStream.Switch((uint32)0);
		}
		if(form_horizontal_2_p_max_value.Size() > 0 && form_horizontal_2_p_min_value.Size() > 0){
			if( atof(form_horizontal_2_p_max_value.Buffer()) >  atof(form_horizontal_2_p_min_value.Buffer())) {
				horizontal_2_p_max_value = (float) atof(form_horizontal_2_p_max_value.Buffer());
				horizontal_2_p_min_value = (float) atof(form_horizontal_2_p_min_value.Buffer());
			}
		} 

		FString form_puffing_duration_in_puffing_feedback_in_ms;
		form_puffing_duration_in_puffing_feedback_in_ms.SetSize(0);
		if (hStream.Switch("InputCommands.form_puffing_duration_in_puffing_feedback_in_ms")){
			hStream.Seek(0);
			hStream.GetToken(form_puffing_duration_in_puffing_feedback_in_ms, "");
			hStream.Switch((uint32)0);
		}
		if(form_puffing_duration_in_puffing_feedback_in_ms.Size() > 0) if(atof(form_puffing_duration_in_puffing_feedback_in_ms.Buffer()) > 0 && atof(form_puffing_duration_in_puffing_feedback_in_ms.Buffer()) < 100.01) puffing_duration_in_puffing_feedback_in_ms = (float) atof(form_puffing_duration_in_puffing_feedback_in_ms.Buffer());

		FString form_puffing_feedback_usec_change_percentage_by_cycle;
		form_puffing_feedback_usec_change_percentage_by_cycle.SetSize(0);
		if (hStream.Switch("InputCommands.form_puffing_feedback_usec_change_percentage_by_cycle")){
			hStream.Seek(0);
			hStream.GetToken(form_puffing_feedback_usec_change_percentage_by_cycle, "");
			hStream.Switch((uint32)0);
		}
		if(form_puffing_feedback_usec_change_percentage_by_cycle.Size() > 0) if(atof(form_puffing_feedback_usec_change_percentage_by_cycle.Buffer()) > 0 && atof(form_puffing_feedback_usec_change_percentage_by_cycle.Buffer()) < 1023) puffing_feedback_usec_change_percentage_by_cycle = (float) atof(form_puffing_feedback_usec_change_percentage_by_cycle.Buffer());

		FString form_maximum_idle_time_in_puffing_feedback_in_ms;
		form_maximum_idle_time_in_puffing_feedback_in_ms.SetSize(0);
		if (hStream.Switch("InputCommands.form_maximum_idle_time_in_puffing_feedback_in_ms")){
			hStream.Seek(0);
			hStream.GetToken(form_maximum_idle_time_in_puffing_feedback_in_ms, "");
			hStream.Switch((uint32)0);
		}
		FString form_minimum_idle_time_in_puffing_feedback_in_ms;
		form_minimum_idle_time_in_puffing_feedback_in_ms.SetSize(0);
		if (hStream.Switch("InputCommands.form_minimum_idle_time_in_puffing_feedback_in_ms")){
			hStream.Seek(0);
			hStream.GetToken(form_minimum_idle_time_in_puffing_feedback_in_ms, "");
			hStream.Switch((uint32)0);
		}
		if(form_maximum_idle_time_in_puffing_feedback_in_ms.Size() > 0 && form_minimum_idle_time_in_puffing_feedback_in_ms.Size() > 0){
			if( atof(form_maximum_idle_time_in_puffing_feedback_in_ms.Buffer()) > atof(form_minimum_idle_time_in_puffing_feedback_in_ms.Buffer()) && atof(form_minimum_idle_time_in_puffing_feedback_in_ms.Buffer()) > 0) {
				maximum_idle_time_in_puffing_feedback_in_ms = (float) atof(form_maximum_idle_time_in_puffing_feedback_in_ms.Buffer());
				minimum_idle_time_in_puffing_feedback_in_ms = (float) atof(form_minimum_idle_time_in_puffing_feedback_in_ms.Buffer());
			}
		} 

		FString form_puffing_feedback_mode;
		form_puffing_feedback_mode.SetSize(0);
		if (hStream.Switch("InputCommands.form_puffing_feedback_mode")){
			hStream.Seek(0);
			hStream.GetToken(form_puffing_feedback_mode, "");
			hStream.Switch((uint32)0);
		}
		if(form_puffing_feedback_mode.Size() > 0) puffing_feedback_mode = atoi(form_puffing_feedback_mode.Buffer());
		
		FString form_thread_priority;
		form_thread_priority.SetSize(0);
		if (hStream.Switch("InputCommands.form_thread_priority")){
			hStream.Seek(0);
			hStream.GetToken(form_thread_priority, "");
			hStream.Switch((uint32)0);
		}
		if(form_thread_priority.Size() > 0) thread_priority = atoi(form_thread_priority.Buffer());

		FString form_run_on_cpu;
		form_run_on_cpu.SetSize(0);
		if (hStream.Switch("InputCommands.form_run_on_cpu")){
			hStream.Seek(0);
			hStream.GetToken(form_run_on_cpu, "");
			hStream.Switch((uint32)0);
		}
		if(form_run_on_cpu.Size() > 0) if(atoi(form_run_on_cpu.Buffer()) >= 0) run_on_cpu = atoi(form_run_on_cpu.Buffer());

//general settings
		FString form_end_discharge_after_unsuccess_bool;
		form_end_discharge_after_unsuccess_bool.SetSize(0);
		if (hStream.Switch("InputCommands.form_end_discharge_after_unsuccess_bool")){
			hStream.Seek(0);
			hStream.GetToken(form_end_discharge_after_unsuccess_bool, "");
			hStream.Switch((uint32)0);
		}
		if(form_end_discharge_after_unsuccess_bool.Size() > 0) end_discharge_after_unsuccess_bool = True;
		else end_discharge_after_unsuccess_bool = False;

		FString form_puffing_mode;
		form_puffing_mode.SetSize(0);
		if (hStream.Switch("InputCommands.form_puffing_mode")){
			hStream.Seek(0);
			hStream.GetToken(form_puffing_mode, "");
			hStream.Switch((uint32)0);
		}
		if(form_puffing_mode.Size() > 0) puffing_mode = atoi(form_puffing_mode.Buffer());

		FString form_usec_pre_pulse_time;
		form_usec_pre_pulse_time.SetSize(0);
		if (hStream.Switch("InputCommands.form_usec_pre_pulse_time")){
			hStream.Seek(0);
			hStream.GetToken(form_usec_pre_pulse_time, "");
			hStream.Switch((uint32)0);
		}
		if(form_usec_pre_pulse_time.Size() > 0) if(atoi(form_usec_pre_pulse_time.Buffer()) >= 0) usec_pre_pulse_time = atoi(form_usec_pre_pulse_time.Buffer());

		FString form_time_between_online_and_discharge;
		form_time_between_online_and_discharge.SetSize(0);
		if (hStream.Switch("InputCommands.form_time_between_online_and_discharge")){
			hStream.Seek(0);
			hStream.GetToken(form_time_between_online_and_discharge, "");
			hStream.Switch((uint32)0);
		}
		if(form_time_between_online_and_discharge.Size() > 0) if(atoi(form_time_between_online_and_discharge.Buffer()) >= 0) time_between_online_and_discharge = atoi(form_time_between_online_and_discharge.Buffer());

		FString form_maximum_inversion_usectime;
		form_maximum_inversion_usectime.SetSize(0);
		if (hStream.Switch("InputCommands.form_maximum_inversion_usectime")){
			hStream.Seek(0);
			hStream.GetToken(form_maximum_inversion_usectime, "");
			hStream.Switch((uint32)0);
		}
		if(form_maximum_inversion_usectime.Size() > 0) if(atoi(form_maximum_inversion_usectime.Buffer()) >= 0) maximum_inversion_usectime = atoi(form_maximum_inversion_usectime.Buffer());

		FString form_iron_core_saturation_value;
		form_iron_core_saturation_value.SetSize(0);
		if (hStream.Switch("InputCommands.form_iron_core_saturation_value")){
			hStream.Seek(0);
			hStream.GetToken(form_iron_core_saturation_value, "");
			hStream.Switch((uint32)0);
		}
		if(form_iron_core_saturation_value.Size() > 0) if(atof(form_iron_core_saturation_value.Buffer()) >= 0) iron_core_saturation_value = atof(form_iron_core_saturation_value.Buffer());

		FString form_iron_core_dangerous_value;
		form_iron_core_dangerous_value.SetSize(0);
		if (hStream.Switch("InputCommands.form_iron_core_dangerous_value")){
			hStream.Seek(0);
			hStream.GetToken(form_iron_core_dangerous_value, "");
			hStream.Switch((uint32)0);
		}
		if(form_iron_core_dangerous_value.Size() > 0) if(atof(form_iron_core_dangerous_value.Buffer()) >= 0) iron_core_dangerous_value = atof(form_iron_core_dangerous_value.Buffer());	



		
//		graphic select
		FString form_graphic_select;
		form_graphic_select.SetSize(0);
		if (hStream.Switch("InputCommands.form_graphic_select")){
			hStream.Seek(0);
			hStream.GetToken(form_graphic_select, "");
			hStream.Switch((uint32)0);
		}
		if(form_graphic_select.Size() > 0){
			graphic_select = atoi(form_graphic_select.Buffer());
			focus_on_t_form_bool = True;
		} 

	//		waveform_points_dropdown
		FString form_delete_selected_points_bool;
		form_delete_selected_points_bool.SetSize(0);
		if (hStream.Switch("InputCommands.form_delete_selected_points_bool")){
			hStream.Seek(0);
			hStream.GetToken(form_delete_selected_points_bool, "");
			hStream.Switch((uint32)0);
		}
		if(form_delete_selected_points_bool.Size() > 0) delete_selected_points_bool = True;
		else delete_selected_points_bool = False;

		if(delete_selected_points_bool){
		
			FString form_select_waveform_points_0;
			form_select_waveform_points_0.SetSize(0);
			if (hStream.Switch("InputCommands.form_select_waveform_points_0")){
				hStream.Seek(0);
				hStream.GetToken(form_select_waveform_points_0, "");
				hStream.Switch((uint32)0);
			}
			if(form_select_waveform_points_0.Size() > 0) RemovePoint(graphic_select, atoi(form_select_waveform_points_0.Buffer()));
			
			FString form_select_waveform_points_1;		
			form_select_waveform_points_1.SetSize(0);
			if (hStream.Switch("InputCommands.form_select_waveform_points_1")){
				hStream.Seek(0);
				hStream.GetToken(form_select_waveform_points_1, "");
				hStream.Switch((uint32)0);
			}
			if(form_select_waveform_points_1.Size() > 0) RemovePoint(graphic_select, atoi(form_select_waveform_points_1.Buffer()));
			
			FString form_select_waveform_points_2;
			form_select_waveform_points_2.SetSize(0);
			if (hStream.Switch("InputCommands.form_select_waveform_points_2")){
				hStream.Seek(0);
				hStream.GetToken(form_select_waveform_points_2, "");
				hStream.Switch((uint32)0);
			}
			if(form_select_waveform_points_2.Size() > 0) RemovePoint(graphic_select, atoi(form_select_waveform_points_2.Buffer()));
			
			FString form_select_waveform_points_3;		
			form_select_waveform_points_3.SetSize(0);
			if (hStream.Switch("InputCommands.form_select_waveform_points_3")){
				hStream.Seek(0);
				hStream.GetToken(form_select_waveform_points_3, "");
				hStream.Switch((uint32)0);
			}
			if(form_select_waveform_points_3.Size() > 0) RemovePoint(graphic_select, atoi(form_select_waveform_points_3.Buffer()));
			
			FString form_select_waveform_points_4;
			form_select_waveform_points_4.SetSize(0);
			if (hStream.Switch("InputCommands.form_select_waveform_points_4")){
				hStream.Seek(0);
				hStream.GetToken(form_select_waveform_points_4, "");
				hStream.Switch((uint32)0);
			}
			if(form_select_waveform_points_4.Size() > 0) RemovePoint(graphic_select, atoi(form_select_waveform_points_4.Buffer()));
			
			FString form_select_waveform_points_5;		
			form_select_waveform_points_5.SetSize(0);
			if (hStream.Switch("InputCommands.form_select_waveform_points_5")){
				hStream.Seek(0);
				hStream.GetToken(form_select_waveform_points_5, "");
				hStream.Switch((uint32)0);
			}
			if(form_select_waveform_points_5.Size() > 0) RemovePoint(graphic_select, atoi(form_select_waveform_points_5.Buffer()));
			
			FString form_select_waveform_points_6;
			form_select_waveform_points_6.SetSize(0);
			if (hStream.Switch("InputCommands.form_select_waveform_points_6")){
				hStream.Seek(0);
				hStream.GetToken(form_select_waveform_points_6, "");
				hStream.Switch((uint32)0);
			}
			if(form_select_waveform_points_6.Size() > 0) RemovePoint(graphic_select, atoi(form_select_waveform_points_6.Buffer()));
			
			FString form_select_waveform_points_7;		
			form_select_waveform_points_7.SetSize(0);
			if (hStream.Switch("InputCommands.form_select_waveform_points_7")){
				hStream.Seek(0);
				hStream.GetToken(form_select_waveform_points_7, "");
				hStream.Switch((uint32)0);
			}
			if(form_select_waveform_points_7.Size() > 0) RemovePoint(graphic_select, atoi(form_select_waveform_points_7.Buffer()));
			
			FString form_select_waveform_points_8;
			form_select_waveform_points_8.SetSize(0);
			if (hStream.Switch("InputCommands.form_select_waveform_points_8")){
				hStream.Seek(0);
				hStream.GetToken(form_select_waveform_points_8, "");
				hStream.Switch((uint32)0);
			}
			if(form_select_waveform_points_8.Size() > 0) RemovePoint(graphic_select, atoi(form_select_waveform_points_8.Buffer()));
			
			FString form_select_waveform_points_9;		
			form_select_waveform_points_9.SetSize(0);
			if (hStream.Switch("InputCommands.form_select_waveform_points_9")){
				hStream.Seek(0);
				hStream.GetToken(form_select_waveform_points_9, "");
				hStream.Switch((uint32)0);
			}
			if(form_select_waveform_points_9.Size() > 0) RemovePoint(graphic_select, atoi(form_select_waveform_points_9.Buffer()));
			
			FString form_select_waveform_points_10;
			form_select_waveform_points_10.SetSize(0);
			if (hStream.Switch("InputCommands.form_select_waveform_points_10")){
				hStream.Seek(0);
				hStream.GetToken(form_select_waveform_points_10, "");
				hStream.Switch((uint32)0);
			}
			if(form_select_waveform_points_10.Size() > 0) RemovePoint(graphic_select, atoi(form_select_waveform_points_10.Buffer()));
			
			FString form_select_waveform_points_11;		
			form_select_waveform_points_11.SetSize(0);
			if (hStream.Switch("InputCommands.form_select_waveform_points_11")){
				hStream.Seek(0);
				hStream.GetToken(form_select_waveform_points_11, "");
				hStream.Switch((uint32)0);
			}
			if(form_select_waveform_points_11.Size() > 0) RemovePoint(graphic_select, atoi(form_select_waveform_points_11.Buffer()));
		
			FString form_select_waveform_points_12;		
			form_select_waveform_points_12.SetSize(0);
			if (hStream.Switch("InputCommands.form_select_waveform_points_12")){
				hStream.Seek(0);
				hStream.GetToken(form_select_waveform_points_12, "");
				hStream.Switch((uint32)0);
			}
			if(form_select_waveform_points_12.Size() > 0) RemovePoint(graphic_select, atoi(form_select_waveform_points_12.Buffer()));
			
			FString form_select_waveform_points_13;		
			form_select_waveform_points_13.SetSize(0);
			if (hStream.Switch("InputCommands.form_select_waveform_points_13")){
				hStream.Seek(0);
				hStream.GetToken(form_select_waveform_points_13, "");
				hStream.Switch((uint32)0);
			}
			if(form_select_waveform_points_13.Size() > 0) RemovePoint(graphic_select, atoi(form_select_waveform_points_13.Buffer()));
			
			FString form_select_waveform_points_14;		
			form_select_waveform_points_14.SetSize(0);
			if (hStream.Switch("InputCommands.form_select_waveform_points_14")){
				hStream.Seek(0);
				hStream.GetToken(form_select_waveform_points_14, "");
				hStream.Switch((uint32)0);
			}
			if(form_select_waveform_points_14.Size() > 0) RemovePoint(graphic_select, atoi(form_select_waveform_points_14.Buffer()));
			
			FString form_select_waveform_points_15;		
			form_select_waveform_points_15.SetSize(0);
			if (hStream.Switch("InputCommands.form_select_waveform_points_15")){
				hStream.Seek(0);
				hStream.GetToken(form_select_waveform_points_15, "");
				hStream.Switch((uint32)0);
			}
			if(form_select_waveform_points_15.Size() > 0) RemovePoint(graphic_select, atoi(form_select_waveform_points_15.Buffer()));
			
			FString form_select_waveform_points_16;		
			form_select_waveform_points_16.SetSize(0);
			if (hStream.Switch("InputCommands.form_select_waveform_points_16")){
				hStream.Seek(0);
				hStream.GetToken(form_select_waveform_points_16, "");
				hStream.Switch((uint32)0);
			}
			if(form_select_waveform_points_16.Size() > 0) RemovePoint(graphic_select, atoi(form_select_waveform_points_16.Buffer()));
			
			FString form_select_waveform_points_17;		
			form_select_waveform_points_17.SetSize(0);
			if (hStream.Switch("InputCommands.form_select_waveform_points_17")){
				hStream.Seek(0);
				hStream.GetToken(form_select_waveform_points_17, "");
				hStream.Switch((uint32)0);
			}
			if(form_select_waveform_points_17.Size() > 0) RemovePoint(graphic_select, atoi(form_select_waveform_points_17.Buffer()));
		}	
		else {
			FString form_select_waveform_points_0;
			form_select_waveform_points_0.SetSize(0);
			if (hStream.Switch("InputCommands.form_select_waveform_points_0")){
				hStream.Seek(0);
				hStream.GetToken(form_select_waveform_points_0, "");
				hStream.Switch((uint32)0);
			}
			if(form_select_waveform_points_0.Size() > 0) DisplayPoint(graphic_select, atoi(form_select_waveform_points_0.Buffer()));
			
			FString form_select_waveform_points_1;		
			form_select_waveform_points_1.SetSize(0);
			if (hStream.Switch("InputCommands.form_select_waveform_points_1")){
				hStream.Seek(0);
				hStream.GetToken(form_select_waveform_points_1, "");
				hStream.Switch((uint32)0);
			}
			if(form_select_waveform_points_1.Size() > 0) DisplayPoint(graphic_select, atoi(form_select_waveform_points_1.Buffer()));
			
			FString form_select_waveform_points_2;
			form_select_waveform_points_2.SetSize(0);
			if (hStream.Switch("InputCommands.form_select_waveform_points_2")){
				hStream.Seek(0);
				hStream.GetToken(form_select_waveform_points_2, "");
				hStream.Switch((uint32)0);
			}
			if(form_select_waveform_points_2.Size() > 0) DisplayPoint(graphic_select, atoi(form_select_waveform_points_2.Buffer()));
			
			FString form_select_waveform_points_3;		
			form_select_waveform_points_3.SetSize(0);
			if (hStream.Switch("InputCommands.form_select_waveform_points_3")){
				hStream.Seek(0);
				hStream.GetToken(form_select_waveform_points_3, "");
				hStream.Switch((uint32)0);
			}
			if(form_select_waveform_points_3.Size() > 0) DisplayPoint(graphic_select, atoi(form_select_waveform_points_3.Buffer()));
			
			FString form_select_waveform_points_4;
			form_select_waveform_points_4.SetSize(0);
			if (hStream.Switch("InputCommands.form_select_waveform_points_4")){
				hStream.Seek(0);
				hStream.GetToken(form_select_waveform_points_4, "");
				hStream.Switch((uint32)0);
			}
			if(form_select_waveform_points_4.Size() > 0) DisplayPoint(graphic_select, atoi(form_select_waveform_points_4.Buffer()));
			
			FString form_select_waveform_points_5;		
			form_select_waveform_points_5.SetSize(0);
			if (hStream.Switch("InputCommands.form_select_waveform_points_5")){
				hStream.Seek(0);
				hStream.GetToken(form_select_waveform_points_5, "");
				hStream.Switch((uint32)0);
			}
			if(form_select_waveform_points_5.Size() > 0) DisplayPoint(graphic_select, atoi(form_select_waveform_points_5.Buffer()));
			
			FString form_select_waveform_points_6;
			form_select_waveform_points_6.SetSize(0);
			if (hStream.Switch("InputCommands.form_select_waveform_points_6")){
				hStream.Seek(0);
				hStream.GetToken(form_select_waveform_points_6, "");
				hStream.Switch((uint32)0);
			}
			if(form_select_waveform_points_6.Size() > 0) DisplayPoint(graphic_select, atoi(form_select_waveform_points_6.Buffer()));
			
			FString form_select_waveform_points_7;		
			form_select_waveform_points_7.SetSize(0);
			if (hStream.Switch("InputCommands.form_select_waveform_points_7")){
				hStream.Seek(0);
				hStream.GetToken(form_select_waveform_points_7, "");
				hStream.Switch((uint32)0);
			}
			if(form_select_waveform_points_7.Size() > 0) DisplayPoint(graphic_select, atoi(form_select_waveform_points_7.Buffer()));
			
			FString form_select_waveform_points_8;
			form_select_waveform_points_8.SetSize(0);
			if (hStream.Switch("InputCommands.form_select_waveform_points_8")){
				hStream.Seek(0);
				hStream.GetToken(form_select_waveform_points_8, "");
				hStream.Switch((uint32)0);
			}
			if(form_select_waveform_points_8.Size() > 0) DisplayPoint(graphic_select, atoi(form_select_waveform_points_8.Buffer()));
			
			FString form_select_waveform_points_9;		
			form_select_waveform_points_9.SetSize(0);
			if (hStream.Switch("InputCommands.form_select_waveform_points_9")){
				hStream.Seek(0);
				hStream.GetToken(form_select_waveform_points_9, "");
				hStream.Switch((uint32)0);
			}
			if(form_select_waveform_points_9.Size() > 0) DisplayPoint(graphic_select, atoi(form_select_waveform_points_9.Buffer()));
			
			FString form_select_waveform_points_10;
			form_select_waveform_points_10.SetSize(0);
			if (hStream.Switch("InputCommands.form_select_waveform_points_10")){
				hStream.Seek(0);
				hStream.GetToken(form_select_waveform_points_10, "");
				hStream.Switch((uint32)0);
			}
			if(form_select_waveform_points_10.Size() > 0) DisplayPoint(graphic_select, atoi(form_select_waveform_points_10.Buffer()));
			
			FString form_select_waveform_points_11;		
			form_select_waveform_points_11.SetSize(0);
			if (hStream.Switch("InputCommands.form_select_waveform_points_11")){
				hStream.Seek(0);
				hStream.GetToken(form_select_waveform_points_11, "");
				hStream.Switch((uint32)0);
			}
			if(form_select_waveform_points_11.Size() > 0) DisplayPoint(graphic_select, atoi(form_select_waveform_points_11.Buffer()));
			
			FString form_select_waveform_points_12;		
			form_select_waveform_points_12.SetSize(0);
			if (hStream.Switch("InputCommands.form_select_waveform_points_12")){
				hStream.Seek(0);
				hStream.GetToken(form_select_waveform_points_12, "");
				hStream.Switch((uint32)0);
			}
			if(form_select_waveform_points_12.Size() > 0) DisplayPoint(graphic_select, atoi(form_select_waveform_points_12.Buffer()));
			
			FString form_select_waveform_points_13;		
			form_select_waveform_points_13.SetSize(0);
			if (hStream.Switch("InputCommands.form_select_waveform_points_13")){
				hStream.Seek(0);
				hStream.GetToken(form_select_waveform_points_13, "");
				hStream.Switch((uint32)0);
			}
			if(form_select_waveform_points_13.Size() > 0) DisplayPoint(graphic_select, atoi(form_select_waveform_points_13.Buffer()));
			
			FString form_select_waveform_points_14;		
			form_select_waveform_points_14.SetSize(0);
			if (hStream.Switch("InputCommands.form_select_waveform_points_14")){
				hStream.Seek(0);
				hStream.GetToken(form_select_waveform_points_14, "");
				hStream.Switch((uint32)0);
			}
			if(form_select_waveform_points_14.Size() > 0) DisplayPoint(graphic_select, atoi(form_select_waveform_points_14.Buffer()));
			
			FString form_select_waveform_points_15;		
			form_select_waveform_points_15.SetSize(0);
			if (hStream.Switch("InputCommands.form_select_waveform_points_15")){
				hStream.Seek(0);
				hStream.GetToken(form_select_waveform_points_15, "");
				hStream.Switch((uint32)0);
			}
			if(form_select_waveform_points_15.Size() > 0) DisplayPoint(graphic_select, atoi(form_select_waveform_points_15.Buffer()));
			
			FString form_select_waveform_points_16;		
			form_select_waveform_points_16.SetSize(0);
			if (hStream.Switch("InputCommands.form_select_waveform_points_16")){
				hStream.Seek(0);
				hStream.GetToken(form_select_waveform_points_16, "");
				hStream.Switch((uint32)0);
			}
			if(form_select_waveform_points_16.Size() > 0) DisplayPoint(graphic_select, atoi(form_select_waveform_points_16.Buffer()));
			
			FString form_select_waveform_points_17;		
			form_select_waveform_points_17.SetSize(0);
			if (hStream.Switch("InputCommands.form_select_waveform_points_17")){
				hStream.Seek(0);
				hStream.GetToken(form_select_waveform_points_17, "");
				hStream.Switch((uint32)0);
			}
			if(form_select_waveform_points_17.Size() > 0) DisplayPoint(graphic_select, atoi(form_select_waveform_points_17.Buffer()));
		}	

		FString submit_insert_point_0;		
		submit_insert_point_0.SetSize(0);
		if (hStream.Switch("InputCommands.submit_insert_point_0")){
			hStream.Seek(0);
			hStream.GetToken(submit_insert_point_0, "");
			hStream.Switch((uint32)0);
		}
		
		FString submit_insert_point_1;		
		submit_insert_point_1.SetSize(0);
		if (hStream.Switch("InputCommands.submit_insert_point_1")){
			hStream.Seek(0);
			hStream.GetToken(submit_insert_point_1, "");
			hStream.Switch((uint32)0);
		}
		
		FString submit_insert_point_2;		
		submit_insert_point_2.SetSize(0);
		if (hStream.Switch("InputCommands.submit_insert_point_2")){
			hStream.Seek(0);
			hStream.GetToken(submit_insert_point_2, "");
			hStream.Switch((uint32)0);
		}
		
		FString submit_insert_point_3;		
		submit_insert_point_3.SetSize(0);
		if (hStream.Switch("InputCommands.submit_insert_point_3")){
			hStream.Seek(0);
			hStream.GetToken(submit_insert_point_3, "");
			hStream.Switch((uint32)0);
		}
		
		FString submit_insert_point_4;		
		submit_insert_point_4.SetSize(0);
		if (hStream.Switch("InputCommands.submit_insert_point_4")){
			hStream.Seek(0);
			hStream.GetToken(submit_insert_point_4, "");
			hStream.Switch((uint32)0);
		}
		
		FString submit_insert_point_5;		
		submit_insert_point_5.SetSize(0);
		if (hStream.Switch("InputCommands.submit_insert_point_5")){
			hStream.Seek(0);
			hStream.GetToken(submit_insert_point_5, "");
			hStream.Switch((uint32)0);
		}
		
		FString submit_insert_point_6;		
		submit_insert_point_6.SetSize(0);
		if (hStream.Switch("InputCommands.submit_insert_point_6")){
			hStream.Seek(0);
			hStream.GetToken(submit_insert_point_6, "");
			hStream.Switch((uint32)0);
		}
		
		FString submit_insert_point_7;		
		submit_insert_point_7.SetSize(0);
		if (hStream.Switch("InputCommands.submit_insert_point_7")){
			hStream.Seek(0);
			hStream.GetToken(submit_insert_point_7, "");
			hStream.Switch((uint32)0);
		}
		
		FString submit_insert_point_8;		
		submit_insert_point_8.SetSize(0);
		if (hStream.Switch("InputCommands.submit_insert_point_8")){
			hStream.Seek(0);
			hStream.GetToken(submit_insert_point_8, "");
			hStream.Switch((uint32)0);
		}
		
		FString submit_insert_point_9;		
		submit_insert_point_9.SetSize(0);
		if (hStream.Switch("InputCommands.submit_insert_point_9")){
			hStream.Seek(0);
			hStream.GetToken(submit_insert_point_9, "");
			hStream.Switch((uint32)0);
		}
		
		FString submit_insert_point_10;		
		submit_insert_point_10.SetSize(0);
		if (hStream.Switch("InputCommands.submit_insert_point_10")){
			hStream.Seek(0);
			hStream.GetToken(submit_insert_point_10, "");
			hStream.Switch((uint32)0);
		}
		
		FString submit_insert_point_11;		
		submit_insert_point_11.SetSize(0);
		if (hStream.Switch("InputCommands.submit_insert_point_11")){
			hStream.Seek(0);
			hStream.GetToken(submit_insert_point_11, "");
			hStream.Switch((uint32)0);
		}
		
		FString submit_insert_point_12;		
		submit_insert_point_12.SetSize(0);
		if (hStream.Switch("InputCommands.submit_insert_point_12")){
			hStream.Seek(0);
			hStream.GetToken(submit_insert_point_12, "");
			hStream.Switch((uint32)0);
		}
		
		FString submit_insert_point_13;		
		submit_insert_point_13.SetSize(0);
		if (hStream.Switch("InputCommands.submit_insert_point_13")){
			hStream.Seek(0);
			hStream.GetToken(submit_insert_point_13, "");
			hStream.Switch((uint32)0);
		}
		
		FString submit_insert_point_14;		
		submit_insert_point_14.SetSize(0);
		if (hStream.Switch("InputCommands.submit_insert_point_14")){
			hStream.Seek(0);
			hStream.GetToken(submit_insert_point_14, "");
			hStream.Switch((uint32)0);
		}
		
		FString submit_insert_point_15;		
		submit_insert_point_15.SetSize(0);
		if (hStream.Switch("InputCommands.submit_insert_point_15")){
			hStream.Seek(0);
			hStream.GetToken(submit_insert_point_15, "");
			hStream.Switch((uint32)0);
		}
		
		FString submit_insert_point_16;		
		submit_insert_point_16.SetSize(0);
		if (hStream.Switch("InputCommands.submit_insert_point_16")){
			hStream.Seek(0);
			hStream.GetToken(submit_insert_point_16, "");
			hStream.Switch((uint32)0);
		}

		FString submit_insert_point_17;		
		submit_insert_point_17.SetSize(0);
		if (hStream.Switch("InputCommands.submit_insert_point_17")){
			hStream.Seek(0);
			hStream.GetToken(submit_insert_point_17, "");
			hStream.Switch((uint32)0);
		}

		if(submit_insert_point_0.Size() > 0) {
			
			FString form_insert_point_x_0;		
			form_insert_point_x_0.SetSize(0);
			if (hStream.Switch("InputCommands.form_insert_point_x_0")){
				hStream.Seek(0);
				hStream.GetToken(form_insert_point_x_0, "");
				hStream.Switch((uint32)0);
			}
			FString form_insert_point_y_0;		
			form_insert_point_y_0.SetSize(0);
			if (hStream.Switch("InputCommands.form_insert_point_y_0")){
				hStream.Seek(0);
				hStream.GetToken(form_insert_point_y_0, "");
				hStream.Switch((uint32)0);
			}
			
			if(form_insert_point_x_0.Size() > 0 && form_insert_point_y_0.Size() > 0) AddPoint(graphic_select, (floor(atof(form_insert_point_x_0.Buffer())*100 + 0.5)/100), (floor(atof(form_insert_point_y_0.Buffer())*100 + 0.5)/100));
		}
		if(submit_insert_point_1.Size() > 0) {
			
			FString form_insert_point_x_1;		
			form_insert_point_x_1.SetSize(0);
			if (hStream.Switch("InputCommands.form_insert_point_x_1")){
				hStream.Seek(0);
				hStream.GetToken(form_insert_point_x_1, "");
				hStream.Switch((uint32)0);
			}
			FString form_insert_point_y_1;		
			form_insert_point_y_1.SetSize(0);
			if (hStream.Switch("InputCommands.form_insert_point_y_1")){
				hStream.Seek(0);
				hStream.GetToken(form_insert_point_y_1, "");
				hStream.Switch((uint32)0);
			}
			
			if(form_insert_point_x_1.Size() > 0 && form_insert_point_y_1.Size() > 0) AddPoint(graphic_select, (floor(atof(form_insert_point_x_1.Buffer())*100 + 0.5)/100), (floor(atof(form_insert_point_y_1.Buffer())*100 + 0.5)/100));
		}
		if(submit_insert_point_2.Size() > 0) {
			
			FString form_insert_point_x_2;		
			form_insert_point_x_2.SetSize(0);
			if (hStream.Switch("InputCommands.form_insert_point_x_2")){
				hStream.Seek(0);
				hStream.GetToken(form_insert_point_x_2, "");
				hStream.Switch((uint32)0);
			}
			FString form_insert_point_y_2;		
			form_insert_point_y_2.SetSize(0);
			if (hStream.Switch("InputCommands.form_insert_point_y_2")){
				hStream.Seek(0);
				hStream.GetToken(form_insert_point_y_2, "");
				hStream.Switch((uint32)0);
			}
			
			if(form_insert_point_x_2.Size() > 0 && form_insert_point_y_2.Size() > 0) AddPoint(graphic_select, (floor(atof(form_insert_point_x_2.Buffer())*100 + 0.5)/100), (floor(atof(form_insert_point_y_2.Buffer())*100 + 0.5)/100));
		}
		if(submit_insert_point_3.Size() > 0) {
			
			FString form_insert_point_x_3;		
			form_insert_point_x_3.SetSize(0);
			if (hStream.Switch("InputCommands.form_insert_point_x_3")){
				hStream.Seek(0);
				hStream.GetToken(form_insert_point_x_3, "");
				hStream.Switch((uint32)0);
			}
			FString form_insert_point_y_3;		
			form_insert_point_y_3.SetSize(0);
			if (hStream.Switch("InputCommands.form_insert_point_y_3")){
				hStream.Seek(0);
				hStream.GetToken(form_insert_point_y_3, "");
				hStream.Switch((uint32)0);
			}
			
			if(form_insert_point_x_3.Size() > 0 && form_insert_point_y_3.Size() > 0) AddPoint(graphic_select, (floor(atof(form_insert_point_x_3.Buffer())*100 + 0.5)/100), (floor(atof(form_insert_point_y_3.Buffer())*100 + 0.5)/100));
		}
		if(submit_insert_point_4.Size() > 0) {
			
			FString form_insert_point_x_4;		
			form_insert_point_x_4.SetSize(0);
			if (hStream.Switch("InputCommands.form_insert_point_x_4")){
				hStream.Seek(0);
				hStream.GetToken(form_insert_point_x_4, "");
				hStream.Switch((uint32)0);
			}
			FString form_insert_point_y_4;		
			form_insert_point_y_4.SetSize(0);
			if (hStream.Switch("InputCommands.form_insert_point_y_4")){
				hStream.Seek(0);
				hStream.GetToken(form_insert_point_y_4, "");
				hStream.Switch((uint32)0);
			}
			
			if(form_insert_point_x_4.Size() > 0 && form_insert_point_y_4.Size() > 0) AddPoint(graphic_select, (floor(atof(form_insert_point_x_4.Buffer())*100 + 0.5)/100), (floor(atof(form_insert_point_y_4.Buffer())*100 + 0.5)/100));
		}
		if(submit_insert_point_5.Size() > 0) {
			
			FString form_insert_point_x_5;		
			form_insert_point_x_5.SetSize(0);
			if (hStream.Switch("InputCommands.form_insert_point_x_5")){
				hStream.Seek(0);
				hStream.GetToken(form_insert_point_x_5, "");
				hStream.Switch((uint32)0);
			}
			FString form_insert_point_y_5;		
			form_insert_point_y_5.SetSize(0);
			if (hStream.Switch("InputCommands.form_insert_point_y_5")){
				hStream.Seek(0);
				hStream.GetToken(form_insert_point_y_5, "");
				hStream.Switch((uint32)0);
			}
			
			if(form_insert_point_x_5.Size() > 0 && form_insert_point_y_5.Size() > 0) AddPoint(graphic_select, (floor(atof(form_insert_point_x_5.Buffer())*100 + 0.5)/100), (floor(atof(form_insert_point_y_5.Buffer())*100 + 0.5)/100));
		}
		if(submit_insert_point_6.Size() > 0) {
			
			FString form_insert_point_x_6;		
			form_insert_point_x_6.SetSize(0);
			if (hStream.Switch("InputCommands.form_insert_point_x_6")){
				hStream.Seek(0);
				hStream.GetToken(form_insert_point_x_6, "");
				hStream.Switch((uint32)0);
			}
			FString form_insert_point_y_6;		
			form_insert_point_y_6.SetSize(0);
			if (hStream.Switch("InputCommands.form_insert_point_y_6")){
				hStream.Seek(0);
				hStream.GetToken(form_insert_point_y_6, "");
				hStream.Switch((uint32)0);
			}
			
			if(form_insert_point_x_6.Size() > 0 && form_insert_point_y_6.Size() > 0) AddPoint(graphic_select, (floor(atof(form_insert_point_x_6.Buffer())*100 + 0.5)/100), (floor(atof(form_insert_point_y_6.Buffer())*100 + 0.5)/100));
		}
		if(submit_insert_point_7.Size() > 0) {
			
			FString form_insert_point_x_7;		
			form_insert_point_x_7.SetSize(0);
			if (hStream.Switch("InputCommands.form_insert_point_x_7")){
				hStream.Seek(0);
				hStream.GetToken(form_insert_point_x_7, "");
				hStream.Switch((uint32)0);
			}
			FString form_insert_point_y_7;		
			form_insert_point_y_7.SetSize(0);
			if (hStream.Switch("InputCommands.form_insert_point_y_7")){
				hStream.Seek(0);
				hStream.GetToken(form_insert_point_y_7, "");
				hStream.Switch((uint32)0);
			}
			
			if(form_insert_point_x_7.Size() > 0 && form_insert_point_y_7.Size() > 0) AddPoint(graphic_select, (floor(atof(form_insert_point_x_7.Buffer())*100 + 0.5)/100), (floor(atof(form_insert_point_y_7.Buffer())*100 + 0.5)/100));
		}
		if(submit_insert_point_8.Size() > 0) {
			
			FString form_insert_point_x_8;		
			form_insert_point_x_8.SetSize(0);
			if (hStream.Switch("InputCommands.form_insert_point_x_8")){
				hStream.Seek(0);
				hStream.GetToken(form_insert_point_x_8, "");
				hStream.Switch((uint32)0);
			}
			FString form_insert_point_y_8;		
			form_insert_point_y_8.SetSize(0);
			if (hStream.Switch("InputCommands.form_insert_point_y_8")){
				hStream.Seek(0);
				hStream.GetToken(form_insert_point_y_8, "");
				hStream.Switch((uint32)0);
			}
			
			if(form_insert_point_x_8.Size() > 0 && form_insert_point_y_8.Size() > 0) AddPoint(graphic_select, (floor(atof(form_insert_point_x_8.Buffer())*100 + 0.5)/100), (floor(atof(form_insert_point_y_8.Buffer())*100 + 0.5)/100));
		}
		if(submit_insert_point_9.Size() > 0) {
			
			FString form_insert_point_x_9;		
			form_insert_point_x_9.SetSize(0);
			if (hStream.Switch("InputCommands.form_insert_point_x_9")){
				hStream.Seek(0);
				hStream.GetToken(form_insert_point_x_9, "");
				hStream.Switch((uint32)0);
			}
			FString form_insert_point_y_9;		
			form_insert_point_y_9.SetSize(0);
			if (hStream.Switch("InputCommands.form_insert_point_y_9")){
				hStream.Seek(0);
				hStream.GetToken(form_insert_point_y_9, "");
				hStream.Switch((uint32)0);
			}
			
			if(form_insert_point_x_9.Size() > 0 && form_insert_point_y_9.Size() > 0) AddPoint(graphic_select, (floor(atof(form_insert_point_x_9.Buffer())*100 + 0.5)/100), (floor(atof(form_insert_point_y_9.Buffer())*100 + 0.5)/100));
		}
		if(submit_insert_point_10.Size() > 0) {
			
			FString form_insert_point_x_10;		
			form_insert_point_x_10.SetSize(0);
			if (hStream.Switch("InputCommands.form_insert_point_x_10")){
				hStream.Seek(0);
				hStream.GetToken(form_insert_point_x_10, "");
				hStream.Switch((uint32)0);
			}
			FString form_insert_point_y_10;		
			form_insert_point_y_10.SetSize(0);
			if (hStream.Switch("InputCommands.form_insert_point_y_10")){
				hStream.Seek(0);
				hStream.GetToken(form_insert_point_y_10, "");
				hStream.Switch((uint32)0);
			}
			
			if(form_insert_point_x_10.Size() > 0 && form_insert_point_y_10.Size() > 0) AddPoint(graphic_select, (floor(atof(form_insert_point_x_10.Buffer())*100 + 0.5)/100), (floor(atof(form_insert_point_y_10.Buffer())*100 + 0.5)/100));
		}
		if(submit_insert_point_11.Size() > 0) {
			
			FString form_insert_point_x_11;		
			form_insert_point_x_11.SetSize(0);
			if (hStream.Switch("InputCommands.form_insert_point_x_11")){
				hStream.Seek(0);
				hStream.GetToken(form_insert_point_x_11, "");
				hStream.Switch((uint32)0);
			}
			FString form_insert_point_y_11;		
			form_insert_point_y_11.SetSize(0);
			if (hStream.Switch("InputCommands.form_insert_point_y_11")){
				hStream.Seek(0);
				hStream.GetToken(form_insert_point_y_11, "");
				hStream.Switch((uint32)0);
			}
			
			if(form_insert_point_x_11.Size() > 0 && form_insert_point_y_11.Size() > 0) AddPoint(graphic_select, (floor(atof(form_insert_point_x_11.Buffer())*100 + 0.5)/100), (floor(atof(form_insert_point_y_11.Buffer())*100 + 0.5)/100));
		}
		if(submit_insert_point_12.Size() > 0) {
			
			FString form_insert_point_x_12;		
			form_insert_point_x_12.SetSize(0);
			if (hStream.Switch("InputCommands.form_insert_point_x_12")){
				hStream.Seek(0);
				hStream.GetToken(form_insert_point_x_12, "");
				hStream.Switch((uint32)0);
			}
			FString form_insert_point_y_12;		
			form_insert_point_y_12.SetSize(0);
			if (hStream.Switch("InputCommands.form_insert_point_y_12")){
				hStream.Seek(0);
				hStream.GetToken(form_insert_point_y_12, "");
				hStream.Switch((uint32)0);
			}
			
			if(form_insert_point_x_12.Size() > 0 && form_insert_point_y_12.Size() > 0) AddPoint(graphic_select, (floor(atof(form_insert_point_x_12.Buffer())*100 + 0.5)/100), (floor(atof(form_insert_point_y_12.Buffer())*100 + 0.5)/100));
		}
		if(submit_insert_point_13.Size() > 0) {
			
			FString form_insert_point_x_13;		
			form_insert_point_x_13.SetSize(0);
			if (hStream.Switch("InputCommands.form_insert_point_x_13")){
				hStream.Seek(0);
				hStream.GetToken(form_insert_point_x_13, "");
				hStream.Switch((uint32)0);
			}
			FString form_insert_point_y_13;		
			form_insert_point_y_13.SetSize(0);
			if (hStream.Switch("InputCommands.form_insert_point_y_13")){
				hStream.Seek(0);
				hStream.GetToken(form_insert_point_y_13, "");
				hStream.Switch((uint32)0);
			}
			
			if(form_insert_point_x_13.Size() > 0 && form_insert_point_y_13.Size() > 0) AddPoint(graphic_select, (floor(atof(form_insert_point_x_13.Buffer())*100 + 0.5)/100), (floor(atof(form_insert_point_y_13.Buffer())*100 + 0.5)/100));
		}
		if(submit_insert_point_14.Size() > 0) {
			
			FString form_insert_point_x_14;		
			form_insert_point_x_14.SetSize(0);
			if (hStream.Switch("InputCommands.form_insert_point_x_14")){
				hStream.Seek(0);
				hStream.GetToken(form_insert_point_x_14, "");
				hStream.Switch((uint32)0);
			}
			FString form_insert_point_y_14;		
			form_insert_point_y_14.SetSize(0);
			if (hStream.Switch("InputCommands.form_insert_point_y_14")){
				hStream.Seek(0);
				hStream.GetToken(form_insert_point_y_14, "");
				hStream.Switch((uint32)0);
			}
			
			if(form_insert_point_x_14.Size() > 0 && form_insert_point_y_14.Size() > 0) AddPoint(graphic_select, (floor(atof(form_insert_point_x_14.Buffer())*100 + 0.5)/100), (floor(atof(form_insert_point_y_14.Buffer())*100 + 0.5)/100));
		}
		if(submit_insert_point_15.Size() > 0) {
			
			FString form_insert_point_x_15;		
			form_insert_point_x_15.SetSize(0);
			if (hStream.Switch("InputCommands.form_insert_point_x_15")){
				hStream.Seek(0);
				hStream.GetToken(form_insert_point_x_15, "");
				hStream.Switch((uint32)0);
			}
			FString form_insert_point_y_15;		
			form_insert_point_y_15.SetSize(0);
			if (hStream.Switch("InputCommands.form_insert_point_y_15")){
				hStream.Seek(0);
				hStream.GetToken(form_insert_point_y_15, "");
				hStream.Switch((uint32)0);
			}
			
			if(form_insert_point_x_15.Size() > 0 && form_insert_point_y_15.Size() > 0) AddPoint(graphic_select, (floor(atof(form_insert_point_x_15.Buffer())*100 + 0.5)/100), (floor(atof(form_insert_point_y_15.Buffer())*100 + 0.5)/100));
		}
		if(submit_insert_point_16.Size() > 0) {
			
			FString form_insert_point_x_16;		
			form_insert_point_x_16.SetSize(0);
			if (hStream.Switch("InputCommands.form_insert_point_x_16")){
				hStream.Seek(0);
				hStream.GetToken(form_insert_point_x_16, "");
				hStream.Switch((uint32)0);
			}
			FString form_insert_point_y_16;		
			form_insert_point_y_16.SetSize(0);
			if (hStream.Switch("InputCommands.form_insert_point_y_16")){
				hStream.Seek(0);
				hStream.GetToken(form_insert_point_y_16, "");
				hStream.Switch((uint32)0);
			}
			
			if(form_insert_point_x_16.Size() > 0 && form_insert_point_y_16.Size() > 0) AddPoint(graphic_select, (floor(atof(form_insert_point_x_16.Buffer())*100 + 0.5)/100), (floor(atof(form_insert_point_y_16.Buffer())*100 + 0.5)/100));
		}
		if(submit_insert_point_17.Size() > 0) {
			
			FString form_insert_point_x_17;		
			form_insert_point_x_17.SetSize(0);
			if (hStream.Switch("InputCommands.form_insert_point_x_17")){
				hStream.Seek(0);
				hStream.GetToken(form_insert_point_x_17, "");
				hStream.Switch((uint32)0);
			}
			FString form_insert_point_y_17;		
			form_insert_point_y_17.SetSize(0);
			if (hStream.Switch("InputCommands.form_insert_point_y_17")){
				hStream.Seek(0);
				hStream.GetToken(form_insert_point_y_17, "");
				hStream.Switch((uint32)0);
			}
			
			if(form_insert_point_x_17.Size() > 0 && form_insert_point_y_17.Size() > 0) AddPoint(graphic_select, (floor(atof(form_insert_point_x_17.Buffer())*100 + 0.5)/100), (floor(atof(form_insert_point_y_17.Buffer())*100 + 0.5)/100));
		}
		
	//	FILE OPERATIONS
		FString form_load_directory_select;
		form_load_directory_select.SetSize(0);
		if (hStream.Switch("InputCommands.form_load_directory_select")){
			hStream.Seek(0);
			hStream.GetToken(form_load_directory_select, "");
			hStream.Switch((uint32)0);
		}
		if(form_load_directory_select.Size() > 0){
			selected_load_directory = form_load_directory_select.Buffer();
			selected_load_file = "";
		} 

		FString form_load_file_name;
		form_load_file_name.SetSize(0);
		if (hStream.Switch("InputCommands.form_load_file_name")){
			hStream.Seek(0);
			hStream.GetToken(form_load_file_name, "");
			hStream.Switch((uint32)0);
		}
		if(form_load_file_name.Size() > 0) selected_load_file = form_load_file_name.Buffer();
	
		temp_string_1 = "";
		FString form_load_file;		
		form_load_file.SetSize(0);
		if (hStream.Switch("InputCommands.form_load_file")){
			hStream.Seek(0);
			hStream.GetToken(form_load_file, "");
			hStream.Switch((uint32)0);
		}
		if(form_load_file.Size() > 0 && selected_load_directory.Size() > temp_string_1.Size() && selected_load_file.Size() > 4) {

			temp_string_1 = config_files_directory.Buffer();
			temp_string_1 += form_load_directory_select.Buffer();
			temp_string_1 += "/";
			temp_string_1 += form_load_file_name.Buffer();

			CStaticAssertErrorCondition(InitialisationError,"MainConfigurator::%s, file to load URL == %s",this->Name(),temp_string_1.Buffer());
			this->ReadConfigurationFile((char *)temp_string_1.Buffer());
		}

		FString form_save_file_as;		
		form_save_file_as.SetSize(0);
		if (hStream.Switch("InputCommands.form_save_file_as")){
			hStream.Seek(0);
			hStream.GetToken(form_save_file_as, "");
			hStream.Switch((uint32)0);
		}
		if(form_save_file_as.Size() > 0) save_as_bool = True;

		if (save_as_bool){
			
			FString form_save_file;		
			form_save_file.SetSize(0);
			if (hStream.Switch("InputCommands.form_save_file")){
				hStream.Seek(0);
				hStream.GetToken(form_save_file, "");
				hStream.Switch((uint32)0);
			}
			if(form_save_file.Size() > 0){
				
				FString form_save_filename;		
				form_save_filename.SetSize(0);
				if (hStream.Switch("InputCommands.form_save_filename")){
					hStream.Seek(0);
					hStream.GetToken(form_save_filename, "");
					hStream.Switch((uint32)0);
				}
				if(form_save_filename.Size() > 0) {
						
					temp_string_1.Printf(config_files_directory.Buffer());
					temp_string_1.Printf(form_save_filename.Buffer());
					
					WriteConfigurationFileWithChanges((char *)default_config_file_path.Buffer(), (char *)temp_string_1.Buffer());				
					save_as_bool = False;
				}
			}
		}
		
		FString form_save_file_and_send;		
		form_save_file_and_send.SetSize(0);
		if (hStream.Switch("InputCommands.form_save_file_and_send")){
			hStream.Seek(0);
			hStream.GetToken(form_save_file_and_send, "");
			hStream.Switch((uint32)0);
		}
		if(form_save_file_and_send.Size() > 0){

			WriteConfigurationFileWithChanges((char *)default_config_file_path.Buffer(), (char *)default_config_file_path.Buffer());				
			
			File temporary_file;
			if(!temporary_file.OpenRead(default_config_file_path.Buffer())){
				CStaticAssertErrorCondition(InitialisationError, "Failed opening File %s", default_config_file_path.Buffer() );
				return False;
			}

			//Try to upload the file to MARTe
			GCRTemplate<MessageEnvelope> envelope(GCFT_Create);
			GCRTemplate<Message>         message(GCFT_Create);                
			message->Init(0, "ChangeConfigFile");
	  
			//Insert the cdb
			ConfigurationDataBase level1CDB;
	//        cfgFile.Seek(0);
			level1CDB->ReadFromStream(temporary_file);
			FString value = "True";
			FString key   = "RebuildAll";
			level1CDB->MoveToRoot();
			CDBExtended cdbext(level1CDB);
			cdbext.WriteFString(value, key.Buffer());
		   

			message->Insert(level1CDB);
			//Send the message
			envelope->PrepareMessageEnvelope(message, marteLocation.Buffer(), MDRF_ManualReply, this);

			GCRTemplate<MessageEnvelope>   reply;
			//prepare the reply
			SendMessageAndWait(envelope, reply, TimeoutType(10000));
			if(!reply.IsValid()){
				AssertErrorCondition(Warning, "Reply from %s isn't valid!", marteLocation.Buffer());
				hmStream.SSPrintf(HtmlTagStreamMode, "h1");
				hmStream.Printf("Reply from %s isn't valid!\n", marteLocation.Buffer());
				hmStream.SSPrintf(HtmlTagStreamMode, "/h1");
			}
			else{
				GCRTemplate<Message> replyMessage = reply->GetMessage();
				if(!replyMessage.IsValid()){
					AssertErrorCondition(Warning, "The reply message from %s is not valid", marteLocation.Buffer());
				}
				else{
					hmStream.SSPrintf(HtmlTagStreamMode, "SCRIPT LANGUAGE=\"javascript\"");
					hmStream.SSPrintf(HtmlTagStreamMode,"!----"); 
					hmStream.Printf("alert('FileWasSuccessfullyCommitted')");
					hmStream.SSPrintf(HtmlTagStreamMode, "!-- END --></SCRIPT");
				}
			}
		}
	}
	this->already_started =True;

	PrintHTTPForm(hmStream);

    hStream.SSPrintf("OutputHttpOtions.Content-Type","text/html");
    hStream.WriteReplyHeader(True);
    return True;
}

bool AdvancedConfigurator::ReadConfigurationFile(char FilePath[]){
	
//	CStaticAssertErrorCondition(InitialisationError,"AdvancedConfigurator::ObjectLoadSetup: FilePath = %s",FilePath);
	File temp_file;
	FString temp_fstring;
	int i;

	ConfigurationDataBase cdb;

   if(!temp_file.OpenRead(FilePath)){
        CStaticAssertErrorCondition(InitialisationError, "ReadWaveformFiles: Failed opening File %s", FilePath );
        return False;
    }
	
	cdb->ReadFromStream(temp_file);
	CDBExtended cdbe(cdb);

	FString fstring_temp;
	
	
//	read real time thread configuration	
	if(!cdbe->Move("+MARTe.+RealTimeThread"))
	{
		CStaticAssertErrorCondition(InitialisationError,"AdvancedConfigurator::ReadConfigurationFile: %s Could not move to \"+MARTe.+RealTimeThread\"",this->Name());
		temp_file.Close();
		return False;
	}	
		if(!cdbe.ReadInt32(thread_priority, "ThreadPriority"))
		{
			AssertErrorCondition(InitialisationError,"AdvancedConfigurator::ReadConfigurationFile: %s thread_priority",this->Name());
			return False;
		}
		else AssertErrorCondition(Information,"AdvancedConfigurator::ReadConfigurationFile: thread_priority = %d",thread_priority);
		if(!cdbe.ReadInt32(run_on_cpu, "RunOnCPU"))
		{
			AssertErrorCondition(InitialisationError,"AdvancedConfigurator::ReadConfigurationFile: %s run_on_cpu",this->Name());
			return False;
		}
		else AssertErrorCondition(Information,"AdvancedConfigurator::ReadConfigurationFile: run_on_cpu = %d",run_on_cpu);
	
	cdbe->MoveToRoot();	
	
//	read timewindows	
	if(!cdbe->Move("+MARTe.+RealTimeThread.+time_windows"))
	{
		CStaticAssertErrorCondition(InitialisationError,"AdvancedConfigurator::ReadConfigurationFile: %s Could not move to \"+MARTe.+RealTimeThread.+time_windows\"",this->Name());
		temp_file.Close();
		return False;
	}	
		if(!cdbe.ReadInt32(usec_pre_pulse_time, "usec_pre_pulse_time"))
		{
			AssertErrorCondition(InitialisationError,"AdvancedConfigurator::ReadConfigurationFile: %s usec_pre_pulse_time",this->Name());
			return False;
		}
		else AssertErrorCondition(Information,"AdvancedConfigurator::ReadConfigurationFile: usec_pre_pulse_time = %d",usec_pre_pulse_time);
		if(!cdbe.ReadInt32(maximum_inversion_usectime, "maximum_inversion_usectime"))
		{
			AssertErrorCondition(InitialisationError,"AdvancedConfigurator::ReadConfigurationFile: %s maximum_inversion_usectime",this->Name());
			return False;
		}
		else AssertErrorCondition(Information,"AdvancedConfigurator::ReadConfigurationFile: maximum_inversion_usectime = %d",maximum_inversion_usectime);
		if(!cdbe.ReadInt32(puffing_mode, "puffing_mode"))
		{
			AssertErrorCondition(InitialisationError,"AdvancedConfigurator::ReadConfigurationFile: %s puffing_mode",this->Name());
			return False;
		}
		else AssertErrorCondition(Information,"AdvancedConfigurator::ReadConfigurationFile: puffing_mode = %d",puffing_mode);
		if(!cdbe.ReadInt32(i, "end_discharge_after_unsuccess_bool"))
		{
			CStaticAssertErrorCondition(InitialisationError,"AdvancedConfigurator::ReadConfigurationFile: %s end_discharge_after_unsuccess_bool",this->Name());
			temp_file.Close();
			return False;
		}
		else 
		{
			end_discharge_after_unsuccess_bool = (bool)i;
			CStaticAssertErrorCondition(Information,"AdvancedConfigurator::ReadConfigurationFile: end_discharge_after_unsuccess_bool = %d",end_discharge_after_unsuccess_bool);
		}	
		if(!cdbe.ReadInt32(time_between_online_and_discharge, "time_between_online_and_discharge"))
		{
			AssertErrorCondition(InitialisationError,"AdvancedConfigurator::ReadConfigurationFile: %s time_between_online_and_discharge",this->Name());
			return False;
		}
		else AssertErrorCondition(Information,"AdvancedConfigurator::ReadConfigurationFile: time_between_online_and_discharge = %d",time_between_online_and_discharge);
	
	cdbe->MoveToRoot();	

//	read control
	if(!cdbe->Move("+MARTe.+RealTimeThread.+controller"))
	{
		CStaticAssertErrorCondition(InitialisationError,"AdvancedConfigurator::ReadConfigurationFile: %s Could not move to \"+MARTe.+RealTimeThread.+controller\"",this->Name());
		temp_file.Close();
		return False;
	}	
		if(!cdbe.ReadFString(control_file_to_load, "file_to_load"))
		{
			CStaticAssertErrorCondition(InitialisationError,"WaveformsDisplay::ObjectLoadSetup: %s Could not control_file_to_load",this->Name());
			return False;
		}
		else CStaticAssertErrorCondition(Information,"AdvancedConfigurator::ReadConfigurationFile: control_file_to_load = %s",control_file_to_load.Buffer());
		if(!cdbe.ReadFloat(puffing_duration_in_puffing_feedback_in_ms, "puffing_duration_in_puffing_feedback_in_ms"))
		{
			CStaticAssertErrorCondition(InitialisationError,"AdvancedConfigurator::ReadConfigurationFile: %s puffing_duration_in_puffing_feedback_in_ms",this->Name());
			temp_file.Close();
			return False;
		}
		else	CStaticAssertErrorCondition(Information,"AdvancedConfigurator::ReadConfigurationFile: puffing_duration_in_puffing_feedback_in_ms = %f",puffing_duration_in_puffing_feedback_in_ms);
		if(!cdbe.ReadFloat(maximum_idle_time_in_puffing_feedback_in_ms, "maximum_idle_time_in_puffing_feedback_in_ms"))
		{
			CStaticAssertErrorCondition(InitialisationError,"AdvancedConfigurator::ReadConfigurationFile: %s maximum_idle_time_in_puffing_feedback_in_ms",this->Name());
			temp_file.Close();
			return False;
		}
		else	CStaticAssertErrorCondition(Information,"AdvancedConfigurator::ReadConfigurationFile: maximum_idle_time_in_puffing_feedback_in_ms = %f",maximum_idle_time_in_puffing_feedback_in_ms);
		if(!cdbe.ReadFloat(minimum_idle_time_in_puffing_feedback_in_ms, "minimum_idle_time_in_puffing_feedback_in_ms"))
		{
			CStaticAssertErrorCondition(InitialisationError,"AdvancedConfigurator::ReadConfigurationFile: %s minimum_idle_time_in_puffing_feedback_in_ms",this->Name());
			temp_file.Close();
			return False;
		}
		else	CStaticAssertErrorCondition(Information,"AdvancedConfigurator::ReadConfigurationFile: minimum_idle_time_in_puffing_feedback_in_ms = %f",minimum_idle_time_in_puffing_feedback_in_ms);
		if(!cdbe.ReadFloat(puffing_feedback_usec_change_percentage_by_cycle, "puffing_feedback_usec_change_percentage_by_cycle"))
		{
			CStaticAssertErrorCondition(InitialisationError,"AdvancedConfigurator::ReadConfigurationFile: %s puffing_feedback_usec_change_percentage_by_cycle",this->Name());
			temp_file.Close();
			return False;
		}
		else	CStaticAssertErrorCondition(Information,"AdvancedConfigurator::ReadConfigurationFile: puffing_feedback_usec_change_percentage_by_cycle = %f",puffing_feedback_usec_change_percentage_by_cycle);
		if(!cdbe.ReadInt32(puffing_feedback_mode, "puffing_feedback_mode"))
		{
			AssertErrorCondition(InitialisationError,"AdvancedConfigurator::ReadConfigurationFile: %s puffing_feedback_mode",this->Name());
			return False;
		}
		else AssertErrorCondition(Information,"AdvancedConfigurator::ReadConfigurationFile: puffing_feedback_mode = %d",puffing_feedback_mode);
	
	cdbe->MoveToRoot();	

//	read +machine_protection
	if(!cdbe->Move("+MARTe.+RealTimeThread.+machine_protection"))
	{
		CStaticAssertErrorCondition(InitialisationError,"AdvancedConfigurator::ReadConfigurationFile: %s Could not move to \"+MARTe.+RealTimeThread.+machine_protection\"",this->Name());
		temp_file.Close();
		return False;
	}	
		if(!cdbe.ReadFloat(iron_core_saturation_value, "iron_core_saturation_value"))
		{
			CStaticAssertErrorCondition(InitialisationError,"AdvancedConfigurator::ReadConfigurationFile: %s iron_core_saturation_value",this->Name());
			temp_file.Close();
			return False;
		}
		else	CStaticAssertErrorCondition(Information,"AdvancedConfigurator::ReadConfigurationFile: iron_core_saturation_value = %f",iron_core_saturation_value);
		if(!cdbe.ReadFloat(iron_core_dangerous_value, "iron_core_dangerous_value"))
		{
			CStaticAssertErrorCondition(InitialisationError,"AdvancedConfigurator::ReadConfigurationFile: %s iron_core_dangerous_value",this->Name());
			temp_file.Close();
			return False;
		}
		else	CStaticAssertErrorCondition(Information,"AdvancedConfigurator::ReadConfigurationFile: iron_core_dangerous_value = %f",iron_core_dangerous_value);
	
	cdbe->MoveToRoot();	

//	read +tomography
	if(!cdbe->Move("+MARTe.+RealTimeThread.+tomography"))
	{
		CStaticAssertErrorCondition(InitialisationError,"AdvancedConfigurator::ReadConfigurationFile: %s Could not move to \"+MARTe.+RealTimeThread.+tomography\"",this->Name());
		temp_file.Close();
		return False;
	}	
		if(!cdbe.ReadFString(tomography_file_to_load, "file_to_load"))
		{
			CStaticAssertErrorCondition(InitialisationError,"WaveformsDisplay::ObjectLoadSetup: %s Could not tomography_file_to_load",this->Name());
			return False;
		}
		else CStaticAssertErrorCondition(Information,"AdvancedConfigurator::ReadConfigurationFile: tomography_file_to_load = %s",tomography_file_to_load.Buffer());
		if(!cdbe.ReadInt32(tomography_n_channels, "nch"))
		{
			AssertErrorCondition(InitialisationError,"AdvancedConfigurator::ReadConfigurationFile: %s tomography_n_channels",this->Name());
			return False;
		}
		else AssertErrorCondition(Information,"AdvancedConfigurator::ReadConfigurationFile: tomography_n_channels = %d",tomography_n_channels);
	
		tomography_online_channels =new int[tomography_n_channels];

		if(!cdbe.ReadInt32Array(tomography_online_channels, (int *)(&tomography_n_channels), 1, "onlineChannels"))
		{
			CStaticAssertErrorCondition(InitialisationError,"ReadWaveformFiles: Could not read tomography_online_channels");
			temp_file.Close();
			return False;
		}
		else for(i=0;i<tomography_n_channels;i++) CStaticAssertErrorCondition(Information,"AdvancedConfigurator::ReadConfigurationFile: tomography_online_channels[%d] = %d",i, tomography_online_channels[i]);
	
	cdbe->MoveToRoot();	
					
//	read maximum values	
	if(!cdbe->Move("+MARTe.+RealTimeThread.+waveform_primary.waveform_mode_1_positive"))
	{
		CStaticAssertErrorCondition(InitialisationError,"AdvancedConfigurator::ReadConfigurationFile: %s Could not move to \"+MARTe.+RealTimeThread.+waveform_primary.waveform_mode_1_positive\"",this->Name());
		temp_file.Close();
		return False;
	}	
		if(!cdbe.ReadFloat(primary_1_p_max_value, "max_value"))
		{
			CStaticAssertErrorCondition(InitialisationError,"AdvancedConfigurator::ReadConfigurationFile: %s primary_1_p_max_value",this->Name());
			temp_file.Close();
			return False;
		}
		else	CStaticAssertErrorCondition(Information,"AdvancedConfigurator::ReadConfigurationFile: primary_1_p_max_value = %f",primary_1_p_max_value);
		if(!cdbe.ReadFloat(primary_1_p_min_value, "min_value"))
		{
			CStaticAssertErrorCondition(InitialisationError,"AdvancedConfigurator::ReadConfigurationFile: %s primary_1_p_min_value",this->Name());
			temp_file.Close();
			return False;
		}
		else	CStaticAssertErrorCondition(Information,"AdvancedConfigurator::ReadConfigurationFile: primary_1_p_min_value = %f",primary_1_p_min_value);

	cdbe->MoveToRoot();	
		
	if(!cdbe->Move("+MARTe.+RealTimeThread.+waveform_primary.waveform_mode_2_positive"))
	{
		CStaticAssertErrorCondition(InitialisationError,"AdvancedConfigurator::ReadConfigurationFile: %s Could not move to \"+MARTe.+RealTimeThread.+waveform_primary.waveform_mode_2_positive\"",this->Name());
		temp_file.Close();
		return False;
	}	
		if(!cdbe.ReadFloat(primary_2_p_max_value, "max_value"))
		{
			CStaticAssertErrorCondition(InitialisationError,"AdvancedConfigurator::ReadConfigurationFile: %s primary_2_p_max_value",this->Name());
			temp_file.Close();
			return False;
		}
		else	CStaticAssertErrorCondition(Information,"AdvancedConfigurator::ReadConfigurationFile: primary_2_p_max_value = %f",primary_2_p_max_value);
		if(!cdbe.ReadFloat(primary_2_p_min_value, "min_value"))
		{
			CStaticAssertErrorCondition(InitialisationError,"AdvancedConfigurator::ReadConfigurationFile: %s primary_2_p_min_value",this->Name());
			temp_file.Close();
			return False;
		}
		else	CStaticAssertErrorCondition(Information,"AdvancedConfigurator::ReadConfigurationFile: primary_2_p_min_value = %f",primary_2_p_min_value);
	
	cdbe->MoveToRoot();	
	
	if(!cdbe->Move("+MARTe.+RealTimeThread.+waveform_vertical.waveform_mode_1_positive"))
	{
		CStaticAssertErrorCondition(InitialisationError,"AdvancedConfigurator::ReadConfigurationFile: %s Could not move to \"+MARTe.+RealTimeThread.+waveform_vertical.waveform_mode_1_positive\"",this->Name());
		temp_file.Close();
		return False;
	}
		if(!cdbe.ReadFloat(vertical_1_p_max_value, "max_value"))
		{
			CStaticAssertErrorCondition(InitialisationError,"AdvancedConfigurator::ReadConfigurationFile: %s vertical_1_p_max_value",this->Name());
			temp_file.Close();
			return False;
		}
		else	CStaticAssertErrorCondition(Information,"AdvancedConfigurator::ReadConfigurationFile: vertical_1_p_max_value = %f",vertical_1_p_max_value);
		if(!cdbe.ReadFloat(vertical_1_p_min_value, "min_value"))
		{
			CStaticAssertErrorCondition(InitialisationError,"AdvancedConfigurator::ReadConfigurationFile: %s vertical_1_p_min_value",this->Name());
			temp_file.Close();
			return False;
		}
		else	CStaticAssertErrorCondition(Information,"AdvancedConfigurator::ReadConfigurationFile: vertical_1_p_min_value = %f",vertical_1_p_min_value);
		
	cdbe->MoveToRoot();	

	if(!cdbe->Move("+MARTe.+RealTimeThread.+waveform_vertical.waveform_mode_2_positive"))
	{
		CStaticAssertErrorCondition(InitialisationError,"AdvancedConfigurator::ReadConfigurationFile: %s Could not move to \"+MARTe.+RealTimeThread.+waveform_vertical.waveform_mode_2_positive\"",this->Name());
		temp_file.Close();
		return False;
	}
		if(!cdbe.ReadFloat(vertical_2_p_max_value, "max_value"))
		{
			CStaticAssertErrorCondition(InitialisationError,"AdvancedConfigurator::ReadConfigurationFile: %s vertical_2_p_max_value",this->Name());
			temp_file.Close();
			return False;
		}
		else	CStaticAssertErrorCondition(Information,"AdvancedConfigurator::ReadConfigurationFile: vertical_2_p_max_value = %f",vertical_2_p_max_value);
		if(!cdbe.ReadFloat(vertical_2_p_min_value, "min_value"))
		{
			CStaticAssertErrorCondition(InitialisationError,"AdvancedConfigurator::ReadConfigurationFile: %s vertical_2_p_min_value",this->Name());
			temp_file.Close();
			return False;
		}
		else	CStaticAssertErrorCondition(Information,"AdvancedConfigurator::ReadConfigurationFile: vertical_2_p_min_value = %f",vertical_2_p_min_value);
		
	cdbe->MoveToRoot();	

	if(!cdbe->Move("+MARTe.+RealTimeThread.+waveform_horizontal.waveform_mode_1_positive"))
	{
		CStaticAssertErrorCondition(InitialisationError,"AdvancedConfigurator::ReadConfigurationFile: %s Could not move to \"+MARTe.+RealTimeThread.+waveform_horizontal.waveform_mode_1_positive\"",this->Name());
		temp_file.Close();
		return False;
	}
		if(!cdbe.ReadFloat(horizontal_1_p_max_value, "max_value"))
		{
			CStaticAssertErrorCondition(InitialisationError,"AdvancedConfigurator::ReadConfigurationFile: %s horizontal_1_p_max_value",this->Name());
			temp_file.Close();
			return False;
		}
		else	CStaticAssertErrorCondition(Information,"AdvancedConfigurator::ReadConfigurationFile: horizontal_1_p_max_value = %f",horizontal_1_p_max_value);
		if(!cdbe.ReadFloat(horizontal_1_p_min_value, "min_value"))
		{
			CStaticAssertErrorCondition(InitialisationError,"AdvancedConfigurator::ReadConfigurationFile: %s horizontal_1_p_min_value",this->Name());
			temp_file.Close();
			return False;
		}
		else	CStaticAssertErrorCondition(Information,"AdvancedConfigurator::ReadConfigurationFile: horizontal_1_p_min_value = %f",horizontal_1_p_min_value);
		
	cdbe->MoveToRoot();	
	
	if(!cdbe->Move("+MARTe.+RealTimeThread.+waveform_horizontal.waveform_mode_2_positive"))
	{
		CStaticAssertErrorCondition(InitialisationError,"AdvancedConfigurator::ReadConfigurationFile: %s Could not move to \"+MARTe.+RealTimeThread.+waveform_horizontal.waveform_mode_2_positive\"",this->Name());
		temp_file.Close();
		return False;
	}
		if(!cdbe.ReadFloat(horizontal_2_p_max_value, "max_value"))
		{
			CStaticAssertErrorCondition(InitialisationError,"AdvancedConfigurator::ReadConfigurationFile: %s horizontal_2_p_max_value",this->Name());
			temp_file.Close();
			return False;
		}
		else	CStaticAssertErrorCondition(Information,"AdvancedConfigurator::ReadConfigurationFile: horizontal_2_p_max_value = %f",horizontal_2_p_max_value);
		if(!cdbe.ReadFloat(horizontal_2_p_min_value, "min_value"))
		{
			CStaticAssertErrorCondition(InitialisationError,"AdvancedConfigurator::ReadConfigurationFile: %s horizontal_2_p_min_value",this->Name());
			temp_file.Close();
			return False;
		}
		else	CStaticAssertErrorCondition(Information,"AdvancedConfigurator::ReadConfigurationFile: horizontal_2_p_min_value = %f",horizontal_2_p_min_value);
		
	cdbe->MoveToRoot();	

	if(!cdbe->Move("+MARTe.+RealTimeThread.+waveform_toroidal.waveform_mode_1_positive"))
	{
		CStaticAssertErrorCondition(InitialisationError,"AdvancedConfigurator::ReadConfigurationFile: %s Could not move to \"+MARTe.+RealTimeThread.+waveform_toroidal.waveform_mode_2_positive\"",this->Name());
		temp_file.Close();
		return False;
	}
		if(!cdbe.ReadFloat(toroidal_1_p_max_value, "max_value"))
		{
			CStaticAssertErrorCondition(InitialisationError,"AdvancedConfigurator::ReadConfigurationFile: %s toroidal_1_p_max_value",this->Name());
			temp_file.Close();
			return False;
		}
		else	CStaticAssertErrorCondition(Information,"AdvancedConfigurator::ReadConfigurationFile: toroidal_1_p_max_value = %f",toroidal_1_p_max_value);
		if(!cdbe.ReadFloat(toroidal_1_p_min_value, "min_value"))
		{
			CStaticAssertErrorCondition(InitialisationError,"AdvancedConfigurator::ReadConfigurationFile: %s toroidal_1_p_min_value",this->Name());
			temp_file.Close();
			return False;
		}
		else	CStaticAssertErrorCondition(Information,"AdvancedConfigurator::ReadConfigurationFile: toroidal_1_p_min_value = %f",toroidal_1_p_min_value);
		
	cdbe->MoveToRoot();	

	if(!cdbe->Move("+MARTe.+RealTimeThread.+waveform_puffing.waveform_mode_1_positive"))
	{
		CStaticAssertErrorCondition(InitialisationError,"AdvancedConfigurator::ReadConfigurationFile: %s Could not move to \"+MARTe.+RealTimeThread.+waveform_puffing.waveform_mode_1_positive\"",this->Name());
		temp_file.Close();
		return False;
	}
		if(!cdbe.ReadFloat(puffing_1_p_max_value, "max_value"))
		{
			CStaticAssertErrorCondition(InitialisationError,"AdvancedConfigurator::ReadConfigurationFile: %s puffing_1_p_max_value",this->Name());
			temp_file.Close();
			return False;
		}
		else	CStaticAssertErrorCondition(Information,"AdvancedConfigurator::ReadConfigurationFile: puffing_1_p_max_value = %f",puffing_1_p_max_value);
		if(!cdbe.ReadFloat(puffing_1_p_min_value, "min_value"))
		{
			CStaticAssertErrorCondition(InitialisationError,"AdvancedConfigurator::ReadConfigurationFile: %s puffing_1_p_min_value",this->Name());
			temp_file.Close();
			return False;
		}
		else	CStaticAssertErrorCondition(Information,"AdvancedConfigurator::ReadConfigurationFile: puffing_1_p_min_value = %f",puffing_1_p_min_value);
		
	cdbe->MoveToRoot();	

	if(!cdbe->Move("+MARTe.+RealTimeThread.+waveform_puffing.waveform_mode_2_positive"))
	{
		CStaticAssertErrorCondition(InitialisationError,"AdvancedConfigurator::ReadConfigurationFile: %s Could not move to \"+MARTe.+RealTimeThread.+waveform_puffing.waveform_mode_2_positive\"",this->Name());
		temp_file.Close();
		return False;
	}
		if(!cdbe.ReadFloat(puffing_2_p_max_value, "max_value"))
		{
			CStaticAssertErrorCondition(InitialisationError,"AdvancedConfigurator::ReadConfigurationFile: %s puffing_2_p_max_value",this->Name());
			temp_file.Close();
			return False;
		}
		else	CStaticAssertErrorCondition(Information,"AdvancedConfigurator::ReadConfigurationFile: puffing_2_p_max_value = %f",puffing_2_p_max_value);
		if(!cdbe.ReadFloat(puffing_2_p_min_value, "min_value"))
		{
			CStaticAssertErrorCondition(InitialisationError,"AdvancedConfigurator::ReadConfigurationFile: %s puffing_2_p_min_value",this->Name());
			temp_file.Close();
			return False;
		}
		else	CStaticAssertErrorCondition(Information,"AdvancedConfigurator::ReadConfigurationFile: puffing_2_p_min_value = %f",puffing_2_p_min_value);
		
	cdbe->MoveToRoot();		

// read config file section: waveform_primary
	if(!cdbe->Move("+MARTe.+RealTimeThread.+waveform_primary.waveform_breakdown"))
	{
		CStaticAssertErrorCondition(InitialisationError,"AdvancedConfigurator::ReadConfigurationFile: %s Could not move to \"+MARTe.+RealTimeThread.+waveform_primary.waveform_breakdown\"",this->Name());
		temp_file.Close();
		return False;
	}
		if(!cdbe.ReadInt32(primary_breakdown_vector_size, "vector_size"))
		{
			CStaticAssertErrorCondition(InitialisationError,"AdvancedConfigurator::ReadConfigurationFile: %s primary_breakdown_vector_size",this->Name());
			temp_file.Close();
			return False;
		}
		else	CStaticAssertErrorCondition(Information,"AdvancedConfigurator::ReadConfigurationFile: primary_breakdown_vector_size = %d",primary_breakdown_vector_size);

		primary_breakdown_index_vector =new float[primary_breakdown_vector_size];
		primary_breakdown_data_vector =new float[primary_breakdown_vector_size];

		if(!cdbe.ReadFloatArray(primary_breakdown_index_vector, (int *)(&primary_breakdown_vector_size), 1, "index_vector"))
		{
			CStaticAssertErrorCondition(InitialisationError,"ReadWaveformFiles: Could not read primary_breakdown_index_vector");
			temp_file.Close();
			return False;
		}
		else for(i=0;i<primary_breakdown_vector_size;i++) CStaticAssertErrorCondition(Information,"AdvancedConfigurator::ReadConfigurationFile: primary_breakdown_index_vector[%d] = %f",i, primary_breakdown_index_vector[i]);
		if(!cdbe.ReadFloatArray(primary_breakdown_data_vector, (int *)(&primary_breakdown_vector_size), 1, "data_vector"))
		{
			CStaticAssertErrorCondition(InitialisationError,"ReadWaveformFiles: Could not read primary_breakdown_data_vector");
			temp_file.Close();
			return False;
		}
		else for(i=0;i<primary_breakdown_vector_size;i++) CStaticAssertErrorCondition(Information,"AdvancedConfigurator::ReadConfigurationFile: primary_breakdown_data_vector[%d] = %f",i, primary_breakdown_data_vector[i]);
		
	cdbe->MoveToRoot();	
	
	if(!cdbe->Move("+MARTe.+RealTimeThread.+waveform_primary.waveform_breakdown_negative"))
	{
		CStaticAssertErrorCondition(InitialisationError,"AdvancedConfigurator::ReadConfigurationFile: %s Could not move to \"+MARTe.+RealTimeThread.+waveform_primary.waveform_breakdown_negative\"",this->Name());
		temp_file.Close();
		return False;
	}
		if(!cdbe.ReadInt32(primary_breakdown_negative_vector_size, "vector_size"))
		{
			CStaticAssertErrorCondition(InitialisationError,"AdvancedConfigurator::ReadConfigurationFile: %s primary_breakdown_negative_vector_size",this->Name());
			temp_file.Close();
			return False;
		}
		else	CStaticAssertErrorCondition(Information,"AdvancedConfigurator::ReadConfigurationFile: primary_breakdown_negative_vector_size = %d",primary_breakdown_negative_vector_size);

		primary_breakdown_negative_index_vector =new float[primary_breakdown_negative_vector_size];
		primary_breakdown_negative_data_vector =new float[primary_breakdown_negative_vector_size];

		if(!cdbe.ReadFloatArray(primary_breakdown_negative_index_vector, (int *)(&primary_breakdown_negative_vector_size), 1, "index_vector"))
		{
			CStaticAssertErrorCondition(InitialisationError,"ReadWaveformFiles: Could not primary_breakdown_negative_index_vector");
			temp_file.Close();
			return False;
		}
		else for(i=0;i<primary_breakdown_negative_vector_size;i++) CStaticAssertErrorCondition(Information,"AdvancedConfigurator::ReadConfigurationFile: primary_breakdown_negative_index_vector[%d] = %f",i, primary_breakdown_negative_index_vector[i]);
		if(!cdbe.ReadFloatArray(primary_breakdown_negative_data_vector, (int *)(&primary_breakdown_negative_vector_size), 1, "data_vector"))
		{
			CStaticAssertErrorCondition(InitialisationError,"ReadWaveformFiles: Could not read primary_breakdown_negative_data_vector");
			temp_file.Close();
			return False;
		}
		else for(i=0;i<primary_breakdown_negative_vector_size;i++) CStaticAssertErrorCondition(Information,"AdvancedConfigurator::ReadConfigurationFile: primary_breakdown_negative_data_vector[%d] = %f",i, primary_breakdown_negative_data_vector[i]);
	
	cdbe->MoveToRoot();
	
	if(!cdbe->Move("+MARTe.+RealTimeThread.+waveform_primary.waveform_inversion_positive_to_negative"))
	{
		CStaticAssertErrorCondition(InitialisationError,"AdvancedConfigurator::ReadConfigurationFile: %s Could not move to \"+MARTe.+RealTimeThread.+waveform_primary.waveform_inversion_positive_to_negative\"",this->Name());
		temp_file.Close();
		return False;
	}
		if(!cdbe.ReadInt32(primary_inversion_positive_vector_size, "vector_size"))
		{
			CStaticAssertErrorCondition(InitialisationError,"AdvancedConfigurator::ReadConfigurationFile: %s primary_inversion_positive_vector_size",this->Name());
			temp_file.Close();
			return False;
		}
		else	CStaticAssertErrorCondition(Information,"AdvancedConfigurator::ReadConfigurationFile: primary_inversion_positive_vector_size = %d",primary_inversion_positive_vector_size);

		primary_inversion_positive_index_vector =new float[primary_inversion_positive_vector_size];
		primary_inversion_positive_data_vector =new float[primary_inversion_positive_vector_size];

		if(!cdbe.ReadFloatArray(primary_inversion_positive_index_vector, (int *)(&primary_inversion_positive_vector_size), 1, "index_vector"))
		{
			CStaticAssertErrorCondition(InitialisationError,"ReadWaveformFiles: Could not read primary_inversion_positive_index_vector");
			temp_file.Close();
			return False;
		}
		else for(i=0;i<primary_inversion_positive_vector_size;i++) CStaticAssertErrorCondition(Information,"AdvancedConfigurator::ReadConfigurationFile: primary_inversion_positive_index_vector[%d] = %f",i, primary_inversion_positive_index_vector[i]);
		if(!cdbe.ReadFloatArray(primary_inversion_positive_data_vector, (int *)(&primary_inversion_positive_vector_size), 1, "data_vector"))
		{
			CStaticAssertErrorCondition(InitialisationError,"ReadWaveformFiles: Could not read primary_inversion_positive_data_vector");
			temp_file.Close();
			return False;
		}
		else for(i=0;i<primary_inversion_positive_vector_size;i++) CStaticAssertErrorCondition(Information,"AdvancedConfigurator::ReadConfigurationFile: primary_inversion_positive_data_vector[%d] = %f",i, primary_inversion_positive_data_vector[i]);
		
	cdbe->MoveToRoot();	
	
	if(!cdbe->Move("+MARTe.+RealTimeThread.+waveform_primary.waveform_inversion_negative_to_positive"))
	{
		CStaticAssertErrorCondition(InitialisationError,"AdvancedConfigurator::ReadConfigurationFile: %s Could not move to \"+MARTe.+RealTimeThread.+waveform_primary.waveform_inversion_negative_to_positive\"",this->Name());
		temp_file.Close();
		return False;
	}
		if(!cdbe.ReadInt32(primary_inversion_negative_vector_size, "vector_size"))
		{
			CStaticAssertErrorCondition(InitialisationError,"AdvancedConfigurator::ReadConfigurationFile: %s primary_inversion_negative_vector_size",this->Name());
			temp_file.Close();
			return False;
		}
		else	CStaticAssertErrorCondition(Information,"AdvancedConfigurator::ReadConfigurationFile: primary_inversion_negative_vector_size = %d",primary_inversion_negative_vector_size);

		primary_inversion_negative_index_vector =new float[primary_inversion_negative_vector_size];
		primary_inversion_negative_data_vector =new float[primary_inversion_negative_vector_size];

		if(!cdbe.ReadFloatArray(primary_inversion_negative_index_vector, (int *)(&primary_inversion_negative_vector_size), 1, "index_vector"))
		{
			CStaticAssertErrorCondition(InitialisationError,"ReadWaveformFiles: Could not read primary_inversion_negative_index_vector");
			temp_file.Close();
			return False;
		}
		else for(i=0;i<primary_inversion_negative_vector_size;i++) CStaticAssertErrorCondition(Information,"AdvancedConfigurator::ReadConfigurationFile: primary_inversion_negative_index_vector[%d] = %f",i, primary_inversion_negative_index_vector[i]);
		if(!cdbe.ReadFloatArray(primary_inversion_negative_data_vector, (int *)(&primary_inversion_negative_vector_size), 1, "data_vector"))
		{
			CStaticAssertErrorCondition(InitialisationError,"ReadWaveformFiles: Could not read primary_inversion_negative_data_vector");
			temp_file.Close();
			return False;
		}
		else for(i=0;i<primary_inversion_negative_vector_size;i++) CStaticAssertErrorCondition(Information,"AdvancedConfigurator::ReadConfigurationFile: primary_inversion_negative_data_vector[%d] = %f",i, primary_inversion_negative_data_vector[i]);
	
	cdbe->MoveToRoot();

// read config file section: waveform_vertical
	if(!cdbe->Move("+MARTe.+RealTimeThread.+waveform_vertical.waveform_breakdown"))
	{
		CStaticAssertErrorCondition(InitialisationError,"AdvancedConfigurator::ReadConfigurationFile: %s Could not move to \"+MARTe.+RealTimeThread.+waveform_vertical.waveform_breakdown\"",this->Name());
		temp_file.Close();
		return False;
	}
		if(!cdbe.ReadInt32(vertical_breakdown_vector_size, "vector_size"))
		{
			CStaticAssertErrorCondition(InitialisationError,"AdvancedConfigurator::ReadConfigurationFile: %s vertical_breakdown_vector_size",this->Name());
			temp_file.Close();
			return False;
		}
		else	CStaticAssertErrorCondition(Information,"AdvancedConfigurator::ReadConfigurationFile: vertical_breakdown_vector_size = %d",vertical_breakdown_vector_size);

		vertical_breakdown_index_vector =new float[vertical_breakdown_vector_size];
		vertical_breakdown_data_vector =new float[vertical_breakdown_vector_size];

		if(!cdbe.ReadFloatArray(vertical_breakdown_index_vector, (int *)(&vertical_breakdown_vector_size), 1, "index_vector"))
		{
			CStaticAssertErrorCondition(InitialisationError,"ReadWaveformFiles: Could not read vertical_breakdown_index_vector");
			temp_file.Close();
			return False;
		}
		else for(i=0;i<vertical_breakdown_vector_size;i++) CStaticAssertErrorCondition(Information,"AdvancedConfigurator::ReadConfigurationFile: vertical_breakdown_index_vector[%d] = %f",i, vertical_breakdown_index_vector[i]);
		if(!cdbe.ReadFloatArray(vertical_breakdown_data_vector, (int *)(&vertical_breakdown_vector_size), 1, "data_vector"))
		{
			CStaticAssertErrorCondition(InitialisationError,"ReadWaveformFiles: Could not read vertical_breakdown_data_vector");
			temp_file.Close();
			return False;
		}
		else for(i=0;i<vertical_breakdown_vector_size;i++) CStaticAssertErrorCondition(Information,"AdvancedConfigurator::ReadConfigurationFile: vertical_breakdown_data_vector[%d] = %f",i, vertical_breakdown_data_vector[i]);
		
	cdbe->MoveToRoot();	
	
	if(!cdbe->Move("+MARTe.+RealTimeThread.+waveform_vertical.waveform_breakdown_negative"))
	{
		CStaticAssertErrorCondition(InitialisationError,"AdvancedConfigurator::ReadConfigurationFile: %s Could not move to \"+MARTe.+RealTimeThread.+waveform_vertical.waveform_breakdown_negative\"",this->Name());
		temp_file.Close();
		return False;
	}
		if(!cdbe.ReadInt32(vertical_breakdown_negative_vector_size, "vector_size"))
		{
			CStaticAssertErrorCondition(InitialisationError,"AdvancedConfigurator::ReadConfigurationFile: %s vertical_breakdown_negative_vector_size",this->Name());
			temp_file.Close();
			return False;
		}
		else	CStaticAssertErrorCondition(Information,"AdvancedConfigurator::ReadConfigurationFile: vertical_breakdown_negative_vector_size = %d",vertical_breakdown_negative_vector_size);

		vertical_breakdown_negative_index_vector =new float[vertical_breakdown_negative_vector_size];
		vertical_breakdown_negative_data_vector =new float[vertical_breakdown_negative_vector_size];

		if(!cdbe.ReadFloatArray(vertical_breakdown_negative_index_vector, (int *)(&vertical_breakdown_negative_vector_size), 1, "index_vector"))
		{
			CStaticAssertErrorCondition(InitialisationError,"ReadWaveformFiles: Could not read vertical_breakdown_negative_index_vector");
			temp_file.Close();
			return False;
		}
		else for(i=0;i<vertical_breakdown_negative_vector_size;i++) CStaticAssertErrorCondition(Information,"AdvancedConfigurator::ReadConfigurationFile: vertical_breakdown_negative_index_vector[%d] = %f",i, vertical_breakdown_negative_index_vector[i]);
		if(!cdbe.ReadFloatArray(vertical_breakdown_negative_data_vector, (int *)(&vertical_breakdown_negative_vector_size), 1, "data_vector"))
		{
			CStaticAssertErrorCondition(InitialisationError,"ReadWaveformFiles: Could not read vertical_breakdown_negative_data_vector");
			temp_file.Close();
			return False;
		}
		else for(i=0;i<vertical_breakdown_negative_vector_size;i++) CStaticAssertErrorCondition(Information,"AdvancedConfigurator::ReadConfigurationFile: vertical_breakdown_negative_data_vector[%d] = %f",i, vertical_breakdown_negative_data_vector[i]);
	
	cdbe->MoveToRoot();
	
	if(!cdbe->Move("+MARTe.+RealTimeThread.+waveform_vertical.waveform_inversion_positive_to_negative"))
	{
		CStaticAssertErrorCondition(InitialisationError,"AdvancedConfigurator::ReadConfigurationFile: %s Could not move to \"+MARTe.+RealTimeThread.+waveform_vertical.waveform_mode_2_positive\"",this->Name());
		temp_file.Close();
		return False;
	}
		if(!cdbe.ReadInt32(vertical_inversion_positive_vector_size, "vector_size"))
		{
			CStaticAssertErrorCondition(InitialisationError,"AdvancedConfigurator::ReadConfigurationFile: %s vertical_inversion_positive_vector_size",this->Name());
			temp_file.Close();
			return False;
		}
		else	CStaticAssertErrorCondition(Information,"AdvancedConfigurator::ReadConfigurationFile: vertical_inversion_positive_vector_size = %d",vertical_inversion_positive_vector_size);

		vertical_inversion_positive_index_vector =new float[vertical_inversion_positive_vector_size];
		vertical_inversion_positive_data_vector =new float[vertical_inversion_positive_vector_size];

		if(!cdbe.ReadFloatArray(vertical_inversion_positive_index_vector, (int *)(&vertical_inversion_positive_vector_size), 1, "index_vector"))
		{
			CStaticAssertErrorCondition(InitialisationError,"ReadWaveformFiles: Could not read vertical_inversion_positive_index_vector");
			temp_file.Close();
			return False;
		}
		else for(i=0;i<vertical_inversion_positive_vector_size;i++) CStaticAssertErrorCondition(Information,"AdvancedConfigurator::ReadConfigurationFile: vertical_inversion_positive_index_vector[%d] = %f",i, vertical_inversion_positive_index_vector[i]);
		if(!cdbe.ReadFloatArray(vertical_inversion_positive_data_vector, (int *)(&vertical_inversion_positive_vector_size), 1, "data_vector"))
		{
			CStaticAssertErrorCondition(InitialisationError,"ReadWaveformFiles: Could not read vertical_breakdown_data_vector");
			temp_file.Close();
			return False;
		}
		else for(i=0;i<vertical_inversion_positive_vector_size;i++) CStaticAssertErrorCondition(Information,"AdvancedConfigurator::ReadConfigurationFile: vertical_inversion_positive_data_vector[%d] = %f",i, vertical_inversion_positive_data_vector[i]);
		
	cdbe->MoveToRoot();	
	
	if(!cdbe->Move("+MARTe.+RealTimeThread.+waveform_vertical.waveform_inversion_negative_to_positive"))
	{
		CStaticAssertErrorCondition(InitialisationError,"AdvancedConfigurator::ReadConfigurationFile: %s Could not move to \"+MARTe.+RealTimeThread.+waveform_vertical.waveform_mode_2_negative\"",this->Name());
		temp_file.Close();
		return False;
	}
		if(!cdbe.ReadInt32(vertical_inversion_negative_vector_size, "vector_size"))
		{
			CStaticAssertErrorCondition(InitialisationError,"AdvancedConfigurator::ReadConfigurationFile: %s vertical_inversion_negative_vector_size",this->Name());
			temp_file.Close();
			return False;
		}
		else	CStaticAssertErrorCondition(Information,"AdvancedConfigurator::ReadConfigurationFile: vertical_inversion_negative_vector_size = %d",vertical_inversion_negative_vector_size);

		vertical_inversion_negative_index_vector =new float[vertical_inversion_negative_vector_size];
		vertical_inversion_negative_data_vector =new float[vertical_inversion_negative_vector_size];

		if(!cdbe.ReadFloatArray(vertical_inversion_negative_index_vector, (int *)(&vertical_inversion_negative_vector_size), 1, "index_vector"))
		{
			CStaticAssertErrorCondition(InitialisationError,"ReadWaveformFiles: Could not read vertical_inversion_negative_index_vector");
			temp_file.Close();
			return False;
		}
		else for(i=0;i<vertical_inversion_negative_vector_size;i++) CStaticAssertErrorCondition(Information,"AdvancedConfigurator::ReadConfigurationFile: vertical_inversion_negative_index_vector[%d] = %f",i, vertical_breakdown_negative_index_vector[i]);
		if(!cdbe.ReadFloatArray(vertical_inversion_negative_data_vector, (int *)(&vertical_inversion_negative_vector_size), 1, "data_vector"))
		{
			CStaticAssertErrorCondition(InitialisationError,"ReadWaveformFiles: Could not read vertical_inversion_negative_data_vector");
			temp_file.Close();
			return False;
		}
		else for(i=0;i<vertical_inversion_negative_vector_size;i++) CStaticAssertErrorCondition(Information,"AdvancedConfigurator::ReadConfigurationFile: vertical_inversion_negative_data_vector[%d] = %f",i, vertical_inversion_negative_data_vector[i]);
	
	cdbe->MoveToRoot();

// read config file section: waveform_horizontal
	if(!cdbe->Move("+MARTe.+RealTimeThread.+waveform_horizontal.waveform_breakdown"))
	{
		CStaticAssertErrorCondition(InitialisationError,"AdvancedConfigurator::ReadConfigurationFile: %s Could not move to \"+MARTe.+RealTimeThread.+waveform_horizontal.waveform_mode_1_positive\"",this->Name());
		temp_file.Close();
		return False;
	}
		if(!cdbe.ReadInt32(horizontal_breakdown_vector_size, "vector_size"))
		{
			CStaticAssertErrorCondition(InitialisationError,"AdvancedConfigurator::ReadConfigurationFile: %s horizontal_breakdown_vector_size",this->Name());
			temp_file.Close();
			return False;
		}
		else	CStaticAssertErrorCondition(Information,"AdvancedConfigurator::ReadConfigurationFile: horizontal_breakdown_vector_size = %d",horizontal_breakdown_vector_size);

		horizontal_breakdown_index_vector =new float[horizontal_breakdown_vector_size];
		horizontal_breakdown_data_vector =new float[horizontal_breakdown_vector_size];

		if(!cdbe.ReadFloatArray(horizontal_breakdown_index_vector, (int *)(&horizontal_breakdown_vector_size), 1, "index_vector"))
		{
			CStaticAssertErrorCondition(InitialisationError,"ReadWaveformFiles: Could not read horizontal_breakdown_index_vector");
			temp_file.Close();
			return False;
		}
		else for(i=0;i<horizontal_breakdown_vector_size;i++) CStaticAssertErrorCondition(Information,"AdvancedConfigurator::ReadConfigurationFile: horizontal_breakdown_index_vector[%d] = %f",i, horizontal_breakdown_index_vector[i]);
		if(!cdbe.ReadFloatArray(horizontal_breakdown_data_vector, (int *)(&horizontal_breakdown_vector_size), 1, "data_vector"))
		{
			CStaticAssertErrorCondition(InitialisationError,"ReadWaveformFiles: Could not read horizontal_breakdown_data_vector");
			temp_file.Close();
			return False;
		}
		else for(i=0;i<horizontal_breakdown_vector_size;i++) CStaticAssertErrorCondition(Information,"AdvancedConfigurator::ReadConfigurationFile: horizontal_breakdown_data_vector[%d] = %f",i, horizontal_breakdown_data_vector[i]);
		
	cdbe->MoveToRoot();	
	
	if(!cdbe->Move("+MARTe.+RealTimeThread.+waveform_horizontal.waveform_breakdown_negative"))
	{
		CStaticAssertErrorCondition(InitialisationError,"AdvancedConfigurator::ReadConfigurationFile: %s Could not move to \"+MARTe.+RealTimeThread.+waveform_horizontal.waveform_mode_1_negative\"",this->Name());
		temp_file.Close();
		return False;
	}
		if(!cdbe.ReadInt32(horizontal_breakdown_negative_vector_size, "vector_size"))
		{
			CStaticAssertErrorCondition(InitialisationError,"AdvancedConfigurator::ReadConfigurationFile: %s horizontal_breakdown_negative_vector_size",this->Name());
			temp_file.Close();
			return False;
		}
		else	CStaticAssertErrorCondition(Information,"AdvancedConfigurator::ReadConfigurationFile: horizontal_breakdown_negative_vector_size = %d",horizontal_breakdown_negative_vector_size);

		horizontal_breakdown_negative_index_vector =new float[horizontal_breakdown_negative_vector_size];
		horizontal_breakdown_negative_data_vector =new float[horizontal_breakdown_negative_vector_size];

		if(!cdbe.ReadFloatArray(horizontal_breakdown_negative_index_vector, (int *)(&horizontal_breakdown_negative_vector_size), 1, "index_vector"))
		{
			CStaticAssertErrorCondition(InitialisationError,"ReadWaveformFiles: Could not read horizontal_breakdown_negative_index_vector");
			temp_file.Close();
			return False;
		}
		else for(i=0;i<horizontal_breakdown_negative_vector_size;i++) CStaticAssertErrorCondition(Information,"AdvancedConfigurator::ReadConfigurationFile: horizontal_breakdown_negative_index_vector[%d] = %f",i, horizontal_breakdown_negative_index_vector[i]);
		if(!cdbe.ReadFloatArray(horizontal_breakdown_negative_data_vector, (int *)(&horizontal_breakdown_negative_vector_size), 1, "data_vector"))
		{
			CStaticAssertErrorCondition(InitialisationError,"ReadWaveformFiles: Could not read horizontal_breakdown_negative_data_vector");
			temp_file.Close();
			return False;
		}
		else for(i=0;i<horizontal_breakdown_negative_vector_size;i++) CStaticAssertErrorCondition(Information,"AdvancedConfigurator::ReadConfigurationFile: horizontal_breakdown_negative_data_vector[%d] = %f",i, horizontal_breakdown_negative_data_vector[i]);
	
	cdbe->MoveToRoot();
	
	if(!cdbe->Move("+MARTe.+RealTimeThread.+waveform_horizontal.waveform_inversion_positive_to_negative"))
	{
		CStaticAssertErrorCondition(InitialisationError,"AdvancedConfigurator::ReadConfigurationFile: %s Could not move to \"+MARTe.+RealTimeThread.+waveform_horizontal.waveform_mode_2_positive\"",this->Name());
		temp_file.Close();
		return False;
	}
		if(!cdbe.ReadInt32(horizontal_inversion_positive_vector_size, "vector_size"))
		{
			CStaticAssertErrorCondition(InitialisationError,"AdvancedConfigurator::ReadConfigurationFile: %s horizontal_inversion_positive_vector_size",this->Name());
			temp_file.Close();
			return False;
		}
		else	CStaticAssertErrorCondition(Information,"AdvancedConfigurator::ReadConfigurationFile: horizontal_inversion_positive_vector_size = %d",horizontal_inversion_positive_vector_size);

		horizontal_inversion_positive_index_vector =new float[horizontal_inversion_positive_vector_size];
		horizontal_inversion_positive_data_vector =new float[horizontal_inversion_positive_vector_size];

		if(!cdbe.ReadFloatArray(horizontal_inversion_positive_index_vector, (int *)(&horizontal_inversion_positive_vector_size), 1, "index_vector"))
		{
			CStaticAssertErrorCondition(InitialisationError,"ReadWaveformFiles: Could not read horizontal_inversion_positive_index_vector");
			temp_file.Close();
			return False;
		}
		else for(i=0;i<horizontal_inversion_positive_vector_size;i++) CStaticAssertErrorCondition(Information,"AdvancedConfigurator::ReadConfigurationFile: horizontal_inversion_positive_index_vector[%d] = %f",i, horizontal_inversion_positive_index_vector[i]);
		if(!cdbe.ReadFloatArray(horizontal_inversion_positive_data_vector, (int *)(&horizontal_inversion_positive_vector_size), 1, "data_vector"))
		{
			CStaticAssertErrorCondition(InitialisationError,"ReadWaveformFiles: Could not read horizontal_inversion_positive_data_vector");
			temp_file.Close();
			return False;
		}
		else for(i=0;i<horizontal_inversion_positive_vector_size;i++) CStaticAssertErrorCondition(Information,"AdvancedConfigurator::ReadConfigurationFile: horizontal_inversion_positive_data_vector[%d] = %f",i, horizontal_inversion_positive_data_vector[i]);
		
	cdbe->MoveToRoot();	
	
	if(!cdbe->Move("+MARTe.+RealTimeThread.+waveform_horizontal.waveform_inversion_negative_to_positive"))
	{
		CStaticAssertErrorCondition(InitialisationError,"AdvancedConfigurator::ReadConfigurationFile: %s Could not move to \"+MARTe.+RealTimeThread.+waveform_horizontal.waveform_mode_2_negative\"",this->Name());
		temp_file.Close();
		return False;
	}
		if(!cdbe.ReadInt32(horizontal_inversion_negative_vector_size, "vector_size"))
		{
			CStaticAssertErrorCondition(InitialisationError,"AdvancedConfigurator::ReadConfigurationFile: %s horizontal_inversion_negative_vector_size",this->Name());
			temp_file.Close();
			return False;
		}
		else	CStaticAssertErrorCondition(Information,"AdvancedConfigurator::ReadConfigurationFile: horizontal_inversion_negative_vector_size = %d",horizontal_inversion_negative_vector_size);

		horizontal_inversion_negative_index_vector =new float[horizontal_inversion_negative_vector_size];
		horizontal_inversion_negative_data_vector =new float[horizontal_inversion_negative_vector_size];

		if(!cdbe.ReadFloatArray(horizontal_inversion_negative_index_vector, (int *)(&horizontal_inversion_negative_vector_size), 1, "index_vector"))
		{
			CStaticAssertErrorCondition(InitialisationError,"ReadWaveformFiles: Could not read horizontal_inversion_negative_index_vector");
			temp_file.Close();
			return False;
		}
		else for(i=0;i<horizontal_inversion_negative_vector_size;i++) CStaticAssertErrorCondition(Information,"AdvancedConfigurator::ReadConfigurationFile: horizontal_inversion_negative_index_vector[%d] = %f",i, horizontal_inversion_negative_index_vector[i]);
		if(!cdbe.ReadFloatArray(horizontal_inversion_negative_data_vector, (int *)(&horizontal_inversion_negative_vector_size), 1, "data_vector"))
		{
			CStaticAssertErrorCondition(InitialisationError,"ReadWaveformFiles: Could not read horizontal_inversion_negative_data_vector");
			temp_file.Close();
			return False;
		}
		else for(i=0;i<horizontal_inversion_negative_vector_size;i++) CStaticAssertErrorCondition(Information,"AdvancedConfigurator::ReadConfigurationFile: horizontal_inversion_negative_data_vector[%d] = %f",i, horizontal_inversion_negative_data_vector[i]);
	
	cdbe->MoveToRoot();
		
// read config file section: waveform_toroidal
	if(!cdbe->Move("+MARTe.+RealTimeThread.+waveform_toroidal.waveform_mode_1_positive"))
	{
		CStaticAssertErrorCondition(InitialisationError,"AdvancedConfigurator::ReadConfigurationFile: %s Could not move to \"+MARTe.+RealTimeThread.+waveform_toroidal.waveform_mode_1_positive\"",this->Name());
		temp_file.Close();
		return False;
	}
		if(!cdbe.ReadInt32(toroidal_1_p_vector_size, "vector_size"))
		{
			CStaticAssertErrorCondition(InitialisationError,"AdvancedConfigurator::ReadConfigurationFile: %s toroidal_1_p_vector_size",this->Name());
			temp_file.Close();
			return False;
		}
		else	CStaticAssertErrorCondition(Information,"AdvancedConfigurator::ReadConfigurationFile: toroidal_1_p_vector_size = %d",toroidal_1_p_vector_size);

		toroidal_1_p_index_vector =new float[toroidal_1_p_vector_size];
		toroidal_1_p_data_vector =new float[toroidal_1_p_vector_size];

		if(!cdbe.ReadFloatArray(toroidal_1_p_index_vector, (int *)(&toroidal_1_p_vector_size), 1, "index_vector"))
		{
			CStaticAssertErrorCondition(InitialisationError,"ReadWaveformFiles: Could not read toroidal_1_p_index_vector");
			temp_file.Close();
			return False;
		}
		else for(i=0;i<toroidal_1_p_vector_size;i++) CStaticAssertErrorCondition(Information,"AdvancedConfigurator::ReadConfigurationFile: toroidal_1_p_index_vector[%d] = %f",i, toroidal_1_p_index_vector[i]);
		if(!cdbe.ReadFloatArray(toroidal_1_p_data_vector, (int *)(&toroidal_1_p_vector_size), 1, "data_vector"))
		{
			CStaticAssertErrorCondition(InitialisationError,"ReadWaveformFiles: Could not read toroidal_1_p_data_vector");
			temp_file.Close();
			return False;
		}
		else for(i=0;i<toroidal_1_p_vector_size;i++) CStaticAssertErrorCondition(Information,"AdvancedConfigurator::ReadConfigurationFile: toroidal_1_p_data_vector[%d] = %f",i, toroidal_1_p_data_vector[i]);
		
	cdbe->MoveToRoot();	

// read config file section: waveform_puffing
	if(!cdbe->Move("+MARTe.+RealTimeThread.+waveform_puffing.waveform_mode_1_positive"))
	{
		CStaticAssertErrorCondition(InitialisationError,"AdvancedConfigurator::ReadConfigurationFile: %s Could not move to \"+MARTe.+RealTimeThread.+waveform_puffing.waveform_mode_1_positive\"",this->Name());
		temp_file.Close();
		return False;
	}
		if(!cdbe.ReadInt32(puffing_1_p_vector_size, "vector_size"))
		{
			CStaticAssertErrorCondition(InitialisationError,"AdvancedConfigurator::ReadConfigurationFile: %s puffing_1_p_vector_size",this->Name());
			temp_file.Close();
			return False;
		}
		else	CStaticAssertErrorCondition(Information,"AdvancedConfigurator::ReadConfigurationFile: puffing_1_p_vector_size = %d",puffing_1_p_vector_size);

		puffing_1_p_index_vector =new float[puffing_1_p_vector_size];
		puffing_1_p_data_vector =new float[puffing_1_p_vector_size];

		if(!cdbe.ReadFloatArray(puffing_1_p_index_vector, (int *)(&puffing_1_p_vector_size), 1, "index_vector"))
		{
			CStaticAssertErrorCondition(InitialisationError,"ReadWaveformFiles: Could not read puffing_1_p_index_vector");
			temp_file.Close();
			return False;
		}
		else for(i=0;i<puffing_1_p_vector_size;i++) CStaticAssertErrorCondition(Information,"AdvancedConfigurator::ReadConfigurationFile: puffing_1_p_index_vector[%d] = %f",i, puffing_1_p_index_vector[i]);
		if(!cdbe.ReadFloatArray(puffing_1_p_data_vector, (int *)(&puffing_1_p_vector_size), 1, "data_vector"))
		{
			CStaticAssertErrorCondition(InitialisationError,"ReadWaveformFiles: Could not read puffing_1_p_data_vector");
			temp_file.Close();
			return False;
		}
		else for(i=0;i<puffing_1_p_vector_size;i++) CStaticAssertErrorCondition(Information,"AdvancedConfigurator::ReadConfigurationFile: puffing_1_p_data_vector[%d] = %f",i, puffing_1_p_data_vector[i]);
		
	cdbe->MoveToRoot();	
	
	if(!cdbe->Move("+MARTe.+RealTimeThread.+waveform_puffing.waveform_mode_1_negative"))
	{
		CStaticAssertErrorCondition(InitialisationError,"AdvancedConfigurator::ReadConfigurationFile: %s Could not move to \"+MARTe.+RealTimeThread.+waveform_puffing.waveform_mode_1_negative\"",this->Name());
		temp_file.Close();
		return False;
	}
		if(!cdbe.ReadInt32(puffing_1_n_vector_size, "vector_size"))
		{
			CStaticAssertErrorCondition(InitialisationError,"AdvancedConfigurator::ReadConfigurationFile: %s puffing_1_n_vector_size",this->Name());
			temp_file.Close();
			return False;
		}
		else	CStaticAssertErrorCondition(Information,"AdvancedConfigurator::ReadConfigurationFile: puffing_1_n_vector_size = %d",puffing_1_n_vector_size);

		puffing_1_n_index_vector =new float[puffing_1_n_vector_size];
		puffing_1_n_data_vector =new float[puffing_1_n_vector_size];

		if(!cdbe.ReadFloatArray(puffing_1_n_index_vector, (int *)(&puffing_1_n_vector_size), 1, "index_vector"))
		{
			CStaticAssertErrorCondition(InitialisationError,"ReadWaveformFiles: Could not read puffing_1_n_index_vector");
			temp_file.Close();
			return False;
		}
		else for(i=0;i<puffing_1_n_vector_size;i++) CStaticAssertErrorCondition(Information,"AdvancedConfigurator::ReadConfigurationFile: puffing_1_n_index_vector[%d] = %f",i, puffing_1_n_index_vector[i]);
		if(!cdbe.ReadFloatArray(puffing_1_n_data_vector, (int *)(&puffing_1_n_vector_size), 1, "data_vector"))
		{
			CStaticAssertErrorCondition(InitialisationError,"ReadWaveformFiles: Could not read puffing_1_n_data_vector");
			temp_file.Close();
			return False;
		}
		else for(i=0;i<puffing_1_n_vector_size;i++) CStaticAssertErrorCondition(Information,"AdvancedConfigurator::ReadConfigurationFile: puffing_1_n_data_vector[%d] = %f",i, puffing_1_n_data_vector[i]);
	
	cdbe->MoveToRoot();
	
	if(!cdbe->Move("+MARTe.+RealTimeThread.+waveform_puffing.waveform_mode_2_positive"))
	{
		CStaticAssertErrorCondition(InitialisationError,"AdvancedConfigurator::ReadConfigurationFile: %s Could not move to \"+MARTe.+RealTimeThread.+waveform_puffing.waveform_mode_2_positive\"",this->Name());
		temp_file.Close();
		return False;
	}
		if(!cdbe.ReadInt32(puffing_2_p_vector_size, "vector_size"))
		{
			CStaticAssertErrorCondition(InitialisationError,"AdvancedConfigurator::ReadConfigurationFile: %s puffing_2_p_vector_size",this->Name());
			temp_file.Close();
			return False;
		}
		else	CStaticAssertErrorCondition(Information,"AdvancedConfigurator::ReadConfigurationFile: puffing_2_p_vector_size = %d",puffing_2_p_vector_size);

		puffing_2_p_index_vector =new float[puffing_2_p_vector_size];
		puffing_2_p_data_vector =new float[puffing_2_p_vector_size];

		if(!cdbe.ReadFloatArray(puffing_2_p_index_vector, (int *)(&puffing_2_p_vector_size), 1, "index_vector"))
		{
			CStaticAssertErrorCondition(InitialisationError,"ReadWaveformFiles: Could not read puffing_2_p_index_vector");
			temp_file.Close();
			return False;
		}
		else for(i=0;i<puffing_2_p_vector_size;i++) CStaticAssertErrorCondition(Information,"AdvancedConfigurator::ReadConfigurationFile: puffing_2_p_index_vector[%d] = %f",i, puffing_2_p_index_vector[i]);
		if(!cdbe.ReadFloatArray(puffing_2_p_data_vector, (int *)(&puffing_2_p_vector_size), 1, "data_vector"))
		{
			CStaticAssertErrorCondition(InitialisationError,"ReadWaveformFiles: Could not read puffing_1_p_data_vector");
			temp_file.Close();
			return False;
		}
		else for(i=0;i<puffing_2_p_vector_size;i++) CStaticAssertErrorCondition(Information,"AdvancedConfigurator::ReadConfigurationFile: puffing_2_p_data_vector[%d] = %f",i, puffing_2_p_data_vector[i]);
		
	cdbe->MoveToRoot();	
	
	if(!cdbe->Move("+MARTe.+RealTimeThread.+waveform_puffing.waveform_mode_2_negative"))
	{
		CStaticAssertErrorCondition(InitialisationError,"AdvancedConfigurator::ReadConfigurationFile: %s Could not move to \"+MARTe.+RealTimeThread.+waveform_puffing.waveform_mode_2_negative\"",this->Name());
		temp_file.Close();
		return False;
	}
		if(!cdbe.ReadInt32(puffing_2_n_vector_size, "vector_size"))
		{
			CStaticAssertErrorCondition(InitialisationError,"AdvancedConfigurator::ReadConfigurationFile: %s puffing_2_n_vector_size",this->Name());
			temp_file.Close();
			return False;
		}
		else	CStaticAssertErrorCondition(Information,"AdvancedConfigurator::ReadConfigurationFile: puffing_2_n_vector_size = %d",puffing_2_n_vector_size);

		puffing_2_n_index_vector =new float[puffing_2_n_vector_size];
		puffing_2_n_data_vector =new float[puffing_2_n_vector_size];

		if(!cdbe.ReadFloatArray(puffing_2_n_index_vector, (int *)(&puffing_2_n_vector_size), 1, "index_vector"))
		{
			CStaticAssertErrorCondition(InitialisationError,"ReadWaveformFiles: Could not read puffing_2_n_index_vector");
			temp_file.Close();
			return False;
		}
		else for(i=0;i<puffing_2_n_vector_size;i++) CStaticAssertErrorCondition(Information,"AdvancedConfigurator::ReadConfigurationFile: puffing_2_n_index_vector[%d] = %f",i, puffing_2_n_index_vector[i]);
		if(!cdbe.ReadFloatArray(puffing_2_n_data_vector, (int *)(&puffing_2_n_vector_size), 1, "data_vector"))
		{
			CStaticAssertErrorCondition(InitialisationError,"ReadWaveformFiles: Could not read puffing_2_n_data_vector");
			temp_file.Close();
			return False;
		}
		else for(i=0;i<puffing_2_n_vector_size;i++) CStaticAssertErrorCondition(Information,"AdvancedConfigurator::ReadConfigurationFile: puffing_2_n_data_vector[%d] = %f",i, puffing_2_n_data_vector[i]);
	
	cdbe->MoveToRoot();
	
	if(!cdbe->Move("+MARTe.+RealTimeThread.+waveform_puffing.waveform_breakdown"))
	{
		CStaticAssertErrorCondition(InitialisationError,"AdvancedConfigurator::ReadConfigurationFile: %s Could not move to \"+MARTe.+RealTimeThread.+waveform_puffing.waveform_mode_1_positive\"",this->Name());
		temp_file.Close();
		return False;
	}
		if(!cdbe.ReadInt32(puffing_absolute_time_vector_size, "vector_size"))
		{
			CStaticAssertErrorCondition(InitialisationError,"AdvancedConfigurator::ReadConfigurationFile: %s puffing_absolute_time_vector_size",this->Name());
			temp_file.Close();
			return False;
		}
		else	CStaticAssertErrorCondition(Information,"AdvancedConfigurator::ReadConfigurationFile: puffing_absolute_time_vector_size = %d",puffing_absolute_time_vector_size);

		puffing_absolute_time_index_vector = new float[puffing_absolute_time_vector_size];
		puffing_absolute_time_data_vector = new float[puffing_absolute_time_vector_size];

		if(!cdbe.ReadFloatArray(puffing_absolute_time_index_vector, (int *)(&puffing_absolute_time_vector_size), 1, "index_vector"))
		{
			CStaticAssertErrorCondition(InitialisationError,"ReadWaveformFiles: Could not read puffing_absolute_time_index_vector");
			temp_file.Close();
			return False;
		}
		else for(i=0;i<puffing_absolute_time_vector_size;i++) CStaticAssertErrorCondition(Information,"AdvancedConfigurator::ReadConfigurationFile: puffing_absolute_time_index_vector[%d] = %f",i, puffing_absolute_time_index_vector[i]);
		if(!cdbe.ReadFloatArray(puffing_absolute_time_data_vector, (int *)(&puffing_absolute_time_vector_size), 1, "data_vector"))
		{
			CStaticAssertErrorCondition(InitialisationError,"ReadWaveformFiles: Could not read puffing_absolute_time_data_vector");
			temp_file.Close();
			return False;
		}
		else for(i=0;i<puffing_absolute_time_vector_size;i++) CStaticAssertErrorCondition(Information,"AdvancedConfigurator::ReadConfigurationFile: puffing_absolute_time_data_vector[%d] = %f",i, puffing_absolute_time_data_vector[i]);
		
	cdbe->MoveToRoot();	

// read config file section: PSCommunicator_vertical
	if(!cdbe->Move("+MARTe.+RealTimeThread.+PSCommunicator_vertical"))
	{
		CStaticAssertErrorCondition(InitialisationError,"AdvancedConfigurator::ReadConfigurationFile: %s Could not move to \"+MARTe.+RealTimeThread.+PSCommunicator_vertical\"",this->Name());
		temp_file.Close();
		return False;
	}
		if(!cdbe.ReadFString(verticalPS_UARTPortAddress, "UARTPortAddress"))
		{
			CStaticAssertErrorCondition(InitialisationError,"WaveformsDisplay::ObjectLoadSetup: %s Could not verticalPS_UARTPortAddress",this->Name());
			return False;
		}
		else CStaticAssertErrorCondition(Information,"AdvancedConfigurator::ReadConfigurationFile: verticalPS_UARTPortAddress = %s",verticalPS_UARTPortAddress.Buffer());
		if(!cdbe.ReadFloat(verticalPS_PointOfZeroCurrent, "PointOfZeroCurrent"))
		{
			CStaticAssertErrorCondition(InitialisationError,"AdvancedConfigurator::ReadConfigurationFile: %s verticalPS_PointOfZeroCurrent",this->Name());
			temp_file.Close();
			return False;
		}
		else CStaticAssertErrorCondition(Information,"AdvancedConfigurator::ReadConfigurationFile: verticalPS_PointOfZeroCurrent = %f",verticalPS_PointOfZeroCurrent);
		if(!cdbe.ReadFloat(verticalPS_CurrentStep, "CurrentStep"))
		{
			CStaticAssertErrorCondition(InitialisationError,"AdvancedConfigurator::ReadConfigurationFile: %s verticalPS_CurrentStep",this->Name());
			temp_file.Close();
			return False;
		}
		else CStaticAssertErrorCondition(Information,"AdvancedConfigurator::ReadConfigurationFile: verticalPS_CurrentStep = %f",verticalPS_CurrentStep);

	cdbe->MoveToRoot();

// read config file section: PSCommunicator_horizontal
	if(!cdbe->Move("+MARTe.+RealTimeThread.+PSCommunicator_horizontal"))
	{
		CStaticAssertErrorCondition(InitialisationError,"AdvancedConfigurator::ReadConfigurationFile: %s Could not move to \"+MARTe.+RealTimeThread.+PSCommunicator_horizontal\"",this->Name());
		temp_file.Close();
		return False;
	}
		if(!cdbe.ReadFString(horizontalPS_UARTPortAddress, "UARTPortAddress"))
		{
			CStaticAssertErrorCondition(InitialisationError,"WaveformsDisplay::ObjectLoadSetup: %s Could not horizontalPS_UARTPortAddress",this->Name());
			return False;
		}
		else CStaticAssertErrorCondition(Information,"AdvancedConfigurator::ReadConfigurationFile: horizontalPS_UARTPortAddress = %s",horizontalPS_UARTPortAddress.Buffer());
		if(!cdbe.ReadFloat(horizontalPS_PointOfZeroCurrent, "PointOfZeroCurrent"))
		{
			CStaticAssertErrorCondition(InitialisationError,"AdvancedConfigurator::ReadConfigurationFile: %s horizontalPS_PointOfZeroCurrent",this->Name());
			temp_file.Close();
			return False;
		}
		else CStaticAssertErrorCondition(Information,"AdvancedConfigurator::ReadConfigurationFile: horizontalPS_PointOfZeroCurrent = %f",horizontalPS_PointOfZeroCurrent);
		if(!cdbe.ReadFloat(horizontalPS_CurrentStep, "CurrentStep"))
		{
			CStaticAssertErrorCondition(InitialisationError,"AdvancedConfigurator::ReadConfigurationFile: %s horizontalPS_CurrentStep",this->Name());
			temp_file.Close();
			return False;
		}
		else CStaticAssertErrorCondition(Information,"AdvancedConfigurator::ReadConfigurationFile: horizontalPS_CurrentStep = %f",horizontalPS_CurrentStep);

	cdbe->MoveToRoot();

// read config file section: PSCommunicator_primary
	if(!cdbe->Move("+MARTe.+RealTimeThread.+PSCommunicator_primary"))
	{
		CStaticAssertErrorCondition(InitialisationError,"AdvancedConfigurator::ReadConfigurationFile: %s Could not move to \"+MARTe.+RealTimeThread.+PSCommunicator_primary\"",this->Name());
		temp_file.Close();
		return False;
	}
		if(!cdbe.ReadFString(primaryPS_UARTPortAddress, "UARTPortAddress"))
		{
			CStaticAssertErrorCondition(InitialisationError,"WaveformsDisplay::ObjectLoadSetup: %s Could not primaryPS_UARTPortAddress",this->Name());
			return False;
		}
		else CStaticAssertErrorCondition(Information,"AdvancedConfigurator::ReadConfigurationFile: primaryPS_UARTPortAddress = %s",primaryPS_UARTPortAddress.Buffer());
		if(!cdbe.ReadFloat(primaryPS_PointOfZeroCurrent, "PointOfZeroCurrent"))
		{
			CStaticAssertErrorCondition(InitialisationError,"AdvancedConfigurator::ReadConfigurationFile: %s primaryPS_PointOfZeroCurrent",this->Name());
			temp_file.Close();
			return False;
		}
		else CStaticAssertErrorCondition(Information,"AdvancedConfigurator::ReadConfigurationFile: primaryPS_PointOfZeroCurrent = %f",primaryPS_PointOfZeroCurrent);
		if(!cdbe.ReadFloat(primaryPS_CurrentStep, "CurrentStep"))
		{
			CStaticAssertErrorCondition(InitialisationError,"AdvancedConfigurator::ReadConfigurationFile: %s primaryPS_CurrentStep",this->Name());
			temp_file.Close();
			return False;
		}
		else CStaticAssertErrorCondition(Information,"AdvancedConfigurator::ReadConfigurationFile: primaryPS_CurrentStep = %f",primaryPS_CurrentStep);

	cdbe->MoveToRoot();

	temp_file.Close();
	return True;
}

bool AdvancedConfigurator::WriteConfigurationFileWithChanges(char BaseFileFilePath[], char TargetFilePath[]){
	
	CDBExtended cdb;
	int i;
   File configIn;
    if(!configIn.OpenRead((char *)BaseFileFilePath)){
       printf("AdvancedConfigurator:: Failed opening file %s\n", BaseFileFilePath);
       return 0;
    }

    cdb->ReadFromStream(configIn);
    configIn.Close();	

//	Write real time thread configuration	
	if(!cdb->Move("+MARTe.+RealTimeThread"))
	{
		CStaticAssertErrorCondition(InitialisationError,"AdvancedConfigurator::WriteConfigurationFile: %s Could not move to \"+MARTe.+RealTimeThread\"",this->Name());
		return False;
	}	
		if(!cdb.WriteInt32(thread_priority, "ThreadPriority"))
		{
			AssertErrorCondition(InitialisationError,"AdvancedConfigurator::WriteConfigurationFile: %s thread_priority",this->Name());
			return False;
		}
		else AssertErrorCondition(Information,"AdvancedConfigurator::WriteConfigurationFile: thread_priority = %d",thread_priority);
		if(!cdb.WriteInt32(run_on_cpu, "RunOnCPU"))
		{
			AssertErrorCondition(InitialisationError,"AdvancedConfigurator::WriteConfigurationFile: %s run_on_cpu",this->Name());
			return False;
		}
		else AssertErrorCondition(Information,"AdvancedConfigurator::WriteConfigurationFile: run_on_cpu = %d",run_on_cpu);
	
	cdb->MoveToRoot();	

	
//	Write timewindows	
	if(!cdb->Move("+MARTe.+RealTimeThread.+time_windows"))
	{
		CStaticAssertErrorCondition(InitialisationError,"AdvancedConfigurator::WriteConfigurationFile: %s Could not move to \"+MARTe.+RealTimeThread.+time_windows\"",this->Name());
		return False;
	}	
		if(!cdb.WriteInt32(usec_pre_pulse_time, "usec_pre_pulse_time"))
		{
			AssertErrorCondition(InitialisationError,"AdvancedConfigurator::WriteConfigurationFile: %s usec_pre_pulse_time",this->Name());
			return False;
		}
		else AssertErrorCondition(Information,"AdvancedConfigurator::WriteConfigurationFile: usec_pre_pulse_time = %d",usec_pre_pulse_time);
		if(!cdb.WriteInt32(maximum_inversion_usectime, "maximum_inversion_usectime"))
		{
			AssertErrorCondition(InitialisationError,"AdvancedConfigurator::WriteConfigurationFile: %s maximum_inversion_usectime",this->Name());
			return False;
		}
		else AssertErrorCondition(Information,"AdvancedConfigurator::WriteConfigurationFile: maximum_inversion_usectime = %d",maximum_inversion_usectime);
		if(!cdb.WriteInt32(puffing_mode, "puffing_mode"))
		{
			AssertErrorCondition(InitialisationError,"AdvancedConfigurator::WriteConfigurationFile: %s puffing_mode",this->Name());
			return False;
		}
		else AssertErrorCondition(Information,"AdvancedConfigurator::WriteConfigurationFile: puffing_mode = %d",puffing_mode);
		if(!cdb.WriteInt32((int) end_discharge_after_unsuccess_bool, "end_discharge_after_unsuccess_bool"))
		{
			CStaticAssertErrorCondition(InitialisationError,"AdvancedConfigurator::WriteConfigurationFile: %s end_discharge_after_unsuccess_bool",this->Name());
			return False;
		}
		else CStaticAssertErrorCondition(Information,"AdvancedConfigurator::WriteConfigurationFile: end_discharge_after_unsuccess_bool = %d",end_discharge_after_unsuccess_bool);
		if(!cdb.WriteInt32(time_between_online_and_discharge, "time_between_online_and_discharge"))
		{
			AssertErrorCondition(InitialisationError,"AdvancedConfigurator::WriteConfigurationFile: %s time_between_online_and_discharge",this->Name());
			return False;
		}
		else AssertErrorCondition(Information,"AdvancedConfigurator::WriteConfigurationFile: time_between_online_and_discharge = %d",time_between_online_and_discharge);
		
	cdb->MoveToRoot();	

//	Write control
	if(!cdb->Move("+MARTe.+RealTimeThread.+controller"))
	{
		CStaticAssertErrorCondition(InitialisationError,"AdvancedConfigurator::WriteConfigurationFile: %s Could not move to \"+MARTe.+RealTimeThread.+controller\"",this->Name());
		return False;
	}	
		if(!cdb.WriteFString(control_file_to_load, "file_to_load"))
		{
			CStaticAssertErrorCondition(InitialisationError,"WaveformsDisplay::ObjectLoadSetup: %s Could not control_file_to_load",this->Name());
			return False;
		}
		else CStaticAssertErrorCondition(Information,"AdvancedConfigurator::WriteConfigurationFile: control_file_to_load = %s",control_file_to_load.Buffer());
		if(!cdb.WriteFloat(horizontal_1_p_max_value, "maximum_horizontal_current"))
		{
			CStaticAssertErrorCondition(InitialisationError,"AdvancedConfigurator::WriteConfigurationFile: %s maximum_horizontal_current",this->Name());
			return False;
		}
		else	CStaticAssertErrorCondition(Information,"AdvancedConfigurator::WriteConfigurationFile: maximum_horizontal_current = %f",horizontal_1_p_max_value);
		if(!cdb.WriteFloat(horizontal_1_p_min_value, "minimum_horizontal_current"))
		{
			CStaticAssertErrorCondition(InitialisationError,"AdvancedConfigurator::WriteConfigurationFile: %s minimum_horizontal_current",this->Name());
			return False;
		}
		else	CStaticAssertErrorCondition(Information,"AdvancedConfigurator::WriteConfigurationFile: minimum_horizontal_current = %f",horizontal_1_p_min_value);
		if(!cdb.WriteFloat(vertical_1_p_max_value, "maximum_vertical_current"))
		{
			CStaticAssertErrorCondition(InitialisationError,"AdvancedConfigurator::WriteConfigurationFile: %s maximum_vertical_current",this->Name());
			return False;
		}
		else	CStaticAssertErrorCondition(Information,"AdvancedConfigurator::WriteConfigurationFile: maximum_vertical_current = %f",vertical_1_p_max_value);
		if(!cdb.WriteFloat(vertical_1_p_min_value, "minimum_vertical_current"))
		{
			CStaticAssertErrorCondition(InitialisationError,"AdvancedConfigurator::WriteConfigurationFile: %s minimum_vertical_current",this->Name());
			return False;
		}
		else	CStaticAssertErrorCondition(Information,"AdvancedConfigurator::WriteConfigurationFile: minimum_vertical_current = %f",vertical_1_p_min_value);
		if(!cdb.WriteFloat(primary_1_p_max_value, "maximum_primary_current"))
		{
			CStaticAssertErrorCondition(InitialisationError,"AdvancedConfigurator::WriteConfigurationFile: %s maximum_primary_current",this->Name());
			return False;
		}
		else	CStaticAssertErrorCondition(Information,"AdvancedConfigurator::WriteConfigurationFile: maximum_primary_current = %f",primary_1_p_max_value);
		if(!cdb.WriteFloat(primary_1_p_min_value, "minimum_primary_current"))
		{
			CStaticAssertErrorCondition(InitialisationError,"AdvancedConfigurator::WriteConfigurationFile: %s minimum_primary_current",this->Name());
			return False;
		}
		else	CStaticAssertErrorCondition(Information,"AdvancedConfigurator::WriteConfigurationFile: minimum_primary_current = %f",primary_1_p_min_value);
		if(!cdb.WriteFloat(horizontal_2_p_max_value, "maximum_horizontal_position"))
		{
			CStaticAssertErrorCondition(InitialisationError,"AdvancedConfigurator::WriteConfigurationFile: %s maximum_horizontal_position",this->Name());
			return False;
		}
		else	CStaticAssertErrorCondition(Information,"AdvancedConfigurator::WriteConfigurationFile: maximum_horizontal_position = %f",horizontal_2_p_max_value);
		if(!cdb.WriteFloat(horizontal_2_p_min_value, "minimum_horizontal_position"))
		{
			CStaticAssertErrorCondition(InitialisationError,"AdvancedConfigurator::WriteConfigurationFile: %s minimum_horizontal_position",this->Name());
			return False;
		}
		else	CStaticAssertErrorCondition(Information,"AdvancedConfigurator::WriteConfigurationFile: minimum_horizontal_position = %f",horizontal_2_p_min_value);
		if(!cdb.WriteFloat(vertical_2_p_max_value, "maximum_vertical_position"))
		{
			CStaticAssertErrorCondition(InitialisationError,"AdvancedConfigurator::WriteConfigurationFile: %s maximum_vertical_position",this->Name());
			return False;
		}
		else	CStaticAssertErrorCondition(Information,"AdvancedConfigurator::WriteConfigurationFile: maximum_vertical_position = %f",vertical_2_p_max_value);
		if(!cdb.WriteFloat(vertical_2_p_min_value, "minimum_vertical_position"))
		{
			CStaticAssertErrorCondition(InitialisationError,"AdvancedConfigurator::WriteConfigurationFile: %s minimum_vertical_position",this->Name());
			return False;
		}
		else	CStaticAssertErrorCondition(Information,"AdvancedConfigurator::WriteConfigurationFile: minimum_vertical_position = %f",vertical_2_p_min_value);
		if(!cdb.WriteFloat(primary_2_p_max_value, "maximum_plasma_current"))
		{
			CStaticAssertErrorCondition(InitialisationError,"AdvancedConfigurator::WriteConfigurationFile: %s maximum_plasma_current",this->Name());
			return False;
		}
		else	CStaticAssertErrorCondition(Information,"AdvancedConfigurator::WriteConfigurationFile: maximum_plasma_current = %f",primary_2_p_max_value);
		if(!cdb.WriteFloat(primary_2_p_min_value, "minimum_plasma_current"))
		{
			CStaticAssertErrorCondition(InitialisationError,"AdvancedConfigurator::WriteConfigurationFile: %s minimum_plasma_current",this->Name());
			return False;
		}
		else	CStaticAssertErrorCondition(Information,"AdvancedConfigurator::WriteConfigurationFile: minimum_plasma_current = %f",primary_2_p_min_value);
		if(!cdb.WriteFloat(toroidal_1_p_max_value, "maximum_toroidal_current"))
		{
			CStaticAssertErrorCondition(InitialisationError,"AdvancedConfigurator::WriteConfigurationFile: %s maximum_toroidal_current",this->Name());
			return False;
		}
		else	CStaticAssertErrorCondition(Information,"AdvancedConfigurator::WriteConfigurationFile: maximum_toroidal_current = %f",toroidal_1_p_max_value);
		if(!cdb.WriteFloat(toroidal_1_p_min_value, "minimum_toroidal_current"))
		{
			CStaticAssertErrorCondition(InitialisationError,"AdvancedConfigurator::WriteConfigurationFile: %s minimum_toroidal_current",this->Name());
			return False;
		}
		else	CStaticAssertErrorCondition(Information,"AdvancedConfigurator::WriteConfigurationFile: minimum_toroidal_current = %f",toroidal_1_p_min_value);
		if(!cdb.WriteFloat(puffing_1_p_max_value, "maximum_puffing_output"))
		{
			CStaticAssertErrorCondition(InitialisationError,"AdvancedConfigurator::WriteConfigurationFile: %s maximum_puffing_output",this->Name());
			return False;
		}
		else	CStaticAssertErrorCondition(Information,"AdvancedConfigurator::WriteConfigurationFile: maximum_puffing_output = %f",puffing_1_p_max_value);
		if(!cdb.WriteFloat(puffing_1_p_min_value, "minimum_puffing_output"))
		{
			CStaticAssertErrorCondition(InitialisationError,"AdvancedConfigurator::WriteConfigurationFile: %s minimum_puffing_output",this->Name());
			return False;
		}
		else	CStaticAssertErrorCondition(Information,"AdvancedConfigurator::WriteConfigurationFile: minimum_puffing_output = %f",puffing_1_p_min_value);

		if(!cdb.WriteFloat(puffing_2_p_max_value, "maximum_density_halpha_scenario"))
		{
			CStaticAssertErrorCondition(InitialisationError,"AdvancedConfigurator::WriteConfigurationFile: %s maximum_density_halpha_scenario",this->Name());
			return False;
		}
		else	CStaticAssertErrorCondition(Information,"AdvancedConfigurator::WriteConfigurationFile: maximum_density_halpha_scenario = %f",puffing_2_p_max_value);
		if(!cdb.WriteFloat(puffing_2_p_min_value, "minimum_density_halpha_scenario"))
		{
			CStaticAssertErrorCondition(InitialisationError,"AdvancedConfigurator::WriteConfigurationFile: %s minimum_density_halpha_scenario",this->Name());
			return False;
		}
		else	CStaticAssertErrorCondition(Information,"AdvancedConfigurator::WriteConfigurationFile: minimum_density_halpha_scenario = %f",puffing_2_p_min_value);
		if(!cdb.WriteFloat(puffing_duration_in_puffing_feedback_in_ms, "puffing_duration_in_puffing_feedback_in_ms"))
		{
			CStaticAssertErrorCondition(InitialisationError,"AdvancedConfigurator::WriteConfigurationFile: %s puffing_duration_in_puffing_feedback_in_ms",this->Name());
			return False;
		}
		else	CStaticAssertErrorCondition(Information,"AdvancedConfigurator::WriteConfigurationFile: puffing_duration_in_puffing_feedback_in_ms = %f",puffing_duration_in_puffing_feedback_in_ms);
		if(!cdb.WriteFloat(maximum_idle_time_in_puffing_feedback_in_ms, "maximum_idle_time_in_puffing_feedback_in_ms"))
		{
			CStaticAssertErrorCondition(InitialisationError,"AdvancedConfigurator::WriteConfigurationFile: %s maximum_idle_time_in_puffing_feedback_in_ms",this->Name());
			return False;
		}
		else	CStaticAssertErrorCondition(Information,"AdvancedConfigurator::WriteConfigurationFile: maximum_idle_time_in_puffing_feedback_in_ms = %f",maximum_idle_time_in_puffing_feedback_in_ms);
		if(!cdb.WriteFloat(minimum_idle_time_in_puffing_feedback_in_ms, "minimum_idle_time_in_puffing_feedback_in_ms"))
		{
			CStaticAssertErrorCondition(InitialisationError,"AdvancedConfigurator::WriteConfigurationFile: %s minimum_idle_time_in_puffing_feedback_in_ms",this->Name());
			return False;
		}
		else	CStaticAssertErrorCondition(Information,"AdvancedConfigurator::WriteConfigurationFile: minimum_idle_time_in_puffing_feedback_in_ms = %f",minimum_idle_time_in_puffing_feedback_in_ms);
		if(!cdb.WriteFloat(puffing_feedback_usec_change_percentage_by_cycle, "puffing_feedback_usec_change_percentage_by_cycle"))
		{
			CStaticAssertErrorCondition(InitialisationError,"AdvancedConfigurator::WriteConfigurationFile: %s puffing_feedback_usec_change_percentage_by_cycle",this->Name());
			return False;
		}
		else	CStaticAssertErrorCondition(Information,"AdvancedConfigurator::WriteConfigurationFile: puffing_feedback_usec_change_percentage_by_cycle = %f",puffing_feedback_usec_change_percentage_by_cycle);
		if(!cdb.WriteInt32(puffing_mode, "puffing_mode"))
		{
			AssertErrorCondition(InitialisationError,"AdvancedConfigurator::WriteConfigurationFile: %s puffing_mode",this->Name());
			return False;
		}
		else AssertErrorCondition(Information,"AdvancedConfigurator::WriteConfigurationFile: puffing_mode = %d",puffing_mode);
		if(!cdb.WriteInt32(puffing_feedback_mode, "puffing_feedback_mode"))
		{
			AssertErrorCondition(InitialisationError,"AdvancedConfigurator::WriteConfigurationFile: %s puffing_feedback_mode",this->Name());
			return False;
		}
		else AssertErrorCondition(Information,"AdvancedConfigurator::WriteConfigurationFile: puffing_feedback_mode = %d",puffing_feedback_mode);

		if(!cdb.WriteInt32(puffing_mode, "puffing_mode"))
		{
			AssertErrorCondition(InitialisationError,"AdvancedConfigurator::WriteConfigurationFile: %s puffing_mode",this->Name());
			return False;
		}
		else AssertErrorCondition(Information,"AdvancedConfigurator::WriteConfigurationFile: puffing_mode = %d",puffing_mode);


	
	cdb->MoveToRoot();	

//	Write +machine_protection
	if(!cdb->Move("+MARTe.+RealTimeThread.+machine_protection"))
	{
		CStaticAssertErrorCondition(InitialisationError,"AdvancedConfigurator::WriteConfigurationFile: %s Could not move to \"+MARTe.+RealTimeThread.+machine_protection\"",this->Name());
		return False;
	}	
		if(!cdb.WriteFloat(iron_core_saturation_value, "iron_core_saturation_value"))
		{
			CStaticAssertErrorCondition(InitialisationError,"AdvancedConfigurator::WriteConfigurationFile: %s iron_core_saturation_value",this->Name());
			return False;
		}
		else	CStaticAssertErrorCondition(Information,"AdvancedConfigurator::WriteConfigurationFile: iron_core_saturation_value = %f",iron_core_saturation_value);
		if(!cdb.WriteFloat(iron_core_dangerous_value, "iron_core_dangerous_value"))
		{
			CStaticAssertErrorCondition(InitialisationError,"AdvancedConfigurator::WriteConfigurationFile: %s iron_core_dangerous_value",this->Name());
			return False;
		}
		else	CStaticAssertErrorCondition(Information,"AdvancedConfigurator::WriteConfigurationFile: iron_core_dangerous_value = %f",iron_core_dangerous_value);
	
	cdb->MoveToRoot();	

//	Write +tomography
	if(!cdb->Move("+MARTe.+RealTimeThread.+tomography"))
	{
		CStaticAssertErrorCondition(InitialisationError,"AdvancedConfigurator::WriteConfigurationFile: %s Could not move to \"+MARTe.+RealTimeThread.+tomography\"",this->Name());
		return False;
	}	
		if(!cdb.WriteFString(tomography_file_to_load, "file_to_load"))
		{
			CStaticAssertErrorCondition(InitialisationError,"WaveformsDisplay::ObjectLoadSetup: %s Could not tomography_file_to_load",this->Name());
			return False;
		}
		else CStaticAssertErrorCondition(Information,"AdvancedConfigurator::WriteConfigurationFile: tomography_file_to_load = %s",tomography_file_to_load.Buffer());
		if(!cdb.WriteInt32Array(tomography_online_channels, (int *)(&tomography_n_channels), 1, "onlineChannels"))
		{
			CStaticAssertErrorCondition(InitialisationError,"WriteWaveformFiles: Could not Write tomography_online_channels");
			return False;
		}
		else for(i=0;i<tomography_n_channels;i++) CStaticAssertErrorCondition(Information,"AdvancedConfigurator::WriteConfigurationFile: tomography_online_channels[%d] = %d",i, tomography_online_channels[i]);
	
	cdb->MoveToRoot();	
				

//	Write maximum values	
	if(!cdb->Move("+MARTe.+RealTimeThread.+waveform_primary.waveform_mode_1_positive"))
	{
		CStaticAssertErrorCondition(InitialisationError,"AdvancedConfigurator::WriteConfigurationFile: %s Could not move to \"+MARTe.+RealTimeThread.+waveform_primary.waveform_mode_1_positive\"",this->Name());
		return False;
	}	
		if(!cdb.WriteFloat(primary_1_p_max_value, "max_value"))
		{
			CStaticAssertErrorCondition(InitialisationError,"AdvancedConfigurator::WriteConfigurationFile: %s primary_1_p_max_value",this->Name());
			return False;
		}
		else	CStaticAssertErrorCondition(Information,"AdvancedConfigurator::WriteConfigurationFile: primary_1_p_max_value = %f",primary_1_p_max_value);
		if(!cdb.WriteFloat(primary_1_p_min_value, "min_value"))
		{
			CStaticAssertErrorCondition(InitialisationError,"AdvancedConfigurator::WriteConfigurationFile: %s primary_1_p_min_value",this->Name());
			return False;
		}
		else	CStaticAssertErrorCondition(Information,"AdvancedConfigurator::WriteConfigurationFile: primary_1_p_min_value = %f",primary_1_p_min_value);

	cdb->MoveToRoot();	
		
	if(!cdb->Move("+MARTe.+RealTimeThread.+waveform_primary.waveform_mode_2_positive"))
	{
		CStaticAssertErrorCondition(InitialisationError,"AdvancedConfigurator::WriteConfigurationFile: %s Could not move to \"+MARTe.+RealTimeThread.+waveform_primary.waveform_mode_2_positive\"",this->Name());
		return False;
	}	
		if(!cdb.WriteFloat(primary_2_p_max_value, "max_value"))
		{
			CStaticAssertErrorCondition(InitialisationError,"AdvancedConfigurator::WriteConfigurationFile: %s primary_2_p_max_value",this->Name());
			return False;
		}
		else	CStaticAssertErrorCondition(Information,"AdvancedConfigurator::WriteConfigurationFile: primary_2_p_max_value = %f",primary_2_p_max_value);
		if(!cdb.WriteFloat(primary_2_p_min_value, "min_value"))
		{
			CStaticAssertErrorCondition(InitialisationError,"AdvancedConfigurator::WriteConfigurationFile: %s primary_2_p_min_value",this->Name());
			return False;
		}
		else	CStaticAssertErrorCondition(Information,"AdvancedConfigurator::WriteConfigurationFile: primary_2_p_min_value = %f",primary_2_p_min_value);
	
	cdb->MoveToRoot();	
	
	if(!cdb->Move("+MARTe.+RealTimeThread.+waveform_vertical.waveform_mode_1_positive"))
	{
		CStaticAssertErrorCondition(InitialisationError,"AdvancedConfigurator::WriteConfigurationFile: %s Could not move to \"+MARTe.+RealTimeThread.+waveform_vertical.waveform_mode_1_positive\"",this->Name());
		return False;
	}
		if(!cdb.WriteFloat(vertical_1_p_max_value, "max_value"))
		{
			CStaticAssertErrorCondition(InitialisationError,"AdvancedConfigurator::WriteConfigurationFile: %s vertical_1_p_max_value",this->Name());
			return False;
		}
		else	CStaticAssertErrorCondition(Information,"AdvancedConfigurator::WriteConfigurationFile: vertical_1_p_max_value = %f",vertical_1_p_max_value);
		if(!cdb.WriteFloat(vertical_1_p_min_value, "min_value"))
		{
			CStaticAssertErrorCondition(InitialisationError,"AdvancedConfigurator::WriteConfigurationFile: %s vertical_1_p_min_value",this->Name());
			return False;
		}
		else	CStaticAssertErrorCondition(Information,"AdvancedConfigurator::WriteConfigurationFile: vertical_1_p_min_value = %f",vertical_1_p_min_value);
		
	cdb->MoveToRoot();	

	if(!cdb->Move("+MARTe.+RealTimeThread.+waveform_vertical.waveform_mode_2_positive"))
	{
		CStaticAssertErrorCondition(InitialisationError,"AdvancedConfigurator::WriteConfigurationFile: %s Could not move to \"+MARTe.+RealTimeThread.+waveform_vertical.waveform_mode_2_positive\"",this->Name());
		return False;
	}
		if(!cdb.WriteFloat(vertical_2_p_max_value, "max_value"))
		{
			CStaticAssertErrorCondition(InitialisationError,"AdvancedConfigurator::WriteConfigurationFile: %s vertical_2_p_max_value",this->Name());
			return False;
		}
		else	CStaticAssertErrorCondition(Information,"AdvancedConfigurator::WriteConfigurationFile: vertical_2_p_max_value = %f",vertical_2_p_max_value);
		if(!cdb.WriteFloat(vertical_2_p_min_value, "min_value"))
		{
			CStaticAssertErrorCondition(InitialisationError,"AdvancedConfigurator::WriteConfigurationFile: %s vertical_2_p_min_value",this->Name());
			return False;
		}
		else	CStaticAssertErrorCondition(Information,"AdvancedConfigurator::WriteConfigurationFile: vertical_2_p_min_value = %f",vertical_2_p_min_value);
		
	cdb->MoveToRoot();	

	if(!cdb->Move("+MARTe.+RealTimeThread.+waveform_horizontal.waveform_mode_1_positive"))
	{
		CStaticAssertErrorCondition(InitialisationError,"AdvancedConfigurator::WriteConfigurationFile: %s Could not move to \"+MARTe.+RealTimeThread.+waveform_horizontal.waveform_mode_1_positive\"",this->Name());
		return False;
	}
		if(!cdb.WriteFloat(horizontal_1_p_max_value, "max_value"))
		{
			CStaticAssertErrorCondition(InitialisationError,"AdvancedConfigurator::WriteConfigurationFile: %s horizontal_1_p_max_value",this->Name());
			return False;
		}
		else	CStaticAssertErrorCondition(Information,"AdvancedConfigurator::WriteConfigurationFile: horizontal_1_p_max_value = %f",horizontal_1_p_max_value);
		if(!cdb.WriteFloat(horizontal_1_p_min_value, "min_value"))
		{
			CStaticAssertErrorCondition(InitialisationError,"AdvancedConfigurator::WriteConfigurationFile: %s horizontal_1_p_min_value",this->Name());
			return False;
		}
		else	CStaticAssertErrorCondition(Information,"AdvancedConfigurator::WriteConfigurationFile: horizontal_1_p_min_value = %f",horizontal_1_p_min_value);
		
	cdb->MoveToRoot();	
	
	if(!cdb->Move("+MARTe.+RealTimeThread.+waveform_horizontal.waveform_mode_2_positive"))
	{
		CStaticAssertErrorCondition(InitialisationError,"AdvancedConfigurator::WriteConfigurationFile: %s Could not move to \"+MARTe.+RealTimeThread.+waveform_horizontal.waveform_mode_2_positive\"",this->Name());
		return False;
	}
		if(!cdb.WriteFloat(horizontal_2_p_max_value, "max_value"))
		{
			CStaticAssertErrorCondition(InitialisationError,"AdvancedConfigurator::WriteConfigurationFile: %s horizontal_2_p_max_value",this->Name());
			return False;
		}
		else	CStaticAssertErrorCondition(Information,"AdvancedConfigurator::WriteConfigurationFile: horizontal_2_p_max_value = %f",horizontal_2_p_max_value);
		if(!cdb.WriteFloat(horizontal_2_p_min_value, "min_value"))
		{
			CStaticAssertErrorCondition(InitialisationError,"AdvancedConfigurator::WriteConfigurationFile: %s horizontal_2_p_min_value",this->Name());
			return False;
		}
		else	CStaticAssertErrorCondition(Information,"AdvancedConfigurator::WriteConfigurationFile: horizontal_2_p_min_value = %f",horizontal_2_p_min_value);
		
	cdb->MoveToRoot();	

	if(!cdb->Move("+MARTe.+RealTimeThread.+waveform_toroidal.waveform_mode_1_positive"))
	{
		CStaticAssertErrorCondition(InitialisationError,"AdvancedConfigurator::WriteConfigurationFile: %s Could not move to \"+MARTe.+RealTimeThread.+waveform_toroidal.waveform_mode_2_positive\"",this->Name());
		return False;
	}
		if(!cdb.WriteFloat(toroidal_1_p_max_value, "max_value"))
		{
			CStaticAssertErrorCondition(InitialisationError,"AdvancedConfigurator::WriteConfigurationFile: %s toroidal_1_p_max_value",this->Name());
			return False;
		}
		else	CStaticAssertErrorCondition(Information,"AdvancedConfigurator::WriteConfigurationFile: toroidal_1_p_max_value = %f",toroidal_1_p_max_value);
		if(!cdb.WriteFloat(toroidal_1_p_min_value, "min_value"))
		{
			CStaticAssertErrorCondition(InitialisationError,"AdvancedConfigurator::WriteConfigurationFile: %s toroidal_1_p_min_value",this->Name());
			return False;
		}
		else	CStaticAssertErrorCondition(Information,"AdvancedConfigurator::WriteConfigurationFile: toroidal_1_p_min_value = %f",toroidal_1_p_min_value);
		
	cdb->MoveToRoot();	

	if(!cdb->Move("+MARTe.+RealTimeThread.+waveform_puffing.waveform_mode_1_positive"))
	{
		CStaticAssertErrorCondition(InitialisationError,"AdvancedConfigurator::WriteConfigurationFile: %s Could not move to \"+MARTe.+RealTimeThread.+waveform_puffing.waveform_mode_1_positive\"",this->Name());
		return False;
	}
		if(!cdb.WriteFloat(puffing_1_p_max_value, "max_value"))
		{
			CStaticAssertErrorCondition(InitialisationError,"AdvancedConfigurator::WriteConfigurationFile: %s puffing_1_p_max_value",this->Name());
			return False;
		}
		else	CStaticAssertErrorCondition(Information,"AdvancedConfigurator::WriteConfigurationFile: puffing_1_p_max_value = %f",puffing_1_p_max_value);
		if(!cdb.WriteFloat(puffing_1_p_min_value, "min_value"))
		{
			CStaticAssertErrorCondition(InitialisationError,"AdvancedConfigurator::WriteConfigurationFile: %s puffing_1_p_min_value",this->Name());
			return False;
		}
		else	CStaticAssertErrorCondition(Information,"AdvancedConfigurator::WriteConfigurationFile: puffing_1_p_min_value = %f",puffing_1_p_min_value);
		
	cdb->MoveToRoot();	

	if(!cdb->Move("+MARTe.+RealTimeThread.+waveform_puffing.waveform_mode_2_positive"))
	{
		CStaticAssertErrorCondition(InitialisationError,"AdvancedConfigurator::WriteConfigurationFile: %s Could not move to \"+MARTe.+RealTimeThread.+waveform_puffing.waveform_mode_2_positive\"",this->Name());
		return False;
	}
		if(!cdb.WriteFloat(puffing_2_p_max_value, "max_value"))
		{
			CStaticAssertErrorCondition(InitialisationError,"AdvancedConfigurator::WriteConfigurationFile: %s puffing_2_p_max_value",this->Name());
			return False;
		}
		else	CStaticAssertErrorCondition(Information,"AdvancedConfigurator::WriteConfigurationFile: puffing_2_p_max_value = %f",puffing_2_p_max_value);
		if(!cdb.WriteFloat(puffing_2_p_min_value, "min_value"))
		{
			CStaticAssertErrorCondition(InitialisationError,"AdvancedConfigurator::WriteConfigurationFile: %s puffing_2_p_min_value",this->Name());
			return False;
		}
		else	CStaticAssertErrorCondition(Information,"AdvancedConfigurator::WriteConfigurationFile: puffing_2_p_min_value = %f",puffing_2_p_min_value);
		
	cdb->MoveToRoot();		

// write config file section: waveform_primary
	if(!cdb->Move("+MARTe.+RealTimeThread.+waveform_primary.waveform_breakdown"))
	{
		CStaticAssertErrorCondition(InitialisationError,"AdvancedConfigurator::WriteConfigurationFile: %s Could not move to \"+MARTe.+RealTimeThread.+waveform_primary.waveform_breakdown\"",this->Name());
		return False;
	}
		if(!cdb.WriteInt32(primary_breakdown_vector_size, "vector_size"))
		{
			CStaticAssertErrorCondition(InitialisationError,"AdvancedConfigurator::WriteConfigurationFile: %s primary_breakdown_vector_size",this->Name());
			return False;
		}
		else	CStaticAssertErrorCondition(Information,"AdvancedConfigurator::WriteConfigurationFile: primary_breakdown_vector_size = %d",primary_breakdown_vector_size);
		if(!cdb.WriteFloatArray(primary_breakdown_index_vector, (int *)(&primary_breakdown_vector_size), 1, "index_vector"))
		{
			CStaticAssertErrorCondition(InitialisationError,"WriteWaveformFiles: Could not write primary_breakdown_index_vector");
			return False;
		}
		else for(i=0;i<primary_breakdown_vector_size;i++) CStaticAssertErrorCondition(Information,"AdvancedConfigurator::WriteConfigurationFile: primary_breakdown_index_vector[%d] = %f",i, primary_breakdown_index_vector[i]);
		if(!cdb.WriteFloatArray(primary_breakdown_data_vector, (int *)(&primary_breakdown_vector_size), 1, "data_vector"))
		{
			CStaticAssertErrorCondition(InitialisationError,"WriteWaveformFiles: Could not write primary_breakdown_data_vector");
			return False;
		}
		else for(i=0;i<primary_breakdown_vector_size;i++) CStaticAssertErrorCondition(Information,"AdvancedConfigurator::WriteConfigurationFile: primary_breakdown_data_vector[%d] = %f",i, primary_breakdown_data_vector[i]);
		
	cdb->MoveToRoot();	
	
	if(!cdb->Move("+MARTe.+RealTimeThread.+waveform_primary.waveform_breakdown_negative"))
	{
		CStaticAssertErrorCondition(InitialisationError,"AdvancedConfigurator::WriteConfigurationFile: %s Could not move to \"+MARTe.+RealTimeThread.+waveform_primary.waveform_breakdown_negative\"",this->Name());
		return False;
	}
		if(!cdb.WriteInt32(primary_breakdown_negative_vector_size, "vector_size"))
		{
			CStaticAssertErrorCondition(InitialisationError,"AdvancedConfigurator::WriteConfigurationFile: %s primary_breakdown_negative_vector_size",this->Name());
			return False;
		}
		else	CStaticAssertErrorCondition(Information,"AdvancedConfigurator::WriteConfigurationFile: primary_breakdown_negative_vector_size = %d",primary_breakdown_negative_vector_size);
		if(!cdb.WriteFloatArray(primary_breakdown_negative_index_vector, (int *)(&primary_breakdown_negative_vector_size), 1, "index_vector"))
		{
			CStaticAssertErrorCondition(InitialisationError,"WriteWaveformFiles: Could not write primary_breakdown_negative_index_vector");
			return False;
		}
		else for(i=0;i<primary_breakdown_negative_vector_size;i++) CStaticAssertErrorCondition(Information,"AdvancedConfigurator::WriteConfigurationFile: primary_breakdown_negative_index_vector[%d] = %f",i, primary_breakdown_negative_index_vector[i]);
		if(!cdb.WriteFloatArray(primary_breakdown_negative_data_vector, (int *)(&primary_breakdown_negative_vector_size), 1, "data_vector"))
		{
			CStaticAssertErrorCondition(InitialisationError,"WriteWaveformFiles: Could not write primary_breakdown_negative_data_vector");
			return False;
		}
		else for(i=0;i<primary_breakdown_negative_vector_size;i++) CStaticAssertErrorCondition(Information,"AdvancedConfigurator::WriteConfigurationFile: primary_breakdown_negative_data_vector[%d] = %f",i, primary_breakdown_negative_data_vector[i]);
	
	cdb->MoveToRoot();
	
	if(!cdb->Move("+MARTe.+RealTimeThread.+waveform_primary.waveform_inversion_positive_to_negative"))
	{
		CStaticAssertErrorCondition(InitialisationError,"AdvancedConfigurator::WriteConfigurationFile: %s Could not move to \"+MARTe.+RealTimeThread.+waveform_primary.waveform_inversion_positive_to_negative\"",this->Name());
		return False;
	}
		if(!cdb.WriteInt32(primary_inversion_positive_vector_size, "vector_size"))
		{
			CStaticAssertErrorCondition(InitialisationError,"AdvancedConfigurator::WriteConfigurationFile: %s primary_inversion_positive_vector_size",this->Name());
			return False;
		}
		else	CStaticAssertErrorCondition(Information,"AdvancedConfigurator::WriteConfigurationFile: primary_inversion_positive_vector_size = %d",primary_inversion_positive_vector_size);
		if(!cdb.WriteFloatArray(primary_inversion_positive_index_vector, (int *)(&primary_inversion_positive_vector_size), 1, "index_vector"))
		{
			CStaticAssertErrorCondition(InitialisationError,"WriteWaveformFiles: Could not write primary_inversion_positive_index_vector");
			return False;
		}
		else for(i=0;i<primary_inversion_positive_vector_size;i++) CStaticAssertErrorCondition(Information,"AdvancedConfigurator::WriteConfigurationFile: primary_inversion_positive_index_vector[%d] = %f",i, primary_inversion_positive_index_vector[i]);
		if(!cdb.WriteFloatArray(primary_inversion_positive_data_vector, (int *)(&primary_inversion_positive_vector_size), 1, "data_vector"))
		{
			CStaticAssertErrorCondition(InitialisationError,"WriteWaveformFiles: Could not write primary_breakdown_data_vector");
			return False;
		}
		else for(i=0;i<primary_inversion_positive_vector_size;i++) CStaticAssertErrorCondition(Information,"AdvancedConfigurator::WriteConfigurationFile: primary_inversion_positive_data_vector[%d] = %f",i, primary_inversion_positive_data_vector[i]);
		
	cdb->MoveToRoot();	
	
	if(!cdb->Move("+MARTe.+RealTimeThread.+waveform_primary.waveform_inversion_negative_to_positive"))
	{
		CStaticAssertErrorCondition(InitialisationError,"AdvancedConfigurator::WriteConfigurationFile: %s Could not move to \"+MARTe.+RealTimeThread.+waveform_inversion_negative_to_positive\"",this->Name());
		return False;
	}
		if(!cdb.WriteInt32(primary_inversion_negative_vector_size, "vector_size"))
		{
			CStaticAssertErrorCondition(InitialisationError,"AdvancedConfigurator::WriteConfigurationFile: %s primary_inversion_negative_vector_size",this->Name());
			return False;
		}
		else	CStaticAssertErrorCondition(Information,"AdvancedConfigurator::WriteConfigurationFile: primary_inversion_negative_vector_size = %d",primary_inversion_negative_vector_size);
		if(!cdb.WriteFloatArray(primary_inversion_negative_index_vector, (int *)(&primary_inversion_negative_vector_size), 1, "index_vector"))
		{
			CStaticAssertErrorCondition(InitialisationError,"WriteWaveformFiles: Could not write primary_inversion_negative_index_vector");
			return False;
		}
		else for(i=0;i<primary_inversion_negative_vector_size;i++) CStaticAssertErrorCondition(Information,"AdvancedConfigurator::WriteConfigurationFile: primary_inversion_negative_index_vector[%d] = %f",i, primary_breakdown_negative_index_vector[i]);
		if(!cdb.WriteFloatArray(primary_inversion_negative_data_vector, (int *)(&primary_inversion_negative_vector_size), 1, "data_vector"))
		{
			CStaticAssertErrorCondition(InitialisationError,"WriteWaveformFiles: Could not write primary_inversion_negative_data_vector");
			return False;
		}
		else for(i=0;i<primary_inversion_negative_vector_size;i++) CStaticAssertErrorCondition(Information,"AdvancedConfigurator::WriteConfigurationFile: primary_inversion_negative_data_vector[%d] = %f",i, primary_inversion_negative_data_vector[i]);
	
	cdb->MoveToRoot();

// write config file section: waveform_vertical
	if(!cdb->Move("+MARTe.+RealTimeThread.+waveform_vertical.waveform_breakdown"))
	{
		CStaticAssertErrorCondition(InitialisationError,"AdvancedConfigurator::WriteConfigurationFile: %s Could not move to \"+MARTe.+RealTimeThread.+waveform_vertical.waveform_breakdown\"",this->Name());
		return False;
	}
		if(!cdb.WriteInt32(vertical_breakdown_vector_size, "vector_size"))
		{
			CStaticAssertErrorCondition(InitialisationError,"AdvancedConfigurator::WriteConfigurationFile: %s vertical_breakdown_vector_size",this->Name());
			return False;
		}
		else	CStaticAssertErrorCondition(Information,"AdvancedConfigurator::WriteConfigurationFile: vertical_breakdown_vector_size = %d",vertical_breakdown_vector_size);
		if(!cdb.WriteFloatArray(vertical_breakdown_index_vector, (int *)(&vertical_breakdown_vector_size), 1, "index_vector"))
		{
			CStaticAssertErrorCondition(InitialisationError,"WriteWaveformFiles: Could not write vertical_breakdown_index_vector");
			return False;
		}
		else for(i=0;i<vertical_breakdown_vector_size;i++) CStaticAssertErrorCondition(Information,"AdvancedConfigurator::WriteConfigurationFile: vertical_breakdown_index_vector[%d] = %f",i, vertical_breakdown_index_vector[i]);
		if(!cdb.WriteFloatArray(vertical_breakdown_data_vector, (int *)(&vertical_breakdown_vector_size), 1, "data_vector"))
		{
			CStaticAssertErrorCondition(InitialisationError,"WriteWaveformFiles: Could not write vertical_breakdown_data_vector");
			return False;
		}
		else for(i=0;i<vertical_breakdown_vector_size;i++) CStaticAssertErrorCondition(Information,"AdvancedConfigurator::WriteConfigurationFile: vertical_breakdown_data_vector[%d] = %f",i, vertical_breakdown_data_vector[i]);
		
	cdb->MoveToRoot();	
	
	if(!cdb->Move("+MARTe.+RealTimeThread.+waveform_vertical.waveform_breakdown_negative"))
	{
		CStaticAssertErrorCondition(InitialisationError,"AdvancedConfigurator::WriteConfigurationFile: %s Could not move to \"+MARTe.+RealTimeThread.+waveform_vertical.waveform_breakdown_negative\"",this->Name());
		return False;
	}
		if(!cdb.WriteInt32(vertical_breakdown_negative_vector_size, "vector_size"))
		{
			CStaticAssertErrorCondition(InitialisationError,"AdvancedConfigurator::WriteConfigurationFile: %s vertical_breakdown_negative_vector_size",this->Name());
			return False;
		}
		else	CStaticAssertErrorCondition(Information,"AdvancedConfigurator::WriteConfigurationFile: vertical_breakdown_negative_vector_size = %d",vertical_breakdown_negative_vector_size);
		if(!cdb.WriteFloatArray(vertical_breakdown_negative_index_vector, (int *)(&vertical_breakdown_negative_vector_size), 1, "index_vector"))
		{
			CStaticAssertErrorCondition(InitialisationError,"WriteWaveformFiles: Could not write vertical_breakdown_negative_index_vector");
			return False;
		}
		else for(i=0;i<vertical_breakdown_negative_vector_size;i++) CStaticAssertErrorCondition(Information,"AdvancedConfigurator::WriteConfigurationFile: vertical_breakdown_negative_index_vector[%d] = %f",i, vertical_breakdown_negative_index_vector[i]);
		if(!cdb.WriteFloatArray(vertical_breakdown_negative_data_vector, (int *)(&vertical_breakdown_negative_vector_size), 1, "data_vector"))
		{
			CStaticAssertErrorCondition(InitialisationError,"WriteWaveformFiles: Could not write vertical_breakdown_negative_data_vector");
			return False;
		}
		else for(i=0;i<vertical_breakdown_negative_vector_size;i++) CStaticAssertErrorCondition(Information,"AdvancedConfigurator::WriteConfigurationFile: vertical_breakdown_negative_data_vector[%d] = %f",i, vertical_breakdown_negative_data_vector[i]);
	
	cdb->MoveToRoot();
	
	if(!cdb->Move("+MARTe.+RealTimeThread.+waveform_vertical.waveform_inversion_positive_to_negative"))
	{
		CStaticAssertErrorCondition(InitialisationError,"AdvancedConfigurator::WriteConfigurationFile: %s Could not move to \"+MARTe.+RealTimeThread.+waveform_vertical.waveform_inversion_positive_to_negative\"",this->Name());
		return False;
	}
		if(!cdb.WriteInt32(vertical_inversion_positive_vector_size, "vector_size"))
		{
			CStaticAssertErrorCondition(InitialisationError,"AdvancedConfigurator::WriteConfigurationFile: %s vertical_inversion_positive_vector_size",this->Name());
			return False;
		}
		else	CStaticAssertErrorCondition(Information,"AdvancedConfigurator::WriteConfigurationFile: vertical_inversion_positive_vector_size = %d",vertical_inversion_positive_vector_size);
		if(!cdb.WriteFloatArray(vertical_inversion_positive_index_vector, (int *)(&vertical_inversion_positive_vector_size), 1, "index_vector"))
		{
			CStaticAssertErrorCondition(InitialisationError,"WriteWaveformFiles: Could not write vertical_inversion_positive_index_vector");
			return False;
		}
		else for(i=0;i<vertical_inversion_positive_vector_size;i++) CStaticAssertErrorCondition(Information,"AdvancedConfigurator::WriteConfigurationFile: vertical_inversion_positive_index_vector[%d] = %f",i, vertical_inversion_positive_index_vector[i]);
		if(!cdb.WriteFloatArray(vertical_inversion_positive_data_vector, (int *)(&vertical_inversion_positive_vector_size), 1, "data_vector"))
		{
			CStaticAssertErrorCondition(InitialisationError,"WriteWaveformFiles: Could not write vertical_breakdown_data_vector");
			return False;
		}
		else for(i=0;i<vertical_inversion_positive_vector_size;i++) CStaticAssertErrorCondition(Information,"AdvancedConfigurator::WriteConfigurationFile: vertical_inversion_positive_data_vector[%d] = %f",i, vertical_inversion_positive_data_vector[i]);
		
	cdb->MoveToRoot();	
	
	if(!cdb->Move("+MARTe.+RealTimeThread.+waveform_vertical.waveform_inversion_negative_to_positive"))
	{
		CStaticAssertErrorCondition(InitialisationError,"AdvancedConfigurator::WriteConfigurationFile: %s Could not move to \"+MARTe.+RealTimeThread.+waveform_vertical.waveform_inversion_negative_to_positive\"",this->Name());
		return False;
	}
		if(!cdb.WriteInt32(vertical_inversion_negative_vector_size, "vector_size"))
		{
			CStaticAssertErrorCondition(InitialisationError,"AdvancedConfigurator::WriteConfigurationFile: %s vertical_inversion_negative_vector_size",this->Name());
			return False;
		}
		else	CStaticAssertErrorCondition(Information,"AdvancedConfigurator::WriteConfigurationFile: vertical_inversion_negative_vector_size = %d",vertical_inversion_negative_vector_size);
		if(!cdb.WriteFloatArray(vertical_inversion_negative_index_vector, (int *)(&vertical_inversion_negative_vector_size), 1, "index_vector"))
		{
			CStaticAssertErrorCondition(InitialisationError,"WriteWaveformFiles: Could not write vertical_inversion_negative_index_vector");
			return False;
		}
		else for(i=0;i<vertical_inversion_negative_vector_size;i++) CStaticAssertErrorCondition(Information,"AdvancedConfigurator::WriteConfigurationFile: vertical_inversion_negative_index_vector[%d] = %f",i, vertical_breakdown_negative_index_vector[i]);
		if(!cdb.WriteFloatArray(vertical_inversion_negative_data_vector, (int *)(&vertical_inversion_negative_vector_size), 1, "data_vector"))
		{
			CStaticAssertErrorCondition(InitialisationError,"WriteWaveformFiles: Could not write vertical_inversion_negative_data_vector");
			return False;
		}
		else for(i=0;i<vertical_inversion_negative_vector_size;i++) CStaticAssertErrorCondition(Information,"AdvancedConfigurator::WriteConfigurationFile: vertical_inversion_negative_data_vector[%d] = %f",i, vertical_inversion_negative_data_vector[i]);
	
	cdb->MoveToRoot();

// write config file section: waveform_horizontal
	if(!cdb->Move("+MARTe.+RealTimeThread.+waveform_horizontal.waveform_breakdown"))
	{
		CStaticAssertErrorCondition(InitialisationError,"AdvancedConfigurator::WriteConfigurationFile: %s Could not move to \"+MARTe.+RealTimeThread.+waveform_horizontal.waveform_breakdown\"",this->Name());
		return False;
	}
		if(!cdb.WriteInt32(horizontal_breakdown_vector_size, "vector_size"))
		{
			CStaticAssertErrorCondition(InitialisationError,"AdvancedConfigurator::WriteConfigurationFile: %s horizontal_breakdown_vector_size",this->Name());
			return False;
		}
		else	CStaticAssertErrorCondition(Information,"AdvancedConfigurator::WriteConfigurationFile: horizontal_breakdown_vector_size = %d",horizontal_breakdown_vector_size);
		if(!cdb.WriteFloatArray(horizontal_breakdown_index_vector, (int *)(&horizontal_breakdown_vector_size), 1, "index_vector"))
		{
			CStaticAssertErrorCondition(InitialisationError,"WriteWaveformFiles: Could not write horizontal_breakdown_index_vector");
			return False;
		}
		else for(i=0;i<horizontal_breakdown_vector_size;i++) CStaticAssertErrorCondition(Information,"AdvancedConfigurator::WriteConfigurationFile: horizontal_breakdown_index_vector[%d] = %f",i, horizontal_breakdown_index_vector[i]);
		if(!cdb.WriteFloatArray(horizontal_breakdown_data_vector, (int *)(&horizontal_breakdown_vector_size), 1, "data_vector"))
		{
			CStaticAssertErrorCondition(InitialisationError,"WriteWaveformFiles: Could not write horizontal_breakdown_data_vector");
			return False;
		}
		else for(i=0;i<horizontal_breakdown_vector_size;i++) CStaticAssertErrorCondition(Information,"AdvancedConfigurator::WriteConfigurationFile: horizontal_breakdown_data_vector[%d] = %f",i, horizontal_breakdown_data_vector[i]);
		
	cdb->MoveToRoot();	
	
	if(!cdb->Move("+MARTe.+RealTimeThread.+waveform_horizontal.waveform_breakdown_negative"))
	{
		CStaticAssertErrorCondition(InitialisationError,"AdvancedConfigurator::WriteConfigurationFile: %s Could not move to \"+MARTe.+RealTimeThread.+waveform_horizontal.waveform_breakdown_negative\"",this->Name());
		return False;
	}
		if(!cdb.WriteInt32(horizontal_breakdown_negative_vector_size, "vector_size"))
		{
			CStaticAssertErrorCondition(InitialisationError,"AdvancedConfigurator::WriteConfigurationFile: %s horizontal_breakdown_negative_vector_size",this->Name());
			return False;
		}
		else	CStaticAssertErrorCondition(Information,"AdvancedConfigurator::WriteConfigurationFile: horizontal_breakdown_negative_vector_size = %d",horizontal_breakdown_negative_vector_size);
		if(!cdb.WriteFloatArray(horizontal_breakdown_negative_index_vector, (int *)(&horizontal_breakdown_negative_vector_size), 1, "index_vector"))
		{
			CStaticAssertErrorCondition(InitialisationError,"WriteWaveformFiles: Could not write horizontal_breakdown_negative_index_vector");
			return False;
		}
		else for(i=0;i<horizontal_breakdown_negative_vector_size;i++) CStaticAssertErrorCondition(Information,"AdvancedConfigurator::WriteConfigurationFile: horizontal_breakdown_negative_index_vector[%d] = %f",i, horizontal_breakdown_negative_index_vector[i]);
		if(!cdb.WriteFloatArray(horizontal_breakdown_negative_data_vector, (int *)(&horizontal_breakdown_negative_vector_size), 1, "data_vector"))
		{
			CStaticAssertErrorCondition(InitialisationError,"WriteWaveformFiles: Could not write horizontal_breakdown_negative_data_vector");
			return False;
		}
		else for(i=0;i<horizontal_breakdown_negative_vector_size;i++) CStaticAssertErrorCondition(Information,"AdvancedConfigurator::WriteConfigurationFile: horizontal_breakdown_negative_data_vector[%d] = %f",i, horizontal_breakdown_negative_data_vector[i]);
	
	cdb->MoveToRoot();
	
	if(!cdb->Move("+MARTe.+RealTimeThread.+waveform_horizontal.waveform_inversion_positive_to_negative"))
	{
		CStaticAssertErrorCondition(InitialisationError,"AdvancedConfigurator::WriteConfigurationFile: %s Could not move to \"+MARTe.+RealTimeThread.+waveform_horizontal.waveform_inversion_positive_to_negative\"",this->Name());
		return False;
	}
		if(!cdb.WriteInt32(horizontal_inversion_positive_vector_size, "vector_size"))
		{
			CStaticAssertErrorCondition(InitialisationError,"AdvancedConfigurator::WriteConfigurationFile: %s horizontal_inversion_positive_vector_size",this->Name());
			return False;
		}
		else	CStaticAssertErrorCondition(Information,"AdvancedConfigurator::WriteConfigurationFile: horizontal_inversion_positive_vector_size = %d",horizontal_inversion_positive_vector_size);
		if(!cdb.WriteFloatArray(horizontal_inversion_positive_index_vector, (int *)(&horizontal_inversion_positive_vector_size), 1, "index_vector"))
		{
			CStaticAssertErrorCondition(InitialisationError,"WriteWaveformFiles: Could not write horizontal_inversion_positive_index_vector");
			return False;
		}
		else for(i=0;i<horizontal_inversion_positive_vector_size;i++) CStaticAssertErrorCondition(Information,"AdvancedConfigurator::WriteConfigurationFile: horizontal_inversion_positive_index_vector[%d] = %f",i, horizontal_inversion_positive_index_vector[i]);
		if(!cdb.WriteFloatArray(horizontal_inversion_positive_data_vector, (int *)(&horizontal_inversion_positive_vector_size), 1, "data_vector"))
		{
			CStaticAssertErrorCondition(InitialisationError,"WriteWaveformFiles: Could not write horizontal_breakdown_data_vector");
			return False;
		}
		else for(i=0;i<horizontal_inversion_positive_vector_size;i++) CStaticAssertErrorCondition(Information,"AdvancedConfigurator::WriteConfigurationFile: horizontal_inversion_positive_data_vector[%d] = %f",i, horizontal_inversion_positive_data_vector[i]);
		
	cdb->MoveToRoot();	
	
	if(!cdb->Move("+MARTe.+RealTimeThread.+waveform_horizontal.waveform_inversion_negative_to_positive"))
	{
		CStaticAssertErrorCondition(InitialisationError,"AdvancedConfigurator::WriteConfigurationFile: %s Could not move to \"+MARTe.+RealTimeThread.+waveform_horizontal.waveform_inversion_negative_to_positive\"",this->Name());
		return False;
	}
		if(!cdb.WriteInt32(horizontal_inversion_negative_vector_size, "vector_size"))
		{
			CStaticAssertErrorCondition(InitialisationError,"AdvancedConfigurator::WriteConfigurationFile: %s horizontal_inversion_negative_vector_size",this->Name());
			return False;
		}
		else	CStaticAssertErrorCondition(Information,"AdvancedConfigurator::WriteConfigurationFile: horizontal_inversion_negative_vector_size = %d",horizontal_inversion_negative_vector_size);
		if(!cdb.WriteFloatArray(horizontal_inversion_negative_index_vector, (int *)(&horizontal_inversion_negative_vector_size), 1, "index_vector"))
		{
			CStaticAssertErrorCondition(InitialisationError,"WriteWaveformFiles: Could not write horizontal_inversion_negative_index_vector");
			return False;
		}
		else for(i=0;i<horizontal_inversion_negative_vector_size;i++) CStaticAssertErrorCondition(Information,"AdvancedConfigurator::WriteConfigurationFile: horizontal_inversion_negative_index_vector[%d] = %f",i, horizontal_breakdown_negative_index_vector[i]);
		if(!cdb.WriteFloatArray(horizontal_inversion_negative_data_vector, (int *)(&horizontal_inversion_negative_vector_size), 1, "data_vector"))
		{
			CStaticAssertErrorCondition(InitialisationError,"WriteWaveformFiles: Could not write horizontal_inversion_negative_data_vector");
			return False;
		}
		else for(i=0;i<horizontal_inversion_negative_vector_size;i++) CStaticAssertErrorCondition(Information,"AdvancedConfigurator::WriteConfigurationFile: horizontal_inversion_negative_data_vector[%d] = %f",i, horizontal_inversion_negative_data_vector[i]);
	
	cdb->MoveToRoot();
		
// Write config file section: waveform_toroidal
	if(!cdb->Move("+MARTe.+RealTimeThread.+waveform_toroidal.waveform_mode_1_positive"))
	{
		CStaticAssertErrorCondition(InitialisationError,"AdvancedConfigurator::WriteConfigurationFile: %s Could not move to \"+MARTe.+RealTimeThread.+waveform_toroidal.waveform_mode_1_positive\"",this->Name());
		return False;
	}
		if(!cdb.WriteInt32(toroidal_1_p_vector_size, "vector_size"))
		{
			CStaticAssertErrorCondition(InitialisationError,"AdvancedConfigurator::WriteConfigurationFile: %s toroidal_1_p_vector_size",this->Name());
			return False;
		}
		else	CStaticAssertErrorCondition(Information,"AdvancedConfigurator::WriteConfigurationFile: toroidal_1_p_vector_size = %d",toroidal_1_p_vector_size);
		if(!cdb.WriteFloatArray(toroidal_1_p_index_vector, (int *)(&toroidal_1_p_vector_size), 1, "index_vector"))
		{
			CStaticAssertErrorCondition(InitialisationError,"WriteWaveformFiles: Could not Write toroidal_1_p_index_vector");
			return False;
		}
		else for(i=0;i<toroidal_1_p_vector_size;i++) CStaticAssertErrorCondition(Information,"AdvancedConfigurator::WriteConfigurationFile: toroidal_1_p_index_vector[%d] = %f",i, toroidal_1_p_index_vector[i]);
		if(!cdb.WriteFloatArray(toroidal_1_p_data_vector, (int *)(&toroidal_1_p_vector_size), 1, "data_vector"))
		{
			CStaticAssertErrorCondition(InitialisationError,"WriteWaveformFiles: Could not Write toroidal_1_p_data_vector");
			return False;
		}
		else for(i=0;i<toroidal_1_p_vector_size;i++) CStaticAssertErrorCondition(Information,"AdvancedConfigurator::WriteConfigurationFile: toroidal_1_p_data_vector[%d] = %f",i, toroidal_1_p_data_vector[i]);
		
	cdb->MoveToRoot();	

// !!!!! very important to have both toroidal waveforms equal (to remain the same with plasma direction change on the WaveformGAM) toroidal in absolute time
	if(!cdb->Move("+MARTe.+RealTimeThread.+waveform_toroidal.waveform_mode_1_negative"))
	{
		CStaticAssertErrorCondition(InitialisationError,"AdvancedConfigurator::WriteConfigurationFile: %s Could not move to \"+MARTe.+RealTimeThread.+waveform_toroidal.waveform_mode_1_negative\"",this->Name());
		return False;
	}
		if(!cdb.WriteInt32(toroidal_1_p_vector_size, "vector_size"))
		{
			CStaticAssertErrorCondition(InitialisationError,"AdvancedConfigurator::WriteConfigurationFile: %s toroidal_1_p_vector_size",this->Name());
			return False;
		}
		else	CStaticAssertErrorCondition(Information,"AdvancedConfigurator::WriteConfigurationFile: toroidal_1_p_vector_size = %d",toroidal_1_p_vector_size);
		if(!cdb.WriteFloatArray(toroidal_1_p_index_vector, (int *)(&toroidal_1_p_vector_size), 1, "index_vector"))
		{
			CStaticAssertErrorCondition(InitialisationError,"WriteWaveformFiles: Could not Write toroidal_1_p_index_vector");
			return False;
		}
		else for(i=0;i<toroidal_1_p_vector_size;i++) CStaticAssertErrorCondition(Information,"AdvancedConfigurator::WriteConfigurationFile: toroidal_1_p_index_vector[%d] = %f",i, toroidal_1_p_index_vector[i]);
		if(!cdb.WriteFloatArray(toroidal_1_p_data_vector, (int *)(&toroidal_1_p_vector_size), 1, "data_vector"))
		{
			CStaticAssertErrorCondition(InitialisationError,"WriteWaveformFiles: Could not Write toroidal_1_p_data_vector");
			return False;
		}
		else for(i=0;i<toroidal_1_p_vector_size;i++) CStaticAssertErrorCondition(Information,"AdvancedConfigurator::WriteConfigurationFile: toroidal_1_p_data_vector[%d] = %f",i, toroidal_1_p_data_vector[i]);
		
	cdb->MoveToRoot();	

// Write config file section: waveform_puffing
	if(!cdb->Move("+MARTe.+RealTimeThread.+waveform_puffing.waveform_mode_1_positive"))
	{
		CStaticAssertErrorCondition(InitialisationError,"AdvancedConfigurator::WriteConfigurationFile: %s Could not move to \"+MARTe.+RealTimeThread.+waveform_puffing.waveform_mode_1_positive\"",this->Name());
		return False;
	}
		if(!cdb.WriteInt32(puffing_1_p_vector_size, "vector_size"))
		{
			CStaticAssertErrorCondition(InitialisationError,"AdvancedConfigurator::WriteConfigurationFile: %s puffing_1_p_vector_size",this->Name());
			return False;
		}
		else	CStaticAssertErrorCondition(Information,"AdvancedConfigurator::WriteConfigurationFile: puffing_1_p_vector_size = %d",puffing_1_p_vector_size);
		if(!cdb.WriteFloatArray(puffing_1_p_index_vector, (int *)(&puffing_1_p_vector_size), 1, "index_vector"))
		{
			CStaticAssertErrorCondition(InitialisationError,"WriteWaveformFiles: Could not Write puffing_1_p_index_vector");
			return False;
		}
		else for(i=0;i<puffing_1_p_vector_size;i++) CStaticAssertErrorCondition(Information,"AdvancedConfigurator::WriteConfigurationFile: puffing_1_p_index_vector[%d] = %f",i, puffing_1_p_index_vector[i]);
		if(!cdb.WriteFloatArray(puffing_1_p_data_vector, (int *)(&puffing_1_p_vector_size), 1, "data_vector"))
		{
			CStaticAssertErrorCondition(InitialisationError,"WriteWaveformFiles: Could not Write puffing_1_p_data_vector");
			return False;
		}
		else for(i=0;i<puffing_1_p_vector_size;i++) CStaticAssertErrorCondition(Information,"AdvancedConfigurator::WriteConfigurationFile: puffing_1_p_data_vector[%d] = %f",i, puffing_1_p_data_vector[i]);
		
	cdb->MoveToRoot();	
	
	if(!cdb->Move("+MARTe.+RealTimeThread.+waveform_puffing.waveform_mode_1_negative"))
	{
		CStaticAssertErrorCondition(InitialisationError,"AdvancedConfigurator::WriteConfigurationFile: %s Could not move to \"+MARTe.+RealTimeThread.+waveform_puffing.waveform_mode_1_negative\"",this->Name());
		return False;
	}
		if(!cdb.WriteInt32(puffing_1_n_vector_size, "vector_size"))
		{
			CStaticAssertErrorCondition(InitialisationError,"AdvancedConfigurator::WriteConfigurationFile: %s puffing_1_n_vector_size",this->Name());
			return False;
		}
		else	CStaticAssertErrorCondition(Information,"AdvancedConfigurator::WriteConfigurationFile: puffing_1_n_vector_size = %d",puffing_1_n_vector_size);
		if(!cdb.WriteFloatArray(puffing_1_n_index_vector, (int *)(&puffing_1_n_vector_size), 1, "index_vector"))
		{
			CStaticAssertErrorCondition(InitialisationError,"WriteWaveformFiles: Could not Write puffing_1_n_index_vector");
			return False;
		}
		else for(i=0;i<puffing_1_n_vector_size;i++) CStaticAssertErrorCondition(Information,"AdvancedConfigurator::WriteConfigurationFile: puffing_1_n_index_vector[%d] = %f",i, puffing_1_n_index_vector[i]);
		if(!cdb.WriteFloatArray(puffing_1_n_data_vector, (int *)(&puffing_1_n_vector_size), 1, "data_vector"))
		{
			CStaticAssertErrorCondition(InitialisationError,"WriteWaveformFiles: Could not Write puffing_1_n_data_vector");
			return False;
		}
		else for(i=0;i<puffing_1_n_vector_size;i++) CStaticAssertErrorCondition(Information,"AdvancedConfigurator::WriteConfigurationFile: puffing_1_n_data_vector[%d] = %f",i, puffing_1_n_data_vector[i]);
	
	cdb->MoveToRoot();
	
	if(!cdb->Move("+MARTe.+RealTimeThread.+waveform_puffing.waveform_mode_2_positive"))
	{
		CStaticAssertErrorCondition(InitialisationError,"AdvancedConfigurator::WriteConfigurationFile: %s Could not move to \"+MARTe.+RealTimeThread.+waveform_puffing.waveform_mode_2_positive\"",this->Name());
		return False;
	}
		if(!cdb.WriteInt32(puffing_2_p_vector_size, "vector_size"))
		{
			CStaticAssertErrorCondition(InitialisationError,"AdvancedConfigurator::WriteConfigurationFile: %s puffing_2_p_vector_size",this->Name());
			return False;
		}
		else	CStaticAssertErrorCondition(Information,"AdvancedConfigurator::WriteConfigurationFile: puffing_2_p_vector_size = %d",puffing_2_p_vector_size);
		if(!cdb.WriteFloatArray(puffing_2_p_index_vector, (int *)(&puffing_2_p_vector_size), 1, "index_vector"))
		{
			CStaticAssertErrorCondition(InitialisationError,"WriteWaveformFiles: Could not Write puffing_2_p_index_vector");
			return False;
		}
		else for(i=0;i<puffing_2_p_vector_size;i++) CStaticAssertErrorCondition(Information,"AdvancedConfigurator::WriteConfigurationFile: puffing_2_p_index_vector[%d] = %f",i, puffing_2_p_index_vector[i]);
		if(!cdb.WriteFloatArray(puffing_2_p_data_vector, (int *)(&puffing_2_p_vector_size), 1, "data_vector"))
		{
			CStaticAssertErrorCondition(InitialisationError,"WriteWaveformFiles: Could not Write puffing_1_p_data_vector");
			return False;
		}
		else for(i=0;i<puffing_2_p_vector_size;i++) CStaticAssertErrorCondition(Information,"AdvancedConfigurator::WriteConfigurationFile: puffing_2_p_data_vector[%d] = %f",i, puffing_2_p_data_vector[i]);
		
	cdb->MoveToRoot();	
	
	if(!cdb->Move("+MARTe.+RealTimeThread.+waveform_puffing.waveform_mode_2_negative"))
	{
		CStaticAssertErrorCondition(InitialisationError,"AdvancedConfigurator::WriteConfigurationFile: %s Could not move to \"+MARTe.+RealTimeThread.+waveform_puffing.waveform_mode_2_negative\"",this->Name());
		return False;
	}
		if(!cdb.WriteInt32(puffing_2_n_vector_size, "vector_size"))
		{
			CStaticAssertErrorCondition(InitialisationError,"AdvancedConfigurator::WriteConfigurationFile: %s puffing_2_n_vector_size",this->Name());
			return False;
		}
		else	CStaticAssertErrorCondition(Information,"AdvancedConfigurator::WriteConfigurationFile: puffing_2_n_vector_size = %d",puffing_2_n_vector_size);
		if(!cdb.WriteFloatArray(puffing_2_n_index_vector, (int *)(&puffing_2_n_vector_size), 1, "index_vector"))
		{
			CStaticAssertErrorCondition(InitialisationError,"WriteWaveformFiles: Could not Write puffing_2_n_index_vector");
			return False;
		}
		else for(i=0;i<puffing_2_n_vector_size;i++) CStaticAssertErrorCondition(Information,"AdvancedConfigurator::WriteConfigurationFile: puffing_2_n_index_vector[%d] = %f",i, puffing_2_n_index_vector[i]);
		if(!cdb.WriteFloatArray(puffing_2_n_data_vector, (int *)(&puffing_2_n_vector_size), 1, "data_vector"))
		{
			CStaticAssertErrorCondition(InitialisationError,"WriteWaveformFiles: Could not Write puffing_2_n_data_vector");
			return False;
		}
		else for(i=0;i<puffing_2_n_vector_size;i++) CStaticAssertErrorCondition(Information,"AdvancedConfigurator::WriteConfigurationFile: puffing_2_n_data_vector[%d] = %f",i, puffing_2_n_data_vector[i]);
	
	cdb->MoveToRoot();
	
	if(!cdb->Move("+MARTe.+RealTimeThread.+waveform_puffing.waveform_breakdown"))
	{
		CStaticAssertErrorCondition(InitialisationError,"AdvancedConfigurator::WriteConfigurationFile: %s Could not move to \"+MARTe.+RealTimeThread.+waveform_puffing.waveform_breakdown\"",this->Name());
		return False;
	}
		if(!cdb.WriteInt32(puffing_absolute_time_vector_size, "vector_size"))
		{
			CStaticAssertErrorCondition(InitialisationError,"AdvancedConfigurator::WriteConfigurationFile: %s puffing_absolute_time_vector_size",this->Name());
			return False;
		}
		else	CStaticAssertErrorCondition(Information,"AdvancedConfigurator::WriteConfigurationFile: puffing_absolute_time_vector_size = %d",puffing_absolute_time_vector_size);
		if(!cdb.WriteFloatArray(puffing_absolute_time_index_vector, (int *)(&puffing_absolute_time_vector_size), 1, "index_vector"))
		{
			CStaticAssertErrorCondition(InitialisationError,"WriteWaveformFiles: Could not Write puffing_absolute_time_index_vector");
			return False;
		}
		else for(i=0;i<puffing_absolute_time_vector_size;i++) CStaticAssertErrorCondition(Information,"AdvancedConfigurator::WriteConfigurationFile: puffing_absolute_time_index_vector[%d] = %f",i, puffing_absolute_time_index_vector[i]);
		if(!cdb.WriteFloatArray(puffing_absolute_time_data_vector, (int *)(&puffing_absolute_time_vector_size), 1, "data_vector"))
		{
			CStaticAssertErrorCondition(InitialisationError,"WriteWaveformFiles: Could not Write puffing_absolute_time_data_vector");
			return False;
		}
		else for(i=0;i<puffing_absolute_time_vector_size;i++) CStaticAssertErrorCondition(Information,"AdvancedConfigurator::WriteConfigurationFile: puffing_absolute_time_data_vector[%d] = %f",i, puffing_absolute_time_data_vector[i]);
		
	cdb->MoveToRoot();	

// !!!!! very important to have both puffing breakdown waveforms equal (to remain the same with plasma direction change on the WaveformGAM) puffing in absolute time

	if(!cdb->Move("+MARTe.+RealTimeThread.+waveform_puffing.waveform_breakdown_negative"))
	{
		CStaticAssertErrorCondition(InitialisationError,"AdvancedConfigurator::WriteConfigurationFile: %s Could not move to \"+MARTe.+RealTimeThread.+waveform_puffing.waveform_breakdown_negative\"",this->Name());
		return False;
	}
		if(!cdb.WriteInt32(puffing_absolute_time_vector_size, "vector_size"))
		{
			CStaticAssertErrorCondition(InitialisationError,"AdvancedConfigurator::WriteConfigurationFile: %s puffing_absolute_time_vector_size",this->Name());
			return False;
		}
		else	CStaticAssertErrorCondition(Information,"AdvancedConfigurator::WriteConfigurationFile: puffing_absolute_time_vector_size = %d",puffing_absolute_time_vector_size);
		if(!cdb.WriteFloatArray(puffing_absolute_time_index_vector, (int *)(&puffing_absolute_time_vector_size), 1, "index_vector"))
		{
			CStaticAssertErrorCondition(InitialisationError,"WriteWaveformFiles: Could not Write puffing_absolute_time_index_vector");
			return False;
		}
		else for(i=0;i<puffing_absolute_time_vector_size;i++) CStaticAssertErrorCondition(Information,"AdvancedConfigurator::WriteConfigurationFile: puffing_absolute_time_index_vector[%d] = %f",i, puffing_absolute_time_index_vector[i]);
		if(!cdb.WriteFloatArray(puffing_absolute_time_data_vector, (int *)(&puffing_absolute_time_vector_size), 1, "data_vector"))
		{
			CStaticAssertErrorCondition(InitialisationError,"WriteWaveformFiles: Could not Write puffing_absolute_time_data_vector");
			return False;
		}
		else for(i=0;i<puffing_absolute_time_vector_size;i++) CStaticAssertErrorCondition(Information,"AdvancedConfigurator::WriteConfigurationFile: puffing_absolute_time_data_vector[%d] = %f",i, puffing_absolute_time_data_vector[i]);
		
	cdb->MoveToRoot();	

	if(!cdb->Move("+MARTe.+RealTimeThread.+waveform_puffing.input_signals.input_time"))
	{
		CStaticAssertErrorCondition(InitialisationError,"AdvancedConfigurator::WriteConfigurationFile: %s Could not move to \"+MARTe.+RealTimeThread.+waveform_puffing.input_signals.input_time\"",this->Name());
		return False;
	}
		FString puffing_time_setting_to_write; // !!!!! use only usecTime or usecDischargeTime in this var, everything else implies a segmentation fault
		if (puffing_mode == 3)	puffing_time_setting_to_write.Printf("usecTime");
		else puffing_time_setting_to_write.Printf("usecDischargeTime");

		if(!cdb.WriteFString(puffing_time_setting_to_write, "SignalName"))
		{
			CStaticAssertErrorCondition(InitialisationError,"WaveformsDisplay::WriteConfigurationFile: %s Could not puffing_time_setting_to_write",this->Name());
			return False;
		}
		else CStaticAssertErrorCondition(Information,"AdvancedConfigurator::WriteConfigurationFile: puffing_time_setting_to_write = %s",puffing_time_setting_to_write.Buffer());

	cdb->MoveToRoot();	

// write config file section: PSCommunicator_vertical
	if(!cdb->Move("+MARTe.+RealTimeThread.+PSCommunicator_vertical"))
	{
		CStaticAssertErrorCondition(InitialisationError,"AdvancedConfigurator::WriteConfigurationFile: %s Could not move to \"+MARTe.+RealTimeThread.+PSCommunicator_vertical\"",this->Name());
		return False;
	}
		if(!cdb.WriteFString(verticalPS_UARTPortAddress, "UARTPortAddress"))
		{
			CStaticAssertErrorCondition(InitialisationError,"WaveformsDisplay::WriteConfigurationFile: %s Could not verticalPS_UARTPortAddress",this->Name());
			return False;
		}
		else CStaticAssertErrorCondition(Information,"AdvancedConfigurator::WriteConfigurationFile: verticalPS_UARTPortAddress = %s",verticalPS_UARTPortAddress.Buffer());
		if(!cdb.WriteFloat(verticalPS_PointOfZeroCurrent, "PointOfZeroCurrent"))
		{
			CStaticAssertErrorCondition(InitialisationError,"AdvancedConfigurator::WriteConfigurationFile: %s verticalPS_PointOfZeroCurrent",this->Name());
			return False;
		}
		else CStaticAssertErrorCondition(Information,"AdvancedConfigurator::WriteConfigurationFile: verticalPS_PointOfZeroCurrent = %f",verticalPS_PointOfZeroCurrent);
		if(!cdb.WriteFloat(verticalPS_CurrentStep, "CurrentStep"))
		{
			CStaticAssertErrorCondition(InitialisationError,"AdvancedConfigurator::WriteConfigurationFile: %s verticalPS_CurrentStep",this->Name());
			return False;
		}
		else CStaticAssertErrorCondition(Information,"AdvancedConfigurator::WriteConfigurationFile: verticalPS_CurrentStep = %f",verticalPS_CurrentStep);

	cdb->MoveToRoot();

// write config file section: PSCommunicator_horizontal
	if(!cdb->Move("+MARTe.+RealTimeThread.+PSCommunicator_horizontal"))
	{
		CStaticAssertErrorCondition(InitialisationError,"AdvancedConfigurator::WriteConfigurationFile: %s Could not move to \"+MARTe.+RealTimeThread.+PSCommunicator_horizontal\"",this->Name());
		return False;
	}
		if(!cdb.WriteFString(horizontalPS_UARTPortAddress, "UARTPortAddress"))
		{
			CStaticAssertErrorCondition(InitialisationError,"WaveformsDisplay::ObjectLoadSetup: %s Could not horizontalPS_UARTPortAddress",this->Name());
			return False;
		}
		else CStaticAssertErrorCondition(Information,"AdvancedConfigurator::WriteConfigurationFile: horizontalPS_UARTPortAddress = %s",horizontalPS_UARTPortAddress.Buffer());
		if(!cdb.WriteFloat(horizontalPS_PointOfZeroCurrent, "PointOfZeroCurrent"))
		{
			CStaticAssertErrorCondition(InitialisationError,"AdvancedConfigurator::WriteConfigurationFile: %s horizontalPS_PointOfZeroCurrent",this->Name());
			return False;
		}
		else CStaticAssertErrorCondition(Information,"AdvancedConfigurator::WriteConfigurationFile: horizontalPS_PointOfZeroCurrent = %f",horizontalPS_PointOfZeroCurrent);
		if(!cdb.WriteFloat(horizontalPS_CurrentStep, "CurrentStep"))
		{
			CStaticAssertErrorCondition(InitialisationError,"AdvancedConfigurator::WriteConfigurationFile: %s horizontalPS_CurrentStep",this->Name());
			return False;
		}
		else CStaticAssertErrorCondition(Information,"AdvancedConfigurator::WriteConfigurationFile: horizontalPS_CurrentStep = %f",horizontalPS_CurrentStep);

	cdb->MoveToRoot();

// write config file section: PSCommunicator_primary
	if(!cdb->Move("+MARTe.+RealTimeThread.+PSCommunicator_primary"))
	{
		CStaticAssertErrorCondition(InitialisationError,"AdvancedConfigurator::WriteConfigurationFile: %s Could not move to \"+MARTe.+RealTimeThread.+PSCommunicator_primary\"",this->Name());
		return False;
	}
		if(!cdb.WriteFString(primaryPS_UARTPortAddress, "UARTPortAddress"))
		{
			CStaticAssertErrorCondition(InitialisationError,"WaveformsDisplay::ObjectLoadSetup: %s Could not primaryPS_UARTPortAddress",this->Name());
			return False;
		}
		else CStaticAssertErrorCondition(Information,"AdvancedConfigurator::WriteConfigurationFile: primaryPS_UARTPortAddress = %s",primaryPS_UARTPortAddress.Buffer());
		if(!cdb.WriteFloat(primaryPS_PointOfZeroCurrent, "PointOfZeroCurrent"))
		{
			CStaticAssertErrorCondition(InitialisationError,"AdvancedConfigurator::WriteConfigurationFile: %s primaryPS_PointOfZeroCurrent",this->Name());
			return False;
		}
		else CStaticAssertErrorCondition(Information,"AdvancedConfigurator::WriteConfigurationFile: primaryPS_PointOfZeroCurrent = %f",primaryPS_PointOfZeroCurrent);
		if(!cdb.WriteFloat(primaryPS_CurrentStep, "CurrentStep"))
		{
			CStaticAssertErrorCondition(InitialisationError,"AdvancedConfigurator::WriteConfigurationFile: %s primaryPS_CurrentStep",this->Name());
			return False;
		}
		else CStaticAssertErrorCondition(Information,"AdvancedConfigurator::WriteConfigurationFile: primaryPS_CurrentStep = %f",primaryPS_CurrentStep);

	cdb->MoveToRoot();

// 	Move to the AdvancedConfiguration

	if(!cdb->Move("+AdvancedConfiguration"))
	{
		CStaticAssertErrorCondition(InitialisationError,"AdvancedConfigurator::ObjectLoadSetup: %s Could not move to \"+AdvancedConfiguration.graphics\"",this->Name());
		return False;
	}
		if(!cdb.WriteFString(Description, "Description"))
		{
			CStaticAssertErrorCondition(InitialisationError,"AdvancedConfigurator::ObjectLoadSetup: %s Could not Write Description",this->Name());
			return False;
		}

	cdb->MoveToRoot();	
	

    FileEraseFile((char *)TargetFilePath);
    File configOut;
    if(!configOut.OpenWrite((char *)TargetFilePath)){
       printf("AdvancedConfigurator:: Failed open write file %s\n", TargetFilePath);
       return 0;
    }

    //Write cdb to string. Tu vais utilizar esta versao (nao vais guardar num ficheiro, claro). Isto e' o que vai depois ser enviado ao MARTe
 //   FString cdbString;
    cdb->WriteToStream(cdbString);
    cdbString.Seek(0);
    //Write to file...
    cdb->WriteToStream(configOut);
    configOut.Close();
 
	return True;
}

bool AdvancedConfigurator::DualVectorSort(int vector_dim, float * vector_x, float * vector_y){
	
	if(vector_dim <1){
		CStaticAssertErrorCondition(InitialisationError,"AdvancedConfigurator::DualVectorSort, vector_dim lower than 1, vector_dim =%d",vector_dim);
		return False;
	} 
	if(vector_dim == 1) return True;
	
	int i;
	int alfa;
	float temp;
	float min = *vector_x;
	int pos;
	
	for(alfa=0;alfa<vector_dim-1;alfa++){	
		pos=alfa;
		min=*(vector_x+alfa);
		
		for (i=alfa;i < vector_dim;i++){
			if (*(vector_x+i) <= min){
				pos = i;
				min = *(vector_x+i);
			}	
		}	

		temp = *(vector_x+alfa);
		*(vector_x+alfa) = *(vector_x+pos);
		*(vector_x+pos) = temp;
		temp = *(vector_y+alfa);
		*(vector_y+alfa) = *(vector_y+pos);
		*(vector_y+pos) = temp;
	}
	
	return True;
}

//	use only after the vectors are sorted
bool AdvancedConfigurator::RemoveRepeatedValues(int * vector_dim, float * vector_x, float * vector_y){
	
	if(*vector_dim <1){
		CStaticAssertErrorCondition(InitialisationError,"AdvancedConfigurator::RemoveRepeatedValues vector_dim lower than 1, vector_dim =%d",*vector_dim);
		return False;
	} 
	if(*vector_dim == 1) return True;
	
	int i=0;
	int alfa;
	
	for (alfa = 0; alfa < *vector_dim-1; alfa++){

		if ( *(vector_x+alfa) == *(vector_x+alfa+1)){
			
			for (i=alfa; i < *vector_dim-2; i++){
				*(vector_x+i+1) = *(vector_x+i+2);
				*(vector_y+i+1) = *(vector_y+i+2);
			}
			alfa--;
			*vector_dim -=1;
		}
	}

	return True;
}

bool AdvancedConfigurator::RemoveRepeatedValues(int option){
	
	int i;
	
	if(!MoveToTemp(option)) return False;
	
	if(vector_dim_temp <1){
		CStaticAssertErrorCondition(InitialisationError,"AdvancedConfigurator::RemoveRepeatedValues, vector_dim lower than 1, vector_dim =%d",vector_dim_temp);
		return False;
	} 
	if(vector_dim_temp == 1) return True;
	
	int alfa;
	
	for (alfa = 0; alfa < vector_dim_temp-1; alfa++){

		if ( temp_vector_x[alfa] == temp_vector_x[alfa+1]){
			
			for (i=alfa; i < vector_dim_temp-2; i++){
				temp_vector_x[i+1] = temp_vector_x[i+2];
				temp_vector_y[i+1] = temp_vector_y[i+2];
			}
			alfa--;
			vector_dim_temp -=1;
		}
	}

	if(vector_dim_temp <2){
		CStaticAssertErrorCondition(InitialisationError,"AdvancedConfigurator::RemoveRepeatedValues, insuficient points for a waveform, vector_dim_temp =%d",vector_dim_temp);
		return False;
	} 

	if(!RetrieveFromTemp(option)) return False;

	return True;
}

bool AdvancedConfigurator::DisplayPoint(int option, int index_to_display){

	int i;

	if(!MoveToTemp(option)) return False;

	if (index_to_display < vector_dim_temp){
		box_display_point_x = temp_vector_x[index_to_display];
//		box_display_point_y = temp_vector_y[index_to_display];
	}

	focus_on_y_form_bool = True;

	return True;
}

bool AdvancedConfigurator::RemovePoint(int option, int index_to_remove){
	
	int i;

	if(!MoveToTemp(option)) return False;
	
	if(vector_dim_temp <3){
		CStaticAssertErrorCondition(InitialisationError,"AdvancedConfigurator::RemovePoint, vector_dim lower than 3, vector_dim =%d",vector_dim_temp);
		return False;
	} 
	if( vector_dim_temp < (index_to_remove+1) || index_to_remove < 0 ){
		CStaticAssertErrorCondition(InitialisationError,"AdvancedConfigurator::RemovePoint, index_to_remove out of bounds, index_to_remove =%d",index_to_remove);
		return False;
	}
	
	for (i=index_to_remove; i < vector_dim_temp-1; i++){
		temp_vector_x[i] = temp_vector_x[i+1];
		temp_vector_y[i] = temp_vector_y[i+1];
	}
	vector_dim_temp -=1;

	if(!RetrieveFromTemp(option)) return False;

	return True;
}
bool AdvancedConfigurator::AddPoint(int option, float point_to_add_x, float point_to_add_y){

	int i;

	if(!MoveToTempWithLimits(option)) return False;
	
	if (point_to_add_y > temp_max_value) point_to_add_y = temp_max_value;
	if (point_to_add_y < temp_min_value) point_to_add_y = temp_min_value;
	
	temp_vector_x[vector_dim_temp] = point_to_add_x;
	temp_vector_y[vector_dim_temp] = point_to_add_y;
	
	vector_dim_temp++;
	
	DualVectorSort(vector_dim_temp, &temp_vector_x[0], &temp_vector_y[0]);
	RemoveRepeatedValues(&vector_dim_temp, &temp_vector_x[0], &temp_vector_y[0]);

	if(!RetrieveFromTemp(option)) return False;

	return True;
}

bool AdvancedConfigurator::MoveToTemp(int option){
	
	int i;
	
	if (option ==0){		//primary_breakdown
		vector_dim_temp = primary_breakdown_vector_size;
		temp_vector_x = new float[primary_breakdown_vector_size];
		temp_vector_y = new float[primary_breakdown_vector_size];
		for (i=0;i< primary_breakdown_vector_size; i++){
			temp_vector_x[i] = primary_breakdown_index_vector[i];
			temp_vector_y[i] = primary_breakdown_data_vector[i];
		}
	} 
	else if (option ==1){		//vertical_breakdown
		vector_dim_temp = vertical_breakdown_vector_size;
		temp_vector_x = new float[vertical_breakdown_vector_size];
		temp_vector_y = new float[vertical_breakdown_vector_size];
		for (i=0;i< vertical_breakdown_vector_size; i++){
			temp_vector_x[i] = vertical_breakdown_index_vector[i];
			temp_vector_y[i] = vertical_breakdown_data_vector[i];
		}
	}	
	else if (option ==2){		//horizontal_breakdown
		vector_dim_temp = horizontal_breakdown_vector_size;
		temp_vector_x = new float[horizontal_breakdown_vector_size];
		temp_vector_y = new float[horizontal_breakdown_vector_size];
		for (i=0;i< horizontal_breakdown_vector_size; i++){
			temp_vector_x[i] = horizontal_breakdown_index_vector[i];
			temp_vector_y[i] = horizontal_breakdown_data_vector[i];
		}
	}
	else if (option ==3){	//primary_breakdown
		vector_dim_temp = primary_breakdown_negative_vector_size;
		temp_vector_x = new float[primary_breakdown_negative_vector_size];
		temp_vector_y = new float[primary_breakdown_negative_vector_size];
		for (i=0;i< primary_breakdown_negative_vector_size; i++){
			temp_vector_x[i] = primary_breakdown_negative_index_vector[i];
			temp_vector_y[i] = primary_breakdown_negative_data_vector[i];
		}
	}
	else if (option ==4){		//vertical_breakdown
		vector_dim_temp = vertical_breakdown_negative_vector_size;
		temp_vector_x = new float[vertical_breakdown_negative_vector_size];
		temp_vector_y = new float[vertical_breakdown_negative_vector_size];
		for (i=0;i< vertical_breakdown_negative_vector_size; i++){
			temp_vector_x[i] = vertical_breakdown_negative_index_vector[i];
			temp_vector_y[i] = vertical_breakdown_negative_data_vector[i];
		}
	}
	else if (option ==5){		//horizontal_breakdown
		vector_dim_temp = horizontal_breakdown_negative_vector_size;
		temp_vector_x = new float[horizontal_breakdown_negative_vector_size];
		temp_vector_y = new float[horizontal_breakdown_negative_vector_size];
		for (i=0;i< horizontal_breakdown_negative_vector_size; i++){
			temp_vector_x[i] = horizontal_breakdown_negative_index_vector[i];
			temp_vector_y[i] = horizontal_breakdown_negative_data_vector[i];
		}
	}
	else if (option ==6){		//primary_inversion
		vector_dim_temp = primary_inversion_positive_vector_size;
		temp_vector_x = new float[primary_inversion_positive_vector_size];
		temp_vector_y = new float[primary_inversion_positive_vector_size];
		for (i=0;i< primary_inversion_positive_vector_size; i++){
			temp_vector_x[i] = primary_inversion_positive_index_vector[i];
			temp_vector_y[i] = primary_inversion_positive_data_vector[i];
		}
	}
	else if (option ==7){		//vertical_inversion
		vector_dim_temp = vertical_inversion_positive_vector_size;
		temp_vector_x = new float[vertical_inversion_positive_vector_size];
		temp_vector_y = new float[vertical_inversion_positive_vector_size];
		for (i=0;i< vertical_inversion_positive_vector_size; i++){
			temp_vector_x[i] = vertical_inversion_positive_index_vector[i];
			temp_vector_y[i] = vertical_inversion_positive_data_vector[i];
		}
	}
	else if (option ==8){		//horizontal_inversion
		vector_dim_temp = horizontal_inversion_positive_vector_size;
		temp_vector_x = new float[horizontal_inversion_positive_vector_size];
		temp_vector_y = new float[horizontal_inversion_positive_vector_size];
		for (i=0;i< horizontal_inversion_positive_vector_size; i++){
		temp_vector_x[i] = horizontal_inversion_positive_index_vector[i];
			temp_vector_y[i] = horizontal_inversion_positive_data_vector[i];
		}
	}		
	else if (option ==9){		//primary_inversion
		vector_dim_temp = primary_inversion_negative_vector_size;
		temp_vector_x = new float[primary_inversion_negative_vector_size];
		temp_vector_y = new float[primary_inversion_negative_vector_size];
		for (i=0;i< primary_inversion_negative_vector_size; i++){
			temp_vector_x[i] = primary_inversion_negative_index_vector[i];
			temp_vector_y[i] = primary_inversion_negative_data_vector[i];
		}
	}
	else if (option ==10){	//vertical_inversion
		vector_dim_temp = vertical_inversion_negative_vector_size;
		temp_vector_x = new float[vertical_inversion_negative_vector_size];
		temp_vector_y = new float[vertical_inversion_negative_vector_size];
		for (i=0;i< vertical_inversion_negative_vector_size; i++){
			temp_vector_x[i] = vertical_inversion_negative_index_vector[i];
			temp_vector_y[i] = vertical_inversion_negative_data_vector[i];
		}
	}
	else if (option ==11){	//horizontal_inversion
		vector_dim_temp = horizontal_inversion_negative_vector_size;
		temp_vector_x = new float[horizontal_inversion_negative_vector_size];
		temp_vector_y = new float[horizontal_inversion_negative_vector_size];
		for (i=0;i< horizontal_inversion_negative_vector_size; i++){
			temp_vector_x[i] = horizontal_inversion_negative_index_vector[i];
			temp_vector_y[i] = horizontal_inversion_negative_data_vector[i];
		}
	}
	else if (option ==12){	//toroidal
		vector_dim_temp = toroidal_1_p_vector_size;
		temp_vector_x = new float[toroidal_1_p_vector_size];
		temp_vector_y = new float[toroidal_1_p_vector_size];
		for (i=0;i< toroidal_1_p_vector_size; i++){
			temp_vector_x[i] = toroidal_1_p_index_vector[i];
			temp_vector_y[i] = toroidal_1_p_data_vector[i];
		}
	}
	else if (option ==13){	//puffing_1_p
		vector_dim_temp = puffing_1_p_vector_size;
		temp_vector_x = new float[puffing_1_p_vector_size];
		temp_vector_y = new float[puffing_1_p_vector_size];
		for (i=0;i< puffing_1_p_vector_size; i++){
			temp_vector_x[i] = puffing_1_p_index_vector[i];
			temp_vector_y[i] = puffing_1_p_data_vector[i];
		}
	}
	else if (option ==14){	//puffing_1_n
		vector_dim_temp = puffing_1_n_vector_size;
		temp_vector_x = new float[puffing_1_n_vector_size];
		temp_vector_y = new float[puffing_1_n_vector_size];
		for (i=0;i< puffing_1_n_vector_size; i++){
			temp_vector_x[i] = puffing_1_n_index_vector[i];
			temp_vector_y[i] = puffing_1_n_data_vector[i];
		}
	}
	else if (option ==15){	//puffing_2_p
		vector_dim_temp = puffing_2_p_vector_size;
		temp_vector_x = new float[puffing_2_p_vector_size];
		temp_vector_y = new float[puffing_2_p_vector_size];
		for (i=0;i< puffing_2_p_vector_size; i++){
			temp_vector_x[i] = puffing_2_p_index_vector[i];
			temp_vector_y[i] = puffing_2_p_data_vector[i];
		}
	}
	else if (option ==16){	//puffing_2_n
		vector_dim_temp = puffing_2_n_vector_size;
		temp_vector_x = new float[puffing_2_n_vector_size];
		temp_vector_y = new float[puffing_2_n_vector_size];
		for (i=0;i< puffing_2_n_vector_size; i++){
			temp_vector_x[i] = puffing_2_n_index_vector[i];
			temp_vector_y[i] = puffing_2_n_data_vector[i];
		}
	}
	else if (option ==17){	//puffing_absolute
		vector_dim_temp = puffing_absolute_time_vector_size;
		temp_vector_x = new float[puffing_absolute_time_vector_size];
		temp_vector_y = new float[puffing_absolute_time_vector_size];
		for (i=0;i< puffing_absolute_time_vector_size; i++){
			temp_vector_x[i] = puffing_absolute_time_index_vector[i];
			temp_vector_y[i] = puffing_absolute_time_data_vector[i];
		}
	}
	else {
		CStaticAssertErrorCondition(InitialisationError,"AdvancedConfigurator::MoveToTemp, option out of bounds, option =%d",option);
		return False;
	}

	return True;
}
bool AdvancedConfigurator::MoveToTempWithLimits(int option){
	
	int i;
	
	if (option ==0){		//primary_breakdown
		vector_dim_temp = primary_breakdown_vector_size;
		temp_max_value = primary_1_p_max_value;
		temp_min_value = primary_1_p_min_value;
		temp_vector_x = new float[primary_breakdown_vector_size+1];
		temp_vector_y = new float[primary_breakdown_vector_size+1];
		for (i=0;i< primary_breakdown_vector_size; i++){
			temp_vector_x[i] = primary_breakdown_index_vector[i];
			temp_vector_y[i] = primary_breakdown_data_vector[i];
		}
	} 
	else if (option ==1){		//vertical_breakdown
		vector_dim_temp = vertical_breakdown_vector_size;
		temp_max_value = vertical_1_p_max_value;
		temp_min_value = vertical_1_p_min_value;
		temp_vector_x = new float[vertical_breakdown_vector_size+1];
		temp_vector_y = new float[vertical_breakdown_vector_size+1];
		for (i=0;i< vertical_breakdown_vector_size; i++){
			temp_vector_x[i] = vertical_breakdown_index_vector[i];
			temp_vector_y[i] = vertical_breakdown_data_vector[i];
		}
	}	
	else if (option ==2){		//horizontal_breakdown
		vector_dim_temp = horizontal_breakdown_vector_size;
		temp_max_value = horizontal_1_p_max_value;
		temp_min_value = horizontal_1_p_min_value;
		temp_vector_x = new float[horizontal_breakdown_vector_size+1];
		temp_vector_y = new float[horizontal_breakdown_vector_size+1];
		for (i=0;i< horizontal_breakdown_vector_size; i++){
			temp_vector_x[i] = horizontal_breakdown_index_vector[i];
			temp_vector_y[i] = horizontal_breakdown_data_vector[i];
		}
	}
	else if (option ==3){	//primary_breakdown
		vector_dim_temp = primary_breakdown_negative_vector_size;
		temp_max_value = primary_1_p_max_value;
		temp_min_value = primary_1_p_min_value;
		temp_vector_x = new float[primary_breakdown_negative_vector_size+1];
		temp_vector_y = new float[primary_breakdown_negative_vector_size+1];
		for (i=0;i< primary_breakdown_negative_vector_size; i++){
			temp_vector_x[i] = primary_breakdown_negative_index_vector[i];
			temp_vector_y[i] = primary_breakdown_negative_data_vector[i];
		}
	}
	else if (option ==4){		//vertical_breakdown
		vector_dim_temp = vertical_breakdown_negative_vector_size;
		temp_max_value = vertical_1_p_max_value;
		temp_min_value = vertical_1_p_min_value;
		temp_vector_x = new float[vertical_breakdown_negative_vector_size+1];
		temp_vector_y = new float[vertical_breakdown_negative_vector_size+1];
		for (i=0;i< vertical_breakdown_negative_vector_size; i++){
			temp_vector_x[i] = vertical_breakdown_negative_index_vector[i];
			temp_vector_y[i] = vertical_breakdown_negative_data_vector[i];
		}
	}
	else if (option ==5){		//horizontal_breakdown
		vector_dim_temp = horizontal_breakdown_negative_vector_size;
		temp_max_value = horizontal_1_p_max_value;
		temp_min_value = horizontal_1_p_min_value;
		temp_vector_x = new float[horizontal_breakdown_negative_vector_size+1];
		temp_vector_y = new float[horizontal_breakdown_negative_vector_size+1];
		for (i=0;i< horizontal_breakdown_negative_vector_size; i++){
			temp_vector_x[i] = horizontal_breakdown_negative_index_vector[i];
			temp_vector_y[i] = horizontal_breakdown_negative_data_vector[i];
		}
	}
	else if (option ==6){		//primary_inversion
		vector_dim_temp = primary_inversion_positive_vector_size;
		temp_max_value = primary_1_p_max_value;
		temp_min_value = primary_1_p_min_value;
		temp_vector_x = new float[primary_inversion_positive_vector_size+1];
		temp_vector_y = new float[primary_inversion_positive_vector_size+1];
		for (i=0;i< primary_inversion_positive_vector_size; i++){
			temp_vector_x[i] = primary_inversion_positive_index_vector[i];
			temp_vector_y[i] = primary_inversion_positive_data_vector[i];
		}
	}
	else if (option ==7){		//vertical_inversion
		vector_dim_temp = vertical_inversion_positive_vector_size;
		temp_max_value = vertical_1_p_max_value;
		temp_min_value = vertical_1_p_min_value;
		temp_vector_x = new float[vertical_inversion_positive_vector_size+1];
		temp_vector_y = new float[vertical_inversion_positive_vector_size+1];
		for (i=0;i< vertical_inversion_positive_vector_size; i++){
			temp_vector_x[i] = vertical_inversion_positive_index_vector[i];
			temp_vector_y[i] = vertical_inversion_positive_data_vector[i];
		}
	}
	else if (option ==8){		//horizontal_inversion
		vector_dim_temp = horizontal_inversion_positive_vector_size;
		temp_max_value = horizontal_1_p_max_value;
		temp_min_value = horizontal_1_p_min_value;
		temp_vector_x = new float[horizontal_inversion_positive_vector_size+1];
		temp_vector_y = new float[horizontal_inversion_positive_vector_size+1];
		for (i=0;i< horizontal_inversion_positive_vector_size; i++){
		temp_vector_x[i] = horizontal_inversion_positive_index_vector[i];
			temp_vector_y[i] = horizontal_inversion_positive_data_vector[i];
		}
	}		
	else if (option ==9){		//primary_inversion
		vector_dim_temp = primary_inversion_negative_vector_size;
		temp_max_value = primary_1_p_max_value;
		temp_min_value = primary_1_p_min_value;
		temp_vector_x = new float[primary_inversion_negative_vector_size+1];
		temp_vector_y = new float[primary_inversion_negative_vector_size+1];
		for (i=0;i< primary_inversion_negative_vector_size; i++){
			temp_vector_x[i] = primary_inversion_negative_index_vector[i];
			temp_vector_y[i] = primary_inversion_negative_data_vector[i];
		}
	}
	else if (option ==10){	//vertical_inversion
		vector_dim_temp = vertical_inversion_negative_vector_size;
		temp_max_value = vertical_1_p_max_value;
		temp_min_value = vertical_1_p_min_value;
		temp_vector_x = new float[vertical_inversion_negative_vector_size+1];
		temp_vector_y = new float[vertical_inversion_negative_vector_size+1];
		for (i=0;i< vertical_inversion_negative_vector_size; i++){
			temp_vector_x[i] = vertical_inversion_negative_index_vector[i];
			temp_vector_y[i] = vertical_inversion_negative_data_vector[i];
		}
	}
	else if (option ==11){	//horizontal_inversion
		vector_dim_temp = horizontal_inversion_negative_vector_size;
		temp_max_value = horizontal_1_p_max_value;
		temp_min_value = horizontal_1_p_min_value;
		temp_vector_x = new float[horizontal_inversion_negative_vector_size+1];
		temp_vector_y = new float[horizontal_inversion_negative_vector_size+1];
		for (i=0;i< horizontal_inversion_negative_vector_size; i++){
			temp_vector_x[i] = horizontal_inversion_negative_index_vector[i];
			temp_vector_y[i] = horizontal_inversion_negative_data_vector[i];
		}
	}
	else if (option ==12){	//toroidal
		vector_dim_temp = toroidal_1_p_vector_size;
		temp_max_value = toroidal_1_p_max_value;
		temp_min_value = toroidal_1_p_min_value;
		temp_vector_x = new float[toroidal_1_p_vector_size+1];
		temp_vector_y = new float[toroidal_1_p_vector_size+1];
		for (i=0;i< toroidal_1_p_vector_size; i++){
			temp_vector_x[i] = toroidal_1_p_index_vector[i];
			temp_vector_y[i] = toroidal_1_p_data_vector[i];
		}
	}
	else if (option ==13){	//puffing_1_p
		vector_dim_temp = puffing_1_p_vector_size;
		temp_max_value = puffing_1_p_max_value;
		temp_min_value = puffing_1_p_min_value;
		temp_vector_x = new float[puffing_1_p_vector_size+1];
		temp_vector_y = new float[puffing_1_p_vector_size+1];
		for (i=0;i < puffing_1_p_vector_size; i++){
			temp_vector_x[i] = puffing_1_p_index_vector[i];
			temp_vector_y[i] = puffing_1_p_data_vector[i];
		}
	}
	else if (option ==14){	//puffing_1_n
		vector_dim_temp = puffing_1_n_vector_size;
		temp_max_value = puffing_1_p_max_value;
		temp_min_value = puffing_1_p_min_value;
		temp_vector_x = new float[puffing_1_n_vector_size+1];
		temp_vector_y = new float[puffing_1_n_vector_size+1];
		for (i=0;i< puffing_1_n_vector_size; i++){
			temp_vector_x[i] = puffing_1_n_index_vector[i];
			temp_vector_y[i] = puffing_1_n_data_vector[i];
		}
	}
	else if (option ==15){	//puffing_2_p
		vector_dim_temp = puffing_2_p_vector_size;
		temp_max_value = puffing_2_p_max_value;
		temp_min_value = puffing_2_p_min_value;
		temp_vector_x = new float[puffing_2_p_vector_size+1];
		temp_vector_y = new float[puffing_2_p_vector_size+1];
		for (i=0;i < puffing_2_p_vector_size; i++){
			temp_vector_x[i] = puffing_2_p_index_vector[i];
			temp_vector_y[i] = puffing_2_p_data_vector[i];
		}
	}
	else if (option ==16){	//puffing_2_n
		vector_dim_temp = puffing_2_n_vector_size;
		temp_max_value = puffing_2_p_max_value;
		temp_min_value = puffing_2_p_min_value;
		temp_vector_x = new float[puffing_2_n_vector_size+1];
		temp_vector_y = new float[puffing_2_n_vector_size+1];
		for (i=0;i< puffing_2_n_vector_size; i++){
			temp_vector_x[i] = puffing_2_n_index_vector[i];
			temp_vector_y[i] = puffing_2_n_data_vector[i];
		}
	}
	else if (option ==17){	//puffing_absolute
		vector_dim_temp = puffing_absolute_time_vector_size;
		temp_max_value = puffing_1_p_max_value;
		temp_min_value = puffing_1_p_min_value;
		temp_vector_x = new float[puffing_absolute_time_vector_size+1];
		temp_vector_y = new float[puffing_absolute_time_vector_size+1];
		for (i=0;i< puffing_absolute_time_vector_size; i++){
			temp_vector_x[i] = puffing_absolute_time_index_vector[i];
			temp_vector_y[i] = puffing_absolute_time_data_vector[i];
		}
	}
	else {
		CStaticAssertErrorCondition(InitialisationError,"AdvancedConfigurator::MoveToTemp, option out of bounds, option =%d",option);
		return False;
	}
	
	return True;
}
	
bool AdvancedConfigurator::RetrieveFromTemp(int option){
	
	int i;
	
	if (option ==0){		//primary_1_p
		primary_breakdown_vector_size = vector_dim_temp;
		primary_breakdown_index_vector = new float[vector_dim_temp];
		primary_breakdown_data_vector = new float[vector_dim_temp];
		for (i=0;i< vector_dim_temp; i++){
			primary_breakdown_index_vector[i] = temp_vector_x[i];
			primary_breakdown_data_vector[i] = temp_vector_y[i];
		}
	}	
	else if (option ==1){		//vertical_1_p
		vertical_breakdown_vector_size = vector_dim_temp;
		vertical_breakdown_index_vector = new float[vector_dim_temp];
		vertical_breakdown_data_vector = new float[vector_dim_temp];
		for (i=0;i< vector_dim_temp; i++){
			vertical_breakdown_index_vector[i] = temp_vector_x[i];
			vertical_breakdown_data_vector[i] = temp_vector_y[i];
		}
	}
	else if (option ==2){		//horizontal_1_p
		horizontal_breakdown_vector_size = vector_dim_temp;
		horizontal_breakdown_index_vector = new float[vector_dim_temp];
		horizontal_breakdown_data_vector = new float[vector_dim_temp];
		for (i=0;i< vector_dim_temp; i++){
			horizontal_breakdown_index_vector[i] = temp_vector_x[i];
			horizontal_breakdown_data_vector[i] = temp_vector_y[i];
		}
	}
	else if (option ==3){		//primary_1_n
		primary_breakdown_negative_vector_size = vector_dim_temp;
		primary_breakdown_negative_index_vector = new float[vector_dim_temp];
		primary_breakdown_negative_data_vector = new float[vector_dim_temp];
		for (i=0;i< vector_dim_temp; i++){
			primary_breakdown_negative_index_vector[i] = temp_vector_x[i];
			primary_breakdown_negative_data_vector[i] = temp_vector_y[i];
		}
	}
	else if (option ==4){		//vertical_1_n
		vertical_breakdown_negative_vector_size = vector_dim_temp;
		vertical_breakdown_negative_index_vector = new float[vector_dim_temp];
		vertical_breakdown_negative_data_vector = new float[vector_dim_temp];
		for (i=0;i< vector_dim_temp; i++){
			vertical_breakdown_negative_index_vector[i] = temp_vector_x[i];
			vertical_breakdown_negative_data_vector[i] = temp_vector_y[i];
		}
	}
	else if (option ==5){		//horizontal_1_n
		horizontal_breakdown_negative_vector_size = vector_dim_temp;
		horizontal_breakdown_negative_index_vector = new float[vector_dim_temp];
		horizontal_breakdown_negative_data_vector = new float[vector_dim_temp];
		for (i=0;i< vector_dim_temp; i++){
			horizontal_breakdown_negative_index_vector[i] = temp_vector_x[i];
			horizontal_breakdown_negative_data_vector[i] = temp_vector_y[i];
		}
	}
	else if (option ==6){		//primary_2_p
		primary_inversion_positive_vector_size = vector_dim_temp;
		primary_inversion_positive_index_vector = new float[vector_dim_temp];
		primary_inversion_positive_data_vector = new float[vector_dim_temp];
		for (i=0;i< vector_dim_temp; i++){
			primary_inversion_positive_index_vector[i] = temp_vector_x[i];
			primary_inversion_positive_data_vector[i] = temp_vector_y[i];
		}
	}
	else if (option ==7){		//vertical_2_p
		vertical_inversion_positive_vector_size = vector_dim_temp;
		vertical_inversion_positive_index_vector = new float[vector_dim_temp];
		vertical_inversion_positive_data_vector = new float[vector_dim_temp];
		for (i=0;i< vector_dim_temp; i++){
			vertical_inversion_positive_index_vector[i] = temp_vector_x[i];
			vertical_inversion_positive_data_vector[i] = temp_vector_y[i];
		}
	}
	else if (option ==8){		//horizontal_2_p
		horizontal_inversion_positive_vector_size = vector_dim_temp;
		horizontal_inversion_positive_index_vector = new float[vector_dim_temp];
		horizontal_inversion_positive_data_vector = new float[vector_dim_temp];
		for (i=0;i< vector_dim_temp; i++){
			horizontal_inversion_positive_index_vector[i] = temp_vector_x[i];
			horizontal_inversion_positive_data_vector[i] = temp_vector_y[i];
		}
	}		
	else if (option ==9){		//primary_2_n
		primary_inversion_negative_vector_size = vector_dim_temp;
		primary_inversion_negative_index_vector = new float[vector_dim_temp];
		primary_inversion_negative_data_vector = new float[vector_dim_temp];
		for (i=0;i< vector_dim_temp; i++){
			primary_inversion_negative_index_vector[i] = temp_vector_x[i];
			primary_inversion_negative_data_vector[i] = temp_vector_y[i];
		}
	}
	else if (option ==10){	//vertical_2_n
		vertical_inversion_negative_vector_size = vector_dim_temp;
		vertical_inversion_negative_index_vector = new float[vector_dim_temp];
		vertical_inversion_negative_data_vector = new float[vector_dim_temp];
		for (i=0;i< vector_dim_temp; i++){
			vertical_inversion_negative_index_vector[i] = temp_vector_x[i];
			vertical_inversion_negative_data_vector[i] = temp_vector_y[i];
		}
	}
	else if (option ==11){	//horizontal_2_n
		horizontal_inversion_negative_vector_size = vector_dim_temp;
		horizontal_inversion_negative_index_vector = new float[vector_dim_temp];
		horizontal_inversion_negative_data_vector = new float[vector_dim_temp];
		for (i=0;i< vector_dim_temp; i++){
			horizontal_inversion_negative_index_vector[i] = temp_vector_x[i];
			horizontal_inversion_negative_data_vector[i] = temp_vector_y[i];
		}
	}		
	else if (option ==12){	//toroidal
		toroidal_1_p_vector_size = vector_dim_temp;
		toroidal_1_p_index_vector = new float[vector_dim_temp];
		toroidal_1_p_data_vector = new float[vector_dim_temp];
		for (i=0;i< vector_dim_temp; i++){
			toroidal_1_p_index_vector[i] = temp_vector_x[i];
			toroidal_1_p_data_vector[i] = temp_vector_y[i];
		}
	}		
	else if (option ==13){	//puffing_1_p
		puffing_1_p_vector_size = vector_dim_temp;
		puffing_1_p_index_vector = new float[vector_dim_temp];
		puffing_1_p_data_vector = new float[vector_dim_temp];
		for (i=0;i< vector_dim_temp; i++){
			puffing_1_p_index_vector[i] = temp_vector_x[i];
			puffing_1_p_data_vector[i] = temp_vector_y[i];
		}
	}		
	else if (option ==14){	//puffing_1_n
		puffing_1_n_vector_size = vector_dim_temp;
		puffing_1_n_index_vector = new float[vector_dim_temp];
		puffing_1_n_data_vector = new float[vector_dim_temp];
		for (i=0;i< vector_dim_temp; i++){
			puffing_1_n_index_vector[i] = temp_vector_x[i];
			puffing_1_n_data_vector[i] = temp_vector_y[i];
		}
	}		
	else if (option ==15){	//puffing_2_p
		puffing_2_p_vector_size = vector_dim_temp;
		puffing_2_p_index_vector = new float[vector_dim_temp];
		puffing_2_p_data_vector = new float[vector_dim_temp];
		for (i=0;i< vector_dim_temp; i++){
			puffing_2_p_index_vector[i] = temp_vector_x[i];
			puffing_2_p_data_vector[i] = temp_vector_y[i];
		}
	}		
	else if (option ==16){	//puffing_2_n
		puffing_2_n_vector_size = vector_dim_temp;
		puffing_2_n_index_vector = new float[vector_dim_temp];
		puffing_2_n_data_vector = new float[vector_dim_temp];
		for (i=0;i< vector_dim_temp; i++){
			puffing_2_n_index_vector[i] = temp_vector_x[i];
			puffing_2_n_data_vector[i] = temp_vector_y[i];
		}
	}		
	
	else if (option ==17){	//puffing_absolute_time
		puffing_absolute_time_vector_size = vector_dim_temp;
		puffing_absolute_time_index_vector = new float[vector_dim_temp];
		puffing_absolute_time_data_vector = new float[vector_dim_temp];
		for (i=0;i< vector_dim_temp; i++){
			puffing_absolute_time_index_vector[i] = temp_vector_x[i];
			puffing_absolute_time_data_vector[i] = temp_vector_y[i];
		}
	}		
	else {
		CStaticAssertErrorCondition(InitialisationError,"AdvancedConfigurator::RemovePoint, option out of bounds, option =%d",option);
		return False;
	}	
	
	return True;
}
