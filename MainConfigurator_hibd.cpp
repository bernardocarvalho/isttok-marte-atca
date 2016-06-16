//	Object properties
#define OBJECTNAME "Discharge Configurator"
/*
modified by Rafael Henriques - rhenriques@ipfn.ist.utl.pt
date: 2016-04-11
changes: add the hibd option for plasma position measurement
*/
/*
#define VERSION "1.00"
#define LASTDATE "2011-03-24"
#define AUTHOR "Ivo Carvalho - ivoc@ipfn.ist.utl.pt"
#define DESCRIPTION "HTML page to change and upload the configuration file parameters"
*/
#define HEADERIMAGELINK "http://www.ipfn.ist.utl.pt/ipfnPortalLayout/themes/ipfn/_img_/logoIPFN_Topo_officialColours.png"
#define FOOTERIMAGELINK "http://backoffice.ipfn.ist.utl.pt/mgallery/default.asp?obj=1"

#include "MainConfigurator.h"
#include "MessageEnvelope.h"
#include "Message.h"


OBJECTLOADREGISTER(MainConfigurator,"$Id: MainConfigurator.cpp,v 1.0 2009/05/22 10:40:56 ivoc Exp $")


   /**  initialise an object from a set of configs */
bool MainConfigurator::ObjectLoadSetup(ConfigurationDataBase & info, StreamInterface * err){
	
	Ip_max_threshold_value = 10000;
	max_value =1000;
	timewindows_dropdown_vector = new int[42];
	int i;
	timewindows_time = new float[14];
	timewindows_bool_vector = new bool[14];
	delete_selected_points_bool = False;
	vector_dim_temp = 0;
	
	already_started = False;
	
	this->graphics_support = new SVGGraphicSupport();
	
	save_filename.Printf("last.cfg");
	
	for(i=0;i<42;i++) timewindows_dropdown_vector[i]=0;
	

        GCReferenceContainer::ObjectLoadSetup(info,err);

        CDBExtended &cdbx = (CDBExtended &)info;

        cdbx.ReadFString(configFileID, "configFileID", "cfgFile");

	if(!cdbx.ReadFString(Description, "Description"))
	{
		CStaticAssertErrorCondition(InitialisationError,"MainConfigurator::ObjectLoadSetup: %s Could not read Description",this->Name());
		return False;
	}
		
	// Move to the graphics branch
	if(!cdbx->Move("graphics"))
	{
		CStaticAssertErrorCondition(InitialisationError,"MainConfigurator::ObjectLoadSetup: %s Could not move to \"graphics\"",this->Name());
		return False;
	}
	
		if(!cdbx.ReadInt32(screen_resolution_x, "screen_resolution_x"))
		{
			CStaticAssertErrorCondition(InitialisationError,"MainConfigurator::ObjectLoadSetup: %s Could not read graph_width",this->Name());
			return False;
		}
		if(!cdbx.ReadInt32(screen_resolution_y, "screen_resolution_y"))
		{
			CStaticAssertErrorCondition(InitialisationError,"MainConfigurator::ObjectLoadSetup: %s Could not read graph_height",this->Name());
			return False;
		}

	// Move back to the GAM main branch
	cdbx->MoveToFather();
	
//			CStaticAssertErrorCondition(InitialisationError,"MainConfigurator::ObjectLoadSetup: %d %d",screen_resolution_x,screen_resolution_y);
	

	// Move to the file branch
	if(!cdbx->Move("colours"))
	{
		CStaticAssertErrorCondition(InitialisationError,"MainConfigurator::ObjectLoadSetup: %s Could not move to \"colours\"",this->Name());
		return False;
	}
		if(!cdbx.ReadFString(header_colour, "header_colour"))
		{
			CStaticAssertErrorCondition(InitialisationError,"MainConfigurator::ObjectLoadSetup: %s Could not read header_colour",this->Name());
			return False;
		}
		if(!cdbx.ReadFString(diagnostics_parameters_colour, "diagnostics_parameters_colour"))
		{
			CStaticAssertErrorCondition(InitialisationError,"MainConfigurator::ObjectLoadSetup: %s Could not read diagnostics_parameters_colour",this->Name());
			return False;
		}
		if(!cdbx.ReadFString(control_parameters_colour, "control_parameters_colour"))
		{
			CStaticAssertErrorCondition(InitialisationError,"MainConfigurator::ObjectLoadSetup: %s Could not read control_parameters_colour",this->Name());
			return False;
		}
		if(!cdbx.ReadFString(time_windows_colour, "time_windows_colour"))
		{
			CStaticAssertErrorCondition(InitialisationError,"MainConfigurator::ObjectLoadSetup: %s Could not read time_windows_colour",this->Name());
			return False;
		}
		if(!cdbx.ReadFString(edit_colour, "edit_colour"))
		{
			CStaticAssertErrorCondition(InitialisationError,"MainConfigurator::ObjectLoadSetup: %s Could not read edit_colour",this->Name());
			return False;
		}
		if(!cdbx.ReadFString(footer_colour, "footer_colour"))
		{
			CStaticAssertErrorCondition(InitialisationError,"MainConfigurator::ObjectLoadSetup: %s Could not read footer_colour",this->Name());
			return False;
		}
	// Move back to the GAM main branch
	cdbx->MoveToFather();
	
//	CStaticAssertErrorCondition(InitialisationError,"MainConfigurator::ObjectLoadSetup: %s %s %s %s %s %s",header_colour.Buffer(),diagnostics_parameters_colour.Buffer(),control_parameters_colour.Buffer(),time_windows_colour.Buffer(),edit_colour.Buffer(),footer_colour.Buffer());
	
	
	if(!cdbx->Move("default_directories"))
	{
		CStaticAssertErrorCondition(InitialisationError,"MainConfigurator::ObjectLoadSetup: %s Could not move to \"default_directories\"",this->Name());
		return False;
	}
		if(!cdbx.ReadFString(default_config_file_path, "default_config_file_path"))
		{
			CStaticAssertErrorCondition(InitialisationError,"MainConfigurator::ObjectLoadSetup: %s Could not read default_config_file_path",this->Name());
			return False;
		}
		if(!cdbx.ReadFString(config_files_directory, "config_files_directory"))
		{
			CStaticAssertErrorCondition(InitialisationError,"MainConfigurator::ObjectLoadSetup: %s Could not read config_files_directory",this->Name());
			return False;
		}
	// Move back to the GAM main branch
	cdbx->MoveToFather();
	
	graphic_select = 0;
	selected_load_directory = "..";
	selected_load_file = "last.cfg";
	
	this->ReadConfigurationFile((char *)default_config_file_path.Buffer());	
	
//	this->WriteConfigurationFileWithChanges((char *)default_config_file_path.Buffer(), (char *) "/home/atcatest/Dropbox/code/atca/sandbox/testeparvo.cfg");
		

	if(!cdbx.ReadFString(marteLocation, "MARTeLocation", "MARTe")){
		AssertErrorCondition(Warning, "ObjectLoadSetup::MARTe location wasn not specified. Using default: %s", marteLocation.Buffer());
	}

	return HttpInterface::ObjectLoadSetup(info,err);
}
	
bool MainConfigurator::PrintHTTPForm(HtmlStream &hmStream){
	
//	to view the limit, delete after complete	
//	hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 0px; left: 1200px;\"");
//	hmStream.SSPrintf(HtmlTagStreamMode, "TEXTAREA NAME=\"form_description\" COLS=1 ROWS=\"1\" WRAP=\"SOFT\"></TEXTAREA");
//	hmStream.SSPrintf(HtmlTagStreamMode, "/div");
	
	FString temp_string_1;
	int i;

	hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 10px; left: 1050px;\"><b>Units used:</b></div");
	hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 30px; left: 1050px;\">Time in miliseconds</div");
	hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 50px; left: 1050px;\">Distance in milimeters</div");
	hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 70px; left: 1050px;\">Current in Ampere</div");

	hmStream.SSPrintf(HtmlTagStreamMode, "form enctype=\"multipart/form-data\" method=\"post\"");
	
//	diagnostics
	hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"top: 130;left: 10;position: absolute;z-index: 1;\"><b>Radial position diagnostics:</b></div");
	hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"top: 130;left: 310;position: absolute;z-index: 1;\"><b>Vertical position diagnostics:</b></div");
	hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"top: 130;left: 590;position: absolute;z-index: 1;\"><b>Other calculations:</b></div");
	hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"top: 150;left: 140;position: absolute;z-index: 1;\">Low Ip</div");
	hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"top: 150;left: 200;position: absolute;z-index: 1;\">High Ip</div");
	hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"top: 150;left: 440;position: absolute;z-index: 1;\">Low Ip</div");
	hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"top: 150;left: 500;position: absolute;z-index: 1;\">High Ip</div");

	hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 120px; left: 780px;\">File Description:</div");

//		horizontal position checkboxs + text
	hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 170px; left: 10px;\"");
	if (tomography_radial_bool) hmStream.SSPrintf(HtmlTagStreamMode, "input type=\"checkbox\" name=\"form_tomography_radial_bool\" CHECKED"); 
	else hmStream.SSPrintf(HtmlTagStreamMode, "input type=\"checkbox\" name=\"form_tomography_radial_bool\"");
	hmStream.Printf("tomography");
	hmStream.SSPrintf(HtmlTagStreamMode, "/div");
	
	hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 192px; left: 10px;\"");
	if (electric_radial_bool) hmStream.SSPrintf(HtmlTagStreamMode, "input type=\"checkbox\" name=\"form_electric_radial_bool\" CHECKED"); 
	else hmStream.SSPrintf(HtmlTagStreamMode, "input type=\"checkbox\" name=\"form_electric_radial_bool\"");
	hmStream.Printf("electric");
	hmStream.SSPrintf(HtmlTagStreamMode, "/div");
	
	hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 214px; left: 10px;\"");
	if (magnetic_radial_bool) hmStream.SSPrintf(HtmlTagStreamMode, "input type=\"checkbox\" name=\"form_magnetic_radial_bool\" CHECKED"); 
	else hmStream.SSPrintf(HtmlTagStreamMode, "input type=\"checkbox\" name=\"form_magnetic_radial_bool\"");
	hmStream.Printf("magnetic");
	hmStream.SSPrintf(HtmlTagStreamMode, "/div");
	
	hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 236px; left: 10px;\"");
	if (cosine_radial_bool) hmStream.SSPrintf(HtmlTagStreamMode, "input type=\"checkbox\" name=\"form_cosine_radial_bool\" CHECKED"); 
	else hmStream.SSPrintf(HtmlTagStreamMode, "input type=\"checkbox\" name=\"form_cosine_radial_bool\"");
	hmStream.Printf("cosine");
	hmStream.SSPrintf(HtmlTagStreamMode, "/div");
	
	hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 258px; left: 10px;\"");
	if (hibd_radial_bool) hmStream.SSPrintf(HtmlTagStreamMode, "input type=\"checkbox\" name=\"form_hibd_radial_bool\" CHECKED"); 
	else hmStream.SSPrintf(HtmlTagStreamMode, "input type=\"checkbox\" name=\"form_hibd_radial_bool\"");
	hmStream.Printf("hibd");
	hmStream.SSPrintf(HtmlTagStreamMode, "/div");
	
//		vertical position checkboxs + text
	hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 170px; left: 310px;\"");
	if (tomography_vertical_bool) hmStream.SSPrintf(HtmlTagStreamMode, "input type=\"checkbox\" name=\"form_tomography_vertical_bool\" CHECKED"); 
	else hmStream.SSPrintf(HtmlTagStreamMode, "input type=\"checkbox\" name=\"form_tomography_vertical_bool\"");
	hmStream.Printf("tomography");
	hmStream.SSPrintf(HtmlTagStreamMode, "/div");
	
	hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 192px; left: 310px;\"");
	if (electric_vertical_bool) hmStream.SSPrintf(HtmlTagStreamMode, "input type=\"checkbox\" name=\"form_electric_vertical_bool\" CHECKED"); 
	else hmStream.SSPrintf(HtmlTagStreamMode, "input type=\"checkbox\" name=\"form_electric_vertical_bool\"");
	hmStream.Printf("electric");
	hmStream.SSPrintf(HtmlTagStreamMode, "/div");
	
	hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 214px; left: 310px;\"");
	if (magnetic_vertical_bool) hmStream.SSPrintf(HtmlTagStreamMode, "input type=\"checkbox\" name=\"form_magnetic_vertical_bool\" CHECKED"); 
	else hmStream.SSPrintf(HtmlTagStreamMode, "input type=\"checkbox\" name=\"form_magnetic_vertical_bool\"");
	hmStream.Printf("magnetic");
	hmStream.SSPrintf(HtmlTagStreamMode, "/div");
	
	hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 236px; left: 310px;\"");
	if (sine_vertical_bool) hmStream.SSPrintf(HtmlTagStreamMode, "input type=\"checkbox\" name=\"form_sine_vertical_bool\" CHECKED"); 
	else hmStream.SSPrintf(HtmlTagStreamMode, "input type=\"checkbox\" name=\"form_sine_vertical_bool\"");
	hmStream.Printf("sine");
	hmStream.SSPrintf(HtmlTagStreamMode, "/div");
	
	hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 258px; left: 310px;\"");
	if (hibd_vertical_bool) hmStream.SSPrintf(HtmlTagStreamMode, "input type=\"checkbox\" name=\"form_hibd_vertical_bool\" CHECKED"); 
	else hmStream.SSPrintf(HtmlTagStreamMode, "input type=\"checkbox\" name=\"form_hibd_vertical_bool\"");
	hmStream.Printf("hibd");
	hmStream.SSPrintf(HtmlTagStreamMode, "/div");
	
	hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 140px; left: 780px;\"");
	hmStream.SSPrintf(HtmlTagStreamMode, "TEXTAREA NAME=\"form_description\" COLS=50 ROWS=\"7\" WRAP=\"SOFT\">%s</TEXTAREA", Description.Buffer());
	hmStream.SSPrintf(HtmlTagStreamMode, "/div");


//		interferometry radial control bool
	hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 160px; left: 590px;\"");
	if (interferometry_radial_control_bool) hmStream.SSPrintf(HtmlTagStreamMode, "input type=\"checkbox\" name=\"form_interferometry_radial_control_bool\" CHECKED"); 
	else hmStream.SSPrintf(HtmlTagStreamMode, "input type=\"checkbox\" name=\"form_interferometry_radial_control_bool\"");
	hmStream.SSPrintf(HtmlTagStreamMode, "/div");
	hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 152px; left: 610px;\">use interferometry</div");
	hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 170px; left: 610px;\">for radial control</div");


	hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 200px; left: 590px;\"");
	hmStream.Printf("Ip threshold:");
	hmStream.SSPrintf(HtmlTagStreamMode, "/div");
	
	

	if (!edit_weights){
		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 100px; left: 230px;\"");
		hmStream.SSPrintf(HtmlTagStreamMode, "input type=\"submit\" name=\"submit_edit_weights\" value=\"Edit parameters\"");
		hmStream.SSPrintf(HtmlTagStreamMode, "/div");

//		print current limit
		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 200px; left: 680px;\"");
		hmStream.Printf("%0.1f A",high_current_threshold_value);
		hmStream.SSPrintf(HtmlTagStreamMode, "/div");

//		print weights
		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 170px; left: 140px;\"");
		hmStream.Printf("%0.2f",tomography_radial_low_current_weight);
		hmStream.SSPrintf(HtmlTagStreamMode, "/div");

		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 170px; left: 200px;\"");
		hmStream.Printf("%0.2f",tomography_radial_high_current_weight);
		hmStream.SSPrintf(HtmlTagStreamMode, "/div");

		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 192px; left: 140px;\"");
		hmStream.Printf("%0.2f",electric_radial_low_current_weight);
		hmStream.SSPrintf(HtmlTagStreamMode, "/div");

		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 192px; left: 200px;\"");
		hmStream.Printf("%0.2f",electric_radial_high_current_weight);
		hmStream.SSPrintf(HtmlTagStreamMode, "/div");

		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 214px; left: 140px;\"");
		hmStream.Printf("%0.2f",magnetic_radial_low_current_weight);
		hmStream.SSPrintf(HtmlTagStreamMode, "/div");

		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 214px; left: 200px;\"");
		hmStream.Printf("%0.2f",magnetic_radial_high_current_weight);
		hmStream.SSPrintf(HtmlTagStreamMode, "/div");

		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 236px; left: 140px;\"");
		hmStream.Printf("%0.2f",cosine_radial_low_current_weight);
		hmStream.SSPrintf(HtmlTagStreamMode, "/div");

		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 236px; left: 200px;\"");
		hmStream.Printf("%0.2f",cosine_radial_high_current_weight);
		hmStream.SSPrintf(HtmlTagStreamMode, "/div");

		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 258px; left: 140px;\"");
		hmStream.Printf("%0.2f",hibd_radial_low_current_weight);
		hmStream.SSPrintf(HtmlTagStreamMode, "/div");

		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 258px; left: 200px;\"");
		hmStream.Printf("%0.2f",hibd_radial_high_current_weight);
		hmStream.SSPrintf(HtmlTagStreamMode, "/div");

		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 170px; left: 440px;\"");
		hmStream.Printf("%0.2f",tomography_vertical_low_current_weight);
		hmStream.SSPrintf(HtmlTagStreamMode, "/div");

		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 170px; left: 500px;\"");
		hmStream.Printf("%0.2f",tomography_vertical_high_current_weight);
		hmStream.SSPrintf(HtmlTagStreamMode, "/div");

		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 192px; left: 440px;\"");
		hmStream.Printf("%0.2f",electric_vertical_low_current_weight);
		hmStream.SSPrintf(HtmlTagStreamMode, "/div");

		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 192px; left: 500px;\"");
		hmStream.Printf("%0.2f",electric_vertical_high_current_weight);
		hmStream.SSPrintf(HtmlTagStreamMode, "/div");

		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 214px; left: 440px;\"");
		hmStream.Printf("%0.2f",magnetic_vertical_low_current_weight);
		hmStream.SSPrintf(HtmlTagStreamMode, "/div");

		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 214px; left: 500px;\"");
		hmStream.Printf("%0.2f",magnetic_vertical_high_current_weight);
		hmStream.SSPrintf(HtmlTagStreamMode, "/div");

		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 236px; left: 440px;\"");
		hmStream.Printf("%0.2f",sine_vertical_low_current_weight);
		hmStream.SSPrintf(HtmlTagStreamMode, "/div");

		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 236px; left: 500px;\"");
		hmStream.Printf("%0.2f",sine_vertical_high_current_weight);
		hmStream.SSPrintf(HtmlTagStreamMode, "/div");

		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 258px; left: 440px;\"");
		hmStream.Printf("%0.2f",hibd_vertical_low_current_weight);
		hmStream.SSPrintf(HtmlTagStreamMode, "/div");

		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 258px; left: 500px;\"");
		hmStream.Printf("%0.2f",hibd_vertical_high_current_weight);
		hmStream.SSPrintf(HtmlTagStreamMode, "/div");
	}
	else 
	{
		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 100px; left: 230px;\"");
		hmStream.SSPrintf(HtmlTagStreamMode, "input type=\"submit\" name=\"submit_save_weights\" value=\"Save parameters\"");
		hmStream.SSPrintf(HtmlTagStreamMode, "/div");
			
//		edit current limit
		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 200px; left: 680px;\"");
		hmStream.SSPrintf(HtmlTagStreamMode, "input type=\"text\" size=\"2\" maxlength=\"6\" name=\"form_high_current_threshold_value\" value=\"%0.1f\"",high_current_threshold_value);
		hmStream.SSPrintf(HtmlTagStreamMode, "/div");

//		edit weights
		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 170px; left: 140px;\"");
		hmStream.SSPrintf(HtmlTagStreamMode, "input type=\"text\" size=\"2\" maxlength=\"6\" name=\"form_tomography_radial_low_current_weight\" value=\"%0.2f\"",tomography_radial_low_current_weight);
		hmStream.SSPrintf(HtmlTagStreamMode, "/div");

		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 170px; left: 200px;\"");
		hmStream.SSPrintf(HtmlTagStreamMode, "input type=\"text\" size=\"2\" maxlength=\"6\" name=\"form_tomography_radial_high_current_weight\" value=\"%0.2f\"",tomography_radial_high_current_weight);
		hmStream.SSPrintf(HtmlTagStreamMode, "/div");

		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 192px; left: 140px;\"");
		hmStream.SSPrintf(HtmlTagStreamMode, "input type=\"text\" size=\"2\" maxlength=\"6\" name=\"form_electric_radial_low_current_weight\" value=\"%0.2f\"",electric_radial_low_current_weight);
		hmStream.SSPrintf(HtmlTagStreamMode, "/div");

		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 192px; left: 200px;\"");
		hmStream.SSPrintf(HtmlTagStreamMode, "input type=\"text\" size=\"2\" maxlength=\"6\" name=\"form_electric_radial_high_current_weight\" value=\"%0.2f\"",electric_radial_high_current_weight);
		hmStream.SSPrintf(HtmlTagStreamMode, "/div");

		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 214px; left: 140px;\"");
		hmStream.SSPrintf(HtmlTagStreamMode, "input type=\"text\" size=\"2\" maxlength=\"6\" name=\"form_magnetic_radial_low_current_weight\" value=\"%0.2f\"",magnetic_radial_low_current_weight);
		hmStream.SSPrintf(HtmlTagStreamMode, "/div");

		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 214px; left: 200px;\"");
		hmStream.SSPrintf(HtmlTagStreamMode, "input type=\"text\" size=\"2\" maxlength=\"6\" name=\"form_magnetic_radial_high_current_weight\" value=\"%0.2f\"",magnetic_radial_high_current_weight);
		hmStream.SSPrintf(HtmlTagStreamMode, "/div");

		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 236px; left: 140px;\"");
		hmStream.SSPrintf(HtmlTagStreamMode, "input type=\"text\" size=\"2\" maxlength=\"6\" name=\"form_cosine_radial_low_current_weight\" value=\"%0.2f\"",cosine_radial_low_current_weight);
		hmStream.SSPrintf(HtmlTagStreamMode, "/div");

		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 236px; left: 200px;\"");
		hmStream.SSPrintf(HtmlTagStreamMode, "input type=\"text\" size=\"2\" maxlength=\"6\" name=\"form_cosine_radial_high_current_weight\" value=\"%0.2f\"",cosine_radial_high_current_weight);
		hmStream.SSPrintf(HtmlTagStreamMode, "/div");

		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 258px; left: 140px;\"");
		hmStream.SSPrintf(HtmlTagStreamMode, "input type=\"text\" size=\"2\" maxlength=\"6\" name=\"form_hibd_radial_low_current_weight\" value=\"%0.2f\"",hibd_radial_low_current_weight);
		hmStream.SSPrintf(HtmlTagStreamMode, "/div");

		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 258px; left: 200px;\"");
		hmStream.SSPrintf(HtmlTagStreamMode, "input type=\"text\" size=\"2\" maxlength=\"6\" name=\"form_hibd_radial_high_current_weight\" value=\"%0.2f\"",hibd_radial_high_current_weight);
		hmStream.SSPrintf(HtmlTagStreamMode, "/div");

		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 170px; left: 440px;\"");
		hmStream.SSPrintf(HtmlTagStreamMode, "input type=\"text\" size=\"2\" maxlength=\"6\" name=\"form_tomography_vertical_low_current_weight\" value=\"%0.2f\"",tomography_vertical_low_current_weight);
		hmStream.SSPrintf(HtmlTagStreamMode, "/div");

		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 170px; left: 500px;\"");
		hmStream.SSPrintf(HtmlTagStreamMode, "input type=\"text\" size=\"2\" maxlength=\"6\" name=\"form_tomography_vertical_high_current_weight\" value=\"%0.2f\"",tomography_vertical_high_current_weight);
		hmStream.SSPrintf(HtmlTagStreamMode, "/div");

		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 192px; left: 440px;\"");
		hmStream.SSPrintf(HtmlTagStreamMode, "input type=\"text\" size=\"2\" maxlength=\"6\" name=\"form_electric_vertical_low_current_weight\" value=\"%0.2f\"",electric_vertical_low_current_weight);
		hmStream.SSPrintf(HtmlTagStreamMode, "/div");

		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 192px; left: 500px;\"");
		hmStream.SSPrintf(HtmlTagStreamMode, "input type=\"text\" size=\"2\" maxlength=\"6\" name=\"form_electric_vertical_high_current_weight\" value=\"%0.2f\"",electric_vertical_high_current_weight);
		hmStream.SSPrintf(HtmlTagStreamMode, "/div");

		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 214px; left: 440px;\"");
		hmStream.SSPrintf(HtmlTagStreamMode, "input type=\"text\" size=\"2\" maxlength=\"6\" name=\"form_magnetic_vertical_low_current_weight\" value=\"%0.2f\"",magnetic_vertical_low_current_weight);
		hmStream.SSPrintf(HtmlTagStreamMode, "/div");

		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 214px; left: 500px;\"");
		hmStream.SSPrintf(HtmlTagStreamMode, "input type=\"text\" size=\"2\" maxlength=\"6\" name=\"form_magnetic_vertical_high_current_weight\" value=\"%0.2f\"",magnetic_vertical_high_current_weight);
		hmStream.SSPrintf(HtmlTagStreamMode, "/div");

		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 236px; left: 440px;\"");
		hmStream.SSPrintf(HtmlTagStreamMode, "input type=\"text\" size=\"2\" maxlength=\"6\" name=\"form_sine_vertical_low_current_weight\" value=\"%0.2f\"",sine_vertical_low_current_weight);
		hmStream.SSPrintf(HtmlTagStreamMode, "/div");

		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 236px; left: 500px;\"");
		hmStream.SSPrintf(HtmlTagStreamMode, "input type=\"text\" size=\"2\" maxlength=\"6\" name=\"form_sine_vertical_high_current_weight\" value=\"%0.2f\"",sine_vertical_high_current_weight);
		hmStream.SSPrintf(HtmlTagStreamMode, "/div");

		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 258px; left: 440px;\"");
		hmStream.SSPrintf(HtmlTagStreamMode, "input type=\"text\" size=\"2\" maxlength=\"6\" name=\"form_hibd_vertical_low_current_weight\" value=\"%0.2f\"",hibd_vertical_low_current_weight);
		hmStream.SSPrintf(HtmlTagStreamMode, "/div");

		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 258px; left: 500px;\"");
		hmStream.SSPrintf(HtmlTagStreamMode, "input type=\"text\" size=\"2\" maxlength=\"6\" name=\"form_hibd_vertical_high_current_weight\" value=\"%0.2f\"",hibd_vertical_high_current_weight);
		hmStream.SSPrintf(HtmlTagStreamMode, "/div");
	}
//	Control section
	hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 344px; left: 10px;\"><b>Power supplies:</b></div");
	hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 322px; left: 815px;\"><b>Magnetic probes:</b></div");
	hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 322px; left: 590px;\"><b>Discharge Settings:</b></div");

	hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 344px; left: 590px;\"");
	hmStream.Printf("Auto Breakdown");
	if (auto_breakdown) hmStream.SSPrintf(HtmlTagStreamMode, "input type=\"checkbox\" name=\"form_auto_breakdown_bool\" CHECKED"); 
	else hmStream.SSPrintf(HtmlTagStreamMode, "input type=\"checkbox\" name=\"form_auto_breakdown_bool\"");
	hmStream.SSPrintf(HtmlTagStreamMode, "/div");
		
	hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 366px; left: 590px;\">Number of cycles:</div");
	hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 388px; left: 590px;\">Discharge time:</div");

	hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 410px; left: 590px;\">MARTe cycle (&#181;s):</div");
	hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 432px; left: 590px;\">start-up delay (&#181;s):</div");

	hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 388px; left: 10px;\">Soft</div");
	hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 410px; left: 10px;\">Medium</div");
	hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 432px; left: 10px;\">Hard</div");
	hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 344px; left: 140px;\">Primary</div");
	hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 344px; left: 285px;\">Horizontal</div");
	hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 344px; left: 450px;\">Vertical</div");
	hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 366px; left: 105px;\">P</div");
	hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 366px; left: 155px;\">I</div");
	hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 366px; left: 205px;\">D</div");
	hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 366px; left: 265px;\">P</div");
	hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 366px; left: 315px;\">I</div");
	hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 366px; left: 365px;\">D</div");
	hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 366px; left: 425px;\">P</div");
	hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 366px; left: 475px;\">I</div");
	hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 366px; left: 525px;\">D</div");

	hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 344px; left: 840px;\">Angle</div");
	hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 344px; left: 890px;\">Calib.</div");
	hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 344px; left: 970px;\">Angle</div");
	hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 344px; left: 1020px;\">Calib.</div");
	hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 344px; left: 1100px;\">Angle</div");
	hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 344px; left: 1150px;\">Calib.</div");

//		magnetic probes checkboxs + number
	hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 366px; left: 815px;\"><b>1</b></div");
	hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 388px; left: 815px;\"><b>2</b></div");
	hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 410px; left: 815px;\"><b>3</b></div");
	hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 432px; left: 815px;\"><b>4</b></div");
	hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 366px; left: 945px;\"><b>5</b></div");
	hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 388px; left: 945px;\"><b>6</b></div");
	hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 410px; left: 945px;\"><b>7</b></div");
	hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 432px; left: 945px;\"><b>8</b></div");
	hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 366px; left: 1075px;\"><b>9</b></div");
	hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 388px; left: 1067px;\"><b>10</b></div");
	hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 410px; left: 1067px;\"><b>11</b></div");
	hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 432px; left: 1067px;\"><b>12</b></div");

	hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 366px; left: 820px;\"");
	if (magnetic_probes_bool_vector[0]) hmStream.SSPrintf(HtmlTagStreamMode, "input type=\"checkbox\" name=\"form_magnetic_probe_bool_0\" CHECKED"); 
	else hmStream.SSPrintf(HtmlTagStreamMode, "input type=\"checkbox\" name=\"form_magnetic_probe_bool_0\"");
	hmStream.SSPrintf(HtmlTagStreamMode, "/div");

	hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 388px; left: 820px;\"");
	if (magnetic_probes_bool_vector[1]) hmStream.SSPrintf(HtmlTagStreamMode, "input type=\"checkbox\" name=\"form_magnetic_probe_bool_1\" CHECKED"); 
	else hmStream.SSPrintf(HtmlTagStreamMode, "input type=\"checkbox\" name=\"form_magnetic_probe_bool_1\"");
	hmStream.SSPrintf(HtmlTagStreamMode, "/div");

	hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 410px; left: 820px;\"");
	if (magnetic_probes_bool_vector[2]) hmStream.SSPrintf(HtmlTagStreamMode, "input type=\"checkbox\" name=\"form_magnetic_probe_bool_2\" CHECKED"); 
	else hmStream.SSPrintf(HtmlTagStreamMode, "input type=\"checkbox\" name=\"form_magnetic_probe_bool_2\"");
	hmStream.SSPrintf(HtmlTagStreamMode, "/div");

	hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 432px; left: 820px;\"");
	if (magnetic_probes_bool_vector[3]) hmStream.SSPrintf(HtmlTagStreamMode, "input type=\"checkbox\" name=\"form_magnetic_probe_bool_3\" CHECKED"); 
	else hmStream.SSPrintf(HtmlTagStreamMode, "input type=\"checkbox\" name=\"form_magnetic_probe_bool_3\"");
	hmStream.SSPrintf(HtmlTagStreamMode, "/div");

	hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 366px; left: 950px;\"");
	if (magnetic_probes_bool_vector[4]) hmStream.SSPrintf(HtmlTagStreamMode, "input type=\"checkbox\" name=\"form_magnetic_probe_bool_4\" CHECKED"); 
	else hmStream.SSPrintf(HtmlTagStreamMode, "input type=\"checkbox\" name=\"form_magnetic_probe_bool_4\"");
	hmStream.SSPrintf(HtmlTagStreamMode, "/div");

	hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 388px; left: 950px;\"");
	if (magnetic_probes_bool_vector[5]) hmStream.SSPrintf(HtmlTagStreamMode, "input type=\"checkbox\" name=\"form_magnetic_probe_bool_5\" CHECKED"); 
	else hmStream.SSPrintf(HtmlTagStreamMode, "input type=\"checkbox\" name=\"form_magnetic_probe_bool_5\"");
	hmStream.SSPrintf(HtmlTagStreamMode, "/div");

	hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 410px; left: 950px;\"");
	if (magnetic_probes_bool_vector[6]) hmStream.SSPrintf(HtmlTagStreamMode, "input type=\"checkbox\" name=\"form_magnetic_probe_bool_6\" CHECKED"); 
	else hmStream.SSPrintf(HtmlTagStreamMode, "input type=\"checkbox\" name=\"form_magnetic_probe_bool_6\"");
	hmStream.SSPrintf(HtmlTagStreamMode, "/div");

	hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 432px; left: 950px;\"");
	if (magnetic_probes_bool_vector[7]) hmStream.SSPrintf(HtmlTagStreamMode, "input type=\"checkbox\" name=\"form_magnetic_probe_bool_7\" CHECKED"); 
	else hmStream.SSPrintf(HtmlTagStreamMode, "input type=\"checkbox\" name=\"form_magnetic_probe_bool_7\"");
	hmStream.SSPrintf(HtmlTagStreamMode, "/div");

	hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 366px; left: 1080px;\"");
	if (magnetic_probes_bool_vector[8]) hmStream.SSPrintf(HtmlTagStreamMode, "input type=\"checkbox\" name=\"form_magnetic_probe_bool_8\" CHECKED"); 
	else hmStream.SSPrintf(HtmlTagStreamMode, "input type=\"checkbox\" name=\"form_magnetic_probe_bool_8\"");
	hmStream.SSPrintf(HtmlTagStreamMode, "/div");

	hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 388px; left: 1080px;\"");
	if (magnetic_probes_bool_vector[9]) hmStream.SSPrintf(HtmlTagStreamMode, "input type=\"checkbox\" name=\"form_magnetic_probe_bool_9\" CHECKED"); 
	else hmStream.SSPrintf(HtmlTagStreamMode, "input type=\"checkbox\" name=\"form_magnetic_probe_bool_9\"");
	hmStream.SSPrintf(HtmlTagStreamMode, "/div");

	hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 410px; left: 1080px;\"");
	if (magnetic_probes_bool_vector[10]) hmStream.SSPrintf(HtmlTagStreamMode, "input type=\"checkbox\" name=\"form_magnetic_probe_bool_10\" CHECKED"); 
	else hmStream.SSPrintf(HtmlTagStreamMode, "input type=\"checkbox\" name=\"form_magnetic_probe_bool_10\"");
	hmStream.SSPrintf(HtmlTagStreamMode, "/div");

	hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 432px; left: 1080px;\"");
	if (magnetic_probes_bool_vector[11]) hmStream.SSPrintf(HtmlTagStreamMode, "input type=\"checkbox\" name=\"form_magnetic_probe_bool_11\" CHECKED"); 
	else hmStream.SSPrintf(HtmlTagStreamMode, "input type=\"checkbox\" name=\"form_magnetic_probe_bool_11\"");
	hmStream.SSPrintf(HtmlTagStreamMode, "/div");


	if(!edit_control)
	{
		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 297px; left: 220px;\"");
		hmStream.SSPrintf(HtmlTagStreamMode, "input type=\"submit\" name=\"submit_edit_control\" value=\"Edit parameters\"");
		hmStream.SSPrintf(HtmlTagStreamMode, "/div");

//		PID constants
		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 388px; left: 90px;\">%.2f</div",PID_primary_proportional_soft);
		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 388px; left: 140px;\">%.2f</div",PID_primary_integral_soft);
		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 388px; left: 190px;\">%.2f</div",PID_primary_derivative_soft);
		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 388px; left: 250px;\">%.2f</div",PID_horizontal_proportional_soft);
		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 388px; left: 300px;\">%.2f</div",PID_horizontal_integral_soft);
		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 388px; left: 350px;\">%.2f</div",PID_horizontal_derivative_soft);
		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 388px; left: 410px;\">%.2f</div",PID_vertical_proportional_soft);
		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 388px; left: 460px;\">%.2f</div",PID_vertical_integral_soft);
		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 388px; left: 510px;\">%.2f</div",PID_vertical_derivative_soft);

		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 410px; left: 90px;\">%.2f</div",PID_primary_proportional_normal);
		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 410px; left: 140px;\">%.2f</div",PID_primary_integral_normal);
		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 410px; left: 190px;\">%.2f</div",PID_primary_derivative_normal);
		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 410px; left: 250px;\">%.2f</div",PID_horizontal_proportional_normal);
		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 410px; left: 300px;\">%.2f</div",PID_horizontal_integral_normal);
		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 410px; left: 350px;\">%.2f</div",PID_horizontal_derivative_normal);
		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 410px; left: 410px;\">%.2f</div",PID_vertical_proportional_normal);
		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 410px; left: 460px;\">%.2f</div",PID_vertical_integral_normal);
		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 410px; left: 510px;\">%.2f</div",PID_vertical_derivative_normal);

		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 432px; left: 90px;\">%.2f</div",PID_primary_proportional_hard);
		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 432px; left: 140px;\">%.2f</div",PID_primary_integral_hard);
		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 432px; left: 190px;\">%.2f</div",PID_primary_derivative_hard);
		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 432px; left: 250px;\">%.2f</div",PID_horizontal_proportional_hard);
		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 432px; left: 300px;\">%.2f</div",PID_horizontal_integral_hard);
		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 432px; left: 350px;\">%.2f</div",PID_horizontal_derivative_hard);
		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 432px; left: 410px;\">%.2f</div",PID_vertical_proportional_hard);
		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 432px; left: 460px;\">%.2f</div",PID_vertical_integral_hard);
		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 432px; left: 510px;\">%.2f</div",PID_vertical_derivative_hard);


		//discharge settings
		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 366px; left: 720px;\">%d</div",number_of_cycles);
		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 388px; left: 720px;\">%.1f ms</div",discharge_time);
		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 410px; left: 720px;\">%d &#181;s</div",marte_usec_cycle_time);
		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 432px; left: 720px;\">%d &#181;s</div",usectime_to_wait_for_starting_operation);


//		magnetic probes angles and calibration factors
		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 366px; left: 840px;\">%.1f</div",magnetic_Angles[0]);
		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 366px; left: 890px;\">%.1f</div",magnetic_Calibration[0]);
		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 388px; left: 840px;\">%.1f</div",magnetic_Angles[1]);
		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 388px; left: 890px;\">%.1f</div",magnetic_Calibration[1]);
		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 410px; left: 840px;\">%.1f</div",magnetic_Angles[2]);
		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 410px; left: 890px;\">%.1f</div",magnetic_Calibration[2]);
		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 432px; left: 840px;\">%.1f</div",magnetic_Angles[3]);
		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 432px; left: 890px;\">%.1f</div",magnetic_Calibration[3]);
		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 366px; left: 970px;\">%.1f</div",magnetic_Angles[4]);
		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 366px; left: 1020px;\">%.1f</div",magnetic_Calibration[4]);
		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 388px; left: 970px;\">%.1f</div",magnetic_Angles[5]);
		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 388px; left: 1020px;\">%.1f</div",magnetic_Calibration[5]);
		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 410px; left: 970px;\">%.1f</div",magnetic_Angles[6]);
		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 410px; left: 1020px;\">%.1f</div",magnetic_Calibration[6]);
		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 432px; left: 970px;\">%.1f</div",magnetic_Angles[7]);
		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 432px; left: 1020px;\">%.1f</div",magnetic_Calibration[7]);
		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 366px; left: 1100px;\">%.1f</div",magnetic_Angles[8]);
		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 366px; left: 1150px;\">%.1f</div",magnetic_Calibration[8]);
		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 388px; left: 1100px;\">%.1f</div",magnetic_Angles[9]);
		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 388px; left: 1150px;\">%.1f</div",magnetic_Calibration[9]);
		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 410px; left: 1100px;\">%.1f</div",magnetic_Angles[10]);
		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 410px; left: 1150px;\">%.1f</div",magnetic_Calibration[10]);
		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 432px; left: 1100px;\">%.1f</div",magnetic_Angles[11]);
		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 432px; left: 1150px;\">%.1f</div",magnetic_Calibration[11]);

	}
	else
	{
		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 297px; left: 220px;\"");
		hmStream.SSPrintf(HtmlTagStreamMode, "input type=\"submit\" name=\"submit_save_control\" value=\"Save parameters\"");
		hmStream.SSPrintf(HtmlTagStreamMode, "/div");

//		control edit boxes
//		PID constants
		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 388px; left: 90px;\"><input type=\"text\" size=\"2\" maxlength=\"6\" name=\"form_PID_primary_proportional_soft\" value=\"%0.1f\"></div",PID_primary_proportional_soft);
		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 388px; left: 140px;\"><input type=\"text\" size=\"2\" maxlength=\"6\" name=\"form_PID_primary_integral_soft\" value=\"%0.1f\"></div",PID_primary_integral_soft);
		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 388px; left: 190px;\"><input type=\"text\" size=\"2\" maxlength=\"6\" name=\"form_PID_primary_derivative_soft\" value=\"%0.1f\"></div",PID_primary_derivative_soft);
		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 388px; left: 250px;\"><input type=\"text\" size=\"2\" maxlength=\"6\" name=\"form_PID_horizontal_proportional_soft\" value=\"%0.1f\"></div",PID_horizontal_proportional_soft);
		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 388px; left: 300px;\"><input type=\"text\" size=\"2\" maxlength=\"6\" name=\"form_PID_horizontal_integral_soft\" value=\"%0.1f\"></div",PID_horizontal_integral_soft);
		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 388px; left: 350px;\"><input type=\"text\" size=\"2\" maxlength=\"6\" name=\"form_PID_horizontal_derivative_soft\" value=\"%0.1f\"></div",PID_horizontal_derivative_soft);
		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 388px; left: 410px;\"><input type=\"text\" size=\"2\" maxlength=\"6\" name=\"form_PID_vertical_proportional_soft\" value=\"%0.1f\"></div",PID_vertical_proportional_soft);
		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 388px; left: 460px;\"><input type=\"text\" size=\"2\" maxlength=\"6\" name=\"form_PID_vertical_integral_soft\" value=\"%0.1f\"></div",PID_vertical_integral_soft);
		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 388px; left: 510px;\"><input type=\"text\" size=\"2\" maxlength=\"6\" name=\"form_PID_vertical_derivative_soft\" value=\"%0.1f\"></div",PID_vertical_derivative_soft);

		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 410px; left: 90px;\"><input type=\"text\" size=\"2\" maxlength=\"6\" name=\"form_PID_primary_proportional_normal\" value=\"%0.1f\"></div",PID_primary_proportional_normal);
		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 410px; left: 140px;\"><input type=\"text\" size=\"2\" maxlength=\"6\" name=\"form_PID_primary_integral_normal\" value=\"%0.1f\"></div",PID_primary_integral_normal);
		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 410px; left: 190px;\"><input type=\"text\" size=\"2\" maxlength=\"6\" name=\"form_PID_primary_derivative_normal\" value=\"%0.1f\"></div",PID_primary_derivative_normal);
		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 410px; left: 250px;\"><input type=\"text\" size=\"2\" maxlength=\"6\" name=\"form_PID_horizontal_proportional_normal\" value=\"%0.2f\"></div",PID_horizontal_proportional_normal);
		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 410px; left: 300px;\"><input type=\"text\" size=\"2\" maxlength=\"6\" name=\"form_PID_horizontal_integral_normal\" value=\"%0.1f\"></div",PID_horizontal_integral_normal);
		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 410px; left: 350px;\"><input type=\"text\" size=\"2\" maxlength=\"6\" name=\"form_PID_horizontal_derivative_normal\" value=\"%0.1f\"></div",PID_horizontal_derivative_normal);
		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 410px; left: 410px;\"><input type=\"text\" size=\"2\" maxlength=\"6\" name=\"form_PID_vertical_proportional_normal\" value=\"%0.1f\"></div",PID_vertical_proportional_normal);
		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 410px; left: 460px;\"><input type=\"text\" size=\"2\" maxlength=\"6\" name=\"form_PID_vertical_integral_normal\" value=\"%0.1f\"></div",PID_vertical_integral_normal);
		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 410px; left: 510px;\"><input type=\"text\" size=\"2\" maxlength=\"6\" name=\"form_PID_vertical_derivative_normal\" value=\"%0.1f\"></div",PID_vertical_derivative_normal);

		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 432px; left: 90px;\"><input type=\"text\" size=\"2\" maxlength=\"6\" name=\"form_PID_primary_proportional_hard\" value=\"%0.1f\"></div",PID_primary_proportional_hard);
		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 432px; left: 140px;\"><input type=\"text\" size=\"2\" maxlength=\"6\" name=\"form_PID_primary_integral_hard\" value=\"%0.1f\"></div",PID_primary_integral_hard);
		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 432px; left: 190px;\"><input type=\"text\" size=\"2\" maxlength=\"6\" name=\"form_PID_primary_derivative_hard\" value=\"%0.1f\"></div",PID_primary_derivative_hard);
		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 432px; left: 250px;\"><input type=\"text\" size=\"2\" maxlength=\"6\" name=\"form_PID_horizontal_proportional_hard\" value=\"%0.1f\"></div",PID_horizontal_proportional_hard);
		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 432px; left: 300px;\"><input type=\"text\" size=\"2\" maxlength=\"6\" name=\"form_PID_horizontal_integral_hard\" value=\"%0.1f\"></div",PID_horizontal_integral_hard);
		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 432px; left: 350px;\"><input type=\"text\" size=\"2\" maxlength=\"6\" name=\"form_PID_horizontal_derivative_hard\" value=\"%0.1f\"></div",PID_horizontal_derivative_hard);
		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 432px; left: 410px;\"><input type=\"text\" size=\"2\" maxlength=\"6\" name=\"form_PID_vertical_proportional_hard\" value=\"%0.1f\"></div",PID_vertical_proportional_hard);
		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 432px; left: 460px;\"><input type=\"text\" size=\"2\" maxlength=\"6\" name=\"form_PID_vertical_integral_hard\" value=\"%0.1f\"></div",PID_vertical_integral_hard);
		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 432px; left: 510px;\"><input type=\"text\" size=\"2\" maxlength=\"6\" name=\"form_PID_vertical_derivative_hard\" value=\"%0.1f\"></div",PID_vertical_derivative_hard);

		//discharge settings
		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 366px; left: 720px;\"><input type=\"text\" size=\"1\" maxlength=\"3\" name=\"form_number_of_cycles\" value=\"%d\"></div",number_of_cycles);
		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 388px; left: 720px;\"><input type=\"text\" size=\"2\" maxlength=\"6\" name=\"form_discharge_time\" value=\"%0.1f\"></div",discharge_time);
		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 410px; left: 720px;\"><input type=\"text\" size=\"3\" maxlength=\"7\" name=\"form_marte_usec_cycle_time\" value=\"%d\"></div",marte_usec_cycle_time);
		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 432px; left: 720px;\"><input type=\"text\" size=\"3\" maxlength=\"7\" name=\"form_usectime_to_wait_for_starting_operation\" value=\"%d\"></div",usectime_to_wait_for_starting_operation);


//		magnetic probes angles and calibration factors
		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 366px; left: 840px;\"><input type=\"text\" size=\"2\" maxlength=\"6\" name=\"form_magnetic_Angles_0\" value=\"%0.1f\"></div",magnetic_Angles[0]);
		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 366px; left: 890px;\"><input type=\"text\" size=\"2\" maxlength=\"6\" name=\"form_magnetic_Calibration_0\" value=\"%0.1f\"></div",magnetic_Calibration[0]);
		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 388px; left: 840px;\"><input type=\"text\" size=\"2\" maxlength=\"6\" name=\"form_magnetic_Angles_1\" value=\"%0.1f\"></div",magnetic_Angles[1]);
		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 388px; left: 890px;\"><input type=\"text\" size=\"2\" maxlength=\"6\" name=\"form_magnetic_Calibration_1\" value=\"%0.1f\"></div",magnetic_Calibration[1]);
		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 410px; left: 840px;\"><input type=\"text\" size=\"2\" maxlength=\"6\" name=\"form_magnetic_Angles_2\" value=\"%0.1f\"></div",magnetic_Angles[2]);
		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 410px; left: 890px;\"><input type=\"text\" size=\"2\" maxlength=\"6\" name=\"form_magnetic_Calibration_2\" value=\"%0.1f\"></div",magnetic_Calibration[2]);
		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 432px; left: 840px;\"><input type=\"text\" size=\"2\" maxlength=\"6\" name=\"form_magnetic_Angles_3\" value=\"%0.1f\"></div",magnetic_Angles[3]);
		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 432px; left: 890px;\"><input type=\"text\" size=\"2\" maxlength=\"6\" name=\"form_magnetic_Calibration_3\" value=\"%0.1f\"></div",magnetic_Calibration[3]);
		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 366px; left: 970px;\"><input type=\"text\" size=\"2\" maxlength=\"6\" name=\"form_magnetic_Angles_4\" value=\"%0.1f\"></div",magnetic_Angles[4]);
		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 366px; left: 1020px;\"><input type=\"text\" size=\"2\" maxlength=\"6\" name=\"form_magnetic_Calibration_4\" value=\"%0.1f\"></div",magnetic_Calibration[4]);
		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 388px; left: 970px;\"><input type=\"text\" size=\"2\" maxlength=\"6\" name=\"form_magnetic_Angles_5\" value=\"%0.1f\"></div",magnetic_Angles[5]);
		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 388px; left: 1020px;\"><input type=\"text\" size=\"2\" maxlength=\"6\" name=\"form_magnetic_Calibration_5\" value=\"%0.1f\"></div",magnetic_Calibration[5]);
		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 410px; left: 970px;\"><input type=\"text\" size=\"2\" maxlength=\"6\" name=\"form_magnetic_Angles_6\" value=\"%0.1f\"></div",magnetic_Angles[6]);
		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 410px; left: 1020px;\"><input type=\"text\" size=\"2\" maxlength=\"6\" name=\"form_magnetic_Calibration_6\" value=\"%0.1f\"></div",magnetic_Calibration[6]);
		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 432px; left: 970px;\"><input type=\"text\" size=\"2\" maxlength=\"6\" name=\"form_magnetic_Angles_7\" value=\"%0.1f\"></div",magnetic_Angles[7]);
		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 432px; left: 1020px;\"><input type=\"text\" size=\"2\" maxlength=\"6\" name=\"form_magnetic_Calibration_7\" value=\"%0.1f\"></div",magnetic_Calibration[7]);
		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 366px; left: 1100px;\"><input type=\"text\" size=\"2\" maxlength=\"6\" name=\"form_magnetic_Angles_8\" value=\"%0.1f\"></div",magnetic_Angles[8]);
		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 366px; left: 1150px;\"><input type=\"text\" size=\"2\" maxlength=\"6\" name=\"form_magnetic_Calibration_8\" value=\"%0.1f\"></div",magnetic_Calibration[8]);
		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 388px; left: 1100px;\"><input type=\"text\" size=\"2\" maxlength=\"6\" name=\"form_magnetic_Angles_9\" value=\"%0.1f\"></div",magnetic_Angles[9]);
		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 388px; left: 1150px;\"><input type=\"text\" size=\"2\" maxlength=\"6\" name=\"form_magnetic_Calibration_9\" value=\"%0.1f\"></div",magnetic_Calibration[9]);
		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 410px; left: 1100px;\"><input type=\"text\" size=\"2\" maxlength=\"6\" name=\"form_magnetic_Angles_10\" value=\"%0.1f\"></div",magnetic_Angles[10]);
		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 410px; left: 1150px;\"><input type=\"text\" size=\"2\" maxlength=\"6\" name=\"form_magnetic_Calibration_10\" value=\"%0.1f\"></div",magnetic_Calibration[10]);
		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 432px; left: 1100px;\"><input type=\"text\" size=\"2\" maxlength=\"6\" name=\"form_magnetic_Angles_11\" value=\"%0.1f\"></div",magnetic_Angles[11]);
		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 432px; left: 1150px;\"><input type=\"text\" size=\"2\" maxlength=\"6\" name=\"form_magnetic_Calibration_11\" value=\"%0.1f\"></div",magnetic_Calibration[11]);

	}

//	TIME WINDOWS SECTION
	hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 522px; left: 10px;\"><b>Positive time-windows:</b></div");
	hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 722px; left: 10px;\"><b>Negative time-windows:</b></div");

	hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 562px; left: 10px;\"><b>T1 =</b></div");
	hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 584px; left: 10px;\"><b>T2 =</b></div");
	hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 606px; left: 10px;\"><b>T3 =</b></div");
	hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 628px; left: 10px;\"><b>T4 =</b></div");
	hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 650px; left: 10px;\"><b>T5 =</b></div");
	hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 672px; left: 10px;\"><b>T6 =</b></div");
	hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 694px; left: 10px;\"><b>T7 =</b></div");

	hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 762px; left: 10px;\"><b>T1 =</b></div");
	hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 784px; left: 10px;\"><b>T2 =</b></div");
	hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 806px; left: 10px;\"><b>T3 =</b></div");
	hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 828px; left: 10px;\"><b>T4 =</b></div");
	hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 850px; left: 10px;\"><b>T5 =</b></div");
	hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 872px; left: 10px;\"><b>T6 =</b></div");
	hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 894px; left: 10px;\"><b>T7 =</b></div");
	
	hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 542px; left: 135px;\"><b>Primary</b></div");
	hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 542px; left: 275px;\"><b>Vertical</b></div");
	hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 542px; left: 400px;\"><b>Horizontal</b></div");
	
	hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 742px; left: 135px;\"><b>Primary</b></div");
	hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 742px; left: 275px;\"><b>Vertical</b></div");
	hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 742px; left: 400px;\"><b>Horizontal</b></div");
	

	hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 507px; left: 270px;\"><b>First cycle:</b");
	if (first_cycle_positive_bool) { 
		hmStream.SSPrintf(HtmlTagStreamMode, "input type=\"radio\" name=\"form_first_cycle_positive_bool\" value=\"1\" CHECKED /><text>positive</text");
		hmStream.SSPrintf(HtmlTagStreamMode, "input type=\"radio\" name=\"form_first_cycle_positive_bool\" value=\"0\" /><text>negative</text");
	}
	else{
		hmStream.SSPrintf(HtmlTagStreamMode, "input type=\"radio\" name=\"form_first_cycle_positive_bool\" value=\"1\" /><text>positive</text");
		hmStream.SSPrintf(HtmlTagStreamMode, "input type=\"radio\" name=\"form_first_cycle_positive_bool\" value=\"0\" CHECKED /><text>negative</text");
	}
	hmStream.SSPrintf(HtmlTagStreamMode, "/div");


	
	if (!edit_timewindows){
		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 467px; left: 180px;\"");
		hmStream.SSPrintf(HtmlTagStreamMode, "input type=\"submit\" name=\"submit_edit_timewindows\" value=\"Edit parameters\"");
		hmStream.SSPrintf(HtmlTagStreamMode, "/div");
	
		if (!timewindows_bool_vector[0]) hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 562px; left: 50px;\">------</div");
		else hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 562px; left: 50px;\">%.2f</div",timewindows_time[0]);
		if (!timewindows_bool_vector[1]) hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 584px; left: 50px;\">------</div");
		else hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 584px; left: 50px;\">%.2f</div",timewindows_time[1]);
		if (!timewindows_bool_vector[2]) hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 606px; left: 50px;\">------</div");
		else hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 606px; left: 50px;\">%.2f</div",timewindows_time[2]);
		if (!timewindows_bool_vector[3]) hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 628px; left: 50px;\">------</div");
		else hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 628px; left: 50px;\">%.2f</div",timewindows_time[3]);
		if (!timewindows_bool_vector[4]) hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 650px; left: 50px;\">------</div");
		else hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 650px; left: 50px;\">%.2f</div",timewindows_time[4]);
		if (!timewindows_bool_vector[5]) hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 672px; left: 50px;\">------</div");
		else hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 672px; left: 50px;\">%.2f</div",timewindows_time[5]);
		if (!timewindows_bool_vector[6]) hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 694px; left: 50px;\">------</div");
		else hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 694px; left: 50px;\">%.2f</div",timewindows_time[6]);
		if (!timewindows_bool_vector[7]) hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 762px; left: 50px;\">------</div");
		else hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 762px; left: 50px;\">%.2f</div",timewindows_time[7]);
		if (!timewindows_bool_vector[8]) hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 784px; left: 50px;\">------</div");
		else hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 784px; left: 50px;\">%.2f</div",timewindows_time[8]);
		if (!timewindows_bool_vector[9]) hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 806px; left: 50px;\">------</div");
		else hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 806px; left: 50px;\">%.2f</div",timewindows_time[9]);
		if (!timewindows_bool_vector[10]) hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 828px; left: 50px;\">------</div");
		else hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 828px; left: 50px;\">%.2f</div",timewindows_time[10]);
		if (!timewindows_bool_vector[11]) hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 850px; left: 50px;\">------</div");
		else hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 850px; left: 50px;\">%.2f</div",timewindows_time[11]);
		if (!timewindows_bool_vector[12]) hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 872px; left: 50px;\">------</div");
		else hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 872px; left: 50px;\">%.2f</div",timewindows_time[12]);
		if (!timewindows_bool_vector[13]) hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 894px; left: 50px;\">------</div");
		else hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 894px; left: 50px;\">%.2f</div",timewindows_time[13]);

		 
		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 562px; left: 94px;\"");
		if (!timewindows_bool_vector[0]) 	  hmStream.Printf("    --------------------");
			else if (timewindows_dropdown_vector[0] == 0) hmStream.Printf("             off");
			else if (timewindows_dropdown_vector[0] == 1) hmStream.Printf("     current control");
			else if (timewindows_dropdown_vector[0] == 2) hmStream.Printf(" Ip control-Soft PID");
			else if (timewindows_dropdown_vector[0] == 3) hmStream.Printf("Ip control-Mean PID");
			else if (timewindows_dropdown_vector[0] == 4) hmStream.Printf("Ip control-Hard PID");
			else if (timewindows_dropdown_vector[0] == 5) hmStream.Printf("Ip control-Auto PID");
			else if (timewindows_dropdown_vector[0] == 6) hmStream.Printf("  Integrated control*");
			else if (timewindows_dropdown_vector[0] == 7) hmStream.Printf("           Invert*");
		hmStream.SSPrintf(HtmlTagStreamMode, "/div");

		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 562px; left: 236px;\"");
			if (!timewindows_bool_vector[0]) 	  hmStream.Printf("    --------------------");
			else if (timewindows_dropdown_vector[1] == 0) hmStream.Printf("             off");
			else if (timewindows_dropdown_vector[1] == 1) hmStream.Printf("     current control");
			else if (timewindows_dropdown_vector[1] == 2) hmStream.Printf("  position-Soft PID");
			else if (timewindows_dropdown_vector[1] == 3) hmStream.Printf(" position-Med. PID");
			else if (timewindows_dropdown_vector[1] == 4) hmStream.Printf(" position-Hard PID");
			else if (timewindows_dropdown_vector[1] == 5) hmStream.Printf(" position-Auto PID");
			else if (timewindows_dropdown_vector[1] == 6) hmStream.Printf(" Integrated control*");
			else if (timewindows_dropdown_vector[1] == 7) hmStream.Printf("           Invert*");
		hmStream.SSPrintf(HtmlTagStreamMode, "/div");

		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 562px; left: 370px;\"");
			if (!timewindows_bool_vector[0]) 	  hmStream.Printf("    --------------------");
			else if (timewindows_dropdown_vector[2] == 0) hmStream.Printf("             off");
			else if (timewindows_dropdown_vector[2] == 1) hmStream.Printf("     current control");
			else if (timewindows_dropdown_vector[2] == 2) hmStream.Printf("  position-Soft PID");
			else if (timewindows_dropdown_vector[2] == 3) hmStream.Printf(" position-Med. PID");
			else if (timewindows_dropdown_vector[2] == 4) hmStream.Printf(" position-Hard PID");
			else if (timewindows_dropdown_vector[2] == 5) hmStream.Printf(" position-Auto PID");
			else if (timewindows_dropdown_vector[2] == 6) hmStream.Printf(" Integrated control*");
			else if (timewindows_dropdown_vector[2] == 7) hmStream.Printf("           Invert*");
		hmStream.SSPrintf(HtmlTagStreamMode, "/div");

		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 584px; left: 94px;\"");
			if (!timewindows_bool_vector[1]) 	  hmStream.Printf("    --------------------");
			else if (timewindows_dropdown_vector[3] == 0) hmStream.Printf("             off");
			else if (timewindows_dropdown_vector[3] == 1) hmStream.Printf("     current control");
			else if (timewindows_dropdown_vector[3] == 2) hmStream.Printf(" Ip control-Soft PID");
			else if (timewindows_dropdown_vector[3] == 3) hmStream.Printf("Ip control-Med. PID");
			else if (timewindows_dropdown_vector[3] == 4) hmStream.Printf("Ip control-Hard PID");
			else if (timewindows_dropdown_vector[3] == 5) hmStream.Printf("Ip control-Auto PID");
			else if (timewindows_dropdown_vector[3] == 6) hmStream.Printf(" Integrated control*");
			else if (timewindows_dropdown_vector[3] == 7) hmStream.Printf("           Invert*");
		hmStream.SSPrintf(HtmlTagStreamMode, "/div");

		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 584px; left: 236px;\"");
			if (!timewindows_bool_vector[1]) 	  hmStream.Printf("    --------------------");
			else if (timewindows_dropdown_vector[4] == 0) hmStream.Printf("             off");
			else if (timewindows_dropdown_vector[4] == 1) hmStream.Printf("     current control");
			else if (timewindows_dropdown_vector[4] == 2) hmStream.Printf("  position-Soft PID");
			else if (timewindows_dropdown_vector[4] == 3) hmStream.Printf(" position-Med. PID");
			else if (timewindows_dropdown_vector[4] == 4) hmStream.Printf(" position-Hard PID");
			else if (timewindows_dropdown_vector[4] == 5) hmStream.Printf(" position-Auto PID");
			else if (timewindows_dropdown_vector[4] == 6) hmStream.Printf(" Integrated control*");
			else if (timewindows_dropdown_vector[4] == 7) hmStream.Printf("           Invert*");
		hmStream.SSPrintf(HtmlTagStreamMode, "/div");

		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 584px; left: 370px;\"");
			if (!timewindows_bool_vector[1]) 	  hmStream.Printf("    --------------------");
			else if (timewindows_dropdown_vector[5] == 0) hmStream.Printf("             off");
			else if (timewindows_dropdown_vector[5] == 1) hmStream.Printf("     current control");
			else if (timewindows_dropdown_vector[5] == 2) hmStream.Printf("  position-Soft PID");
			else if (timewindows_dropdown_vector[5] == 3) hmStream.Printf(" position-Med. PID");
			else if (timewindows_dropdown_vector[5] == 4) hmStream.Printf(" position-Hard PID");
			else if (timewindows_dropdown_vector[5] == 5) hmStream.Printf(" position-Auto PID");
			else if (timewindows_dropdown_vector[5] == 6) hmStream.Printf(" Integrated control*");
			else if (timewindows_dropdown_vector[5] == 7) hmStream.Printf("           Invert*");
		hmStream.SSPrintf(HtmlTagStreamMode, "/div");

		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 606px; left: 94px;\"");
			if (!timewindows_bool_vector[2]) 	  hmStream.Printf("    --------------------");
			else if (timewindows_dropdown_vector[6] == 0) hmStream.Printf("             off");
			else if (timewindows_dropdown_vector[6] == 1) hmStream.Printf("     current control");
			else if (timewindows_dropdown_vector[6] == 2) hmStream.Printf(" Ip control-Soft PID");
			else if (timewindows_dropdown_vector[6] == 3) hmStream.Printf("Ip control-Med. PID");
			else if (timewindows_dropdown_vector[6] == 4) hmStream.Printf("Ip control-Hard PID");
			else if (timewindows_dropdown_vector[6] == 5) hmStream.Printf("Ip control-Auto PID");
			else if (timewindows_dropdown_vector[6] == 6) hmStream.Printf(" Integrated control*");
			else if (timewindows_dropdown_vector[6] == 7) hmStream.Printf("           Invert*");
		hmStream.SSPrintf(HtmlTagStreamMode, "/div");

		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 606px; left: 236px;\"");
			if (!timewindows_bool_vector[2]) 	  hmStream.Printf("    --------------------");
			else if (timewindows_dropdown_vector[7] == 0) hmStream.Printf("             off");
			else if (timewindows_dropdown_vector[7] == 1) hmStream.Printf("     current control");
			else if (timewindows_dropdown_vector[7] == 2) hmStream.Printf("  position-Soft PID");
			else if (timewindows_dropdown_vector[7] == 3) hmStream.Printf(" position-Med. PID");
			else if (timewindows_dropdown_vector[7] == 4) hmStream.Printf(" position-Hard PID");
			else if (timewindows_dropdown_vector[7] == 5) hmStream.Printf(" position-Auto PID");
			else if (timewindows_dropdown_vector[7] == 6) hmStream.Printf(" Integrated control*");
			else if (timewindows_dropdown_vector[7] == 7) hmStream.Printf("           Invert*");
		hmStream.SSPrintf(HtmlTagStreamMode, "/div");

		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 606px; left: 370px;\"");
			if (!timewindows_bool_vector[2]) 	  hmStream.Printf("    --------------------");
			else if (timewindows_dropdown_vector[8] == 0) hmStream.Printf("             off");
			else if (timewindows_dropdown_vector[8] == 1) hmStream.Printf("     current control");
			else if (timewindows_dropdown_vector[8] == 2) hmStream.Printf("  position-Soft PID");
			else if (timewindows_dropdown_vector[8] == 3) hmStream.Printf(" position-Med. PID");
			else if (timewindows_dropdown_vector[8] == 4) hmStream.Printf(" position-Hard PID");
			else if (timewindows_dropdown_vector[8] == 5) hmStream.Printf(" position-Auto PID");
			else if (timewindows_dropdown_vector[8] == 6) hmStream.Printf(" Integrated control*");
			else if (timewindows_dropdown_vector[8] == 7) hmStream.Printf("           Invert*");
		hmStream.SSPrintf(HtmlTagStreamMode, "/div");
	
		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 628px; left: 94px;\""); 
			if (!timewindows_bool_vector[3]) 	  hmStream.Printf("    --------------------");
			else if (timewindows_dropdown_vector[9] == 0) hmStream.Printf("             off");
			else if (timewindows_dropdown_vector[9] == 1) hmStream.Printf("     current control");
			else if (timewindows_dropdown_vector[9] == 2) hmStream.Printf(" Ip control-Soft PID");
			else if (timewindows_dropdown_vector[9] == 3) hmStream.Printf("Ip control-Med. PID");
			else if (timewindows_dropdown_vector[9] == 4) hmStream.Printf("Ip control-Hard PID");
			else if (timewindows_dropdown_vector[9] == 5) hmStream.Printf("Ip control-Auto PID");
			else if (timewindows_dropdown_vector[9] == 6) hmStream.Printf(" Integrated control*");
			else if (timewindows_dropdown_vector[9] == 7) hmStream.Printf("           Invert*");
		hmStream.SSPrintf(HtmlTagStreamMode, "/div");

		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 628px; left: 236px;\"");
			if (!timewindows_bool_vector[3]) 	  hmStream.Printf("    --------------------");
			else if (timewindows_dropdown_vector[10] == 0) hmStream.Printf("             off");
			else if (timewindows_dropdown_vector[10] == 1) hmStream.Printf("     current control");
			else if (timewindows_dropdown_vector[10] == 2) hmStream.Printf("  position-Soft PID");
			else if (timewindows_dropdown_vector[10] == 3) hmStream.Printf(" position-Med. PID");
			else if (timewindows_dropdown_vector[10] == 4) hmStream.Printf(" position-Hard PID");
			else if (timewindows_dropdown_vector[10] == 5) hmStream.Printf(" position-Auto PID");
			else if (timewindows_dropdown_vector[10] == 6) hmStream.Printf(" Integrated control*");
			else if (timewindows_dropdown_vector[10] == 7) hmStream.Printf("           Invert*");
		hmStream.SSPrintf(HtmlTagStreamMode, "/div");

		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 628px; left: 370px;\"");
			if (!timewindows_bool_vector[3]) 	  hmStream.Printf("    --------------------");
			else if (timewindows_dropdown_vector[11] == 0) hmStream.Printf("             off");
			else if (timewindows_dropdown_vector[11] == 1) hmStream.Printf("     current control");
			else if (timewindows_dropdown_vector[11] == 2) hmStream.Printf("  position-Soft PID");
			else if (timewindows_dropdown_vector[11] == 3) hmStream.Printf(" position-Med. PID");
			else if (timewindows_dropdown_vector[11] == 4) hmStream.Printf(" position-Hard PID");
			else if (timewindows_dropdown_vector[11] == 5) hmStream.Printf(" position-Auto PID");
			else if (timewindows_dropdown_vector[11] == 6) hmStream.Printf(" Integrated control*");
			else if (timewindows_dropdown_vector[11] == 7) hmStream.Printf("           Invert*");
		hmStream.SSPrintf(HtmlTagStreamMode, "/div");

		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 650px; left: 94px;\""); 
			if (!timewindows_bool_vector[4]) 	  hmStream.Printf("    --------------------");
			else if (timewindows_dropdown_vector[12] == 0) hmStream.Printf("             off");
			else if (timewindows_dropdown_vector[12] == 1) hmStream.Printf("     current control");
			else if (timewindows_dropdown_vector[12] == 2) hmStream.Printf(" Ip control-Soft PID");
			else if (timewindows_dropdown_vector[12] == 3) hmStream.Printf("Ip control-Med. PID");
			else if (timewindows_dropdown_vector[12] == 4) hmStream.Printf("Ip control-Hard PID");
			else if (timewindows_dropdown_vector[12] == 5) hmStream.Printf("Ip control-Auto PID");
			else if (timewindows_dropdown_vector[12] == 6) hmStream.Printf(" Integrated control*");
			else if (timewindows_dropdown_vector[12] == 7) hmStream.Printf("           Invert*");
		hmStream.SSPrintf(HtmlTagStreamMode, "/div");

		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 650px; left: 236px;\"");
			if (!timewindows_bool_vector[4]) 	  hmStream.Printf("    --------------------");
			else if (timewindows_dropdown_vector[13] == 0) hmStream.Printf("             off");
			else if (timewindows_dropdown_vector[13] == 1) hmStream.Printf("     current control");
			else if (timewindows_dropdown_vector[13] == 2) hmStream.Printf("  position-Soft PID");
			else if (timewindows_dropdown_vector[13] == 3) hmStream.Printf(" position-Med. PID");
			else if (timewindows_dropdown_vector[13] == 4) hmStream.Printf(" position-Hard PID");
			else if (timewindows_dropdown_vector[13] == 5) hmStream.Printf(" position-Auto PID");
			else if (timewindows_dropdown_vector[13] == 6) hmStream.Printf(" Integrated control*");
			else if (timewindows_dropdown_vector[13] == 7) hmStream.Printf("           Invert*");
		hmStream.SSPrintf(HtmlTagStreamMode, "/div");

		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 650px; left: 370px;\"");
			if (!timewindows_bool_vector[4]) 	  hmStream.Printf("    --------------------");
			else if (timewindows_dropdown_vector[14] == 0) hmStream.Printf("             off");
			else if (timewindows_dropdown_vector[14] == 1) hmStream.Printf("     current control");
			else if (timewindows_dropdown_vector[14] == 2) hmStream.Printf("  position-Soft PID");
			else if (timewindows_dropdown_vector[14] == 3) hmStream.Printf(" position-Med. PID");
			else if (timewindows_dropdown_vector[14] == 4) hmStream.Printf(" position-Hard PID");
			else if (timewindows_dropdown_vector[14] == 5) hmStream.Printf(" position-Auto PID");
			else if (timewindows_dropdown_vector[14] == 6) hmStream.Printf(" Integrated control*");
			else if (timewindows_dropdown_vector[14] == 7) hmStream.Printf("           Invert*");
		hmStream.SSPrintf(HtmlTagStreamMode, "/div");

		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 672px; left: 94px;\""); 
			if (!timewindows_bool_vector[5]) 	  hmStream.Printf("    --------------------");
			else if (timewindows_dropdown_vector[15] == 0) hmStream.Printf("             off");
			else if (timewindows_dropdown_vector[15] == 1) hmStream.Printf("     current control");
			else if (timewindows_dropdown_vector[15] == 2) hmStream.Printf(" Ip control-Soft PID");
			else if (timewindows_dropdown_vector[15] == 3) hmStream.Printf("Ip control-Med. PID");
			else if (timewindows_dropdown_vector[15] == 4) hmStream.Printf("Ip control-Hard PID");
			else if (timewindows_dropdown_vector[15] == 5) hmStream.Printf("Ip control-Auto PID");
			else if (timewindows_dropdown_vector[15] == 6) hmStream.Printf(" Integrated control*");
			else if (timewindows_dropdown_vector[15] == 7) hmStream.Printf("           Invert*");
		hmStream.SSPrintf(HtmlTagStreamMode, "/div");

		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 672px; left: 236px;\"");
			if (!timewindows_bool_vector[5]) 	  hmStream.Printf("    --------------------");
			else if (timewindows_dropdown_vector[16] == 0) hmStream.Printf("             off");
			else if (timewindows_dropdown_vector[16] == 1) hmStream.Printf("     current control");
			else if (timewindows_dropdown_vector[16] == 2) hmStream.Printf("  position-Soft PID");
			else if (timewindows_dropdown_vector[16] == 3) hmStream.Printf(" position-Med. PID");
			else if (timewindows_dropdown_vector[16] == 4) hmStream.Printf(" position-Hard PID");
			else if (timewindows_dropdown_vector[16] == 5) hmStream.Printf(" position-Auto PID");
			else if (timewindows_dropdown_vector[16] == 6) hmStream.Printf(" Integrated control*");
			else if (timewindows_dropdown_vector[16] == 7) hmStream.Printf("           Invert*");
		hmStream.SSPrintf(HtmlTagStreamMode, "/div");

		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 672px; left: 370px;\"");
			if (!timewindows_bool_vector[5]) 	  hmStream.Printf("    --------------------");
			else if (timewindows_dropdown_vector[17] == 0) hmStream.Printf("             off");
			else if (timewindows_dropdown_vector[17] == 1) hmStream.Printf("     current control");
			else if (timewindows_dropdown_vector[17] == 2) hmStream.Printf("  position-Soft PID");
			else if (timewindows_dropdown_vector[17] == 3) hmStream.Printf(" position-Med. PID");
			else if (timewindows_dropdown_vector[17] == 4) hmStream.Printf(" position-Hard PID");
			else if (timewindows_dropdown_vector[17] == 5) hmStream.Printf(" position-Auto PID");
			else if (timewindows_dropdown_vector[17] == 6) hmStream.Printf(" Integrated control*");
			else if (timewindows_dropdown_vector[17] == 7) hmStream.Printf("           Invert*");
		hmStream.SSPrintf(HtmlTagStreamMode, "/div");

		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 694px; left: 94px;\""); 
			if (!timewindows_bool_vector[6]) 	  hmStream.Printf("    --------------------");
			else if (timewindows_dropdown_vector[18] == 0) hmStream.Printf("             off");
			else if (timewindows_dropdown_vector[18] == 1) hmStream.Printf("     current control");
			else if (timewindows_dropdown_vector[18] == 2) hmStream.Printf(" Ip control-Soft PID");
			else if (timewindows_dropdown_vector[18] == 3) hmStream.Printf("Ip control-Med. PID");
			else if (timewindows_dropdown_vector[18] == 4) hmStream.Printf("Ip control-Hard PID");
			else if (timewindows_dropdown_vector[18] == 5) hmStream.Printf("Ip control-Auto PID");
			else if (timewindows_dropdown_vector[18] == 6) hmStream.Printf(" Integrated control*");
			else if (timewindows_dropdown_vector[18] == 7) hmStream.Printf("           Invert*");
		hmStream.SSPrintf(HtmlTagStreamMode, "/div");

		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 694px; left: 236px;\"");
			if (!timewindows_bool_vector[6]) 	  hmStream.Printf("    --------------------");
			else if (timewindows_dropdown_vector[19] == 0) hmStream.Printf("             off");
			else if (timewindows_dropdown_vector[19] == 1) hmStream.Printf("     current control");
			else if (timewindows_dropdown_vector[19] == 2) hmStream.Printf("  position-Soft PID");
			else if (timewindows_dropdown_vector[19] == 3) hmStream.Printf(" position-Med. PID");
			else if (timewindows_dropdown_vector[19] == 4) hmStream.Printf(" position-Hard PID");
			else if (timewindows_dropdown_vector[19] == 5) hmStream.Printf(" position-Auto PID");
			else if (timewindows_dropdown_vector[19] == 6) hmStream.Printf(" Integrated control*");
			else if (timewindows_dropdown_vector[19] == 7) hmStream.Printf("           Invert*");
		hmStream.SSPrintf(HtmlTagStreamMode, "/div");

		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 694px; left: 370px;\"");
			if (!timewindows_bool_vector[6]) 	  hmStream.Printf("    --------------------");
			else if (timewindows_dropdown_vector[20] == 0) hmStream.Printf("             off");
			else if (timewindows_dropdown_vector[20] == 1) hmStream.Printf("     current control");
			else if (timewindows_dropdown_vector[20] == 2) hmStream.Printf("  position-Soft PID");
			else if (timewindows_dropdown_vector[20] == 3) hmStream.Printf(" position-Med. PID");
			else if (timewindows_dropdown_vector[20] == 4) hmStream.Printf(" position-Hard PID");
			else if (timewindows_dropdown_vector[20] == 5) hmStream.Printf(" position-Auto PID");
			else if (timewindows_dropdown_vector[20] == 6) hmStream.Printf(" Integrated control*");
			else if (timewindows_dropdown_vector[20] == 7) hmStream.Printf("           Invert*");
		hmStream.SSPrintf(HtmlTagStreamMode, "/div");
	
		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 762px; left: 94px;\""); 
			if (!timewindows_bool_vector[7]) 	  hmStream.Printf("    --------------------");
			else if (timewindows_dropdown_vector[21] == 0) hmStream.Printf("             off");
			else if (timewindows_dropdown_vector[21] == 1) hmStream.Printf("     current control");
			else if (timewindows_dropdown_vector[21] == 2) hmStream.Printf(" Ip control-Soft PID");
			else if (timewindows_dropdown_vector[21] == 3) hmStream.Printf("Ip control-Med. PID");
			else if (timewindows_dropdown_vector[21] == 4) hmStream.Printf("Ip control-Hard PID");
			else if (timewindows_dropdown_vector[21] == 5) hmStream.Printf("Ip control-Auto PID");
			else if (timewindows_dropdown_vector[21] == 6) hmStream.Printf(" Integrated control*");
			else if (timewindows_dropdown_vector[21] == 7) hmStream.Printf("           Invert*");
		hmStream.SSPrintf(HtmlTagStreamMode, "/div");

		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 762px; left: 236px;\"");
			if (!timewindows_bool_vector[7]) 	  hmStream.Printf("    --------------------");
			else if (timewindows_dropdown_vector[22] == 0) hmStream.Printf("             off");
			else if (timewindows_dropdown_vector[22] == 1) hmStream.Printf("     current control");
			else if (timewindows_dropdown_vector[22] == 2) hmStream.Printf("  position-Soft PID");
			else if (timewindows_dropdown_vector[22] == 3) hmStream.Printf(" position-Med. PID");
			else if (timewindows_dropdown_vector[22] == 4) hmStream.Printf(" position-Hard PID");
			else if (timewindows_dropdown_vector[22] == 5) hmStream.Printf(" position-Auto PID");
			else if (timewindows_dropdown_vector[22] == 6) hmStream.Printf(" Integrated control*");
			else if (timewindows_dropdown_vector[22] == 7) hmStream.Printf("           Invert*");
		hmStream.SSPrintf(HtmlTagStreamMode, "/div");

		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 762px; left: 370px;\"");
			if (!timewindows_bool_vector[7]) 	  hmStream.Printf("    --------------------");
			else if (timewindows_dropdown_vector[23] == 0) hmStream.Printf("             off");
			else if (timewindows_dropdown_vector[23] == 1) hmStream.Printf("     current control");
			else if (timewindows_dropdown_vector[23] == 2) hmStream.Printf("  position-Soft PID");
			else if (timewindows_dropdown_vector[23] == 3) hmStream.Printf(" position-Med. PID");
			else if (timewindows_dropdown_vector[23] == 4) hmStream.Printf(" position-Hard PID");
			else if (timewindows_dropdown_vector[23] == 5) hmStream.Printf(" position-Auto PID");
			else if (timewindows_dropdown_vector[23] == 6) hmStream.Printf(" Integrated control*");
			else if (timewindows_dropdown_vector[23] == 7) hmStream.Printf("           Invert*");
		hmStream.SSPrintf(HtmlTagStreamMode, "/div");

		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 784px; left: 94px;\"");
			if (!timewindows_bool_vector[8]) 	  hmStream.Printf("    --------------------");
			else if (timewindows_dropdown_vector[24] == 0) hmStream.Printf("             off");
			else if (timewindows_dropdown_vector[24] == 1) hmStream.Printf("     current control");
			else if (timewindows_dropdown_vector[24] == 2) hmStream.Printf(" Ip control-Soft PID");
			else if (timewindows_dropdown_vector[24] == 3) hmStream.Printf("Ip control-Med. PID");
			else if (timewindows_dropdown_vector[24] == 4) hmStream.Printf("Ip control-Hard PID");
			else if (timewindows_dropdown_vector[24] == 5) hmStream.Printf("Ip control-Auto PID");
			else if (timewindows_dropdown_vector[24] == 6) hmStream.Printf(" Integrated control*");
			else if (timewindows_dropdown_vector[24] == 7) hmStream.Printf("           Invert*");
		hmStream.SSPrintf(HtmlTagStreamMode, "/div");

		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 784px; left: 236px;\"");
			if (!timewindows_bool_vector[8]) 	  hmStream.Printf("    --------------------");
			else if (timewindows_dropdown_vector[25] == 0) hmStream.Printf("             off");
			else if (timewindows_dropdown_vector[25] == 1) hmStream.Printf("     current control");
			else if (timewindows_dropdown_vector[25] == 2) hmStream.Printf("  position-Soft PID");
			else if (timewindows_dropdown_vector[25] == 3) hmStream.Printf(" position-Med. PID");
			else if (timewindows_dropdown_vector[25] == 4) hmStream.Printf(" position-Hard PID");
			else if (timewindows_dropdown_vector[25] == 5) hmStream.Printf(" position-Auto PID");
			else if (timewindows_dropdown_vector[25] == 6) hmStream.Printf(" Integrated control*");
			else if (timewindows_dropdown_vector[25] == 7) hmStream.Printf("           Invert*");
		hmStream.SSPrintf(HtmlTagStreamMode, "/div");

		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 784px; left: 370px;\"");
			if (!timewindows_bool_vector[8]) 	  hmStream.Printf("    --------------------");
			else if (timewindows_dropdown_vector[26] == 0) hmStream.Printf("             off");
			else if (timewindows_dropdown_vector[26] == 1) hmStream.Printf("     current control");
			else if (timewindows_dropdown_vector[26] == 2) hmStream.Printf("  position-Soft PID");
			else if (timewindows_dropdown_vector[26] == 3) hmStream.Printf(" position-Med. PID");
			else if (timewindows_dropdown_vector[26] == 4) hmStream.Printf(" position-Hard PID");
			else if (timewindows_dropdown_vector[26] == 5) hmStream.Printf(" position-Auto PID");
			else if (timewindows_dropdown_vector[26] == 6) hmStream.Printf(" Integrated control*");
			else if (timewindows_dropdown_vector[26] == 7) hmStream.Printf("           Invert*");
		hmStream.SSPrintf(HtmlTagStreamMode, "/div");

		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 806px; left: 94px;\"");
			if (!timewindows_bool_vector[9]) 	  hmStream.Printf("    --------------------");
			else if (timewindows_dropdown_vector[27] == 0) hmStream.Printf("             off");
			else if (timewindows_dropdown_vector[27] == 1) hmStream.Printf("     current control");
			else if (timewindows_dropdown_vector[27] == 2) hmStream.Printf(" Ip control-Soft PID");
			else if (timewindows_dropdown_vector[27] == 3) hmStream.Printf("Ip control-Med. PID");
			else if (timewindows_dropdown_vector[27] == 4) hmStream.Printf("Ip control-Hard PID");
			else if (timewindows_dropdown_vector[27] == 5) hmStream.Printf("Ip control-Auto PID");
			else if (timewindows_dropdown_vector[27] == 6) hmStream.Printf(" Integrated control*");
			else if (timewindows_dropdown_vector[27] == 7) hmStream.Printf("           Invert*");
		hmStream.SSPrintf(HtmlTagStreamMode, "/div");

		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 806px; left: 236px;\"");
			if (!timewindows_bool_vector[9]) 	  hmStream.Printf("    --------------------");
			else if (timewindows_dropdown_vector[28] == 0) hmStream.Printf("             off");
			else if (timewindows_dropdown_vector[28] == 1) hmStream.Printf("     current control");
			else if (timewindows_dropdown_vector[28] == 2) hmStream.Printf("  position-Soft PID");
			else if (timewindows_dropdown_vector[28] == 3) hmStream.Printf(" position-Med. PID");
			else if (timewindows_dropdown_vector[28] == 4) hmStream.Printf(" position-Hard PID");
			else if (timewindows_dropdown_vector[28] == 5) hmStream.Printf(" position-Auto PID");
			else if (timewindows_dropdown_vector[28] == 6) hmStream.Printf(" Integrated control*");
			else if (timewindows_dropdown_vector[28] == 7) hmStream.Printf("           Invert*");
		hmStream.SSPrintf(HtmlTagStreamMode, "/div");

		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 806px; left: 370px;\"");
			if (!timewindows_bool_vector[9]) 	  hmStream.Printf("    --------------------");
			else if (timewindows_dropdown_vector[29] == 0) hmStream.Printf("             off");
			else if (timewindows_dropdown_vector[29] == 1) hmStream.Printf("     current control");
			else if (timewindows_dropdown_vector[29] == 2) hmStream.Printf("  position-Soft PID");
			else if (timewindows_dropdown_vector[29] == 3) hmStream.Printf(" position-Med. PID");
			else if (timewindows_dropdown_vector[29] == 4) hmStream.Printf(" position-Hard PID");
			else if (timewindows_dropdown_vector[29] == 5) hmStream.Printf(" position-Auto PID");
			else if (timewindows_dropdown_vector[29] == 6) hmStream.Printf(" Integrated control*");
			else if (timewindows_dropdown_vector[29] == 7) hmStream.Printf("           Invert*");
		hmStream.SSPrintf(HtmlTagStreamMode, "/div");
	
		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 828px; left: 94px;\""); 
			if (!timewindows_bool_vector[10]) 	  hmStream.Printf("    --------------------");
			else if (timewindows_dropdown_vector[30] == 0) hmStream.Printf("             off");
			else if (timewindows_dropdown_vector[30] == 1) hmStream.Printf("     current control");
			else if (timewindows_dropdown_vector[30] == 2) hmStream.Printf(" Ip control-Soft PID");
			else if (timewindows_dropdown_vector[30] == 3) hmStream.Printf("Ip control-Med. PID");
			else if (timewindows_dropdown_vector[30] == 4) hmStream.Printf("Ip control-Hard PID");
			else if (timewindows_dropdown_vector[30] == 5) hmStream.Printf("Ip control-Auto PID");
			else if (timewindows_dropdown_vector[30] == 6) hmStream.Printf(" Integrated control*");
			else if (timewindows_dropdown_vector[30] == 7) hmStream.Printf("           Invert*");
		hmStream.SSPrintf(HtmlTagStreamMode, "/div");

		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 828px; left: 236px;\"");
			if (!timewindows_bool_vector[10]) 	  hmStream.Printf("    --------------------");
			else if (timewindows_dropdown_vector[31] == 0) hmStream.Printf("             off");
			else if (timewindows_dropdown_vector[31] == 1) hmStream.Printf("     current control");
			else if (timewindows_dropdown_vector[31] == 2) hmStream.Printf("  position-Soft PID");
			else if (timewindows_dropdown_vector[31] == 3) hmStream.Printf(" position-Med. PID");
			else if (timewindows_dropdown_vector[31] == 4) hmStream.Printf(" position-Hard PID");
			else if (timewindows_dropdown_vector[31] == 5) hmStream.Printf(" position-Auto PID");
			else if (timewindows_dropdown_vector[31] == 6) hmStream.Printf(" Integrated control*");
			else if (timewindows_dropdown_vector[31] == 7) hmStream.Printf("           Invert*");
		hmStream.SSPrintf(HtmlTagStreamMode, "/div");

		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 828px; left: 370px;\"");
			if (!timewindows_bool_vector[10]) 	  hmStream.Printf("    --------------------");
			else if (timewindows_dropdown_vector[32] == 0) hmStream.Printf("             off");
			else if (timewindows_dropdown_vector[32] == 1) hmStream.Printf("     current control");
			else if (timewindows_dropdown_vector[32] == 2) hmStream.Printf("  position-Soft PID");
			else if (timewindows_dropdown_vector[32] == 3) hmStream.Printf(" position-Med. PID");
			else if (timewindows_dropdown_vector[32] == 4) hmStream.Printf(" position-Hard PID");
			else if (timewindows_dropdown_vector[32] == 5) hmStream.Printf(" position-Auto PID");
			else if (timewindows_dropdown_vector[32] == 6) hmStream.Printf(" Integrated control*");
			else if (timewindows_dropdown_vector[32] == 7) hmStream.Printf("           Invert*");
		hmStream.SSPrintf(HtmlTagStreamMode, "/div");

		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 850px; left: 94px;\""); 
			if (!timewindows_bool_vector[11]) 	  hmStream.Printf("    --------------------");
			else if (timewindows_dropdown_vector[33] == 0) hmStream.Printf("             off");
			else if (timewindows_dropdown_vector[33] == 1) hmStream.Printf("     current control");
			else if (timewindows_dropdown_vector[33] == 2) hmStream.Printf(" Ip control-Soft PID");
			else if (timewindows_dropdown_vector[33] == 3) hmStream.Printf("Ip control-Med. PID");
			else if (timewindows_dropdown_vector[33] == 4) hmStream.Printf("Ip control-Hard PID");
			else if (timewindows_dropdown_vector[33] == 5) hmStream.Printf("Ip control-Auto PID");
			else if (timewindows_dropdown_vector[33] == 6) hmStream.Printf(" Integrated control*");
			else if (timewindows_dropdown_vector[33] == 7) hmStream.Printf("           Invert*");
		hmStream.SSPrintf(HtmlTagStreamMode, "/div");

		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 850px; left: 236px;\"");
			if (!timewindows_bool_vector[11]) 	  hmStream.Printf("    --------------------");
			else if (timewindows_dropdown_vector[34] == 0) hmStream.Printf("             off");
			else if (timewindows_dropdown_vector[34] == 1) hmStream.Printf("     current control");
			else if (timewindows_dropdown_vector[34] == 2) hmStream.Printf("  position-Soft PID");
			else if (timewindows_dropdown_vector[34] == 3) hmStream.Printf(" position-Med. PID");
			else if (timewindows_dropdown_vector[34] == 4) hmStream.Printf(" position-Hard PID");
			else if (timewindows_dropdown_vector[34] == 5) hmStream.Printf(" position-Auto PID");
			else if (timewindows_dropdown_vector[34] == 6) hmStream.Printf(" Integrated control*");
			else if (timewindows_dropdown_vector[34] == 7) hmStream.Printf("           Invert*");
		hmStream.SSPrintf(HtmlTagStreamMode, "/div");

		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 850px; left: 370px;\"");
			if (!timewindows_bool_vector[11]) 	  hmStream.Printf("    --------------------");
			else if (timewindows_dropdown_vector[35] == 0) hmStream.Printf("             off");
			else if (timewindows_dropdown_vector[35] == 1) hmStream.Printf("     current control");
			else if (timewindows_dropdown_vector[35] == 2) hmStream.Printf("  position-Soft PID");
			else if (timewindows_dropdown_vector[35] == 3) hmStream.Printf(" position-Med. PID");
			else if (timewindows_dropdown_vector[35] == 4) hmStream.Printf(" position-Hard PID");
			else if (timewindows_dropdown_vector[35] == 5) hmStream.Printf(" position-Auto PID");
			else if (timewindows_dropdown_vector[35] == 6) hmStream.Printf(" Integrated control*");
			else if (timewindows_dropdown_vector[35] == 7) hmStream.Printf("           Invert*");
		hmStream.SSPrintf(HtmlTagStreamMode, "/div");

		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 872px; left: 94px;\""); 
			if (!timewindows_bool_vector[12]) 	  hmStream.Printf("    --------------------");
			else if (timewindows_dropdown_vector[36] == 0) hmStream.Printf("             off");
			else if (timewindows_dropdown_vector[36] == 1) hmStream.Printf("     current control");
			else if (timewindows_dropdown_vector[36] == 2) hmStream.Printf(" Ip control-Soft PID");
			else if (timewindows_dropdown_vector[36] == 3) hmStream.Printf("Ip control-Med. PID");
			else if (timewindows_dropdown_vector[36] == 4) hmStream.Printf("Ip control-Hard PID");
			else if (timewindows_dropdown_vector[36] == 5) hmStream.Printf("Ip control-Auto PID");
			else if (timewindows_dropdown_vector[36] == 6) hmStream.Printf(" Integrated control*");
			else if (timewindows_dropdown_vector[36] == 7) hmStream.Printf("           Invert*");
		hmStream.SSPrintf(HtmlTagStreamMode, "/div");

		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 872px; left: 236px;\"");
			if (!timewindows_bool_vector[12]) 	  hmStream.Printf("    --------------------");
			else if (timewindows_dropdown_vector[37] == 0) hmStream.Printf("             off");
			else if (timewindows_dropdown_vector[37] == 1) hmStream.Printf("     current control");
			else if (timewindows_dropdown_vector[37] == 2) hmStream.Printf("  position-Soft PID");
			else if (timewindows_dropdown_vector[37] == 3) hmStream.Printf(" position-Med. PID");
			else if (timewindows_dropdown_vector[37] == 4) hmStream.Printf(" position-Hard PID");
			else if (timewindows_dropdown_vector[37] == 5) hmStream.Printf(" position-Auto PID");
			else if (timewindows_dropdown_vector[37] == 6) hmStream.Printf(" Integrated control*");
			else if (timewindows_dropdown_vector[37] == 7) hmStream.Printf("           Invert*");
		hmStream.SSPrintf(HtmlTagStreamMode, "/div");

		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 872px; left: 370px;\"");
			if (!timewindows_bool_vector[12]) 	  hmStream.Printf("    --------------------");
			else if (timewindows_dropdown_vector[38] == 0) hmStream.Printf("             off");
			else if (timewindows_dropdown_vector[38] == 1) hmStream.Printf("     current control");
			else if (timewindows_dropdown_vector[38] == 2) hmStream.Printf("  position-Soft PID");
			else if (timewindows_dropdown_vector[38] == 3) hmStream.Printf(" position-Med. PID");
			else if (timewindows_dropdown_vector[38] == 4) hmStream.Printf(" position-Hard PID");
			else if (timewindows_dropdown_vector[38] == 5) hmStream.Printf(" position-Auto PID");
			else if (timewindows_dropdown_vector[38] == 6) hmStream.Printf(" Integrated control*");
			else if (timewindows_dropdown_vector[38] == 7) hmStream.Printf("           Invert*");
		hmStream.SSPrintf(HtmlTagStreamMode, "/div");

		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 894px; left: 94px;\""); 
			if (!timewindows_bool_vector[13]) 	  hmStream.Printf("    --------------------");
			else if (timewindows_dropdown_vector[39] == 0) hmStream.Printf("             off");
			else if (timewindows_dropdown_vector[39] == 1) hmStream.Printf("     current control");
			else if (timewindows_dropdown_vector[39] == 2) hmStream.Printf(" Ip control-Soft PID");
			else if (timewindows_dropdown_vector[39] == 3) hmStream.Printf("Ip control-Med. PID");
			else if (timewindows_dropdown_vector[39] == 4) hmStream.Printf("Ip control-Hard PID");
			else if (timewindows_dropdown_vector[39] == 5) hmStream.Printf("Ip control-Auto PID");
			else if (timewindows_dropdown_vector[39] == 6) hmStream.Printf(" Integrated control*");
			else if (timewindows_dropdown_vector[39] == 7) hmStream.Printf("           Invert*");
		hmStream.SSPrintf(HtmlTagStreamMode, "/div");

		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 894px; left: 236px;\"");
			if (!timewindows_bool_vector[13]) 	  hmStream.Printf("    --------------------");
			else if (timewindows_dropdown_vector[40] == 0) hmStream.Printf("             off");
			else if (timewindows_dropdown_vector[40] == 1) hmStream.Printf("     current control");
			else if (timewindows_dropdown_vector[40] == 2) hmStream.Printf("  position-Soft PID");
			else if (timewindows_dropdown_vector[40] == 3) hmStream.Printf(" position-Med. PID");
			else if (timewindows_dropdown_vector[40] == 4) hmStream.Printf(" position-Hard PID");
			else if (timewindows_dropdown_vector[40] == 5) hmStream.Printf(" position-Auto PID");
			else if (timewindows_dropdown_vector[40] == 6) hmStream.Printf(" Integrated control*");
			else if (timewindows_dropdown_vector[40] == 7) hmStream.Printf("           Invert*");
		hmStream.SSPrintf(HtmlTagStreamMode, "/div");

		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 894px; left: 370px;\"");
			if (!timewindows_bool_vector[13]) 	  hmStream.Printf("    --------------------");
			else if (timewindows_dropdown_vector[41] == 0) hmStream.Printf("             off");
			else if (timewindows_dropdown_vector[41] == 1) hmStream.Printf("     current control");
			else if (timewindows_dropdown_vector[41] == 2) hmStream.Printf("  position-Soft PID");
			else if (timewindows_dropdown_vector[41] == 3) hmStream.Printf(" position-Med. PID");
			else if (timewindows_dropdown_vector[41] == 4) hmStream.Printf(" position-Hard PID");
			else if (timewindows_dropdown_vector[41] == 5) hmStream.Printf(" position-Auto PID");
			else if (timewindows_dropdown_vector[41] == 6) hmStream.Printf(" Integrated control*");
			else if (timewindows_dropdown_vector[41] == 7) hmStream.Printf("           Invert*");
		hmStream.SSPrintf(HtmlTagStreamMode, "/div");

	}
	else {
		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 467px; left: 180px;\"");
		hmStream.SSPrintf(HtmlTagStreamMode, "input type=\"submit\" name=\"submit_save_timewindows\" value=\"Save parameters\"");
		hmStream.SSPrintf(HtmlTagStreamMode, "/div");
	
		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 562px; left: 94px;\""); 
			hmStream.SSPrintf(HtmlTagStreamMode, "select NAME=\"form_timewindows_drop_0\""); 
				if (timewindows_dropdown_vector[0] == 0) hmStream.SSPrintf(HtmlTagStreamMode, "option selected=\"selected\" value=0 >off</option");
				else hmStream.SSPrintf(HtmlTagStreamMode, "option value=0>off</option");
				if (timewindows_dropdown_vector[0] == 1) hmStream.SSPrintf(HtmlTagStreamMode, "option selected=\"selected\" value=1>current control</option");
				else hmStream.SSPrintf(HtmlTagStreamMode, "option value=1>current control</option");
				if (timewindows_dropdown_vector[0] == 2) hmStream.SSPrintf(HtmlTagStreamMode, "option selected=\"selected\" value=2>Ip control-Soft PID</option");
				else hmStream.SSPrintf(HtmlTagStreamMode, "option value=2>Ip control-Soft PID</option");
				if (timewindows_dropdown_vector[0] == 3) hmStream.SSPrintf(HtmlTagStreamMode, "option selected=\"selected\" value=3>Ip control-Med. PID</option");
				else hmStream.SSPrintf(HtmlTagStreamMode, "option value=3>Ip control-Med. PID</option");
				if (timewindows_dropdown_vector[0] == 4) hmStream.SSPrintf(HtmlTagStreamMode, "option selected=\"selected\" value=4>Ip control-Hard PID</option");
				else hmStream.SSPrintf(HtmlTagStreamMode, "option value=4>Ip control-Hard PID</option");
				if (timewindows_dropdown_vector[0] == 5) hmStream.SSPrintf(HtmlTagStreamMode, "option selected=\"selected\" value=5>Ip control-Auto PID</option");
				else hmStream.SSPrintf(HtmlTagStreamMode, "option value=5>Ip control-Auto PID</option");
				if (timewindows_dropdown_vector[0] == 6) hmStream.SSPrintf(HtmlTagStreamMode, "option selected=\"selected\" value=6>Integrated control*</option");
				else hmStream.SSPrintf(HtmlTagStreamMode, "option value=6>Integrated control*</option");
				if (timewindows_dropdown_vector[0] == 7) hmStream.SSPrintf(HtmlTagStreamMode, "option selected=\"selected\" value=7>Invert*</option");
				else hmStream.SSPrintf(HtmlTagStreamMode, "option value=7>Invert*</option");
			hmStream.SSPrintf(HtmlTagStreamMode, "/select");
		hmStream.SSPrintf(HtmlTagStreamMode, "/div");

		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 562px; left: 236px;\"");
			hmStream.SSPrintf(HtmlTagStreamMode, "select NAME=\"form_timewindows_drop_1\""); 
				if (timewindows_dropdown_vector[1] == 0) hmStream.SSPrintf(HtmlTagStreamMode, "option selected=\"selected\" value=0 >off</option");
				else hmStream.SSPrintf(HtmlTagStreamMode, "option value=0>off</option");
				if (timewindows_dropdown_vector[1] == 1) hmStream.SSPrintf(HtmlTagStreamMode, "option selected=\"selected\" value=1>current control</option");
				else hmStream.SSPrintf(HtmlTagStreamMode, "option value=1>current control</option");
				if (timewindows_dropdown_vector[1] == 2) hmStream.SSPrintf(HtmlTagStreamMode, "option selected=\"selected\" value=2>position-Soft PID</option");
				else hmStream.SSPrintf(HtmlTagStreamMode, "option value=2>position-Soft PID</option");
				if (timewindows_dropdown_vector[1] == 3) hmStream.SSPrintf(HtmlTagStreamMode, "option selected=\"selected\" value=3>position-Med. PID</option");
				else hmStream.SSPrintf(HtmlTagStreamMode, "option value=3>position-Med. PID</option");
				if (timewindows_dropdown_vector[1] == 4) hmStream.SSPrintf(HtmlTagStreamMode, "option selected=\"selected\" value=4>position-Hard PID</option");
				else hmStream.SSPrintf(HtmlTagStreamMode, "option value=4>position-Hard PID</option");
				if (timewindows_dropdown_vector[1] == 5) hmStream.SSPrintf(HtmlTagStreamMode, "option selected=\"selected\" value=5>position-Auto PID</option");
				else hmStream.SSPrintf(HtmlTagStreamMode, "option value=5>position-Auto PID</option");
				if (timewindows_dropdown_vector[1] == 6) hmStream.SSPrintf(HtmlTagStreamMode, "option selected=\"selected\" value=6>Integrated control*</option");
				else hmStream.SSPrintf(HtmlTagStreamMode, "option value=6>Integrated control*</option");
				if (timewindows_dropdown_vector[1] == 7) hmStream.SSPrintf(HtmlTagStreamMode, "option selected=\"selected\" value=7>Invert*</option");
				else hmStream.SSPrintf(HtmlTagStreamMode, "option value=7>Invert*</option");
			hmStream.SSPrintf(HtmlTagStreamMode, "/select");
		hmStream.SSPrintf(HtmlTagStreamMode, "/div");

		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 562px; left: 370px;\"");
			hmStream.SSPrintf(HtmlTagStreamMode, "select NAME=\"form_timewindows_drop_2\""); 
				if (timewindows_dropdown_vector[2] == 0) hmStream.SSPrintf(HtmlTagStreamMode, "option selected=\"selected\" value=0 >off</option");
				else hmStream.SSPrintf(HtmlTagStreamMode, "option value=0>off</option");
				if (timewindows_dropdown_vector[2] == 1) hmStream.SSPrintf(HtmlTagStreamMode, "option selected=\"selected\" value=1>current control</option");
				else hmStream.SSPrintf(HtmlTagStreamMode, "option value=1>current control</option");
				if (timewindows_dropdown_vector[2] == 2) hmStream.SSPrintf(HtmlTagStreamMode, "option selected=\"selected\" value=2>position-Soft PID</option");
				else hmStream.SSPrintf(HtmlTagStreamMode, "option value=2>position-Soft PID</option");
				if (timewindows_dropdown_vector[2] == 3) hmStream.SSPrintf(HtmlTagStreamMode, "option selected=\"selected\" value=3>position-Med. PID</option");
				else hmStream.SSPrintf(HtmlTagStreamMode, "option value=3>position-Med. PID</option");
				if (timewindows_dropdown_vector[2] == 4) hmStream.SSPrintf(HtmlTagStreamMode, "option selected=\"selected\" value=4>position-Hard PID</option");
				else hmStream.SSPrintf(HtmlTagStreamMode, "option value=4>position-Hard PID</option");
				if (timewindows_dropdown_vector[2] == 5) hmStream.SSPrintf(HtmlTagStreamMode, "option selected=\"selected\" value=5>position-Auto PID</option");
				else hmStream.SSPrintf(HtmlTagStreamMode, "option value=5>position-Auto PID</option");
				if (timewindows_dropdown_vector[2] == 6) hmStream.SSPrintf(HtmlTagStreamMode, "option selected=\"selected\" value=6>Integrated control*</option");
				else hmStream.SSPrintf(HtmlTagStreamMode, "option value=6>Integrated control*</option");
				if (timewindows_dropdown_vector[2] == 7) hmStream.SSPrintf(HtmlTagStreamMode, "option selected=\"selected\" value=7>Invert*</option");
				else hmStream.SSPrintf(HtmlTagStreamMode, "option value=7>Invert*</option");
			hmStream.SSPrintf(HtmlTagStreamMode, "/select");
		hmStream.SSPrintf(HtmlTagStreamMode, "/div");

		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 584px; left: 94px;\"");
			hmStream.SSPrintf(HtmlTagStreamMode, "select NAME=\"form_timewindows_drop_3\""); 
				if (timewindows_dropdown_vector[3] == 0) hmStream.SSPrintf(HtmlTagStreamMode, "option selected=\"selected\" value=0 >off</option");
				else hmStream.SSPrintf(HtmlTagStreamMode, "option value=0>off</option");
				if (timewindows_dropdown_vector[3] == 1) hmStream.SSPrintf(HtmlTagStreamMode, "option selected=\"selected\" value=1>current control</option");
				else hmStream.SSPrintf(HtmlTagStreamMode, "option value=1>current control</option");
				if (timewindows_dropdown_vector[3] == 2) hmStream.SSPrintf(HtmlTagStreamMode, "option selected=\"selected\" value=2>Ip control-Soft PID</option");
				else hmStream.SSPrintf(HtmlTagStreamMode, "option value=2>Ip control-Soft PID</option");
				if (timewindows_dropdown_vector[3] == 3) hmStream.SSPrintf(HtmlTagStreamMode, "option selected=\"selected\" value=3>Ip control-Med. PID</option");
				else hmStream.SSPrintf(HtmlTagStreamMode, "option value=3>Ip control-Med. PID</option");
				if (timewindows_dropdown_vector[3] == 4) hmStream.SSPrintf(HtmlTagStreamMode, "option selected=\"selected\" value=4>Ip control-Hard PID</option");
				else hmStream.SSPrintf(HtmlTagStreamMode, "option value=4>Ip control-Hard PID</option");
				if (timewindows_dropdown_vector[3] == 5) hmStream.SSPrintf(HtmlTagStreamMode, "option selected=\"selected\" value=5>Ip control-Auto PID</option");
				else hmStream.SSPrintf(HtmlTagStreamMode, "option value=5>Ip control-Auto PID</option");
				if (timewindows_dropdown_vector[3] == 6) hmStream.SSPrintf(HtmlTagStreamMode, "option selected=\"selected\" value=6>Integrated control*</option");
				else hmStream.SSPrintf(HtmlTagStreamMode, "option value=6>Integrated control*</option");
				if (timewindows_dropdown_vector[3] == 7) hmStream.SSPrintf(HtmlTagStreamMode, "option selected=\"selected\" value=7>Invert*</option");
				else hmStream.SSPrintf(HtmlTagStreamMode, "option value=7>Invert*</option");
			hmStream.SSPrintf(HtmlTagStreamMode, "/select");
		hmStream.SSPrintf(HtmlTagStreamMode, "/div");

		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 584px; left: 236px;\"");
			hmStream.SSPrintf(HtmlTagStreamMode, "select NAME=\"form_timewindows_drop_4\""); 
				if (timewindows_dropdown_vector[4] == 0) hmStream.SSPrintf(HtmlTagStreamMode, "option selected=\"selected\" value=0 >off</option");
				else hmStream.SSPrintf(HtmlTagStreamMode, "option value=0>off</option");
				if (timewindows_dropdown_vector[4] == 1) hmStream.SSPrintf(HtmlTagStreamMode, "option selected=\"selected\" value=1>current control</option");
				else hmStream.SSPrintf(HtmlTagStreamMode, "option value=1>current control</option");
				if (timewindows_dropdown_vector[4] == 2) hmStream.SSPrintf(HtmlTagStreamMode, "option selected=\"selected\" value=2>position-Soft PID</option");
				else hmStream.SSPrintf(HtmlTagStreamMode, "option value=2>position-Soft PID</option");
				if (timewindows_dropdown_vector[4] == 3) hmStream.SSPrintf(HtmlTagStreamMode, "option selected=\"selected\" value=3>position-Med. PID</option");
				else hmStream.SSPrintf(HtmlTagStreamMode, "option value=3>position-Med. PID</option");
				if (timewindows_dropdown_vector[4] == 4) hmStream.SSPrintf(HtmlTagStreamMode, "option selected=\"selected\" value=4>position-Hard PID</option");
				else hmStream.SSPrintf(HtmlTagStreamMode, "option value=4>position-Hard PID</option");
				if (timewindows_dropdown_vector[4] == 5) hmStream.SSPrintf(HtmlTagStreamMode, "option selected=\"selected\" value=5>position-Auto PID</option");
				else hmStream.SSPrintf(HtmlTagStreamMode, "option value=5>position-Auto PID</option");
				if (timewindows_dropdown_vector[4] == 6) hmStream.SSPrintf(HtmlTagStreamMode, "option selected=\"selected\" value=6>Integrated control*</option");
				else hmStream.SSPrintf(HtmlTagStreamMode, "option value=6>Integrated control*</option");
				if (timewindows_dropdown_vector[4] == 7) hmStream.SSPrintf(HtmlTagStreamMode, "option selected=\"selected\" value=7>Invert*</option");
				else hmStream.SSPrintf(HtmlTagStreamMode, "option value=7>Invert*</option");
			hmStream.SSPrintf(HtmlTagStreamMode, "/select");
		hmStream.SSPrintf(HtmlTagStreamMode, "/div");

		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 584px; left: 370px;\"");
			hmStream.SSPrintf(HtmlTagStreamMode, "select NAME=\"form_timewindows_drop_5\""); 
				if (timewindows_dropdown_vector[5] == 0) hmStream.SSPrintf(HtmlTagStreamMode, "option selected=\"selected\" value=0 >off</option");
				else hmStream.SSPrintf(HtmlTagStreamMode, "option value=0>off</option");
				if (timewindows_dropdown_vector[5] == 1) hmStream.SSPrintf(HtmlTagStreamMode, "option selected=\"selected\" value=1>current control</option");
				else hmStream.SSPrintf(HtmlTagStreamMode, "option value=1>current control</option");
				if (timewindows_dropdown_vector[5] == 2) hmStream.SSPrintf(HtmlTagStreamMode, "option selected=\"selected\" value=2>position-Soft PID</option");
				else hmStream.SSPrintf(HtmlTagStreamMode, "option value=2>position-Soft PID</option");
				if (timewindows_dropdown_vector[5] == 3) hmStream.SSPrintf(HtmlTagStreamMode, "option selected=\"selected\" value=3>position-Med. PID</option");
				else hmStream.SSPrintf(HtmlTagStreamMode, "option value=3>position-Med. PID</option");
				if (timewindows_dropdown_vector[5] == 4) hmStream.SSPrintf(HtmlTagStreamMode, "option selected=\"selected\" value=4>position-Hard PID</option");
				else hmStream.SSPrintf(HtmlTagStreamMode, "option value=4>position-Hard PID</option");
				if (timewindows_dropdown_vector[5] == 5) hmStream.SSPrintf(HtmlTagStreamMode, "option selected=\"selected\" value=5>position-Auto PID</option");
				else hmStream.SSPrintf(HtmlTagStreamMode, "option value=5>position-Auto PID</option");
				if (timewindows_dropdown_vector[5] == 6) hmStream.SSPrintf(HtmlTagStreamMode, "option selected=\"selected\" value=6>Integrated control*</option");
				else hmStream.SSPrintf(HtmlTagStreamMode, "option value=6>Integrated control*</option");
				if (timewindows_dropdown_vector[5] == 7) hmStream.SSPrintf(HtmlTagStreamMode, "option selected=\"selected\" value=7>Invert*</option");
				else hmStream.SSPrintf(HtmlTagStreamMode, "option value=7>Invert*</option");
			hmStream.SSPrintf(HtmlTagStreamMode, "/select");
		hmStream.SSPrintf(HtmlTagStreamMode, "/div");

		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 606px; left: 94px;\"");
			hmStream.SSPrintf(HtmlTagStreamMode, "select NAME=\"form_timewindows_drop_6\""); 
				if (timewindows_dropdown_vector[6] == 0) hmStream.SSPrintf(HtmlTagStreamMode, "option selected=\"selected\" value=0 >off</option");
				else hmStream.SSPrintf(HtmlTagStreamMode, "option value=0>off</option");
				if (timewindows_dropdown_vector[6] == 1) hmStream.SSPrintf(HtmlTagStreamMode, "option selected=\"selected\" value=1>current control</option");
				else hmStream.SSPrintf(HtmlTagStreamMode, "option value=1>current control</option");
				if (timewindows_dropdown_vector[6] == 2) hmStream.SSPrintf(HtmlTagStreamMode, "option selected=\"selected\" value=2>Ip control-Soft PID</option");
				else hmStream.SSPrintf(HtmlTagStreamMode, "option value=2>Ip control-Soft PID</option");
				if (timewindows_dropdown_vector[6] == 3) hmStream.SSPrintf(HtmlTagStreamMode, "option selected=\"selected\" value=3>Ip control-Med. PID</option");
				else hmStream.SSPrintf(HtmlTagStreamMode, "option value=3>Ip control-Med. PID</option");
				if (timewindows_dropdown_vector[6] == 4) hmStream.SSPrintf(HtmlTagStreamMode, "option selected=\"selected\" value=4>Ip control-Hard PID</option");
				else hmStream.SSPrintf(HtmlTagStreamMode, "option value=4>Ip control-Hard PID</option");
				if (timewindows_dropdown_vector[6] == 5) hmStream.SSPrintf(HtmlTagStreamMode, "option selected=\"selected\" value=5>Ip control-Auto PID</option");
				else hmStream.SSPrintf(HtmlTagStreamMode, "option value=5>Ip control-Auto PID</option");
				if (timewindows_dropdown_vector[6] == 6) hmStream.SSPrintf(HtmlTagStreamMode, "option selected=\"selected\" value=6>Integrated control*</option");
				else hmStream.SSPrintf(HtmlTagStreamMode, "option value=6>Integrated control*</option");
				if (timewindows_dropdown_vector[6] == 7) hmStream.SSPrintf(HtmlTagStreamMode, "option selected=\"selected\" value=7>Invert*</option");
				else hmStream.SSPrintf(HtmlTagStreamMode, "option value=7>Invert*</option");
			hmStream.SSPrintf(HtmlTagStreamMode, "/select");
		hmStream.SSPrintf(HtmlTagStreamMode, "/div");

		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 606px; left: 236px;\"");
			hmStream.SSPrintf(HtmlTagStreamMode, "select NAME=\"form_timewindows_drop_7\""); 
				if (timewindows_dropdown_vector[7] == 0) hmStream.SSPrintf(HtmlTagStreamMode, "option selected=\"selected\" value=0 >off</option");
				else hmStream.SSPrintf(HtmlTagStreamMode, "option value=0>off</option");
				if (timewindows_dropdown_vector[7] == 1) hmStream.SSPrintf(HtmlTagStreamMode, "option selected=\"selected\" value=1>current control</option");
				else hmStream.SSPrintf(HtmlTagStreamMode, "option value=1>current control</option");
				if (timewindows_dropdown_vector[7] == 2) hmStream.SSPrintf(HtmlTagStreamMode, "option selected=\"selected\" value=2>position-Soft PID</option");
				else hmStream.SSPrintf(HtmlTagStreamMode, "option value=2>position-Soft PID</option");
				if (timewindows_dropdown_vector[7] == 3) hmStream.SSPrintf(HtmlTagStreamMode, "option selected=\"selected\" value=3>position-Med. PID</option");
				else hmStream.SSPrintf(HtmlTagStreamMode, "option value=3>position-Med. PID</option");
				if (timewindows_dropdown_vector[7] == 4) hmStream.SSPrintf(HtmlTagStreamMode, "option selected=\"selected\" value=4>position-Hard PID</option");
				else hmStream.SSPrintf(HtmlTagStreamMode, "option value=4>position-Hard PID</option");
				if (timewindows_dropdown_vector[7] == 5) hmStream.SSPrintf(HtmlTagStreamMode, "option selected=\"selected\" value=5>position-Auto PID</option");
				else hmStream.SSPrintf(HtmlTagStreamMode, "option value=5>position-Auto PID</option");
				if (timewindows_dropdown_vector[7] == 6) hmStream.SSPrintf(HtmlTagStreamMode, "option selected=\"selected\" value=6>Integrated control*</option");
				else hmStream.SSPrintf(HtmlTagStreamMode, "option value=6>Integrated control*</option");
				if (timewindows_dropdown_vector[7] == 7) hmStream.SSPrintf(HtmlTagStreamMode, "option selected=\"selected\" value=7>Invert*</option");
				else hmStream.SSPrintf(HtmlTagStreamMode, "option value=7>Invert*</option");
			hmStream.SSPrintf(HtmlTagStreamMode, "/select");
		hmStream.SSPrintf(HtmlTagStreamMode, "/div");

		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 606px; left: 370px;\"");
			hmStream.SSPrintf(HtmlTagStreamMode, "select NAME=\"form_timewindows_drop_8\""); 
				if (timewindows_dropdown_vector[8] == 0) hmStream.SSPrintf(HtmlTagStreamMode, "option selected=\"selected\" value=0 >off</option");
				else hmStream.SSPrintf(HtmlTagStreamMode, "option value=0>off</option");
				if (timewindows_dropdown_vector[8] == 1) hmStream.SSPrintf(HtmlTagStreamMode, "option selected=\"selected\" value=1>current control</option");
				else hmStream.SSPrintf(HtmlTagStreamMode, "option value=1>current control</option");
				if (timewindows_dropdown_vector[8] == 2) hmStream.SSPrintf(HtmlTagStreamMode, "option selected=\"selected\" value=2>position-Soft PID</option");
				else hmStream.SSPrintf(HtmlTagStreamMode, "option value=2>position-Soft PID</option");
				if (timewindows_dropdown_vector[8] == 3) hmStream.SSPrintf(HtmlTagStreamMode, "option selected=\"selected\" value=3>position-Med. PID</option");
				else hmStream.SSPrintf(HtmlTagStreamMode, "option value=3>position-Med. PID</option");
				if (timewindows_dropdown_vector[8] == 4) hmStream.SSPrintf(HtmlTagStreamMode, "option selected=\"selected\" value=4>position-Hard PID</option");
				else hmStream.SSPrintf(HtmlTagStreamMode, "option value=4>position-Hard PID</option");
				if (timewindows_dropdown_vector[8] == 5) hmStream.SSPrintf(HtmlTagStreamMode, "option selected=\"selected\" value=5>position-Auto PID</option");
				else hmStream.SSPrintf(HtmlTagStreamMode, "option value=5>position-Auto PID</option");
				if (timewindows_dropdown_vector[8] == 6) hmStream.SSPrintf(HtmlTagStreamMode, "option selected=\"selected\" value=6>Integrated control*</option");
				else hmStream.SSPrintf(HtmlTagStreamMode, "option value=6>Integrated control*</option");
				if (timewindows_dropdown_vector[8] == 7) hmStream.SSPrintf(HtmlTagStreamMode, "option selected=\"selected\" value=7>Invert*</option");
				else hmStream.SSPrintf(HtmlTagStreamMode, "option value=7>Invert*</option");
			hmStream.SSPrintf(HtmlTagStreamMode, "/select");
		hmStream.SSPrintf(HtmlTagStreamMode, "/div");
	
		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 628px; left: 94px;\""); 
			hmStream.SSPrintf(HtmlTagStreamMode, "select NAME=\"form_timewindows_drop_9\""); 
				if (timewindows_dropdown_vector[9] == 0) hmStream.SSPrintf(HtmlTagStreamMode, "option selected=\"selected\" value=0 >off</option");
				else hmStream.SSPrintf(HtmlTagStreamMode, "option value=0>off</option");
				if (timewindows_dropdown_vector[9] == 1) hmStream.SSPrintf(HtmlTagStreamMode, "option selected=\"selected\" value=1>current control</option");
				else hmStream.SSPrintf(HtmlTagStreamMode, "option value=1>current control</option");
				if (timewindows_dropdown_vector[9] == 2) hmStream.SSPrintf(HtmlTagStreamMode, "option selected=\"selected\" value=2>Ip control-Soft PID</option");
				else hmStream.SSPrintf(HtmlTagStreamMode, "option value=2>Ip control-Soft PID</option");
				if (timewindows_dropdown_vector[9] == 3) hmStream.SSPrintf(HtmlTagStreamMode, "option selected=\"selected\" value=3>Ip control-Med. PID</option");
				else hmStream.SSPrintf(HtmlTagStreamMode, "option value=3>Ip control-Med. PID</option");
				if (timewindows_dropdown_vector[9] == 4) hmStream.SSPrintf(HtmlTagStreamMode, "option selected=\"selected\" value=4>Ip control-Hard PID</option");
				else hmStream.SSPrintf(HtmlTagStreamMode, "option value=4>Ip control-Hard PID</option");
				if (timewindows_dropdown_vector[9] == 5) hmStream.SSPrintf(HtmlTagStreamMode, "option selected=\"selected\" value=5>Ip control-Auto PID</option");
				else hmStream.SSPrintf(HtmlTagStreamMode, "option value=5>Ip control-Auto PID</option");
				if (timewindows_dropdown_vector[9] == 6) hmStream.SSPrintf(HtmlTagStreamMode, "option selected=\"selected\" value=6>Integrated control*</option");
				else hmStream.SSPrintf(HtmlTagStreamMode, "option value=6>Integrated control*</option");
				if (timewindows_dropdown_vector[9] == 7) hmStream.SSPrintf(HtmlTagStreamMode, "option selected=\"selected\" value=7>Invert*</option");
				else hmStream.SSPrintf(HtmlTagStreamMode, "option value=7>Invert*</option");
			hmStream.SSPrintf(HtmlTagStreamMode, "/select");
		hmStream.SSPrintf(HtmlTagStreamMode, "/div");

		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 628px; left: 236px;\"");
			hmStream.SSPrintf(HtmlTagStreamMode, "select NAME=\"form_timewindows_drop_10\""); 
				if (timewindows_dropdown_vector[10] == 0) hmStream.SSPrintf(HtmlTagStreamMode, "option selected=\"selected\" value=0 >off</option");
				else hmStream.SSPrintf(HtmlTagStreamMode, "option value=0>off</option");
				if (timewindows_dropdown_vector[10] == 1) hmStream.SSPrintf(HtmlTagStreamMode, "option selected=\"selected\" value=1>current control</option");
				else hmStream.SSPrintf(HtmlTagStreamMode, "option value=1>current control</option");
				if (timewindows_dropdown_vector[10] == 2) hmStream.SSPrintf(HtmlTagStreamMode, "option selected=\"selected\" value=2>position-Soft PID</option");
				else hmStream.SSPrintf(HtmlTagStreamMode, "option value=2>position-Soft PID</option");
				if (timewindows_dropdown_vector[10] == 3) hmStream.SSPrintf(HtmlTagStreamMode, "option selected=\"selected\" value=3>position-Med. PID</option");
				else hmStream.SSPrintf(HtmlTagStreamMode, "option value=3>position-Med. PID</option");
				if (timewindows_dropdown_vector[10] == 4) hmStream.SSPrintf(HtmlTagStreamMode, "option selected=\"selected\" value=4>position-Hard PID</option");
				else hmStream.SSPrintf(HtmlTagStreamMode, "option value=4>position-Hard PID</option");
				if (timewindows_dropdown_vector[10] == 5) hmStream.SSPrintf(HtmlTagStreamMode, "option selected=\"selected\" value=5>position-Auto PID</option");
				else hmStream.SSPrintf(HtmlTagStreamMode, "option value=5>position-Auto PID</option");
				if (timewindows_dropdown_vector[10] == 6) hmStream.SSPrintf(HtmlTagStreamMode, "option selected=\"selected\" value=6>Integrated control*</option");
				else hmStream.SSPrintf(HtmlTagStreamMode, "option value=6>Integrated control*</option");
				if (timewindows_dropdown_vector[10] == 7) hmStream.SSPrintf(HtmlTagStreamMode, "option selected=\"selected\" value=7>Invert*</option");
				else hmStream.SSPrintf(HtmlTagStreamMode, "option value=7>Invert*</option");
			hmStream.SSPrintf(HtmlTagStreamMode, "/select");
		hmStream.SSPrintf(HtmlTagStreamMode, "/div");

		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 628px; left: 370px;\"");
			hmStream.SSPrintf(HtmlTagStreamMode, "select NAME=\"form_timewindows_drop_11\""); 
				if (timewindows_dropdown_vector[11] == 0) hmStream.SSPrintf(HtmlTagStreamMode, "option selected=\"selected\" value=0 >off</option");
				else hmStream.SSPrintf(HtmlTagStreamMode, "option value=0>off</option");
				if (timewindows_dropdown_vector[11] == 1) hmStream.SSPrintf(HtmlTagStreamMode, "option selected=\"selected\" value=1>current control</option");
				else hmStream.SSPrintf(HtmlTagStreamMode, "option value=1>current control</option");
				if (timewindows_dropdown_vector[11] == 2) hmStream.SSPrintf(HtmlTagStreamMode, "option selected=\"selected\" value=2>position-Soft PID</option");
				else hmStream.SSPrintf(HtmlTagStreamMode, "option value=2>position-Soft PID</option");
				if (timewindows_dropdown_vector[11] == 3) hmStream.SSPrintf(HtmlTagStreamMode, "option selected=\"selected\" value=3>position-Med. PID</option");
				else hmStream.SSPrintf(HtmlTagStreamMode, "option value=3>position-Med. PID</option");
				if (timewindows_dropdown_vector[11] == 4) hmStream.SSPrintf(HtmlTagStreamMode, "option selected=\"selected\" value=4>position-Hard PID</option");
				else hmStream.SSPrintf(HtmlTagStreamMode, "option value=4>position-Hard PID</option");
				if (timewindows_dropdown_vector[11] == 5) hmStream.SSPrintf(HtmlTagStreamMode, "option selected=\"selected\" value=5>position-Auto PID</option");
				else hmStream.SSPrintf(HtmlTagStreamMode, "option value=5>position-Auto PID</option");
				if (timewindows_dropdown_vector[11] == 6) hmStream.SSPrintf(HtmlTagStreamMode, "option selected=\"selected\" value=6>Integrated control*</option");
				else hmStream.SSPrintf(HtmlTagStreamMode, "option value=6>Integrated control*</option");
				if (timewindows_dropdown_vector[11] == 7) hmStream.SSPrintf(HtmlTagStreamMode, "option selected=\"selected\" value=7>Invert*</option");
				else hmStream.SSPrintf(HtmlTagStreamMode, "option value=7>Invert*</option");
			hmStream.SSPrintf(HtmlTagStreamMode, "/select");
		hmStream.SSPrintf(HtmlTagStreamMode, "/div");

		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 650px; left: 94px;\""); 
			hmStream.SSPrintf(HtmlTagStreamMode, "select NAME=\"form_timewindows_drop_12\""); 
				if (timewindows_dropdown_vector[12] == 0) hmStream.SSPrintf(HtmlTagStreamMode, "option selected=\"selected\" value=0 >off</option");
				else hmStream.SSPrintf(HtmlTagStreamMode, "option value=0>off</option");
				if (timewindows_dropdown_vector[12] == 1) hmStream.SSPrintf(HtmlTagStreamMode, "option selected=\"selected\" value=1>current control</option");
				else hmStream.SSPrintf(HtmlTagStreamMode, "option value=1>current control</option");
				if (timewindows_dropdown_vector[12] == 2) hmStream.SSPrintf(HtmlTagStreamMode, "option selected=\"selected\" value=2>Ip control-Soft PID</option");
				else hmStream.SSPrintf(HtmlTagStreamMode, "option value=2>Ip control-Soft PID</option");
				if (timewindows_dropdown_vector[12] == 3) hmStream.SSPrintf(HtmlTagStreamMode, "option selected=\"selected\" value=3>Ip control-Med. PID</option");
				else hmStream.SSPrintf(HtmlTagStreamMode, "option value=3>Ip control-Med. PID</option");
				if (timewindows_dropdown_vector[12] == 4) hmStream.SSPrintf(HtmlTagStreamMode, "option selected=\"selected\" value=4>Ip control-Hard PID</option");
				else hmStream.SSPrintf(HtmlTagStreamMode, "option value=4>Ip control-Hard PID</option");
				if (timewindows_dropdown_vector[12] == 5) hmStream.SSPrintf(HtmlTagStreamMode, "option selected=\"selected\" value=5>Ip control-Auto PID</option");
				else hmStream.SSPrintf(HtmlTagStreamMode, "option value=5>Ip control-Auto PID</option");
				if (timewindows_dropdown_vector[12] == 6) hmStream.SSPrintf(HtmlTagStreamMode, "option selected=\"selected\" value=6>Integrated control*</option");
				else hmStream.SSPrintf(HtmlTagStreamMode, "option value=6>Integrated control*</option");
				if (timewindows_dropdown_vector[12] == 7) hmStream.SSPrintf(HtmlTagStreamMode, "option selected=\"selected\" value=7>Invert*</option");
				else hmStream.SSPrintf(HtmlTagStreamMode, "option value=7>Invert*</option");
			hmStream.SSPrintf(HtmlTagStreamMode, "/select");
		hmStream.SSPrintf(HtmlTagStreamMode, "/div");

		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 650px; left: 236px;\"");
			hmStream.SSPrintf(HtmlTagStreamMode, "select NAME=\"form_timewindows_drop_13\""); 
				if (timewindows_dropdown_vector[13] == 0) hmStream.SSPrintf(HtmlTagStreamMode, "option selected=\"selected\" value=0 >off</option");
				else hmStream.SSPrintf(HtmlTagStreamMode, "option value=0>off</option");
				if (timewindows_dropdown_vector[13] == 1) hmStream.SSPrintf(HtmlTagStreamMode, "option selected=\"selected\" value=1>current control</option");
				else hmStream.SSPrintf(HtmlTagStreamMode, "option value=1>current control</option");
				if (timewindows_dropdown_vector[13] == 2) hmStream.SSPrintf(HtmlTagStreamMode, "option selected=\"selected\" value=2>position-Soft PID</option");
				else hmStream.SSPrintf(HtmlTagStreamMode, "option value=2>position-Soft PID</option");
				if (timewindows_dropdown_vector[13] == 3) hmStream.SSPrintf(HtmlTagStreamMode, "option selected=\"selected\" value=3>position-Med. PID</option");
				else hmStream.SSPrintf(HtmlTagStreamMode, "option value=3>position-Med. PID</option");
				if (timewindows_dropdown_vector[13] == 4) hmStream.SSPrintf(HtmlTagStreamMode, "option selected=\"selected\" value=4>position-Hard PID</option");
				else hmStream.SSPrintf(HtmlTagStreamMode, "option value=4>position-Hard PID</option");
				if (timewindows_dropdown_vector[13] == 5) hmStream.SSPrintf(HtmlTagStreamMode, "option selected=\"selected\" value=5>position-Auto PID</option");
				else hmStream.SSPrintf(HtmlTagStreamMode, "option value=5>position-Auto PID</option");
				if (timewindows_dropdown_vector[13] == 6) hmStream.SSPrintf(HtmlTagStreamMode, "option selected=\"selected\" value=6>Integrated control*</option");
				else hmStream.SSPrintf(HtmlTagStreamMode, "option value=6>Integrated control*</option");
				if (timewindows_dropdown_vector[13] == 7) hmStream.SSPrintf(HtmlTagStreamMode, "option selected=\"selected\" value=7>Invert*</option");
				else hmStream.SSPrintf(HtmlTagStreamMode, "option value=7>Invert*</option");
			hmStream.SSPrintf(HtmlTagStreamMode, "/select");
		hmStream.SSPrintf(HtmlTagStreamMode, "/div");

		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 650px; left: 370px;\"");
			hmStream.SSPrintf(HtmlTagStreamMode, "select NAME=\"form_timewindows_drop_14\""); 
				if (timewindows_dropdown_vector[14] == 0) hmStream.SSPrintf(HtmlTagStreamMode, "option selected=\"selected\" value=0 >off</option");
				else hmStream.SSPrintf(HtmlTagStreamMode, "option value=0>off</option");
				if (timewindows_dropdown_vector[14] == 1) hmStream.SSPrintf(HtmlTagStreamMode, "option selected=\"selected\" value=1>current control</option");
				else hmStream.SSPrintf(HtmlTagStreamMode, "option value=1>current control</option");
				if (timewindows_dropdown_vector[14] == 2) hmStream.SSPrintf(HtmlTagStreamMode, "option selected=\"selected\" value=2>position-Soft PID</option");
				else hmStream.SSPrintf(HtmlTagStreamMode, "option value=2>position-Soft PID</option");
				if (timewindows_dropdown_vector[14] == 3) hmStream.SSPrintf(HtmlTagStreamMode, "option selected=\"selected\" value=3>position-Med. PID</option");
				else hmStream.SSPrintf(HtmlTagStreamMode, "option value=3>position-Med. PID</option");
				if (timewindows_dropdown_vector[14] == 4) hmStream.SSPrintf(HtmlTagStreamMode, "option selected=\"selected\" value=4>position-Hard PID</option");
				else hmStream.SSPrintf(HtmlTagStreamMode, "option value=4>position-Hard PID</option");
				if (timewindows_dropdown_vector[14] == 5) hmStream.SSPrintf(HtmlTagStreamMode, "option selected=\"selected\" value=5>position-Auto PID</option");
				else hmStream.SSPrintf(HtmlTagStreamMode, "option value=5>position-Auto PID</option");
				if (timewindows_dropdown_vector[14] == 6) hmStream.SSPrintf(HtmlTagStreamMode, "option selected=\"selected\" value=6>Integrated control*</option");
				else hmStream.SSPrintf(HtmlTagStreamMode, "option value=6>Integrated control*</option");
				if (timewindows_dropdown_vector[14] == 7) hmStream.SSPrintf(HtmlTagStreamMode, "option selected=\"selected\" value=7>Invert*</option");
				else hmStream.SSPrintf(HtmlTagStreamMode, "option value=7>Invert*</option");
			hmStream.SSPrintf(HtmlTagStreamMode, "/select");
		hmStream.SSPrintf(HtmlTagStreamMode, "/div");

		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 672px; left: 94px;\""); 
			hmStream.SSPrintf(HtmlTagStreamMode, "select NAME=\"form_timewindows_drop_15\""); 
				if (timewindows_dropdown_vector[15] == 0) hmStream.SSPrintf(HtmlTagStreamMode, "option selected=\"selected\" value=0 >off</option");
				else hmStream.SSPrintf(HtmlTagStreamMode, "option value=0>off</option");
				if (timewindows_dropdown_vector[15] == 1) hmStream.SSPrintf(HtmlTagStreamMode, "option selected=\"selected\" value=1>current control</option");
				else hmStream.SSPrintf(HtmlTagStreamMode, "option value=1>current control</option");
				if (timewindows_dropdown_vector[15] == 2) hmStream.SSPrintf(HtmlTagStreamMode, "option selected=\"selected\" value=2>Ip control-Soft PID</option");
				else hmStream.SSPrintf(HtmlTagStreamMode, "option value=2>Ip control-Soft PID</option");
				if (timewindows_dropdown_vector[15] == 3) hmStream.SSPrintf(HtmlTagStreamMode, "option selected=\"selected\" value=3>Ip control-Med. PID</option");
				else hmStream.SSPrintf(HtmlTagStreamMode, "option value=3>Ip control-Med. PID</option");
				if (timewindows_dropdown_vector[15] == 4) hmStream.SSPrintf(HtmlTagStreamMode, "option selected=\"selected\" value=4>Ip control-Hard PID</option");
				else hmStream.SSPrintf(HtmlTagStreamMode, "option value=4>Ip control-Hard PID</option");
				if (timewindows_dropdown_vector[15] == 5) hmStream.SSPrintf(HtmlTagStreamMode, "option selected=\"selected\" value=5>Ip control-Auto PID</option");
				else hmStream.SSPrintf(HtmlTagStreamMode, "option value=5>Ip control-Auto PID</option");
				if (timewindows_dropdown_vector[15] == 6) hmStream.SSPrintf(HtmlTagStreamMode, "option selected=\"selected\" value=6>Integrated control*</option");
				else hmStream.SSPrintf(HtmlTagStreamMode, "option value=6>Integrated control*</option");
				if (timewindows_dropdown_vector[15] == 7) hmStream.SSPrintf(HtmlTagStreamMode, "option selected=\"selected\" value=7>Invert*</option");
				else hmStream.SSPrintf(HtmlTagStreamMode, "option value=7>Invert*</option");
			hmStream.SSPrintf(HtmlTagStreamMode, "/select");
		hmStream.SSPrintf(HtmlTagStreamMode, "/div");

		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 672px; left: 236px;\"");
			hmStream.SSPrintf(HtmlTagStreamMode, "select NAME=\"form_timewindows_drop_16\""); 
				if (timewindows_dropdown_vector[16] == 0) hmStream.SSPrintf(HtmlTagStreamMode, "option selected=\"selected\" value=0 >off</option");
				else hmStream.SSPrintf(HtmlTagStreamMode, "option value=0>off</option");
				if (timewindows_dropdown_vector[16] == 1) hmStream.SSPrintf(HtmlTagStreamMode, "option selected=\"selected\" value=1>current control</option");
				else hmStream.SSPrintf(HtmlTagStreamMode, "option value=1>current control</option");
				if (timewindows_dropdown_vector[16] == 2) hmStream.SSPrintf(HtmlTagStreamMode, "option selected=\"selected\" value=2>position-Soft PID</option");
				else hmStream.SSPrintf(HtmlTagStreamMode, "option value=2>position-Soft PID</option");
				if (timewindows_dropdown_vector[16] == 3) hmStream.SSPrintf(HtmlTagStreamMode, "option selected=\"selected\" value=3>position-Med. PID</option");
				else hmStream.SSPrintf(HtmlTagStreamMode, "option value=3>position-Med. PID</option");
				if (timewindows_dropdown_vector[16] == 4) hmStream.SSPrintf(HtmlTagStreamMode, "option selected=\"selected\" value=4>position-Hard PID</option");
				else hmStream.SSPrintf(HtmlTagStreamMode, "option value=4>position-Hard PID</option");
				if (timewindows_dropdown_vector[16] == 5) hmStream.SSPrintf(HtmlTagStreamMode, "option selected=\"selected\" value=5>position-Auto PID</option");
				else hmStream.SSPrintf(HtmlTagStreamMode, "option value=5>position-Auto PID</option");
				if (timewindows_dropdown_vector[16] == 6) hmStream.SSPrintf(HtmlTagStreamMode, "option selected=\"selected\" value=6>Integrated control*</option");
				else hmStream.SSPrintf(HtmlTagStreamMode, "option value=6>Integrated control*</option");
				if (timewindows_dropdown_vector[16] == 7) hmStream.SSPrintf(HtmlTagStreamMode, "option selected=\"selected\" value=7>Invert*</option");
				else hmStream.SSPrintf(HtmlTagStreamMode, "option value=7>Invert*</option");
			hmStream.SSPrintf(HtmlTagStreamMode, "/select");
		hmStream.SSPrintf(HtmlTagStreamMode, "/div");

		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 672px; left: 370px;\"");
			hmStream.SSPrintf(HtmlTagStreamMode, "select NAME=\"form_timewindows_drop_17\""); 
				if (timewindows_dropdown_vector[17] == 0) hmStream.SSPrintf(HtmlTagStreamMode, "option selected=\"selected\" value=0 >off</option");
				else hmStream.SSPrintf(HtmlTagStreamMode, "option value=0>off</option");
				if (timewindows_dropdown_vector[17] == 1) hmStream.SSPrintf(HtmlTagStreamMode, "option selected=\"selected\" value=1>current control</option");
				else hmStream.SSPrintf(HtmlTagStreamMode, "option value=1>current control</option");
				if (timewindows_dropdown_vector[17] == 2) hmStream.SSPrintf(HtmlTagStreamMode, "option selected=\"selected\" value=2>position-Soft PID</option");
				else hmStream.SSPrintf(HtmlTagStreamMode, "option value=2>position-Soft PID</option");
				if (timewindows_dropdown_vector[17] == 3) hmStream.SSPrintf(HtmlTagStreamMode, "option selected=\"selected\" value=3>position-Med. PID</option");
				else hmStream.SSPrintf(HtmlTagStreamMode, "option value=3>position-Med. PID</option");
				if (timewindows_dropdown_vector[17] == 4) hmStream.SSPrintf(HtmlTagStreamMode, "option selected=\"selected\" value=4>position-Hard PID</option");
				else hmStream.SSPrintf(HtmlTagStreamMode, "option value=4>position-Hard PID</option");
				if (timewindows_dropdown_vector[17] == 5) hmStream.SSPrintf(HtmlTagStreamMode, "option selected=\"selected\" value=5>position-Auto PID</option");
				else hmStream.SSPrintf(HtmlTagStreamMode, "option value=5>position-Auto PID</option");
				if (timewindows_dropdown_vector[17] == 6) hmStream.SSPrintf(HtmlTagStreamMode, "option selected=\"selected\" value=6>Integrated control*</option");
				else hmStream.SSPrintf(HtmlTagStreamMode, "option value=6>Integrated control*</option");
				if (timewindows_dropdown_vector[17] == 7) hmStream.SSPrintf(HtmlTagStreamMode, "option selected=\"selected\" value=7>Invert*</option");
				else hmStream.SSPrintf(HtmlTagStreamMode, "option value=7>Invert*</option");
			hmStream.SSPrintf(HtmlTagStreamMode, "/select");
		hmStream.SSPrintf(HtmlTagStreamMode, "/div");

		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 694px; left: 94px;\""); 
			hmStream.SSPrintf(HtmlTagStreamMode, "select NAME=\"form_timewindows_drop_18\""); 
				if (timewindows_dropdown_vector[18] == 0) hmStream.SSPrintf(HtmlTagStreamMode, "option selected=\"selected\" value=0 >off</option");
				else hmStream.SSPrintf(HtmlTagStreamMode, "option value=0>off</option");
				if (timewindows_dropdown_vector[18] == 1) hmStream.SSPrintf(HtmlTagStreamMode, "option selected=\"selected\" value=1>current control</option");
				else hmStream.SSPrintf(HtmlTagStreamMode, "option value=1>current control</option");
				if (timewindows_dropdown_vector[18] == 2) hmStream.SSPrintf(HtmlTagStreamMode, "option selected=\"selected\" value=2>Ip control-Soft PID</option");
				else hmStream.SSPrintf(HtmlTagStreamMode, "option value=2>Ip control-Soft PID</option");
				if (timewindows_dropdown_vector[18] == 3) hmStream.SSPrintf(HtmlTagStreamMode, "option selected=\"selected\" value=3>Ip control-Med. PID</option");
				else hmStream.SSPrintf(HtmlTagStreamMode, "option value=3>Ip control-Med. PID</option");
				if (timewindows_dropdown_vector[18] == 4) hmStream.SSPrintf(HtmlTagStreamMode, "option selected=\"selected\" value=4>Ip control-Hard PID</option");
				else hmStream.SSPrintf(HtmlTagStreamMode, "option value=4>Ip control-Hard PID</option");
				if (timewindows_dropdown_vector[18] == 5) hmStream.SSPrintf(HtmlTagStreamMode, "option selected=\"selected\" value=5>Ip control-Auto PID</option");
				else hmStream.SSPrintf(HtmlTagStreamMode, "option value=5>Ip control-Auto PID</option");
				if (timewindows_dropdown_vector[18] == 6) hmStream.SSPrintf(HtmlTagStreamMode, "option selected=\"selected\" value=6>Integrated control*</option");
				else hmStream.SSPrintf(HtmlTagStreamMode, "option value=6>Integrated control*</option");
				if (timewindows_dropdown_vector[18] == 7) hmStream.SSPrintf(HtmlTagStreamMode, "option selected=\"selected\" value=7>Invert*</option");
				else hmStream.SSPrintf(HtmlTagStreamMode, "option value=7>Invert*</option");
			hmStream.SSPrintf(HtmlTagStreamMode, "/select");
		hmStream.SSPrintf(HtmlTagStreamMode, "/div");

		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 694px; left: 236px;\"");
			hmStream.SSPrintf(HtmlTagStreamMode, "select NAME=\"form_timewindows_drop_19\""); 
				if (timewindows_dropdown_vector[19] == 0) hmStream.SSPrintf(HtmlTagStreamMode, "option selected=\"selected\" value=0 >off</option");
				else hmStream.SSPrintf(HtmlTagStreamMode, "option value=0>off</option");
				if (timewindows_dropdown_vector[19] == 1) hmStream.SSPrintf(HtmlTagStreamMode, "option selected=\"selected\" value=1>current control</option");
				else hmStream.SSPrintf(HtmlTagStreamMode, "option value=1>current control</option");
				if (timewindows_dropdown_vector[19] == 2) hmStream.SSPrintf(HtmlTagStreamMode, "option selected=\"selected\" value=2>position-Soft PID</option");
				else hmStream.SSPrintf(HtmlTagStreamMode, "option value=2>position-Soft PID</option");
				if (timewindows_dropdown_vector[19] == 3) hmStream.SSPrintf(HtmlTagStreamMode, "option selected=\"selected\" value=3>position-Med. PID</option");
				else hmStream.SSPrintf(HtmlTagStreamMode, "option value=3>position-Med. PID</option");
				if (timewindows_dropdown_vector[19] == 4) hmStream.SSPrintf(HtmlTagStreamMode, "option selected=\"selected\" value=4>position-Hard PID</option");
				else hmStream.SSPrintf(HtmlTagStreamMode, "option value=4>position-Hard PID</option");
				if (timewindows_dropdown_vector[19] == 5) hmStream.SSPrintf(HtmlTagStreamMode, "option selected=\"selected\" value=5>position-Auto PID</option");
				else hmStream.SSPrintf(HtmlTagStreamMode, "option value=5>position-Auto PID</option");
				if (timewindows_dropdown_vector[19] == 6) hmStream.SSPrintf(HtmlTagStreamMode, "option selected=\"selected\" value=6>Integrated control*</option");
				else hmStream.SSPrintf(HtmlTagStreamMode, "option value=6>Integrated control*</option");
				if (timewindows_dropdown_vector[19] == 7) hmStream.SSPrintf(HtmlTagStreamMode, "option selected=\"selected\" value=7>Invert*</option");
				else hmStream.SSPrintf(HtmlTagStreamMode, "option value=7>Invert*</option");
			hmStream.SSPrintf(HtmlTagStreamMode, "/select");
		hmStream.SSPrintf(HtmlTagStreamMode, "/div");

		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 694px; left: 370px;\"");
			hmStream.SSPrintf(HtmlTagStreamMode, "select NAME=\"form_timewindows_drop_20\""); 
				if (timewindows_dropdown_vector[20] == 0) hmStream.SSPrintf(HtmlTagStreamMode, "option selected=\"selected\" value=0 >off</option");
				else hmStream.SSPrintf(HtmlTagStreamMode, "option value=0>off</option");
				if (timewindows_dropdown_vector[20] == 1) hmStream.SSPrintf(HtmlTagStreamMode, "option selected=\"selected\" value=1>current control</option");
				else hmStream.SSPrintf(HtmlTagStreamMode, "option value=1>current control</option");
				if (timewindows_dropdown_vector[20] == 2) hmStream.SSPrintf(HtmlTagStreamMode, "option selected=\"selected\" value=2>position-Soft PID</option");
				else hmStream.SSPrintf(HtmlTagStreamMode, "option value=2>position-Soft PID</option");
				if (timewindows_dropdown_vector[20] == 3) hmStream.SSPrintf(HtmlTagStreamMode, "option selected=\"selected\" value=3>position-Med. PID</option");
				else hmStream.SSPrintf(HtmlTagStreamMode, "option value=3>position-Med. PID</option");
				if (timewindows_dropdown_vector[20] == 4) hmStream.SSPrintf(HtmlTagStreamMode, "option selected=\"selected\" value=4>position-Hard PID</option");
				else hmStream.SSPrintf(HtmlTagStreamMode, "option value=4>position-Hard PID</option");
				if (timewindows_dropdown_vector[20] == 5) hmStream.SSPrintf(HtmlTagStreamMode, "option selected=\"selected\" value=5>position-Auto PID</option");
				else hmStream.SSPrintf(HtmlTagStreamMode, "option value=5>position-Auto PID</option");
				if (timewindows_dropdown_vector[20] == 6) hmStream.SSPrintf(HtmlTagStreamMode, "option selected=\"selected\" value=6>Integrated control*</option");
				else hmStream.SSPrintf(HtmlTagStreamMode, "option value=6>Integrated control*</option");
				if (timewindows_dropdown_vector[20] == 7) hmStream.SSPrintf(HtmlTagStreamMode, "option selected=\"selected\" value=7>Invert*</option");
				else hmStream.SSPrintf(HtmlTagStreamMode, "option value=7>Invert*</option");
			hmStream.SSPrintf(HtmlTagStreamMode, "/select");
		hmStream.SSPrintf(HtmlTagStreamMode, "/div");
	
		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 762px; left: 94px;\""); 
			hmStream.SSPrintf(HtmlTagStreamMode, "select NAME=\"form_timewindows_drop_21\""); 
				if (timewindows_dropdown_vector[21] == 0) hmStream.SSPrintf(HtmlTagStreamMode, "option selected=\"selected\" value=0 >off</option");
				else hmStream.SSPrintf(HtmlTagStreamMode, "option value=0>off</option");
				if (timewindows_dropdown_vector[21] == 1) hmStream.SSPrintf(HtmlTagStreamMode, "option selected=\"selected\" value=1>current control</option");
				else hmStream.SSPrintf(HtmlTagStreamMode, "option value=1>current control</option");
				if (timewindows_dropdown_vector[21] == 2) hmStream.SSPrintf(HtmlTagStreamMode, "option selected=\"selected\" value=2>Ip control-Soft PID</option");
				else hmStream.SSPrintf(HtmlTagStreamMode, "option value=2>Ip control-Soft PID</option");
				if (timewindows_dropdown_vector[21] == 3) hmStream.SSPrintf(HtmlTagStreamMode, "option selected=\"selected\" value=3>Ip control-Med. PID</option");
				else hmStream.SSPrintf(HtmlTagStreamMode, "option value=3>Ip control-Med. PID</option");
				if (timewindows_dropdown_vector[21] == 4) hmStream.SSPrintf(HtmlTagStreamMode, "option selected=\"selected\" value=4>Ip control-Hard PID</option");
				else hmStream.SSPrintf(HtmlTagStreamMode, "option value=4>Ip control-Hard PID</option");
				if (timewindows_dropdown_vector[21] == 5) hmStream.SSPrintf(HtmlTagStreamMode, "option selected=\"selected\" value=5>Ip control-Auto PID</option");
				else hmStream.SSPrintf(HtmlTagStreamMode, "option value=5>Ip control-Auto PID</option");
				if (timewindows_dropdown_vector[21] == 6) hmStream.SSPrintf(HtmlTagStreamMode, "option selected=\"selected\" value=6>Integrated control*</option");
				else hmStream.SSPrintf(HtmlTagStreamMode, "option value=6>Integrated control*</option");
				if (timewindows_dropdown_vector[21] == 7) hmStream.SSPrintf(HtmlTagStreamMode, "option selected=\"selected\" value=7>Invert*</option");
				else hmStream.SSPrintf(HtmlTagStreamMode, "option value=7>Invert*</option");
			hmStream.SSPrintf(HtmlTagStreamMode, "/select");												
		hmStream.SSPrintf(HtmlTagStreamMode, "/div");

		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 762px; left: 236px;\"");
			hmStream.SSPrintf(HtmlTagStreamMode, "select NAME=\"form_timewindows_drop_22\""); 
				if (timewindows_dropdown_vector[22] == 0) hmStream.SSPrintf(HtmlTagStreamMode, "option selected=\"selected\" value=0 >off</option");
				else hmStream.SSPrintf(HtmlTagStreamMode, "option value=0>off</option");
				if (timewindows_dropdown_vector[22] == 1) hmStream.SSPrintf(HtmlTagStreamMode, "option selected=\"selected\" value=1>current control</option");
				else hmStream.SSPrintf(HtmlTagStreamMode, "option value=1>current control</option");
				if (timewindows_dropdown_vector[22] == 2) hmStream.SSPrintf(HtmlTagStreamMode, "option selected=\"selected\" value=2>position-Soft PID</option");
				else hmStream.SSPrintf(HtmlTagStreamMode, "option value=2>position-Soft PID</option");
				if (timewindows_dropdown_vector[22] == 3) hmStream.SSPrintf(HtmlTagStreamMode, "option selected=\"selected\" value=3>position-Med. PID</option");
				else hmStream.SSPrintf(HtmlTagStreamMode, "option value=3>position-Med. PID</option");
				if (timewindows_dropdown_vector[22] == 4) hmStream.SSPrintf(HtmlTagStreamMode, "option selected=\"selected\" value=4>position-Hard PID</option");
				else hmStream.SSPrintf(HtmlTagStreamMode, "option value=4>position-Hard PID</option");
				if (timewindows_dropdown_vector[22] == 5) hmStream.SSPrintf(HtmlTagStreamMode, "option selected=\"selected\" value=5>position-Auto PID</option");
				else hmStream.SSPrintf(HtmlTagStreamMode, "option value=5>position-Auto PID</option");
				if (timewindows_dropdown_vector[22] == 6) hmStream.SSPrintf(HtmlTagStreamMode, "option selected=\"selected\" value=6>Integrated control*</option");
				else hmStream.SSPrintf(HtmlTagStreamMode, "option value=6>Integrated control*</option");
				if (timewindows_dropdown_vector[22] == 7) hmStream.SSPrintf(HtmlTagStreamMode, "option selected=\"selected\" value=7>Invert*</option");
				else hmStream.SSPrintf(HtmlTagStreamMode, "option value=7>Invert*</option");
			hmStream.SSPrintf(HtmlTagStreamMode, "/select");												
		hmStream.SSPrintf(HtmlTagStreamMode, "/div");

		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 762px; left: 370px;\"");
			hmStream.SSPrintf(HtmlTagStreamMode, "select NAME=\"form_timewindows_drop_23\""); 
				if (timewindows_dropdown_vector[23] == 0) hmStream.SSPrintf(HtmlTagStreamMode, "option selected=\"selected\" value=0 >off</option");
				else hmStream.SSPrintf(HtmlTagStreamMode, "option value=0>off</option");
				if (timewindows_dropdown_vector[23] == 1) hmStream.SSPrintf(HtmlTagStreamMode, "option selected=\"selected\" value=1>current control</option");
				else hmStream.SSPrintf(HtmlTagStreamMode, "option value=1>current control</option");
				if (timewindows_dropdown_vector[23] == 2) hmStream.SSPrintf(HtmlTagStreamMode, "option selected=\"selected\" value=2>position-Soft PID</option");
				else hmStream.SSPrintf(HtmlTagStreamMode, "option value=2>position-Soft PID</option");
				if (timewindows_dropdown_vector[23] == 3) hmStream.SSPrintf(HtmlTagStreamMode, "option selected=\"selected\" value=3>position-Med. PID</option");
				else hmStream.SSPrintf(HtmlTagStreamMode, "option value=3>position-Med. PID</option");
				if (timewindows_dropdown_vector[23] == 4) hmStream.SSPrintf(HtmlTagStreamMode, "option selected=\"selected\" value=4>position-Hard PID</option");
				else hmStream.SSPrintf(HtmlTagStreamMode, "option value=4>position-Hard PID</option");
				if (timewindows_dropdown_vector[23] == 5) hmStream.SSPrintf(HtmlTagStreamMode, "option selected=\"selected\" value=5>position-Auto PID</option");
				else hmStream.SSPrintf(HtmlTagStreamMode, "option value=5>position-Auto PID</option");
				if (timewindows_dropdown_vector[23] == 6) hmStream.SSPrintf(HtmlTagStreamMode, "option selected=\"selected\" value=6>Integrated control*</option");
				else hmStream.SSPrintf(HtmlTagStreamMode, "option value=6>Integrated control*</option");
				if (timewindows_dropdown_vector[23] == 7) hmStream.SSPrintf(HtmlTagStreamMode, "option selected=\"selected\" value=7>Invert*</option");
				else hmStream.SSPrintf(HtmlTagStreamMode, "option value=7>Invert*</option");
			hmStream.SSPrintf(HtmlTagStreamMode, "/select");												
		hmStream.SSPrintf(HtmlTagStreamMode, "/div");

		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 784px; left: 94px;\"");
			hmStream.SSPrintf(HtmlTagStreamMode, "select NAME=\"form_timewindows_drop_24\""); 
				if (timewindows_dropdown_vector[24] == 0) hmStream.SSPrintf(HtmlTagStreamMode, "option selected=\"selected\" value=0 >off</option");
				else hmStream.SSPrintf(HtmlTagStreamMode, "option value=0>off</option");
				if (timewindows_dropdown_vector[24] == 1) hmStream.SSPrintf(HtmlTagStreamMode, "option selected=\"selected\" value=1>current control</option");
				else hmStream.SSPrintf(HtmlTagStreamMode, "option value=1>current control</option");
				if (timewindows_dropdown_vector[24] == 2) hmStream.SSPrintf(HtmlTagStreamMode, "option selected=\"selected\" value=2>Ip control-Soft PID</option");
				else hmStream.SSPrintf(HtmlTagStreamMode, "option value=2>Ip control-Soft PID</option");
				if (timewindows_dropdown_vector[24] == 3) hmStream.SSPrintf(HtmlTagStreamMode, "option selected=\"selected\" value=3>Ip control-Med. PID</option");
				else hmStream.SSPrintf(HtmlTagStreamMode, "option value=3>Ip control-Med. PID</option");
				if (timewindows_dropdown_vector[24] == 4) hmStream.SSPrintf(HtmlTagStreamMode, "option selected=\"selected\" value=4>Ip control-Hard PID</option");
				else hmStream.SSPrintf(HtmlTagStreamMode, "option value=4>Ip control-Hard PID</option");
				if (timewindows_dropdown_vector[24] == 5) hmStream.SSPrintf(HtmlTagStreamMode, "option selected=\"selected\" value=5>Ip control-Auto PID</option");
				else hmStream.SSPrintf(HtmlTagStreamMode, "option value=5>Ip control-Auto PID</option");
				if (timewindows_dropdown_vector[24] == 6) hmStream.SSPrintf(HtmlTagStreamMode, "option selected=\"selected\" value=6>Integrated control*</option");
				else hmStream.SSPrintf(HtmlTagStreamMode, "option value=6>Integrated control*</option");
				if (timewindows_dropdown_vector[24] == 7) hmStream.SSPrintf(HtmlTagStreamMode, "option selected=\"selected\" value=7>Invert*</option");
				else hmStream.SSPrintf(HtmlTagStreamMode, "option value=7>Invert*</option");
			hmStream.SSPrintf(HtmlTagStreamMode, "/select");												
		hmStream.SSPrintf(HtmlTagStreamMode, "/div");

		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 784px; left: 236px;\"");
			hmStream.SSPrintf(HtmlTagStreamMode, "select NAME=\"form_timewindows_drop_25\""); 
				if (timewindows_dropdown_vector[25] == 0) hmStream.SSPrintf(HtmlTagStreamMode, "option selected=\"selected\" value=0 >off</option");
				else hmStream.SSPrintf(HtmlTagStreamMode, "option value=0>off</option");
				if (timewindows_dropdown_vector[25] == 1) hmStream.SSPrintf(HtmlTagStreamMode, "option selected=\"selected\" value=1>current control</option");
				else hmStream.SSPrintf(HtmlTagStreamMode, "option value=1>current control</option");
				if (timewindows_dropdown_vector[25] == 2) hmStream.SSPrintf(HtmlTagStreamMode, "option selected=\"selected\" value=2>position-Soft PID</option");
				else hmStream.SSPrintf(HtmlTagStreamMode, "option value=2>position-Soft PID</option");
				if (timewindows_dropdown_vector[25] == 3) hmStream.SSPrintf(HtmlTagStreamMode, "option selected=\"selected\" value=3>position-Med. PID</option");
				else hmStream.SSPrintf(HtmlTagStreamMode, "option value=3>position-Med. PID</option");
				if (timewindows_dropdown_vector[25] == 4) hmStream.SSPrintf(HtmlTagStreamMode, "option selected=\"selected\" value=4>position-Hard PID</option");
				else hmStream.SSPrintf(HtmlTagStreamMode, "option value=4>position-Hard PID</option");
				if (timewindows_dropdown_vector[25] == 5) hmStream.SSPrintf(HtmlTagStreamMode, "option selected=\"selected\" value=5>position-Auto PID</option");
				else hmStream.SSPrintf(HtmlTagStreamMode, "option value=5>position-Auto PID</option");
				if (timewindows_dropdown_vector[25] == 6) hmStream.SSPrintf(HtmlTagStreamMode, "option selected=\"selected\" value=6>Integrated control*</option");
				else hmStream.SSPrintf(HtmlTagStreamMode, "option value=6>Integrated control*</option");
				if (timewindows_dropdown_vector[25] == 7) hmStream.SSPrintf(HtmlTagStreamMode, "option selected=\"selected\" value=7>Invert*</option");
				else hmStream.SSPrintf(HtmlTagStreamMode, "option value=7>Invert*</option");
			hmStream.SSPrintf(HtmlTagStreamMode, "/select");												
		hmStream.SSPrintf(HtmlTagStreamMode, "/div");

		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 784px; left: 370px;\"");
			hmStream.SSPrintf(HtmlTagStreamMode, "select NAME=\"form_timewindows_drop_26\""); 
				if (timewindows_dropdown_vector[26] == 0) hmStream.SSPrintf(HtmlTagStreamMode, "option selected=\"selected\" value=0 >off</option");
				else hmStream.SSPrintf(HtmlTagStreamMode, "option value=0>off</option");
				if (timewindows_dropdown_vector[26] == 1) hmStream.SSPrintf(HtmlTagStreamMode, "option selected=\"selected\" value=1>current control</option");
				else hmStream.SSPrintf(HtmlTagStreamMode, "option value=1>current control</option");
				if (timewindows_dropdown_vector[26] == 2) hmStream.SSPrintf(HtmlTagStreamMode, "option selected=\"selected\" value=2>position-Soft PID</option");
				else hmStream.SSPrintf(HtmlTagStreamMode, "option value=2>position-Soft PID</option");
				if (timewindows_dropdown_vector[26] == 3) hmStream.SSPrintf(HtmlTagStreamMode, "option selected=\"selected\" value=3>position-Med. PID</option");
				else hmStream.SSPrintf(HtmlTagStreamMode, "option value=3>position-Med. PID</option");
				if (timewindows_dropdown_vector[26] == 4) hmStream.SSPrintf(HtmlTagStreamMode, "option selected=\"selected\" value=4>position-Hard PID</option");
				else hmStream.SSPrintf(HtmlTagStreamMode, "option value=4>position-Hard PID</option");
				if (timewindows_dropdown_vector[26] == 5) hmStream.SSPrintf(HtmlTagStreamMode, "option selected=\"selected\" value=5>position-Auto PID</option");
				else hmStream.SSPrintf(HtmlTagStreamMode, "option value=5>position-Auto PID</option");
				if (timewindows_dropdown_vector[26] == 6) hmStream.SSPrintf(HtmlTagStreamMode, "option selected=\"selected\" value=6>Integrated control*</option");
				else hmStream.SSPrintf(HtmlTagStreamMode, "option value=6>Integrated control*</option");
				if (timewindows_dropdown_vector[26] == 7) hmStream.SSPrintf(HtmlTagStreamMode, "option selected=\"selected\" value=7>Invert*</option");
				else hmStream.SSPrintf(HtmlTagStreamMode, "option value=7>Invert*</option");
			hmStream.SSPrintf(HtmlTagStreamMode, "/select");												
		hmStream.SSPrintf(HtmlTagStreamMode, "/div");

		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 806px; left: 94px;\"");
			hmStream.SSPrintf(HtmlTagStreamMode, "select NAME=\"form_timewindows_drop_27\""); 
				if (timewindows_dropdown_vector[27] == 0) hmStream.SSPrintf(HtmlTagStreamMode, "option selected=\"selected\" value=0 >off</option");
				else hmStream.SSPrintf(HtmlTagStreamMode, "option value=0>off</option");
				if (timewindows_dropdown_vector[27] == 1) hmStream.SSPrintf(HtmlTagStreamMode, "option selected=\"selected\" value=1>current control</option");
				else hmStream.SSPrintf(HtmlTagStreamMode, "option value=1>current control</option");
				if (timewindows_dropdown_vector[27] == 2) hmStream.SSPrintf(HtmlTagStreamMode, "option selected=\"selected\" value=2>Ip control-Soft PID</option");
				else hmStream.SSPrintf(HtmlTagStreamMode, "option value=2>Ip control-Soft PID</option");
				if (timewindows_dropdown_vector[27] == 3) hmStream.SSPrintf(HtmlTagStreamMode, "option selected=\"selected\" value=3>Ip control-Med. PID</option");
				else hmStream.SSPrintf(HtmlTagStreamMode, "option value=3>Ip control-Med. PID</option");
				if (timewindows_dropdown_vector[27] == 4) hmStream.SSPrintf(HtmlTagStreamMode, "option selected=\"selected\" value=4>Ip control-Hard PID</option");
				else hmStream.SSPrintf(HtmlTagStreamMode, "option value=4>Ip control-Hard PID</option");
				if (timewindows_dropdown_vector[27] == 5) hmStream.SSPrintf(HtmlTagStreamMode, "option selected=\"selected\" value=5>Ip control-Auto PID</option");
				else hmStream.SSPrintf(HtmlTagStreamMode, "option value=5>Ip control-Auto PID</option");
				if (timewindows_dropdown_vector[27] == 6) hmStream.SSPrintf(HtmlTagStreamMode, "option selected=\"selected\" value=6>Integrated control*</option");
				else hmStream.SSPrintf(HtmlTagStreamMode, "option value=6>Integrated control*</option");
				if (timewindows_dropdown_vector[27] == 7) hmStream.SSPrintf(HtmlTagStreamMode, "option selected=\"selected\" value=7>Invert*</option");
				else hmStream.SSPrintf(HtmlTagStreamMode, "option value=7>Invert*</option");
			hmStream.SSPrintf(HtmlTagStreamMode, "/select");												
		hmStream.SSPrintf(HtmlTagStreamMode, "/div");

		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 806px; left: 236px;\"");
			hmStream.SSPrintf(HtmlTagStreamMode, "select NAME=\"form_timewindows_drop_28\""); 
				if (timewindows_dropdown_vector[28] == 0) hmStream.SSPrintf(HtmlTagStreamMode, "option selected=\"selected\" value=0 >off</option");
				else hmStream.SSPrintf(HtmlTagStreamMode, "option value=0>off</option");
				if (timewindows_dropdown_vector[28] == 1) hmStream.SSPrintf(HtmlTagStreamMode, "option selected=\"selected\" value=1>current control</option");
				else hmStream.SSPrintf(HtmlTagStreamMode, "option value=1>current control</option");
				if (timewindows_dropdown_vector[28] == 2) hmStream.SSPrintf(HtmlTagStreamMode, "option selected=\"selected\" value=2>position-Soft PID</option");
				else hmStream.SSPrintf(HtmlTagStreamMode, "option value=2>position-Soft PID</option");
				if (timewindows_dropdown_vector[28] == 3) hmStream.SSPrintf(HtmlTagStreamMode, "option selected=\"selected\" value=3>position-Med. PID</option");
				else hmStream.SSPrintf(HtmlTagStreamMode, "option value=3>position-Med. PID</option");
				if (timewindows_dropdown_vector[28] == 4) hmStream.SSPrintf(HtmlTagStreamMode, "option selected=\"selected\" value=4>position-Hard PID</option");
				else hmStream.SSPrintf(HtmlTagStreamMode, "option value=4>position-Hard PID</option");
				if (timewindows_dropdown_vector[28] == 5) hmStream.SSPrintf(HtmlTagStreamMode, "option selected=\"selected\" value=5>position-Auto PID</option");
				else hmStream.SSPrintf(HtmlTagStreamMode, "option value=5>position-Auto PID</option");
				if (timewindows_dropdown_vector[28] == 6) hmStream.SSPrintf(HtmlTagStreamMode, "option selected=\"selected\" value=6>Integrated control*</option");
				else hmStream.SSPrintf(HtmlTagStreamMode, "option value=6>Integrated control*</option");
				if (timewindows_dropdown_vector[28] == 7) hmStream.SSPrintf(HtmlTagStreamMode, "option selected=\"selected\" value=7>Invert*</option");
				else hmStream.SSPrintf(HtmlTagStreamMode, "option value=7>Invert*</option");
			hmStream.SSPrintf(HtmlTagStreamMode, "/select");												
		hmStream.SSPrintf(HtmlTagStreamMode, "/div");

		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 806px; left: 370px;\"");
			hmStream.SSPrintf(HtmlTagStreamMode, "select NAME=\"form_timewindows_drop_29\""); 
				if (timewindows_dropdown_vector[29] == 0) hmStream.SSPrintf(HtmlTagStreamMode, "option selected=\"selected\" value=0 >off</option");
				else hmStream.SSPrintf(HtmlTagStreamMode, "option value=0>off</option");
				if (timewindows_dropdown_vector[29] == 1) hmStream.SSPrintf(HtmlTagStreamMode, "option selected=\"selected\" value=1>current control</option");
				else hmStream.SSPrintf(HtmlTagStreamMode, "option value=1>current control</option");
				if (timewindows_dropdown_vector[29] == 2) hmStream.SSPrintf(HtmlTagStreamMode, "option selected=\"selected\" value=2>position-Soft PID</option");
				else hmStream.SSPrintf(HtmlTagStreamMode, "option value=2>position-Soft PID</option");
				if (timewindows_dropdown_vector[29] == 3) hmStream.SSPrintf(HtmlTagStreamMode, "option selected=\"selected\" value=3>position-Med. PID</option");
				else hmStream.SSPrintf(HtmlTagStreamMode, "option value=3>position-Med. PID</option");
				if (timewindows_dropdown_vector[29] == 4) hmStream.SSPrintf(HtmlTagStreamMode, "option selected=\"selected\" value=4>position-Hard PID</option");
				else hmStream.SSPrintf(HtmlTagStreamMode, "option value=4>position-Hard PID</option");
				if (timewindows_dropdown_vector[29] == 5) hmStream.SSPrintf(HtmlTagStreamMode, "option selected=\"selected\" value=5>position-Auto PID</option");
				else hmStream.SSPrintf(HtmlTagStreamMode, "option value=5>position-Auto PID</option");
				if (timewindows_dropdown_vector[29] == 6) hmStream.SSPrintf(HtmlTagStreamMode, "option selected=\"selected\" value=6>Integrated control*</option");
				else hmStream.SSPrintf(HtmlTagStreamMode, "option value=6>Integrated control*</option");
				if (timewindows_dropdown_vector[29] == 7) hmStream.SSPrintf(HtmlTagStreamMode, "option selected=\"selected\" value=7>Invert*</option");
				else hmStream.SSPrintf(HtmlTagStreamMode, "option value=7>Invert*</option");
			hmStream.SSPrintf(HtmlTagStreamMode, "/select");												
		hmStream.SSPrintf(HtmlTagStreamMode, "/div");
	
		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 828px; left: 94px;\""); 
			hmStream.SSPrintf(HtmlTagStreamMode, "select NAME=\"form_timewindows_drop_30\""); 
				if (timewindows_dropdown_vector[30] == 0) hmStream.SSPrintf(HtmlTagStreamMode, "option selected=\"selected\" value=0 >off</option");
				else hmStream.SSPrintf(HtmlTagStreamMode, "option value=0>off</option");
				if (timewindows_dropdown_vector[30] == 1) hmStream.SSPrintf(HtmlTagStreamMode, "option selected=\"selected\" value=1>current control</option");
				else hmStream.SSPrintf(HtmlTagStreamMode, "option value=1>current control</option");
				if (timewindows_dropdown_vector[30] == 2) hmStream.SSPrintf(HtmlTagStreamMode, "option selected=\"selected\" value=2>Ip control-Soft PID</option");
				else hmStream.SSPrintf(HtmlTagStreamMode, "option value=2>Ip control-Soft PID</option");
				if (timewindows_dropdown_vector[30] == 3) hmStream.SSPrintf(HtmlTagStreamMode, "option selected=\"selected\" value=3>Ip control-Med. PID</option");
				else hmStream.SSPrintf(HtmlTagStreamMode, "option value=3>Ip control-Med. PID</option");
				if (timewindows_dropdown_vector[30] == 4) hmStream.SSPrintf(HtmlTagStreamMode, "option selected=\"selected\" value=4>Ip control-Hard PID</option");
				else hmStream.SSPrintf(HtmlTagStreamMode, "option value=4>Ip control-Hard PID</option");
				if (timewindows_dropdown_vector[30] == 5) hmStream.SSPrintf(HtmlTagStreamMode, "option selected=\"selected\" value=5>Ip control-Auto PID</option");
				else hmStream.SSPrintf(HtmlTagStreamMode, "option value=5>Ip control-Auto PID</option");
				if (timewindows_dropdown_vector[30] == 6) hmStream.SSPrintf(HtmlTagStreamMode, "option selected=\"selected\" value=6>Integrated control*</option");
				else hmStream.SSPrintf(HtmlTagStreamMode, "option value=6>Integrated control*</option");
				if (timewindows_dropdown_vector[30] == 7) hmStream.SSPrintf(HtmlTagStreamMode, "option selected=\"selected\" value=7>Invert*</option");
				else hmStream.SSPrintf(HtmlTagStreamMode, "option value=7>Invert*</option");
			hmStream.SSPrintf(HtmlTagStreamMode, "/select");												
		hmStream.SSPrintf(HtmlTagStreamMode, "/div");

		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 828px; left: 236px;\"");
			hmStream.SSPrintf(HtmlTagStreamMode, "select NAME=\"form_timewindows_drop_31\""); 
				if (timewindows_dropdown_vector[31] == 0) hmStream.SSPrintf(HtmlTagStreamMode, "option selected=\"selected\" value=0 >off</option");
				else hmStream.SSPrintf(HtmlTagStreamMode, "option value=0>off</option");
				if (timewindows_dropdown_vector[31] == 1) hmStream.SSPrintf(HtmlTagStreamMode, "option selected=\"selected\" value=1>current control</option");
				else hmStream.SSPrintf(HtmlTagStreamMode, "option value=1>current control</option");
				if (timewindows_dropdown_vector[31] == 2) hmStream.SSPrintf(HtmlTagStreamMode, "option selected=\"selected\" value=2>position-Soft PID</option");
				else hmStream.SSPrintf(HtmlTagStreamMode, "option value=2>position-Soft PID</option");
				if (timewindows_dropdown_vector[31] == 3) hmStream.SSPrintf(HtmlTagStreamMode, "option selected=\"selected\" value=3>position-Med. PID</option");
				else hmStream.SSPrintf(HtmlTagStreamMode, "option value=3>position-Med. PID</option");
				if (timewindows_dropdown_vector[31] == 4) hmStream.SSPrintf(HtmlTagStreamMode, "option selected=\"selected\" value=4>position-Hard PID</option");
				else hmStream.SSPrintf(HtmlTagStreamMode, "option value=4>position-Hard PID</option");
				if (timewindows_dropdown_vector[31] == 5) hmStream.SSPrintf(HtmlTagStreamMode, "option selected=\"selected\" value=5>position-Auto PID</option");
				else hmStream.SSPrintf(HtmlTagStreamMode, "option value=5>position-Auto PID</option");
				if (timewindows_dropdown_vector[31] == 6) hmStream.SSPrintf(HtmlTagStreamMode, "option selected=\"selected\" value=6>Integrated control*</option");
				else hmStream.SSPrintf(HtmlTagStreamMode, "option value=6>Integrated control*</option");
				if (timewindows_dropdown_vector[31] == 7) hmStream.SSPrintf(HtmlTagStreamMode, "option selected=\"selected\" value=7>Invert*</option");
				else hmStream.SSPrintf(HtmlTagStreamMode, "option value=7>Invert*</option");
			hmStream.SSPrintf(HtmlTagStreamMode, "/select");												
		hmStream.SSPrintf(HtmlTagStreamMode, "/div");

		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 828px; left: 370px;\"");
			hmStream.SSPrintf(HtmlTagStreamMode, "select NAME=\"form_timewindows_drop_32\""); 
				if (timewindows_dropdown_vector[32] == 0) hmStream.SSPrintf(HtmlTagStreamMode, "option selected=\"selected\" value=0 >off</option");
				else hmStream.SSPrintf(HtmlTagStreamMode, "option value=0>off</option");
				if (timewindows_dropdown_vector[32] == 1) hmStream.SSPrintf(HtmlTagStreamMode, "option selected=\"selected\" value=1>current control</option");
				else hmStream.SSPrintf(HtmlTagStreamMode, "option value=1>current control</option");
				if (timewindows_dropdown_vector[32] == 2) hmStream.SSPrintf(HtmlTagStreamMode, "option selected=\"selected\" value=2>position-Soft PID</option");
				else hmStream.SSPrintf(HtmlTagStreamMode, "option value=2>position-Soft PID</option");
				if (timewindows_dropdown_vector[32] == 3) hmStream.SSPrintf(HtmlTagStreamMode, "option selected=\"selected\" value=3>position-Med. PID</option");
				else hmStream.SSPrintf(HtmlTagStreamMode, "option value=3>position-Med. PID</option");
				if (timewindows_dropdown_vector[32] == 4) hmStream.SSPrintf(HtmlTagStreamMode, "option selected=\"selected\" value=4>position-Hard PID</option");
				else hmStream.SSPrintf(HtmlTagStreamMode, "option value=4>position-Hard PID</option");
				if (timewindows_dropdown_vector[32] == 5) hmStream.SSPrintf(HtmlTagStreamMode, "option selected=\"selected\" value=5>position-Auto PID</option");
				else hmStream.SSPrintf(HtmlTagStreamMode, "option value=5>position-Auto PID</option");
				if (timewindows_dropdown_vector[32] == 6) hmStream.SSPrintf(HtmlTagStreamMode, "option selected=\"selected\" value=6>Integrated control*</option");
				else hmStream.SSPrintf(HtmlTagStreamMode, "option value=6>Integrated control*</option");
				if (timewindows_dropdown_vector[32] == 7) hmStream.SSPrintf(HtmlTagStreamMode, "option selected=\"selected\" value=7>Invert*</option");
				else hmStream.SSPrintf(HtmlTagStreamMode, "option value=7>Invert*</option");
			hmStream.SSPrintf(HtmlTagStreamMode, "/select");												
		hmStream.SSPrintf(HtmlTagStreamMode, "/div");

		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 850px; left: 94px;\""); 
			hmStream.SSPrintf(HtmlTagStreamMode, "select NAME=\"form_timewindows_drop_33\""); 
				if (timewindows_dropdown_vector[33] == 0) hmStream.SSPrintf(HtmlTagStreamMode, "option selected=\"selected\" value=0 >off</option");
				else hmStream.SSPrintf(HtmlTagStreamMode, "option value=0>off</option");
				if (timewindows_dropdown_vector[33] == 1) hmStream.SSPrintf(HtmlTagStreamMode, "option selected=\"selected\" value=1>current control</option");
				else hmStream.SSPrintf(HtmlTagStreamMode, "option value=1>current control</option");
				if (timewindows_dropdown_vector[33] == 2) hmStream.SSPrintf(HtmlTagStreamMode, "option selected=\"selected\" value=2>Ip control-Soft PID</option");
				else hmStream.SSPrintf(HtmlTagStreamMode, "option value=2>Ip control-Soft PID</option");
				if (timewindows_dropdown_vector[33] == 3) hmStream.SSPrintf(HtmlTagStreamMode, "option selected=\"selected\" value=3>Ip control-Med. PID</option");
				else hmStream.SSPrintf(HtmlTagStreamMode, "option value=3>Ip control-Med. PID</option");
				if (timewindows_dropdown_vector[33] == 4) hmStream.SSPrintf(HtmlTagStreamMode, "option selected=\"selected\" value=4>Ip control-Hard PID</option");
				else hmStream.SSPrintf(HtmlTagStreamMode, "option value=4>Ip control-Hard PID</option");
				if (timewindows_dropdown_vector[33] == 5) hmStream.SSPrintf(HtmlTagStreamMode, "option selected=\"selected\" value=5>Ip control-Auto PID</option");
				else hmStream.SSPrintf(HtmlTagStreamMode, "option value=5>Ip control-Auto PID</option");
				if (timewindows_dropdown_vector[33] == 6) hmStream.SSPrintf(HtmlTagStreamMode, "option selected=\"selected\" value=6>Integrated control*</option");
				else hmStream.SSPrintf(HtmlTagStreamMode, "option value=6>Integrated control*</option");
				if (timewindows_dropdown_vector[33] == 7) hmStream.SSPrintf(HtmlTagStreamMode, "option selected=\"selected\" value=7>Invert*</option");
				else hmStream.SSPrintf(HtmlTagStreamMode, "option value=7>Invert*</option");
			hmStream.SSPrintf(HtmlTagStreamMode, "/select");												
		hmStream.SSPrintf(HtmlTagStreamMode, "/div");

		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 850px; left: 236px;\"");
			hmStream.SSPrintf(HtmlTagStreamMode, "select NAME=\"form_timewindows_drop_34\""); 
				if (timewindows_dropdown_vector[34] == 0) hmStream.SSPrintf(HtmlTagStreamMode, "option selected=\"selected\" value=0 >off</option");
				else hmStream.SSPrintf(HtmlTagStreamMode, "option value=0>off</option");
				if (timewindows_dropdown_vector[34] == 1) hmStream.SSPrintf(HtmlTagStreamMode, "option selected=\"selected\" value=1>current control</option");
				else hmStream.SSPrintf(HtmlTagStreamMode, "option value=1>current control</option");
				if (timewindows_dropdown_vector[34] == 2) hmStream.SSPrintf(HtmlTagStreamMode, "option selected=\"selected\" value=2>position-Soft PID</option");
				else hmStream.SSPrintf(HtmlTagStreamMode, "option value=2>position-Soft PID</option");
				if (timewindows_dropdown_vector[34] == 3) hmStream.SSPrintf(HtmlTagStreamMode, "option selected=\"selected\" value=3>position-Med. PID</option");
				else hmStream.SSPrintf(HtmlTagStreamMode, "option value=3>position-Med. PID</option");
				if (timewindows_dropdown_vector[34] == 4) hmStream.SSPrintf(HtmlTagStreamMode, "option selected=\"selected\" value=4>position-Hard PID</option");
				else hmStream.SSPrintf(HtmlTagStreamMode, "option value=4>position-Hard PID</option");
				if (timewindows_dropdown_vector[34] == 5) hmStream.SSPrintf(HtmlTagStreamMode, "option selected=\"selected\" value=5>position-Auto PID</option");
				else hmStream.SSPrintf(HtmlTagStreamMode, "option value=5>position-Auto PID</option");
				if (timewindows_dropdown_vector[34] == 6) hmStream.SSPrintf(HtmlTagStreamMode, "option selected=\"selected\" value=6>Integrated control*</option");
				else hmStream.SSPrintf(HtmlTagStreamMode, "option value=6>Integrated control*</option");
				if (timewindows_dropdown_vector[34] == 7) hmStream.SSPrintf(HtmlTagStreamMode, "option selected=\"selected\" value=7>Invert*</option");
				else hmStream.SSPrintf(HtmlTagStreamMode, "option value=7>Invert*</option");
			hmStream.SSPrintf(HtmlTagStreamMode, "/select");												
		hmStream.SSPrintf(HtmlTagStreamMode, "/div");

		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 850px; left: 370px;\"");
			hmStream.SSPrintf(HtmlTagStreamMode, "select NAME=\"form_timewindows_drop_35\""); 
				if (timewindows_dropdown_vector[35] == 0) hmStream.SSPrintf(HtmlTagStreamMode, "option selected=\"selected\" value=0 >off</option");
				else hmStream.SSPrintf(HtmlTagStreamMode, "option value=0>off</option");
				if (timewindows_dropdown_vector[35] == 1) hmStream.SSPrintf(HtmlTagStreamMode, "option selected=\"selected\" value=1>current control</option");
				else hmStream.SSPrintf(HtmlTagStreamMode, "option value=1>current control</option");
				if (timewindows_dropdown_vector[35] == 2) hmStream.SSPrintf(HtmlTagStreamMode, "option selected=\"selected\" value=2>position-Soft PID</option");
				else hmStream.SSPrintf(HtmlTagStreamMode, "option value=2>position-Soft PID</option");
				if (timewindows_dropdown_vector[35] == 3) hmStream.SSPrintf(HtmlTagStreamMode, "option selected=\"selected\" value=3>position-Med. PID</option");
				else hmStream.SSPrintf(HtmlTagStreamMode, "option value=3>position-Med. PID</option");
				if (timewindows_dropdown_vector[35] == 4) hmStream.SSPrintf(HtmlTagStreamMode, "option selected=\"selected\" value=4>position-Hard PID</option");
				else hmStream.SSPrintf(HtmlTagStreamMode, "option value=4>position-Hard PID</option");
				if (timewindows_dropdown_vector[35] == 5) hmStream.SSPrintf(HtmlTagStreamMode, "option selected=\"selected\" value=5>position-Auto PID</option");
				else hmStream.SSPrintf(HtmlTagStreamMode, "option value=5>position-Auto PID</option");
				if (timewindows_dropdown_vector[35] == 6) hmStream.SSPrintf(HtmlTagStreamMode, "option selected=\"selected\" value=6>Integrated control*</option");
				else hmStream.SSPrintf(HtmlTagStreamMode, "option value=6>Integrated control*</option");
				if (timewindows_dropdown_vector[35] == 7) hmStream.SSPrintf(HtmlTagStreamMode, "option selected=\"selected\" value=7>Invert*</option");
				else hmStream.SSPrintf(HtmlTagStreamMode, "option value=7>Invert*</option");
			hmStream.SSPrintf(HtmlTagStreamMode, "/select");												
		hmStream.SSPrintf(HtmlTagStreamMode, "/div");

		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 872px; left: 94px;\""); 
			hmStream.SSPrintf(HtmlTagStreamMode, "select NAME=\"form_timewindows_drop_36\""); 
				if (timewindows_dropdown_vector[36] == 0) hmStream.SSPrintf(HtmlTagStreamMode, "option selected=\"selected\" value=0 >off</option");
				else hmStream.SSPrintf(HtmlTagStreamMode, "option value=0>off</option");
				if (timewindows_dropdown_vector[36] == 1) hmStream.SSPrintf(HtmlTagStreamMode, "option selected=\"selected\" value=1>current control</option");
				else hmStream.SSPrintf(HtmlTagStreamMode, "option value=1>current control</option");
				if (timewindows_dropdown_vector[36] == 2) hmStream.SSPrintf(HtmlTagStreamMode, "option selected=\"selected\" value=2>Ip control-Soft PID</option");
				else hmStream.SSPrintf(HtmlTagStreamMode, "option value=2>Ip control-Soft PID</option");
				if (timewindows_dropdown_vector[36] == 3) hmStream.SSPrintf(HtmlTagStreamMode, "option selected=\"selected\" value=3>Ip control-Med. PID</option");
				else hmStream.SSPrintf(HtmlTagStreamMode, "option value=3>Ip control-Med. PID</option");
				if (timewindows_dropdown_vector[36] == 4) hmStream.SSPrintf(HtmlTagStreamMode, "option selected=\"selected\" value=4>Ip control-Hard PID</option");
				else hmStream.SSPrintf(HtmlTagStreamMode, "option value=4>Ip control-Hard PID</option");
				if (timewindows_dropdown_vector[36] == 5) hmStream.SSPrintf(HtmlTagStreamMode, "option selected=\"selected\" value=5>Ip control-Auto PID</option");
				else hmStream.SSPrintf(HtmlTagStreamMode, "option value=5>Ip control-Auto PID</option");
				if (timewindows_dropdown_vector[36] == 6) hmStream.SSPrintf(HtmlTagStreamMode, "option selected=\"selected\" value=6>Integrated control*</option");
				else hmStream.SSPrintf(HtmlTagStreamMode, "option value=6>Integrated control*</option");
				if (timewindows_dropdown_vector[36] == 7) hmStream.SSPrintf(HtmlTagStreamMode, "option selected=\"selected\" value=7>Invert*</option");
				else hmStream.SSPrintf(HtmlTagStreamMode, "option value=7>Invert*</option");
			hmStream.SSPrintf(HtmlTagStreamMode, "/select");												
		hmStream.SSPrintf(HtmlTagStreamMode, "/div");

		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 872px; left: 236px;\"");
			hmStream.SSPrintf(HtmlTagStreamMode, "select NAME=\"form_timewindows_drop_37\""); 
				if (timewindows_dropdown_vector[37] == 0) hmStream.SSPrintf(HtmlTagStreamMode, "option selected=\"selected\" value=0 >off</option");
				else hmStream.SSPrintf(HtmlTagStreamMode, "option value=0>off</option");
				if (timewindows_dropdown_vector[37] == 1) hmStream.SSPrintf(HtmlTagStreamMode, "option selected=\"selected\" value=1>current control</option");
				else hmStream.SSPrintf(HtmlTagStreamMode, "option value=1>current control</option");
				if (timewindows_dropdown_vector[37] == 2) hmStream.SSPrintf(HtmlTagStreamMode, "option selected=\"selected\" value=2>position-Soft PID</option");
				else hmStream.SSPrintf(HtmlTagStreamMode, "option value=2>position-Soft PID</option");
				if (timewindows_dropdown_vector[37] == 3) hmStream.SSPrintf(HtmlTagStreamMode, "option selected=\"selected\" value=3>position-Med. PID</option");
				else hmStream.SSPrintf(HtmlTagStreamMode, "option value=3>position-Med. PID</option");
				if (timewindows_dropdown_vector[37] == 4) hmStream.SSPrintf(HtmlTagStreamMode, "option selected=\"selected\" value=4>position-Hard PID</option");
				else hmStream.SSPrintf(HtmlTagStreamMode, "option value=4>position-Hard PID</option");
				if (timewindows_dropdown_vector[37] == 5) hmStream.SSPrintf(HtmlTagStreamMode, "option selected=\"selected\" value=5>position-Auto PID</option");
				else hmStream.SSPrintf(HtmlTagStreamMode, "option value=5>position-Auto PID</option");
				if (timewindows_dropdown_vector[37] == 6) hmStream.SSPrintf(HtmlTagStreamMode, "option selected=\"selected\" value=6>Integrated control*</option");
				else hmStream.SSPrintf(HtmlTagStreamMode, "option value=6>Integrated control*</option");
				if (timewindows_dropdown_vector[37] == 7) hmStream.SSPrintf(HtmlTagStreamMode, "option selected=\"selected\" value=7>Invert*</option");
				else hmStream.SSPrintf(HtmlTagStreamMode, "option value=7>Invert*</option");
			hmStream.SSPrintf(HtmlTagStreamMode, "/select");												
		hmStream.SSPrintf(HtmlTagStreamMode, "/div");

		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 872px; left: 370px;\"");
			hmStream.SSPrintf(HtmlTagStreamMode, "select NAME=\"form_timewindows_drop_38\""); 
				if (timewindows_dropdown_vector[38] == 0) hmStream.SSPrintf(HtmlTagStreamMode, "option selected=\"selected\" value=0 >off</option");
				else hmStream.SSPrintf(HtmlTagStreamMode, "option value=0>off</option");
				if (timewindows_dropdown_vector[38] == 1) hmStream.SSPrintf(HtmlTagStreamMode, "option selected=\"selected\" value=1>current control</option");
				else hmStream.SSPrintf(HtmlTagStreamMode, "option value=1>current control</option");
				if (timewindows_dropdown_vector[38] == 2) hmStream.SSPrintf(HtmlTagStreamMode, "option selected=\"selected\" value=2>position-Soft PID</option");
				else hmStream.SSPrintf(HtmlTagStreamMode, "option value=2>position-Soft PID</option");
				if (timewindows_dropdown_vector[38] == 3) hmStream.SSPrintf(HtmlTagStreamMode, "option selected=\"selected\" value=3>position-Med. PID</option");
				else hmStream.SSPrintf(HtmlTagStreamMode, "option value=3>position-Med. PID</option");
				if (timewindows_dropdown_vector[38] == 4) hmStream.SSPrintf(HtmlTagStreamMode, "option selected=\"selected\" value=4>position-Hard PID</option");
				else hmStream.SSPrintf(HtmlTagStreamMode, "option value=4>position-Hard PID</option");
				if (timewindows_dropdown_vector[38] == 5) hmStream.SSPrintf(HtmlTagStreamMode, "option selected=\"selected\" value=5>position-Auto PID</option");
				else hmStream.SSPrintf(HtmlTagStreamMode, "option value=5>position-Auto PID</option");
				if (timewindows_dropdown_vector[38] == 6) hmStream.SSPrintf(HtmlTagStreamMode, "option selected=\"selected\" value=6>Integrated control*</option");
				else hmStream.SSPrintf(HtmlTagStreamMode, "option value=6>Integrated control*</option");
				if (timewindows_dropdown_vector[38] == 7) hmStream.SSPrintf(HtmlTagStreamMode, "option selected=\"selected\" value=7>Invert*</option");
				else hmStream.SSPrintf(HtmlTagStreamMode, "option value=7>Invert*</option");
			hmStream.SSPrintf(HtmlTagStreamMode, "/select");
		hmStream.SSPrintf(HtmlTagStreamMode, "/div");

		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 894px; left: 94px;\""); 
			hmStream.SSPrintf(HtmlTagStreamMode, "select NAME=\"form_timewindows_drop_39\""); 
				if (timewindows_dropdown_vector[39] == 0) hmStream.SSPrintf(HtmlTagStreamMode, "option selected=\"selected\" value=0 >off</option");
				else hmStream.SSPrintf(HtmlTagStreamMode, "option value=0>off</option");
				if (timewindows_dropdown_vector[39] == 1) hmStream.SSPrintf(HtmlTagStreamMode, "option selected=\"selected\" value=1>current control</option");
				else hmStream.SSPrintf(HtmlTagStreamMode, "option value=1>current control</option");
				if (timewindows_dropdown_vector[39] == 2) hmStream.SSPrintf(HtmlTagStreamMode, "option selected=\"selected\" value=2>Ip control-Soft PID</option");
				else hmStream.SSPrintf(HtmlTagStreamMode, "option value=2>Ip control-Soft PID</option");
				if (timewindows_dropdown_vector[39] == 3) hmStream.SSPrintf(HtmlTagStreamMode, "option selected=\"selected\" value=3>Ip control-Med. PID</option");
				else hmStream.SSPrintf(HtmlTagStreamMode, "option value=3>Ip control-Med. PID</option");
				if (timewindows_dropdown_vector[39] == 4) hmStream.SSPrintf(HtmlTagStreamMode, "option selected=\"selected\" value=4>Ip control-Hard PID</option");
				else hmStream.SSPrintf(HtmlTagStreamMode, "option value=4>Ip control-Hard PID</option");
				if (timewindows_dropdown_vector[39] == 5) hmStream.SSPrintf(HtmlTagStreamMode, "option selected=\"selected\" value=5>Ip control-Auto PID</option");
				else hmStream.SSPrintf(HtmlTagStreamMode, "option value=5>Ip control-Auto PID</option");
				if (timewindows_dropdown_vector[39] == 6) hmStream.SSPrintf(HtmlTagStreamMode, "option selected=\"selected\" value=6>Integrated control*</option");
				else hmStream.SSPrintf(HtmlTagStreamMode, "option value=6>Integrated control*</option");
				if (timewindows_dropdown_vector[39] == 7) hmStream.SSPrintf(HtmlTagStreamMode, "option selected=\"selected\" value=7>Invert*</option");
				else hmStream.SSPrintf(HtmlTagStreamMode, "option value=7>Invert*</option");
			hmStream.SSPrintf(HtmlTagStreamMode, "/select");
		hmStream.SSPrintf(HtmlTagStreamMode, "/div");

		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 894px; left: 236px;\"");
			hmStream.SSPrintf(HtmlTagStreamMode, "select NAME=\"form_timewindows_drop_40\""); 
				if (timewindows_dropdown_vector[40] == 0) hmStream.SSPrintf(HtmlTagStreamMode, "option selected=\"selected\" value=0 >off</option");
				else hmStream.SSPrintf(HtmlTagStreamMode, "option value=0>off</option");
				if (timewindows_dropdown_vector[40] == 1) hmStream.SSPrintf(HtmlTagStreamMode, "option selected=\"selected\" value=1>current control</option");
				else hmStream.SSPrintf(HtmlTagStreamMode, "option value=1>current control</option");
				if (timewindows_dropdown_vector[40] == 2) hmStream.SSPrintf(HtmlTagStreamMode, "option selected=\"selected\" value=2>position-Soft PID</option");
				else hmStream.SSPrintf(HtmlTagStreamMode, "option value=2>position-Soft PID</option");
				if (timewindows_dropdown_vector[40] == 3) hmStream.SSPrintf(HtmlTagStreamMode, "option selected=\"selected\" value=3>position-Med. PID</option");
				else hmStream.SSPrintf(HtmlTagStreamMode, "option value=3>position-Med. PID</option");
				if (timewindows_dropdown_vector[40] == 4) hmStream.SSPrintf(HtmlTagStreamMode, "option selected=\"selected\" value=4>position-Hard PID</option");
				else hmStream.SSPrintf(HtmlTagStreamMode, "option value=4>position-Hard PID</option");
				if (timewindows_dropdown_vector[40] == 5) hmStream.SSPrintf(HtmlTagStreamMode, "option selected=\"selected\" value=5>position-Auto PID</option");
				else hmStream.SSPrintf(HtmlTagStreamMode, "option value=5>position-Auto PID</option");
				if (timewindows_dropdown_vector[40] == 6) hmStream.SSPrintf(HtmlTagStreamMode, "option selected=\"selected\" value=6>Integrated control*</option");
				else hmStream.SSPrintf(HtmlTagStreamMode, "option value=6>Integrated control*</option");
				if (timewindows_dropdown_vector[40] == 7) hmStream.SSPrintf(HtmlTagStreamMode, "option selected=\"selected\" value=7>Invert*</option");
				else hmStream.SSPrintf(HtmlTagStreamMode, "option value=7>Invert*</option");
			hmStream.SSPrintf(HtmlTagStreamMode, "/select");
		hmStream.SSPrintf(HtmlTagStreamMode, "/div");

		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 894px; left: 370px;\"");
			hmStream.SSPrintf(HtmlTagStreamMode, "select NAME=\"form_timewindows_drop_41\""); 
				if (timewindows_dropdown_vector[41] == 0) hmStream.SSPrintf(HtmlTagStreamMode, "option selected=\"selected\" value=0 >off</option");
				else hmStream.SSPrintf(HtmlTagStreamMode, "option value=0>off</option");
				if (timewindows_dropdown_vector[41] == 1) hmStream.SSPrintf(HtmlTagStreamMode, "option selected=\"selected\" value=1>current control</option");
				else hmStream.SSPrintf(HtmlTagStreamMode, "option value=1>current control</option");
				if (timewindows_dropdown_vector[41] == 2) hmStream.SSPrintf(HtmlTagStreamMode, "option selected=\"selected\" value=2>position-Soft PID</option");
				else hmStream.SSPrintf(HtmlTagStreamMode, "option value=2>position-Soft PID</option");
				if (timewindows_dropdown_vector[41] == 3) hmStream.SSPrintf(HtmlTagStreamMode, "option selected=\"selected\" value=3>position-Med. PID</option");
				else hmStream.SSPrintf(HtmlTagStreamMode, "option value=3>position-Med. PID</option");
				if (timewindows_dropdown_vector[41] == 4) hmStream.SSPrintf(HtmlTagStreamMode, "option selected=\"selected\" value=4>position-Hard PID</option");
				else hmStream.SSPrintf(HtmlTagStreamMode, "option value=4>position-Hard PID</option");
				if (timewindows_dropdown_vector[41] == 5) hmStream.SSPrintf(HtmlTagStreamMode, "option selected=\"selected\" value=5>position-Auto PID</option");
				else hmStream.SSPrintf(HtmlTagStreamMode, "option value=5>position-Auto PID</option");
				if (timewindows_dropdown_vector[41] == 6) hmStream.SSPrintf(HtmlTagStreamMode, "option selected=\"selected\" value=6>Integrated control*</option");
				else hmStream.SSPrintf(HtmlTagStreamMode, "option value=6>Integrated control*</option");
				if (timewindows_dropdown_vector[41] == 7) hmStream.SSPrintf(HtmlTagStreamMode, "option selected=\"selected\" value=7>Invert*</option");
				else hmStream.SSPrintf(HtmlTagStreamMode, "option value=7>Invert*</option");
			hmStream.SSPrintf(HtmlTagStreamMode, "/select");
		hmStream.SSPrintf(HtmlTagStreamMode, "/div");

		if (timewindows_bool_vector[0]) hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 562px; left: 45px;\"><input type=\"text\" size=\"2\" maxlength=\"6\" name=\"form_timewindows_time_0\" value=\"%0.2f\"></div",timewindows_time[0]);
		else hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 562px; left: 45px;\"><input type=\"text\" size=\"2\" maxlength=\"6\" name=\"form_timewindows_time_0\"></div");
		if (timewindows_bool_vector[1]) hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 584px; left: 45px;\"><input type=\"text\" size=\"2\" maxlength=\"6\" name=\"form_timewindows_time_1\" value=\"%0.2f\"></div",timewindows_time[1]);
		else hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 584px; left: 45px;\"><input type=\"text\" size=\"2\" maxlength=\"6\" name=\"form_timewindows_time_1\"></div");
		if (timewindows_bool_vector[2]) hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 606px; left: 45px;\"><input type=\"text\" size=\"2\" maxlength=\"6\" name=\"form_timewindows_time_2\" value=\"%0.2f\"></div",timewindows_time[2]);
		else hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 606px; left: 45px;\"><input type=\"text\" size=\"2\" maxlength=\"6\" name=\"form_timewindows_time_2\"></div");
		if (timewindows_bool_vector[3]) hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 628px; left: 45px;\"><input type=\"text\" size=\"2\" maxlength=\"6\" name=\"form_timewindows_time_3\" value=\"%0.2f\"></div",timewindows_time[3]);
		else hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 628px; left: 45px;\"><input type=\"text\" size=\"2\" maxlength=\"6\" name=\"form_timewindows_time_3\"></div");
		if (timewindows_bool_vector[4]) hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 650px; left: 45px;\"><input type=\"text\" size=\"2\" maxlength=\"6\" name=\"form_timewindows_time_4\" value=\"%0.2f\"></div",timewindows_time[4]);
		else hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 650px; left: 45px;\"><input type=\"text\" size=\"2\" maxlength=\"6\" name=\"form_timewindows_time_4\"></div");
		if (timewindows_bool_vector[5]) hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 672px; left: 45px;\"><input type=\"text\" size=\"2\" maxlength=\"6\" name=\"form_timewindows_time_5\" value=\"%0.2f\"></div",timewindows_time[5]);
		else hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 672px; left: 45px;\"><input type=\"text\" size=\"2\" maxlength=\"6\" name=\"form_timewindows_time_5\"></div");
		if (timewindows_bool_vector[6]) hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 694px; left: 45px;\"><input type=\"text\" size=\"2\" maxlength=\"6\" name=\"form_timewindows_time_6\" value=\"%0.2f\"></div",timewindows_time[6]);
		else hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 694px; left: 45px;\"><input type=\"text\" size=\"2\" maxlength=\"6\" name=\"form_timewindows_time_6\"></div");

		if (timewindows_bool_vector[7]) hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 762px; left: 45px;\"><input type=\"text\" size=\"2\" maxlength=\"6\" name=\"form_timewindows_time_7\" value=\"%0.2f\"></div",timewindows_time[7]);
		else hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 762px; left: 45px;\"><input type=\"text\" size=\"2\" maxlength=\"6\" name=\"form_timewindows_time_7\"></div");
		if (timewindows_bool_vector[8]) hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 784px; left: 45px;\"><input type=\"text\" size=\"2\" maxlength=\"6\" name=\"form_timewindows_time_8\" value=\"%0.2f\"></div",timewindows_time[8]);
		else hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 784px; left: 45px;\"><input type=\"text\" size=\"2\" maxlength=\"6\" name=\"form_timewindows_time_8\"></div");
		if (timewindows_bool_vector[9]) hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 806px; left: 45px;\"><input type=\"text\" size=\"2\" maxlength=\"6\" name=\"form_timewindows_time_9\" value=\"%0.2f\"></div",timewindows_time[9]);
		else hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 806px; left: 45px;\"><input type=\"text\" size=\"2\" maxlength=\"6\" name=\"form_timewindows_time_9\"></div");
		if (timewindows_bool_vector[10]) hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 828px; left: 45px;\"><input type=\"text\" size=\"2\" maxlength=\"6\" name=\"form_timewindows_time_10\" value=\"%0.2f\"></div",timewindows_time[10]);
		else hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 828px; left: 45px;\"><input type=\"text\" size=\"2\" maxlength=\"6\" name=\"form_timewindows_time_10\"></div");
		if (timewindows_bool_vector[11]) hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 850px; left: 45px;\"><input type=\"text\" size=\"2\" maxlength=\"6\" name=\"form_timewindows_time_11\" value=\"%0.2f\"></div",timewindows_time[11]);
		else hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 850px; left: 45px;\"><input type=\"text\" size=\"2\" maxlength=\"6\" name=\"form_timewindows_time_11\"></div");
		if (timewindows_bool_vector[12]) hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 872px; left: 45px;\"><input type=\"text\" size=\"2\" maxlength=\"6\" name=\"form_timewindows_time_12\" value=\"%0.2f\"></div",timewindows_time[12]);
		else hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 872px; left: 45px;\"><input type=\"text\" size=\"2\" maxlength=\"6\" name=\"form_timewindows_time_12\"></div");
		if (timewindows_bool_vector[13]) hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 894px; left: 45px;\"><input type=\"text\" size=\"2\" maxlength=\"6\" name=\"form_timewindows_time_13\" value=\"%0.2f\"></div",timewindows_time[13]);
		else hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 894px; left: 45px;\"><input type=\"text\" size=\"2\" maxlength=\"6\" name=\"form_timewindows_time_13\"></div");
	}
	
	hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 532px; left: 738px;\">Select waveform:  \
		<select NAME=\"form_graphic_select\" onchange='this.form.submit()'");
			hmStream.SSPrintf(HtmlTagStreamMode, "optgroup label=\"Current control, Ip > 0\"");
				if(graphic_select == 0) hmStream.SSPrintf(HtmlTagStreamMode, "option selected=\"selected\" value=0>Primary current, Ip > 0</option");	
				else hmStream.SSPrintf(HtmlTagStreamMode, "option value=0>Primary current, Ip > 0</option");	
				if(graphic_select == 1) hmStream.SSPrintf(HtmlTagStreamMode, "option selected=\"selected\" value=1>Vertical current, Ip > 0</option");
				else hmStream.SSPrintf(HtmlTagStreamMode, "option value=1>Vertical current, Ip > 0</option");
				if(graphic_select == 2) hmStream.SSPrintf(HtmlTagStreamMode, "option selected=\"selected\" value=2>Horizontal current, Ip > 0</option");
				else hmStream.SSPrintf(HtmlTagStreamMode, "option value=2>Horizontal current, Ip > 0</option");
			hmStream.SSPrintf(HtmlTagStreamMode, "/optgroup");	
			hmStream.SSPrintf(HtmlTagStreamMode, "optgroup label=\"Scenario control, Ip > 0\"");
				if(graphic_select == 3) hmStream.SSPrintf(HtmlTagStreamMode, "option selected=\"selected\" value=3>Plasma current, Ip > 0</option");						
				else hmStream.SSPrintf(HtmlTagStreamMode, "option value=3>Plasma current, Ip > 0</option");						
				if(graphic_select == 4) hmStream.SSPrintf(HtmlTagStreamMode, "option selected=\"selected\" value=4>Radial position, Ip > 0</option");
				else hmStream.SSPrintf(HtmlTagStreamMode, "option value=4>Radial position, Ip > 0</option");
				if(graphic_select == 5) hmStream.SSPrintf(HtmlTagStreamMode, "option selected=\"selected\" value=5>Vertical position, Ip > 0</option");
				else hmStream.SSPrintf(HtmlTagStreamMode, "option value=5>Vertical position, Ip > 0</option");
			hmStream.SSPrintf(HtmlTagStreamMode, "/optgroup");											
			hmStream.SSPrintf(HtmlTagStreamMode, "optgroup label=\"Current control, Ip < 0\"");				
				if(graphic_select == 6) hmStream.SSPrintf(HtmlTagStreamMode, "option selected=\"selected\" value=6>Primary current, Ip < 0</option");	
				else hmStream.SSPrintf(HtmlTagStreamMode, "option value=6>Primary current, Ip < 0</option");	
				if(graphic_select == 7) hmStream.SSPrintf(HtmlTagStreamMode, "option selected=\"selected\" value=7>Vertical current, Ip < 0</option");
				else hmStream.SSPrintf(HtmlTagStreamMode, "option value=7>Vertical current, Ip < 0</option");
				if(graphic_select == 8) hmStream.SSPrintf(HtmlTagStreamMode, "option selected=\"selected\" value=8>Horizontal current, Ip < 0</option");
				else hmStream.SSPrintf(HtmlTagStreamMode, "option value=8>Horizontal current, Ip < 0</option");
			hmStream.SSPrintf(HtmlTagStreamMode, "/optgroup");	
			hmStream.SSPrintf(HtmlTagStreamMode, "optgroup label=\"Scenario control, Ip < 0\"");	
				if(graphic_select == 9) hmStream.SSPrintf(HtmlTagStreamMode, "option selected=\"selected\" value=9>Plasma current, Ip < 0</option");	
				else hmStream.SSPrintf(HtmlTagStreamMode, "option value=9>Plasma current, Ip < 0</option");	
				if(graphic_select == 10) hmStream.SSPrintf(HtmlTagStreamMode, "option selected=\"selected\" value=10>Radial position, Ip < 0</option");
				else hmStream.SSPrintf(HtmlTagStreamMode, "option value=10>Radial position, Ip < 0</option");
				if(graphic_select == 11) hmStream.SSPrintf(HtmlTagStreamMode, "option selected=\"selected\" value=11>Vertical position, Ip < 0</option");
				else hmStream.SSPrintf(HtmlTagStreamMode, "option value=11>Vertical position, Ip < 0</option");
			hmStream.SSPrintf(HtmlTagStreamMode, "/optgroup");	
		hmStream.SSPrintf(HtmlTagStreamMode, "/select");	
	hmStream.SSPrintf(HtmlTagStreamMode, "/div");

	float xmin,ymin,xmax,ymax,majortickx,majorticky;
	int number_of_sub_ticks_x,number_of_sub_ticks_y;
	
	if(graphic_select == 0){
		DualVectorSort(primary_1_p_vector_size, &primary_1_p_index_vector[0], &primary_1_p_data_vector[0]);
		RemoveRepeatedValues(&primary_1_p_vector_size, &primary_1_p_index_vector[0], &primary_1_p_data_vector[0]);
		
		this->graphics_support->OptimalDisplayFromVectors(primary_1_p_vector_size, &primary_1_p_index_vector[0], &primary_1_p_data_vector[0],&xmin,&ymin,&xmax,&ymax,&majortickx,&majorticky,&number_of_sub_ticks_x,&number_of_sub_ticks_y);
		this->graphics_support->GetGraphSVG(&temp_string_1,(char *) this->Name(),primary_1_p_vector_size,&primary_1_p_index_vector[0],&primary_1_p_data_vector[0], 560, 350,xmin,ymin,xmax,ymax,majortickx,majorticky,number_of_sub_ticks_x,number_of_sub_ticks_y, True,True);
		
		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 562px; left: 520px;\"");
			hmStream.SSPrintf(HtmlTagStreamMode, temp_string_1.Buffer());
		hmStream.SSPrintf(HtmlTagStreamMode, "/div");
		
		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 562px; left: 1085px;\"");
			hmStream.SSPrintf(HtmlTagStreamMode, "select NAME=\"form_select_waveform_points_0\" size=\"14\" onchange='this.form.submit()'");
				for (i=0;i<primary_1_p_vector_size;i++) hmStream.SSPrintf(HtmlTagStreamMode, "option value=%d>%.2f | %.2f</option",i,primary_1_p_index_vector[i],primary_1_p_data_vector[i]);
			hmStream.SSPrintf(HtmlTagStreamMode, "/select");	
		hmStream.SSPrintf(HtmlTagStreamMode, "/div");

		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 802px; left: 1085px;\"");
		hmStream.Printf("Delete points");
		if (delete_selected_points_bool) hmStream.SSPrintf(HtmlTagStreamMode, "input type=\"checkbox\" name=\"form_delete_selected_points_bool\" CHECKED"); 
		else hmStream.SSPrintf(HtmlTagStreamMode, "input type=\"checkbox\" name=\"form_delete_selected_points_bool\"");
		hmStream.SSPrintf(HtmlTagStreamMode, "/div");

		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 837px; left: 1105px;\">t =<input type=\"text\" size=\"2\" maxlength=\"7\" name=\"form_insert_point_x_0\" id=\"form_insert_point_x_0\" value=%.2f></div",box_display_point_x);
//		if (box_display_point_x == 0) hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 837px; left: 1105px;\">t =<input type=\"text\" size=\"2\" maxlength=\"7\" name=\"form_insert_point_x_0\"></div");
//		else hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 837px; left: 1105px;\">t =<input type=\"text\" size=\"2\" maxlength=\"7\" name=\"form_insert_point_x_0\" value=%.2f></div",box_display_point_x);
//		if (box_display_point_y == 0) hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 862px; left: 1105px;\">y=<input type=\"text\" size=\"2\" maxlength=\"7\" name=\"form_insert_point_y_0\"></div");
//		else hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 862px; left: 1105px;\">y=<input type=\"text\" size=\"2\" maxlength=\"7\" name=\"form_insert_point_y_0\" value=%.2f></div",box_display_point_y);
		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 862px; left: 1105px;\">y=<input type=\"text\" size=\"2\" maxlength=\"7\" name=\"form_insert_point_y_0\" id=\"form_insert_point_y_0\"></div");

		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 886px; left: 1105px;\"");
		hmStream.SSPrintf(HtmlTagStreamMode, "input type=\"submit\" name=\"submit_insert_point_0\" value=\"Add point\"");
		hmStream.SSPrintf(HtmlTagStreamMode, "/div");
		
		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 522px; left: 1085px;\">Max: %.2f A</div",primary_1_p_max_value);
		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 542px; left: 1085px;\">Min: %.2f A</div",primary_1_p_min_value);
	}

	if(graphic_select == 1){
		DualVectorSort(vertical_1_p_vector_size, &vertical_1_p_index_vector[0], &vertical_1_p_data_vector[0]);
		RemoveRepeatedValues(&vertical_1_p_vector_size, &vertical_1_p_index_vector[0], &vertical_1_p_data_vector[0]);
		
		this->graphics_support->OptimalDisplayFromVectors(vertical_1_p_vector_size, &vertical_1_p_index_vector[0], &vertical_1_p_data_vector[0],&xmin,&ymin,&xmax,&ymax,&majortickx,&majorticky,&number_of_sub_ticks_x,&number_of_sub_ticks_y);
		this->graphics_support->GetGraphSVG(&temp_string_1,(char *) this->Name(),vertical_1_p_vector_size,&vertical_1_p_index_vector[0],&vertical_1_p_data_vector[0], 560, 350,xmin,ymin,xmax,ymax,majortickx,majorticky,number_of_sub_ticks_x,number_of_sub_ticks_y, True,True);
		
		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 562px; left: 520px;\"");
			hmStream.SSPrintf(HtmlTagStreamMode, temp_string_1.Buffer());
		hmStream.SSPrintf(HtmlTagStreamMode, "/div");

		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 562px; left: 1085px;\"");
			hmStream.SSPrintf(HtmlTagStreamMode, "select NAME=\"form_select_waveform_points_1\" size=\"14\" onchange='this.form.submit()'");
				for (i=0;i<vertical_1_p_vector_size;i++) hmStream.SSPrintf(HtmlTagStreamMode, "option value=%d>%.2f | %.2f</option",i,vertical_1_p_index_vector[i],vertical_1_p_data_vector[i]);
			hmStream.SSPrintf(HtmlTagStreamMode, "/select");	
		hmStream.SSPrintf(HtmlTagStreamMode, "/div");

		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 802px; left: 1085px;\"");
		hmStream.Printf("Delete points");
		if (delete_selected_points_bool) hmStream.SSPrintf(HtmlTagStreamMode, "input type=\"checkbox\" name=\"form_delete_selected_points_bool\" CHECKED"); 
		else hmStream.SSPrintf(HtmlTagStreamMode, "input type=\"checkbox\" name=\"form_delete_selected_points_bool\"");
		hmStream.SSPrintf(HtmlTagStreamMode, "/div");
	
		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 837px; left: 1105px;\">t =<input type=\"text\" size=\"2\" maxlength=\"7\" name=\"form_insert_point_x_1\" id=\"form_insert_point_x_1\" value=%.2f></div",box_display_point_x);
//		if (box_display_point_x == 0) hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 837px; left: 1105px;\">t =<input type=\"text\" size=\"2\" maxlength=\"7\" name=\"form_insert_point_x_1\"></div");
//		else hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 837px; left: 1105px;\">t =<input type=\"text\" size=\"2\" maxlength=\"7\" name=\"form_insert_point_x_1\" value=%.2f></div",box_display_point_x);
//		if (box_display_point_y == 0) hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 862px; left: 1105px;\">y=<input type=\"text\" size=\"2\" maxlength=\"7\" name=\"form_insert_point_y_1\"></div");
//		else hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 862px; left: 1105px;\">y=<input type=\"text\" size=\"2\" maxlength=\"7\" name=\"form_insert_point_y_1\" value=%.2f></div",box_display_point_y);
		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 862px; left: 1105px;\">y=<input type=\"text\" size=\"2\" maxlength=\"7\" name=\"form_insert_point_y_1\" id=\"form_insert_point_y_1\"></div");


		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 886px; left: 1105px;\"");
		hmStream.SSPrintf(HtmlTagStreamMode, "input type=\"submit\" name=\"submit_insert_point_1\" value=\"Add point\"");
		hmStream.SSPrintf(HtmlTagStreamMode, "/div");
		
		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 522px; left: 1085px;\">Max: %.2f A</div",vertical_1_p_max_value);
		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 542px; left: 1085px;\">Min: %.2f A</div",vertical_1_p_min_value);
	}
	 
	if(graphic_select == 2){
		DualVectorSort(horizontal_1_p_vector_size, &horizontal_1_p_index_vector[0], &horizontal_1_p_data_vector[0]);
		RemoveRepeatedValues(&horizontal_1_p_vector_size, &horizontal_1_p_index_vector[0], &horizontal_1_p_data_vector[0]);
		
		this->graphics_support->OptimalDisplayFromVectors(horizontal_1_p_vector_size, &horizontal_1_p_index_vector[0], &horizontal_1_p_data_vector[0],&xmin,&ymin,&xmax,&ymax,&majortickx,&majorticky,&number_of_sub_ticks_x,&number_of_sub_ticks_y);
		this->graphics_support->GetGraphSVG(&temp_string_1,(char *) this->Name(),horizontal_1_p_vector_size,&horizontal_1_p_index_vector[0],&horizontal_1_p_data_vector[0], 560, 350,xmin,ymin,xmax,ymax,majortickx,majorticky,number_of_sub_ticks_x,number_of_sub_ticks_y, True,True);
		
		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 562px; left: 520px;\"");
			hmStream.SSPrintf(HtmlTagStreamMode, temp_string_1.Buffer());
		hmStream.SSPrintf(HtmlTagStreamMode, "/div");

		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 562px; left: 1085px;\"");
			hmStream.SSPrintf(HtmlTagStreamMode, "select NAME=\"form_select_waveform_points_2\" size=\"14\" onchange='this.form.submit()'");
				for (i=0;i<horizontal_1_p_vector_size;i++) hmStream.SSPrintf(HtmlTagStreamMode, "option value=%d>%.2f | %.2f</option",i,horizontal_1_p_index_vector[i],horizontal_1_p_data_vector[i]);
			hmStream.SSPrintf(HtmlTagStreamMode, "/select");	
		hmStream.SSPrintf(HtmlTagStreamMode, "/div");

		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 802px; left: 1085px;\"");
		hmStream.Printf("Delete points");
		if (delete_selected_points_bool) hmStream.SSPrintf(HtmlTagStreamMode, "input type=\"checkbox\" name=\"form_delete_selected_points_bool\" CHECKED"); 
		else hmStream.SSPrintf(HtmlTagStreamMode, "input type=\"checkbox\" name=\"form_delete_selected_points_bool\"");
		hmStream.SSPrintf(HtmlTagStreamMode, "/div");

		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 837px; left: 1105px;\">t =<input type=\"text\" size=\"2\" maxlength=\"7\" name=\"form_insert_point_x_2\" id=\"form_insert_point_x_2\" value=%.2f></div",box_display_point_x);
//		if (box_display_point_x == 0) hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 837px; left: 1105px;\">t =<input type=\"text\" size=\"2\" maxlength=\"7\" name=\"form_insert_point_x_2\"></div");
//		else hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 837px; left: 1105px;\">t =<input type=\"text\" size=\"2\" maxlength=\"7\" name=\"form_insert_point_x_2\" value=%.2f></div",box_display_point_x);
//		if (box_display_point_y == 0) hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 862px; left: 1105px;\">y=<input type=\"text\" size=\"2\" maxlength=\"7\" name=\"form_insert_point_y_2\"></div");
//		else hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 862px; left: 1105px;\">y=<input type=\"text\" size=\"2\" maxlength=\"7\" name=\"form_insert_point_y_2\" value=%.2f></div",box_display_point_y);
		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 862px; left: 1105px;\">y=<input type=\"text\" size=\"2\" maxlength=\"7\" name=\"form_insert_point_y_2\" id=\"form_insert_point_y_2\"></div");

		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 886px; left: 1105px;\"");
		hmStream.SSPrintf(HtmlTagStreamMode, "input type=\"submit\" name=\"submit_insert_point_2\" value=\"Add point\"");
		hmStream.SSPrintf(HtmlTagStreamMode, "/div");
		
		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 522px; left: 1085px;\">Max: %.2f A</div",horizontal_1_p_max_value);
		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 542px; left: 1085px;\">Min: %.2f A</div",horizontal_1_p_min_value);
	}

	if(graphic_select == 3){
		DualVectorSort(primary_2_p_vector_size, &primary_2_p_index_vector[0], &primary_2_p_data_vector[0]);
		RemoveRepeatedValues(&primary_2_p_vector_size, &primary_2_p_index_vector[0], &primary_2_p_data_vector[0]);
		
		this->graphics_support->OptimalDisplayFromVectors(primary_2_p_vector_size, &primary_2_p_index_vector[0], &primary_2_p_data_vector[0],&xmin,&ymin,&xmax,&ymax,&majortickx,&majorticky,&number_of_sub_ticks_x,&number_of_sub_ticks_y);
		this->graphics_support->GetGraphSVG(&temp_string_1,(char *) this->Name(),primary_2_p_vector_size,&primary_2_p_index_vector[0],&primary_2_p_data_vector[0], 560, 350,xmin,ymin,xmax,ymax,majortickx,majorticky,number_of_sub_ticks_x,number_of_sub_ticks_y, True,True);
		
		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 562px; left: 520px;\"");
			hmStream.SSPrintf(HtmlTagStreamMode, temp_string_1.Buffer());
		hmStream.SSPrintf(HtmlTagStreamMode, "/div");

		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 562px; left: 1085px;\"");
			hmStream.SSPrintf(HtmlTagStreamMode, "select NAME=\"form_select_waveform_points_3\" size=\"14\" onchange='this.form.submit()'");
				for (i=0;i<primary_2_p_vector_size;i++) hmStream.SSPrintf(HtmlTagStreamMode, "option value=%d>%.2f | %.2f</option",i,primary_2_p_index_vector[i],primary_2_p_data_vector[i]);
			hmStream.SSPrintf(HtmlTagStreamMode, "/select");	
		hmStream.SSPrintf(HtmlTagStreamMode, "/div");

		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 802px; left: 1085px;\"");
		hmStream.Printf("Delete points");
		if (delete_selected_points_bool) hmStream.SSPrintf(HtmlTagStreamMode, "input type=\"checkbox\" name=\"form_delete_selected_points_bool\" CHECKED"); 
		else hmStream.SSPrintf(HtmlTagStreamMode, "input type=\"checkbox\" name=\"form_delete_selected_points_bool\"");
		hmStream.SSPrintf(HtmlTagStreamMode, "/div");

		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 837px; left: 1105px;\">t =<input type=\"text\" size=\"2\" maxlength=\"7\" name=\"form_insert_point_x_3\" id=\"form_insert_point_x_3\" value=%.2f></div",box_display_point_x);
//		if (box_display_point_x == 0) hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 837px; left: 1105px;\">t =<input type=\"text\" size=\"2\" maxlength=\"7\" name=\"form_insert_point_x_3\"></div");
//		else hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 837px; left: 1105px;\">t =<input type=\"text\" size=\"2\" maxlength=\"7\" name=\"form_insert_point_x_3\" value=%.2f></div",box_display_point_x);
//		if (box_display_point_y == 0) hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 862px; left: 1105px;\">y=<input type=\"text\" size=\"2\" maxlength=\"7\" name=\"form_insert_point_y_3\"></div");
//		else hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 862px; left: 1105px;\">y=<input type=\"text\" size=\"2\" maxlength=\"7\" name=\"form_insert_point_y_3\" value=%.2f></div",box_display_point_y);
		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 862px; left: 1105px;\">y=<input type=\"text\" size=\"2\" maxlength=\"7\" name=\"form_insert_point_y_3\" id=\"form_insert_point_y_3\"></div");

		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 886px; left: 1105px;\"");
		hmStream.SSPrintf(HtmlTagStreamMode, "input type=\"submit\" name=\"submit_insert_point_3\" value=\"Add point\"");
		hmStream.SSPrintf(HtmlTagStreamMode, "/div");
		
		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 522px; left: 1085px;\">Max: %.2f A</div",primary_2_p_max_value);
		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 542px; left: 1085px;\">Min: %.2f A</div",primary_2_p_min_value);
	}

	if(graphic_select == 4){
		DualVectorSort(vertical_2_p_vector_size, &vertical_2_p_index_vector[0], &vertical_2_p_data_vector[0]);
		RemoveRepeatedValues(&vertical_2_p_vector_size, &vertical_2_p_index_vector[0], &vertical_2_p_data_vector[0]);
		
		this->graphics_support->OptimalDisplayFromVectors(vertical_2_p_vector_size, &vertical_2_p_index_vector[0], &vertical_2_p_data_vector[0],&xmin,&ymin,&xmax,&ymax,&majortickx,&majorticky,&number_of_sub_ticks_x,&number_of_sub_ticks_y);
		this->graphics_support->GetGraphSVG(&temp_string_1,(char *) this->Name(),vertical_2_p_vector_size,&vertical_2_p_index_vector[0],&vertical_2_p_data_vector[0], 560, 350,xmin,ymin,xmax,ymax,majortickx,majorticky,number_of_sub_ticks_x,number_of_sub_ticks_y, True,True);
		
		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 562px; left: 520px;\"");
			hmStream.SSPrintf(HtmlTagStreamMode, temp_string_1.Buffer());
		hmStream.SSPrintf(HtmlTagStreamMode, "/div");

		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 562px; left: 1085px;\"");
			hmStream.SSPrintf(HtmlTagStreamMode, "select NAME=\"form_select_waveform_points_4\" size=\"14\" onchange='this.form.submit()'");
				for (i=0;i<vertical_2_p_vector_size;i++) hmStream.SSPrintf(HtmlTagStreamMode, "option value=%d>%.2f | %.2f</option",i,vertical_2_p_index_vector[i],vertical_2_p_data_vector[i]);
			hmStream.SSPrintf(HtmlTagStreamMode, "/select");	
		hmStream.SSPrintf(HtmlTagStreamMode, "/div");

		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 802px; left: 1085px;\"");
		hmStream.Printf("Delete points");
		if (delete_selected_points_bool) hmStream.SSPrintf(HtmlTagStreamMode, "input type=\"checkbox\" name=\"form_delete_selected_points_bool\" CHECKED"); 
		else hmStream.SSPrintf(HtmlTagStreamMode, "input type=\"checkbox\" name=\"form_delete_selected_points_bool\"");
		hmStream.SSPrintf(HtmlTagStreamMode, "/div");

		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 837px; left: 1105px;\">t =<input type=\"text\" size=\"2\" maxlength=\"7\" name=\"form_insert_point_x_4\" id=\"form_insert_point_x_4\" value=%.2f></div",box_display_point_x);
//		if (box_display_point_x == 0) hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 837px; left: 1105px;\">t =<input type=\"text\" size=\"2\" maxlength=\"7\" name=\"form_insert_point_x_4\"></div");
//		else hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 837px; left: 1105px;\">t =<input type=\"text\" size=\"2\" maxlength=\"7\" name=\"form_insert_point_x_4\" value=%.2f></div",box_display_point_x);
//		if (box_display_point_y == 0) hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 862px; left: 1105px;\">y=<input type=\"text\" size=\"2\" maxlength=\"7\" name=\"form_insert_point_y_4\"></div");
//		else hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 862px; left: 1105px;\">y=<input type=\"text\" size=\"2\" maxlength=\"7\" name=\"form_insert_point_y_4\" value=%.2f></div",box_display_point_y);
		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 862px; left: 1105px;\">y=<input type=\"text\" size=\"2\" maxlength=\"7\" name=\"form_insert_point_y_4\" id=\"form_insert_point_y_4\"></div");

		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 886px; left: 1105px;\"");
		hmStream.SSPrintf(HtmlTagStreamMode, "input type=\"submit\" name=\"submit_insert_point_4\" value=\"Add point\"");
		hmStream.SSPrintf(HtmlTagStreamMode, "/div");
		
		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 522px; left: 1085px;\">Max: %.2f mm</div",vertical_2_p_max_value);
		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 542px; left: 1085px;\">Min: %.2f mm</div",vertical_2_p_min_value);
	}

	if(graphic_select == 5){
		DualVectorSort(horizontal_2_p_vector_size, &horizontal_2_p_index_vector[0], &horizontal_2_p_data_vector[0]);
		RemoveRepeatedValues(&horizontal_2_p_vector_size, &horizontal_2_p_index_vector[0], &horizontal_2_p_data_vector[0]);
		
		this->graphics_support->OptimalDisplayFromVectors(horizontal_2_p_vector_size, &horizontal_2_p_index_vector[0], &horizontal_2_p_data_vector[0],&xmin,&ymin,&xmax,&ymax,&majortickx,&majorticky,&number_of_sub_ticks_x,&number_of_sub_ticks_y);
		this->graphics_support->GetGraphSVG(&temp_string_1,(char *) this->Name(),horizontal_2_p_vector_size,&horizontal_2_p_index_vector[0],&horizontal_2_p_data_vector[0], 560, 350,xmin,ymin,xmax,ymax,majortickx,majorticky,number_of_sub_ticks_x,number_of_sub_ticks_y, True,True);
		
		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 562px; left: 520px;\"");
			hmStream.SSPrintf(HtmlTagStreamMode, temp_string_1.Buffer());
		hmStream.SSPrintf(HtmlTagStreamMode, "/div");

		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 562px; left: 1085px;\"");
			hmStream.SSPrintf(HtmlTagStreamMode, "select NAME=\"form_select_waveform_points_5\" size=\"14\" onchange='this.form.submit()'");
				for (i=0;i<horizontal_2_p_vector_size;i++) hmStream.SSPrintf(HtmlTagStreamMode, "option value=%d>%.2f | %.2f</option",i,horizontal_2_p_index_vector[i],horizontal_2_p_data_vector[i]);
			hmStream.SSPrintf(HtmlTagStreamMode, "/select");	
		hmStream.SSPrintf(HtmlTagStreamMode, "/div");

		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 802px; left: 1085px;\"");
		hmStream.Printf("Delete points");
		if (delete_selected_points_bool) hmStream.SSPrintf(HtmlTagStreamMode, "input type=\"checkbox\" name=\"form_delete_selected_points_bool\" CHECKED"); 
		else hmStream.SSPrintf(HtmlTagStreamMode, "input type=\"checkbox\" name=\"form_delete_selected_points_bool\"");
		hmStream.SSPrintf(HtmlTagStreamMode, "/div");

		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 837px; left: 1105px;\">t =<input type=\"text\" size=\"2\" maxlength=\"7\" name=\"form_insert_point_x_5\" id=\"form_insert_point_x_5\" value=%.2f></div",box_display_point_x);
//		if (box_display_point_x == 0) hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 837px; left: 1105px;\">t =<input type=\"text\" size=\"2\" maxlength=\"7\" name=\"form_insert_point_x_5\"></div");
//		else hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 837px; left: 1105px;\">t =<input type=\"text\" size=\"2\" maxlength=\"7\" name=\"form_insert_point_x_5\" value=%.2f></div",box_display_point_x);
//		if (box_display_point_y == 0) hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 862px; left: 1105px;\">y=<input type=\"text\" size=\"2\" maxlength=\"7\" name=\"form_insert_point_y_5\"></div");
//		else hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 862px; left: 1105px;\">y=<input type=\"text\" size=\"2\" maxlength=\"7\" name=\"form_insert_point_y_5\" value=%.2f></div",box_display_point_y);
		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 862px; left: 1105px;\">y=<input type=\"text\" size=\"2\" maxlength=\"7\" name=\"form_insert_point_y_5\" id=\"form_insert_point_y_5\"></div");

		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 886px; left: 1105px;\"");
		hmStream.SSPrintf(HtmlTagStreamMode, "input type=\"submit\" name=\"submit_insert_point_5\" value=\"Add point\"");
		hmStream.SSPrintf(HtmlTagStreamMode, "/div");
		
		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 522px; left: 1085px;\">Max: %.2f mm</div",horizontal_2_p_max_value);
		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 542px; left: 1085px;\">Min: %.2f mm</div",horizontal_2_p_min_value);
	}

	if(graphic_select == 6){
		DualVectorSort(primary_1_n_vector_size, &primary_1_n_index_vector[0], &primary_1_n_data_vector[0]);
		RemoveRepeatedValues(&primary_1_n_vector_size, &primary_1_n_index_vector[0], &primary_1_n_data_vector[0]);
		
		this->graphics_support->OptimalDisplayFromVectors(primary_1_n_vector_size, &primary_1_n_index_vector[0], &primary_1_n_data_vector[0],&xmin,&ymin,&xmax,&ymax,&majortickx,&majorticky,&number_of_sub_ticks_x,&number_of_sub_ticks_y);
		this->graphics_support->GetGraphSVG(&temp_string_1,(char *) this->Name(),primary_1_n_vector_size,&primary_1_n_index_vector[0],&primary_1_n_data_vector[0], 560, 350,xmin,ymin,xmax,ymax,majortickx,majorticky,number_of_sub_ticks_x,number_of_sub_ticks_y, True,True);
		
		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 562px; left: 520px;\"");
			hmStream.SSPrintf(HtmlTagStreamMode, temp_string_1.Buffer());
		hmStream.SSPrintf(HtmlTagStreamMode, "/div");

		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 562px; left: 1085px;\"");
			hmStream.SSPrintf(HtmlTagStreamMode, "select NAME=\"form_select_waveform_points_6\" size=\"14\" onchange='this.form.submit()'");
				for (i=0;i<primary_1_n_vector_size;i++) hmStream.SSPrintf(HtmlTagStreamMode, "option value=%d>%.2f | %.2f</option",i,primary_1_n_index_vector[i],primary_1_n_data_vector[i]);
			hmStream.SSPrintf(HtmlTagStreamMode, "/select");	
		hmStream.SSPrintf(HtmlTagStreamMode, "/div");

		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 802px; left: 1085px;\"");
		hmStream.Printf("Delete points");
		if (delete_selected_points_bool) hmStream.SSPrintf(HtmlTagStreamMode, "input type=\"checkbox\" name=\"form_delete_selected_points_bool\" CHECKED"); 
		else hmStream.SSPrintf(HtmlTagStreamMode, "input type=\"checkbox\" name=\"form_delete_selected_points_bool\"");
		hmStream.SSPrintf(HtmlTagStreamMode, "/div");

		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 837px; left: 1105px;\">t =<input type=\"text\" size=\"2\" maxlength=\"7\" name=\"form_insert_point_x_6\" id=\"form_insert_point_x_6\" value=%.2f></div",box_display_point_x);
//		if (box_display_point_x == 0) hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 837px; left: 1105px;\">t =<input type=\"text\" size=\"2\" maxlength=\"7\" name=\"form_insert_point_x_6\"></div");
//		else hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 837px; left: 1105px;\">t =<input type=\"text\" size=\"2\" maxlength=\"7\" name=\"form_insert_point_x_6\" value=%.2f></div",box_display_point_x);
//		if (box_display_point_y == 0) hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 862px; left: 1105px;\">y=<input type=\"text\" size=\"2\" maxlength=\"7\" name=\"form_insert_point_y_6\"></div");
//		else hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 862px; left: 1105px;\">y=<input type=\"text\" size=\"2\" maxlength=\"7\" name=\"form_insert_point_y_6\" value=%.2f></div",box_display_point_y);
		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 862px; left: 1105px;\">y=<input type=\"text\" size=\"2\" maxlength=\"7\" name=\"form_insert_point_y_6\" id=\"form_insert_point_y_6\"></div");

		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 886px; left: 1105px;\"");
		hmStream.SSPrintf(HtmlTagStreamMode, "input type=\"submit\" name=\"submit_insert_point_6\" value=\"Add point\"");
		hmStream.SSPrintf(HtmlTagStreamMode, "/div");
		
		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 522px; left: 1085px;\">Max: %.2f A</div",primary_1_p_max_value);
		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 542px; left: 1085px;\">Min: %.2f A</div",primary_1_p_min_value);
	}

	if(graphic_select == 7){
		DualVectorSort(vertical_1_n_vector_size, &vertical_1_n_index_vector[0], &vertical_1_n_data_vector[0]);
		RemoveRepeatedValues(&vertical_1_n_vector_size, &vertical_1_n_index_vector[0], &vertical_1_n_data_vector[0]);
		
		this->graphics_support->OptimalDisplayFromVectors(vertical_1_n_vector_size, &vertical_1_n_index_vector[0], &vertical_1_n_data_vector[0],&xmin,&ymin,&xmax,&ymax,&majortickx,&majorticky,&number_of_sub_ticks_x,&number_of_sub_ticks_y);
		this->graphics_support->GetGraphSVG(&temp_string_1,(char *) this->Name(),vertical_1_n_vector_size,&vertical_1_n_index_vector[0],&vertical_1_n_data_vector[0], 560, 350,xmin,ymin,xmax,ymax,majortickx,majorticky,number_of_sub_ticks_x,number_of_sub_ticks_y, True,True);
		
		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 562px; left: 520px;\"");
			hmStream.SSPrintf(HtmlTagStreamMode, temp_string_1.Buffer());
		hmStream.SSPrintf(HtmlTagStreamMode, "/div");

		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 562px; left: 1085px;\"");
			hmStream.SSPrintf(HtmlTagStreamMode, "select NAME=\"form_select_waveform_points_7\" size=\"14\" onchange='this.form.submit()'");
				for (i=0;i<vertical_1_n_vector_size;i++) hmStream.SSPrintf(HtmlTagStreamMode, "option value=%d>%.2f | %.2f</option",i,vertical_1_n_index_vector[i],vertical_1_n_data_vector[i]);
			hmStream.SSPrintf(HtmlTagStreamMode, "/select");	
		hmStream.SSPrintf(HtmlTagStreamMode, "/div");

		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 802px; left: 1085px;\"");
		hmStream.Printf("Delete points");
		if (delete_selected_points_bool) hmStream.SSPrintf(HtmlTagStreamMode, "input type=\"checkbox\" name=\"form_delete_selected_points_bool\" CHECKED"); 
		else hmStream.SSPrintf(HtmlTagStreamMode, "input type=\"checkbox\" name=\"form_delete_selected_points_bool\"");
		hmStream.SSPrintf(HtmlTagStreamMode, "/div");

		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 837px; left: 1105px;\">t =<input type=\"text\" size=\"2\" maxlength=\"7\" name=\"form_insert_point_x_7\" id=\"form_insert_point_x_7\" value=%.2f></div",box_display_point_x);
//		if (box_display_point_x == 0) hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 837px; left: 1105px;\">t =<input type=\"text\" size=\"2\" maxlength=\"7\" name=\"form_insert_point_x_7\"></div");
//		else hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 837px; left: 1105px;\">t =<input type=\"text\" size=\"2\" maxlength=\"7\" name=\"form_insert_point_x_7\" value=%.2f></div",box_display_point_x);
//		if (box_display_point_y == 0) hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 862px; left: 1105px;\">y=<input type=\"text\" size=\"2\" maxlength=\"7\" name=\"form_insert_point_y_7\"></div");
//		else hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 862px; left: 1105px;\">y=<input type=\"text\" size=\"2\" maxlength=\"7\" name=\"form_insert_point_y_7\" value=%.2f></div",box_display_point_y);
		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 862px; left: 1105px;\">y=<input type=\"text\" size=\"2\" maxlength=\"7\" name=\"form_insert_point_y_7\" id=\"form_insert_point_y_7\"></div");

		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 886px; left: 1105px;\"");
		hmStream.SSPrintf(HtmlTagStreamMode, "input type=\"submit\" name=\"submit_insert_point_7\" value=\"Add point\"");
		hmStream.SSPrintf(HtmlTagStreamMode, "/div");
		
		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 522px; left: 1085px;\">Max: %.2f A</div",vertical_1_p_max_value);
		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 542px; left: 1085px;\">Min: %.2f A</div",vertical_1_p_min_value);
	}

	if(graphic_select == 8){
		DualVectorSort(horizontal_1_n_vector_size, &horizontal_1_n_index_vector[0], &horizontal_1_n_data_vector[0]);
		RemoveRepeatedValues(&horizontal_1_n_vector_size, &horizontal_1_n_index_vector[0], &horizontal_1_n_data_vector[0]);
		
		this->graphics_support->OptimalDisplayFromVectors(horizontal_1_n_vector_size, &horizontal_1_n_index_vector[0], &horizontal_1_n_data_vector[0],&xmin,&ymin,&xmax,&ymax,&majortickx,&majorticky,&number_of_sub_ticks_x,&number_of_sub_ticks_y);
		this->graphics_support->GetGraphSVG(&temp_string_1,(char *) this->Name(),horizontal_1_n_vector_size,&horizontal_1_n_index_vector[0],&horizontal_1_n_data_vector[0], 560, 350,xmin,ymin,xmax,ymax,majortickx,majorticky,number_of_sub_ticks_x,number_of_sub_ticks_y, True,True);
		
		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 562px; left: 520px;\"");
			hmStream.SSPrintf(HtmlTagStreamMode, temp_string_1.Buffer());
		hmStream.SSPrintf(HtmlTagStreamMode, "/div");

		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 562px; left: 1085px;\"");
			hmStream.SSPrintf(HtmlTagStreamMode, "select NAME=\"form_select_waveform_points_8\" size=\"14\" onchange='this.form.submit()'");
				for (i=0;i<horizontal_1_n_vector_size;i++) hmStream.SSPrintf(HtmlTagStreamMode, "option value=%d>%.2f | %.2f</option",i,horizontal_1_n_index_vector[i],horizontal_1_n_data_vector[i]);
			hmStream.SSPrintf(HtmlTagStreamMode, "/select");	
		hmStream.SSPrintf(HtmlTagStreamMode, "/div");

		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 802px; left: 1085px;\"");
		hmStream.Printf("Delete points");
		if (delete_selected_points_bool) hmStream.SSPrintf(HtmlTagStreamMode, "input type=\"checkbox\" name=\"form_delete_selected_points_bool\" CHECKED"); 
		else hmStream.SSPrintf(HtmlTagStreamMode, "input type=\"checkbox\" name=\"form_delete_selected_points_bool\"");
		hmStream.SSPrintf(HtmlTagStreamMode, "/div");

		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 837px; left: 1105px;\">t =<input type=\"text\" size=\"2\" maxlength=\"7\" name=\"form_insert_point_x_8\" id=\"form_insert_point_x_8\" value=%.2f></div",box_display_point_x);
//		if (box_display_point_x == 0) hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 837px; left: 1105px;\">t =<input type=\"text\" size=\"2\" maxlength=\"7\" name=\"form_insert_point_x_8\"></div");
//		else hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 837px; left: 1105px;\">t =<input type=\"text\" size=\"2\" maxlength=\"7\" name=\"form_insert_point_x_8\" value=%.2f></div",box_display_point_x);
//		if (box_display_point_y == 0) hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 862px; left: 1105px;\">y=<input type=\"text\" size=\"2\" maxlength=\"7\" name=\"form_insert_point_y_8\"></div");
//		else hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 862px; left: 1105px;\">y=<input type=\"text\" size=\"2\" maxlength=\"7\" name=\"form_insert_point_y_8\" value=%.2f></div",box_display_point_y);
		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 862px; left: 1105px;\">y=<input type=\"text\" size=\"2\" maxlength=\"7\" name=\"form_insert_point_y_8\" id=\"form_insert_point_y_8\"></div");

		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 886px; left: 1105px;\"");
		hmStream.SSPrintf(HtmlTagStreamMode, "input type=\"submit\" name=\"submit_insert_point_8\" value=\"Add point\"");
		hmStream.SSPrintf(HtmlTagStreamMode, "/div");
		
		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 522px; left: 1085px;\">Max: %.2f A</div",horizontal_1_p_max_value);
		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 542px; left: 1085px;\">Min: %.2f A</div",horizontal_1_p_min_value);
	}

	if(graphic_select == 9){
		DualVectorSort(primary_2_n_vector_size, &primary_2_n_index_vector[0], &primary_2_n_data_vector[0]);
		RemoveRepeatedValues(&primary_2_n_vector_size, &primary_2_n_index_vector[0], &primary_2_n_data_vector[0]);
		
		this->graphics_support->OptimalDisplayFromVectors(primary_2_n_vector_size, &primary_2_n_index_vector[0], &primary_2_n_data_vector[0],&xmin,&ymin,&xmax,&ymax,&majortickx,&majorticky,&number_of_sub_ticks_x,&number_of_sub_ticks_y);
		this->graphics_support->GetGraphSVG(&temp_string_1,(char *) this->Name(),primary_2_n_vector_size,&primary_2_n_index_vector[0],&primary_2_n_data_vector[0], 560, 350,xmin,ymin,xmax,ymax,majortickx,majorticky,number_of_sub_ticks_x,number_of_sub_ticks_y, True,True);
		
		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 562px; left: 520px;\"");
			hmStream.SSPrintf(HtmlTagStreamMode, temp_string_1.Buffer());
		hmStream.SSPrintf(HtmlTagStreamMode, "/div");

		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 562px; left: 1085px;\"");
			hmStream.SSPrintf(HtmlTagStreamMode, "select NAME=\"form_select_waveform_points_9\" size=\"14\" onchange='this.form.submit()'");
				for (i=0;i<primary_2_n_vector_size;i++) hmStream.SSPrintf(HtmlTagStreamMode, "option value=%d>%.2f | %.2f</option",i,primary_2_n_index_vector[i],primary_2_n_data_vector[i]);
			hmStream.SSPrintf(HtmlTagStreamMode, "/select");	
		hmStream.SSPrintf(HtmlTagStreamMode, "/div");

		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 802px; left: 1085px;\"");
		hmStream.Printf("Delete points");
		if (delete_selected_points_bool) hmStream.SSPrintf(HtmlTagStreamMode, "input type=\"checkbox\" name=\"form_delete_selected_points_bool\" CHECKED"); 
		else hmStream.SSPrintf(HtmlTagStreamMode, "input type=\"checkbox\" name=\"form_delete_selected_points_bool\"");
		hmStream.SSPrintf(HtmlTagStreamMode, "/div");

		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 837px; left: 1105px;\">t =<input type=\"text\" size=\"2\" maxlength=\"7\" name=\"form_insert_point_x_9\" id=\"form_insert_point_x_9\" value=%.2f></div",box_display_point_x);
//		if (box_display_point_x == 0) hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 837px; left: 1105px;\">t =<input type=\"text\" size=\"2\" maxlength=\"7\" name=\"form_insert_point_x_9\"></div");
//		else hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 837px; left: 1105px;\">t =<input type=\"text\" size=\"2\" maxlength=\"7\" name=\"form_insert_point_x_9\" value=%.2f></div",box_display_point_x);
//		if (box_display_point_y == 0) hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 862px; left: 1105px;\">y=<input type=\"text\" size=\"2\" maxlength=\"7\" name=\"form_insert_point_y_9\"></div");
//		else hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 862px; left: 1105px;\">y=<input type=\"text\" size=\"2\" maxlength=\"7\" name=\"form_insert_point_y_9\" value=%.2f></div",box_display_point_y);
		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 862px; left: 1105px;\">y=<input type=\"text\" size=\"2\" maxlength=\"7\" name=\"form_insert_point_y_9\" id=\"form_insert_point_y_9\"></div");

		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 886px; left: 1105px;\"");
		hmStream.SSPrintf(HtmlTagStreamMode, "input type=\"submit\" name=\"submit_insert_point_9\" value=\"Add point\"");
		hmStream.SSPrintf(HtmlTagStreamMode, "/div");
		
		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 522px; left: 1085px;\">Max: %.2f A</div",primary_2_p_max_value);
		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 542px; left: 1085px;\">Min: %.2f A</div",primary_2_p_min_value);
	}

	if(graphic_select == 10){
		DualVectorSort(vertical_2_n_vector_size, &vertical_2_n_index_vector[0], &vertical_2_n_data_vector[0]);
		RemoveRepeatedValues(&vertical_2_n_vector_size, &vertical_2_n_index_vector[0], &vertical_2_n_data_vector[0]);
		
		this->graphics_support->OptimalDisplayFromVectors(vertical_2_n_vector_size, &vertical_2_n_index_vector[0], &vertical_2_n_data_vector[0],&xmin,&ymin,&xmax,&ymax,&majortickx,&majorticky,&number_of_sub_ticks_x,&number_of_sub_ticks_y);
		this->graphics_support->GetGraphSVG(&temp_string_1,(char *) this->Name(),vertical_2_n_vector_size,&vertical_2_n_index_vector[0],&vertical_2_n_data_vector[0], 560, 350,xmin,ymin,xmax,ymax,majortickx,majorticky,number_of_sub_ticks_x,number_of_sub_ticks_y, True,True);
		
		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 562px; left: 520px;\"");
			hmStream.SSPrintf(HtmlTagStreamMode, temp_string_1.Buffer());
		hmStream.SSPrintf(HtmlTagStreamMode, "/div");

		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 562px; left: 1085px;\"");
			hmStream.SSPrintf(HtmlTagStreamMode, "select NAME=\"form_select_waveform_points_10\" size=\"14\" onchange='this.form.submit()'");
				for (i=0;i<vertical_2_n_vector_size;i++) hmStream.SSPrintf(HtmlTagStreamMode, "option value=%d>%.2f | %.2f</option",i,vertical_2_n_index_vector[i],vertical_2_n_data_vector[i]);
			hmStream.SSPrintf(HtmlTagStreamMode, "/select");	
		hmStream.SSPrintf(HtmlTagStreamMode, "/div");

		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 802px; left: 1085px;\"");
		hmStream.Printf("Delete points");
		if (delete_selected_points_bool) hmStream.SSPrintf(HtmlTagStreamMode, "input type=\"checkbox\" name=\"form_delete_selected_points_bool\" CHECKED"); 
		else hmStream.SSPrintf(HtmlTagStreamMode, "input type=\"checkbox\" name=\"form_delete_selected_points_bool\"");
		hmStream.SSPrintf(HtmlTagStreamMode, "/div");

		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 837px; left: 1105px;\">t =<input type=\"text\" size=\"2\" maxlength=\"7\" name=\"form_insert_point_x_10\" id=\"form_insert_point_x_10\" value=%.2f></div",box_display_point_x);
//		if (box_display_point_x == 0) hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 837px; left: 1105px;\">t =<input type=\"text\" size=\"2\" maxlength=\"7\" name=\"form_insert_point_x_10\" id=\"form_insert_point_x_10\"></div");
//		else hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 837px; left: 1105px;\">t =<input type=\"text\" size=\"2\" maxlength=\"7\" name=\"form_insert_point_x_10\" value=%.2f></div",box_display_point_x);
//		if (box_display_point_y == 0) hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 862px; left: 1105px;\">y=<input type=\"text\" size=\"2\" maxlength=\"7\" name=\"form_insert_point_y_10\"></div");
//		else hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 862px; left: 1105px;\">y=<input type=\"text\" size=\"2\" maxlength=\"7\" name=\"form_insert_point_y_10\" value=%.2f></div",box_display_point_y);
		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 862px; left: 1105px;\">y=<input type=\"text\" size=\"2\" maxlength=\"7\" name=\"form_insert_point_y_10\" id=\"form_insert_point_y_10\"></div");

		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 886px; left: 1105px;\"");
		hmStream.SSPrintf(HtmlTagStreamMode, "input type=\"submit\" name=\"submit_insert_point_10\" value=\"Add point\"");
		hmStream.SSPrintf(HtmlTagStreamMode, "/div");

		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 522px; left: 1085px;\">Max: %.2f mm</div",vertical_2_p_max_value);
		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 542px; left: 1085px;\">Min: %.2f mm</div",vertical_2_p_min_value);
	}

	if(graphic_select == 11){
		DualVectorSort(horizontal_2_n_vector_size, &horizontal_2_n_index_vector[0], &horizontal_2_n_data_vector[0]);
		RemoveRepeatedValues(&horizontal_2_n_vector_size, &horizontal_2_n_index_vector[0], &horizontal_2_n_data_vector[0]);
		
		this->graphics_support->OptimalDisplayFromVectors(horizontal_2_n_vector_size, &horizontal_2_n_index_vector[0], &horizontal_2_n_data_vector[0],&xmin,&ymin,&xmax,&ymax,&majortickx,&majorticky,&number_of_sub_ticks_x,&number_of_sub_ticks_y);
		this->graphics_support->GetGraphSVG(&temp_string_1,(char *) this->Name(),horizontal_2_n_vector_size,&horizontal_2_n_index_vector[0],&horizontal_2_n_data_vector[0], 560, 350,xmin,ymin,xmax,ymax,majortickx,majorticky,number_of_sub_ticks_x,number_of_sub_ticks_y, True,True);
		
		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 562px; left: 520px;\"");
			hmStream.SSPrintf(HtmlTagStreamMode, temp_string_1.Buffer());
		hmStream.SSPrintf(HtmlTagStreamMode, "/div");

		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 562px; left: 1085px;\"");
			hmStream.SSPrintf(HtmlTagStreamMode, "select NAME=\"form_select_waveform_points_11\" size=\"14\" onchange='this.form.submit()'");
				for (i=0;i<horizontal_2_n_vector_size;i++) hmStream.SSPrintf(HtmlTagStreamMode, "option value=%d>%.2f | %.2f</option",i,horizontal_2_n_index_vector[i],horizontal_2_n_data_vector[i]);
			hmStream.SSPrintf(HtmlTagStreamMode, "/select");	
		hmStream.SSPrintf(HtmlTagStreamMode, "/div");

		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 802px; left: 1085px;\"");
		hmStream.Printf("Delete points");
		if (delete_selected_points_bool) hmStream.SSPrintf(HtmlTagStreamMode, "input type=\"checkbox\" name=\"form_delete_selected_points_bool\" CHECKED"); 
		else hmStream.SSPrintf(HtmlTagStreamMode, "input type=\"checkbox\" name=\"form_delete_selected_points_bool\"");
		hmStream.SSPrintf(HtmlTagStreamMode, "/div");

		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 837px; left: 1105px;\">t =<input type=\"text\" size=\"2\" maxlength=\"7\" name=\"form_insert_point_x_11\" id=\"form_insert_point_x_11\" value=%.2f></div",box_display_point_x);
//		if (box_display_point_x == 0) hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 837px; left: 1105px;\">t =<input type=\"text\" size=\"2\" maxlength=\"7\" name=\"form_insert_point_x_11\"></div");
//		else hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 837px; left: 1105px;\">t =<input type=\"text\" size=\"2\" maxlength=\"7\" name=\"form_insert_point_x_11\" value=%.2f></div",box_display_point_x);
//		if (box_display_point_y == 0) hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 862px; left: 1105px;\">y=<input type=\"text\" size=\"2\" maxlength=\"7\" name=\"form_insert_point_y_11\"></div");
//		else hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 862px; left: 1105px;\">y=<input type=\"text\" size=\"2\" maxlength=\"7\" name=\"form_insert_point_y_11\" value=%.2f></div",box_display_point_y);
		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 862px; left: 1105px;\">y=<input type=\"text\" size=\"2\" maxlength=\"7\" name=\"form_insert_point_y_11\" id=\"form_insert_point_y_11\"></div");

		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 886px; left: 1105px;\"");
		hmStream.SSPrintf(HtmlTagStreamMode, "input type=\"submit\" name=\"submit_insert_point_11\" value=\"Add point\"");
		hmStream.SSPrintf(HtmlTagStreamMode, "/div");

		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 522px; left: 1085px;\">Max: %.2f mm</div",horizontal_2_p_max_value);
		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 542px; left: 1085px;\">Min: %.2f mm</div",horizontal_2_p_min_value);
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
	hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 917px; left: 10px;\"");
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

	if (!edit_control && !edit_weights && !edit_timewindows){

		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 917px; left: 420px;\"");
		hmStream.SSPrintf(HtmlTagStreamMode, "input type=\"submit\" name=\"form_save_file_as\" value=\"Save as...\"");
		if (save_as_bool){
			hmStream.SSPrintf(HtmlTagStreamMode, "input type=\"text\" size=\"20\" maxlength=\"40\" name=\"form_save_filename\" value=\"%s\"",save_filename.Buffer());
			hmStream.SSPrintf(HtmlTagStreamMode, "input type=\"submit\" name=\"form_save_file\" value=\"Save\"");
		} 
		hmStream.SSPrintf(HtmlTagStreamMode, "/div");

		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 917px; left: 850px;\"");
		hmStream.SSPrintf(HtmlTagStreamMode, "input type=\"submit\" name=\"form_save_file_and_send\" value=\"Save on default filename and Commit\"");
		hmStream.SSPrintf(HtmlTagStreamMode, "/div");
	}
	else {
		hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 920px; left: 620px;\"><b>Save and commit options only available after all parameters saved!</b></div");
	}

	hmStream.SSPrintf(HtmlTagStreamMode, "/form");

    return True;
}

bool MainConfigurator::ProcessHttpMessage(HttpStream &hStream){    
    HtmlStream hmStream(hStream);

	FString temp_string_1;
	int i;
	
	this->graphics_support->HeaderSVG(&temp_string_1,(char *) OBJECTNAME, 90, 500, 0,422,87,(char *) HEADERIMAGELINK,10,52, (char *) this->Name(),(char *) header_colour.Buffer());
	hmStream.SSPrintf(HtmlTagStreamMode, temp_string_1.Buffer());

	//Design the Footer with the GAM description
//	this->graphics_support->FooterSVG(&temp_string_1, 133, 0, 0, 283, 133,(char *) FOOTERIMAGELINK, 6, 100, 200,(char *) OBJECTNAME,(char *) VERSION,(char *) LASTDATE,(char *) AUTHOR,(char *) DESCRIPTION);
//	hmStream.SSPrintf(HtmlTagStreamMode, temp_string_1.Buffer());
	hmStream.SSPrintf(HtmlTagStreamMode, "svg width=\"100&#37;\" height=\"%d\" style=\"background-color: %s;\">\n\
			</svg", 192, diagnostics_parameters_colour.Buffer());

	hmStream.SSPrintf(HtmlTagStreamMode, "svg width=\"100&#37;\" height=\"%d\" style=\"background-color: %s;\">\n\
			</svg", 170, control_parameters_colour.Buffer());

	hmStream.SSPrintf(HtmlTagStreamMode, "svg width=\"100&#37;\" height=\"%d\" style=\"background-color: %s;\">\n\
			</svg", 455, time_windows_colour.Buffer());

	hmStream.SSPrintf(HtmlTagStreamMode, "svg width=\"100&#37;\" height=\"%d\" style=\"background-color: %s;\">\n\
			</svg", 30, "DarkKhaki");

	hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 80px; left: 10px;\"><b><h2>Diagnostics Settings</h2></b></div");
	
	hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 277px; left: 10px;\"><b><h2>Controller Settings</h2></b></div");
	
	hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"position: absolute; top: 447px; left: 10px;\"><b><h2>Time Windows</h2></b></div");
	
		
	if (already_started){
	//	Diagnostics
	//		checkboxes
		FString form_tomography_radial_bool;
		form_tomography_radial_bool.SetSize(0);
		if (hStream.Switch("InputCommands.form_tomography_radial_bool")){
			hStream.Seek(0);
			hStream.GetToken(form_tomography_radial_bool, "");
			hStream.Switch((uint32)0);
		}

		FString form_electric_radial_bool;
		form_electric_radial_bool.SetSize(0);
		if (hStream.Switch("InputCommands.form_electric_radial_bool")){
			hStream.Seek(0);
			hStream.GetToken(form_electric_radial_bool, "");
			hStream.Switch((uint32)0);
		}

		FString form_magnetic_radial_bool;
		form_magnetic_radial_bool.SetSize(0);
		if (hStream.Switch("InputCommands.form_magnetic_radial_bool")){
			hStream.Seek(0);
			hStream.GetToken(form_magnetic_radial_bool, "");
			hStream.Switch((uint32)0);
		}

		FString form_cosine_radial_bool;
		form_cosine_radial_bool.SetSize(0);
		if (hStream.Switch("InputCommands.form_cosine_radial_bool")){
			hStream.Seek(0);
			hStream.GetToken(form_cosine_radial_bool, "");
			hStream.Switch((uint32)0);
		}

		FString form_hibd_radial_bool;
		form_hibd_radial_bool.SetSize(0);
		if (hStream.Switch("InputCommands.form_hibd_radial_bool")){
			hStream.Seek(0);
			hStream.GetToken(form_hibd_radial_bool, "");
			hStream.Switch((uint32)0);
		}

		FString form_tomography_vertical_bool;
		form_tomography_vertical_bool.SetSize(0);
		if (hStream.Switch("InputCommands.form_tomography_vertical_bool")){
			hStream.Seek(0);
			hStream.GetToken(form_tomography_vertical_bool, "");
			hStream.Switch((uint32)0);
		}

		FString form_electric_vertical_bool;
		form_electric_vertical_bool.SetSize(0);
		if (hStream.Switch("InputCommands.form_electric_vertical_bool")){
			hStream.Seek(0);
			hStream.GetToken(form_electric_vertical_bool, "");
			hStream.Switch((uint32)0);
		}

		FString form_magnetic_vertical_bool;
		form_magnetic_vertical_bool.SetSize(0);
		if (hStream.Switch("InputCommands.form_magnetic_vertical_bool")){
			hStream.Seek(0);
			hStream.GetToken(form_magnetic_vertical_bool, "");
			hStream.Switch((uint32)0);
		}

		FString form_sine_vertical_bool;
		form_sine_vertical_bool.SetSize(0);
		if (hStream.Switch("InputCommands.form_sine_vertical_bool")){
			hStream.Seek(0);
			hStream.GetToken(form_sine_vertical_bool, "");
			hStream.Switch((uint32)0);
		}

		FString form_hibd_vertical_bool;
		form_hibd_vertical_bool.SetSize(0);
		if (hStream.Switch("InputCommands.form_hibd_vertical_bool")){
			hStream.Seek(0);
			hStream.GetToken(form_hibd_vertical_bool, "");
			hStream.Switch((uint32)0);
		}

		if ((form_tomography_radial_bool.Size() > 0) || (form_electric_radial_bool.Size() > 0) || (form_magnetic_radial_bool.Size() > 0) || (form_cosine_radial_bool.Size() > 0) || (form_hibd_radial_bool.Size() > 0) || (form_electric_vertical_bool.Size() > 0) || (form_tomography_vertical_bool.Size() > 0) || (form_magnetic_vertical_bool.Size() > 0) || (form_sine_vertical_bool.Size() > 0)  || (form_hibd_vertical_bool.Size() > 0)){
			if(form_tomography_radial_bool.Size() > 0) tomography_radial_bool = True;
			else tomography_radial_bool = False;
			if(form_electric_radial_bool.Size() > 0) electric_radial_bool = True;
			else electric_radial_bool = False;
			if(form_magnetic_radial_bool.Size() > 0) magnetic_radial_bool = True;
			else magnetic_radial_bool = False;
			if(form_cosine_radial_bool.Size() > 0) cosine_radial_bool = True;
			else cosine_radial_bool = False;
			if(form_hibd_radial_bool.Size() > 0) hibd_radial_bool = True;
			else hibd_radial_bool = False;
			if(form_electric_vertical_bool.Size() > 0) electric_vertical_bool = True;
			else electric_vertical_bool = False;
			if(form_tomography_vertical_bool.Size() > 0) tomography_vertical_bool = True;
			else tomography_vertical_bool = False;
			if(form_magnetic_vertical_bool.Size() > 0) magnetic_vertical_bool = True;
			else magnetic_vertical_bool = False;
			if(form_sine_vertical_bool.Size() > 0) sine_vertical_bool = True;
			else sine_vertical_bool = False;
			if(form_hibd_vertical_bool.Size() > 0) hibd_vertical_bool = True;
			else hibd_vertical_bool = False;
		}
	//		textbox
		FString form_description;
		form_description.SetSize(0);
		if (hStream.Switch("InputCommands.form_description")){
			hStream.Seek(0);
			hStream.GetToken(form_description, "");
			hStream.Switch((uint32)0);
		}
		if(form_description.Size() > 0) Description = form_description.Buffer();

	//		edit/save buttons
		FString submit_save_weights;
		submit_save_weights.SetSize(0);
		if (hStream.Switch("InputCommands.submit_save_weights")){
			hStream.Seek(0);
			hStream.GetToken(submit_save_weights, "");
			hStream.Switch((uint32)0);
		}
		if(submit_save_weights.Size() > 0) edit_weights = False;
		
		FString submit_edit_weights;
		submit_edit_weights.SetSize(0);
		if (hStream.Switch("InputCommands.submit_edit_weights")){
			hStream.Seek(0);
			hStream.GetToken(submit_edit_weights, "");
			hStream.Switch((uint32)0);
		}
		if(submit_edit_weights.Size() > 0) edit_weights = True;

	//		current threshold value
		FString form_high_current_threshold_value;
		form_high_current_threshold_value.SetSize(0);
		if (hStream.Switch("InputCommands.form_high_current_threshold_value")){
			hStream.Seek(0);
			hStream.GetToken(form_high_current_threshold_value, "");
			hStream.Switch((uint32)0);
		}
		if(form_high_current_threshold_value.Size() > 0) if(atof(form_high_current_threshold_value.Buffer()) > 0 && atof(form_high_current_threshold_value.Buffer()) < Ip_max_threshold_value) high_current_threshold_value = atof(form_high_current_threshold_value.Buffer());

	//		weights
		FString form_tomography_radial_low_current_weight;
		form_tomography_radial_low_current_weight.SetSize(0);
		if (hStream.Switch("InputCommands.form_tomography_radial_low_current_weight")){
			hStream.Seek(0);
			hStream.GetToken(form_tomography_radial_low_current_weight, "");
			hStream.Switch((uint32)0);
		}
		if(form_tomography_radial_low_current_weight.Size() > 0) if(atof(form_tomography_radial_low_current_weight.Buffer()) > 0 && atof(form_tomography_radial_low_current_weight.Buffer()) < max_value) tomography_radial_low_current_weight = atof(form_tomography_radial_low_current_weight.Buffer());

		FString form_tomography_radial_high_current_weight;
		form_tomography_radial_high_current_weight.SetSize(0);
		if (hStream.Switch("InputCommands.form_tomography_radial_high_current_weight")){
			hStream.Seek(0);
			hStream.GetToken(form_tomography_radial_high_current_weight, "");
			hStream.Switch((uint32)0);
		}
		if(form_tomography_radial_high_current_weight.Size() > 0) if(atof(form_tomography_radial_high_current_weight.Buffer()) > 0 && atof(form_tomography_radial_high_current_weight.Buffer()) < max_value) tomography_radial_high_current_weight = atof(form_tomography_radial_high_current_weight.Buffer());

		FString form_electric_radial_low_current_weight;
		form_electric_radial_low_current_weight.SetSize(0);
		if (hStream.Switch("InputCommands.form_electric_radial_low_current_weight")){
			hStream.Seek(0);
			hStream.GetToken(form_electric_radial_low_current_weight, "");
			hStream.Switch((uint32)0);
		}
		if(form_electric_radial_low_current_weight.Size() > 0) if(atof(form_electric_radial_low_current_weight.Buffer()) > 0 && atof(form_electric_radial_low_current_weight.Buffer()) < max_value) electric_radial_low_current_weight = atof(form_electric_radial_low_current_weight.Buffer());

		FString form_electric_radial_high_current_weight;
		form_electric_radial_high_current_weight.SetSize(0);
		if (hStream.Switch("InputCommands.form_electric_radial_high_current_weight")){
			hStream.Seek(0);
			hStream.GetToken(form_electric_radial_high_current_weight, "");
			hStream.Switch((uint32)0);
		}
		if(form_electric_radial_high_current_weight.Size() > 0) if(atof(form_electric_radial_high_current_weight.Buffer()) > 0 && atof(form_electric_radial_high_current_weight.Buffer()) < max_value) electric_radial_high_current_weight = atof(form_electric_radial_high_current_weight.Buffer());

		FString form_magnetic_radial_low_current_weight;
		form_magnetic_radial_low_current_weight.SetSize(0);
		if (hStream.Switch("InputCommands.form_magnetic_radial_low_current_weight")){
			hStream.Seek(0);
			hStream.GetToken(form_magnetic_radial_low_current_weight, "");
			hStream.Switch((uint32)0);
		}
		if(form_magnetic_radial_low_current_weight.Size() > 0) if(atof(form_magnetic_radial_low_current_weight.Buffer()) > 0 && atof(form_magnetic_radial_low_current_weight.Buffer()) < max_value) magnetic_radial_low_current_weight = atof(form_magnetic_radial_low_current_weight.Buffer());

		FString form_magnetic_radial_high_current_weight;
		form_magnetic_radial_high_current_weight.SetSize(0);
		if (hStream.Switch("InputCommands.form_magnetic_radial_high_current_weight")){
			hStream.Seek(0);
			hStream.GetToken(form_magnetic_radial_high_current_weight, "");
			hStream.Switch((uint32)0);
		}
		if(form_magnetic_radial_high_current_weight.Size() > 0) if(atof(form_magnetic_radial_high_current_weight.Buffer()) > 0 && atof(form_magnetic_radial_high_current_weight.Buffer()) < max_value) magnetic_radial_high_current_weight = atof(form_magnetic_radial_high_current_weight.Buffer());

		FString form_cosine_radial_low_current_weight;
		form_cosine_radial_low_current_weight.SetSize(0);
		if (hStream.Switch("InputCommands.form_cosine_radial_low_current_weight")){
			hStream.Seek(0);
			hStream.GetToken(form_cosine_radial_low_current_weight, "");
			hStream.Switch((uint32)0);
		}
		if(form_cosine_radial_low_current_weight.Size() > 0) if(atof(form_cosine_radial_low_current_weight.Buffer()) > 0 && atof(form_cosine_radial_low_current_weight.Buffer()) < max_value) cosine_radial_low_current_weight = atof(form_cosine_radial_low_current_weight.Buffer());

		FString form_cosine_radial_high_current_weight;
		form_cosine_radial_high_current_weight.SetSize(0);
		if (hStream.Switch("InputCommands.form_cosine_radial_high_current_weight")){
			hStream.Seek(0);
			hStream.GetToken(form_cosine_radial_high_current_weight, "");
			hStream.Switch((uint32)0);
		}
		if(form_cosine_radial_high_current_weight.Size() > 0) if(atof(form_cosine_radial_high_current_weight.Buffer()) > 0 && atof(form_cosine_radial_high_current_weight.Buffer()) < max_value) cosine_radial_high_current_weight = atof(form_cosine_radial_high_current_weight.Buffer());

		FString form_hibd_radial_low_current_weight;
		form_hibd_radial_low_current_weight.SetSize(0);
		if (hStream.Switch("InputCommands.form_hibd_radial_low_current_weight")){
			hStream.Seek(0);
			hStream.GetToken(form_hibd_radial_low_current_weight, "");
			hStream.Switch((uint32)0);
		}
		if(form_hibd_radial_low_current_weight.Size() > 0) if(atof(form_hibd_radial_low_current_weight.Buffer()) > 0 && atof(form_hibd_radial_low_current_weight.Buffer()) < max_value) hibd_radial_low_current_weight = atof(form_hibd_radial_low_current_weight.Buffer());

		FString form_hibd_radial_high_current_weight;
		form_hibd_radial_high_current_weight.SetSize(0);
		if (hStream.Switch("InputCommands.form_hibd_radial_high_current_weight")){
			hStream.Seek(0);
			hStream.GetToken(form_hibd_radial_high_current_weight, "");
			hStream.Switch((uint32)0);
		}
		if(form_hibd_radial_high_current_weight.Size() > 0) if(atof(form_hibd_radial_high_current_weight.Buffer()) > 0 && atof(form_hibd_radial_high_current_weight.Buffer()) < max_value) hibd_radial_high_current_weight = atof(form_hibd_radial_high_current_weight.Buffer());

		FString form_tomography_vertical_low_current_weight;
		form_tomography_vertical_low_current_weight.SetSize(0);
		if (hStream.Switch("InputCommands.form_tomography_vertical_low_current_weight")){
			hStream.Seek(0);
			hStream.GetToken(form_tomography_vertical_low_current_weight, "");
			hStream.Switch((uint32)0);
		}
		if(form_tomography_vertical_low_current_weight.Size() > 0) if(atof(form_tomography_vertical_low_current_weight.Buffer()) > 0 && atof(form_tomography_vertical_low_current_weight.Buffer()) < max_value) tomography_vertical_low_current_weight = atof(form_tomography_vertical_low_current_weight.Buffer());

		FString form_tomography_vertical_high_current_weight;
		form_tomography_vertical_high_current_weight.SetSize(0);
		if (hStream.Switch("InputCommands.form_tomography_vertical_high_current_weight")){
			hStream.Seek(0);
			hStream.GetToken(form_tomography_vertical_high_current_weight, "");
			hStream.Switch((uint32)0);
		}
		if(form_tomography_vertical_high_current_weight.Size() > 0) if(atof(form_tomography_vertical_high_current_weight.Buffer()) > 0 && atof(form_tomography_vertical_high_current_weight.Buffer()) < max_value) tomography_vertical_high_current_weight = atof(form_tomography_vertical_high_current_weight.Buffer());

		FString form_electric_vertical_low_current_weight;
		form_electric_vertical_low_current_weight.SetSize(0);
		if (hStream.Switch("InputCommands.form_electric_vertical_low_current_weight")){
			hStream.Seek(0);
			hStream.GetToken(form_electric_vertical_low_current_weight, "");
			hStream.Switch((uint32)0);
		}
		if(form_electric_vertical_low_current_weight.Size() > 0) if(atof(form_electric_vertical_low_current_weight.Buffer()) > 0 && atof(form_electric_vertical_low_current_weight.Buffer()) < max_value) electric_vertical_low_current_weight = atof(form_electric_vertical_low_current_weight.Buffer());

		FString form_electric_vertical_high_current_weight;
		form_electric_vertical_high_current_weight.SetSize(0);
		if (hStream.Switch("InputCommands.form_electric_vertical_high_current_weight")){
			hStream.Seek(0);
			hStream.GetToken(form_electric_vertical_high_current_weight, "");
			hStream.Switch((uint32)0);
		}
		if(form_electric_vertical_high_current_weight.Size() > 0) if(atof(form_electric_vertical_high_current_weight.Buffer()) > 0 && atof(form_electric_vertical_high_current_weight.Buffer()) < max_value) electric_vertical_high_current_weight = atof(form_electric_vertical_high_current_weight.Buffer());

		FString form_magnetic_vertical_low_current_weight;
		form_magnetic_vertical_low_current_weight.SetSize(0);
		if (hStream.Switch("InputCommands.form_magnetic_vertical_low_current_weight")){
			hStream.Seek(0);
			hStream.GetToken(form_magnetic_vertical_low_current_weight, "");
			hStream.Switch((uint32)0);
		}
		if(form_magnetic_vertical_low_current_weight.Size() > 0) if(atof(form_magnetic_vertical_low_current_weight.Buffer()) > 0 && atof(form_magnetic_vertical_low_current_weight.Buffer()) < max_value) magnetic_vertical_low_current_weight = atof(form_magnetic_vertical_low_current_weight.Buffer());

		FString form_magnetic_vertical_high_current_weight;
		form_magnetic_vertical_high_current_weight.SetSize(0);
		if (hStream.Switch("InputCommands.form_magnetic_vertical_high_current_weight")){
			hStream.Seek(0);
			hStream.GetToken(form_magnetic_vertical_high_current_weight, "");
			hStream.Switch((uint32)0);
		}
		if(form_magnetic_vertical_high_current_weight.Size() > 0) if(atof(form_magnetic_vertical_high_current_weight.Buffer()) > 0 && atof(form_magnetic_vertical_high_current_weight.Buffer()) < max_value) magnetic_vertical_high_current_weight = atof(form_magnetic_vertical_high_current_weight.Buffer());

		FString form_sine_vertical_low_current_weight;
		form_sine_vertical_low_current_weight.SetSize(0);
		if (hStream.Switch("InputCommands.form_sine_vertical_low_current_weight")){
			hStream.Seek(0);
			hStream.GetToken(form_sine_vertical_low_current_weight, "");
			hStream.Switch((uint32)0);
		}
		if(form_sine_vertical_low_current_weight.Size() > 0) if(atof(form_sine_vertical_low_current_weight.Buffer()) > 0 && atof(form_sine_vertical_low_current_weight.Buffer()) < max_value) sine_vertical_low_current_weight = atof(form_sine_vertical_low_current_weight.Buffer());

		FString form_sine_vertical_high_current_weight;
		form_sine_vertical_high_current_weight.SetSize(0);
		if (hStream.Switch("InputCommands.form_sine_vertical_high_current_weight")){
			hStream.Seek(0);
			hStream.GetToken(form_sine_vertical_high_current_weight, "");
			hStream.Switch((uint32)0);
		}
		if(form_sine_vertical_high_current_weight.Size() > 0) if(atof(form_sine_vertical_high_current_weight.Buffer()) > 0 && atof(form_sine_vertical_high_current_weight.Buffer()) < max_value) sine_vertical_high_current_weight = atof(form_sine_vertical_high_current_weight.Buffer());

		FString form_hibd_vertical_low_current_weight;
		form_hibd_vertical_low_current_weight.SetSize(0);
		if (hStream.Switch("InputCommands.form_hibd_vertical_low_current_weight")){
			hStream.Seek(0);
			hStream.GetToken(form_hibd_vertical_low_current_weight, "");
			hStream.Switch((uint32)0);
		}
		if(form_hibd_vertical_low_current_weight.Size() > 0) if(atof(form_hibd_vertical_low_current_weight.Buffer()) > 0 && atof(form_hibd_vertical_low_current_weight.Buffer()) < max_value) hibd_vertical_low_current_weight = atof(form_hibd_vertical_low_current_weight.Buffer());

		FString form_hibd_vertical_high_current_weight;
		form_hibd_vertical_high_current_weight.SetSize(0);
		if (hStream.Switch("InputCommands.form_hibd_vertical_high_current_weight")){
			hStream.Seek(0);
			hStream.GetToken(form_hibd_vertical_high_current_weight, "");
			hStream.Switch((uint32)0);
		}
		if(form_hibd_vertical_high_current_weight.Size() > 0) if(atof(form_hibd_vertical_high_current_weight.Buffer()) > 0 && atof(form_hibd_vertical_high_current_weight.Buffer()) < max_value) hibd_vertical_high_current_weight = atof(form_hibd_vertical_high_current_weight.Buffer());

	//	Control
	//		edit/save buttons
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

	//		control checkboxs
		FString form_magnetic_probe_bool_0;
		form_magnetic_probe_bool_0.SetSize(0);
		if (hStream.Switch("InputCommands.form_magnetic_probe_bool_0")){
			hStream.Seek(0);
			hStream.GetToken(form_magnetic_probe_bool_0, "");
			hStream.Switch((uint32)0);
		}
		
		FString form_magnetic_probe_bool_1;
		form_magnetic_probe_bool_1.SetSize(0);
		if (hStream.Switch("InputCommands.form_magnetic_probe_bool_1")){
			hStream.Seek(0);
			hStream.GetToken(form_magnetic_probe_bool_1, "");
			hStream.Switch((uint32)0);
		}
		
		FString form_magnetic_probe_bool_2;
		form_magnetic_probe_bool_2.SetSize(0);
		if (hStream.Switch("InputCommands.form_magnetic_probe_bool_2")){
			hStream.Seek(0);
			hStream.GetToken(form_magnetic_probe_bool_2, "");
			hStream.Switch((uint32)0);
		}
		
		FString form_magnetic_probe_bool_3;
		form_magnetic_probe_bool_3.SetSize(0);
		if (hStream.Switch("InputCommands.form_magnetic_probe_bool_3")){
			hStream.Seek(0);
			hStream.GetToken(form_magnetic_probe_bool_3, "");
			hStream.Switch((uint32)0);
		}
		
		FString form_magnetic_probe_bool_4;
		form_magnetic_probe_bool_4.SetSize(0);
		if (hStream.Switch("InputCommands.form_magnetic_probe_bool_4")){
			hStream.Seek(0);
			hStream.GetToken(form_magnetic_probe_bool_4, "");
			hStream.Switch((uint32)0);
		}
		
		FString form_magnetic_probe_bool_5;
		form_magnetic_probe_bool_5.SetSize(0);
		if (hStream.Switch("InputCommands.form_magnetic_probe_bool_5")){
			hStream.Seek(0);
			hStream.GetToken(form_magnetic_probe_bool_5, "");
			hStream.Switch((uint32)0);
		}
		
		FString form_magnetic_probe_bool_6;
		form_magnetic_probe_bool_6.SetSize(0);
		if (hStream.Switch("InputCommands.form_magnetic_probe_bool_6")){
			hStream.Seek(0);
			hStream.GetToken(form_magnetic_probe_bool_6, "");
			hStream.Switch((uint32)0);
		}
		
		FString form_magnetic_probe_bool_7;
		form_magnetic_probe_bool_7.SetSize(0);
		if (hStream.Switch("InputCommands.form_magnetic_probe_bool_7")){
			hStream.Seek(0);
			hStream.GetToken(form_magnetic_probe_bool_7, "");
			hStream.Switch((uint32)0);
		}
		
		FString form_magnetic_probe_bool_8;
		form_magnetic_probe_bool_8.SetSize(0);
		if (hStream.Switch("InputCommands.form_magnetic_probe_bool_8")){
			hStream.Seek(0);
			hStream.GetToken(form_magnetic_probe_bool_8, "");
			hStream.Switch((uint32)0);
		}
		
		FString form_magnetic_probe_bool_9;
		form_magnetic_probe_bool_9.SetSize(0);
		if (hStream.Switch("InputCommands.form_magnetic_probe_bool_9")){
			hStream.Seek(0);
			hStream.GetToken(form_magnetic_probe_bool_9, "");
			hStream.Switch((uint32)0);
		}
		
		FString form_magnetic_probe_bool_10;
		form_magnetic_probe_bool_10.SetSize(0);
		if (hStream.Switch("InputCommands.form_magnetic_probe_bool_10")){
			hStream.Seek(0);
			hStream.GetToken(form_magnetic_probe_bool_10, "");
			hStream.Switch((uint32)0);
		}
		
		FString form_magnetic_probe_bool_11;
		form_magnetic_probe_bool_11.SetSize(0);
		if (hStream.Switch("InputCommands.form_magnetic_probe_bool_11")){
			hStream.Seek(0);
			hStream.GetToken(form_magnetic_probe_bool_11, "");
			hStream.Switch((uint32)0);
		}

		FString form_interferometry_radial_control_bool;
		form_interferometry_radial_control_bool.SetSize(0);
		if (hStream.Switch("InputCommands.form_interferometry_radial_control_bool")){
			hStream.Seek(0);
			hStream.GetToken(form_interferometry_radial_control_bool, "");
			hStream.Switch((uint32)0);
		}

		if ( form_magnetic_probe_bool_11.Size() > 0 || form_magnetic_probe_bool_10.Size() > 0 || form_magnetic_probe_bool_9.Size() > 0 || form_magnetic_probe_bool_8.Size() > 0 || form_magnetic_probe_bool_7.Size() > 0 || form_magnetic_probe_bool_6.Size() > 0 || form_magnetic_probe_bool_5.Size() > 0 || form_magnetic_probe_bool_4.Size() > 0 || form_magnetic_probe_bool_3.Size() > 0 || form_magnetic_probe_bool_2.Size() > 0 || form_magnetic_probe_bool_1.Size() > 0 || form_magnetic_probe_bool_0.Size() > 0 ){

			if(form_magnetic_probe_bool_0.Size() > 0)	magnetic_probes_bool_vector[0] = True;
			else magnetic_probes_bool_vector[0] = False;
		
			if(form_magnetic_probe_bool_1.Size() > 0)	magnetic_probes_bool_vector[1] = True;
			else magnetic_probes_bool_vector[1] = False;
		
			if(form_magnetic_probe_bool_2.Size() > 0)	magnetic_probes_bool_vector[2] = True;
			else magnetic_probes_bool_vector[2] = False;
		
			if(form_magnetic_probe_bool_3.Size() > 0)	magnetic_probes_bool_vector[3] = True;
			else magnetic_probes_bool_vector[3] = False;
		
			if(form_magnetic_probe_bool_4.Size() > 0)	magnetic_probes_bool_vector[4] = True;
			else magnetic_probes_bool_vector[4] = False;
		
			if(form_magnetic_probe_bool_5.Size() > 0)	magnetic_probes_bool_vector[5] = True;
			else magnetic_probes_bool_vector[5] = False;
		
			if(form_magnetic_probe_bool_6.Size() > 0)	magnetic_probes_bool_vector[6] = True;
			else magnetic_probes_bool_vector[6] = False;
		
			if(form_magnetic_probe_bool_7.Size() > 0)	magnetic_probes_bool_vector[7] = True;
			else magnetic_probes_bool_vector[7] = False;
		
			if(form_magnetic_probe_bool_8.Size() > 0)	magnetic_probes_bool_vector[8] = True;
			else magnetic_probes_bool_vector[8] = False;
		
			if(form_magnetic_probe_bool_9.Size() > 0)	magnetic_probes_bool_vector[9] = True;
			else magnetic_probes_bool_vector[9] = False;
		
			if(form_magnetic_probe_bool_10.Size() > 0)	magnetic_probes_bool_vector[10] = True;
			else magnetic_probes_bool_vector[10] = False;
		
			if(form_magnetic_probe_bool_11.Size() > 0)	magnetic_probes_bool_vector[11] = True;
			else magnetic_probes_bool_vector[11] = False;

			if(form_interferometry_radial_control_bool.Size() > 0) interferometry_radial_control_bool = True;
			else interferometry_radial_control_bool = False;

			int counter = 0;
			for (i=0; i<NumberOfProbes; i++) if (magnetic_probes_bool_vector[i]) counter++;
			
			NumberOfMeasurements = counter;
			ProbeNumbers = new int[counter];
			counter = 0;
			for (i=0;i<NumberOfProbes; i++){
				if (magnetic_probes_bool_vector[i]){
					ProbeNumbers[counter] = i;
					counter++;
				} 
			}
		}

	//		control edit boxes
	//		PID constants
		FString form_PID_primary_proportional_soft;
		form_PID_primary_proportional_soft.SetSize(0);
		if (hStream.Switch("InputCommands.form_PID_primary_proportional_soft")){
			hStream.Seek(0);
			hStream.GetToken(form_PID_primary_proportional_soft, "");
			hStream.Switch((uint32)0);
		}
		if(form_PID_primary_proportional_soft.Size() > 0) if(atof(form_PID_primary_proportional_soft.Buffer()) >= 0 && atof(form_PID_primary_proportional_soft.Buffer()) < max_value) PID_primary_proportional_soft = atof(form_PID_primary_proportional_soft.Buffer());

		FString form_PID_primary_integral_soft;
		form_PID_primary_integral_soft.SetSize(0);
		if (hStream.Switch("InputCommands.form_PID_primary_integral_soft")){
			hStream.Seek(0);
			hStream.GetToken(form_PID_primary_integral_soft, "");
			hStream.Switch((uint32)0);
		}
		if(form_PID_primary_integral_soft.Size() > 0) if(atof(form_PID_primary_integral_soft.Buffer()) >= 0 && atof(form_PID_primary_integral_soft.Buffer()) < max_value) PID_primary_integral_soft = atof(form_PID_primary_integral_soft.Buffer());

		FString form_PID_primary_derivative_soft;
		form_PID_primary_derivative_soft.SetSize(0);
		if (hStream.Switch("InputCommands.form_PID_primary_derivative_soft")){
			hStream.Seek(0);
			hStream.GetToken(form_PID_primary_derivative_soft, "");
			hStream.Switch((uint32)0);
		}
		if(form_PID_primary_derivative_soft.Size() > 0) if(atof(form_PID_primary_derivative_soft.Buffer()) >= 0 && atof(form_PID_primary_derivative_soft.Buffer()) < max_value) PID_primary_derivative_soft = atof(form_PID_primary_derivative_soft.Buffer());

		FString form_PID_primary_proportional_normal;
		form_PID_primary_proportional_normal.SetSize(0);
		if (hStream.Switch("InputCommands.form_PID_primary_proportional_normal")){
			hStream.Seek(0);
			hStream.GetToken(form_PID_primary_proportional_normal, "");
			hStream.Switch((uint32)0);
		}
		if(form_PID_primary_proportional_normal.Size() > 0) if(atof(form_PID_primary_proportional_normal.Buffer()) >= 0 && atof(form_PID_primary_proportional_normal.Buffer()) < max_value) PID_primary_proportional_normal = atof(form_PID_primary_proportional_normal.Buffer());

		FString form_PID_primary_integral_normal;
		form_PID_primary_integral_normal.SetSize(0);
		if (hStream.Switch("InputCommands.form_PID_primary_integral_normal")){
			hStream.Seek(0);
			hStream.GetToken(form_PID_primary_integral_normal, "");
			hStream.Switch((uint32)0);
		}
		if(form_PID_primary_integral_normal.Size() > 0) if(atof(form_PID_primary_integral_normal.Buffer()) >= 0 && atof(form_PID_primary_integral_normal.Buffer()) < max_value) PID_primary_integral_normal = atof(form_PID_primary_integral_normal.Buffer());

		FString form_PID_primary_derivative_normal;
		form_PID_primary_derivative_normal.SetSize(0);
		if (hStream.Switch("InputCommands.form_PID_primary_derivative_normal")){
			hStream.Seek(0);
			hStream.GetToken(form_PID_primary_derivative_normal, "");
			hStream.Switch((uint32)0);
		}
		if(form_PID_primary_derivative_normal.Size() > 0) if(atof(form_PID_primary_derivative_normal.Buffer()) >= 0 && atof(form_PID_primary_derivative_normal.Buffer()) < max_value) PID_primary_derivative_normal = atof(form_PID_primary_derivative_normal.Buffer());

		FString form_PID_primary_proportional_hard;
		form_PID_primary_proportional_hard.SetSize(0);
		if (hStream.Switch("InputCommands.form_PID_primary_proportional_hard")){
			hStream.Seek(0);
			hStream.GetToken(form_PID_primary_proportional_hard, "");
			hStream.Switch((uint32)0);
		}
		if(form_PID_primary_proportional_hard.Size() > 0) if(atof(form_PID_primary_proportional_hard.Buffer()) >= 0 && atof(form_PID_primary_proportional_hard.Buffer()) < max_value) PID_primary_proportional_hard = atof(form_PID_primary_proportional_hard.Buffer());

		FString form_PID_primary_integral_hard;
		form_PID_primary_integral_hard.SetSize(0);
		if (hStream.Switch("InputCommands.form_PID_primary_integral_hard")){
			hStream.Seek(0);
			hStream.GetToken(form_PID_primary_integral_hard, "");
			hStream.Switch((uint32)0);
		}
		if(form_PID_primary_integral_hard.Size() > 0) if(atof(form_PID_primary_integral_hard.Buffer()) >= 0 && atof(form_PID_primary_integral_hard.Buffer()) < max_value) PID_primary_integral_hard = atof(form_PID_primary_integral_hard.Buffer());

		FString form_PID_primary_derivative_hard;
		form_PID_primary_derivative_hard.SetSize(0);
		if (hStream.Switch("InputCommands.form_PID_primary_derivative_hard")){
			hStream.Seek(0);
			hStream.GetToken(form_PID_primary_derivative_hard, "");
			hStream.Switch((uint32)0);
		}
		if(form_PID_primary_derivative_hard.Size() > 0) if(atof(form_PID_primary_derivative_hard.Buffer()) >= 0 && atof(form_PID_primary_derivative_hard.Buffer()) < max_value) PID_primary_derivative_hard = atof(form_PID_primary_derivative_hard.Buffer());

		FString form_PID_horizontal_proportional_soft;
		form_PID_horizontal_proportional_soft.SetSize(0);
		if (hStream.Switch("InputCommands.form_PID_horizontal_proportional_soft")){
			hStream.Seek(0);
			hStream.GetToken(form_PID_horizontal_proportional_soft, "");
			hStream.Switch((uint32)0);
		}
		if(form_PID_horizontal_proportional_soft.Size() > 0) if(atof(form_PID_horizontal_proportional_soft.Buffer()) >= 0 && atof(form_PID_horizontal_proportional_soft.Buffer()) < max_value) PID_horizontal_proportional_soft = atof(form_PID_horizontal_proportional_soft.Buffer());

		FString form_PID_horizontal_integral_soft;
		form_PID_horizontal_integral_soft.SetSize(0);
		if (hStream.Switch("InputCommands.form_PID_horizontal_integral_soft")){
			hStream.Seek(0);
			hStream.GetToken(form_PID_horizontal_integral_soft, "");
			hStream.Switch((uint32)0);
		}
		if(form_PID_horizontal_integral_soft.Size() > 0) if(atof(form_PID_horizontal_integral_soft.Buffer()) >= 0 && atof(form_PID_horizontal_integral_soft.Buffer()) < max_value) PID_horizontal_integral_soft = atof(form_PID_horizontal_integral_soft.Buffer());

		FString form_PID_horizontal_derivative_soft;
		form_PID_horizontal_derivative_soft.SetSize(0);
		if (hStream.Switch("InputCommands.form_PID_horizontal_derivative_soft")){
			hStream.Seek(0);
			hStream.GetToken(form_PID_horizontal_derivative_soft, "");
			hStream.Switch((uint32)0);
		}
		if(form_PID_horizontal_derivative_soft.Size() > 0) if(atof(form_PID_horizontal_derivative_soft.Buffer()) >= 0 && atof(form_PID_horizontal_derivative_soft.Buffer()) < max_value) PID_horizontal_derivative_soft = atof(form_PID_horizontal_derivative_soft.Buffer());

		FString form_PID_horizontal_proportional_normal;
		form_PID_horizontal_proportional_normal.SetSize(0);
		if (hStream.Switch("InputCommands.form_PID_horizontal_proportional_normal")){
			hStream.Seek(0);
			hStream.GetToken(form_PID_horizontal_proportional_normal, "");
			hStream.Switch((uint32)0);
		}
		if(form_PID_horizontal_proportional_normal.Size() > 0) if(atof(form_PID_horizontal_proportional_normal.Buffer()) >= 0 && atof(form_PID_horizontal_proportional_normal.Buffer()) < max_value) PID_horizontal_proportional_normal = atof(form_PID_horizontal_proportional_normal.Buffer());

		FString form_PID_horizontal_integral_normal;
		form_PID_horizontal_integral_normal.SetSize(0);
		if (hStream.Switch("InputCommands.form_PID_horizontal_integral_normal")){
			hStream.Seek(0);
			hStream.GetToken(form_PID_horizontal_integral_normal, "");
			hStream.Switch((uint32)0);
		}
		if(form_PID_horizontal_integral_normal.Size() > 0) if(atof(form_PID_horizontal_integral_normal.Buffer()) >= 0 && atof(form_PID_horizontal_integral_normal.Buffer()) < max_value) PID_horizontal_integral_normal = atof(form_PID_horizontal_integral_normal.Buffer());

		FString form_PID_horizontal_derivative_normal;
		form_PID_horizontal_derivative_normal.SetSize(0);
		if (hStream.Switch("InputCommands.form_PID_horizontal_derivative_normal")){
			hStream.Seek(0);
			hStream.GetToken(form_PID_horizontal_derivative_normal, "");
			hStream.Switch((uint32)0);
		}
		if(form_PID_horizontal_derivative_normal.Size() > 0) if(atof(form_PID_horizontal_derivative_normal.Buffer()) >= 0 && atof(form_PID_horizontal_derivative_normal.Buffer()) < max_value) PID_horizontal_derivative_normal = atof(form_PID_horizontal_derivative_normal.Buffer());

		FString form_PID_horizontal_proportional_hard;
		form_PID_horizontal_proportional_hard.SetSize(0);
		if (hStream.Switch("InputCommands.form_PID_horizontal_proportional_hard")){
			hStream.Seek(0);
			hStream.GetToken(form_PID_horizontal_proportional_hard, "");
			hStream.Switch((uint32)0);
		}
		if(form_PID_horizontal_proportional_hard.Size() > 0) if(atof(form_PID_horizontal_proportional_hard.Buffer()) >= 0 && atof(form_PID_horizontal_proportional_hard.Buffer()) < max_value) PID_horizontal_proportional_hard = atof(form_PID_horizontal_proportional_hard.Buffer());

		FString form_PID_horizontal_integral_hard;
		form_PID_horizontal_integral_hard.SetSize(0);
		if (hStream.Switch("InputCommands.form_PID_horizontal_integral_hard")){
			hStream.Seek(0);
			hStream.GetToken(form_PID_horizontal_integral_hard, "");
			hStream.Switch((uint32)0);
		}
		if(form_PID_horizontal_integral_hard.Size() > 0) if(atof(form_PID_horizontal_integral_hard.Buffer()) >= 0 && atof(form_PID_horizontal_integral_hard.Buffer()) < max_value) PID_horizontal_integral_hard = atof(form_PID_horizontal_integral_hard.Buffer());

		FString form_PID_horizontal_derivative_hard;
		form_PID_horizontal_derivative_hard.SetSize(0);
		if (hStream.Switch("InputCommands.form_PID_horizontal_derivative_hard")){
			hStream.Seek(0);
			hStream.GetToken(form_PID_horizontal_derivative_hard, "");
			hStream.Switch((uint32)0);
		}
		if(form_PID_horizontal_derivative_hard.Size() > 0) if(atof(form_PID_horizontal_derivative_hard.Buffer()) >= 0 && atof(form_PID_horizontal_derivative_hard.Buffer()) < max_value) PID_horizontal_derivative_hard = atof(form_PID_horizontal_derivative_hard.Buffer());

		FString form_PID_vertical_proportional_soft;
		form_PID_vertical_proportional_soft.SetSize(0);
		if (hStream.Switch("InputCommands.form_PID_vertical_proportional_soft")){
			hStream.Seek(0);
			hStream.GetToken(form_PID_vertical_proportional_soft, "");
			hStream.Switch((uint32)0);
		}
		if(form_PID_vertical_proportional_soft.Size() > 0) if(atof(form_PID_vertical_proportional_soft.Buffer()) >= 0 && atof(form_PID_vertical_proportional_soft.Buffer()) < max_value) PID_vertical_proportional_soft = atof(form_PID_vertical_proportional_soft.Buffer());

		FString form_PID_vertical_integral_soft;
		form_PID_vertical_integral_soft.SetSize(0);
		if (hStream.Switch("InputCommands.form_PID_vertical_integral_soft")){
			hStream.Seek(0);
			hStream.GetToken(form_PID_vertical_integral_soft, "");
			hStream.Switch((uint32)0);
		}
		if(form_PID_vertical_integral_soft.Size() > 0) if(atof(form_PID_vertical_integral_soft.Buffer()) >= 0 && atof(form_PID_vertical_integral_soft.Buffer()) < max_value) PID_vertical_integral_soft = atof(form_PID_vertical_integral_soft.Buffer());

		FString form_PID_vertical_derivative_soft;
		form_PID_vertical_derivative_soft.SetSize(0);
		if (hStream.Switch("InputCommands.form_PID_vertical_derivative_soft")){
			hStream.Seek(0);
			hStream.GetToken(form_PID_vertical_derivative_soft, "");
			hStream.Switch((uint32)0);
		}
		if(form_PID_vertical_derivative_soft.Size() > 0) if(atof(form_PID_vertical_derivative_soft.Buffer()) >= 0 && atof(form_PID_vertical_derivative_soft.Buffer()) < max_value) PID_vertical_derivative_soft = atof(form_PID_vertical_derivative_soft.Buffer());

		FString form_PID_vertical_proportional_normal;
		form_PID_vertical_proportional_normal.SetSize(0);
		if (hStream.Switch("InputCommands.form_PID_vertical_proportional_normal")){
			hStream.Seek(0);
			hStream.GetToken(form_PID_vertical_proportional_normal, "");
			hStream.Switch((uint32)0);
		}
		if(form_PID_vertical_proportional_normal.Size() > 0) if(atof(form_PID_vertical_proportional_normal.Buffer()) >= 0 && atof(form_PID_vertical_proportional_normal.Buffer()) < max_value) PID_vertical_proportional_normal = atof(form_PID_vertical_proportional_normal.Buffer());

		FString form_PID_vertical_integral_normal;
		form_PID_vertical_integral_normal.SetSize(0);
		if (hStream.Switch("InputCommands.form_PID_vertical_integral_normal")){
			hStream.Seek(0);
			hStream.GetToken(form_PID_vertical_integral_normal, "");
			hStream.Switch((uint32)0);
		}
		if(form_PID_vertical_integral_normal.Size() > 0) if(atof(form_PID_vertical_integral_normal.Buffer()) >= 0 && atof(form_PID_vertical_integral_normal.Buffer()) < max_value) PID_vertical_integral_normal = atof(form_PID_vertical_integral_normal.Buffer());

		FString form_PID_vertical_derivative_normal;
		form_PID_vertical_derivative_normal.SetSize(0);
		if (hStream.Switch("InputCommands.form_PID_vertical_derivative_normal")){
			hStream.Seek(0);
			hStream.GetToken(form_PID_vertical_derivative_normal, "");
			hStream.Switch((uint32)0);
		}
		if(form_PID_vertical_derivative_normal.Size() > 0) if(atof(form_PID_vertical_derivative_normal.Buffer()) >= 0 && atof(form_PID_vertical_derivative_normal.Buffer()) < max_value) PID_vertical_derivative_normal = atof(form_PID_vertical_derivative_normal.Buffer());

		FString form_PID_vertical_proportional_hard;
		form_PID_vertical_proportional_hard.SetSize(0);
		if (hStream.Switch("InputCommands.form_PID_vertical_proportional_hard")){
			hStream.Seek(0);
			hStream.GetToken(form_PID_vertical_proportional_hard, "");
			hStream.Switch((uint32)0);
		}
		if(form_PID_vertical_proportional_hard.Size() > 0) if(atof(form_PID_vertical_proportional_hard.Buffer()) >= 0 && atof(form_PID_vertical_proportional_hard.Buffer()) < max_value) PID_vertical_proportional_hard = atof(form_PID_vertical_proportional_hard.Buffer());

		FString form_PID_vertical_integral_hard;
		form_PID_vertical_integral_hard.SetSize(0);
		if (hStream.Switch("InputCommands.form_PID_vertical_integral_hard")){
			hStream.Seek(0);
			hStream.GetToken(form_PID_vertical_integral_hard, "");
			hStream.Switch((uint32)0);
		}
		if(form_PID_vertical_integral_hard.Size() > 0) if(atof(form_PID_vertical_integral_hard.Buffer()) >= 0 && atof(form_PID_vertical_integral_hard.Buffer()) < max_value) PID_vertical_integral_hard = atof(form_PID_vertical_integral_hard.Buffer());

		FString form_PID_vertical_derivative_hard;
		form_PID_vertical_derivative_hard.SetSize(0);
		if (hStream.Switch("InputCommands.form_PID_vertical_derivative_hard")){
			hStream.Seek(0);
			hStream.GetToken(form_PID_vertical_derivative_hard, "");
			hStream.Switch((uint32)0);
		}
		if(form_PID_vertical_derivative_hard.Size() > 0) if(atof(form_PID_vertical_derivative_hard.Buffer()) >= 0 && atof(form_PID_vertical_derivative_hard.Buffer()) < max_value) PID_vertical_derivative_hard = atof(form_PID_vertical_derivative_hard.Buffer());



//		MARTe Advanced control 
		FString form_marte_usec_cycle_time;
		form_marte_usec_cycle_time.SetSize(0);
		if (hStream.Switch("InputCommands.form_marte_usec_cycle_time")){
			hStream.Seek(0);
			hStream.GetToken(form_marte_usec_cycle_time, "");
			hStream.Switch((uint32)0);
		}
		if(form_marte_usec_cycle_time.Size() > 0) if(atoi(form_marte_usec_cycle_time.Buffer()) > 0) marte_usec_cycle_time = atoi(form_marte_usec_cycle_time.Buffer());

		FString form_usectime_to_wait_for_starting_operation;
		form_usectime_to_wait_for_starting_operation.SetSize(0);
		if (hStream.Switch("InputCommands.form_usectime_to_wait_for_starting_operation")){
			hStream.Seek(0);
			hStream.GetToken(form_usectime_to_wait_for_starting_operation, "");
			hStream.Switch((uint32)0);
		}
		if(form_usectime_to_wait_for_starting_operation.Size() > 0) if(atoi(form_usectime_to_wait_for_starting_operation.Buffer()) >= 0) usectime_to_wait_for_starting_operation = atoi(form_usectime_to_wait_for_starting_operation.Buffer());




	//		magnetic probes angles and calibration factors
		FString form_magnetic_Angles_0;
		form_magnetic_Angles_0.SetSize(0);
		if (hStream.Switch("InputCommands.form_magnetic_Angles_0")){
			hStream.Seek(0);
			hStream.GetToken(form_magnetic_Angles_0, "");
			hStream.Switch((uint32)0);
		}
		if(form_magnetic_Angles_0.Size() > 0) if(atof(form_magnetic_Angles_0.Buffer()) > (0-max_value) && atof(form_magnetic_Angles_0.Buffer()) < max_value) magnetic_Angles[0] = atof(form_magnetic_Angles_0.Buffer());

		FString form_magnetic_Calibration_0;
		form_magnetic_Calibration_0.SetSize(0);
		if (hStream.Switch("InputCommands.form_magnetic_Calibration_0")){
			hStream.Seek(0);
			hStream.GetToken(form_magnetic_Calibration_0, "");
			hStream.Switch((uint32)0);
		}
		if(form_magnetic_Calibration_0.Size() > 0) if(atof(form_magnetic_Calibration_0.Buffer()) > (0-max_value) && atof(form_magnetic_Calibration_0.Buffer()) < max_value) magnetic_Calibration[0] = atof(form_magnetic_Calibration_0.Buffer());

		FString form_magnetic_Angles_1;
		form_magnetic_Angles_1.SetSize(0);
		if (hStream.Switch("InputCommands.form_magnetic_Angles_1")){
			hStream.Seek(0);
			hStream.GetToken(form_magnetic_Angles_1, "");
			hStream.Switch((uint32)0);
		}
		if(form_magnetic_Angles_1.Size() > 0) if(atof(form_magnetic_Angles_1.Buffer()) > (0-max_value) && atof(form_magnetic_Angles_1.Buffer()) < max_value) magnetic_Angles[1] = atof(form_magnetic_Angles_1.Buffer());

		FString form_magnetic_Calibration_1;
		form_magnetic_Calibration_1.SetSize(0);
		if (hStream.Switch("InputCommands.form_magnetic_Calibration_1")){
			hStream.Seek(0);
			hStream.GetToken(form_magnetic_Calibration_1, "");
			hStream.Switch((uint32)0);
		}
		if(form_magnetic_Calibration_1.Size() > 0) if(atof(form_magnetic_Calibration_1.Buffer()) > (0-max_value) && atof(form_magnetic_Calibration_1.Buffer()) < max_value) magnetic_Calibration[1] = atof(form_magnetic_Calibration_1.Buffer());

		FString form_magnetic_Angles_2;
		form_magnetic_Angles_2.SetSize(0);
		if (hStream.Switch("InputCommands.form_magnetic_Angles_2")){
			hStream.Seek(0);
			hStream.GetToken(form_magnetic_Angles_2, "");
			hStream.Switch((uint32)0);
		}
		if(form_magnetic_Angles_2.Size() > 0) if(atof(form_magnetic_Angles_2.Buffer()) > (0-max_value) && atof(form_magnetic_Angles_2.Buffer()) < max_value) magnetic_Angles[2] = atof(form_magnetic_Angles_2.Buffer());

		FString form_magnetic_Calibration_2;
		form_magnetic_Calibration_2.SetSize(0);
		if (hStream.Switch("InputCommands.form_magnetic_Calibration_2")){
			hStream.Seek(0);
			hStream.GetToken(form_magnetic_Calibration_2, "");
			hStream.Switch((uint32)0);
		}
		if(form_magnetic_Calibration_2.Size() > 0) if(atof(form_magnetic_Calibration_2.Buffer()) > (0-max_value) && atof(form_magnetic_Calibration_2.Buffer()) < max_value) magnetic_Calibration[2] = atof(form_magnetic_Calibration_2.Buffer());

		FString form_magnetic_Angles_3;
		form_magnetic_Angles_3.SetSize(0);
		if (hStream.Switch("InputCommands.form_magnetic_Angles_3")){
			hStream.Seek(0);
			hStream.GetToken(form_magnetic_Angles_3, "");
			hStream.Switch((uint32)0);
		}
		if(form_magnetic_Angles_3.Size() > 0) if(atof(form_magnetic_Angles_3.Buffer()) > (0-max_value) && atof(form_magnetic_Angles_3.Buffer()) < max_value) magnetic_Angles[3] = atof(form_magnetic_Angles_3.Buffer());

		FString form_magnetic_Calibration_3;
		form_magnetic_Calibration_3.SetSize(0);
		if (hStream.Switch("InputCommands.form_magnetic_Calibration_3")){
			hStream.Seek(0);
			hStream.GetToken(form_magnetic_Calibration_3, "");
			hStream.Switch((uint32)0);
		}
		if(form_magnetic_Calibration_3.Size() > 0) if(atof(form_magnetic_Calibration_3.Buffer()) > (0-max_value) && atof(form_magnetic_Calibration_3.Buffer()) < max_value) magnetic_Calibration[3] = atof(form_magnetic_Calibration_3.Buffer());

		FString form_magnetic_Angles_4;
		form_magnetic_Angles_4.SetSize(0);
		if (hStream.Switch("InputCommands.form_magnetic_Angles_4")){
			hStream.Seek(0);
			hStream.GetToken(form_magnetic_Angles_4, "");
			hStream.Switch((uint32)0);
		}
		if(form_magnetic_Angles_4.Size() > 0) if(atof(form_magnetic_Angles_4.Buffer()) > (0-max_value) && atof(form_magnetic_Angles_4.Buffer()) < max_value) magnetic_Angles[4] = atof(form_magnetic_Angles_4.Buffer());

		FString form_magnetic_Calibration_4;
		form_magnetic_Calibration_4.SetSize(0);
		if (hStream.Switch("InputCommands.form_magnetic_Calibration_4")){
			hStream.Seek(0);
			hStream.GetToken(form_magnetic_Calibration_4, "");
			hStream.Switch((uint32)0);
		}
		if(form_magnetic_Calibration_4.Size() > 0) if(atof(form_magnetic_Calibration_4.Buffer()) > (0-max_value) && atof(form_magnetic_Calibration_4.Buffer()) < max_value) magnetic_Calibration[4] = atof(form_magnetic_Calibration_4.Buffer());

		FString form_magnetic_Angles_5;
		form_magnetic_Angles_5.SetSize(0);
		if (hStream.Switch("InputCommands.form_magnetic_Angles_5")){
			hStream.Seek(0);
			hStream.GetToken(form_magnetic_Angles_5, "");
			hStream.Switch((uint32)0);
		}
		if(form_magnetic_Angles_5.Size() > 0) if(atof(form_magnetic_Angles_5.Buffer()) > (0-max_value) && atof(form_magnetic_Angles_5.Buffer()) < max_value) magnetic_Angles[5] = atof(form_magnetic_Angles_5.Buffer());

		FString form_magnetic_Calibration_5;
		form_magnetic_Calibration_5.SetSize(0);
		if (hStream.Switch("InputCommands.form_magnetic_Calibration_5")){
			hStream.Seek(0);
			hStream.GetToken(form_magnetic_Calibration_5, "");
			hStream.Switch((uint32)0);
		}
		if(form_magnetic_Calibration_5.Size() > 0) if(atof(form_magnetic_Calibration_5.Buffer()) > (0-max_value) && atof(form_magnetic_Calibration_5.Buffer()) < max_value) magnetic_Calibration[5] = atof(form_magnetic_Calibration_5.Buffer());

		FString form_magnetic_Angles_6;
		form_magnetic_Angles_6.SetSize(0);
		if (hStream.Switch("InputCommands.form_magnetic_Angles_6")){
			hStream.Seek(0);
			hStream.GetToken(form_magnetic_Angles_6, "");
			hStream.Switch((uint32)0);
		}
		if(form_magnetic_Angles_6.Size() > 0) if(atof(form_magnetic_Angles_6.Buffer()) > (0-max_value) && atof(form_magnetic_Angles_6.Buffer()) < max_value) magnetic_Angles[6] = atof(form_magnetic_Angles_6.Buffer());

		FString form_magnetic_Calibration_6;
		form_magnetic_Calibration_6.SetSize(0);
		if (hStream.Switch("InputCommands.form_magnetic_Calibration_6")){
			hStream.Seek(0);
			hStream.GetToken(form_magnetic_Calibration_6, "");
			hStream.Switch((uint32)0);
		}
		if(form_magnetic_Calibration_6.Size() > 0) if(atof(form_magnetic_Calibration_6.Buffer()) > (0-max_value) && atof(form_magnetic_Calibration_6.Buffer()) < max_value) magnetic_Calibration[6] = atof(form_magnetic_Calibration_6.Buffer());

		FString form_magnetic_Angles_7;
		form_magnetic_Angles_7.SetSize(0);
		if (hStream.Switch("InputCommands.form_magnetic_Angles_7")){
			hStream.Seek(0);
			hStream.GetToken(form_magnetic_Angles_7, "");
			hStream.Switch((uint32)0);
		}
		if(form_magnetic_Angles_7.Size() > 0) if(atof(form_magnetic_Angles_7.Buffer()) > (0-max_value) && atof(form_magnetic_Angles_7.Buffer()) < max_value) magnetic_Angles[7] = atof(form_magnetic_Angles_7.Buffer());

		FString form_magnetic_Calibration_7;
		form_magnetic_Calibration_7.SetSize(0);
		if (hStream.Switch("InputCommands.form_magnetic_Calibration_7")){
			hStream.Seek(0);
			hStream.GetToken(form_magnetic_Calibration_7, "");
			hStream.Switch((uint32)0);
		}
		if(form_magnetic_Calibration_7.Size() > 0) if(atof(form_magnetic_Calibration_7.Buffer()) > (0-max_value) && atof(form_magnetic_Calibration_7.Buffer()) < max_value) magnetic_Calibration[7] = atof(form_magnetic_Calibration_7.Buffer());

		FString form_magnetic_Angles_8;
		form_magnetic_Angles_8.SetSize(0);
		if (hStream.Switch("InputCommands.form_magnetic_Angles_8")){
			hStream.Seek(0);
			hStream.GetToken(form_magnetic_Angles_8, "");
			hStream.Switch((uint32)0);
		}
		if(form_magnetic_Angles_8.Size() > 0) if(atof(form_magnetic_Angles_8.Buffer()) > (0-max_value) && atof(form_magnetic_Angles_8.Buffer()) < max_value) magnetic_Angles[8] = atof(form_magnetic_Angles_8.Buffer());

		FString form_magnetic_Calibration_8;
		form_magnetic_Calibration_8.SetSize(0);
		if (hStream.Switch("InputCommands.form_magnetic_Calibration_8")){
			hStream.Seek(0);
			hStream.GetToken(form_magnetic_Calibration_8, "");
			hStream.Switch((uint32)0);
		}
		if(form_magnetic_Calibration_8.Size() > 0) if(atof(form_magnetic_Calibration_8.Buffer()) > (0-max_value) && atof(form_magnetic_Calibration_8.Buffer()) < max_value) magnetic_Calibration[8] = atof(form_magnetic_Calibration_8.Buffer());

		FString form_magnetic_Angles_9;
		form_magnetic_Angles_9.SetSize(0);
		if (hStream.Switch("InputCommands.form_magnetic_Angles_9")){
			hStream.Seek(0);
			hStream.GetToken(form_magnetic_Angles_9, "");
			hStream.Switch((uint32)0);
		}
		if(form_magnetic_Angles_9.Size() > 0) if(atof(form_magnetic_Angles_9.Buffer()) > (0-max_value) && atof(form_magnetic_Angles_9.Buffer()) < max_value) magnetic_Angles[9] = atof(form_magnetic_Angles_9.Buffer());

		FString form_magnetic_Calibration_9;
		form_magnetic_Calibration_9.SetSize(0);
		if (hStream.Switch("InputCommands.form_magnetic_Calibration_9")){
			hStream.Seek(0);
			hStream.GetToken(form_magnetic_Calibration_9, "");
			hStream.Switch((uint32)0);
		}
		if(form_magnetic_Calibration_9.Size() > 0) if(atof(form_magnetic_Calibration_9.Buffer()) > (0-max_value) && atof(form_magnetic_Calibration_9.Buffer()) < max_value) magnetic_Calibration[9] = atof(form_magnetic_Calibration_9.Buffer());

		FString form_magnetic_Angles_10;
		form_magnetic_Angles_10.SetSize(0);
		if (hStream.Switch("InputCommands.form_magnetic_Angles_10")){
			hStream.Seek(0);
			hStream.GetToken(form_magnetic_Angles_10, "");
			hStream.Switch((uint32)0);
		}
		if(form_magnetic_Angles_10.Size() > 0) if(atof(form_magnetic_Angles_10.Buffer()) > (0-max_value) && atof(form_magnetic_Angles_10.Buffer()) < max_value) magnetic_Angles[10] = atof(form_magnetic_Angles_10.Buffer());

		FString form_magnetic_Calibration_10;
		form_magnetic_Calibration_10.SetSize(0);
		if (hStream.Switch("InputCommands.form_magnetic_Calibration_10")){
			hStream.Seek(0);
			hStream.GetToken(form_magnetic_Calibration_10, "");
			hStream.Switch((uint32)0);
		}
		if(form_magnetic_Calibration_10.Size() > 0) if(atof(form_magnetic_Calibration_10.Buffer()) > (0-max_value) && atof(form_magnetic_Calibration_10.Buffer()) < max_value) magnetic_Calibration[10] = atof(form_magnetic_Calibration_10.Buffer());

		FString form_magnetic_Angles_11;
		form_magnetic_Angles_11.SetSize(0);
		if (hStream.Switch("InputCommands.form_magnetic_Angles_11")){
			hStream.Seek(0);
			hStream.GetToken(form_magnetic_Angles_11, "");
			hStream.Switch((uint32)0);
		}
		if(form_magnetic_Angles_11.Size() > 0) if(atof(form_magnetic_Angles_11.Buffer()) > (0-max_value) && atof(form_magnetic_Angles_11.Buffer()) < max_value) magnetic_Angles[11] = atof(form_magnetic_Angles_11.Buffer());

		FString form_magnetic_Calibration_11;
		form_magnetic_Calibration_11.SetSize(0);
		if (hStream.Switch("InputCommands.form_magnetic_Calibration_11")){
			hStream.Seek(0);
			hStream.GetToken(form_magnetic_Calibration_11, "");
			hStream.Switch((uint32)0);
		}
		if(form_magnetic_Calibration_11.Size() > 0) if(atof(form_magnetic_Calibration_11.Buffer()) > (0-max_value) && atof(form_magnetic_Calibration_11.Buffer()) < max_value) magnetic_Calibration[11] = atof(form_magnetic_Calibration_11.Buffer());

	//	TIME_WINDOWS SECTION
	//		edit/save buttons
		FString submit_save_timewindows;
		submit_save_timewindows.SetSize(0);
		if (hStream.Switch("InputCommands.submit_save_timewindows")){
			hStream.Seek(0);
			hStream.GetToken(submit_save_timewindows, "");
			hStream.Switch((uint32)0);
		}
		if(submit_save_timewindows.Size() > 0) edit_timewindows = False;
		
		FString submit_edit_timewindows;
		submit_edit_timewindows.SetSize(0);
		if (hStream.Switch("InputCommands.submit_edit_timewindows")){
			hStream.Seek(0);
			hStream.GetToken(submit_edit_timewindows, "");
			hStream.Switch((uint32)0);
		}
		if(submit_edit_timewindows.Size() > 0) edit_timewindows = True;

	//		first cycle positive_bool
		FString form_first_cycle_positive_bool;
		form_first_cycle_positive_bool.SetSize(0);
		if (hStream.Switch("InputCommands.form_first_cycle_positive_bool")){
			hStream.Seek(0);
			hStream.GetToken(form_first_cycle_positive_bool, "");
			hStream.Switch((uint32)0);
		}
		if(form_first_cycle_positive_bool.Size() > 0) first_cycle_positive_bool = atoi(form_first_cycle_positive_bool.Buffer());	
		
		FString form_auto_breakdown_bool;
		form_auto_breakdown_bool.SetSize(0);
		if (hStream.Switch("InputCommands.form_auto_breakdown_bool")){
			hStream.Seek(0);
			hStream.GetToken(form_auto_breakdown_bool, "");
			hStream.Switch((uint32)0);
		}
		if(form_auto_breakdown_bool.Size() > 0) auto_breakdown = True;
		else auto_breakdown = False;

	//		number of cycles, discharge time
		FString form_number_of_cycles;
		form_number_of_cycles.SetSize(0);
		if (hStream.Switch("InputCommands.form_number_of_cycles")){
			hStream.Seek(0);
			hStream.GetToken(form_number_of_cycles, "");
			hStream.Switch((uint32)0);
		}
		if(form_number_of_cycles.Size() > 0) number_of_cycles = atoi(form_number_of_cycles.Buffer());

		FString form_discharge_time;
		form_discharge_time.SetSize(0);
		if (hStream.Switch("InputCommands.form_discharge_time")){
			hStream.Seek(0);
			hStream.GetToken(form_discharge_time, "");
			hStream.Switch((uint32)0);
		}
		if(form_discharge_time.Size() > 0) discharge_time = atof(form_discharge_time.Buffer());

	//		save timewindows data
		if(submit_save_timewindows.Size() > 0){
			
			FString form_timewindows_time_0;
			form_timewindows_time_0.SetSize(0);
			if (hStream.Switch("InputCommands.form_timewindows_time_0")){
				hStream.Seek(0);
				hStream.GetToken(form_timewindows_time_0, "");
				hStream.Switch((uint32)0);
			}
			if(form_timewindows_time_0.Size() > 0 && (atof(form_timewindows_time_0.Buffer()) >= 0)){
				timewindows_time[0] = floor(atof(form_timewindows_time_0.Buffer())*100+0.5)/100;
				timewindows_bool_vector[0] = True;
				
				FString form_timewindows_drop_0;
				form_timewindows_drop_0.SetSize(0);
				if (hStream.Switch("InputCommands.form_timewindows_drop_0")){
					hStream.Seek(0);
					hStream.GetToken(form_timewindows_drop_0, "");
					hStream.Switch((uint32)0);
				}
				if(form_timewindows_drop_0.Size() > 0) timewindows_dropdown_vector[0] = atoi(form_timewindows_drop_0.Buffer());
				
				FString form_timewindows_drop_1;
				form_timewindows_drop_1.SetSize(0);
				if (hStream.Switch("InputCommands.form_timewindows_drop_1")){
					hStream.Seek(0);
					hStream.GetToken(form_timewindows_drop_1, "");
					hStream.Switch((uint32)0);
				}
				if(form_timewindows_drop_1.Size() > 0) timewindows_dropdown_vector[1] = atoi(form_timewindows_drop_1.Buffer());
				
				FString form_timewindows_drop_2;
				form_timewindows_drop_2.SetSize(0);
				if (hStream.Switch("InputCommands.form_timewindows_drop_2")){
					hStream.Seek(0);
					hStream.GetToken(form_timewindows_drop_2, "");
					hStream.Switch((uint32)0);
				}
				if(form_timewindows_drop_2.Size() > 0) timewindows_dropdown_vector[2] = atoi(form_timewindows_drop_2.Buffer());
				
			}
			else timewindows_bool_vector[0] = False;
			
			FString form_timewindows_time_1;
			form_timewindows_time_1.SetSize(0);
			if (hStream.Switch("InputCommands.form_timewindows_time_1")){
				hStream.Seek(0);
				hStream.GetToken(form_timewindows_time_1, "");
				hStream.Switch((uint32)0);
			}
			if(form_timewindows_time_1.Size() > 0 && (atof(form_timewindows_time_1.Buffer()) >= 0)){
				timewindows_time[1] = floor(atof(form_timewindows_time_1.Buffer())*100+0.5)/100;
				timewindows_bool_vector[1] = True;
				
				FString form_timewindows_drop_3;
				form_timewindows_drop_3.SetSize(0);
				if (hStream.Switch("InputCommands.form_timewindows_drop_3")){
					hStream.Seek(0);
					hStream.GetToken(form_timewindows_drop_3, "");
					hStream.Switch((uint32)0);
				}
				if(form_timewindows_drop_3.Size() > 0) timewindows_dropdown_vector[3] = atoi(form_timewindows_drop_3.Buffer());
				
				FString form_timewindows_drop_4;
				form_timewindows_drop_4.SetSize(0);
				if (hStream.Switch("InputCommands.form_timewindows_drop_4")){
					hStream.Seek(0);
					hStream.GetToken(form_timewindows_drop_4, "");
					hStream.Switch((uint32)0);
				}
				if(form_timewindows_drop_4.Size() > 0) timewindows_dropdown_vector[4] = atoi(form_timewindows_drop_4.Buffer());
				
				FString form_timewindows_drop_5;
				form_timewindows_drop_5.SetSize(0);
				if (hStream.Switch("InputCommands.form_timewindows_drop_5")){
					hStream.Seek(0);
					hStream.GetToken(form_timewindows_drop_5, "");
					hStream.Switch((uint32)0);
				}
				if(form_timewindows_drop_5.Size() > 0) timewindows_dropdown_vector[5] = atoi(form_timewindows_drop_5.Buffer());
				
			}
			else timewindows_bool_vector[1] = False;

			FString form_timewindows_time_2;
			form_timewindows_time_2.SetSize(0);
			if (hStream.Switch("InputCommands.form_timewindows_time_2")){
				hStream.Seek(0);
				hStream.GetToken(form_timewindows_time_2, "");
				hStream.Switch((uint32)0);
			}
			if(form_timewindows_time_2.Size() > 0 && (atof(form_timewindows_time_2.Buffer()) >= 0)){
				timewindows_time[2] = floor(atof(form_timewindows_time_2.Buffer())*100+0.5)/100;
				timewindows_bool_vector[2] = True;
				
				FString form_timewindows_drop_6;
				form_timewindows_drop_6.SetSize(0);
				if (hStream.Switch("InputCommands.form_timewindows_drop_6")){
					hStream.Seek(0);
					hStream.GetToken(form_timewindows_drop_6, "");
					hStream.Switch((uint32)0);
				}
				if(form_timewindows_drop_6.Size() > 0) timewindows_dropdown_vector[6] = atoi(form_timewindows_drop_6.Buffer());
				
				FString form_timewindows_drop_7;
				form_timewindows_drop_7.SetSize(0);
				if (hStream.Switch("InputCommands.form_timewindows_drop_7")){
					hStream.Seek(0);
					hStream.GetToken(form_timewindows_drop_7, "");
					hStream.Switch((uint32)0);
				}
				if(form_timewindows_drop_7.Size() > 0) timewindows_dropdown_vector[7] = atoi(form_timewindows_drop_7.Buffer());
				
				FString form_timewindows_drop_8;
				form_timewindows_drop_8.SetSize(0);
				if (hStream.Switch("InputCommands.form_timewindows_drop_8")){
					hStream.Seek(0);
					hStream.GetToken(form_timewindows_drop_8, "");
					hStream.Switch((uint32)0);
				}
				if(form_timewindows_drop_8.Size() > 0) timewindows_dropdown_vector[8] = atoi(form_timewindows_drop_8.Buffer());
				
			}
			else timewindows_bool_vector[2] = False;

			FString form_timewindows_time_3;
			form_timewindows_time_3.SetSize(0);
			if (hStream.Switch("InputCommands.form_timewindows_time_3")){
				hStream.Seek(0);
				hStream.GetToken(form_timewindows_time_3, "");
				hStream.Switch((uint32)0);
			}
			if(form_timewindows_time_3.Size() > 0 && (atof(form_timewindows_time_3.Buffer()) >= 0)){
				timewindows_time[3] = floor(atof(form_timewindows_time_3.Buffer())*100+0.5)/100;
				timewindows_bool_vector[3] = True;
				
				FString form_timewindows_drop_9;
				form_timewindows_drop_9.SetSize(0);
				if (hStream.Switch("InputCommands.form_timewindows_drop_9")){
					hStream.Seek(0);
					hStream.GetToken(form_timewindows_drop_9, "");
					hStream.Switch((uint32)0);
				}
				if(form_timewindows_drop_9.Size() > 0) timewindows_dropdown_vector[9] = atoi(form_timewindows_drop_9.Buffer());
				
				FString form_timewindows_drop_10;
				form_timewindows_drop_10.SetSize(0);
				if (hStream.Switch("InputCommands.form_timewindows_drop_10")){
					hStream.Seek(0);
					hStream.GetToken(form_timewindows_drop_10, "");
					hStream.Switch((uint32)0);
				}
				if(form_timewindows_drop_10.Size() > 0) timewindows_dropdown_vector[10] = atoi(form_timewindows_drop_10.Buffer());
				
				FString form_timewindows_drop_11;
				form_timewindows_drop_11.SetSize(0);
				if (hStream.Switch("InputCommands.form_timewindows_drop_11")){
					hStream.Seek(0);
					hStream.GetToken(form_timewindows_drop_11, "");
					hStream.Switch((uint32)0);
				}
				if(form_timewindows_drop_11.Size() > 0) timewindows_dropdown_vector[11] = atoi(form_timewindows_drop_11.Buffer());
				
			}
			else timewindows_bool_vector[3] = False;

			FString form_timewindows_time_4;
			form_timewindows_time_4.SetSize(0);
			if (hStream.Switch("InputCommands.form_timewindows_time_4")){
				hStream.Seek(0);
				hStream.GetToken(form_timewindows_time_4, "");
				hStream.Switch((uint32)0);
			}
			if(form_timewindows_time_4.Size() > 0 && (atof(form_timewindows_time_4.Buffer()) >= 0)){
				timewindows_time[4] = floor(atof(form_timewindows_time_4.Buffer())*100+0.5)/100;
				timewindows_bool_vector[4] = True;
				
				FString form_timewindows_drop_12;
				form_timewindows_drop_12.SetSize(0);
				if (hStream.Switch("InputCommands.form_timewindows_drop_12")){
					hStream.Seek(0);
					hStream.GetToken(form_timewindows_drop_12, "");
					hStream.Switch((uint32)0);
				}
				if(form_timewindows_drop_12.Size() > 0) timewindows_dropdown_vector[12] = atoi(form_timewindows_drop_12.Buffer());
				
				FString form_timewindows_drop_13;
				form_timewindows_drop_13.SetSize(0);
				if (hStream.Switch("InputCommands.form_timewindows_drop_13")){
					hStream.Seek(0);
					hStream.GetToken(form_timewindows_drop_13, "");
					hStream.Switch((uint32)0);
				}
				if(form_timewindows_drop_13.Size() > 0) timewindows_dropdown_vector[13] = atoi(form_timewindows_drop_13.Buffer());
				
				FString form_timewindows_drop_14;
				form_timewindows_drop_14.SetSize(0);
				if (hStream.Switch("InputCommands.form_timewindows_drop_14")){
					hStream.Seek(0);
					hStream.GetToken(form_timewindows_drop_14, "");
					hStream.Switch((uint32)0);
				}
				if(form_timewindows_drop_14.Size() > 0) timewindows_dropdown_vector[14] = atoi(form_timewindows_drop_14.Buffer());
				
			}
			else timewindows_bool_vector[4] = False;

			FString form_timewindows_time_5;
			form_timewindows_time_5.SetSize(0);
			if (hStream.Switch("InputCommands.form_timewindows_time_5")){
				hStream.Seek(0);
				hStream.GetToken(form_timewindows_time_5, "");
				hStream.Switch((uint32)0);
			}
			if(form_timewindows_time_5.Size() > 0 && (atof(form_timewindows_time_5.Buffer()) >= 0)){
				timewindows_time[5] = floor(atof(form_timewindows_time_5.Buffer())*100+0.5)/100;
				timewindows_bool_vector[5] = True;
				
				FString form_timewindows_drop_15;
				form_timewindows_drop_15.SetSize(0);
				if (hStream.Switch("InputCommands.form_timewindows_drop_15")){
					hStream.Seek(0);
					hStream.GetToken(form_timewindows_drop_15, "");
					hStream.Switch((uint32)0);
				}
				if(form_timewindows_drop_15.Size() > 0) timewindows_dropdown_vector[15] = atoi(form_timewindows_drop_15.Buffer());
				
				FString form_timewindows_drop_16;
				form_timewindows_drop_16.SetSize(0);
				if (hStream.Switch("InputCommands.form_timewindows_drop_16")){
					hStream.Seek(0);
					hStream.GetToken(form_timewindows_drop_16, "");
					hStream.Switch((uint32)0);
				}
				if(form_timewindows_drop_16.Size() > 0) timewindows_dropdown_vector[16] = atoi(form_timewindows_drop_16.Buffer());
				
				FString form_timewindows_drop_17;
				form_timewindows_drop_17.SetSize(0);
				if (hStream.Switch("InputCommands.form_timewindows_drop_17")){
					hStream.Seek(0);
					hStream.GetToken(form_timewindows_drop_17, "");
					hStream.Switch((uint32)0);
				}
				if(form_timewindows_drop_17.Size() > 0) timewindows_dropdown_vector[17] = atoi(form_timewindows_drop_17.Buffer());
				
			}
			else timewindows_bool_vector[5] = False;

			FString form_timewindows_time_6;
			form_timewindows_time_6.SetSize(0);
			if (hStream.Switch("InputCommands.form_timewindows_time_6")){
				hStream.Seek(0);
				hStream.GetToken(form_timewindows_time_6, "");
				hStream.Switch((uint32)0);
			}
			if(form_timewindows_time_6.Size() > 0 && (atof(form_timewindows_time_6.Buffer()) >= 0)){
				timewindows_time[6] = floor(atof(form_timewindows_time_6.Buffer())*100+0.5)/100;
				timewindows_bool_vector[6] = True;
				
				FString form_timewindows_drop_18;
				form_timewindows_drop_18.SetSize(0);
				if (hStream.Switch("InputCommands.form_timewindows_drop_18")){
					hStream.Seek(0);
					hStream.GetToken(form_timewindows_drop_18, "");
					hStream.Switch((uint32)0);
				}
				if(form_timewindows_drop_18.Size() > 0) timewindows_dropdown_vector[18] = atoi(form_timewindows_drop_18.Buffer());
				
				FString form_timewindows_drop_19;
				form_timewindows_drop_19.SetSize(0);
				if (hStream.Switch("InputCommands.form_timewindows_drop_19")){
					hStream.Seek(0);
					hStream.GetToken(form_timewindows_drop_19, "");
					hStream.Switch((uint32)0);
				}
				if(form_timewindows_drop_19.Size() > 0) timewindows_dropdown_vector[19] = atoi(form_timewindows_drop_19.Buffer());
				
				FString form_timewindows_drop_20;
				form_timewindows_drop_20.SetSize(0);
				if (hStream.Switch("InputCommands.form_timewindows_drop_20")){
					hStream.Seek(0);
					hStream.GetToken(form_timewindows_drop_20, "");
					hStream.Switch((uint32)0);
				}
				if(form_timewindows_drop_20.Size() > 0) timewindows_dropdown_vector[20] = atoi(form_timewindows_drop_20.Buffer());
				
			}
			else timewindows_bool_vector[6] = False;

			FString form_timewindows_time_7;
			form_timewindows_time_7.SetSize(0);
			if (hStream.Switch("InputCommands.form_timewindows_time_7")){
				hStream.Seek(0);
				hStream.GetToken(form_timewindows_time_7, "");
				hStream.Switch((uint32)0);
			}
			if(form_timewindows_time_7.Size() > 0 && (atof(form_timewindows_time_7.Buffer()) >= 0)){
				timewindows_time[7] = floor(atof(form_timewindows_time_7.Buffer())*100+0.5)/100;
				timewindows_bool_vector[7] = True;
				
				FString form_timewindows_drop_21;
				form_timewindows_drop_21.SetSize(0);
				if (hStream.Switch("InputCommands.form_timewindows_drop_21")){
					hStream.Seek(0);
					hStream.GetToken(form_timewindows_drop_21, "");
					hStream.Switch((uint32)0);
				}
				if(form_timewindows_drop_21.Size() > 0) timewindows_dropdown_vector[21] = atoi(form_timewindows_drop_21.Buffer());
				
				FString form_timewindows_drop_22;
				form_timewindows_drop_22.SetSize(0);
				if (hStream.Switch("InputCommands.form_timewindows_drop_22")){
					hStream.Seek(0);
					hStream.GetToken(form_timewindows_drop_22, "");
					hStream.Switch((uint32)0);
				}
				if(form_timewindows_drop_22.Size() > 0) timewindows_dropdown_vector[22] = atoi(form_timewindows_drop_22.Buffer());
				
				FString form_timewindows_drop_23;
				form_timewindows_drop_23.SetSize(0);
				if (hStream.Switch("InputCommands.form_timewindows_drop_23")){
					hStream.Seek(0);
					hStream.GetToken(form_timewindows_drop_23, "");
					hStream.Switch((uint32)0);
				}
				if(form_timewindows_drop_23.Size() > 0) timewindows_dropdown_vector[23] = atoi(form_timewindows_drop_23.Buffer());
				
			}
			else timewindows_bool_vector[7] = False;

			FString form_timewindows_time_8;
			form_timewindows_time_8.SetSize(0);
			if (hStream.Switch("InputCommands.form_timewindows_time_8")){
				hStream.Seek(0);
				hStream.GetToken(form_timewindows_time_8, "");
				hStream.Switch((uint32)0);
			}
			if(form_timewindows_time_8.Size() > 0 && (atof(form_timewindows_time_8.Buffer()) >= 0)){
				timewindows_time[8] = floor(atof(form_timewindows_time_8.Buffer())*100+0.5)/100;
				timewindows_bool_vector[8] = True;
				
				FString form_timewindows_drop_24;
				form_timewindows_drop_24.SetSize(0);
				if (hStream.Switch("InputCommands.form_timewindows_drop_24")){
					hStream.Seek(0);
					hStream.GetToken(form_timewindows_drop_24, "");
					hStream.Switch((uint32)0);
				}
				if(form_timewindows_drop_24.Size() > 0) timewindows_dropdown_vector[24] = atoi(form_timewindows_drop_24.Buffer());
				
				FString form_timewindows_drop_25;
				form_timewindows_drop_25.SetSize(0);
				if (hStream.Switch("InputCommands.form_timewindows_drop_25")){
					hStream.Seek(0);
					hStream.GetToken(form_timewindows_drop_25, "");
					hStream.Switch((uint32)0);
				}
				if(form_timewindows_drop_25.Size() > 0) timewindows_dropdown_vector[25] = atoi(form_timewindows_drop_25.Buffer());
				
				FString form_timewindows_drop_26;
				form_timewindows_drop_26.SetSize(0);
				if (hStream.Switch("InputCommands.form_timewindows_drop_26")){
					hStream.Seek(0);
					hStream.GetToken(form_timewindows_drop_26, "");
					hStream.Switch((uint32)0);
				}
				if(form_timewindows_drop_26.Size() > 0) timewindows_dropdown_vector[26] = atoi(form_timewindows_drop_26.Buffer());
				
			}
			else timewindows_bool_vector[8] = False;

			FString form_timewindows_time_9;
			form_timewindows_time_9.SetSize(0);
			if (hStream.Switch("InputCommands.form_timewindows_time_9")){
				hStream.Seek(0);
				hStream.GetToken(form_timewindows_time_9, "");
				hStream.Switch((uint32)0);
			}
			if(form_timewindows_time_9.Size() > 0 && (atof(form_timewindows_time_9.Buffer()) >= 0)){
				timewindows_time[9] = floor(atof(form_timewindows_time_9.Buffer())*100+0.5)/100;
				timewindows_bool_vector[9] = True;
				
				FString form_timewindows_drop_27;
				form_timewindows_drop_27.SetSize(0);
				if (hStream.Switch("InputCommands.form_timewindows_drop_27")){
					hStream.Seek(0);
					hStream.GetToken(form_timewindows_drop_27, "");
					hStream.Switch((uint32)0);
				}
				if(form_timewindows_drop_27.Size() > 0) timewindows_dropdown_vector[27] = atoi(form_timewindows_drop_27.Buffer());
				
				FString form_timewindows_drop_28;
				form_timewindows_drop_28.SetSize(0);
				if (hStream.Switch("InputCommands.form_timewindows_drop_28")){
					hStream.Seek(0);
					hStream.GetToken(form_timewindows_drop_28, "");
					hStream.Switch((uint32)0);
				}
				if(form_timewindows_drop_28.Size() > 0) timewindows_dropdown_vector[28] = atoi(form_timewindows_drop_28.Buffer());
				
				FString form_timewindows_drop_29;
				form_timewindows_drop_29.SetSize(0);
				if (hStream.Switch("InputCommands.form_timewindows_drop_29")){
					hStream.Seek(0);
					hStream.GetToken(form_timewindows_drop_29, "");
					hStream.Switch((uint32)0);
				}
				if(form_timewindows_drop_29.Size() > 0) timewindows_dropdown_vector[29] = atoi(form_timewindows_drop_29.Buffer());
				
			}
			else timewindows_bool_vector[9] = False;

			FString form_timewindows_time_10;
			form_timewindows_time_10.SetSize(0);
			if (hStream.Switch("InputCommands.form_timewindows_time_10")){
				hStream.Seek(0);
				hStream.GetToken(form_timewindows_time_10, "");
				hStream.Switch((uint32)0);
			}
			if(form_timewindows_time_10.Size() > 0 && (atof(form_timewindows_time_10.Buffer()) >= 0)){
				timewindows_time[10] = floor(atof(form_timewindows_time_10.Buffer())*100+0.5)/100;
				timewindows_bool_vector[10] = True;
				
				FString form_timewindows_drop_30;
				form_timewindows_drop_30.SetSize(0);
				if (hStream.Switch("InputCommands.form_timewindows_drop_30")){
					hStream.Seek(0);
					hStream.GetToken(form_timewindows_drop_30, "");
					hStream.Switch((uint32)0);
				}
				if(form_timewindows_drop_30.Size() > 0) timewindows_dropdown_vector[30] = atoi(form_timewindows_drop_30.Buffer());
				
				FString form_timewindows_drop_31;
				form_timewindows_drop_31.SetSize(0);
				if (hStream.Switch("InputCommands.form_timewindows_drop_31")){
					hStream.Seek(0);
					hStream.GetToken(form_timewindows_drop_31, "");
					hStream.Switch((uint32)0);
				}
				if(form_timewindows_drop_31.Size() > 0) timewindows_dropdown_vector[31] = atoi(form_timewindows_drop_31.Buffer());
				
				FString form_timewindows_drop_32;
				form_timewindows_drop_32.SetSize(0);
				if (hStream.Switch("InputCommands.form_timewindows_drop_32")){
					hStream.Seek(0);
					hStream.GetToken(form_timewindows_drop_32, "");
					hStream.Switch((uint32)0);
				}
				if(form_timewindows_drop_32.Size() > 0) timewindows_dropdown_vector[32] = atoi(form_timewindows_drop_32.Buffer());
				
			}
			else timewindows_bool_vector[10] = False;

			FString form_timewindows_time_11;
			form_timewindows_time_11.SetSize(0);
			if (hStream.Switch("InputCommands.form_timewindows_time_11")){
				hStream.Seek(0);
				hStream.GetToken(form_timewindows_time_11, "");
				hStream.Switch((uint32)0);
			}
			if(form_timewindows_time_11.Size() > 0 && (atof(form_timewindows_time_11.Buffer()) >= 0)){
				timewindows_time[11] = floor(atof(form_timewindows_time_11.Buffer())*100+0.5)/100;
				timewindows_bool_vector[11] = True;
				
				FString form_timewindows_drop_33;
				form_timewindows_drop_33.SetSize(0);
				if (hStream.Switch("InputCommands.form_timewindows_drop_33")){
					hStream.Seek(0);
					hStream.GetToken(form_timewindows_drop_33, "");
					hStream.Switch((uint32)0);
				}
				if(form_timewindows_drop_33.Size() > 0) timewindows_dropdown_vector[33] = atoi(form_timewindows_drop_33.Buffer());
				
				FString form_timewindows_drop_34;
				form_timewindows_drop_34.SetSize(0);
				if (hStream.Switch("InputCommands.form_timewindows_drop_34")){
					hStream.Seek(0);
					hStream.GetToken(form_timewindows_drop_34, "");
					hStream.Switch((uint32)0);
				}
				if(form_timewindows_drop_34.Size() > 0) timewindows_dropdown_vector[34] = atoi(form_timewindows_drop_34.Buffer());
				
				FString form_timewindows_drop_35;
				form_timewindows_drop_35.SetSize(0);
				if (hStream.Switch("InputCommands.form_timewindows_drop_35")){
					hStream.Seek(0);
					hStream.GetToken(form_timewindows_drop_35, "");
					hStream.Switch((uint32)0);
				}
				if(form_timewindows_drop_35.Size() > 0) timewindows_dropdown_vector[35] = atoi(form_timewindows_drop_35.Buffer());
				
			}
			else timewindows_bool_vector[11] = False;

			FString form_timewindows_time_12;
			form_timewindows_time_12.SetSize(0);
			if (hStream.Switch("InputCommands.form_timewindows_time_12")){
				hStream.Seek(0);
				hStream.GetToken(form_timewindows_time_12, "");
				hStream.Switch((uint32)0);
			}
			if(form_timewindows_time_12.Size() > 0 && (atof(form_timewindows_time_12.Buffer()) >= 0)){
				timewindows_time[12] = floor(atof(form_timewindows_time_12.Buffer())*100+0.5)/100;
				timewindows_bool_vector[12] = True;
				
				FString form_timewindows_drop_36;
				form_timewindows_drop_36.SetSize(0);
				if (hStream.Switch("InputCommands.form_timewindows_drop_36")){
					hStream.Seek(0);
					hStream.GetToken(form_timewindows_drop_36, "");
					hStream.Switch((uint32)0);
				}
				if(form_timewindows_drop_36.Size() > 0) timewindows_dropdown_vector[36] = atoi(form_timewindows_drop_36.Buffer());
				
				FString form_timewindows_drop_37;
				form_timewindows_drop_37.SetSize(0);
				if (hStream.Switch("InputCommands.form_timewindows_drop_37")){
					hStream.Seek(0);
					hStream.GetToken(form_timewindows_drop_37, "");
					hStream.Switch((uint32)0);
				}
				if(form_timewindows_drop_37.Size() > 0) timewindows_dropdown_vector[37] = atoi(form_timewindows_drop_37.Buffer());
				
				FString form_timewindows_drop_38;
				form_timewindows_drop_38.SetSize(0);
				if (hStream.Switch("InputCommands.form_timewindows_drop_38")){
					hStream.Seek(0);
					hStream.GetToken(form_timewindows_drop_38, "");
					hStream.Switch((uint32)0);
				}
				if(form_timewindows_drop_38.Size() > 0) timewindows_dropdown_vector[38] = atoi(form_timewindows_drop_38.Buffer());
				
			}
			else timewindows_bool_vector[12] = False;

			FString form_timewindows_time_13;
			form_timewindows_time_13.SetSize(0);
			if (hStream.Switch("InputCommands.form_timewindows_time_13")){
				hStream.Seek(0);
				hStream.GetToken(form_timewindows_time_13, "");
				hStream.Switch((uint32)0);
			}
			if(form_timewindows_time_13.Size() > 0 && (atof(form_timewindows_time_13.Buffer()) >= 0)){
				timewindows_time[13] = floor(atof(form_timewindows_time_13.Buffer())*100+0.5)/100;
				timewindows_bool_vector[13] = True;
				
				FString form_timewindows_drop_39;
				form_timewindows_drop_39.SetSize(0);
				if (hStream.Switch("InputCommands.form_timewindows_drop_39")){
					hStream.Seek(0);
					hStream.GetToken(form_timewindows_drop_39, "");
					hStream.Switch((uint32)0);
				}
				if(form_timewindows_drop_39.Size() > 0) timewindows_dropdown_vector[39] = atoi(form_timewindows_drop_39.Buffer());
				
				FString form_timewindows_drop_40;
				form_timewindows_drop_40.SetSize(0);
				if (hStream.Switch("InputCommands.form_timewindows_drop_40")){
					hStream.Seek(0);
					hStream.GetToken(form_timewindows_drop_40, "");
					hStream.Switch((uint32)0);
				}
				if(form_timewindows_drop_40.Size() > 0) timewindows_dropdown_vector[40] = atoi(form_timewindows_drop_40.Buffer());
				
				FString form_timewindows_drop_41;
				form_timewindows_drop_41.SetSize(0);
				if (hStream.Switch("InputCommands.form_timewindows_drop_41")){
					hStream.Seek(0);
					hStream.GetToken(form_timewindows_drop_41, "");
					hStream.Switch((uint32)0);
				}
				if(form_timewindows_drop_41.Size() > 0) timewindows_dropdown_vector[41] = atoi(form_timewindows_drop_41.Buffer());
				
			}
			else timewindows_bool_vector[13] = False;
		
			
			SortTimeWindows();
		}


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

		box_display_point_x = 0;
		box_display_point_y = 0;


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
	already_started =True;

	PrintHTTPForm(hmStream);

    hStream.SSPrintf("OutputHttpOtions.Content-Type","text/html");
    hStream.WriteReplyHeader(True);
    return True;
}

bool MainConfigurator::ReadConfigurationFile(char FilePath[]){
	
//	CStaticAssertErrorCondition(InitialisationError,"MainConfigurator::ObjectLoadSetup: FilePath = %s",FilePath);
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



//	read +ExternalTimeTriggeringService	
	if(!cdbe->Move("+MARTe.+ExternalTimeTriggeringService"))
	{
		CStaticAssertErrorCondition(InitialisationError,"MainConfigurator::ReadConfigurationFile: %s Could not move to \"+MARTe.+ExternalTimeTriggeringService\"",this->Name());
		temp_file.Close();
		return False;
	}
		if(!cdbe.ReadInt32(marte_usec_cycle_time, "TsOnlineUsecPeriod"))
		{
			AssertErrorCondition(InitialisationError,"MainConfigurator::ReadConfigurationFile: %s marte_usec_cycle_time",this->Name());
			return False;
		}
		else AssertErrorCondition(Information,"MainConfigurator::ReadConfigurationFile: marte_usec_cycle_time = %d",marte_usec_cycle_time);

	cdbe->MoveToRoot();		



// read config file section: waveform_primary
	if(!cdbe->Move("+MARTe.+RealTimeThread.+waveform_primary.waveform_mode_1_positive"))
	{
		CStaticAssertErrorCondition(InitialisationError,"MainConfigurator::ReadConfigurationFile: %s Could not move to \"+MARTe.+RealTimeThread.+waveform_primary.waveform_mode_1_positive\"",this->Name());
		temp_file.Close();
		return False;
	}
		if(!cdbe.ReadInt32(primary_1_p_vector_size, "vector_size"))
		{
			CStaticAssertErrorCondition(InitialisationError,"MainConfigurator::ReadConfigurationFile: %s primary_1_p_vector_size",this->Name());
			temp_file.Close();
			return False;
		}
		else	CStaticAssertErrorCondition(Information,"MainConfigurator::ReadConfigurationFile: primary_1_p_vector_size = %d",primary_1_p_vector_size);

		primary_1_p_index_vector =new float[primary_1_p_vector_size];
		primary_1_p_data_vector =new float[primary_1_p_vector_size];

		if(!cdbe.ReadFloatArray(primary_1_p_index_vector, (int *)(&primary_1_p_vector_size), 1, "index_vector"))
		{
			CStaticAssertErrorCondition(InitialisationError,"ReadWaveformFiles: Could not read primary_1_p_index_vector");
			temp_file.Close();
			return False;
		}
		else for(i=0;i<primary_1_p_vector_size;i++) CStaticAssertErrorCondition(Information,"MainConfigurator::ReadConfigurationFile: primary_1_p_index_vector[%d] = %f",i, primary_1_p_index_vector[i]);
		if(!cdbe.ReadFloatArray(primary_1_p_data_vector, (int *)(&primary_1_p_vector_size), 1, "data_vector"))
		{
			CStaticAssertErrorCondition(InitialisationError,"ReadWaveformFiles: Could not read primary_1_p_data_vector");
			temp_file.Close();
			return False;
		}
		else for(i=0;i<primary_1_p_vector_size;i++) CStaticAssertErrorCondition(Information,"MainConfigurator::ReadConfigurationFile: primary_1_p_data_vector[%d] = %f",i, primary_1_p_data_vector[i]);
		if(!cdbe.ReadFloat(primary_1_p_max_value, "max_value"))
		{
			CStaticAssertErrorCondition(InitialisationError,"MainConfigurator::ReadConfigurationFile: %s primary_1_p_max_value",this->Name());
			temp_file.Close();
			return False;
		}
		else	CStaticAssertErrorCondition(Information,"MainConfigurator::ReadConfigurationFile: primary_1_p_max_value = %f",primary_1_p_max_value);
		if(!cdbe.ReadFloat(primary_1_p_min_value, "min_value"))
		{
			CStaticAssertErrorCondition(InitialisationError,"MainConfigurator::ReadConfigurationFile: %s primary_1_p_min_value",this->Name());
			temp_file.Close();
			return False;
		}
		else	CStaticAssertErrorCondition(Information,"MainConfigurator::ReadConfigurationFile: primary_1_p_min_value = %f",primary_1_p_min_value);
		
	cdbe->MoveToRoot();	
	
	if(!cdbe->Move("+MARTe.+RealTimeThread.+waveform_primary.waveform_mode_1_negative"))
	{
		CStaticAssertErrorCondition(InitialisationError,"MainConfigurator::ReadConfigurationFile: %s Could not move to \"+MARTe.+RealTimeThread.+waveform_primary.waveform_mode_1_negative\"",this->Name());
		temp_file.Close();
		return False;
	}
		if(!cdbe.ReadInt32(primary_1_n_vector_size, "vector_size"))
		{
			CStaticAssertErrorCondition(InitialisationError,"MainConfigurator::ReadConfigurationFile: %s primary_1_n_vector_size",this->Name());
			temp_file.Close();
			return False;
		}
		else	CStaticAssertErrorCondition(Information,"MainConfigurator::ReadConfigurationFile: primary_1_n_vector_size = %d",primary_1_n_vector_size);

		primary_1_n_index_vector =new float[primary_1_n_vector_size];
		primary_1_n_data_vector =new float[primary_1_n_vector_size];

		if(!cdbe.ReadFloatArray(primary_1_n_index_vector, (int *)(&primary_1_n_vector_size), 1, "index_vector"))
		{
			CStaticAssertErrorCondition(InitialisationError,"ReadWaveformFiles: Could not read primary_1_n_index_vector");
			temp_file.Close();
			return False;
		}
		else for(i=0;i<primary_1_n_vector_size;i++) CStaticAssertErrorCondition(Information,"MainConfigurator::ReadConfigurationFile: primary_1_n_index_vector[%d] = %f",i, primary_1_n_index_vector[i]);
		if(!cdbe.ReadFloatArray(primary_1_n_data_vector, (int *)(&primary_1_n_vector_size), 1, "data_vector"))
		{
			CStaticAssertErrorCondition(InitialisationError,"ReadWaveformFiles: Could not read primary_1_n_data_vector");
			temp_file.Close();
			return False;
		}
		else for(i=0;i<primary_1_n_vector_size;i++) CStaticAssertErrorCondition(Information,"MainConfigurator::ReadConfigurationFile: primary_1_n_data_vector[%d] = %f",i, primary_1_n_data_vector[i]);
	
	cdbe->MoveToRoot();
	
	if(!cdbe->Move("+MARTe.+RealTimeThread.+waveform_primary.waveform_mode_2_positive"))
	{
		CStaticAssertErrorCondition(InitialisationError,"MainConfigurator::ReadConfigurationFile: %s Could not move to \"+MARTe.+RealTimeThread.+waveform_primary.waveform_mode_2_positive\"",this->Name());
		temp_file.Close();
		return False;
	}
		if(!cdbe.ReadInt32(primary_2_p_vector_size, "vector_size"))
		{
			CStaticAssertErrorCondition(InitialisationError,"MainConfigurator::ReadConfigurationFile: %s primary_2_p_vector_size",this->Name());
			temp_file.Close();
			return False;
		}
		else	CStaticAssertErrorCondition(Information,"MainConfigurator::ReadConfigurationFile: primary_2_p_vector_size = %d",primary_2_p_vector_size);

		primary_2_p_index_vector =new float[primary_2_p_vector_size];
		primary_2_p_data_vector =new float[primary_2_p_vector_size];

		if(!cdbe.ReadFloatArray(primary_2_p_index_vector, (int *)(&primary_2_p_vector_size), 1, "index_vector"))
		{
			CStaticAssertErrorCondition(InitialisationError,"ReadWaveformFiles: Could not read primary_2_p_index_vector");
			temp_file.Close();
			return False;
		}
		else for(i=0;i<primary_2_p_vector_size;i++) CStaticAssertErrorCondition(Information,"MainConfigurator::ReadConfigurationFile: primary_2_p_index_vector[%d] = %f",i, primary_2_p_index_vector[i]);
		if(!cdbe.ReadFloatArray(primary_2_p_data_vector, (int *)(&primary_2_p_vector_size), 1, "data_vector"))
		{
			CStaticAssertErrorCondition(InitialisationError,"ReadWaveformFiles: Could not read primary_1_p_data_vector");
			temp_file.Close();
			return False;
		}
		else for(i=0;i<primary_2_p_vector_size;i++) CStaticAssertErrorCondition(Information,"MainConfigurator::ReadConfigurationFile: primary_2_p_data_vector[%d] = %f",i, primary_2_p_data_vector[i]);
		if(!cdbe.ReadFloat(primary_2_p_max_value, "max_value"))
		{
			CStaticAssertErrorCondition(InitialisationError,"MainConfigurator::ReadConfigurationFile: %s primary_2_p_max_value",this->Name());
			temp_file.Close();
			return False;
		}
		else	CStaticAssertErrorCondition(Information,"MainConfigurator::ReadConfigurationFile: primary_2_p_max_value = %f",primary_2_p_max_value);
		if(!cdbe.ReadFloat(primary_2_p_min_value, "min_value"))
		{
			CStaticAssertErrorCondition(InitialisationError,"MainConfigurator::ReadConfigurationFile: %s primary_2_p_min_value",this->Name());
			temp_file.Close();
			return False;
		}
		else	CStaticAssertErrorCondition(Information,"MainConfigurator::ReadConfigurationFile: primary_2_p_min_value = %f",primary_2_p_min_value);
		
	cdbe->MoveToRoot();	
	
	if(!cdbe->Move("+MARTe.+RealTimeThread.+waveform_primary.waveform_mode_2_negative"))
	{
		CStaticAssertErrorCondition(InitialisationError,"MainConfigurator::ReadConfigurationFile: %s Could not move to \"+MARTe.+RealTimeThread.+waveform_primary.waveform_mode_2_negative\"",this->Name());
		temp_file.Close();
		return False;
	}
		if(!cdbe.ReadInt32(primary_2_n_vector_size, "vector_size"))
		{
			CStaticAssertErrorCondition(InitialisationError,"MainConfigurator::ReadConfigurationFile: %s primary_2_n_vector_size",this->Name());
			temp_file.Close();
			return False;
		}
		else	CStaticAssertErrorCondition(Information,"MainConfigurator::ReadConfigurationFile: primary_2_n_vector_size = %d",primary_2_n_vector_size);

		primary_2_n_index_vector =new float[primary_2_n_vector_size];
		primary_2_n_data_vector =new float[primary_2_n_vector_size];

		if(!cdbe.ReadFloatArray(primary_2_n_index_vector, (int *)(&primary_2_n_vector_size), 1, "index_vector"))
		{
			CStaticAssertErrorCondition(InitialisationError,"ReadWaveformFiles: Could not read primary_2_n_index_vector");
			temp_file.Close();
			return False;
		}
		else for(i=0;i<primary_2_n_vector_size;i++) CStaticAssertErrorCondition(Information,"MainConfigurator::ReadConfigurationFile: primary_2_n_index_vector[%d] = %f",i, primary_2_n_index_vector[i]);
		if(!cdbe.ReadFloatArray(primary_2_n_data_vector, (int *)(&primary_2_n_vector_size), 1, "data_vector"))
		{
			CStaticAssertErrorCondition(InitialisationError,"ReadWaveformFiles: Could not read primary_2_n_data_vector");
			temp_file.Close();
			return False;
		}
		else for(i=0;i<primary_2_n_vector_size;i++) CStaticAssertErrorCondition(Information,"MainConfigurator::ReadConfigurationFile: primary_2_n_data_vector[%d] = %f",i, primary_2_n_data_vector[i]);
	
	cdbe->MoveToRoot();

// read config file section: waveform_vertical
	if(!cdbe->Move("+MARTe.+RealTimeThread.+waveform_vertical.waveform_mode_1_positive"))
	{
		CStaticAssertErrorCondition(InitialisationError,"MainConfigurator::ReadConfigurationFile: %s Could not move to \"+MARTe.+RealTimeThread.+waveform_vertical.waveform_mode_1_positive\"",this->Name());
		temp_file.Close();
		return False;
	}
		if(!cdbe.ReadInt32(vertical_1_p_vector_size, "vector_size"))
		{
			CStaticAssertErrorCondition(InitialisationError,"MainConfigurator::ReadConfigurationFile: %s vertical_1_p_vector_size",this->Name());
			temp_file.Close();
			return False;
		}
		else	CStaticAssertErrorCondition(Information,"MainConfigurator::ReadConfigurationFile: vertical_1_p_vector_size = %d",vertical_1_p_vector_size);

		vertical_1_p_index_vector =new float[vertical_1_p_vector_size];
		vertical_1_p_data_vector =new float[vertical_1_p_vector_size];

		if(!cdbe.ReadFloatArray(vertical_1_p_index_vector, (int *)(&vertical_1_p_vector_size), 1, "index_vector"))
		{
			CStaticAssertErrorCondition(InitialisationError,"ReadWaveformFiles: Could not read vertical_1_p_index_vector");
			temp_file.Close();
			return False;
		}
		else for(i=0;i<vertical_1_p_vector_size;i++) CStaticAssertErrorCondition(Information,"MainConfigurator::ReadConfigurationFile: vertical_1_p_index_vector[%d] = %f",i, vertical_1_p_index_vector[i]);
		if(!cdbe.ReadFloatArray(vertical_1_p_data_vector, (int *)(&vertical_1_p_vector_size), 1, "data_vector"))
		{
			CStaticAssertErrorCondition(InitialisationError,"ReadWaveformFiles: Could not read vertical_1_p_data_vector");
			temp_file.Close();
			return False;
		}
		else for(i=0;i<vertical_1_p_vector_size;i++) CStaticAssertErrorCondition(Information,"MainConfigurator::ReadConfigurationFile: vertical_1_p_data_vector[%d] = %f",i, vertical_1_p_data_vector[i]);
		if(!cdbe.ReadFloat(vertical_1_p_max_value, "max_value"))
		{
			CStaticAssertErrorCondition(InitialisationError,"MainConfigurator::ReadConfigurationFile: %s vertical_1_p_max_value",this->Name());
			temp_file.Close();
			return False;
		}
		else	CStaticAssertErrorCondition(Information,"MainConfigurator::ReadConfigurationFile: vertical_1_p_max_value = %f",vertical_1_p_max_value);
		if(!cdbe.ReadFloat(vertical_1_p_min_value, "min_value"))
		{
			CStaticAssertErrorCondition(InitialisationError,"MainConfigurator::ReadConfigurationFile: %s vertical_1_p_min_value",this->Name());
			temp_file.Close();
			return False;
		}
		else	CStaticAssertErrorCondition(Information,"MainConfigurator::ReadConfigurationFile: vertical_1_p_min_value = %f",vertical_1_p_min_value);
		
	cdbe->MoveToRoot();	
	
	if(!cdbe->Move("+MARTe.+RealTimeThread.+waveform_vertical.waveform_mode_1_negative"))
	{
		CStaticAssertErrorCondition(InitialisationError,"MainConfigurator::ReadConfigurationFile: %s Could not move to \"+MARTe.+RealTimeThread.+waveform_vertical.waveform_mode_1_negative\"",this->Name());
		temp_file.Close();
		return False;
	}
		if(!cdbe.ReadInt32(vertical_1_n_vector_size, "vector_size"))
		{
			CStaticAssertErrorCondition(InitialisationError,"MainConfigurator::ReadConfigurationFile: %s vertical_1_n_vector_size",this->Name());
			temp_file.Close();
			return False;
		}
		else	CStaticAssertErrorCondition(Information,"MainConfigurator::ReadConfigurationFile: vertical_1_n_vector_size = %d",vertical_1_n_vector_size);

		vertical_1_n_index_vector =new float[vertical_1_n_vector_size];
		vertical_1_n_data_vector =new float[vertical_1_n_vector_size];

		if(!cdbe.ReadFloatArray(vertical_1_n_index_vector, (int *)(&vertical_1_n_vector_size), 1, "index_vector"))
		{
			CStaticAssertErrorCondition(InitialisationError,"ReadWaveformFiles: Could not read vertical_1_n_index_vector");
			temp_file.Close();
			return False;
		}
		else for(i=0;i<vertical_1_n_vector_size;i++) CStaticAssertErrorCondition(Information,"MainConfigurator::ReadConfigurationFile: vertical_1_n_index_vector[%d] = %f",i, vertical_1_n_index_vector[i]);
		if(!cdbe.ReadFloatArray(vertical_1_n_data_vector, (int *)(&vertical_1_n_vector_size), 1, "data_vector"))
		{
			CStaticAssertErrorCondition(InitialisationError,"ReadWaveformFiles: Could not read vertical_1_n_data_vector");
			temp_file.Close();
			return False;
		}
		else for(i=0;i<vertical_1_n_vector_size;i++) CStaticAssertErrorCondition(Information,"MainConfigurator::ReadConfigurationFile: vertical_1_n_data_vector[%d] = %f",i, vertical_1_n_data_vector[i]);
	
	cdbe->MoveToRoot();
	
	if(!cdbe->Move("+MARTe.+RealTimeThread.+waveform_vertical.waveform_mode_2_positive"))
	{
		CStaticAssertErrorCondition(InitialisationError,"MainConfigurator::ReadConfigurationFile: %s Could not move to \"+MARTe.+RealTimeThread.+waveform_vertical.waveform_mode_2_positive\"",this->Name());
		temp_file.Close();
		return False;
	}
		if(!cdbe.ReadInt32(vertical_2_p_vector_size, "vector_size"))
		{
			CStaticAssertErrorCondition(InitialisationError,"MainConfigurator::ReadConfigurationFile: %s vertical_2_p_vector_size",this->Name());
			temp_file.Close();
			return False;
		}
		else	CStaticAssertErrorCondition(Information,"MainConfigurator::ReadConfigurationFile: vertical_2_p_vector_size = %d",vertical_2_p_vector_size);

		vertical_2_p_index_vector =new float[vertical_2_p_vector_size];
		vertical_2_p_data_vector =new float[vertical_2_p_vector_size];

		if(!cdbe.ReadFloatArray(vertical_2_p_index_vector, (int *)(&vertical_2_p_vector_size), 1, "index_vector"))
		{
			CStaticAssertErrorCondition(InitialisationError,"ReadWaveformFiles: Could not read vertical_2_p_index_vector");
			temp_file.Close();
			return False;
		}
		else for(i=0;i<vertical_2_p_vector_size;i++) CStaticAssertErrorCondition(Information,"MainConfigurator::ReadConfigurationFile: vertical_2_p_index_vector[%d] = %f",i, vertical_2_p_index_vector[i]);
		if(!cdbe.ReadFloatArray(vertical_2_p_data_vector, (int *)(&vertical_2_p_vector_size), 1, "data_vector"))
		{
			CStaticAssertErrorCondition(InitialisationError,"ReadWaveformFiles: Could not read vertical_1_p_data_vector");
			temp_file.Close();
			return False;
		}
		else for(i=0;i<vertical_2_p_vector_size;i++) CStaticAssertErrorCondition(Information,"MainConfigurator::ReadConfigurationFile: vertical_2_p_data_vector[%d] = %f",i, vertical_2_p_data_vector[i]);
		if(!cdbe.ReadFloat(vertical_2_p_max_value, "max_value"))
		{
			CStaticAssertErrorCondition(InitialisationError,"MainConfigurator::ReadConfigurationFile: %s vertical_2_p_max_value",this->Name());
			temp_file.Close();
			return False;
		}
		else	CStaticAssertErrorCondition(Information,"MainConfigurator::ReadConfigurationFile: vertical_2_p_max_value = %f",vertical_2_p_max_value);
		if(!cdbe.ReadFloat(vertical_2_p_min_value, "min_value"))
		{
			CStaticAssertErrorCondition(InitialisationError,"MainConfigurator::ReadConfigurationFile: %s vertical_2_p_min_value",this->Name());
			temp_file.Close();
			return False;
		}
		else	CStaticAssertErrorCondition(Information,"MainConfigurator::ReadConfigurationFile: vertical_2_p_min_value = %f",vertical_2_p_min_value);
		
	cdbe->MoveToRoot();	
	
	if(!cdbe->Move("+MARTe.+RealTimeThread.+waveform_vertical.waveform_mode_2_negative"))
	{
		CStaticAssertErrorCondition(InitialisationError,"MainConfigurator::ReadConfigurationFile: %s Could not move to \"+MARTe.+RealTimeThread.+waveform_vertical.waveform_mode_2_negative\"",this->Name());
		temp_file.Close();
		return False;
	}
		if(!cdbe.ReadInt32(vertical_2_n_vector_size, "vector_size"))
		{
			CStaticAssertErrorCondition(InitialisationError,"MainConfigurator::ReadConfigurationFile: %s vertical_2_n_vector_size",this->Name());
			temp_file.Close();
			return False;
		}
		else	CStaticAssertErrorCondition(Information,"MainConfigurator::ReadConfigurationFile: vertical_2_n_vector_size = %d",vertical_2_n_vector_size);

		vertical_2_n_index_vector =new float[vertical_2_n_vector_size];
		vertical_2_n_data_vector =new float[vertical_2_n_vector_size];

		if(!cdbe.ReadFloatArray(vertical_2_n_index_vector, (int *)(&vertical_2_n_vector_size), 1, "index_vector"))
		{
			CStaticAssertErrorCondition(InitialisationError,"ReadWaveformFiles: Could not read vertical_2_n_index_vector");
			temp_file.Close();
			return False;
		}
		else for(i=0;i<vertical_2_n_vector_size;i++) CStaticAssertErrorCondition(Information,"MainConfigurator::ReadConfigurationFile: vertical_2_n_index_vector[%d] = %f",i, vertical_1_n_index_vector[i]);
		if(!cdbe.ReadFloatArray(vertical_2_n_data_vector, (int *)(&vertical_2_n_vector_size), 1, "data_vector"))
		{
			CStaticAssertErrorCondition(InitialisationError,"ReadWaveformFiles: Could not read vertical_2_n_data_vector");
			temp_file.Close();
			return False;
		}
		else for(i=0;i<vertical_2_n_vector_size;i++) CStaticAssertErrorCondition(Information,"MainConfigurator::ReadConfigurationFile: vertical_2_n_data_vector[%d] = %f",i, vertical_2_n_data_vector[i]);
	
	cdbe->MoveToRoot();

// read config file section: waveform_horizontal
	if(!cdbe->Move("+MARTe.+RealTimeThread.+waveform_horizontal.waveform_mode_1_positive"))
	{
		CStaticAssertErrorCondition(InitialisationError,"MainConfigurator::ReadConfigurationFile: %s Could not move to \"+MARTe.+RealTimeThread.+waveform_horizontal.waveform_mode_1_positive\"",this->Name());
		temp_file.Close();
		return False;
	}
		if(!cdbe.ReadInt32(horizontal_1_p_vector_size, "vector_size"))
		{
			CStaticAssertErrorCondition(InitialisationError,"MainConfigurator::ReadConfigurationFile: %s horizontal_1_p_vector_size",this->Name());
			temp_file.Close();
			return False;
		}
		else	CStaticAssertErrorCondition(Information,"MainConfigurator::ReadConfigurationFile: horizontal_1_p_vector_size = %d",horizontal_1_p_vector_size);

		horizontal_1_p_index_vector =new float[horizontal_1_p_vector_size];
		horizontal_1_p_data_vector =new float[horizontal_1_p_vector_size];

		if(!cdbe.ReadFloatArray(horizontal_1_p_index_vector, (int *)(&horizontal_1_p_vector_size), 1, "index_vector"))
		{
			CStaticAssertErrorCondition(InitialisationError,"ReadWaveformFiles: Could not read horizontal_1_p_index_vector");
			temp_file.Close();
			return False;
		}
		else for(i=0;i<horizontal_1_p_vector_size;i++) CStaticAssertErrorCondition(Information,"MainConfigurator::ReadConfigurationFile: horizontal_1_p_index_vector[%d] = %f",i, horizontal_1_p_index_vector[i]);
		if(!cdbe.ReadFloatArray(horizontal_1_p_data_vector, (int *)(&horizontal_1_p_vector_size), 1, "data_vector"))
		{
			CStaticAssertErrorCondition(InitialisationError,"ReadWaveformFiles: Could not read horizontal_1_p_data_vector");
			temp_file.Close();
			return False;
		}
		else for(i=0;i<horizontal_1_p_vector_size;i++) CStaticAssertErrorCondition(Information,"MainConfigurator::ReadConfigurationFile: horizontal_1_p_data_vector[%d] = %f",i, horizontal_1_p_data_vector[i]);
		if(!cdbe.ReadFloat(horizontal_1_p_max_value, "max_value"))
		{
			CStaticAssertErrorCondition(InitialisationError,"MainConfigurator::ReadConfigurationFile: %s horizontal_1_p_max_value",this->Name());
			temp_file.Close();
			return False;
		}
		else	CStaticAssertErrorCondition(Information,"MainConfigurator::ReadConfigurationFile: horizontal_1_p_max_value = %f",horizontal_1_p_max_value);
		if(!cdbe.ReadFloat(horizontal_1_p_min_value, "min_value"))
		{
			CStaticAssertErrorCondition(InitialisationError,"MainConfigurator::ReadConfigurationFile: %s horizontal_1_p_min_value",this->Name());
			temp_file.Close();
			return False;
		}
		else	CStaticAssertErrorCondition(Information,"MainConfigurator::ReadConfigurationFile: horizontal_1_p_min_value = %f",horizontal_1_p_min_value);
		
	cdbe->MoveToRoot();	
	
	if(!cdbe->Move("+MARTe.+RealTimeThread.+waveform_horizontal.waveform_mode_1_negative"))
	{
		CStaticAssertErrorCondition(InitialisationError,"MainConfigurator::ReadConfigurationFile: %s Could not move to \"+MARTe.+RealTimeThread.+waveform_horizontal.waveform_mode_1_negative\"",this->Name());
		temp_file.Close();
		return False;
	}
		if(!cdbe.ReadInt32(horizontal_1_n_vector_size, "vector_size"))
		{
			CStaticAssertErrorCondition(InitialisationError,"MainConfigurator::ReadConfigurationFile: %s horizontal_1_n_vector_size",this->Name());
			temp_file.Close();
			return False;
		}
		else	CStaticAssertErrorCondition(Information,"MainConfigurator::ReadConfigurationFile: horizontal_1_n_vector_size = %d",horizontal_1_n_vector_size);

		horizontal_1_n_index_vector =new float[horizontal_1_n_vector_size];
		horizontal_1_n_data_vector =new float[horizontal_1_n_vector_size];

		if(!cdbe.ReadFloatArray(horizontal_1_n_index_vector, (int *)(&horizontal_1_n_vector_size), 1, "index_vector"))
		{
			CStaticAssertErrorCondition(InitialisationError,"ReadWaveformFiles: Could not read horizontal_1_n_index_vector");
			temp_file.Close();
			return False;
		}
		else for(i=0;i<horizontal_1_n_vector_size;i++) CStaticAssertErrorCondition(Information,"MainConfigurator::ReadConfigurationFile: horizontal_1_n_index_vector[%d] = %f",i, horizontal_1_n_index_vector[i]);
		if(!cdbe.ReadFloatArray(horizontal_1_n_data_vector, (int *)(&horizontal_1_n_vector_size), 1, "data_vector"))
		{
			CStaticAssertErrorCondition(InitialisationError,"ReadWaveformFiles: Could not read horizontal_1_n_data_vector");
			temp_file.Close();
			return False;
		}
		else for(i=0;i<horizontal_1_n_vector_size;i++) CStaticAssertErrorCondition(Information,"MainConfigurator::ReadConfigurationFile: horizontal_1_n_data_vector[%d] = %f",i, horizontal_1_n_data_vector[i]);
	
	cdbe->MoveToRoot();
	
	if(!cdbe->Move("+MARTe.+RealTimeThread.+waveform_horizontal.waveform_mode_2_positive"))
	{
		CStaticAssertErrorCondition(InitialisationError,"MainConfigurator::ReadConfigurationFile: %s Could not move to \"+MARTe.+RealTimeThread.+waveform_horizontal.waveform_mode_2_positive\"",this->Name());
		temp_file.Close();
		return False;
	}
		if(!cdbe.ReadInt32(horizontal_2_p_vector_size, "vector_size"))
		{
			CStaticAssertErrorCondition(InitialisationError,"MainConfigurator::ReadConfigurationFile: %s horizontal_2_p_vector_size",this->Name());
			temp_file.Close();
			return False;
		}
		else	CStaticAssertErrorCondition(Information,"MainConfigurator::ReadConfigurationFile: horizontal_2_p_vector_size = %d",horizontal_2_p_vector_size);

		horizontal_2_p_index_vector =new float[horizontal_2_p_vector_size];
		horizontal_2_p_data_vector =new float[horizontal_2_p_vector_size];

		if(!cdbe.ReadFloatArray(horizontal_2_p_index_vector, (int *)(&horizontal_2_p_vector_size), 1, "index_vector"))
		{
			CStaticAssertErrorCondition(InitialisationError,"ReadWaveformFiles: Could not read horizontal_2_p_index_vector");
			temp_file.Close();
			return False;
		}
		else for(i=0;i<horizontal_2_p_vector_size;i++) CStaticAssertErrorCondition(Information,"MainConfigurator::ReadConfigurationFile: horizontal_2_p_index_vector[%d] = %f",i, horizontal_2_p_index_vector[i]);
		if(!cdbe.ReadFloatArray(horizontal_2_p_data_vector, (int *)(&horizontal_2_p_vector_size), 1, "data_vector"))
		{
			CStaticAssertErrorCondition(InitialisationError,"ReadWaveformFiles: Could not read horizontal_2_p_data_vector");
			temp_file.Close();
			return False;
		}
		else for(i=0;i<horizontal_2_p_vector_size;i++) CStaticAssertErrorCondition(Information,"MainConfigurator::ReadConfigurationFile: horizontal_2_p_data_vector[%d] = %f",i, horizontal_2_p_data_vector[i]);
		if(!cdbe.ReadFloat(horizontal_2_p_max_value, "max_value"))
		{
			CStaticAssertErrorCondition(InitialisationError,"MainConfigurator::ReadConfigurationFile: %s horizontal_2_p_max_value",this->Name());
			temp_file.Close();
			return False;
		}
		else	CStaticAssertErrorCondition(Information,"MainConfigurator::ReadConfigurationFile: horizontal_2_p_max_value = %f",horizontal_2_p_max_value);
		if(!cdbe.ReadFloat(horizontal_2_p_min_value, "min_value"))
		{
			CStaticAssertErrorCondition(InitialisationError,"MainConfigurator::ReadConfigurationFile: %s horizontal_2_p_min_value",this->Name());
			temp_file.Close();
			return False;
		}
		else	CStaticAssertErrorCondition(Information,"MainConfigurator::ReadConfigurationFile: horizontal_2_p_min_value = %f",horizontal_2_p_min_value);
		
	cdbe->MoveToRoot();	
	
	if(!cdbe->Move("+MARTe.+RealTimeThread.+waveform_horizontal.waveform_mode_2_negative"))
	{
		CStaticAssertErrorCondition(InitialisationError,"MainConfigurator::ReadConfigurationFile: %s Could not move to \"+MARTe.+RealTimeThread.+waveform_horizontal.waveform_mode_2_negative\"",this->Name());
		temp_file.Close();
		return False;
	}
		if(!cdbe.ReadInt32(horizontal_2_n_vector_size, "vector_size"))
		{
			CStaticAssertErrorCondition(InitialisationError,"MainConfigurator::ReadConfigurationFile: %s horizontal_2_n_vector_size",this->Name());
			temp_file.Close();
			return False;
		}
		else	CStaticAssertErrorCondition(Information,"MainConfigurator::ReadConfigurationFile: horizontal_2_n_vector_size = %d",horizontal_2_n_vector_size);

		horizontal_2_n_index_vector =new float[horizontal_2_n_vector_size];
		horizontal_2_n_data_vector =new float[horizontal_2_n_vector_size];

		if(!cdbe.ReadFloatArray(horizontal_2_n_index_vector, (int *)(&horizontal_2_n_vector_size), 1, "index_vector"))
		{
			CStaticAssertErrorCondition(InitialisationError,"ReadWaveformFiles: Could not read horizontal_2_n_index_vector");
			temp_file.Close();
			return False;
		}
		else for(i=0;i<horizontal_2_n_vector_size;i++) CStaticAssertErrorCondition(Information,"MainConfigurator::ReadConfigurationFile: horizontal_2_n_index_vector[%d] = %f",i, horizontal_2_n_index_vector[i]);
		if(!cdbe.ReadFloatArray(horizontal_2_n_data_vector, (int *)(&horizontal_2_n_vector_size), 1, "data_vector"))
		{
			CStaticAssertErrorCondition(InitialisationError,"ReadWaveformFiles: Could not read horizontal_2_n_data_vector");
			temp_file.Close();
			return False;
		}
		else for(i=0;i<horizontal_2_n_vector_size;i++) CStaticAssertErrorCondition(Information,"MainConfigurator::ReadConfigurationFile: horizontal_2_n_data_vector[%d] = %f",i, horizontal_2_n_data_vector[i]);
	
	cdbe->MoveToRoot();

// read config file section: time_windows
	if(!cdbe->Move("+MARTe.+RealTimeThread.+time_windows"))
	{
		CStaticAssertErrorCondition(InitialisationError,"MainConfigurator::ReadConfigurationFile: %s Could not move to \"+MARTe.+RealTimeThread.+time_windows\"",this->Name());
		temp_file.Close();
		return False;
	}
		if(!cdbe.ReadFloat(discharge_time, "discharge_time"))
		{
			CStaticAssertErrorCondition(InitialisationError,"MainConfigurator::ReadConfigurationFile: %s discharge_time",this->Name());
			temp_file.Close();
			return False;
		}
		else	CStaticAssertErrorCondition(Information,"MainConfigurator::ReadConfigurationFile: discharge_time = %f",discharge_time);
		if(!cdbe.ReadInt32(number_of_cycles, "number_of_cycles"))
		{
			CStaticAssertErrorCondition(InitialisationError,"MainConfigurator::ReadConfigurationFile: %s number_of_cycles",this->Name());
			temp_file.Close();
			return False;
		}
		else	CStaticAssertErrorCondition(Information,"MainConfigurator::ReadConfigurationFile: number_of_cycles = %d",number_of_cycles);
		if(!cdbe.ReadInt32(i, "first_cycle_positive_bool"))
		{
			CStaticAssertErrorCondition(InitialisationError,"MainConfigurator::ReadConfigurationFile: %s first_cycle_positive_bool",this->Name());
			temp_file.Close();
			return False;
		}
		else 
		{
			first_cycle_positive_bool = (bool)i;
			CStaticAssertErrorCondition(Information,"MainConfigurator::ReadConfigurationFile: first_cycle_positive_bool = %d",first_cycle_positive_bool);
		}	
		if(!cdbe.ReadInt32(i, "auto_breakdown"))
		{
			CStaticAssertErrorCondition(InitialisationError,"MainConfigurator::ReadConfigurationFile: %s auto_breakdown",this->Name());
			temp_file.Close();
			return False;
		}
		else 
		{
			auto_breakdown = (bool)i;
			CStaticAssertErrorCondition(Information,"MainConfigurator::ReadConfigurationFile: auto_breakdown = %d",auto_breakdown);
		}	
		if(!cdbe.ReadInt32(usectime_to_wait_for_starting_operation, "usectime_to_wait_for_starting_operation"))
		{
			AssertErrorCondition(InitialisationError,"MainConfigurator::ReadConfigurationFile: %s usectime_to_wait_for_starting_operation",this->Name());
			return False;
		}
		else AssertErrorCondition(Information,"MainConfigurator::ReadConfigurationFile: usectime_to_wait_for_starting_operation = %d",usectime_to_wait_for_starting_operation);

	cdbe->MoveToRoot();

	if(!cdbe->Move("+MARTe.+RealTimeThread.+time_windows.positive_time_windows"))
	{
		CStaticAssertErrorCondition(InitialisationError,"MainConfigurator::ReadConfigurationFile: %s Could not move to \"+MARTe.+RealTimeThread.+time_windows.positive_time_windows\"",this->Name());
		temp_file.Close();
		return False;
	}
		if(!cdbe.ReadInt32(positive_number_of_time_windows, "number_of_time_windows"))
		{
			CStaticAssertErrorCondition(InitialisationError,"MainConfigurator::ReadConfigurationFile: %s positive_number_of_time_windows",this->Name());
			temp_file.Close();
			return False;
		}
		else	CStaticAssertErrorCondition(Information,"MainConfigurator::ReadConfigurationFile: positive_number_of_time_windows = %d",positive_number_of_time_windows);

		positive_time_windows_values =new float[positive_number_of_time_windows];
		positive_primary_mode =new int[positive_number_of_time_windows];
		positive_horizontal_mode =new int[positive_number_of_time_windows];
		positive_vertical_mode =new int[positive_number_of_time_windows];

		if(!cdbe.ReadFloatArray(positive_time_windows_values, (int *)(&positive_number_of_time_windows), 1, "time_windows_values"))
		{
			CStaticAssertErrorCondition(InitialisationError,"ReadWaveformFiles: Could not read positive_time_windows_values");
			temp_file.Close();
			return False;
		}
		else for(i=0;i<positive_number_of_time_windows;i++) CStaticAssertErrorCondition(Information,"MainConfigurator::ReadConfigurationFile: positive_time_windows_values[%d] = %f",i, positive_time_windows_values[i]);
		if(!cdbe.ReadInt32Array(positive_primary_mode, (int *)(&positive_number_of_time_windows), 1, "primary_mode"))
		{
			CStaticAssertErrorCondition(InitialisationError,"ReadWaveformFiles: Could not positive_primary_mode");
			temp_file.Close();
			return False;
		}
		else for(i=0;i<positive_number_of_time_windows;i++) CStaticAssertErrorCondition(Information,"MainConfigurator::ReadConfigurationFile: positive_primary_mode[%d] = %d",i, positive_primary_mode[i]);
		if(!cdbe.ReadInt32Array(positive_horizontal_mode, (int *)(&positive_number_of_time_windows), 1, "horizontal_mode"))
		{
			CStaticAssertErrorCondition(InitialisationError,"ReadWaveformFiles: Could not read positive_horizontal_mode");
			temp_file.Close();
			return False;
		}
		else for(i=0;i<positive_number_of_time_windows;i++) CStaticAssertErrorCondition(Information,"MainConfigurator::ReadConfigurationFile: positive_horizontal_mode[%d] = %d",i, positive_horizontal_mode[i]);
		if(!cdbe.ReadInt32Array(positive_vertical_mode, (int *)(&positive_number_of_time_windows), 1, "vertical_mode"))
		{
			CStaticAssertErrorCondition(InitialisationError,"ReadWaveformFiles: Could not read positive_vertical_mode");
			temp_file.Close();
			return False;
		}
		else for(i=0;i<positive_number_of_time_windows;i++) CStaticAssertErrorCondition(Information,"MainConfigurator::ReadConfigurationFile: positive_vertical_mode[%d] = %d",i, positive_vertical_mode[i]);

	cdbe->MoveToRoot();


	if(!cdbe->Move("+MARTe.+RealTimeThread.+time_windows.negative_time_windows"))
	{
		CStaticAssertErrorCondition(InitialisationError,"MainConfigurator::ReadConfigurationFile: %s Could not move to \"+MARTe.+RealTimeThread.+time_windows.negative_time_windows\"",this->Name());
		temp_file.Close();
		return False;
	}
		if(!cdbe.ReadInt32(negative_number_of_time_windows, "number_of_time_windows"))
		{
			CStaticAssertErrorCondition(InitialisationError,"MainConfigurator::ReadConfigurationFile: %s negative_number_of_time_windows",this->Name());
			temp_file.Close();
			return False;
		}
		else	CStaticAssertErrorCondition(Information,"MainConfigurator::ReadConfigurationFile: negative_number_of_time_windows = %d",negative_number_of_time_windows);

		negative_time_windows_values =new float[negative_number_of_time_windows];
		negative_primary_mode =new int[negative_number_of_time_windows];
		negative_horizontal_mode =new int[negative_number_of_time_windows];
		negative_vertical_mode =new int[negative_number_of_time_windows];

		if(!cdbe.ReadFloatArray(negative_time_windows_values, (int *)(&negative_number_of_time_windows), 1, "time_windows_values"))
		{
			CStaticAssertErrorCondition(InitialisationError,"ReadWaveformFiles: Could not read negative_time_windows_values");
			temp_file.Close();
			return False;
		}
		else for(i=0;i<negative_number_of_time_windows;i++) CStaticAssertErrorCondition(Information,"MainConfigurator::ReadConfigurationFile: negative_time_windows_values[%d] = %f",i, negative_time_windows_values[i]);
		if(!cdbe.ReadInt32Array(negative_primary_mode, (int *)(&negative_number_of_time_windows), 1, "primary_mode"))
		{
			CStaticAssertErrorCondition(InitialisationError,"ReadWaveformFiles: Could not negative_primary_mode");
			temp_file.Close();
			return False;
		}
		else for(i=0;i<negative_number_of_time_windows;i++) CStaticAssertErrorCondition(Information,"MainConfigurator::ReadConfigurationFile: negative_primary_mode[%d] = %d",i, negative_primary_mode[i]);
		if(!cdbe.ReadInt32Array(negative_horizontal_mode, (int *)(&negative_number_of_time_windows), 1, "horizontal_mode"))
		{
			CStaticAssertErrorCondition(InitialisationError,"ReadWaveformFiles: Could not read negative_horizontal_mode");
			temp_file.Close();
			return False;
		}
		else for(i=0;i<negative_number_of_time_windows;i++) CStaticAssertErrorCondition(Information,"MainConfigurator::ReadConfigurationFile: negative_horizontal_mode[%d] = %d",i, negative_horizontal_mode[i]);
		if(!cdbe.ReadInt32Array(negative_vertical_mode, (int *)(&negative_number_of_time_windows), 1, "vertical_mode"))
		{
			CStaticAssertErrorCondition(InitialisationError,"ReadWaveformFiles: Could not read negative_vertical_mode");
			temp_file.Close();
			return False;
		}
		else for(i=0;i<negative_number_of_time_windows;i++) CStaticAssertErrorCondition(Information,"MainConfigurator::ReadConfigurationFile: negative_vertical_mode[%d] = %d",i, negative_vertical_mode[i]);

	cdbe->MoveToRoot();

// read config file section: tomography
	if(!cdbe->Move("+MARTe.+RealTimeThread.+tomography"))
	{
		CStaticAssertErrorCondition(InitialisationError,"MainConfigurator::ReadConfigurationFile: %s Could not move to \"+MARTe.+RealTimeThread.+tomography\"",this->Name());
		temp_file.Close();
		return False;
	}
		if(!cdbe.ReadInt32(i, "tomography_radial_bool"))
		{
			CStaticAssertErrorCondition(InitialisationError,"MainConfigurator::ReadConfigurationFile: %s tomography_radial_bool",this->Name());
			temp_file.Close();
			return False;
		}
		else 
		{
			tomography_radial_bool = (bool)i;
			CStaticAssertErrorCondition(Information,"MainConfigurator::ReadConfigurationFile: tomography_radial_bool = %d",tomography_radial_bool);
		}	
		if(!cdbe.ReadInt32(i, "tomography_vertical_bool"))
		{
			CStaticAssertErrorCondition(InitialisationError,"MainConfigurator::ReadConfigurationFile: %s tomography_vertical_bool",this->Name());
			temp_file.Close();
			return False;
		}
		else 
		{
			tomography_vertical_bool = (bool)i;
			CStaticAssertErrorCondition(Information,"MainConfigurator::ReadConfigurationFile: tomography_vertical_bool = %d",tomography_vertical_bool);
		}	

	cdbe->MoveToRoot();


// read config file section: electric_probes
	if(!cdbe->Move("+MARTe.+RealTimeThread.+electric_probes"))
	{
		CStaticAssertErrorCondition(InitialisationError,"MainConfigurator::ReadConfigurationFile: %s Could not move to \"+MARTe.+RealTimeThread.+electric_probes\"",this->Name());
		temp_file.Close();
		return False;
	}
		if(!cdbe.ReadInt32(i, "electric_radial_bool"))
		{
			CStaticAssertErrorCondition(InitialisationError,"MainConfigurator::ReadConfigurationFile: %s electric_radial_bool",this->Name());
			temp_file.Close();
			return False;
		}
		else 
		{
			electric_radial_bool = (bool)i;
			CStaticAssertErrorCondition(Information,"MainConfigurator::ReadConfigurationFile: electric_radial_bool = %d",electric_radial_bool);
		}	
		if(!cdbe.ReadInt32(i, "electric_vertical_bool"))
		{
			CStaticAssertErrorCondition(InitialisationError,"MainConfigurator::ReadConfigurationFile: %s electric_vertical_bool",this->Name());
			temp_file.Close();
			return False;
		}
		else 
		{
			electric_vertical_bool = (bool)i;
			CStaticAssertErrorCondition(Information,"MainConfigurator::ReadConfigurationFile: electric_vertical_bool = %d",electric_vertical_bool);
		}	

	cdbe->MoveToRoot();


// read config file section: magnetic_probes
	if(!cdbe->Move("+MARTe.+RealTimeThread.+magnetic_probes"))
	{
		CStaticAssertErrorCondition(InitialisationError,"MainConfigurator::ReadConfigurationFile: %s Could not move to \"+MARTe.+RealTimeThread.+magnetic_probes\"",this->Name());
		temp_file.Close();
		return False;
	}
		if(!cdbe.ReadInt32(i, "magnetic_radial_bool"))
		{
			CStaticAssertErrorCondition(InitialisationError,"MainConfigurator::ReadConfigurationFile: %s magnetic_radial_bool",this->Name());
			temp_file.Close();
			return False;
		}
		else 
		{
			magnetic_radial_bool = (bool)i;
			CStaticAssertErrorCondition(Information,"MainConfigurator::ReadConfigurationFile: magnetic_radial_bool = %d",magnetic_radial_bool);
		}	
		if(!cdbe.ReadInt32(i, "magnetic_vertical_bool"))
		{
			CStaticAssertErrorCondition(InitialisationError,"MainConfigurator::ReadConfigurationFile: %s magnetic_vertical_bool",this->Name());
			temp_file.Close();
			return False;
		}
		else 
		{
			magnetic_vertical_bool = (bool)i;
			CStaticAssertErrorCondition(Information,"MainConfigurator::ReadConfigurationFile: magnetic_vertical_bool = %d",magnetic_vertical_bool);
		}	

	cdbe->MoveToRoot();

	if(!cdbe->Move("+MARTe.+RealTimeThread.+magnetic_probes.MirnovArrayDescription"))
	{
		CStaticAssertErrorCondition(InitialisationError,"MainConfigurator::ReadConfigurationFile: %s Could not move to \"+MARTe.+RealTimeThread.+magnetic_probes.MirnovArrayDescription\"",this->Name());
		temp_file.Close();
		return False;
	}
		if(!cdbe.ReadInt32(NumberOfProbes, "NumberOfProbes"))
		{
			CStaticAssertErrorCondition(InitialisationError,"MainConfigurator::ReadConfigurationFile: %s NumberOfProbes",this->Name());
			temp_file.Close();
			return False;
		}
		else	CStaticAssertErrorCondition(Information,"MainConfigurator::ReadConfigurationFile: NumberOfProbes = %d",NumberOfProbes);

		magnetic_Angles =new float[NumberOfProbes];
		magnetic_Calibration =new float[NumberOfProbes];

		if(!cdbe.ReadFloatArray(magnetic_Angles, (int *)(&NumberOfProbes), 1, "Angles"))
		{
			CStaticAssertErrorCondition(InitialisationError,"ReadWaveformFiles: Could not read magnetic_Angles");
			temp_file.Close();
			return False;
		}
		else for(i=0;i<NumberOfProbes;i++) CStaticAssertErrorCondition(Information,"MainConfigurator::ReadConfigurationFile: magnetic_Angles[%d] = %f",i, magnetic_Angles[i]);
		if(!cdbe.ReadFloatArray(magnetic_Calibration, (int *)(&NumberOfProbes), 1, "Calibration"))
		{
			CStaticAssertErrorCondition(InitialisationError,"ReadWaveformFiles: Could not read magnetic_Calibration");
			temp_file.Close();
			return False;
		}
		else for(i=0;i<NumberOfProbes;i++) CStaticAssertErrorCondition(Information,"MainConfigurator::ReadConfigurationFile: magnetic_Calibration[%d] = %f",i, magnetic_Calibration[i]);

	cdbe->MoveToRoot();

	if(!cdbe->Move("+MARTe.+RealTimeThread.+magnetic_probes.Measurements"))
	{
		CStaticAssertErrorCondition(InitialisationError,"MainConfigurator::ReadConfigurationFile: %s Could not move to \"+MARTe.+RealTimeThread.+magnetic_probes.Measurements\"",this->Name());
		temp_file.Close();
		return False;
	}
		if(!cdbe.ReadInt32(NumberOfMeasurements, "NumberOfMeasurements"))
		{
			CStaticAssertErrorCondition(InitialisationError,"MainConfigurator::ReadConfigurationFile: %s NumberOfMeasurements",this->Name());
			temp_file.Close();
			return False;
		}
		else	CStaticAssertErrorCondition(Information,"MainConfigurator::ReadConfigurationFile: NumberOfMeasurements = %d",NumberOfMeasurements);

		ProbeNumbers =new int[NumberOfMeasurements];

		if(!cdbe.ReadInt32Array(ProbeNumbers, (int *)(&NumberOfMeasurements), 1, "ProbeNumbers"))
		{
			CStaticAssertErrorCondition(InitialisationError,"ReadWaveformFiles: Could not read ProbeNumbers");
			temp_file.Close();
			return False;
		}
		else for(i=0;i<NumberOfMeasurements;i++) CStaticAssertErrorCondition(Information,"MainConfigurator::ReadConfigurationFile: ProbeNumbers[%d] = %d",i, ProbeNumbers[i]);

	cdbe->MoveToRoot();

// read config file section: sine_probe
	if(!cdbe->Move("+MARTe.+RealTimeThread.+sine_probe"))
	{
		CStaticAssertErrorCondition(InitialisationError,"MainConfigurator::ReadConfigurationFile: %s Could not move to \"+MARTe.+RealTimeThread.+sine_probe\"",this->Name());
		temp_file.Close();
		return False;
	}
		if(!cdbe.ReadInt32(i, "sine_vertical_bool"))
		{
			CStaticAssertErrorCondition(InitialisationError,"MainConfigurator::ReadConfigurationFile: %s sine_vertical_bool",this->Name());
			temp_file.Close();
			return False;
		}
		else 
		{
			sine_vertical_bool = (bool)i;
			CStaticAssertErrorCondition(Information,"MainConfigurator::ReadConfigurationFile: sine_vertical_bool = %d",sine_vertical_bool);
		}	

	cdbe->MoveToRoot();

// read config file section: cosine_probe
	if(!cdbe->Move("+MARTe.+RealTimeThread.+cosine_probe"))
	{
		CStaticAssertErrorCondition(InitialisationError,"MainConfigurator::ReadConfigurationFile: %s Could not move to \"+MARTe.+RealTimeThread.+cosine_probe\"",this->Name());
		temp_file.Close();
		return False;
	}
		if(!cdbe.ReadInt32(i, "cosine_radial_bool"))
		{
			CStaticAssertErrorCondition(InitialisationError,"MainConfigurator::ReadConfigurationFile: %s cosine_radial_bool",this->Name());
			temp_file.Close();
			return False;
		}
		else 
		{
			cosine_radial_bool = (bool)i;
			CStaticAssertErrorCondition(Information,"MainConfigurator::ReadConfigurationFile: cosine_radial_bool = %d",cosine_radial_bool);
		}	

	cdbe->MoveToRoot();

// read config file section: hibd
	if(!cdbe->Move("+MARTe.+RealTimeThread.+hibd"))
	{
		CStaticAssertErrorCondition(InitialisationError,"MainConfigurator::ReadConfigurationFile: %s Could not move to \"+MARTe.+RealTimeThread.+hibd\"",this->Name());
		temp_file.Close();
		return False;
	}
		if(!cdbe.ReadInt32(i, "hibd_radial_bool"))
		{
			CStaticAssertErrorCondition(InitialisationError,"MainConfigurator::ReadConfigurationFile: %s hibd_radial_bool",this->Name());
			temp_file.Close();
			return False;
		}
		else 
		{
			hibd_radial_bool = (bool)i;
			CStaticAssertErrorCondition(Information,"MainConfigurator::ReadConfigurationFile: hibd_radial_bool = %d",hibd_radial_bool);
		}	
		if(!cdbe.ReadInt32(i, "hibd_vertical_bool"))
		{
			CStaticAssertErrorCondition(InitialisationError,"MainConfigurator::ReadConfigurationFile: %s hibd_vertical_bool",this->Name());
			temp_file.Close();
			return False;
		}
		else
		{
			hibd_vertical_bool = (bool)i;
			CStaticAssertErrorCondition(Information,"MainConfigurator::ReadConfigurationFile: hibd_vertical_bool = %d",hibd_vertical_bool);
		}	

	cdbe->MoveToRoot();	

// read config file section: interferometry
	if(!cdbe->Move("+MARTe.+RealTimeThread.+interferometry"))
	{
		CStaticAssertErrorCondition(InitialisationError,"MainConfigurator::ReadConfigurationFile: %s Could not move to \"+MARTe.+RealTimeThread.+interferometry\"",this->Name());
		temp_file.Close();
		return False;
	}
		if(!cdbe.ReadInt32(i, "interferometry_radial_control_bool"))
		{
			CStaticAssertErrorCondition(InitialisationError,"MainConfigurator::ReadConfigurationFile: %s interferometry_radial_control_bool",this->Name());
			temp_file.Close();
			return False;
		}
		else 
		{
			interferometry_radial_control_bool = (bool)i;
			CStaticAssertErrorCondition(Information,"MainConfigurator::ReadConfigurationFile: interferometry_radial_control_bool = %d",interferometry_radial_control_bool);
		}	

	cdbe->MoveToRoot();

// read config file section: machine_protection
	if(!cdbe->Move("+MARTe.+RealTimeThread.+machine_protection"))
	{
		CStaticAssertErrorCondition(InitialisationError,"MainConfigurator::ReadConfigurationFile: %s Could not move to \"+MARTe.+RealTimeThread.+machine_protection\"",this->Name());
		temp_file.Close();
		return False;
	}
		if(!cdbe.ReadFloat(iron_core_saturation_value, "iron_core_saturation_value"))
		{
			CStaticAssertErrorCondition(InitialisationError,"MainConfigurator::ReadConfigurationFile: %s iron_core_saturation_value",this->Name());
			temp_file.Close();
			return False;
		}
		else CStaticAssertErrorCondition(Information,"MainConfigurator::ReadConfigurationFile: iron_core_saturation_value = %f",iron_core_saturation_value);
		if(!cdbe.ReadFloat(iron_core_dangerous_value, "iron_core_dangerous_value"))
		{
			CStaticAssertErrorCondition(InitialisationError,"MainConfigurator::ReadConfigurationFile: %s iron_core_dangerous_value",this->Name());
			temp_file.Close();
			return False;
		}
		else CStaticAssertErrorCondition(Information,"MainConfigurator::ReadConfigurationFile: iron_core_dangerous_value = %f",iron_core_dangerous_value);

	cdbe->MoveToRoot();

// read config file section: plasma_parameters
	if(!cdbe->Move("+MARTe.+RealTimeThread.+plasma_parameters"))
	{
		CStaticAssertErrorCondition(InitialisationError,"MainConfigurator::ReadConfigurationFile: %s Could not move to \"+MARTe.+RealTimeThread.+plasma_parameters\"",this->Name());
		temp_file.Close();
		return False;
	}
		if(!cdbe.ReadFloat(high_current_threshold_value, "high_current_threshold_value"))
		{
			CStaticAssertErrorCondition(InitialisationError,"MainConfigurator::ReadConfigurationFile: %s high_current_threshold_value",this->Name());
			temp_file.Close();
			return False;
		}
		else CStaticAssertErrorCondition(Information,"MainConfigurator::ReadConfigurationFile: high_current_threshold_value = %f",high_current_threshold_value);

	cdbe->MoveToRoot();

	if(!cdbe->Move("+MARTe.+RealTimeThread.+plasma_parameters.tomography"))
	{
		CStaticAssertErrorCondition(InitialisationError,"MainConfigurator::ReadConfigurationFile: %s Could not move to \"+MARTe.+RealTimeThread.+plasma_parameters.tomography\"",this->Name());
		temp_file.Close();
		return False;
	}
		if(!cdbe.ReadFloat(tomography_radial_high_current_weight, "radial_high_current_weight"))
		{
			CStaticAssertErrorCondition(InitialisationError,"MainConfigurator::ReadConfigurationFile: %s tomography_radial_high_current_weight",this->Name());
			temp_file.Close();
			return False;
		}
		else CStaticAssertErrorCondition(Information,"MainConfigurator::ReadConfigurationFile: tomography_radial_high_current_weight = %f",tomography_radial_high_current_weight);
		if(!cdbe.ReadFloat(tomography_radial_low_current_weight, "radial_low_current_weight"))
		{
			CStaticAssertErrorCondition(InitialisationError,"MainConfigurator::ReadConfigurationFile: %s tomography_radial_low_current_weight",this->Name());
			temp_file.Close();
			return False;
		}
		else CStaticAssertErrorCondition(Information,"MainConfigurator::ReadConfigurationFile: tomography_radial_low_current_weight = %f",tomography_radial_low_current_weight);
		if(!cdbe.ReadFloat(tomography_vertical_high_current_weight, "vertical_high_current_weight"))
		{
			CStaticAssertErrorCondition(InitialisationError,"MainConfigurator::ReadConfigurationFile: %s tomography_vertical_high_current_weight",this->Name());
			temp_file.Close();
			return False;
		}
		else CStaticAssertErrorCondition(Information,"MainConfigurator::ReadConfigurationFile: tomography_vertical_high_current_weight = %f",tomography_vertical_high_current_weight);
		if(!cdbe.ReadFloat(tomography_vertical_low_current_weight, "vertical_low_current_weight"))
		{
			CStaticAssertErrorCondition(InitialisationError,"MainConfigurator::ReadConfigurationFile: %s tomography_vertical_low_current_weight",this->Name());
			temp_file.Close();
			return False;
		}
		else CStaticAssertErrorCondition(Information,"MainConfigurator::ReadConfigurationFile: tomography_vertical_low_current_weight = %f",tomography_vertical_low_current_weight);

	cdbe->MoveToRoot();

	if(!cdbe->Move("+MARTe.+RealTimeThread.+plasma_parameters.electric_probes"))
	{
		CStaticAssertErrorCondition(InitialisationError,"MainConfigurator::ReadConfigurationFile: %s Could not move to \"+MARTe.+RealTimeThread.+plasma_parameters.electric_probes\"",this->Name());
		temp_file.Close();
		return False;
	}
		if(!cdbe.ReadFloat(electric_radial_high_current_weight, "radial_high_current_weight"))
		{
			CStaticAssertErrorCondition(InitialisationError,"MainConfigurator::ReadConfigurationFile: %s electric_radial_high_current_weight",this->Name());
			temp_file.Close();
			return False;
		}
		else CStaticAssertErrorCondition(Information,"MainConfigurator::ReadConfigurationFile: electric_radial_high_current_weight = %f",electric_radial_high_current_weight);
		if(!cdbe.ReadFloat(electric_radial_low_current_weight, "radial_low_current_weight"))
		{
			CStaticAssertErrorCondition(InitialisationError,"MainConfigurator::ReadConfigurationFile: %s electric_radial_low_current_weight",this->Name());
			temp_file.Close();
			return False;
		}
		else CStaticAssertErrorCondition(Information,"MainConfigurator::ReadConfigurationFile: electric_radial_low_current_weight = %f",electric_radial_low_current_weight);
		if(!cdbe.ReadFloat(electric_vertical_high_current_weight, "vertical_high_current_weight"))
		{
			CStaticAssertErrorCondition(InitialisationError,"MainConfigurator::ReadConfigurationFile: %s electric_vertical_high_current_weight",this->Name());
			temp_file.Close();
			return False;
		}
		else CStaticAssertErrorCondition(Information,"MainConfigurator::ReadConfigurationFile: electric_vertical_high_current_weight = %f",electric_vertical_high_current_weight);
		if(!cdbe.ReadFloat(electric_vertical_low_current_weight, "vertical_low_current_weight"))
		{
			CStaticAssertErrorCondition(InitialisationError,"MainConfigurator::ReadConfigurationFile: %s electric_vertical_low_current_weight",this->Name());
			temp_file.Close();
			return False;
		}
		else CStaticAssertErrorCondition(Information,"MainConfigurator::ReadConfigurationFile: electric_vertical_low_current_weight = %f",electric_vertical_low_current_weight);

	cdbe->MoveToRoot();

	if(!cdbe->Move("+MARTe.+RealTimeThread.+plasma_parameters.magnetic_probes"))
	{
		CStaticAssertErrorCondition(InitialisationError,"MainConfigurator::ReadConfigurationFile: %s Could not move to \"+MARTe.+RealTimeThread.+plasma_parameters.magnetic_probes\"",this->Name());
		temp_file.Close();
		return False;
	}
		if(!cdbe.ReadFloat(magnetic_radial_high_current_weight, "radial_high_current_weight"))
		{
			CStaticAssertErrorCondition(InitialisationError,"MainConfigurator::ReadConfigurationFile: %s magnetic_radial_high_current_weight",this->Name());
			temp_file.Close();
			return False;
		}
		else CStaticAssertErrorCondition(Information,"MainConfigurator::ReadConfigurationFile: magnetic_radial_high_current_weight = %f",magnetic_radial_high_current_weight);
		if(!cdbe.ReadFloat(magnetic_radial_low_current_weight, "radial_low_current_weight"))
		{
			CStaticAssertErrorCondition(InitialisationError,"MainConfigurator::ReadConfigurationFile: %s magnetic_radial_low_current_weight",this->Name());
			temp_file.Close();
			return False;
		}
		else CStaticAssertErrorCondition(Information,"MainConfigurator::ReadConfigurationFile: magnetic_radial_low_current_weight = %f",magnetic_radial_low_current_weight);
		if(!cdbe.ReadFloat(magnetic_vertical_high_current_weight, "vertical_high_current_weight"))
		{
			CStaticAssertErrorCondition(InitialisationError,"MainConfigurator::ReadConfigurationFile: %s magnetic_vertical_high_current_weight",this->Name());
			temp_file.Close();
			return False;
		}
		else CStaticAssertErrorCondition(Information,"MainConfigurator::ReadConfigurationFile: magnetic_vertical_high_current_weight = %f",magnetic_vertical_high_current_weight);
		if(!cdbe.ReadFloat(magnetic_vertical_low_current_weight, "vertical_low_current_weight"))
		{
			CStaticAssertErrorCondition(InitialisationError,"MainConfigurator::ReadConfigurationFile: %s magnetic_vertical_low_current_weight",this->Name());
			temp_file.Close();
			return False;
		}
		else CStaticAssertErrorCondition(Information,"MainConfigurator::ReadConfigurationFile: magnetic_vertical_low_current_weight = %f",magnetic_vertical_low_current_weight);

	cdbe->MoveToRoot();

	if(!cdbe->Move("+MARTe.+RealTimeThread.+plasma_parameters.sine_probe"))
	{
		CStaticAssertErrorCondition(InitialisationError,"MainConfigurator::ReadConfigurationFile: %s Could not move to \"+MARTe.+RealTimeThread.+plasma_parameters.sine_probe\"",this->Name());
		temp_file.Close();
		return False;
	}
		if(!cdbe.ReadFloat(sine_vertical_high_current_weight, "vertical_high_current_weight"))
		{
			CStaticAssertErrorCondition(InitialisationError,"MainConfigurator::ReadConfigurationFile: %s sine_vertical_high_current_weight",this->Name());
			temp_file.Close();
			return False;
		}
		else CStaticAssertErrorCondition(Information,"MainConfigurator::ReadConfigurationFile: sine_vertical_high_current_weight = %f",sine_vertical_high_current_weight);
		if(!cdbe.ReadFloat(sine_vertical_low_current_weight, "vertical_low_current_weight"))
		{
			CStaticAssertErrorCondition(InitialisationError,"MainConfigurator::ReadConfigurationFile: %s sine_vertical_low_current_weight",this->Name());
			temp_file.Close();
			return False;
		}
		else CStaticAssertErrorCondition(Information,"MainConfigurator::ReadConfigurationFile: sine_vertical_low_current_weight = %f",sine_vertical_low_current_weight);

	cdbe->MoveToRoot();

	if(!cdbe->Move("+MARTe.+RealTimeThread.+plasma_parameters.cosine_probe"))
	{
		CStaticAssertErrorCondition(InitialisationError,"MainConfigurator::ReadConfigurationFile: %s Could not move to \"+MARTe.+RealTimeThread.+plasma_parameters.cosine_probe\"",this->Name());
		temp_file.Close();
		return False;
	}
		if(!cdbe.ReadFloat(cosine_radial_high_current_weight, "radial_high_current_weight"))
		{
			CStaticAssertErrorCondition(InitialisationError,"MainConfigurator::ReadConfigurationFile: %s cosine_radial_high_current_weight",this->Name());
			temp_file.Close();
			return False;
		}
		else CStaticAssertErrorCondition(Information,"MainConfigurator::ReadConfigurationFile: cosine_radial_high_current_weight = %f",cosine_radial_high_current_weight);
		if(!cdbe.ReadFloat(cosine_radial_low_current_weight, "radial_low_current_weight"))
		{
			CStaticAssertErrorCondition(InitialisationError,"MainConfigurator::ReadConfigurationFile: %s cosine_radial_low_current_weight",this->Name());
			temp_file.Close();
			return False;
		}
		else CStaticAssertErrorCondition(Information,"MainConfigurator::ReadConfigurationFile: cosine_radial_low_current_weight = %f",cosine_radial_low_current_weight);

	cdbe->MoveToRoot();

	if(!cdbe->Move("+MARTe.+RealTimeThread.+plasma_parameters.hibd"))
	{
		CStaticAssertErrorCondition(InitialisationError,"MainConfigurator::ReadConfigurationFile: %s Could not move to \"+MARTe.+RealTimeThread.+plasma_parameters.hibd\"",this->Name());
		temp_file.Close();
		return False;
	}
		if(!cdbe.ReadFloat(hibd_radial_high_current_weight, "radial_high_current_weight"))
		{
			CStaticAssertErrorCondition(InitialisationError,"MainConfigurator::ReadConfigurationFile: %s hibd_radial_high_current_weight",this->Name());
			temp_file.Close();
			return False;
		}
		else CStaticAssertErrorCondition(Information,"MainConfigurator::ReadConfigurationFile: hibd_radial_high_current_weight = %f",hibd_radial_high_current_weight);
		if(!cdbe.ReadFloat(hibd_radial_low_current_weight, "radial_low_current_weight"))
		{
			CStaticAssertErrorCondition(InitialisationError,"MainConfigurator::ReadConfigurationFile: %s hibd_radial_low_current_weight",this->Name());
			temp_file.Close();
			return False;
		}
		else CStaticAssertErrorCondition(Information,"MainConfigurator::ReadConfigurationFile: hibd_radial_low_current_weight = %f",hibd_radial_low_current_weight);
		if(!cdbe.ReadFloat(hibd_vertical_high_current_weight, "vertical_high_current_weight"))
		{
			CStaticAssertErrorCondition(InitialisationError,"MainConfigurator::ReadConfigurationFile: %s hibd_vertical_high_current_weight",this->Name());
			temp_file.Close();
			return False;
		}
		else CStaticAssertErrorCondition(Information,"MainConfigurator::ReadConfigurationFile: hibd_vertical_high_current_weight = %f",hibd_vertical_high_current_weight);
		if(!cdbe.ReadFloat(hibd_vertical_low_current_weight, "vertical_low_current_weight"))
		{
			CStaticAssertErrorCondition(InitialisationError,"MainConfigurator::ReadConfigurationFile: %s hibd_vertical_low_current_weight",this->Name());
			temp_file.Close();
			return False;
		}
		else CStaticAssertErrorCondition(Information,"MainConfigurator::ReadConfigurationFile: hibd_vertical_low_current_weight = %f",hibd_vertical_low_current_weight);

	cdbe->MoveToRoot();
	

// read config file section: controller
	if(!cdbe->Move("+MARTe.+RealTimeThread.+controller.PID_horizontal"))
	{
		CStaticAssertErrorCondition(InitialisationError,"MainConfigurator::ReadConfigurationFile: %s Could not move to \"+MARTe.+RealTimeThread.+controller.PID_horizontal\"",this->Name());
		temp_file.Close();
		return False;
	}
		if(!cdbe.ReadFloat(PID_horizontal_proportional_soft, "proportional_soft"))
		{
			CStaticAssertErrorCondition(InitialisationError,"MainConfigurator::ReadConfigurationFile: %s PID_horizontal_proportional_soft",this->Name());
			temp_file.Close();
			return False;
		}
		else CStaticAssertErrorCondition(Information,"MainConfigurator::ReadConfigurationFile: PID_horizontal_proportional_soft = %f",PID_horizontal_proportional_soft);
		if(!cdbe.ReadFloat(PID_horizontal_proportional_normal, "proportional_normal"))
		{
			CStaticAssertErrorCondition(InitialisationError,"MainConfigurator::ReadConfigurationFile: %s PID_horizontal_proportional_normal",this->Name());
			temp_file.Close();
			return False;
		}
		else CStaticAssertErrorCondition(Information,"MainConfigurator::ReadConfigurationFile: PID_horizontal_proportional_normal = %f",PID_horizontal_proportional_normal);
		if(!cdbe.ReadFloat(PID_horizontal_proportional_hard, "proportional_hard"))
		{
			CStaticAssertErrorCondition(InitialisationError,"MainConfigurator::ReadConfigurationFile: %s PID_horizontal_proportional_hard",this->Name());
			temp_file.Close();
			return False;
		}
		else CStaticAssertErrorCondition(Information,"MainConfigurator::ReadConfigurationFile: PID_horizontal_proportional_hard = %f",PID_horizontal_proportional_hard);
		if(!cdbe.ReadFloat(PID_horizontal_integral_soft, "integral_soft"))
		{
			CStaticAssertErrorCondition(InitialisationError,"MainConfigurator::ReadConfigurationFile: %s PID_horizontal_integral_soft",this->Name());
			temp_file.Close();
			return False;
		}
		else CStaticAssertErrorCondition(Information,"MainConfigurator::ReadConfigurationFile: PID_horizontal_integral_soft = %f",PID_horizontal_integral_soft);
		if(!cdbe.ReadFloat(PID_horizontal_integral_normal, "integral_normal"))
		{
			CStaticAssertErrorCondition(InitialisationError,"MainConfigurator::ReadConfigurationFile: %s PID_horizontal_integral_normal",this->Name());
			temp_file.Close();
			return False;
		}
		else CStaticAssertErrorCondition(Information,"MainConfigurator::ReadConfigurationFile: PID_horizontal_integral_normal = %f",PID_horizontal_integral_normal);
		if(!cdbe.ReadFloat(PID_horizontal_integral_hard, "integral_hard"))
		{
			CStaticAssertErrorCondition(InitialisationError,"MainConfigurator::ReadConfigurationFile: %s PID_horizontal_integral_hard",this->Name());
			temp_file.Close();
			return False;
		}
		else CStaticAssertErrorCondition(Information,"MainConfigurator::ReadConfigurationFile: PID_horizontal_integral_hard = %f",PID_horizontal_integral_hard);
		if(!cdbe.ReadFloat(PID_horizontal_derivative_soft, "derivative_soft"))
		{
			CStaticAssertErrorCondition(InitialisationError,"MainConfigurator::ReadConfigurationFile: %s PID_horizontal_derivative_soft",this->Name());
			temp_file.Close();
			return False;
		}
		else CStaticAssertErrorCondition(Information,"MainConfigurator::ReadConfigurationFile: PID_horizontal_derivative_soft = %f",PID_horizontal_derivative_soft);
		if(!cdbe.ReadFloat(PID_horizontal_derivative_normal, "derivative_normal"))
		{
			CStaticAssertErrorCondition(InitialisationError,"MainConfigurator::ReadConfigurationFile: %s PID_horizontal_derivative_normal",this->Name());
			temp_file.Close();
			return False;
		}
		else CStaticAssertErrorCondition(Information,"MainConfigurator::ReadConfigurationFile: PID_horizontal_derivative_normal = %f",PID_horizontal_derivative_normal);
		if(!cdbe.ReadFloat(PID_horizontal_derivative_hard, "derivative_hard"))
		{
			CStaticAssertErrorCondition(InitialisationError,"MainConfigurator::ReadConfigurationFile: %s PID_horizontal_derivative_hard",this->Name());
			temp_file.Close();
			return False;
		}
		else CStaticAssertErrorCondition(Information,"MainConfigurator::ReadConfigurationFile: PID_horizontal_derivative_hard = %f",PID_horizontal_derivative_hard);

	cdbe->MoveToRoot();

	if(!cdbe->Move("+MARTe.+RealTimeThread.+controller.PID_vertical"))
	{
		CStaticAssertErrorCondition(InitialisationError,"MainConfigurator::ReadConfigurationFile: %s Could not move to \"+MARTe.+RealTimeThread.+controller.PID_vertical\"",this->Name());
		temp_file.Close();
		return False;
	}
		if(!cdbe.ReadFloat(PID_vertical_proportional_soft, "proportional_soft"))
		{
			CStaticAssertErrorCondition(InitialisationError,"MainConfigurator::ReadConfigurationFile: %s PID_vertical_proportional_soft",this->Name());
			temp_file.Close();
			return False;
		}
		else CStaticAssertErrorCondition(Information,"MainConfigurator::ReadConfigurationFile: PID_vertical_proportional_soft = %f",PID_vertical_proportional_soft);
		if(!cdbe.ReadFloat(PID_vertical_proportional_normal, "proportional_normal"))
		{
			CStaticAssertErrorCondition(InitialisationError,"MainConfigurator::ReadConfigurationFile: %s PID_vertical_proportional_normal",this->Name());
			temp_file.Close();
			return False;
		}
		else CStaticAssertErrorCondition(Information,"MainConfigurator::ReadConfigurationFile: PID_vertical_proportional_normal = %f",PID_vertical_proportional_normal);
		if(!cdbe.ReadFloat(PID_vertical_proportional_hard, "proportional_hard"))
		{
			CStaticAssertErrorCondition(InitialisationError,"MainConfigurator::ReadConfigurationFile: %s PID_vertical_proportional_hard",this->Name());
			temp_file.Close();
			return False;
		}
		else CStaticAssertErrorCondition(Information,"MainConfigurator::ReadConfigurationFile: PID_vertical_proportional_hard = %f",PID_vertical_proportional_hard);
		if(!cdbe.ReadFloat(PID_vertical_integral_soft, "integral_soft"))
		{
			CStaticAssertErrorCondition(InitialisationError,"MainConfigurator::ReadConfigurationFile: %s PID_vertical_integral_soft",this->Name());
			temp_file.Close();
			return False;
		}
		else CStaticAssertErrorCondition(Information,"MainConfigurator::ReadConfigurationFile: PID_vertical_integral_soft = %f",PID_vertical_integral_soft);
		if(!cdbe.ReadFloat(PID_vertical_integral_normal, "integral_normal"))
		{
			CStaticAssertErrorCondition(InitialisationError,"MainConfigurator::ReadConfigurationFile: %s PID_vertical_integral_normal",this->Name());
			temp_file.Close();
			return False;
		}
		else CStaticAssertErrorCondition(Information,"MainConfigurator::ReadConfigurationFile: PID_vertical_integral_normal = %f",PID_vertical_integral_normal);
		if(!cdbe.ReadFloat(PID_vertical_integral_hard, "integral_hard"))
		{
			CStaticAssertErrorCondition(InitialisationError,"MainConfigurator::ReadConfigurationFile: %s PID_vertical_integral_hard",this->Name());
			temp_file.Close();
			return False;
		}
		else CStaticAssertErrorCondition(Information,"MainConfigurator::ReadConfigurationFile: PID_vertical_integral_hard = %f",PID_vertical_integral_hard);
		if(!cdbe.ReadFloat(PID_vertical_derivative_soft, "derivative_soft"))
		{
			CStaticAssertErrorCondition(InitialisationError,"MainConfigurator::ReadConfigurationFile: %s PID_vertical_derivative_soft",this->Name());
			temp_file.Close();
			return False;
		}
		else CStaticAssertErrorCondition(Information,"MainConfigurator::ReadConfigurationFile: PID_vertical_derivative_soft = %f",PID_vertical_derivative_soft);
		if(!cdbe.ReadFloat(PID_vertical_derivative_normal, "derivative_normal"))
		{
			CStaticAssertErrorCondition(InitialisationError,"MainConfigurator::ReadConfigurationFile: %s PID_vertical_derivative_normal",this->Name());
			temp_file.Close();
			return False;
		}
		else CStaticAssertErrorCondition(Information,"MainConfigurator::ReadConfigurationFile: PID_vertical_derivative_normal = %f",PID_vertical_derivative_normal);
		if(!cdbe.ReadFloat(PID_vertical_derivative_hard, "derivative_hard"))
		{
			CStaticAssertErrorCondition(InitialisationError,"MainConfigurator::ReadConfigurationFile: %s PID_vertical_derivative_hard",this->Name());
			temp_file.Close();
			return False;
		}
		else CStaticAssertErrorCondition(Information,"MainConfigurator::ReadConfigurationFile: PID_vertical_derivative_hard = %f",PID_vertical_derivative_hard);

	cdbe->MoveToRoot();

	if(!cdbe->Move("+MARTe.+RealTimeThread.+controller.PID_primary"))
	{
		CStaticAssertErrorCondition(InitialisationError,"MainConfigurator::ReadConfigurationFile: %s Could not move to \"+MARTe.+RealTimeThread.+controller.PID_primary\"",this->Name());
		temp_file.Close();
		return False;
	}
		if(!cdbe.ReadFloat(PID_primary_proportional_soft, "proportional_soft"))
		{
			CStaticAssertErrorCondition(InitialisationError,"MainConfigurator::ReadConfigurationFile: %s PID_primary_proportional_soft",this->Name());
			temp_file.Close();
			return False;
		}
		else CStaticAssertErrorCondition(Information,"MainConfigurator::ReadConfigurationFile: PID_primary_proportional_soft = %f",PID_primary_proportional_soft);
		if(!cdbe.ReadFloat(PID_primary_proportional_normal, "proportional_normal"))
		{
			CStaticAssertErrorCondition(InitialisationError,"MainConfigurator::ReadConfigurationFile: %s PID_primary_proportional_normal",this->Name());
			temp_file.Close();
			return False;
		}
		else CStaticAssertErrorCondition(Information,"MainConfigurator::ReadConfigurationFile: PID_primary_proportional_normal = %f",PID_primary_proportional_normal);
		if(!cdbe.ReadFloat(PID_primary_proportional_hard, "proportional_hard"))
		{
			CStaticAssertErrorCondition(InitialisationError,"MainConfigurator::ReadConfigurationFile: %s PID_primary_proportional_hard",this->Name());
			temp_file.Close();
			return False;
		}
		else CStaticAssertErrorCondition(Information,"MainConfigurator::ReadConfigurationFile: PID_primary_proportional_hard = %f",PID_primary_proportional_hard);
		if(!cdbe.ReadFloat(PID_primary_integral_soft, "integral_soft"))
		{
			CStaticAssertErrorCondition(InitialisationError,"MainConfigurator::ReadConfigurationFile: %s PID_primary_integral_soft",this->Name());
			temp_file.Close();
			return False;
		}
		else CStaticAssertErrorCondition(Information,"MainConfigurator::ReadConfigurationFile: PID_primary_integral_soft = %f",PID_primary_integral_soft);
		if(!cdbe.ReadFloat(PID_primary_integral_normal, "integral_normal"))
		{
			CStaticAssertErrorCondition(InitialisationError,"MainConfigurator::ReadConfigurationFile: %s PID_primary_integral_normal",this->Name());
			temp_file.Close();
			return False;
		}
		else CStaticAssertErrorCondition(Information,"MainConfigurator::ReadConfigurationFile: PID_primary_integral_normal = %f",PID_primary_integral_normal);
		if(!cdbe.ReadFloat(PID_primary_integral_hard, "integral_hard"))
		{
			CStaticAssertErrorCondition(InitialisationError,"MainConfigurator::ReadConfigurationFile: %s PID_primary_integral_hard",this->Name());
			temp_file.Close();
			return False;
		}
		else CStaticAssertErrorCondition(Information,"MainConfigurator::ReadConfigurationFile: PID_primary_integral_hard = %f",PID_primary_integral_hard);
		if(!cdbe.ReadFloat(PID_primary_derivative_soft, "derivative_soft"))
		{
			CStaticAssertErrorCondition(InitialisationError,"MainConfigurator::ReadConfigurationFile: %s PID_primary_derivative_soft",this->Name());
			temp_file.Close();
			return False;
		}
		else CStaticAssertErrorCondition(Information,"MainConfigurator::ReadConfigurationFile: PID_primary_derivative_soft = %f",PID_primary_derivative_soft);
		if(!cdbe.ReadFloat(PID_primary_derivative_normal, "derivative_normal"))
		{
			CStaticAssertErrorCondition(InitialisationError,"MainConfigurator::ReadConfigurationFile: %s PID_primary_derivative_normal",this->Name());
			temp_file.Close();
			return False;
		}
		else CStaticAssertErrorCondition(Information,"MainConfigurator::ReadConfigurationFile: PID_primary_derivative_normal = %f",PID_primary_derivative_normal);
		if(!cdbe.ReadFloat(PID_primary_derivative_hard, "derivative_hard"))
		{
			CStaticAssertErrorCondition(InitialisationError,"MainConfigurator::ReadConfigurationFile: %s PID_primary_derivative_hard",this->Name());
			temp_file.Close();
			return False;
		}
		else CStaticAssertErrorCondition(Information,"MainConfigurator::ReadConfigurationFile: PID_primary_derivative_hard = %f",PID_primary_derivative_hard);

	cdbe->MoveToRoot();

// read config file section: PSCommunicator_vertical
	if(!cdbe->Move("+MARTe.+RealTimeThread.+PSCommunicator_vertical"))
	{
		CStaticAssertErrorCondition(InitialisationError,"MainConfigurator::ReadConfigurationFile: %s Could not move to \"+MARTe.+RealTimeThread.+PSCommunicator_vertical\"",this->Name());
		temp_file.Close();
		return False;
	}
		if(!cdbe.ReadFString(verticalPS_UARTPortAddress, "UARTPortAddress"))
		{
			CStaticAssertErrorCondition(InitialisationError,"WaveformsDisplay::ObjectLoadSetup: %s Could not verticalPS_UARTPortAddress",this->Name());
			return False;
		}
		else CStaticAssertErrorCondition(Information,"MainConfigurator::ReadConfigurationFile: verticalPS_UARTPortAddress = %s",verticalPS_UARTPortAddress.Buffer());
		if(!cdbe.ReadFloat(verticalPS_PointOfZeroCurrent, "PointOfZeroCurrent"))
		{
			CStaticAssertErrorCondition(InitialisationError,"MainConfigurator::ReadConfigurationFile: %s verticalPS_PointOfZeroCurrent",this->Name());
			temp_file.Close();
			return False;
		}
		else CStaticAssertErrorCondition(Information,"MainConfigurator::ReadConfigurationFile: verticalPS_PointOfZeroCurrent = %f",verticalPS_PointOfZeroCurrent);
		if(!cdbe.ReadFloat(verticalPS_CurrentStep, "CurrentStep"))
		{
			CStaticAssertErrorCondition(InitialisationError,"MainConfigurator::ReadConfigurationFile: %s verticalPS_CurrentStep",this->Name());
			temp_file.Close();
			return False;
		}
		else CStaticAssertErrorCondition(Information,"MainConfigurator::ReadConfigurationFile: verticalPS_CurrentStep = %f",verticalPS_CurrentStep);

	cdbe->MoveToRoot();

// read config file section: PSCommunicator_horizontal
	if(!cdbe->Move("+MARTe.+RealTimeThread.+PSCommunicator_horizontal"))
	{
		CStaticAssertErrorCondition(InitialisationError,"MainConfigurator::ReadConfigurationFile: %s Could not move to \"+MARTe.+RealTimeThread.+PSCommunicator_horizontal\"",this->Name());
		temp_file.Close();
		return False;
	}
		if(!cdbe.ReadFString(horizontalPS_UARTPortAddress, "UARTPortAddress"))
		{
			CStaticAssertErrorCondition(InitialisationError,"WaveformsDisplay::ObjectLoadSetup: %s Could not horizontalPS_UARTPortAddress",this->Name());
			return False;
		}
		else CStaticAssertErrorCondition(Information,"MainConfigurator::ReadConfigurationFile: horizontalPS_UARTPortAddress = %s",horizontalPS_UARTPortAddress.Buffer());
		if(!cdbe.ReadFloat(horizontalPS_PointOfZeroCurrent, "PointOfZeroCurrent"))
		{
			CStaticAssertErrorCondition(InitialisationError,"MainConfigurator::ReadConfigurationFile: %s horizontalPS_PointOfZeroCurrent",this->Name());
			temp_file.Close();
			return False;
		}
		else CStaticAssertErrorCondition(Information,"MainConfigurator::ReadConfigurationFile: horizontalPS_PointOfZeroCurrent = %f",horizontalPS_PointOfZeroCurrent);
		if(!cdbe.ReadFloat(horizontalPS_CurrentStep, "CurrentStep"))
		{
			CStaticAssertErrorCondition(InitialisationError,"MainConfigurator::ReadConfigurationFile: %s horizontalPS_CurrentStep",this->Name());
			temp_file.Close();
			return False;
		}
		else CStaticAssertErrorCondition(Information,"MainConfigurator::ReadConfigurationFile: horizontalPS_CurrentStep = %f",horizontalPS_CurrentStep);

	cdbe->MoveToRoot();

// read config file section: PSCommunicator_primary
	if(!cdbe->Move("+MARTe.+RealTimeThread.+PSCommunicator_primary"))
	{
		CStaticAssertErrorCondition(InitialisationError,"MainConfigurator::ReadConfigurationFile: %s Could not move to \"+MARTe.+RealTimeThread.+PSCommunicator_primary\"",this->Name());
		temp_file.Close();
		return False;
	}
		if(!cdbe.ReadFString(primaryPS_UARTPortAddress, "UARTPortAddress"))
		{
			CStaticAssertErrorCondition(InitialisationError,"WaveformsDisplay::ObjectLoadSetup: %s Could not primaryPS_UARTPortAddress",this->Name());
			return False;
		}
		else CStaticAssertErrorCondition(Information,"MainConfigurator::ReadConfigurationFile: primaryPS_UARTPortAddress = %s",primaryPS_UARTPortAddress.Buffer());
		if(!cdbe.ReadFloat(primaryPS_PointOfZeroCurrent, "PointOfZeroCurrent"))
		{
			CStaticAssertErrorCondition(InitialisationError,"MainConfigurator::ReadConfigurationFile: %s primaryPS_PointOfZeroCurrent",this->Name());
			temp_file.Close();
			return False;
		}
		else CStaticAssertErrorCondition(Information,"MainConfigurator::ReadConfigurationFile: primaryPS_PointOfZeroCurrent = %f",primaryPS_PointOfZeroCurrent);
		if(!cdbe.ReadFloat(primaryPS_CurrentStep, "CurrentStep"))
		{
			CStaticAssertErrorCondition(InitialisationError,"MainConfigurator::ReadConfigurationFile: %s primaryPS_CurrentStep",this->Name());
			temp_file.Close();
			return False;
		}
		else CStaticAssertErrorCondition(Information,"MainConfigurator::ReadConfigurationFile: primaryPS_CurrentStep = %f",primaryPS_CurrentStep);

	cdbe->MoveToRoot();
	
	// magnetic auxiliary variables
	magnetic_probes_bool_vector = new bool[NumberOfProbes];
	for(i=0; i<NumberOfProbes; i++) magnetic_probes_bool_vector[i] = False;
	for(i=0; i<NumberOfMeasurements; i++) magnetic_probes_bool_vector[(ProbeNumbers[i])] = True;
	
	
	// timewindows auxiliary variables
	for(i=0;i< positive_number_of_time_windows;i++) {
		timewindows_bool_vector[i] = True;
		timewindows_time[i] = positive_time_windows_values[i];
		timewindows_dropdown_vector[(i*3)] = positive_primary_mode[i];
		timewindows_dropdown_vector[(i*3+1)] = positive_vertical_mode[i];
		timewindows_dropdown_vector[(i*3+2)] = positive_horizontal_mode[i];
	}
	for(i=positive_number_of_time_windows; i<7;i++) {
		timewindows_bool_vector[i] = False;
		timewindows_time[i] = 0;
		timewindows_dropdown_vector[(i*3)] = 0;
		timewindows_dropdown_vector[(i*3+1)] = 0;
		timewindows_dropdown_vector[(i*3+2)] = 0;
	}
	for(i=0;i< negative_number_of_time_windows;i++){
		timewindows_bool_vector[(i+7)] = True;
		timewindows_time[(i+7)] = negative_time_windows_values[i];
		timewindows_dropdown_vector[((i+7)*3)] = negative_primary_mode[i];
		timewindows_dropdown_vector[((i+7)*3+1)] = negative_vertical_mode[i];
		timewindows_dropdown_vector[((i+7)*3+2)] = negative_horizontal_mode[i];
	} 
	for(i=negative_number_of_time_windows; i<7;i++){
		timewindows_bool_vector[i+7] = False;
		timewindows_time[(i+7)] = 0;
		timewindows_dropdown_vector[((i+7)*3)] = 0;
		timewindows_dropdown_vector[((i+7)*3+1)] = 0;
		timewindows_dropdown_vector[((i+7)*3+2)] = 0;
	} 
	
	SortTimeWindows();


	temp_file.Close();
	return True;
}

bool MainConfigurator::WriteConfigurationFileWithChanges(char BaseFileFilePath[], char TargetFilePath[]){
	
	CDBExtended cdb;
	int i;
   File configIn;
    if(!configIn.OpenRead((char *)BaseFileFilePath)){
       printf("MainConfigurator:: Failed opening file %s\n", BaseFileFilePath);
       return 0;
    }

    cdb->ReadFromStream(configIn);
    configIn.Close();	

//	Write +ExternalTimeTriggeringService	
	if(!cdb->Move("+MARTe.+ExternalTimeTriggeringService"))
	{
		CStaticAssertErrorCondition(InitialisationError,"MainConfigurator::WriteConfigurationFile: %s Could not move to \"+MARTe.+ExternalTimeTriggeringService\"",this->Name());
		return False;
	}
		if(!cdb.WriteInt32(marte_usec_cycle_time, "TsOnlineUsecPeriod"))
		{
			AssertErrorCondition(InitialisationError,"MainConfigurator::WriteConfigurationFile: %s marte_usec_cycle_time",this->Name());
			return False;
		}
		else AssertErrorCondition(Information,"MainConfigurator::WriteConfigurationFile: marte_usec_cycle_time = %d",marte_usec_cycle_time);
		if(!cdb.WriteInt32(marte_usec_cycle_time, "TsOfflineUsecPeriod"))
		{
			AssertErrorCondition(InitialisationError,"MainConfigurator::WriteConfigurationFile: %s marte_usec_cycle_time",this->Name());
			return False;
		}
		else AssertErrorCondition(Information,"MainConfigurator::WriteConfigurationFile: marte_usec_cycle_time = %d",marte_usec_cycle_time);

	cdb->MoveToRoot();		

// Write DataCollection	
	if(!cdb->Move("+MARTe.+RealTimeThread.+DataCollection.EventTrigger.TimeWindow0"))
	{
		CStaticAssertErrorCondition(InitialisationError,"MainConfigurator::WriteConfigurationFile: %s Could not move to \".+RealTimeThread.+DataCollection.EventTrigger.TimeWindow0\"",this->Name());
		return False;
	}
		if(!cdb.WriteInt32(marte_usec_cycle_time, "UsecPeriod"))
		{
			AssertErrorCondition(InitialisationError,"MainConfigurator::WriteConfigurationFile: %s UsecPeriod",this->Name());
			return False;
		}
		else AssertErrorCondition(Information,"MainConfigurator::WriteConfigurationFile: UsecPeriod = %d",marte_usec_cycle_time);

		datacollection_n_of_samples = (int) (1000*(discharge_time + 10. + (usectime_to_wait_for_starting_operation)/1000.)/(marte_usec_cycle_time));

		if(!cdb.WriteInt32(datacollection_n_of_samples, "NOfSamples"))
		{
			AssertErrorCondition(InitialisationError,"MainConfigurator::WriteConfigurationFile: %s NOfSamples",this->Name());
			return False;
		}
		else AssertErrorCondition(Information,"MainConfigurator::WriteConfigurationFile: NOfSamples = %d",datacollection_n_of_samples);


	cdb->MoveToRoot();

// write config file section: NOfAcquisitionSamples
	if(!cdb->Move("+MARTe.+RealTimeThread.+DataCollection"))
	{
		CStaticAssertErrorCondition(InitialisationError,"MainConfigurator::WriteConfigurationFile: %s Could not move to \".+RealTimeThread.+DataCollection\"",this->Name());
		return False;
	}

		if(!cdb.WriteInt32(datacollection_n_of_samples, "NOfAcquisitionSamples"))
		{
			AssertErrorCondition(InitialisationError,"MainConfigurator::WriteConfigurationFile: %s NOfAcquisitionSamples",this->Name());
			return False;
		}
		else AssertErrorCondition(Information,"MainConfigurator::WriteConfigurationFile: NOfAcquisitionSamples = %d",datacollection_n_of_samples);

	cdb->MoveToRoot();
		

// write config file section: waveform_primary
	if(!cdb->Move("+MARTe.+RealTimeThread.+waveform_primary.waveform_mode_1_positive"))
	{
		CStaticAssertErrorCondition(InitialisationError,"MainConfigurator::WriteConfigurationFile: %s Could not move to \"+MARTe.+RealTimeThread.+waveform_primary.waveform_mode_1_positive\"",this->Name());
		return False;
	}
		if(!cdb.WriteInt32(primary_1_p_vector_size, "vector_size"))
		{
			CStaticAssertErrorCondition(InitialisationError,"MainConfigurator::WriteConfigurationFile: %s primary_1_p_vector_size",this->Name());
			return False;
		}
		else	CStaticAssertErrorCondition(Information,"MainConfigurator::WriteConfigurationFile: primary_1_p_vector_size = %d",primary_1_p_vector_size);
		if(!cdb.WriteFloatArray(primary_1_p_index_vector, (int *)(&primary_1_p_vector_size), 1, "index_vector"))
		{
			CStaticAssertErrorCondition(InitialisationError,"WriteWaveformFiles: Could not write primary_1_p_index_vector");
			return False;
		}
		else for(i=0;i<primary_1_p_vector_size;i++) CStaticAssertErrorCondition(Information,"MainConfigurator::WriteConfigurationFile: primary_1_p_index_vector[%d] = %f",i, primary_1_p_index_vector[i]);
		if(!cdb.WriteFloatArray(primary_1_p_data_vector, (int *)(&primary_1_p_vector_size), 1, "data_vector"))
		{
			CStaticAssertErrorCondition(InitialisationError,"WriteWaveformFiles: Could not write primary_1_p_data_vector");
			return False;
		}
		else for(i=0;i<primary_1_p_vector_size;i++) CStaticAssertErrorCondition(Information,"MainConfigurator::WriteConfigurationFile: primary_1_p_data_vector[%d] = %f",i, primary_1_p_data_vector[i]);
		if(!cdb.WriteFloat(primary_1_p_max_value, "max_value"))
		{
			CStaticAssertErrorCondition(InitialisationError,"MainConfigurator::WriteConfigurationFile: %s primary_1_p_max_value",this->Name());
			return False;
		}
		else	CStaticAssertErrorCondition(Information,"MainConfigurator::WriteConfigurationFile: primary_1_p_max_value = %f",primary_1_p_max_value);
		if(!cdb.WriteFloat(primary_1_p_min_value, "min_value"))
		{
			CStaticAssertErrorCondition(InitialisationError,"MainConfigurator::WriteConfigurationFile: %s primary_1_p_min_value",this->Name());
			return False;
		}
		else	CStaticAssertErrorCondition(Information,"MainConfigurator::WriteConfigurationFile: primary_1_p_min_value = %f",primary_1_p_min_value);
		
	cdb->MoveToRoot();	
	
	if(!cdb->Move("+MARTe.+RealTimeThread.+waveform_primary.waveform_mode_1_negative"))
	{
		CStaticAssertErrorCondition(InitialisationError,"MainConfigurator::WriteConfigurationFile: %s Could not move to \"+MARTe.+RealTimeThread.+waveform_primary.waveform_mode_1_negative\"",this->Name());
		return False;
	}
		if(!cdb.WriteInt32(primary_1_n_vector_size, "vector_size"))
		{
			CStaticAssertErrorCondition(InitialisationError,"MainConfigurator::WriteConfigurationFile: %s primary_1_n_vector_size",this->Name());
			return False;
		}
		else	CStaticAssertErrorCondition(Information,"MainConfigurator::WriteConfigurationFile: primary_1_n_vector_size = %d",primary_1_n_vector_size);
		if(!cdb.WriteFloatArray(primary_1_n_index_vector, (int *)(&primary_1_n_vector_size), 1, "index_vector"))
		{
			CStaticAssertErrorCondition(InitialisationError,"WriteWaveformFiles: Could not write primary_1_n_index_vector");
			return False;
		}
		else for(i=0;i<primary_1_n_vector_size;i++) CStaticAssertErrorCondition(Information,"MainConfigurator::WriteConfigurationFile: primary_1_n_index_vector[%d] = %f",i, primary_1_n_index_vector[i]);
		if(!cdb.WriteFloatArray(primary_1_n_data_vector, (int *)(&primary_1_n_vector_size), 1, "data_vector"))
		{
			CStaticAssertErrorCondition(InitialisationError,"WriteWaveformFiles: Could not write primary_1_n_data_vector");
			return False;
		}
		else for(i=0;i<primary_1_n_vector_size;i++) CStaticAssertErrorCondition(Information,"MainConfigurator::WriteConfigurationFile: primary_1_n_data_vector[%d] = %f",i, primary_1_n_data_vector[i]);
	
	cdb->MoveToRoot();
	
	if(!cdb->Move("+MARTe.+RealTimeThread.+waveform_primary.waveform_mode_2_positive"))
	{
		CStaticAssertErrorCondition(InitialisationError,"MainConfigurator::WriteConfigurationFile: %s Could not move to \"+MARTe.+RealTimeThread.+waveform_primary.waveform_mode_2_positive\"",this->Name());
		return False;
	}
		if(!cdb.WriteInt32(primary_2_p_vector_size, "vector_size"))
		{
			CStaticAssertErrorCondition(InitialisationError,"MainConfigurator::WriteConfigurationFile: %s primary_2_p_vector_size",this->Name());
			return False;
		}
		else	CStaticAssertErrorCondition(Information,"MainConfigurator::WriteConfigurationFile: primary_2_p_vector_size = %d",primary_2_p_vector_size);
		if(!cdb.WriteFloatArray(primary_2_p_index_vector, (int *)(&primary_2_p_vector_size), 1, "index_vector"))
		{
			CStaticAssertErrorCondition(InitialisationError,"WriteWaveformFiles: Could not write primary_2_p_index_vector");
			return False;
		}
		else for(i=0;i<primary_2_p_vector_size;i++) CStaticAssertErrorCondition(Information,"MainConfigurator::WriteConfigurationFile: primary_2_p_index_vector[%d] = %f",i, primary_2_p_index_vector[i]);
		if(!cdb.WriteFloatArray(primary_2_p_data_vector, (int *)(&primary_2_p_vector_size), 1, "data_vector"))
		{
			CStaticAssertErrorCondition(InitialisationError,"WriteWaveformFiles: Could not write primary_1_p_data_vector");
			return False;
		}
		else for(i=0;i<primary_2_p_vector_size;i++) CStaticAssertErrorCondition(Information,"MainConfigurator::WriteConfigurationFile: primary_2_p_data_vector[%d] = %f",i, primary_2_p_data_vector[i]);
		if(!cdb.WriteFloat(primary_2_p_max_value, "max_value"))
		{
			CStaticAssertErrorCondition(InitialisationError,"MainConfigurator::WriteConfigurationFile: %s primary_2_p_max_value",this->Name());
			return False;
		}
		else	CStaticAssertErrorCondition(Information,"MainConfigurator::WriteConfigurationFile: primary_2_p_max_value = %f",primary_2_p_max_value);
		if(!cdb.WriteFloat(primary_2_p_min_value, "min_value"))
		{
			CStaticAssertErrorCondition(InitialisationError,"MainConfigurator::WriteConfigurationFile: %s primary_2_p_min_value",this->Name());
			return False;
		}
		else	CStaticAssertErrorCondition(Information,"MainConfigurator::WriteConfigurationFile: primary_2_p_min_value = %f",primary_2_p_min_value);
		
	cdb->MoveToRoot();	
	
	if(!cdb->Move("+MARTe.+RealTimeThread.+waveform_primary.waveform_mode_2_negative"))
	{
		CStaticAssertErrorCondition(InitialisationError,"MainConfigurator::WriteConfigurationFile: %s Could not move to \"+MARTe.+RealTimeThread.+waveform_primary.waveform_mode_2_negative\"",this->Name());
		return False;
	}
		if(!cdb.WriteInt32(primary_2_n_vector_size, "vector_size"))
		{
			CStaticAssertErrorCondition(InitialisationError,"MainConfigurator::WriteConfigurationFile: %s primary_2_n_vector_size",this->Name());
			return False;
		}
		else	CStaticAssertErrorCondition(Information,"MainConfigurator::WriteConfigurationFile: primary_2_n_vector_size = %d",primary_2_n_vector_size);
		if(!cdb.WriteFloatArray(primary_2_n_index_vector, (int *)(&primary_2_n_vector_size), 1, "index_vector"))
		{
			CStaticAssertErrorCondition(InitialisationError,"WriteWaveformFiles: Could not write primary_2_n_index_vector");
			return False;
		}
		else for(i=0;i<primary_2_n_vector_size;i++) CStaticAssertErrorCondition(Information,"MainConfigurator::WriteConfigurationFile: primary_2_n_index_vector[%d] = %f",i, primary_2_n_index_vector[i]);
		if(!cdb.WriteFloatArray(primary_2_n_data_vector, (int *)(&primary_2_n_vector_size), 1, "data_vector"))
		{
			CStaticAssertErrorCondition(InitialisationError,"WriteWaveformFiles: Could not write primary_2_n_data_vector");
			return False;
		}
		else for(i=0;i<primary_2_n_vector_size;i++) CStaticAssertErrorCondition(Information,"MainConfigurator::WriteConfigurationFile: primary_2_n_data_vector[%d] = %f",i, primary_2_n_data_vector[i]);
	
	cdb->MoveToRoot();

// write config file section: waveform_vertical
	if(!cdb->Move("+MARTe.+RealTimeThread.+waveform_vertical.waveform_mode_1_positive"))
	{
		CStaticAssertErrorCondition(InitialisationError,"MainConfigurator::WriteConfigurationFile: %s Could not move to \"+MARTe.+RealTimeThread.+waveform_vertical.waveform_mode_1_positive\"",this->Name());
		return False;
	}
		if(!cdb.WriteInt32(vertical_1_p_vector_size, "vector_size"))
		{
			CStaticAssertErrorCondition(InitialisationError,"MainConfigurator::WriteConfigurationFile: %s vertical_1_p_vector_size",this->Name());
			return False;
		}
		else	CStaticAssertErrorCondition(Information,"MainConfigurator::WriteConfigurationFile: vertical_1_p_vector_size = %d",vertical_1_p_vector_size);
		if(!cdb.WriteFloatArray(vertical_1_p_index_vector, (int *)(&vertical_1_p_vector_size), 1, "index_vector"))
		{
			CStaticAssertErrorCondition(InitialisationError,"WriteWaveformFiles: Could not write vertical_1_p_index_vector");
			return False;
		}
		else for(i=0;i<vertical_1_p_vector_size;i++) CStaticAssertErrorCondition(Information,"MainConfigurator::WriteConfigurationFile: vertical_1_p_index_vector[%d] = %f",i, vertical_1_p_index_vector[i]);
		if(!cdb.WriteFloatArray(vertical_1_p_data_vector, (int *)(&vertical_1_p_vector_size), 1, "data_vector"))
		{
			CStaticAssertErrorCondition(InitialisationError,"WriteWaveformFiles: Could not write vertical_1_p_data_vector");
			return False;
		}
		else for(i=0;i<vertical_1_p_vector_size;i++) CStaticAssertErrorCondition(Information,"MainConfigurator::WriteConfigurationFile: vertical_1_p_data_vector[%d] = %f",i, vertical_1_p_data_vector[i]);
		if(!cdb.WriteFloat(vertical_1_p_max_value, "max_value"))
		{
			CStaticAssertErrorCondition(InitialisationError,"MainConfigurator::WriteConfigurationFile: %s vertical_1_p_max_value",this->Name());
			return False;
		}
		else	CStaticAssertErrorCondition(Information,"MainConfigurator::WriteConfigurationFile: vertical_1_p_max_value = %f",vertical_1_p_max_value);
		if(!cdb.WriteFloat(vertical_1_p_min_value, "min_value"))
		{
			CStaticAssertErrorCondition(InitialisationError,"MainConfigurator::WriteConfigurationFile: %s vertical_1_p_min_value",this->Name());
			return False;
		}
		else	CStaticAssertErrorCondition(Information,"MainConfigurator::WriteConfigurationFile: vertical_1_p_min_value = %f",vertical_1_p_min_value);
		
	cdb->MoveToRoot();	
	
	if(!cdb->Move("+MARTe.+RealTimeThread.+waveform_vertical.waveform_mode_1_negative"))
	{
		CStaticAssertErrorCondition(InitialisationError,"MainConfigurator::WriteConfigurationFile: %s Could not move to \"+MARTe.+RealTimeThread.+waveform_vertical.waveform_mode_1_negative\"",this->Name());
		return False;
	}
		if(!cdb.WriteInt32(vertical_1_n_vector_size, "vector_size"))
		{
			CStaticAssertErrorCondition(InitialisationError,"MainConfigurator::WriteConfigurationFile: %s vertical_1_n_vector_size",this->Name());
			return False;
		}
		else	CStaticAssertErrorCondition(Information,"MainConfigurator::WriteConfigurationFile: vertical_1_n_vector_size = %d",vertical_1_n_vector_size);
		if(!cdb.WriteFloatArray(vertical_1_n_index_vector, (int *)(&vertical_1_n_vector_size), 1, "index_vector"))
		{
			CStaticAssertErrorCondition(InitialisationError,"WriteWaveformFiles: Could not write vertical_1_n_index_vector");
			return False;
		}
		else for(i=0;i<vertical_1_n_vector_size;i++) CStaticAssertErrorCondition(Information,"MainConfigurator::WriteConfigurationFile: vertical_1_n_index_vector[%d] = %f",i, vertical_1_n_index_vector[i]);
		if(!cdb.WriteFloatArray(vertical_1_n_data_vector, (int *)(&vertical_1_n_vector_size), 1, "data_vector"))
		{
			CStaticAssertErrorCondition(InitialisationError,"WriteWaveformFiles: Could not write vertical_1_n_data_vector");
			return False;
		}
		else for(i=0;i<vertical_1_n_vector_size;i++) CStaticAssertErrorCondition(Information,"MainConfigurator::WriteConfigurationFile: vertical_1_n_data_vector[%d] = %f",i, vertical_1_n_data_vector[i]);
	
	cdb->MoveToRoot();
	
	if(!cdb->Move("+MARTe.+RealTimeThread.+waveform_vertical.waveform_mode_2_positive"))
	{
		CStaticAssertErrorCondition(InitialisationError,"MainConfigurator::WriteConfigurationFile: %s Could not move to \"+MARTe.+RealTimeThread.+waveform_vertical.waveform_mode_2_positive\"",this->Name());
		return False;
	}
		if(!cdb.WriteInt32(vertical_2_p_vector_size, "vector_size"))
		{
			CStaticAssertErrorCondition(InitialisationError,"MainConfigurator::WriteConfigurationFile: %s vertical_2_p_vector_size",this->Name());
			return False;
		}
		else	CStaticAssertErrorCondition(Information,"MainConfigurator::WriteConfigurationFile: vertical_2_p_vector_size = %d",vertical_2_p_vector_size);
		if(!cdb.WriteFloatArray(vertical_2_p_index_vector, (int *)(&vertical_2_p_vector_size), 1, "index_vector"))
		{
			CStaticAssertErrorCondition(InitialisationError,"WriteWaveformFiles: Could not write vertical_2_p_index_vector");
			return False;
		}
		else for(i=0;i<vertical_2_p_vector_size;i++) CStaticAssertErrorCondition(Information,"MainConfigurator::WriteConfigurationFile: vertical_2_p_index_vector[%d] = %f",i, vertical_2_p_index_vector[i]);
		if(!cdb.WriteFloatArray(vertical_2_p_data_vector, (int *)(&vertical_2_p_vector_size), 1, "data_vector"))
		{
			CStaticAssertErrorCondition(InitialisationError,"WriteWaveformFiles: Could not write vertical_1_p_data_vector");
			return False;
		}
		else for(i=0;i<vertical_2_p_vector_size;i++) CStaticAssertErrorCondition(Information,"MainConfigurator::WriteConfigurationFile: vertical_2_p_data_vector[%d] = %f",i, vertical_2_p_data_vector[i]);
		if(!cdb.WriteFloat(vertical_2_p_max_value, "max_value"))
		{
			CStaticAssertErrorCondition(InitialisationError,"MainConfigurator::WriteConfigurationFile: %s vertical_2_p_max_value",this->Name());
			return False;
		}
		else	CStaticAssertErrorCondition(Information,"MainConfigurator::WriteConfigurationFile: vertical_2_p_max_value = %f",vertical_2_p_max_value);
		if(!cdb.WriteFloat(vertical_2_p_min_value, "min_value"))
		{
			CStaticAssertErrorCondition(InitialisationError,"MainConfigurator::WriteConfigurationFile: %s vertical_2_p_min_value",this->Name());
			return False;
		}
		else	CStaticAssertErrorCondition(Information,"MainConfigurator::WriteConfigurationFile: vertical_2_p_min_value = %f",vertical_2_p_min_value);
		
	cdb->MoveToRoot();	
	
	if(!cdb->Move("+MARTe.+RealTimeThread.+waveform_vertical.waveform_mode_2_negative"))
	{
		CStaticAssertErrorCondition(InitialisationError,"MainConfigurator::WriteConfigurationFile: %s Could not move to \"+MARTe.+RealTimeThread.+waveform_vertical.waveform_mode_2_negative\"",this->Name());
		return False;
	}
		if(!cdb.WriteInt32(vertical_2_n_vector_size, "vector_size"))
		{
			CStaticAssertErrorCondition(InitialisationError,"MainConfigurator::WriteConfigurationFile: %s vertical_2_n_vector_size",this->Name());
			return False;
		}
		else	CStaticAssertErrorCondition(Information,"MainConfigurator::WriteConfigurationFile: vertical_2_n_vector_size = %d",vertical_2_n_vector_size);
		if(!cdb.WriteFloatArray(vertical_2_n_index_vector, (int *)(&vertical_2_n_vector_size), 1, "index_vector"))
		{
			CStaticAssertErrorCondition(InitialisationError,"WriteWaveformFiles: Could not write vertical_2_n_index_vector");
			return False;
		}
		else for(i=0;i<vertical_2_n_vector_size;i++) CStaticAssertErrorCondition(Information,"MainConfigurator::WriteConfigurationFile: vertical_2_n_index_vector[%d] = %f",i, vertical_1_n_index_vector[i]);
		if(!cdb.WriteFloatArray(vertical_2_n_data_vector, (int *)(&vertical_2_n_vector_size), 1, "data_vector"))
		{
			CStaticAssertErrorCondition(InitialisationError,"WriteWaveformFiles: Could not write vertical_2_n_data_vector");
			return False;
		}
		else for(i=0;i<vertical_2_n_vector_size;i++) CStaticAssertErrorCondition(Information,"MainConfigurator::WriteConfigurationFile: vertical_2_n_data_vector[%d] = %f",i, vertical_2_n_data_vector[i]);
	
	cdb->MoveToRoot();

// write config file section: waveform_horizontal
	if(!cdb->Move("+MARTe.+RealTimeThread.+waveform_horizontal.waveform_mode_1_positive"))
	{
		CStaticAssertErrorCondition(InitialisationError,"MainConfigurator::WriteConfigurationFile: %s Could not move to \"+MARTe.+RealTimeThread.+waveform_horizontal.waveform_mode_1_positive\"",this->Name());
		return False;
	}
		if(!cdb.WriteInt32(horizontal_1_p_vector_size, "vector_size"))
		{
			CStaticAssertErrorCondition(InitialisationError,"MainConfigurator::WriteConfigurationFile: %s horizontal_1_p_vector_size",this->Name());
			return False;
		}
		else	CStaticAssertErrorCondition(Information,"MainConfigurator::WriteConfigurationFile: horizontal_1_p_vector_size = %d",horizontal_1_p_vector_size);
		if(!cdb.WriteFloatArray(horizontal_1_p_index_vector, (int *)(&horizontal_1_p_vector_size), 1, "index_vector"))
		{
			CStaticAssertErrorCondition(InitialisationError,"WriteWaveformFiles: Could not write horizontal_1_p_index_vector");
			return False;
		}
		else for(i=0;i<horizontal_1_p_vector_size;i++) CStaticAssertErrorCondition(Information,"MainConfigurator::WriteConfigurationFile: horizontal_1_p_index_vector[%d] = %f",i, horizontal_1_p_index_vector[i]);
		if(!cdb.WriteFloatArray(horizontal_1_p_data_vector, (int *)(&horizontal_1_p_vector_size), 1, "data_vector"))
		{
			CStaticAssertErrorCondition(InitialisationError,"WriteWaveformFiles: Could not write horizontal_1_p_data_vector");
			return False;
		}
		else for(i=0;i<horizontal_1_p_vector_size;i++) CStaticAssertErrorCondition(Information,"MainConfigurator::WriteConfigurationFile: horizontal_1_p_data_vector[%d] = %f",i, horizontal_1_p_data_vector[i]);
		if(!cdb.WriteFloat(horizontal_1_p_max_value, "max_value"))
		{
			CStaticAssertErrorCondition(InitialisationError,"MainConfigurator::WriteConfigurationFile: %s horizontal_1_p_max_value",this->Name());
			return False;
		}
		else	CStaticAssertErrorCondition(Information,"MainConfigurator::WriteConfigurationFile: horizontal_1_p_max_value = %f",horizontal_1_p_max_value);
		if(!cdb.WriteFloat(horizontal_1_p_min_value, "min_value"))
		{
			CStaticAssertErrorCondition(InitialisationError,"MainConfigurator::WriteConfigurationFile: %s horizontal_1_p_min_value",this->Name());
			return False;
		}
		else	CStaticAssertErrorCondition(Information,"MainConfigurator::WriteConfigurationFile: horizontal_1_p_min_value = %f",horizontal_1_p_min_value);
		
	cdb->MoveToRoot();	
	
	if(!cdb->Move("+MARTe.+RealTimeThread.+waveform_horizontal.waveform_mode_1_negative"))
	{
		CStaticAssertErrorCondition(InitialisationError,"MainConfigurator::WriteConfigurationFile: %s Could not move to \"+MARTe.+RealTimeThread.+waveform_horizontal.waveform_mode_1_negative\"",this->Name());
		return False;
	}
		if(!cdb.WriteInt32(horizontal_1_n_vector_size, "vector_size"))
		{
			CStaticAssertErrorCondition(InitialisationError,"MainConfigurator::WriteConfigurationFile: %s horizontal_1_n_vector_size",this->Name());
			return False;
		}
		else	CStaticAssertErrorCondition(Information,"MainConfigurator::WriteConfigurationFile: horizontal_1_n_vector_size = %d",horizontal_1_n_vector_size);
		if(!cdb.WriteFloatArray(horizontal_1_n_index_vector, (int *)(&horizontal_1_n_vector_size), 1, "index_vector"))
		{
			CStaticAssertErrorCondition(InitialisationError,"WriteWaveformFiles: Could not write horizontal_1_n_index_vector");
			return False;
		}
		else for(i=0;i<horizontal_1_n_vector_size;i++) CStaticAssertErrorCondition(Information,"MainConfigurator::WriteConfigurationFile: horizontal_1_n_index_vector[%d] = %f",i, horizontal_1_n_index_vector[i]);
		if(!cdb.WriteFloatArray(horizontal_1_n_data_vector, (int *)(&horizontal_1_n_vector_size), 1, "data_vector"))
		{
			CStaticAssertErrorCondition(InitialisationError,"WriteWaveformFiles: Could not write horizontal_1_n_data_vector");
			return False;
		}
		else for(i=0;i<horizontal_1_n_vector_size;i++) CStaticAssertErrorCondition(Information,"MainConfigurator::WriteConfigurationFile: horizontal_1_n_data_vector[%d] = %f",i, horizontal_1_n_data_vector[i]);
	
	cdb->MoveToRoot();
	
	if(!cdb->Move("+MARTe.+RealTimeThread.+waveform_horizontal.waveform_mode_2_positive"))
	{
		CStaticAssertErrorCondition(InitialisationError,"MainConfigurator::WriteConfigurationFile: %s Could not move to \"+MARTe.+RealTimeThread.+waveform_horizontal.waveform_mode_2_positive\"",this->Name());
		return False;
	}
		if(!cdb.WriteInt32(horizontal_2_p_vector_size, "vector_size"))
		{
			CStaticAssertErrorCondition(InitialisationError,"MainConfigurator::WriteConfigurationFile: %s horizontal_2_p_vector_size",this->Name());
			return False;
		}
		else	CStaticAssertErrorCondition(Information,"MainConfigurator::WriteConfigurationFile: horizontal_2_p_vector_size = %d",horizontal_2_p_vector_size);
		if(!cdb.WriteFloatArray(horizontal_2_p_index_vector, (int *)(&horizontal_2_p_vector_size), 1, "index_vector"))
		{
			CStaticAssertErrorCondition(InitialisationError,"WriteWaveformFiles: Could not write horizontal_2_p_index_vector");
			return False;
		}
		else for(i=0;i<horizontal_2_p_vector_size;i++) CStaticAssertErrorCondition(Information,"MainConfigurator::WriteConfigurationFile: horizontal_2_p_index_vector[%d] = %f",i, horizontal_2_p_index_vector[i]);
		if(!cdb.WriteFloatArray(horizontal_2_p_data_vector, (int *)(&horizontal_2_p_vector_size), 1, "data_vector"))
		{
			CStaticAssertErrorCondition(InitialisationError,"WriteWaveformFiles: Could not write horizontal_1_p_data_vector");
			return False;
		}
		else for(i=0;i<horizontal_2_p_vector_size;i++) CStaticAssertErrorCondition(Information,"MainConfigurator::WriteConfigurationFile: horizontal_2_p_data_vector[%d] = %f",i, horizontal_2_p_data_vector[i]);
		if(!cdb.WriteFloat(horizontal_2_p_max_value, "max_value"))
		{
			CStaticAssertErrorCondition(InitialisationError,"MainConfigurator::WriteConfigurationFile: %s horizontal_2_p_max_value",this->Name());
			return False;
		}
		else	CStaticAssertErrorCondition(Information,"MainConfigurator::WriteConfigurationFile: horizontal_2_p_max_value = %f",horizontal_2_p_max_value);
		if(!cdb.WriteFloat(horizontal_2_p_min_value, "min_value"))
		{
			CStaticAssertErrorCondition(InitialisationError,"MainConfigurator::WriteConfigurationFile: %s horizontal_2_p_min_value",this->Name());
			return False;
		}
		else	CStaticAssertErrorCondition(Information,"MainConfigurator::WriteConfigurationFile: horizontal_2_p_min_value = %f",horizontal_2_p_min_value);
		
	cdb->MoveToRoot();	
	
	if(!cdb->Move("+MARTe.+RealTimeThread.+waveform_horizontal.waveform_mode_2_negative"))
	{
		CStaticAssertErrorCondition(InitialisationError,"MainConfigurator::WriteConfigurationFile: %s Could not move to \"+MARTe.+RealTimeThread.+waveform_horizontal.waveform_mode_2_negative\"",this->Name());
		return False;
	}
		if(!cdb.WriteInt32(horizontal_2_n_vector_size, "vector_size"))
		{
			CStaticAssertErrorCondition(InitialisationError,"MainConfigurator::WriteConfigurationFile: %s horizontal_2_n_vector_size",this->Name());
			return False;
		}
		else	CStaticAssertErrorCondition(Information,"MainConfigurator::WriteConfigurationFile: horizontal_2_n_vector_size = %d",horizontal_2_n_vector_size);
		if(!cdb.WriteFloatArray(horizontal_2_n_index_vector, (int *)(&horizontal_2_n_vector_size), 1, "index_vector"))
		{
			CStaticAssertErrorCondition(InitialisationError,"WriteWaveformFiles: Could not write horizontal_2_n_index_vector");
			return False;
		}
		else for(i=0;i<horizontal_2_n_vector_size;i++) CStaticAssertErrorCondition(Information,"MainConfigurator::WriteConfigurationFile: horizontal_2_n_index_vector[%d] = %f",i, horizontal_1_n_index_vector[i]);
		if(!cdb.WriteFloatArray(horizontal_2_n_data_vector, (int *)(&horizontal_2_n_vector_size), 1, "data_vector"))
		{
			CStaticAssertErrorCondition(InitialisationError,"WriteWaveformFiles: Could not write horizontal_2_n_data_vector");
			return False;
		}
		else for(i=0;i<horizontal_2_n_vector_size;i++) CStaticAssertErrorCondition(Information,"MainConfigurator::WriteConfigurationFile: horizontal_2_n_data_vector[%d] = %f",i, horizontal_2_n_data_vector[i]);
	
	cdb->MoveToRoot();

// write config file section: time_windows
	if(!cdb->Move("+MARTe.+RealTimeThread.+time_windows"))
	{
		CStaticAssertErrorCondition(InitialisationError,"MainConfigurator::WriteConfigurationFile: %s Could not move to \"+MARTe.+RealTimeThread.+time_windows\"",this->Name());
		return False;
	}
		if(!cdb.WriteFloat(discharge_time, "discharge_time"))
		{
			CStaticAssertErrorCondition(InitialisationError,"MainConfigurator::WriteConfigurationFile: %s discharge_time",this->Name());
			return False;
		}
		else	CStaticAssertErrorCondition(Information,"MainConfigurator::WriteConfigurationFile: discharge_time = %f",discharge_time);
		if(!cdb.WriteInt32(number_of_cycles, "number_of_cycles"))
		{
			CStaticAssertErrorCondition(InitialisationError,"MainConfigurator::WriteConfigurationFile: %s number_of_cycles",this->Name());
			return False;
		}
		else	CStaticAssertErrorCondition(Information,"MainConfigurator::WriteConfigurationFile: number_of_cycles = %d",number_of_cycles);
		if(!cdb.WriteInt32((int) first_cycle_positive_bool, "first_cycle_positive_bool"))
		{
			CStaticAssertErrorCondition(InitialisationError,"MainConfigurator::WriteConfigurationFile: %s first_cycle_positive_bool",this->Name());
			return False;
		}
		else CStaticAssertErrorCondition(Information,"MainConfigurator::WriteConfigurationFile: first_cycle_positive_bool = %d",first_cycle_positive_bool);
		if(!cdb.WriteInt32((int) auto_breakdown, "auto_breakdown"))
		{
			CStaticAssertErrorCondition(InitialisationError,"MainConfigurator::WriteConfigurationFile: %s auto_breakdown",this->Name());
			return False;
		}
		else CStaticAssertErrorCondition(Information,"MainConfigurator::WriteConfigurationFile: auto_breakdown = %d",auto_breakdown);
		if(!cdb.WriteInt32(marte_usec_cycle_time, "usecthread_cycle_time"))
		{
			AssertErrorCondition(InitialisationError,"MainConfigurator::WriteConfigurationFile: %s marte_usec_cycle_time",this->Name());
			return False;
		}
		else AssertErrorCondition(Information,"MainConfigurator::WriteConfigurationFile: marte_usec_cycle_time = %d",marte_usec_cycle_time);
		if(!cdb.WriteInt32(usectime_to_wait_for_starting_operation, "usectime_to_wait_for_starting_operation"))
		{
			AssertErrorCondition(InitialisationError,"MainConfigurator::WriteConfigurationFile: %s usectime_to_wait_for_starting_operation",this->Name());
			return False;
		}
		else AssertErrorCondition(Information,"MainConfigurator::WriteConfigurationFile: usectime_to_wait_for_starting_operation = %d",usectime_to_wait_for_starting_operation);

	cdb->MoveToRoot();

	if(!cdb->Move("+MARTe.+RealTimeThread.+time_windows.positive_time_windows"))
	{
		CStaticAssertErrorCondition(InitialisationError,"MainConfigurator::WriteConfigurationFile: %s Could not move to \"+MARTe.+RealTimeThread.+time_windows.positive_time_windows\"",this->Name());
		return False;
	}
		if(!cdb.WriteInt32(positive_number_of_time_windows, "number_of_time_windows"))
		{
			CStaticAssertErrorCondition(InitialisationError,"MainConfigurator::WriteConfigurationFile: %s positive_number_of_time_windows",this->Name());
			return False;
		}
		else	CStaticAssertErrorCondition(Information,"MainConfigurator::WriteConfigurationFile: positive_number_of_time_windows = %d",positive_number_of_time_windows);
		if(!cdb.WriteFloatArray(positive_time_windows_values, (int *)(&positive_number_of_time_windows), 1, "time_windows_values"))
		{
			CStaticAssertErrorCondition(InitialisationError,"WriteWaveformFiles: Could not write positive_time_windows_values");
			return False;
		}
		else for(i=0;i<positive_number_of_time_windows;i++) CStaticAssertErrorCondition(Information,"MainConfigurator::WriteConfigurationFile: positive_time_windows_values[%d] = %f",i, positive_time_windows_values[i]);
		if(!cdb.WriteInt32Array(positive_primary_mode, (int *)(&positive_number_of_time_windows), 1, "primary_mode"))
		{
			CStaticAssertErrorCondition(InitialisationError,"WriteWaveformFiles: Could not positive_primary_mode");
			return False;
		}
		else for(i=0;i<positive_number_of_time_windows;i++) CStaticAssertErrorCondition(Information,"MainConfigurator::WriteConfigurationFile: positive_primary_mode[%d] = %d",i, positive_primary_mode[i]);
		if(!cdb.WriteInt32Array(positive_horizontal_mode, (int *)(&positive_number_of_time_windows), 1, "horizontal_mode"))
		{
			CStaticAssertErrorCondition(InitialisationError,"WriteWaveformFiles: Could not write positive_horizontal_mode");
			return False;
		}
		else for(i=0;i<positive_number_of_time_windows;i++) CStaticAssertErrorCondition(Information,"MainConfigurator::WriteConfigurationFile: positive_horizontal_mode[%d] = %d",i, positive_horizontal_mode[i]);
		if(!cdb.WriteInt32Array(positive_vertical_mode, (int *)(&positive_number_of_time_windows), 1, "vertical_mode"))
		{
			CStaticAssertErrorCondition(InitialisationError,"WriteWaveformFiles: Could not write positive_vertical_mode");
			return False;
		}
		else for(i=0;i<positive_number_of_time_windows;i++) CStaticAssertErrorCondition(Information,"MainConfigurator::WriteConfigurationFile: positive_vertical_mode[%d] = %d",i, positive_vertical_mode[i]);

	cdb->MoveToRoot();


	if(!cdb->Move("+MARTe.+RealTimeThread.+time_windows.negative_time_windows"))
	{
		CStaticAssertErrorCondition(InitialisationError,"MainConfigurator::WriteConfigurationFile: %s Could not move to \"+MARTe.+RealTimeThread.+time_windows.negative_time_windows\"",this->Name());
		return False;
	}
		if(!cdb.WriteInt32(negative_number_of_time_windows, "number_of_time_windows"))
		{
			CStaticAssertErrorCondition(InitialisationError,"MainConfigurator::WriteConfigurationFile: %s negative_number_of_time_windows",this->Name());
			return False;
		}
		else	CStaticAssertErrorCondition(Information,"MainConfigurator::WriteConfigurationFile: negative_number_of_time_windows = %d",negative_number_of_time_windows);
		if(!cdb.WriteFloatArray(negative_time_windows_values, (int *)(&negative_number_of_time_windows), 1, "time_windows_values"))
		{
			CStaticAssertErrorCondition(InitialisationError,"WriteWaveformFiles: Could not write negative_time_windows_values");
			return False;
		}
		else for(i=0;i<negative_number_of_time_windows;i++) CStaticAssertErrorCondition(Information,"MainConfigurator::WriteConfigurationFile: negative_time_windows_values[%d] = %f",i, negative_time_windows_values[i]);
		if(!cdb.WriteInt32Array(negative_primary_mode, (int *)(&negative_number_of_time_windows), 1, "primary_mode"))
		{
			CStaticAssertErrorCondition(InitialisationError,"WriteWaveformFiles: Could not negative_primary_mode");
			return False;
		}
		else for(i=0;i<negative_number_of_time_windows;i++) CStaticAssertErrorCondition(Information,"MainConfigurator::WriteConfigurationFile: negative_primary_mode[%d] = %d",i, negative_primary_mode[i]);
		if(!cdb.WriteInt32Array(negative_horizontal_mode, (int *)(&negative_number_of_time_windows), 1, "horizontal_mode"))
		{
			CStaticAssertErrorCondition(InitialisationError,"WriteWaveformFiles: Could not write negative_horizontal_mode");
			return False;
		}
		else for(i=0;i<negative_number_of_time_windows;i++) CStaticAssertErrorCondition(Information,"MainConfigurator::WriteConfigurationFile: negative_horizontal_mode[%d] = %d",i, negative_horizontal_mode[i]);
		if(!cdb.WriteInt32Array(negative_vertical_mode, (int *)(&negative_number_of_time_windows), 1, "vertical_mode"))
		{
			CStaticAssertErrorCondition(InitialisationError,"WriteWaveformFiles: Could not write negative_vertical_mode");
			return False;
		}
		else for(i=0;i<negative_number_of_time_windows;i++) CStaticAssertErrorCondition(Information,"MainConfigurator::WriteConfigurationFile: negative_vertical_mode[%d] = %d",i, negative_vertical_mode[i]);

	cdb->MoveToRoot();

// write config file section: tomography
	if(!cdb->Move("+MARTe.+RealTimeThread.+tomography"))
	{
		CStaticAssertErrorCondition(InitialisationError,"MainConfigurator::WriteConfigurationFile: %s Could not move to \"+MARTe.+RealTimeThread.+tomography\"",this->Name());
		return False;
	}
		if(!cdb.WriteInt32((int) tomography_radial_bool, "tomography_radial_bool"))
		{
			CStaticAssertErrorCondition(InitialisationError,"MainConfigurator::WriteConfigurationFile: %s tomography_radial_bool",this->Name());
			return False;
		}
		else CStaticAssertErrorCondition(Information,"MainConfigurator::WriteConfigurationFile: tomography_radial_bool = %d",tomography_radial_bool);
		if(!cdb.WriteInt32((int) tomography_vertical_bool, "tomography_vertical_bool"))
		{
			CStaticAssertErrorCondition(InitialisationError,"MainConfigurator::WriteConfigurationFile: %s tomography_vertical_bool",this->Name());
			return False;
		}
		else CStaticAssertErrorCondition(Information,"MainConfigurator::WriteConfigurationFile: tomography_vertical_bool = %d",tomography_vertical_bool);
		if(!cdb.WriteInt32(usectime_to_wait_for_starting_operation, "usectime_to_wait_for_starting_operation"))
		{
			AssertErrorCondition(InitialisationError,"MainConfigurator::WriteConfigurationFile: %s usectime_to_wait_for_starting_operation",this->Name());
			return False;
		}
		else AssertErrorCondition(Information,"MainConfigurator::WriteConfigurationFile: usectime_to_wait_for_starting_operation = %d",usectime_to_wait_for_starting_operation);

	cdb->MoveToRoot();


// write config file section: electric_probes
	if(!cdb->Move("+MARTe.+RealTimeThread.+electric_probes"))
	{
		CStaticAssertErrorCondition(InitialisationError,"MainConfigurator::WriteConfigurationFile: %s Could not move to \"+MARTe.+RealTimeThread.+electric_probes\"",this->Name());
		return False;
	}
		if(!cdb.WriteInt32((int) electric_radial_bool, "electric_radial_bool"))
		{
			CStaticAssertErrorCondition(InitialisationError,"MainConfigurator::WriteConfigurationFile: %s electric_radial_bool",this->Name());
			return False;
		}
		else CStaticAssertErrorCondition(Information,"MainConfigurator::WriteConfigurationFile: electric_radial_bool = %d",electric_radial_bool);
		if(!cdb.WriteInt32((int) electric_vertical_bool, "electric_vertical_bool"))
		{
			CStaticAssertErrorCondition(InitialisationError,"MainConfigurator::WriteConfigurationFile: %s electric_vertical_bool",this->Name());
			return False;
		}
		else CStaticAssertErrorCondition(Information,"MainConfigurator::WriteConfigurationFile: electric_vertical_bool = %d",electric_vertical_bool);
		if(!cdb.WriteInt32(usectime_to_wait_for_starting_operation, "usectime_to_wait_for_starting_operation"))
		{
			AssertErrorCondition(InitialisationError,"MainConfigurator::WriteConfigurationFile: %s usectime_to_wait_for_starting_operation",this->Name());
			return False;
		}
		else AssertErrorCondition(Information,"MainConfigurator::WriteConfigurationFile: usectime_to_wait_for_starting_operation = %d",usectime_to_wait_for_starting_operation);

	cdb->MoveToRoot();


// write config file section: magnetic_probes
	if(!cdb->Move("+MARTe.+RealTimeThread.+magnetic_probes"))
	{
		CStaticAssertErrorCondition(InitialisationError,"MainConfigurator::WriteConfigurationFile: %s Could not move to \"+MARTe.+RealTimeThread.+magnetic_probes\"",this->Name());
		return False;
	}
		if(!cdb.WriteInt32(usectime_to_wait_for_starting_operation, "usectime_to_wait_for_starting_operation"))
		{
			AssertErrorCondition(InitialisationError,"MainConfigurator::WriteConfigurationFile: %s usectime_to_wait_for_starting_operation",this->Name());
			return False;
		}
		else AssertErrorCondition(Information,"MainConfigurator::WriteConfigurationFile: usectime_to_wait_for_starting_operation = %d",usectime_to_wait_for_starting_operation);
		if(!cdb.WriteInt32((int) magnetic_radial_bool, "magnetic_radial_bool"))
		{
			CStaticAssertErrorCondition(InitialisationError,"MainConfigurator::WriteConfigurationFile: %s magnetic_radial_bool",this->Name());
			return False;
		}
		else CStaticAssertErrorCondition(Information,"MainConfigurator::WriteConfigurationFile: magnetic_radial_bool = %d",magnetic_radial_bool);
		if(!cdb.WriteInt32((int) magnetic_vertical_bool, "magnetic_vertical_bool"))
		{
			CStaticAssertErrorCondition(InitialisationError,"MainConfigurator::WriteConfigurationFile: %s magnetic_vertical_bool",this->Name());
			return False;
		}
		else CStaticAssertErrorCondition(Information,"MainConfigurator::WriteConfigurationFile: magnetic_vertical_bool = %d",magnetic_vertical_bool);

	cdb->MoveToRoot();

	if(!cdb->Move("+MARTe.+RealTimeThread.+magnetic_probes.MirnovArrayDescription"))
	{
		CStaticAssertErrorCondition(InitialisationError,"MainConfigurator::WriteConfigurationFile: %s Could not move to \"+MARTe.+RealTimeThread.+magnetic_probes.MirnovArrayDescription\"",this->Name());
		return False;
	}
		if(!cdb.WriteInt32(NumberOfProbes, "NumberOfProbes"))
		{
			CStaticAssertErrorCondition(InitialisationError,"MainConfigurator::WriteConfigurationFile: %s NumberOfProbes",this->Name());
			return False;
		}
		else	CStaticAssertErrorCondition(Information,"MainConfigurator::WriteConfigurationFile: NumberOfProbes = %d",NumberOfProbes);
		if(!cdb.WriteFloatArray(magnetic_Angles, (int *)(&NumberOfProbes), 1, "Angles"))
		{
			CStaticAssertErrorCondition(InitialisationError,"WriteWaveformFiles: Could not write magnetic_Angles");
			return False;
		}
		else for(i=0;i<NumberOfProbes;i++) CStaticAssertErrorCondition(Information,"MainConfigurator::WriteConfigurationFile: magnetic_Angles[%d] = %f",i, magnetic_Angles[i]);
		if(!cdb.WriteFloatArray(magnetic_Calibration, (int *)(&NumberOfProbes), 1, "Calibration"))
		{
			CStaticAssertErrorCondition(InitialisationError,"WriteWaveformFiles: Could not write magnetic_Calibration");
			return False;
		}
		else for(i=0;i<NumberOfProbes;i++) CStaticAssertErrorCondition(Information,"MainConfigurator::WriteConfigurationFile: magnetic_Calibration[%d] = %f",i, magnetic_Calibration[i]);

	cdb->MoveToRoot();

	if(!cdb->Move("+MARTe.+RealTimeThread.+magnetic_probes.Measurements"))
	{
		CStaticAssertErrorCondition(InitialisationError,"MainConfigurator::WriteConfigurationFile: %s Could not move to \"+MARTe.+RealTimeThread.+magnetic_probes.Measurements\"",this->Name());
		return False;
	}
		if(!cdb.WriteInt32(NumberOfMeasurements, "NumberOfMeasurements"))
		{
			CStaticAssertErrorCondition(InitialisationError,"MainConfigurator::WriteConfigurationFile: %s NumberOfMeasurements",this->Name());
			return False;
		}
		else	CStaticAssertErrorCondition(Information,"MainConfigurator::WriteConfigurationFile: NumberOfMeasurements = %d",NumberOfMeasurements);
		if(!cdb.WriteInt32Array(ProbeNumbers, (int *)(&NumberOfMeasurements), 1, "ProbeNumbers"))
		{
			CStaticAssertErrorCondition(InitialisationError,"WriteWaveformFiles: Could not write ProbeNumbers");
			return False;
		}
		else for(i=0;i<NumberOfMeasurements;i++) CStaticAssertErrorCondition(Information,"MainConfigurator::WriteConfigurationFile: ProbeNumbers[%d] = %d",i, ProbeNumbers[i]);

	cdb->MoveToRoot();

// write config file section: sine_probe
	if(!cdb->Move("+MARTe.+RealTimeThread.+sine_probe"))
	{
		CStaticAssertErrorCondition(InitialisationError,"MainConfigurator::WriteConfigurationFile: %s Could not move to \"+MARTe.+RealTimeThread.+sine_probe\"",this->Name());
		return False;
	}
		if(!cdb.WriteInt32(usectime_to_wait_for_starting_operation, "usectime_to_wait_for_starting_operation"))
		{
			AssertErrorCondition(InitialisationError,"MainConfigurator::WriteConfigurationFile: %s usectime_to_wait_for_starting_operation",this->Name());
			return False;
		}
		else AssertErrorCondition(Information,"MainConfigurator::WriteConfigurationFile: usectime_to_wait_for_starting_operation = %d",usectime_to_wait_for_starting_operation);
		if(!cdb.WriteInt32((int) sine_vertical_bool, "sine_vertical_bool"))
		{
			CStaticAssertErrorCondition(InitialisationError,"MainConfigurator::WriteConfigurationFile: %s sine_vertical_bool",this->Name());
			return False;
		}
		else CStaticAssertErrorCondition(Information,"MainConfigurator::WriteConfigurationFile: sine_vertical_bool = %d",sine_vertical_bool);

	cdb->MoveToRoot();

// write config file section: cosine_probe
	if(!cdb->Move("+MARTe.+RealTimeThread.+cosine_probe"))
	{
		CStaticAssertErrorCondition(InitialisationError,"MainConfigurator::WriteConfigurationFile: %s Could not move to \"+MARTe.+RealTimeThread.+cosine_probe\"",this->Name());
		return False;
	}
		if(!cdb.WriteInt32(usectime_to_wait_for_starting_operation, "usectime_to_wait_for_starting_operation"))
		{
			AssertErrorCondition(InitialisationError,"MainConfigurator::WriteConfigurationFile: %s usectime_to_wait_for_starting_operation",this->Name());
			return False;
		}
		else AssertErrorCondition(Information,"MainConfigurator::WriteConfigurationFile: usectime_to_wait_for_starting_operation = %d",usectime_to_wait_for_starting_operation);
		if(!cdb.WriteInt32((int) cosine_radial_bool, "cosine_radial_bool"))
		{
			CStaticAssertErrorCondition(InitialisationError,"MainConfigurator::WriteConfigurationFile: %s cosine_radial_bool",this->Name());
			return False;
		}
		else CStaticAssertErrorCondition(Information,"MainConfigurator::WriteConfigurationFile: cosine_radial_bool = %d",cosine_radial_bool);

	cdb->MoveToRoot();

// write config file section: hibd
	if(!cdb->Move("+MARTe.+RealTimeThread.+hibd"))
	{
		CStaticAssertErrorCondition(InitialisationError,"MainConfigurator::WriteConfigurationFile: %s Could not move to \"+MARTe.+RealTimeThread.+hibd\"",this->Name());
		return False;
	}
		if(!cdb.WriteInt32((int) hibd_radial_bool, "hibd_radial_bool"))
		{
			CStaticAssertErrorCondition(InitialisationError,"MainConfigurator::WriteConfigurationFile: %s hibd_radial_bool",this->Name());
			return False;
		}
		else CStaticAssertErrorCondition(Information,"MainConfigurator::WriteConfigurationFile: hibd_radial_bool = %d",hibd_radial_bool);
		if(!cdb.WriteInt32((int) hibd_vertical_bool, "hibd_vertical_bool"))
		{
			CStaticAssertErrorCondition(InitialisationError,"MainConfigurator::WriteConfigurationFile: %s hibd_vertical_bool",this->Name());
			return False;
		}
		else CStaticAssertErrorCondition(Information,"MainConfigurator::WriteConfigurationFile: hibd_vertical_bool = %d",hibd_vertical_bool);

	cdb->MoveToRoot();

// write config file section: main_rogowski

	if(!cdb->Move("+MARTe.+RealTimeThread.+main_rogowski"))
	{
		CStaticAssertErrorCondition(InitialisationError,"MainConfigurator::WriteConfigurationFile: %s Could not move to \"+MARTe.+RealTimeThread.+main_rogowski\"",this->Name());
		return False;
	}
		if(!cdb.WriteInt32(usectime_to_wait_for_starting_operation, "usectime_to_wait_for_starting_operation"))
		{
			AssertErrorCondition(InitialisationError,"MainConfigurator::WriteConfigurationFile: %s usectime_to_wait_for_starting_operation",this->Name());
			return False;
		}
		else AssertErrorCondition(Information,"MainConfigurator::WriteConfigurationFile: usectime_to_wait_for_starting_operation = %d",usectime_to_wait_for_starting_operation);

	cdb->MoveToRoot();

// write config file section: interferometry
	if(!cdb->Move("+MARTe.+RealTimeThread.+interferometry"))
	{
		CStaticAssertErrorCondition(InitialisationError,"MainConfigurator::WriteConfigurationFile: %s Could not move to \"+MARTe.+RealTimeThread.+interferometry\"",this->Name());
		return False;
	}
		if(!cdb.WriteInt32(usectime_to_wait_for_starting_operation, "usectime_to_wait_for_starting_operation"))
		{
			AssertErrorCondition(InitialisationError,"MainConfigurator::WriteConfigurationFile: %s usectime_to_wait_for_starting_operation",this->Name());
			return False;
		}
		else AssertErrorCondition(Information,"MainConfigurator::WriteConfigurationFile: usectime_to_wait_for_starting_operation = %d",usectime_to_wait_for_starting_operation);
		if(!cdb.WriteInt32((int) interferometry_radial_control_bool, "interferometry_radial_control_bool"))
		{
			CStaticAssertErrorCondition(InitialisationError,"MainConfigurator::WriteConfigurationFile: %s interferometry_radial_control_bool",this->Name());
			return False;
		}
		else CStaticAssertErrorCondition(Information,"MainConfigurator::WriteConfigurationFile: interferometry_radial_control_bool = %d",interferometry_radial_control_bool);

	cdb->MoveToRoot();

// write config file section: H_alpha
	if(!cdb->Move("+MARTe.+RealTimeThread.+H_alpha"))
	{
		CStaticAssertErrorCondition(InitialisationError,"MainConfigurator::WriteConfigurationFile: %s Could not move to \"+MARTe.+RealTimeThread.+H_alpha\"",this->Name());
		return False;
	}
		if(!cdb.WriteInt32(usectime_to_wait_for_starting_operation, "usectime_to_wait_for_starting_operation"))
		{
			AssertErrorCondition(InitialisationError,"MainConfigurator::WriteConfigurationFile: %s usectime_to_wait_for_starting_operation",this->Name());
			return False;
		}
		else AssertErrorCondition(Information,"MainConfigurator::WriteConfigurationFile: usectime_to_wait_for_starting_operation = %d",usectime_to_wait_for_starting_operation);

	cdb->MoveToRoot();

// write config file section: technical_signals
	if(!cdb->Move("+MARTe.+RealTimeThread.+technical_signals"))
	{
		CStaticAssertErrorCondition(InitialisationError,"MainConfigurator::WriteConfigurationFile: %s Could not move to \"+MARTe.+RealTimeThread.+technical_signals\"",this->Name());
		return False;
	}
		if(!cdb.WriteInt32(usectime_to_wait_for_starting_operation, "usectime_to_wait_for_starting_operation"))
		{
			AssertErrorCondition(InitialisationError,"MainConfigurator::WriteConfigurationFile: %s usectime_to_wait_for_starting_operation",this->Name());
			return False;
		}
		else AssertErrorCondition(Information,"MainConfigurator::WriteConfigurationFile: usectime_to_wait_for_starting_operation = %d",usectime_to_wait_for_starting_operation);

	cdb->MoveToRoot();

// write config file section: machine_protection
	if(!cdb->Move("+MARTe.+RealTimeThread.+machine_protection"))
	{
		CStaticAssertErrorCondition(InitialisationError,"MainConfigurator::WriteConfigurationFile: %s Could not move to \"+MARTe.+RealTimeThread.+machine_protection\"",this->Name());
		return False;
	}
		if(!cdb.WriteFloat(iron_core_saturation_value, "iron_core_saturation_value"))
		{
			CStaticAssertErrorCondition(InitialisationError,"MainConfigurator::WriteConfigurationFile: %s iron_core_saturation_value",this->Name());
			return False;
		}
		else CStaticAssertErrorCondition(Information,"MainConfigurator::WriteConfigurationFile: iron_core_saturation_value = %f",iron_core_saturation_value);
		if(!cdb.WriteFloat(iron_core_dangerous_value, "iron_core_dangerous_value"))
		{
			CStaticAssertErrorCondition(InitialisationError,"MainConfigurator::WriteConfigurationFile: %s iron_core_dangerous_value",this->Name());
			return False;
		}
		else CStaticAssertErrorCondition(Information,"MainConfigurator::WriteConfigurationFile: iron_core_dangerous_value = %f",iron_core_dangerous_value);

	cdb->MoveToRoot();

// write config file section: plasma_parameters
	if(!cdb->Move("+MARTe.+RealTimeThread.+plasma_parameters"))
	{
		CStaticAssertErrorCondition(InitialisationError,"MainConfigurator::WriteConfigurationFile: %s Could not move to \"+MARTe.+RealTimeThread.+plasma_parameters\"",this->Name());
		return False;
	}
		if(!cdb.WriteFloat(high_current_threshold_value, "high_current_threshold_value"))
		{
			CStaticAssertErrorCondition(InitialisationError,"MainConfigurator::WriteConfigurationFile: %s high_current_threshold_value",this->Name());
			return False;
		}
		else CStaticAssertErrorCondition(Information,"MainConfigurator::WriteConfigurationFile: high_current_threshold_value = %f",high_current_threshold_value);

	cdb->MoveToRoot();

	if(!cdb->Move("+MARTe.+RealTimeThread.+plasma_parameters.tomography"))
	{
		CStaticAssertErrorCondition(InitialisationError,"MainConfigurator::WriteConfigurationFile: %s Could not move to \"+MARTe.+RealTimeThread.+plasma_parameters.tomography\"",this->Name());
		return False;
	}
		if(!cdb.WriteFloat(tomography_radial_high_current_weight, "radial_high_current_weight"))
		{
			CStaticAssertErrorCondition(InitialisationError,"MainConfigurator::WriteConfigurationFile: %s tomography_radial_high_current_weight",this->Name());
			return False;
		}
		else CStaticAssertErrorCondition(Information,"MainConfigurator::WriteConfigurationFile: tomography_radial_high_current_weight = %f",tomography_radial_high_current_weight);
		if(!cdb.WriteFloat(tomography_radial_low_current_weight, "radial_low_current_weight"))
		{
			CStaticAssertErrorCondition(InitialisationError,"MainConfigurator::WriteConfigurationFile: %s tomography_radial_low_current_weight",this->Name());
			return False;
		}
		else CStaticAssertErrorCondition(Information,"MainConfigurator::WriteConfigurationFile: tomography_radial_low_current_weight = %f",tomography_radial_low_current_weight);
		if(!cdb.WriteFloat(tomography_vertical_high_current_weight, "vertical_high_current_weight"))
		{
			CStaticAssertErrorCondition(InitialisationError,"MainConfigurator::WriteConfigurationFile: %s tomography_vertical_high_current_weight",this->Name());
			return False;
		}
		else CStaticAssertErrorCondition(Information,"MainConfigurator::WriteConfigurationFile: tomography_vertical_high_current_weight = %f",tomography_vertical_high_current_weight);
		if(!cdb.WriteFloat(tomography_vertical_low_current_weight, "vertical_low_current_weight"))
		{
			CStaticAssertErrorCondition(InitialisationError,"MainConfigurator::WriteConfigurationFile: %s tomography_vertical_low_current_weight",this->Name());
			return False;
		}
		else CStaticAssertErrorCondition(Information,"MainConfigurator::WriteConfigurationFile: tomography_vertical_low_current_weight = %f",tomography_vertical_low_current_weight);
		if(!cdb.WriteInt32((int) tomography_radial_bool, "tomography_radial_bool"))
		{
			CStaticAssertErrorCondition(InitialisationError,"MainConfigurator::WriteConfigurationFile: %s tomography_radial_bool",this->Name());
			return False;
		}
		else CStaticAssertErrorCondition(Information,"MainConfigurator::WriteConfigurationFile: tomography_radial_bool = %d",tomography_radial_bool);
		if(!cdb.WriteInt32((int) tomography_vertical_bool, "tomography_vertical_bool"))
		{
			CStaticAssertErrorCondition(InitialisationError,"MainConfigurator::WriteConfigurationFile: %s tomography_vertical_bool",this->Name());
			return False;
		}
		else CStaticAssertErrorCondition(Information,"MainConfigurator::WriteConfigurationFile: tomography_vertical_bool = %d",tomography_vertical_bool);

	cdb->MoveToRoot();

	if(!cdb->Move("+MARTe.+RealTimeThread.+plasma_parameters.electric_probes"))
	{
		CStaticAssertErrorCondition(InitialisationError,"MainConfigurator::WriteConfigurationFile: %s Could not move to \"+MARTe.+RealTimeThread.+plasma_parameters.electric_probes\"",this->Name());
		return False;
	}
		if(!cdb.WriteFloat(electric_radial_high_current_weight, "radial_high_current_weight"))
		{
			CStaticAssertErrorCondition(InitialisationError,"MainConfigurator::WriteConfigurationFile: %s electric_radial_high_current_weight",this->Name());
			return False;
		}
		else CStaticAssertErrorCondition(Information,"MainConfigurator::WriteConfigurationFile: electric_radial_high_current_weight = %f",electric_radial_high_current_weight);
		if(!cdb.WriteFloat(electric_radial_low_current_weight, "radial_low_current_weight"))
		{
			CStaticAssertErrorCondition(InitialisationError,"MainConfigurator::WriteConfigurationFile: %s electric_radial_low_current_weight",this->Name());
			return False;
		}
		else CStaticAssertErrorCondition(Information,"MainConfigurator::WriteConfigurationFile: electric_radial_low_current_weight = %f",electric_radial_low_current_weight);
		if(!cdb.WriteFloat(electric_vertical_high_current_weight, "vertical_high_current_weight"))
		{
			CStaticAssertErrorCondition(InitialisationError,"MainConfigurator::WriteConfigurationFile: %s electric_vertical_high_current_weight",this->Name());
			return False;
		}
		else CStaticAssertErrorCondition(Information,"MainConfigurator::WriteConfigurationFile: electric_vertical_high_current_weight = %f",electric_vertical_high_current_weight);
		if(!cdb.WriteFloat(electric_vertical_low_current_weight, "vertical_low_current_weight"))
		{
			CStaticAssertErrorCondition(InitialisationError,"MainConfigurator::WriteConfigurationFile: %s electric_vertical_low_current_weight",this->Name());
			return False;
		}
		else CStaticAssertErrorCondition(Information,"MainConfigurator::WriteConfigurationFile: electric_vertical_low_current_weight = %f",electric_vertical_low_current_weight);
		if(!cdb.WriteInt32((int) electric_radial_bool, "electric_radial_bool"))
		{
			CStaticAssertErrorCondition(InitialisationError,"MainConfigurator::WriteConfigurationFile: %s electric_radial_bool",this->Name());
			return False;
		}
		else CStaticAssertErrorCondition(Information,"MainConfigurator::WriteConfigurationFile: electric_radial_bool = %d",electric_radial_bool);
		if(!cdb.WriteInt32((int) electric_vertical_bool, "electric_vertical_bool"))
		{
			CStaticAssertErrorCondition(InitialisationError,"MainConfigurator::WriteConfigurationFile: %s electric_vertical_bool",this->Name());
			return False;
		}
		else CStaticAssertErrorCondition(Information,"MainConfigurator::WriteConfigurationFile: electric_vertical_bool = %d",electric_vertical_bool);

	cdb->MoveToRoot();

	if(!cdb->Move("+MARTe.+RealTimeThread.+plasma_parameters.magnetic_probes"))
	{
		CStaticAssertErrorCondition(InitialisationError,"MainConfigurator::WriteConfigurationFile: %s Could not move to \"+MARTe.+RealTimeThread.+plasma_parameters.magnetic_probes\"",this->Name());
		return False;
	}
		if(!cdb.WriteFloat(magnetic_radial_high_current_weight, "radial_high_current_weight"))
		{
			CStaticAssertErrorCondition(InitialisationError,"MainConfigurator::WriteConfigurationFile: %s magnetic_radial_high_current_weight",this->Name());
			return False;
		}
		else CStaticAssertErrorCondition(Information,"MainConfigurator::WriteConfigurationFile: magnetic_radial_high_current_weight = %f",magnetic_radial_high_current_weight);
		if(!cdb.WriteFloat(magnetic_radial_low_current_weight, "radial_low_current_weight"))
		{
			CStaticAssertErrorCondition(InitialisationError,"MainConfigurator::WriteConfigurationFile: %s magnetic_radial_low_current_weight",this->Name());
			return False;
		}
		else CStaticAssertErrorCondition(Information,"MainConfigurator::WriteConfigurationFile: magnetic_radial_low_current_weight = %f",magnetic_radial_low_current_weight);
		if(!cdb.WriteFloat(magnetic_vertical_high_current_weight, "vertical_high_current_weight"))
		{
			CStaticAssertErrorCondition(InitialisationError,"MainConfigurator::WriteConfigurationFile: %s magnetic_vertical_high_current_weight",this->Name());
			return False;
		}
		else CStaticAssertErrorCondition(Information,"MainConfigurator::WriteConfigurationFile: magnetic_vertical_high_current_weight = %f",magnetic_vertical_high_current_weight);
		if(!cdb.WriteFloat(magnetic_vertical_low_current_weight, "vertical_low_current_weight"))
		{
			CStaticAssertErrorCondition(InitialisationError,"MainConfigurator::WriteConfigurationFile: %s magnetic_vertical_low_current_weight",this->Name());
			return False;
		}
		else CStaticAssertErrorCondition(Information,"MainConfigurator::WriteConfigurationFile: magnetic_vertical_low_current_weight = %f",magnetic_vertical_low_current_weight);
		if(!cdb.WriteInt32((int) magnetic_radial_bool, "magnetic_radial_bool"))
		{
			CStaticAssertErrorCondition(InitialisationError,"MainConfigurator::WriteConfigurationFile: %s magnetic_radial_bool",this->Name());
			return False;
		}
		else CStaticAssertErrorCondition(Information,"MainConfigurator::WriteConfigurationFile: magnetic_radial_bool = %d",magnetic_radial_bool);
		if(!cdb.WriteInt32((int) magnetic_vertical_bool, "magnetic_vertical_bool"))
		{
			CStaticAssertErrorCondition(InitialisationError,"MainConfigurator::WriteConfigurationFile: %s magnetic_vertical_bool",this->Name());
			return False;
		}
		else CStaticAssertErrorCondition(Information,"MainConfigurator::WriteConfigurationFile: magnetic_vertical_bool = %d",magnetic_vertical_bool);

	cdb->MoveToRoot();

	if(!cdb->Move("+MARTe.+RealTimeThread.+plasma_parameters.sine_probe"))
	{
		CStaticAssertErrorCondition(InitialisationError,"MainConfigurator::WriteConfigurationFile: %s Could not move to \"+MARTe.+RealTimeThread.+plasma_parameters.sine_probe\"",this->Name());
		return False;
	}
		if(!cdb.WriteFloat(sine_vertical_high_current_weight, "radial_high_current_weight"))
		{
			CStaticAssertErrorCondition(InitialisationError,"MainConfigurator::WriteConfigurationFile: %s sine_vertical_high_current_weight",this->Name());
			return False;
		}
		else CStaticAssertErrorCondition(Information,"MainConfigurator::WriteConfigurationFile: sine_vertical_high_current_weight = %f",sine_vertical_high_current_weight);
		if(!cdb.WriteFloat(sine_vertical_low_current_weight, "radial_low_current_weight"))
		{
			CStaticAssertErrorCondition(InitialisationError,"MainConfigurator::WriteConfigurationFile: %s sine_vertical_low_current_weight",this->Name());
			return False;
		}
		else CStaticAssertErrorCondition(Information,"MainConfigurator::WriteConfigurationFile: sine_vertical_low_current_weight = %f",sine_vertical_low_current_weight);
		if(!cdb.WriteInt32((int) sine_vertical_bool, "sine_vertical_bool"))
		{
			CStaticAssertErrorCondition(InitialisationError,"MainConfigurator::WriteConfigurationFile: %s sine_vertical_bool",this->Name());
			return False;
		}
		else CStaticAssertErrorCondition(Information,"MainConfigurator::WriteConfigurationFile: sine_vertical_bool = %d",sine_vertical_bool);

	cdb->MoveToRoot();

	if(!cdb->Move("+MARTe.+RealTimeThread.+plasma_parameters.cosine_probe"))
	{
		CStaticAssertErrorCondition(InitialisationError,"MainConfigurator::WriteConfigurationFile: %s Could not move to \"+MARTe.+RealTimeThread.+plasma_parameters.cosine_probe\"",this->Name());
		return False;
	}
		if(!cdb.WriteFloat(cosine_radial_high_current_weight, "vertical_high_current_weight"))
		{
			CStaticAssertErrorCondition(InitialisationError,"MainConfigurator::WriteConfigurationFile: %s cosine_radial_high_current_weight",this->Name());
			return False;
		}
		else CStaticAssertErrorCondition(Information,"MainConfigurator::WriteConfigurationFile: cosine_radial_high_current_weight = %f",cosine_radial_high_current_weight);
		if(!cdb.WriteFloat(cosine_radial_low_current_weight, "vertical_low_current_weight"))
		{
			CStaticAssertErrorCondition(InitialisationError,"MainConfigurator::WriteConfigurationFile: %s cosine_radial_low_current_weight",this->Name());
			return False;
		}
		else CStaticAssertErrorCondition(Information,"MainConfigurator::WriteConfigurationFile: cosine_radial_low_current_weight = %f",cosine_radial_low_current_weight);
		if(!cdb.WriteInt32((int) cosine_radial_bool, "cosine_radial_bool"))
		{
			CStaticAssertErrorCondition(InitialisationError,"MainConfigurator::WriteConfigurationFile: %s cosine_radial_bool",this->Name());
			return False;
		}
		else CStaticAssertErrorCondition(Information,"MainConfigurator::WriteConfigurationFile: cosine_radial_bool = %d",cosine_radial_bool);

	cdb->MoveToRoot();

	if(!cdb->Move("+MARTe.+RealTimeThread.+plasma_parameters.hibd"))
	{
		CStaticAssertErrorCondition(InitialisationError,"MainConfigurator::WriteConfigurationFile: %s Could not move to \"+MARTe.+RealTimeThread.+plasma_parameters.hibd\"",this->Name());
		return False;
	}
		if(!cdb.WriteFloat(hibd_radial_high_current_weight, "radial_high_current_weight"))
		{
			CStaticAssertErrorCondition(InitialisationError,"MainConfigurator::WriteConfigurationFile: %s hibd_radial_high_current_weight",this->Name());
			return False;
		}
		else CStaticAssertErrorCondition(Information,"MainConfigurator::WriteConfigurationFile: hibd_radial_high_current_weight = %f",hibd_radial_high_current_weight);
		if(!cdb.WriteFloat(hibd_radial_low_current_weight, "radial_low_current_weight"))
		{
			CStaticAssertErrorCondition(InitialisationError,"MainConfigurator::WriteConfigurationFile: %s hibd_radial_low_current_weight",this->Name());
			return False;
		}
		else CStaticAssertErrorCondition(Information,"MainConfigurator::WriteConfigurationFile: hibd_radial_low_current_weight = %f",hibd_radial_low_current_weight);
		if(!cdb.WriteFloat(hibd_vertical_high_current_weight, "vertical_high_current_weight"))
		{
			CStaticAssertErrorCondition(InitialisationError,"MainConfigurator::WriteConfigurationFile: %s hibd_vertical_high_current_weight",this->Name());
			return False;
		}
		else CStaticAssertErrorCondition(Information,"MainConfigurator::WriteConfigurationFile: hibd_vertical_high_current_weight = %f",hibd_vertical_high_current_weight);
		if(!cdb.WriteFloat(hibd_vertical_low_current_weight, "vertical_low_current_weight"))
		{
			CStaticAssertErrorCondition(InitialisationError,"MainConfigurator::WriteConfigurationFile: %s hibd_vertical_low_current_weight",this->Name());
			return False;
		}
		else CStaticAssertErrorCondition(Information,"MainConfigurator::WriteConfigurationFile: hibd_vertical_low_current_weight = %f",hibd_vertical_low_current_weight);
		if(!cdb.WriteInt32((int) hibd_radial_bool, "hibd_radial_bool"))
		{
			CStaticAssertErrorCondition(InitialisationError,"MainConfigurator::WriteConfigurationFile: %s hibd_radial_bool",this->Name());
			return False;
		}
		else CStaticAssertErrorCondition(Information,"MainConfigurator::WriteConfigurationFile: hibd_radial_bool = %d",hibd_radial_bool);
		if(!cdb.WriteInt32((int) hibd_vertical_bool, "hibd_vertical_bool"))
		{
			CStaticAssertErrorCondition(InitialisationError,"MainConfigurator::WriteConfigurationFile: %s hibd_vertical_bool",this->Name());
			return False;
		}
		else CStaticAssertErrorCondition(Information,"MainConfigurator::WriteConfigurationFile: hibd_vertical_bool = %d",hibd_vertical_bool);

	cdb->MoveToRoot();

// write config file section: controller
	if(!cdb->Move("+MARTe.+RealTimeThread.+controller"))
	{
		CStaticAssertErrorCondition(InitialisationError,"MainConfigurator::WriteConfigurationFile: %s Could not move to \"+MARTe.+RealTimeThread.+controller\"",this->Name());
		return False;
	}
		if(!cdb.WriteInt32(marte_usec_cycle_time, "usecthread_cycle_time"))
		{
			AssertErrorCondition(InitialisationError,"MainConfigurator::WriteConfigurationFile: %s marte_usec_cycle_time",this->Name());
			return False;
		}
		else AssertErrorCondition(Information,"MainConfigurator::WriteConfigurationFile: marte_usec_cycle_time = %d",marte_usec_cycle_time);
		if(!cdb.WriteInt32((int) interferometry_radial_control_bool, "interferometry_radial_control_bool"))
		{
			CStaticAssertErrorCondition(InitialisationError,"MainConfigurator::WriteConfigurationFile: %s interferometry_radial_control_bool",this->Name());
			return False;
		}
		else CStaticAssertErrorCondition(Information,"MainConfigurator::WriteConfigurationFile: interferometry_radial_control_bool = %d",interferometry_radial_control_bool);

	cdb->MoveToRoot();

	if(!cdb->Move("+MARTe.+RealTimeThread.+controller.PID_horizontal"))
	{
		CStaticAssertErrorCondition(InitialisationError,"MainConfigurator::WriteConfigurationFile: %s Could not move to \"+MARTe.+RealTimeThread.+controller.PID_horizontal\"",this->Name());
		return False;
	}
		if(!cdb.WriteFloat(PID_horizontal_proportional_soft, "proportional_soft"))
		{
			CStaticAssertErrorCondition(InitialisationError,"MainConfigurator::WriteConfigurationFile: %s PID_horizontal_proportional_soft",this->Name());
			return False;
		}
		else CStaticAssertErrorCondition(Information,"MainConfigurator::WriteConfigurationFile: PID_horizontal_proportional_soft = %f",PID_horizontal_proportional_soft);
		if(!cdb.WriteFloat(PID_horizontal_proportional_normal, "proportional_normal"))
		{
			CStaticAssertErrorCondition(InitialisationError,"MainConfigurator::WriteConfigurationFile: %s PID_horizontal_proportional_normal",this->Name());
			return False;
		}
		else CStaticAssertErrorCondition(Information,"MainConfigurator::WriteConfigurationFile: PID_horizontal_proportional_normal = %f",PID_horizontal_proportional_normal);
		if(!cdb.WriteFloat(PID_horizontal_proportional_hard, "proportional_hard"))
		{
			CStaticAssertErrorCondition(InitialisationError,"MainConfigurator::WriteConfigurationFile: %s PID_horizontal_proportional_hard",this->Name());
			return False;
		}
		else CStaticAssertErrorCondition(Information,"MainConfigurator::WriteConfigurationFile: PID_horizontal_proportional_hard = %f",PID_horizontal_proportional_hard);
		if(!cdb.WriteFloat(PID_horizontal_integral_soft, "integral_soft"))
		{
			CStaticAssertErrorCondition(InitialisationError,"MainConfigurator::WriteConfigurationFile: %s PID_horizontal_integral_soft",this->Name());
			return False;
		}
		else CStaticAssertErrorCondition(Information,"MainConfigurator::WriteConfigurationFile: PID_horizontal_integral_soft = %f",PID_horizontal_integral_soft);
		if(!cdb.WriteFloat(PID_horizontal_integral_normal, "integral_normal"))
		{
			CStaticAssertErrorCondition(InitialisationError,"MainConfigurator::WriteConfigurationFile: %s PID_horizontal_integral_normal",this->Name());
			return False;
		}
		else CStaticAssertErrorCondition(Information,"MainConfigurator::WriteConfigurationFile: PID_horizontal_integral_normal = %f",PID_horizontal_integral_normal);
		if(!cdb.WriteFloat(PID_horizontal_integral_hard, "integral_hard"))
		{
			CStaticAssertErrorCondition(InitialisationError,"MainConfigurator::WriteConfigurationFile: %s PID_horizontal_integral_hard",this->Name());
			return False;
		}
		else CStaticAssertErrorCondition(Information,"MainConfigurator::WriteConfigurationFile: PID_horizontal_integral_hard = %f",PID_horizontal_integral_hard);
		if(!cdb.WriteFloat(PID_horizontal_derivative_soft, "derivative_soft"))
		{
			CStaticAssertErrorCondition(InitialisationError,"MainConfigurator::WriteConfigurationFile: %s PID_horizontal_derivative_soft",this->Name());
			return False;
		}
		else CStaticAssertErrorCondition(Information,"MainConfigurator::WriteConfigurationFile: PID_horizontal_derivative_soft = %f",PID_horizontal_derivative_soft);
		if(!cdb.WriteFloat(PID_horizontal_derivative_normal, "derivative_normal"))
		{
			CStaticAssertErrorCondition(InitialisationError,"MainConfigurator::WriteConfigurationFile: %s PID_horizontal_derivative_normal",this->Name());
			return False;
		}
		else CStaticAssertErrorCondition(Information,"MainConfigurator::WriteConfigurationFile: PID_horizontal_derivative_normal = %f",PID_horizontal_derivative_normal);
		if(!cdb.WriteFloat(PID_horizontal_derivative_hard, "derivative_hard"))
		{
			CStaticAssertErrorCondition(InitialisationError,"MainConfigurator::WriteConfigurationFile: %s PID_horizontal_derivative_hard",this->Name());
			return False;
		}
		else CStaticAssertErrorCondition(Information,"MainConfigurator::WriteConfigurationFile: PID_horizontal_derivative_hard = %f",PID_horizontal_derivative_hard);

	cdb->MoveToRoot();

	if(!cdb->Move("+MARTe.+RealTimeThread.+controller.PID_vertical"))
	{
		CStaticAssertErrorCondition(InitialisationError,"MainConfigurator::WriteConfigurationFile: %s Could not move to \"+MARTe.+RealTimeThread.+controller.PID_vertical\"",this->Name());
		return False;
	}
		if(!cdb.WriteFloat(PID_vertical_proportional_soft, "proportional_soft"))
		{
			CStaticAssertErrorCondition(InitialisationError,"MainConfigurator::WriteConfigurationFile: %s PID_vertical_proportional_soft",this->Name());
			return False;
		}
		else CStaticAssertErrorCondition(Information,"MainConfigurator::WriteConfigurationFile: PID_vertical_proportional_soft = %f",PID_vertical_proportional_soft);
		if(!cdb.WriteFloat(PID_vertical_proportional_normal, "proportional_normal"))
		{
			CStaticAssertErrorCondition(InitialisationError,"MainConfigurator::WriteConfigurationFile: %s PID_vertical_proportional_normal",this->Name());
			return False;
		}
		else CStaticAssertErrorCondition(Information,"MainConfigurator::WriteConfigurationFile: PID_vertical_proportional_normal = %f",PID_vertical_proportional_normal);
		if(!cdb.WriteFloat(PID_vertical_proportional_hard, "proportional_hard"))
		{
			CStaticAssertErrorCondition(InitialisationError,"MainConfigurator::WriteConfigurationFile: %s PID_vertical_proportional_hard",this->Name());
			return False;
		}
		else CStaticAssertErrorCondition(Information,"MainConfigurator::WriteConfigurationFile: PID_vertical_proportional_hard = %f",PID_vertical_proportional_hard);
		if(!cdb.WriteFloat(PID_vertical_integral_soft, "integral_soft"))
		{
			CStaticAssertErrorCondition(InitialisationError,"MainConfigurator::WriteConfigurationFile: %s PID_vertical_integral_soft",this->Name());
			return False;
		}
		else CStaticAssertErrorCondition(Information,"MainConfigurator::WriteConfigurationFile: PID_vertical_integral_soft = %f",PID_vertical_integral_soft);
		if(!cdb.WriteFloat(PID_vertical_integral_normal, "integral_normal"))
		{
			CStaticAssertErrorCondition(InitialisationError,"MainConfigurator::WriteConfigurationFile: %s PID_vertical_integral_normal",this->Name());
			return False;
		}
		else CStaticAssertErrorCondition(Information,"MainConfigurator::WriteConfigurationFile: PID_vertical_integral_normal = %f",PID_vertical_integral_normal);
		if(!cdb.WriteFloat(PID_vertical_integral_hard, "integral_hard"))
		{
			CStaticAssertErrorCondition(InitialisationError,"MainConfigurator::WriteConfigurationFile: %s PID_vertical_integral_hard",this->Name());
			return False;
		}
		else CStaticAssertErrorCondition(Information,"MainConfigurator::WriteConfigurationFile: PID_vertical_integral_hard = %f",PID_vertical_integral_hard);
		if(!cdb.WriteFloat(PID_vertical_derivative_soft, "derivative_soft"))
		{
			CStaticAssertErrorCondition(InitialisationError,"MainConfigurator::WriteConfigurationFile: %s PID_vertical_derivative_soft",this->Name());
			return False;
		}
		else CStaticAssertErrorCondition(Information,"MainConfigurator::WriteConfigurationFile: PID_vertical_derivative_soft = %f",PID_vertical_derivative_soft);
		if(!cdb.WriteFloat(PID_vertical_derivative_normal, "derivative_normal"))
		{
			CStaticAssertErrorCondition(InitialisationError,"MainConfigurator::WriteConfigurationFile: %s PID_vertical_derivative_normal",this->Name());
			return False;
		}
		else CStaticAssertErrorCondition(Information,"MainConfigurator::WriteConfigurationFile: PID_vertical_derivative_normal = %f",PID_vertical_derivative_normal);
		if(!cdb.WriteFloat(PID_vertical_derivative_hard, "derivative_hard"))
		{
			CStaticAssertErrorCondition(InitialisationError,"MainConfigurator::WriteConfigurationFile: %s PID_vertical_derivative_hard",this->Name());
			return False;
		}
		else CStaticAssertErrorCondition(Information,"MainConfigurator::WriteConfigurationFile: PID_vertical_derivative_hard = %f",PID_vertical_derivative_hard);

	cdb->MoveToRoot();

	if(!cdb->Move("+MARTe.+RealTimeThread.+controller.PID_primary"))
	{
		CStaticAssertErrorCondition(InitialisationError,"MainConfigurator::WriteConfigurationFile: %s Could not move to \"+MARTe.+RealTimeThread.+controller.PID_primary\"",this->Name());
		return False;
	}
		if(!cdb.WriteFloat(PID_primary_proportional_soft, "proportional_soft"))
		{
			CStaticAssertErrorCondition(InitialisationError,"MainConfigurator::WriteConfigurationFile: %s PID_primary_proportional_soft",this->Name());
			return False;
		}
		else CStaticAssertErrorCondition(Information,"MainConfigurator::WriteConfigurationFile: PID_primary_proportional_soft = %f",PID_primary_proportional_soft);
		if(!cdb.WriteFloat(PID_primary_proportional_normal, "proportional_normal"))
		{
			CStaticAssertErrorCondition(InitialisationError,"MainConfigurator::WriteConfigurationFile: %s PID_primary_proportional_normal",this->Name());
			return False;
		}
		else CStaticAssertErrorCondition(Information,"MainConfigurator::WriteConfigurationFile: PID_primary_proportional_normal = %f",PID_primary_proportional_normal);
		if(!cdb.WriteFloat(PID_primary_proportional_hard, "proportional_hard"))
		{
			CStaticAssertErrorCondition(InitialisationError,"MainConfigurator::WriteConfigurationFile: %s PID_primary_proportional_hard",this->Name());
			return False;
		}
		else CStaticAssertErrorCondition(Information,"MainConfigurator::WriteConfigurationFile: PID_primary_proportional_hard = %f",PID_primary_proportional_hard);
		if(!cdb.WriteFloat(PID_primary_integral_soft, "integral_soft"))
		{
			CStaticAssertErrorCondition(InitialisationError,"MainConfigurator::WriteConfigurationFile: %s PID_primary_integral_soft",this->Name());
			return False;
		}
		else CStaticAssertErrorCondition(Information,"MainConfigurator::WriteConfigurationFile: PID_primary_integral_soft = %f",PID_primary_integral_soft);
		if(!cdb.WriteFloat(PID_primary_integral_normal, "integral_normal"))
		{
			CStaticAssertErrorCondition(InitialisationError,"MainConfigurator::WriteConfigurationFile: %s PID_primary_integral_normal",this->Name());
			return False;
		}
		else CStaticAssertErrorCondition(Information,"MainConfigurator::WriteConfigurationFile: PID_primary_integral_normal = %f",PID_primary_integral_normal);
		if(!cdb.WriteFloat(PID_primary_integral_hard, "integral_hard"))
		{
			CStaticAssertErrorCondition(InitialisationError,"MainConfigurator::WriteConfigurationFile: %s PID_primary_integral_hard",this->Name());
			return False;
		}
		else CStaticAssertErrorCondition(Information,"MainConfigurator::WriteConfigurationFile: PID_primary_integral_hard = %f",PID_primary_integral_hard);
		if(!cdb.WriteFloat(PID_primary_derivative_soft, "derivative_soft"))
		{
			CStaticAssertErrorCondition(InitialisationError,"MainConfigurator::WriteConfigurationFile: %s PID_primary_derivative_soft",this->Name());
			return False;
		}
		else CStaticAssertErrorCondition(Information,"MainConfigurator::WriteConfigurationFile: PID_primary_derivative_soft = %f",PID_primary_derivative_soft);
		if(!cdb.WriteFloat(PID_primary_derivative_normal, "derivative_normal"))
		{
			CStaticAssertErrorCondition(InitialisationError,"MainConfigurator::WriteConfigurationFile: %s PID_primary_derivative_normal",this->Name());
			return False;
		}
		else CStaticAssertErrorCondition(Information,"MainConfigurator::WriteConfigurationFile: PID_primary_derivative_normal = %f",PID_primary_derivative_normal);
		if(!cdb.WriteFloat(PID_primary_derivative_hard, "derivative_hard"))
		{
			CStaticAssertErrorCondition(InitialisationError,"MainConfigurator::WriteConfigurationFile: %s PID_primary_derivative_hard",this->Name());
			return False;
		}
		else CStaticAssertErrorCondition(Information,"MainConfigurator::WriteConfigurationFile: PID_primary_derivative_hard = %f",PID_primary_derivative_hard);

	cdb->MoveToRoot();

// write config file section: PSCommunicator_vertical
	if(!cdb->Move("+MARTe.+RealTimeThread.+PSCommunicator_vertical"))
	{
		CStaticAssertErrorCondition(InitialisationError,"MainConfigurator::WriteConfigurationFile: %s Could not move to \"+MARTe.+RealTimeThread.+PSCommunicator_vertical\"",this->Name());
		return False;
	}
		if(!cdb.WriteFString(verticalPS_UARTPortAddress, "UARTPortAddress"))
		{
			CStaticAssertErrorCondition(InitialisationError,"WaveformsDisplay::WriteConfigurationFile: %s Could not verticalPS_UARTPortAddress",this->Name());
			return False;
		}
		else CStaticAssertErrorCondition(Information,"MainConfigurator::WriteConfigurationFile: verticalPS_UARTPortAddress = %s",verticalPS_UARTPortAddress.Buffer());
		if(!cdb.WriteFloat(verticalPS_PointOfZeroCurrent, "PointOfZeroCurrent"))
		{
			CStaticAssertErrorCondition(InitialisationError,"MainConfigurator::WriteConfigurationFile: %s verticalPS_PointOfZeroCurrent",this->Name());
			return False;
		}
		else CStaticAssertErrorCondition(Information,"MainConfigurator::WriteConfigurationFile: verticalPS_PointOfZeroCurrent = %f",verticalPS_PointOfZeroCurrent);
		if(!cdb.WriteFloat(verticalPS_CurrentStep, "CurrentStep"))
		{
			CStaticAssertErrorCondition(InitialisationError,"MainConfigurator::WriteConfigurationFile: %s verticalPS_CurrentStep",this->Name());
			return False;
		}
		else CStaticAssertErrorCondition(Information,"MainConfigurator::WriteConfigurationFile: verticalPS_CurrentStep = %f",verticalPS_CurrentStep);
		if(!cdb.WriteInt32(marte_usec_cycle_time, "usecGlobalPeriod"))
		{
			AssertErrorCondition(InitialisationError,"MainConfigurator::WriteConfigurationFile: %s marte_usec_cycle_time",this->Name());
			return False;
		}
		else AssertErrorCondition(Information,"MainConfigurator::WriteConfigurationFile: marte_usec_cycle_time = %d",marte_usec_cycle_time);

	cdb->MoveToRoot();

// write config file section: PSCommunicator_horizontal
	if(!cdb->Move("+MARTe.+RealTimeThread.+PSCommunicator_horizontal"))
	{
		CStaticAssertErrorCondition(InitialisationError,"MainConfigurator::WriteConfigurationFile: %s Could not move to \"+MARTe.+RealTimeThread.+PSCommunicator_horizontal\"",this->Name());
		return False;
	}
		if(!cdb.WriteFString(horizontalPS_UARTPortAddress, "UARTPortAddress"))
		{
			CStaticAssertErrorCondition(InitialisationError,"WaveformsDisplay::ObjectLoadSetup: %s Could not horizontalPS_UARTPortAddress",this->Name());
			return False;
		}
		else CStaticAssertErrorCondition(Information,"MainConfigurator::WriteConfigurationFile: horizontalPS_UARTPortAddress = %s",horizontalPS_UARTPortAddress.Buffer());
		if(!cdb.WriteFloat(horizontalPS_PointOfZeroCurrent, "PointOfZeroCurrent"))
		{
			CStaticAssertErrorCondition(InitialisationError,"MainConfigurator::WriteConfigurationFile: %s horizontalPS_PointOfZeroCurrent",this->Name());
			return False;
		}
		else CStaticAssertErrorCondition(Information,"MainConfigurator::WriteConfigurationFile: horizontalPS_PointOfZeroCurrent = %f",horizontalPS_PointOfZeroCurrent);
		if(!cdb.WriteFloat(horizontalPS_CurrentStep, "CurrentStep"))
		{
			CStaticAssertErrorCondition(InitialisationError,"MainConfigurator::WriteConfigurationFile: %s horizontalPS_CurrentStep",this->Name());
			return False;
		}
		else CStaticAssertErrorCondition(Information,"MainConfigurator::WriteConfigurationFile: horizontalPS_CurrentStep = %f",horizontalPS_CurrentStep);
		if(!cdb.WriteInt32(marte_usec_cycle_time, "usecGlobalPeriod"))
		{
			AssertErrorCondition(InitialisationError,"MainConfigurator::WriteConfigurationFile: %s marte_usec_cycle_time",this->Name());
			return False;
		}
		else AssertErrorCondition(Information,"MainConfigurator::WriteConfigurationFile: marte_usec_cycle_time = %d",marte_usec_cycle_time);

	cdb->MoveToRoot();

// write config file section: PSCommunicator_primary
	if(!cdb->Move("+MARTe.+RealTimeThread.+PSCommunicator_primary"))
	{
		CStaticAssertErrorCondition(InitialisationError,"MainConfigurator::WriteConfigurationFile: %s Could not move to \"+MARTe.+RealTimeThread.+PSCommunicator_primary\"",this->Name());
		return False;
	}
		if(!cdb.WriteFString(primaryPS_UARTPortAddress, "UARTPortAddress"))
		{
			CStaticAssertErrorCondition(InitialisationError,"WaveformsDisplay::ObjectLoadSetup: %s Could not primaryPS_UARTPortAddress",this->Name());
			return False;
		}
		else CStaticAssertErrorCondition(Information,"MainConfigurator::WriteConfigurationFile: primaryPS_UARTPortAddress = %s",primaryPS_UARTPortAddress.Buffer());
		if(!cdb.WriteFloat(primaryPS_PointOfZeroCurrent, "PointOfZeroCurrent"))
		{
			CStaticAssertErrorCondition(InitialisationError,"MainConfigurator::WriteConfigurationFile: %s primaryPS_PointOfZeroCurrent",this->Name());
			return False;
		}
		else CStaticAssertErrorCondition(Information,"MainConfigurator::WriteConfigurationFile: primaryPS_PointOfZeroCurrent = %f",primaryPS_PointOfZeroCurrent);
		if(!cdb.WriteFloat(primaryPS_CurrentStep, "CurrentStep"))
		{
			CStaticAssertErrorCondition(InitialisationError,"MainConfigurator::WriteConfigurationFile: %s primaryPS_CurrentStep",this->Name());
			return False;
		}
		else CStaticAssertErrorCondition(Information,"MainConfigurator::WriteConfigurationFile: primaryPS_CurrentStep = %f",primaryPS_CurrentStep);
		if(!cdb.WriteInt32(marte_usec_cycle_time, "usecGlobalPeriod"))
		{
			AssertErrorCondition(InitialisationError,"MainConfigurator::WriteConfigurationFile: %s marte_usec_cycle_time",this->Name());
			return False;
		}
		else AssertErrorCondition(Information,"MainConfigurator::WriteConfigurationFile: marte_usec_cycle_time = %d",marte_usec_cycle_time);

	cdb->MoveToRoot();

// 	Move to the DischargeConfigurator

	if(!cdb->Move("+DischargeConfigurator"))
	{
		CStaticAssertErrorCondition(InitialisationError,"MainConfigurator::ObjectLoadSetup: %s Could not move to \"+DischargeConfigurator.graphics\"",this->Name());
		return False;
	}
		if(!cdb.WriteFString(Description, "Description"))
		{
			CStaticAssertErrorCondition(InitialisationError,"MainConfigurator::ObjectLoadSetup: %s Could not Write Description",this->Name());
			return False;
		}

	cdb->MoveToRoot();	

	if(!cdb->Move("+DischargeConfigurator.graphics"))
	{
		CStaticAssertErrorCondition(InitialisationError,"MainConfigurator::ObjectLoadSetup: %s Could not move to \"+DischargeConfigurator.graphics\"",this->Name());
		return False;
	}
	
		if(!cdb.WriteInt32(screen_resolution_x, "screen_resolution_x"))
		{
			CStaticAssertErrorCondition(InitialisationError,"MainConfigurator::ObjectLoadSetup: %s Could not Write graph_width",this->Name());
			return False;
		}
		else CStaticAssertErrorCondition(Information,"MainConfigurator::WriteConfigurationFile: screen_resolution_x = %d",screen_resolution_x);
		if(!cdb.WriteInt32(screen_resolution_y, "screen_resolution_y"))
		{
			CStaticAssertErrorCondition(InitialisationError,"MainConfigurator::ObjectLoadSetup: %s Could not Write graph_height",this->Name());
			return False;
		}
		else CStaticAssertErrorCondition(Information,"MainConfigurator::WriteConfigurationFile: screen_resolution_y = %d",screen_resolution_y);

	cdb->MoveToRoot();	

	if(!cdb->Move("+DischargeConfigurator.colours"))
	{
		CStaticAssertErrorCondition(InitialisationError,"MainConfigurator::ObjectLoadSetup: %s Could not move to \"+DischargeConfigurator.colours\"",this->Name());
		return False;
	}
		if(!cdb.WriteFString(header_colour, "header_colour"))
		{
			CStaticAssertErrorCondition(InitialisationError,"MainConfigurator::ObjectLoadSetup: %s Could not Write header_colour",this->Name());
			return False;
		}
		else CStaticAssertErrorCondition(Information,"MainConfigurator::WriteConfigurationFile: header_colour = %s",header_colour.Buffer());
		if(!cdb.WriteFString(diagnostics_parameters_colour, "diagnostics_parameters_colour"))
		{
			CStaticAssertErrorCondition(InitialisationError,"MainConfigurator::ObjectLoadSetup: %s Could not Write diagnostics_parameters_colour",this->Name());
			return False;
		}
		else CStaticAssertErrorCondition(Information,"MainConfigurator::WriteConfigurationFile: diagnostics_parameters_colour = %s",diagnostics_parameters_colour.Buffer());
		if(!cdb.WriteFString(control_parameters_colour, "control_parameters_colour"))
		{
			CStaticAssertErrorCondition(InitialisationError,"MainConfigurator::ObjectLoadSetup: %s Could not Write control_parameters_colour",this->Name());
			return False;
		}
		else CStaticAssertErrorCondition(Information,"MainConfigurator::WriteConfigurationFile: control_parameters_colour = %s",control_parameters_colour.Buffer());
		if(!cdb.WriteFString(time_windows_colour, "time_windows_colour"))
		{
			CStaticAssertErrorCondition(InitialisationError,"MainConfigurator::ObjectLoadSetup: %s Could not Write time_windows_colour",this->Name());
			return False;
		}
		else CStaticAssertErrorCondition(Information,"MainConfigurator::WriteConfigurationFile: time_windows_colour = %s",time_windows_colour.Buffer());
		if(!cdb.WriteFString(edit_colour, "edit_colour"))
		{
			CStaticAssertErrorCondition(InitialisationError,"MainConfigurator::ObjectLoadSetup: %s Could not Write edit_colour",this->Name());
			return False;
		}
		else CStaticAssertErrorCondition(Information,"MainConfigurator::WriteConfigurationFile: edit_colour = %s",edit_colour.Buffer());
		if(!cdb.WriteFString(footer_colour, "footer_colour"))
		{
			CStaticAssertErrorCondition(InitialisationError,"MainConfigurator::ObjectLoadSetup: %s Could not Write footer_colour",this->Name());
			return False;
		}
		else CStaticAssertErrorCondition(Information,"MainConfigurator::WriteConfigurationFile: footer_colour = %s",footer_colour.Buffer());

	cdb->MoveToRoot();	
	
	if(!cdb->Move("+DischargeConfigurator.default_directories"))
	{
		CStaticAssertErrorCondition(InitialisationError,"MainConfigurator::ObjectLoadSetup: %s Could not move to \"+DischargeConfigurator.default_directories\"",this->Name());
		return False;
	}
		if(!cdb.WriteFString(default_config_file_path, "default_config_file_path"))
		{
			CStaticAssertErrorCondition(InitialisationError,"MainConfigurator::ObjectLoadSetup: %s Could not Write default_config_file_path",this->Name());
			return False;
		}
		else CStaticAssertErrorCondition(Information,"MainConfigurator::WriteConfigurationFile: default_config_file_path = %s",default_config_file_path.Buffer());
		if(!cdb.WriteFString(config_files_directory, "config_files_directory"))
		{
			CStaticAssertErrorCondition(InitialisationError,"MainConfigurator::ObjectLoadSetup: %s Could not Write config_files_directory",this->Name());
			return False;
		}
		else CStaticAssertErrorCondition(Information,"MainConfigurator::WriteConfigurationFile: config_files_directory = %s",config_files_directory.Buffer());

	cdb->MoveToRoot();


    FileEraseFile((char *)TargetFilePath);
    File configOut;
    if(!configOut.OpenWrite((char *)TargetFilePath)){
       printf("MainConfigurator:: Failed open write file %s\n", TargetFilePath);
       return 0;
    }

//   FString cdbString;
    cdb->WriteToStream(cdbString);
    cdbString.Seek(0);
    //Write to file...
    cdb->WriteToStream(configOut);
    configOut.Close();
 
	return True;
}

bool MainConfigurator::DualVectorSort(int vector_dim, float * vector_x, float * vector_y){
	
	if(vector_dim <1){
		CStaticAssertErrorCondition(InitialisationError,"MainConfigurator::DualVectorSort, vector_dim lower than 1, vector_dim =%d",vector_dim);
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
bool MainConfigurator::RemoveRepeatedValues(int * vector_dim, float * vector_x, float * vector_y){
	
	if(*vector_dim <1){
		CStaticAssertErrorCondition(InitialisationError,"SVGGraphicSupport.h !!! RemoveRepeatedValues vector_dim lower than 1, vector_dim =%d",*vector_dim);
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

bool MainConfigurator::RemoveRepeatedValues(int option){
	
	int i;
	
	if(!MoveToTemp(option)) return False;
	
	if(vector_dim_temp <1){
		CStaticAssertErrorCondition(InitialisationError,"MainConfigurator::RemoveRepeatedValues, vector_dim lower than 1, vector_dim =%d",vector_dim_temp);
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
		CStaticAssertErrorCondition(InitialisationError,"MainConfigurator::RemoveRepeatedValues, insuficient points for a waveform, vector_dim_temp =%d",vector_dim_temp);
		return False;
	} 

	if(!RetrieveFromTemp(option)) return False;

	return True;
}

bool MainConfigurator::DisplayPoint(int option, int index_to_display){

	int i;

	if(!MoveToTemp(option)) return False;

	if (index_to_display < vector_dim_temp){
		box_display_point_x = temp_vector_x[index_to_display];
//		box_display_point_y = temp_vector_y[index_to_display];
	}

	focus_on_y_form_bool = True;

	return True;
}

bool MainConfigurator::RemovePoint(int option, int index_to_remove){
	
	int i;

	if(!MoveToTemp(option)) return False;
	
	if(vector_dim_temp <3){
		CStaticAssertErrorCondition(InitialisationError,"MainConfigurator::RemovePoint, vector_dim lower than 3, vector_dim =%d",vector_dim_temp);
		return False;
	} 
	if( vector_dim_temp < (index_to_remove+1) || index_to_remove < 0 ){
		CStaticAssertErrorCondition(InitialisationError,"MainConfigurator::RemovePoint, index_to_remove out of bounds, index_to_remove =%d",index_to_remove);
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
bool MainConfigurator::AddPoint(int option, float point_to_add_x, float point_to_add_y){

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

bool MainConfigurator::MoveToTemp(int option){
	
	int i;
	
	if (option ==0){		//primary_1_p
		vector_dim_temp = primary_1_p_vector_size;
		temp_vector_x = new float[primary_1_p_vector_size];
		temp_vector_y = new float[primary_1_p_vector_size];
		for (i=0;i< primary_1_p_vector_size; i++){
			temp_vector_x[i] = primary_1_p_index_vector[i];
			temp_vector_y[i] = primary_1_p_data_vector[i];
		}
	} 
	else if (option ==1){		//vertical_1_p
		vector_dim_temp = vertical_1_p_vector_size;
		temp_vector_x = new float[vertical_1_p_vector_size];
		temp_vector_y = new float[vertical_1_p_vector_size];
		for (i=0;i< vertical_1_p_vector_size; i++){
			temp_vector_x[i] = vertical_1_p_index_vector[i];
			temp_vector_y[i] = vertical_1_p_data_vector[i];
		}
	}	
	else if (option ==2){		//horizontal_1_p
		vector_dim_temp = horizontal_1_p_vector_size;
		temp_vector_x = new float[horizontal_1_p_vector_size];
		temp_vector_y = new float[horizontal_1_p_vector_size];
		for (i=0;i< horizontal_1_p_vector_size; i++){
			temp_vector_x[i] = horizontal_1_p_index_vector[i];
			temp_vector_y[i] = horizontal_1_p_data_vector[i];
		}
	}
	else if (option ==3){		//primary_2_p
		vector_dim_temp = primary_2_p_vector_size;
		temp_vector_x = new float[primary_2_p_vector_size];
		temp_vector_y = new float[primary_2_p_vector_size];
		for (i=0;i< primary_2_p_vector_size; i++){
			temp_vector_x[i] = primary_2_p_index_vector[i];
			temp_vector_y[i] = primary_2_p_data_vector[i];
		}
	}
	else if (option ==4){		//vertical_2_p
		vector_dim_temp = vertical_2_p_vector_size;
		temp_vector_x = new float[vertical_2_p_vector_size];
		temp_vector_y = new float[vertical_2_p_vector_size];
		for (i=0;i< vertical_2_p_vector_size; i++){
			temp_vector_x[i] = vertical_2_p_index_vector[i];
			temp_vector_y[i] = vertical_2_p_data_vector[i];
		}
	}
	else if (option ==5){		//horizontal_2_p
		vector_dim_temp = horizontal_2_p_vector_size;
		temp_vector_x = new float[horizontal_2_p_vector_size];
		temp_vector_y = new float[horizontal_2_p_vector_size];
		for (i=0;i< horizontal_2_p_vector_size; i++){
		temp_vector_x[i] = horizontal_2_p_index_vector[i];
			temp_vector_y[i] = horizontal_2_p_data_vector[i];
		}
	}		
	else if (option ==6){	//primary_1_n
		vector_dim_temp = primary_1_n_vector_size;
		temp_vector_x = new float[primary_1_n_vector_size];
		temp_vector_y = new float[primary_1_n_vector_size];
		for (i=0;i< primary_1_n_vector_size; i++){
			temp_vector_x[i] = primary_1_n_index_vector[i];
			temp_vector_y[i] = primary_1_n_data_vector[i];
		}
	}
	else if (option ==7){		//vertical_1_n
		vector_dim_temp = vertical_1_n_vector_size;
		temp_vector_x = new float[vertical_1_n_vector_size];
		temp_vector_y = new float[vertical_1_n_vector_size];
		for (i=0;i< vertical_1_n_vector_size; i++){
			temp_vector_x[i] = vertical_1_n_index_vector[i];
			temp_vector_y[i] = vertical_1_n_data_vector[i];
		}
	}
	else if (option ==8){		//horizontal_1_n
		vector_dim_temp = horizontal_1_n_vector_size;
		temp_vector_x = new float[horizontal_1_n_vector_size];
		temp_vector_y = new float[horizontal_1_n_vector_size];
		for (i=0;i< horizontal_1_n_vector_size; i++){
			temp_vector_x[i] = horizontal_1_n_index_vector[i];
			temp_vector_y[i] = horizontal_1_n_data_vector[i];
		}
	}
	else if (option ==9){		//primary_2_n
		vector_dim_temp = primary_2_n_vector_size;
		temp_vector_x = new float[primary_2_n_vector_size];
		temp_vector_y = new float[primary_2_n_vector_size];
		for (i=0;i< primary_2_n_vector_size; i++){
			temp_vector_x[i] = primary_2_n_index_vector[i];
			temp_vector_y[i] = primary_2_n_data_vector[i];
		}
	}
	else if (option ==10){	//vertical_2_n
		vector_dim_temp = vertical_2_n_vector_size;
		temp_vector_x = new float[vertical_2_n_vector_size];
		temp_vector_y = new float[vertical_2_n_vector_size];
		for (i=0;i< vertical_2_n_vector_size; i++){
			temp_vector_x[i] = vertical_2_n_index_vector[i];
			temp_vector_y[i] = vertical_2_n_data_vector[i];
		}
	}
	else if (option ==11){	//horizontal_2_n
		vector_dim_temp = horizontal_2_n_vector_size;
		temp_vector_x = new float[horizontal_2_n_vector_size];
		temp_vector_y = new float[horizontal_2_n_vector_size];
		for (i=0;i< horizontal_2_n_vector_size; i++){
			temp_vector_x[i] = horizontal_2_n_index_vector[i];
			temp_vector_y[i] = horizontal_2_n_data_vector[i];
		}
	}
	else {
		CStaticAssertErrorCondition(InitialisationError,"MainConfigurator::MoveToTemp, option out of bounds, option =%d",option);
		return False;
	}
	
	return True;
}
bool MainConfigurator::MoveToTempWithLimits(int option){
	
	int i;
	
	if (option ==0){		//primary_1_p
		vector_dim_temp = primary_1_p_vector_size;
		temp_max_value = primary_1_p_max_value;
		temp_min_value = primary_1_p_min_value;
		temp_vector_x = new float[primary_1_p_vector_size+1];
		temp_vector_y = new float[primary_1_p_vector_size+1];
		for (i=0;i< primary_1_p_vector_size; i++){
			temp_vector_x[i] = primary_1_p_index_vector[i];
			temp_vector_y[i] = primary_1_p_data_vector[i];
		}
	} 
	else if (option ==1){		//vertical_1_p
		vector_dim_temp = vertical_1_p_vector_size;
		temp_max_value = vertical_1_p_max_value;
		temp_min_value = vertical_1_p_min_value;
		temp_vector_x = new float[vertical_1_p_vector_size+1];
		temp_vector_y = new float[vertical_1_p_vector_size+1];
		for (i=0;i< vertical_1_p_vector_size; i++){
			temp_vector_x[i] = vertical_1_p_index_vector[i];
			temp_vector_y[i] = vertical_1_p_data_vector[i];
		}
	}	
	else if (option ==2){		//horizontal_1_p
		vector_dim_temp = horizontal_1_p_vector_size;
		temp_max_value = horizontal_1_p_max_value;
		temp_min_value = horizontal_1_p_min_value;
		temp_vector_x = new float[horizontal_1_p_vector_size+1];
		temp_vector_y = new float[horizontal_1_p_vector_size+1];
		for (i=0;i< horizontal_1_p_vector_size; i++){
			temp_vector_x[i] = horizontal_1_p_index_vector[i];
			temp_vector_y[i] = horizontal_1_p_data_vector[i];
		}
	}
	else if (option ==3){		//primary_2_p
		vector_dim_temp = primary_2_p_vector_size;
		temp_max_value = primary_2_p_max_value;
		temp_min_value = primary_2_p_min_value;
		temp_vector_x = new float[primary_2_p_vector_size+1];
		temp_vector_y = new float[primary_2_p_vector_size+1];
		for (i=0;i< primary_2_p_vector_size; i++){
			temp_vector_x[i] = primary_2_p_index_vector[i];
			temp_vector_y[i] = primary_2_p_data_vector[i];
		}
	}
	else if (option ==4){		//vertical_2_p
		vector_dim_temp = vertical_2_p_vector_size;
		temp_max_value = vertical_2_p_max_value;
		temp_min_value = vertical_2_p_min_value;
		temp_vector_x = new float[vertical_2_p_vector_size+1];
		temp_vector_y = new float[vertical_2_p_vector_size+1];
		for (i=0;i< vertical_2_p_vector_size; i++){
			temp_vector_x[i] = vertical_2_p_index_vector[i];
			temp_vector_y[i] = vertical_2_p_data_vector[i];
		}
	}
	else if (option ==5){		//horizontal_2_p
		vector_dim_temp = horizontal_2_p_vector_size;
		temp_max_value = horizontal_2_p_max_value;
		temp_min_value = horizontal_2_p_min_value;
		temp_vector_x = new float[horizontal_2_p_vector_size+1];
		temp_vector_y = new float[horizontal_2_p_vector_size+1];
		for (i=0;i< horizontal_2_p_vector_size; i++){
		temp_vector_x[i] = horizontal_2_p_index_vector[i];
			temp_vector_y[i] = horizontal_2_p_data_vector[i];
		}
	}		
	else if (option ==6){	//primary_1_n
		vector_dim_temp = primary_1_n_vector_size;
		temp_max_value = primary_1_p_max_value;
		temp_min_value = primary_1_p_min_value;
		temp_vector_x = new float[primary_1_n_vector_size+1];
		temp_vector_y = new float[primary_1_n_vector_size+1];
		for (i=0;i< primary_1_n_vector_size; i++){
			temp_vector_x[i] = primary_1_n_index_vector[i];
			temp_vector_y[i] = primary_1_n_data_vector[i];
		}
	}
	else if (option ==7){		//vertical_1_n
		vector_dim_temp = vertical_1_n_vector_size;
		temp_max_value = vertical_1_p_max_value;
		temp_min_value = vertical_1_p_min_value;
		temp_vector_x = new float[vertical_1_n_vector_size+1];
		temp_vector_y = new float[vertical_1_n_vector_size+1];
		for (i=0;i< vertical_1_n_vector_size; i++){
			temp_vector_x[i] = vertical_1_n_index_vector[i];
			temp_vector_y[i] = vertical_1_n_data_vector[i];
		}
	}
	else if (option ==8){		//horizontal_1_n
		vector_dim_temp = horizontal_1_n_vector_size;
		temp_max_value = horizontal_1_p_max_value;
		temp_min_value = horizontal_1_p_min_value;
		temp_vector_x = new float[horizontal_1_n_vector_size+1];
		temp_vector_y = new float[horizontal_1_n_vector_size+1];
		for (i=0;i< horizontal_1_n_vector_size; i++){
			temp_vector_x[i] = horizontal_1_n_index_vector[i];
			temp_vector_y[i] = horizontal_1_n_data_vector[i];
		}
	}
	else if (option ==9){		//primary_2_n
		vector_dim_temp = primary_2_n_vector_size;
		temp_max_value = primary_2_p_max_value;
		temp_min_value = primary_2_p_min_value;
		temp_vector_x = new float[primary_2_n_vector_size+1];
		temp_vector_y = new float[primary_2_n_vector_size+1];
		for (i=0;i< primary_2_n_vector_size; i++){
			temp_vector_x[i] = primary_2_n_index_vector[i];
			temp_vector_y[i] = primary_2_n_data_vector[i];
		}
	}
	else if (option ==10){	//vertical_2_n
		vector_dim_temp = vertical_2_n_vector_size;
		temp_max_value = vertical_2_p_max_value;
		temp_min_value = vertical_2_p_min_value;
		temp_vector_x = new float[vertical_2_n_vector_size+1];
		temp_vector_y = new float[vertical_2_n_vector_size+1];
		for (i=0;i< vertical_2_n_vector_size; i++){
			temp_vector_x[i] = vertical_2_n_index_vector[i];
			temp_vector_y[i] = vertical_2_n_data_vector[i];
		}
	}
	else if (option ==11){	//horizontal_2_n
		vector_dim_temp = horizontal_2_n_vector_size;
		temp_max_value = horizontal_2_p_max_value;
		temp_min_value = horizontal_2_p_min_value;
		temp_vector_x = new float[horizontal_2_n_vector_size+1];
		temp_vector_y = new float[horizontal_2_n_vector_size+1];
		for (i=0;i< horizontal_2_n_vector_size; i++){
			temp_vector_x[i] = horizontal_2_n_index_vector[i];
			temp_vector_y[i] = horizontal_2_n_data_vector[i];
		}
	}
	else {
		CStaticAssertErrorCondition(InitialisationError,"MainConfigurator::MoveToTemp, option out of bounds, option =%d",option);
		return False;
	}
	
	return True;
}
	
bool MainConfigurator::RetrieveFromTemp(int option){
	
	int i;
	
	if (option ==0){		//primary_1_p
		primary_1_p_vector_size = vector_dim_temp;
		primary_1_p_index_vector = new float[vector_dim_temp];
		primary_1_p_data_vector = new float[vector_dim_temp];
		for (i=0;i< vector_dim_temp; i++){
			primary_1_p_index_vector[i] = temp_vector_x[i];
			primary_1_p_data_vector[i] = temp_vector_y[i];
		}
	}	
	else if (option ==1){		//vertical_1_p
		vertical_1_p_vector_size = vector_dim_temp;
		vertical_1_p_index_vector = new float[vector_dim_temp];
		vertical_1_p_data_vector = new float[vector_dim_temp];
		for (i=0;i< vector_dim_temp; i++){
			vertical_1_p_index_vector[i] = temp_vector_x[i];
			vertical_1_p_data_vector[i] = temp_vector_y[i];
		}
	}
	else if (option ==2){		//horizontal_1_p
		horizontal_1_p_vector_size = vector_dim_temp;
		horizontal_1_p_index_vector = new float[vector_dim_temp];
		horizontal_1_p_data_vector = new float[vector_dim_temp];
		for (i=0;i< vector_dim_temp; i++){
			horizontal_1_p_index_vector[i] = temp_vector_x[i];
			horizontal_1_p_data_vector[i] = temp_vector_y[i];
		}
	}
	else if (option ==3){		//primary_2_p
		primary_2_p_vector_size = vector_dim_temp;
		primary_2_p_index_vector = new float[vector_dim_temp];
		primary_2_p_data_vector = new float[vector_dim_temp];
		for (i=0;i< vector_dim_temp; i++){
			primary_2_p_index_vector[i] = temp_vector_x[i];
			primary_2_p_data_vector[i] = temp_vector_y[i];
		}
	}
	else if (option ==4){		//vertical_2_p
		vertical_2_p_vector_size = vector_dim_temp;
		vertical_2_p_index_vector = new float[vector_dim_temp];
		vertical_2_p_data_vector = new float[vector_dim_temp];
		for (i=0;i< vector_dim_temp; i++){
			vertical_2_p_index_vector[i] = temp_vector_x[i];
			vertical_2_p_data_vector[i] = temp_vector_y[i];
		}
	}
	else if (option ==5){		//horizontal_2_p
		horizontal_2_p_vector_size = vector_dim_temp;
		horizontal_2_p_index_vector = new float[vector_dim_temp];
		horizontal_2_p_data_vector = new float[vector_dim_temp];
		for (i=0;i< vector_dim_temp; i++){
			horizontal_2_p_index_vector[i] = temp_vector_x[i];
			horizontal_2_p_data_vector[i] = temp_vector_y[i];
		}
	}		
	else if (option ==6){		//primary_1_n
		primary_1_n_vector_size = vector_dim_temp;
		primary_1_n_index_vector = new float[vector_dim_temp];
		primary_1_n_data_vector = new float[vector_dim_temp];
		for (i=0;i< vector_dim_temp; i++){
			primary_1_n_index_vector[i] = temp_vector_x[i];
			primary_1_n_data_vector[i] = temp_vector_y[i];
		}
	}
	else if (option ==7){		//vertical_1_n
		vertical_1_n_vector_size = vector_dim_temp;
		vertical_1_n_index_vector = new float[vector_dim_temp];
		vertical_1_n_data_vector = new float[vector_dim_temp];
		for (i=0;i< vector_dim_temp; i++){
			vertical_1_n_index_vector[i] = temp_vector_x[i];
			vertical_1_n_data_vector[i] = temp_vector_y[i];
		}
	}
	else if (option ==8){		//horizontal_1_n
		horizontal_1_n_vector_size = vector_dim_temp;
		horizontal_1_n_index_vector = new float[vector_dim_temp];
		horizontal_1_n_data_vector = new float[vector_dim_temp];
		for (i=0;i< vector_dim_temp; i++){
			horizontal_1_n_index_vector[i] = temp_vector_x[i];
			horizontal_1_n_data_vector[i] = temp_vector_y[i];
		}
	}
	else if (option ==9){		//primary_2_n
		primary_2_n_vector_size = vector_dim_temp;
		primary_2_n_index_vector = new float[vector_dim_temp];
		primary_2_n_data_vector = new float[vector_dim_temp];
		for (i=0;i< vector_dim_temp; i++){
			primary_2_n_index_vector[i] = temp_vector_x[i];
			primary_2_n_data_vector[i] = temp_vector_y[i];
		}
	}
	else if (option ==10){	//vertical_2_n
		vertical_2_n_vector_size = vector_dim_temp;
		vertical_2_n_index_vector = new float[vector_dim_temp];
		vertical_2_n_data_vector = new float[vector_dim_temp];
		for (i=0;i< vector_dim_temp; i++){
			vertical_2_n_index_vector[i] = temp_vector_x[i];
			vertical_2_n_data_vector[i] = temp_vector_y[i];
		}
	}
	else if (option ==11){	//horizontal_2_n
		horizontal_2_n_vector_size = vector_dim_temp;
		horizontal_2_n_index_vector = new float[vector_dim_temp];
		horizontal_2_n_data_vector = new float[vector_dim_temp];
		for (i=0;i< vector_dim_temp; i++){
			horizontal_2_n_index_vector[i] = temp_vector_x[i];
			horizontal_2_n_data_vector[i] = temp_vector_y[i];
		}
	}		
	else {
		CStaticAssertErrorCondition(InitialisationError,"MainConfigurator::RemovePoint, option out of bounds, option =%d",option);
		return False;
	}	
	
	return True;
}

bool MainConfigurator::SortTimeWindows(){
	
	bool temp_timewindows_bool;
	int temp_timewindows_dropdown;
	float temp_timewindows_time;

	int i,j;
	int count;
	int alfa;
	int pos;
	float min;

	count = 0;
	for (i=0;i<7;i++) if(timewindows_bool_vector[i]) count++;
	if (count == 0 ){
		positive_time_windows_values = new float[1];
		positive_primary_mode = new int[1];
		positive_vertical_mode = new int[1];
		positive_horizontal_mode = new int[1];
		
		timewindows_time[0] = 0.;
		positive_primary_mode[0] = 0;
		positive_vertical_mode[0] = 0;
		positive_horizontal_mode[0] = 0;
		positive_number_of_time_windows = 1;
		
		for (j=0;j<21;j++) timewindows_dropdown_vector[j] = 0;
		timewindows_bool_vector[0] = True;
	}
	if (count == 1){
		for (i=0;i<7;i++){
			if(timewindows_bool_vector[i]) {
				timewindows_dropdown_vector[0] = timewindows_dropdown_vector[(i*3)];
				timewindows_dropdown_vector[1] = timewindows_dropdown_vector[(i*3+1)];
				timewindows_dropdown_vector[2] = timewindows_dropdown_vector[(i*3+2)];
				timewindows_time[0] = timewindows_time[i];
				timewindows_bool_vector[0] = timewindows_bool_vector[i];
			}
		}
		positive_number_of_time_windows = 1;
		positive_time_windows_values = new float[1];
		positive_primary_mode = new int[1];
		positive_vertical_mode = new int[1];
		positive_horizontal_mode = new int[1];

		if (timewindows_dropdown_vector[0] >= 6){
			timewindows_dropdown_vector[1] = timewindows_dropdown_vector[0];
			timewindows_dropdown_vector[2] = timewindows_dropdown_vector[0];
		}
		if (timewindows_dropdown_vector[1] >= 6){
			timewindows_dropdown_vector[0] = timewindows_dropdown_vector[1];
			timewindows_dropdown_vector[2] = timewindows_dropdown_vector[1];
		}
		if (timewindows_dropdown_vector[2] >= 6){
			timewindows_dropdown_vector[0] = timewindows_dropdown_vector[2];
			timewindows_dropdown_vector[1] = timewindows_dropdown_vector[2];
		}

		positive_time_windows_values[0] = timewindows_time[0];
		positive_primary_mode[0] = timewindows_dropdown_vector[0];
		positive_vertical_mode[0] = timewindows_dropdown_vector[1];
		positive_horizontal_mode[0] = timewindows_dropdown_vector[2];
		for (i=1;i<7;i++){
			timewindows_dropdown_vector[(i*3)] = 0;
			timewindows_dropdown_vector[(i*3 + 1)] = 0;
			timewindows_dropdown_vector[(i*3 + 2)] = 0;
			timewindows_time[i] = 0;
			timewindows_bool_vector[i] = False;
		}
		if (timewindows_time[0] > 0. ){
				
			timewindows_time[1] = timewindows_time[0];
			timewindows_bool_vector[1] = timewindows_bool_vector[0];
			timewindows_dropdown_vector[3] = timewindows_dropdown_vector[0];
			timewindows_dropdown_vector[4] = timewindows_dropdown_vector[1];
			timewindows_dropdown_vector[5] = timewindows_dropdown_vector[2];

//			timewindows_bool_vector[0] = True;
			timewindows_time[0] = 0.;
			timewindows_dropdown_vector[0] = 0;
			timewindows_dropdown_vector[1] = 0;
			timewindows_dropdown_vector[2] = 0;
			
			positive_number_of_time_windows = 2;
			positive_time_windows_values = new float[2];
			positive_primary_mode = new int[2];
			positive_vertical_mode = new int[2];
			positive_horizontal_mode = new int[2];
			
			positive_time_windows_values[0] = 0.;
			positive_primary_mode[0] = 0;
			positive_vertical_mode[0] = 0;
			positive_horizontal_mode[0] = 0;
			positive_time_windows_values[1] = timewindows_time[1];
			positive_primary_mode[1] = timewindows_dropdown_vector[3];
			positive_vertical_mode[1] = timewindows_dropdown_vector[4];
			positive_horizontal_mode[1] = timewindows_dropdown_vector[5];
		}
	}
	if (count >1){
		j=0;
		for (i=0;i<7;i++){
			if(timewindows_bool_vector[i]) {
				timewindows_dropdown_vector[(j*3)] = timewindows_dropdown_vector[(i*3)];
				timewindows_dropdown_vector[(j*3+1)] = timewindows_dropdown_vector[(i*3+1)];
				timewindows_dropdown_vector[(j*3+2)] = timewindows_dropdown_vector[(i*3+2)];
				timewindows_time[j] = timewindows_time[i];
				timewindows_bool_vector[j] = timewindows_bool_vector[i];
				j++;
			}
		}
		for (i=0;i<j;i++){
				
			if (timewindows_dropdown_vector[(i*3)] >= 6){
				timewindows_dropdown_vector[(i*3+1)] = timewindows_dropdown_vector[(i*3)];
				timewindows_dropdown_vector[(i*3+2)] = timewindows_dropdown_vector[(i*3)];
			}
			if (timewindows_dropdown_vector[(i*3+1)] >= 6){
				timewindows_dropdown_vector[(i*3)] = timewindows_dropdown_vector[(i*3+1)];
				timewindows_dropdown_vector[(i*3+2)] = timewindows_dropdown_vector[(i*3+1)];
			}
			if (timewindows_dropdown_vector[(i*3+2)] >= 6){
				timewindows_dropdown_vector[(i*3)] = timewindows_dropdown_vector[(i*3+2)];
				timewindows_dropdown_vector[(i*3+1)] = timewindows_dropdown_vector[(i*3+2)];
			}
		}
		//		sorting
		for(alfa=0;alfa<j-1;alfa++){	
			pos = alfa;
			min = timewindows_time[alfa];
			
			for (i=alfa;i < j;i++){
				if (timewindows_time[i] <= min){
					pos = i;
					min = timewindows_time[i];
				}	
			}	

			temp_timewindows_time = timewindows_time[alfa];
			timewindows_time[alfa] = timewindows_time[pos];
			timewindows_time[pos] = temp_timewindows_time;
			
			temp_timewindows_bool = timewindows_bool_vector[alfa];
			timewindows_bool_vector[alfa] = timewindows_bool_vector[pos];
			timewindows_bool_vector[pos] = temp_timewindows_bool;
			
			temp_timewindows_dropdown = timewindows_dropdown_vector[(alfa*3)];
			timewindows_dropdown_vector[(alfa*3)] = timewindows_dropdown_vector[(pos*3)];
			timewindows_dropdown_vector[(pos*3)] = temp_timewindows_dropdown;
			
			temp_timewindows_dropdown = timewindows_dropdown_vector[(alfa*3+1)];
			timewindows_dropdown_vector[(alfa*3+1)] = timewindows_dropdown_vector[(pos*3+1)];
			timewindows_dropdown_vector[(pos*3+1)] = temp_timewindows_dropdown;
			
			temp_timewindows_dropdown = timewindows_dropdown_vector[(alfa*3+2)];
			timewindows_dropdown_vector[(alfa*3+2)] = timewindows_dropdown_vector[(pos*3+2)];
			timewindows_dropdown_vector[(pos*3+2)] = temp_timewindows_dropdown;
		}
		for (alfa = 0; alfa < j-1; alfa++){

			if ( timewindows_time[alfa] == timewindows_time[alfa+1]){
				
				for (i=alfa; i < j-2; i++){
					timewindows_time[i+1] = timewindows_time[i+2];
					timewindows_bool_vector[i+1] = timewindows_bool_vector[i+2];
					timewindows_dropdown_vector[((i+1)*3)] = timewindows_dropdown_vector[((i+2)*3)];
					timewindows_dropdown_vector[((i+1)*3+1)] = timewindows_dropdown_vector[((i+2)*3+1)];
					timewindows_dropdown_vector[((i+1)*3+2)] = timewindows_dropdown_vector[((i+2)*3+2)];
				}
				alfa--;
				j -=1;
			}
		}
		if (j>=1){
			for (i=j-1;i>=0;i--){
				if (timewindows_dropdown_vector[((i)*3)] == 7) j = i+1;
			}
		}
		// clear the rest
		for (i=j;i<7;i++){
			timewindows_dropdown_vector[(i*3)] = 0;
			timewindows_dropdown_vector[(i*3 + 1)] = 0;
			timewindows_dropdown_vector[(i*3 + 2)] = 0;
			timewindows_time[i] = 0;
			timewindows_bool_vector[i] = False;
		}
		if (timewindows_time[0] >0 ){
			for (i=j-1; i>=0;i--){
				if (i<6){	
					timewindows_time[i+1] = timewindows_time[i];
					timewindows_bool_vector[i+1] = timewindows_bool_vector[i];
					timewindows_dropdown_vector[((i+1)*3)] = timewindows_dropdown_vector[((i)*3)];
					timewindows_dropdown_vector[((i+1)*3+1)] = timewindows_dropdown_vector[((i)*3+1)];
					timewindows_dropdown_vector[((i+1)*3+2)] = timewindows_dropdown_vector[((i)*3+2)];
				}
			}
			timewindows_bool_vector[0] = True;
			timewindows_time[0] = 0;
			timewindows_dropdown_vector[0] = 0;
			timewindows_dropdown_vector[1] = 0;
			timewindows_dropdown_vector[2] = 0;
			j++;
		}

		//	move to writable constants
		positive_number_of_time_windows = j;
		positive_time_windows_values = new float[j];
		positive_primary_mode = new int[j];
		positive_vertical_mode = new int[j];
		positive_horizontal_mode = new int[j];

		for (i=0;i<j;i++){
			positive_time_windows_values[i] = timewindows_time[i];
			positive_primary_mode[i] = timewindows_dropdown_vector[(i*3)];
			positive_vertical_mode[i] = timewindows_dropdown_vector[(i*3+1)];
			positive_horizontal_mode[i] = timewindows_dropdown_vector[(i*3+2)];
		}
	}
	
// negative time windows sorting	
	
	count = 0;
	for (i=7;i<14;i++) if(timewindows_bool_vector[i]) count++;
	if (count == 0 ){
		negative_time_windows_values = new float[1];
		negative_primary_mode = new int[1];
		negative_vertical_mode = new int[1];
		negative_horizontal_mode = new int[1];
		
		timewindows_time[7] = 0;
		negative_primary_mode[0] = 0;
		negative_vertical_mode[0] = 0;
		negative_horizontal_mode[0] = 0;
		negative_number_of_time_windows = 1;
		
		for (j=21;j<42;j++) timewindows_dropdown_vector[j] = 0;
		timewindows_bool_vector[7] = True;
	}
	if (count == 1){
		for (i=7;i<14;i++){
			if(timewindows_bool_vector[i]) {
				timewindows_dropdown_vector[21] = timewindows_dropdown_vector[(i*3)];
				timewindows_dropdown_vector[22] = timewindows_dropdown_vector[(i*3+1)];
				timewindows_dropdown_vector[23] = timewindows_dropdown_vector[(i*3+2)];
				timewindows_time[7] = timewindows_time[i];
				timewindows_bool_vector[7] = timewindows_bool_vector[i];
			}
		}
		positive_number_of_time_windows = 1;
		positive_time_windows_values = new float[1];
		positive_primary_mode = new int[1];
		positive_vertical_mode = new int[1];
		positive_horizontal_mode = new int[1];

		if (timewindows_dropdown_vector[21] >= 6){
			timewindows_dropdown_vector[22] = timewindows_dropdown_vector[21];
			timewindows_dropdown_vector[23] = timewindows_dropdown_vector[21];
		}
		if (timewindows_dropdown_vector[22] >= 6){
			timewindows_dropdown_vector[21] = timewindows_dropdown_vector[22];
			timewindows_dropdown_vector[23] = timewindows_dropdown_vector[22];
		}
		if (timewindows_dropdown_vector[23] >= 6){
			timewindows_dropdown_vector[21] = timewindows_dropdown_vector[23];
			timewindows_dropdown_vector[22] = timewindows_dropdown_vector[23];
		}
		negative_time_windows_values[0] = timewindows_time[7];
		negative_primary_mode[0] = timewindows_dropdown_vector[21];
		negative_vertical_mode[0] = timewindows_dropdown_vector[22];
		negative_horizontal_mode[0] = timewindows_dropdown_vector[23];
		for (i=8;i<14;i++){
			timewindows_dropdown_vector[(i*3)] = 0;
			timewindows_dropdown_vector[(i*3 + 1)] = 0;
			timewindows_dropdown_vector[(i*3 + 2)] = 0;
			timewindows_time[i] = 0;
			timewindows_bool_vector[i] = False;
		}
		if (timewindows_time[7] > 0 ){
				
			timewindows_time[8] = timewindows_time[7];
			timewindows_bool_vector[8] = timewindows_bool_vector[7];
			timewindows_dropdown_vector[24] = timewindows_dropdown_vector[21];
			timewindows_dropdown_vector[25] = timewindows_dropdown_vector[22];
			timewindows_dropdown_vector[26] = timewindows_dropdown_vector[23];

			timewindows_bool_vector[7] = True;
			timewindows_time[7] = 0;
			timewindows_dropdown_vector[21] = 0;
			timewindows_dropdown_vector[22] = 0;
			timewindows_dropdown_vector[23] = 0;

			negative_number_of_time_windows = 2;
			negative_time_windows_values = new float[2];
			negative_primary_mode = new int[2];
			negative_vertical_mode = new int[2];
			negative_horizontal_mode = new int[2];
			
			negative_time_windows_values[0] = 0;
			negative_primary_mode[0] = 0;
			negative_vertical_mode[0] = 0;
			negative_horizontal_mode[0] = 0;
			negative_time_windows_values[1] = timewindows_time[8];
			negative_primary_mode[1] = timewindows_dropdown_vector[24];
			negative_vertical_mode[1] = timewindows_dropdown_vector[25];
			negative_horizontal_mode[1] = timewindows_dropdown_vector[26];
		}
	}
	if (count > 1){
		j=0;
		for (i=7;i<14;i++){
			if(timewindows_bool_vector[i]) {
				timewindows_dropdown_vector[((j+7)*3)] = timewindows_dropdown_vector[(i*3)];
				timewindows_dropdown_vector[((j+7)*3+1)] = timewindows_dropdown_vector[(i*3+1)];
				timewindows_dropdown_vector[((j+7)*3+2)] = timewindows_dropdown_vector[(i*3+2)];
				timewindows_time[(j+7)] = timewindows_time[i];
				timewindows_bool_vector[(j+7)] = timewindows_bool_vector[i];
				j++;
			}
		}
		for (i=0;i<j;i++){
				
			if (timewindows_dropdown_vector[((i+7)*3)] >= 6){
				timewindows_dropdown_vector[((i+7)*3+1)] = timewindows_dropdown_vector[((i+7)*3)];
				timewindows_dropdown_vector[((i+7)*3+2)] = timewindows_dropdown_vector[((i+7)*3)];
			}
			if (timewindows_dropdown_vector[((i+7)*3+1)] >= 6){
				timewindows_dropdown_vector[((i+7)*3)] = timewindows_dropdown_vector[((i+7)*3+1)];
				timewindows_dropdown_vector[((i+7)*3+2)] = timewindows_dropdown_vector[((i+7)*3+1)];
			}
			if (timewindows_dropdown_vector[((i+7)*3+2)] >= 6){
				timewindows_dropdown_vector[((i+7)*3)] = timewindows_dropdown_vector[((i+7)*3+2)];
				timewindows_dropdown_vector[((i+7)*3+1)] = timewindows_dropdown_vector[((i+7)*3+2)];
			}
		}
		//		sorting
		for(alfa=0;alfa<j-1;alfa++){	
			pos = alfa;
			min = timewindows_time[(alfa+7)];
			
			for (i=alfa;i < j;i++){
				if (timewindows_time[(i+7)] <= min){
					pos = i;
					min = timewindows_time[(i+7)];
				}	
			}	

			temp_timewindows_time = timewindows_time[(alfa+7)];
			timewindows_time[(alfa+7)] = timewindows_time[(pos+7)];
			timewindows_time[(pos+7)] = temp_timewindows_time;
			
			temp_timewindows_bool = timewindows_bool_vector[(alfa+7)];
			timewindows_bool_vector[(alfa+7)] = timewindows_bool_vector[(pos+7)];
			timewindows_bool_vector[(pos+7)] = temp_timewindows_bool;
			
			temp_timewindows_dropdown = timewindows_dropdown_vector[((alfa+7)*3)];
			timewindows_dropdown_vector[((alfa+7)*3)] = timewindows_dropdown_vector[((pos+7)*3)];
			timewindows_dropdown_vector[((pos+7)*3)] = temp_timewindows_dropdown;
			
			temp_timewindows_dropdown = timewindows_dropdown_vector[((alfa+7)*3+1)];
			timewindows_dropdown_vector[((alfa+7)*3+1)] = timewindows_dropdown_vector[((pos+7)*3+1)];
			timewindows_dropdown_vector[((pos+7)*3+1)] = temp_timewindows_dropdown;
			
			temp_timewindows_dropdown = timewindows_dropdown_vector[((alfa+7)*3+2)];
			timewindows_dropdown_vector[((alfa+7)*3+2)] = timewindows_dropdown_vector[((pos+7)*3+2)];
			timewindows_dropdown_vector[((pos+7)*3+2)] = temp_timewindows_dropdown;
		}

		for (alfa = 0; alfa < j-1; alfa++){

			if ( timewindows_time[(alfa+7)] == timewindows_time[((alfa+7)+1)]){
				
				for (i=alfa; i < j-2; i++){
					timewindows_time[(i+8)] = timewindows_time[(i+9)];
					timewindows_bool_vector[(i+8)] = timewindows_bool_vector[(i+9)];
					timewindows_dropdown_vector[((i+8)*3)] = timewindows_dropdown_vector[((i+9)*3)];
					timewindows_dropdown_vector[((i+8)*3+1)] = timewindows_dropdown_vector[((i+9)*3+1)];
					timewindows_dropdown_vector[((i+8)*3+2)] = timewindows_dropdown_vector[((i+9)*3+2)];
				}
				alfa--;
				j -=1;
			}
		}
		if (j>=1){
			for (i=j-1;i>=0;i--){
				if (timewindows_dropdown_vector[((i+7)*3)] == 7) j = i+1;
			}
		}
		// clear the rest
		for (i=j;i<7;i++){
			timewindows_dropdown_vector[((i+7)*3)] = 0;
			timewindows_dropdown_vector[((i+7)*3 + 1)] = 0;
			timewindows_dropdown_vector[((i+7)*3 + 2)] = 0;
			timewindows_time[(i+7)] = 0;
			timewindows_bool_vector[(i+7)] = False;
		}
		if (timewindows_time[7] >0 ){
			for (i=j-1; i>=0;i--){
				if (i<6){	
					timewindows_time[i+8] = timewindows_time[i+7];
					timewindows_bool_vector[i+8] = timewindows_bool_vector[i+7];
					timewindows_dropdown_vector[((i+8)*3)] = timewindows_dropdown_vector[((i+7)*3)];
					timewindows_dropdown_vector[((i+8)*3+1)] = timewindows_dropdown_vector[((i+7)*3+1)];
					timewindows_dropdown_vector[((i+8)*3+2)] = timewindows_dropdown_vector[((i+7)*3+2)];
				}
			}
			timewindows_bool_vector[7] = True;
			timewindows_time[7] = 0;
			timewindows_dropdown_vector[21] = 0;
			timewindows_dropdown_vector[22] = 0;
			timewindows_dropdown_vector[23] = 0;
			j++;
		}
		//	move to writable constants
		negative_number_of_time_windows = j;
		negative_time_windows_values = new float[j];
		negative_primary_mode = new int[j];
		negative_vertical_mode = new int[j];
		negative_horizontal_mode = new int[j];

		for (i=0;i<j;i++){
			negative_time_windows_values[i] = timewindows_time[(i+7)];
			negative_primary_mode[i] = timewindows_dropdown_vector[((i+7)*3)];
			negative_vertical_mode[i] = timewindows_dropdown_vector[((i+7)*3+1)];
			negative_horizontal_mode[i] = timewindows_dropdown_vector[((i+7)*3+2)];
		}
	}	
	
	return True;
}

