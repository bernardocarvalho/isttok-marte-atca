/**
 * @file allows to configure ISTTOK and upload configuration file to MARTe
 */
#ifndef CFG_UPLOADER_H
#define CFG_UPLOADER_H

#include "Level0.h"
#include "Level1.h"
#include "Level2.h"
#include "Level3.h"

#include "System.h"
#include "HttpStream.h"
#include "HttpInterface.h"
#include "HtmlStream.h"
#include "FString.h"
#include "GCReferenceContainer.h"
#include "CDBExtended.h"
#include "GlobalObjectDataBase.h"
#include "MessageHandler.h"

#include "SVGGraphicSupport.h"

#include <dirent.h> //to load files located on the atca
#include <string.h> // to compare strings


OBJECT_DLL(AdvancedConfigurator)
class AdvancedConfigurator: public GCReferenceContainer, public HttpInterface, public MessageHandler{
OBJECT_DLL_STUFF(AdvancedConfigurator)

private:
/** The id of the configuration file entry as received from the http request*/
    FString configFileID;

/** The location of MARTe*/
    FString marteLocation;
    FString cdbString; //string to be uploaded
	
	int graphic_select;
	FString save_filename;
	bool save_as_bool;
	bool delete_selected_points_bool;
	
	int vector_dim_temp;
	float *temp_vector_x;
	float *temp_vector_y;
	float temp_max_value;
	float temp_min_value;

	bool already_started;

	bool focus_on_t_form_bool;
	bool focus_on_y_form_bool;

	float box_display_point_x;
	
	bool edit_main;
	bool edit_control;
	bool edit_general;
	bool end_discharge_after_unsuccess_bool;

	FString Description;

	FString header_colour;
	FString colour1;
	FString colour2;
	FString colour3;
	FString colour4;
	FString colour5;
	FString footer_colour;

	FString selected_load_directory;
	FString selected_load_file;

	FString selected_save_directory;
	FString selected_save_file;
	
	FString dummy_fstring;	
	
	FString default_config_file_path;
	FString config_files_directory;

//MARTe configuration
	int thread_priority;
	int run_on_cpu;
	
//Main 
	int usec_pre_pulse_time;
	int maximum_inversion_usectime;
	FString tomography_file_to_load;
	int tomography_n_channels;
	int *tomography_online_channels;
	float iron_core_saturation_value;
	float iron_core_dangerous_value;
	int time_between_online_and_discharge;


	float puffing_duration_in_puffing_feedback_in_ms;
	float maximum_idle_time_in_puffing_feedback_in_ms;
	float minimum_idle_time_in_puffing_feedback_in_ms;
	float puffing_feedback_usec_change_percentage_by_cycle;
	int puffing_feedback_mode;



//	+control settings
//		PID_horizontal
	float horizontalPS_P;
	float horizontalPS_I;
	float horizontalPS_D;
//		PID_vertical =
	float verticalPS_P;
	float verticalPS_I;
	float verticalPS_D;
//		PID_primary 
	float primaryPS_P;
	float primaryPS_I;
	float primaryPS_D;
	FString control_file_to_load;
	int puffing_mode; //0-> 0ff 1->time-windows, percentage output, 2->time windows with feedback, 3-> absolute time, out percentage, 4-> open loop up to time windows (use absolute timing waveform until the end of breakdown), then feedback
	
	
//	+waveform_primary
//	waveform_mode_1_positive 
	int primary_breakdown_vector_size;
	float *primary_breakdown_index_vector;
	float *primary_breakdown_data_vector;
//	waveform_mode_1_negative  
	int primary_breakdown_negative_vector_size;
	float *primary_breakdown_negative_index_vector;
	float *primary_breakdown_negative_data_vector;
//	waveform_mode_2_positive 
	int primary_inversion_positive_vector_size;
	float *primary_inversion_positive_index_vector;
	float *primary_inversion_positive_data_vector;
//	waveform_mode_2_negative 
	int primary_inversion_negative_vector_size;
	float *primary_inversion_negative_index_vector;
	float *primary_inversion_negative_data_vector;

//	+waveform_vertical
//		waveform_mode_1_positive 
	int vertical_breakdown_vector_size;
	float *vertical_breakdown_index_vector;
	float *vertical_breakdown_data_vector;
//		waveform_mode_1_negative  
	int vertical_breakdown_negative_vector_size;
	float *vertical_breakdown_negative_index_vector;
	float *vertical_breakdown_negative_data_vector;
//		waveform_mode_2_positive 
	int vertical_inversion_positive_vector_size;
	float *vertical_inversion_positive_index_vector;
	float *vertical_inversion_positive_data_vector;
//		waveform_mode_2_negative 
	int vertical_inversion_negative_vector_size;
	float *vertical_inversion_negative_index_vector;
	float *vertical_inversion_negative_data_vector;

//	+waveform_horizontal
//		waveform_mode_1_positive 
	int horizontal_breakdown_vector_size;
	float *horizontal_breakdown_index_vector;
	float *horizontal_breakdown_data_vector;
//		waveform_mode_1_negative  
	int horizontal_breakdown_negative_vector_size;
	float *horizontal_breakdown_negative_index_vector;
	float *horizontal_breakdown_negative_data_vector;
//		waveform_mode_2_positive 
	int horizontal_inversion_positive_vector_size;
	float *horizontal_inversion_positive_index_vector;
	float *horizontal_inversion_positive_data_vector;
//		waveform_mode_2_negative 
	int horizontal_inversion_negative_vector_size;
	float *horizontal_inversion_negative_index_vector;
	float *horizontal_inversion_negative_data_vector;

//	+waveform_toroidal
//		waveform_mode_1_positive 
	int toroidal_1_p_vector_size;
	float *toroidal_1_p_index_vector;
	float *toroidal_1_p_data_vector;

//	+waveform_puffing
//		waveform_mode_1_positive 
	int puffing_1_p_vector_size;
	float *puffing_1_p_index_vector;
	float *puffing_1_p_data_vector;
//		waveform_mode_1_negative  
	int puffing_1_n_vector_size;
	float *puffing_1_n_index_vector;
	float *puffing_1_n_data_vector;
//		waveform_mode_2_positive 
	int puffing_2_p_vector_size;
	float *puffing_2_p_index_vector;
	float *puffing_2_p_data_vector;
//		waveform_mode_2_negative 
	int puffing_2_n_vector_size;
	float *puffing_2_n_index_vector;
	float *puffing_2_n_data_vector;
//		waveform_mode_2_negative 
	int puffing_absolute_time_vector_size;
	float *puffing_absolute_time_index_vector;
	float *puffing_absolute_time_data_vector;

//	maximum and minimum waveform values
	float primary_1_p_max_value;
	float primary_1_p_min_value;
	float primary_2_p_max_value;
	float primary_2_p_min_value;
	float vertical_1_p_max_value;
	float vertical_1_p_min_value;
	float vertical_2_p_max_value;
	float vertical_2_p_min_value;
	float horizontal_1_p_max_value;
	float horizontal_1_p_min_value;
	float horizontal_2_p_max_value;
	float horizontal_2_p_min_value;
	float toroidal_1_p_max_value;
	float toroidal_1_p_min_value;
	float puffing_1_p_max_value;
	float puffing_1_p_min_value;
	float puffing_2_p_max_value;
	float puffing_2_p_min_value;

//	+FACommunicator_vertical
	FString	verticalPS_UARTPortAddress;
	float verticalPS_PointOfZeroCurrent;
	float verticalPS_CurrentStep;

//	+FACommunicator_horizontal
	FString	horizontalPS_UARTPortAddress;
	float horizontalPS_PointOfZeroCurrent;
	float horizontalPS_CurrentStep;

//	+FACommunicator_primary
	FString	primaryPS_UARTPortAddress;
	float primaryPS_PointOfZeroCurrent;
	float primaryPS_CurrentStep;




	SVGGraphicSupport *graphics_support;


public:

    /** the main entry point for HttpInterface */
    virtual     bool    ProcessHttpMessage(HttpStream &hStream);

    /** the default constructor */
    AdvancedConfigurator(){
    }

    ~AdvancedConfigurator(){
    }

	virtual bool ObjectLoadSetup(ConfigurationDataBase & info, StreamInterface * err);

    /** save an object content into a set of configs */
    virtual bool ObjectSaveSetup( ConfigurationDataBase & info, StreamInterface * err){

        GCReferenceContainer::ObjectSaveSetup(info,err);
        return HttpInterface::ObjectSaveSetup(info,err);
    }

   
private:
    /**Utility method to print the form*/
	bool Initialise();
 
	bool PrintHTTPForm(HtmlStream &hmStream);	// prints the html steam
	
	bool WriteConfigurationFileWithChanges(char BaseFileFilePath[], char TargetFilePath[]);	// writes a config file from the stored variables
	
	bool ReadConfigurationFile(char FilePath[]);	// reads a configuration file and saves the data to the correspondent variables
	
	bool DualVectorSort(int vector_dim, float * vector_x, float * vector_y);	// sorts 2 vectors based on the vector_x values
	
	virtual bool RemoveRepeatedValues(int * vector_dim, float * vector_x, float * vector_y);	// remove repeated values of generic vectors
	
	virtual bool RemoveRepeatedValues(int option);		// remove repeated values of a certain waveform (option dependant)
	
	bool MoveToTemp(int option);		//	used with remove point

	bool MoveToTempWithLimits(int option);	//	used with add point
	
	bool RetrieveFromTemp(int option);		//		used with remove/add point
	
	bool RemovePoint(int option, int index_to_remove);		// remove a point for a certain waveform
	
	bool DisplayPoint(int option, int index_to_display);
	
	bool AddPoint(int option, float point_to_add_x, float point_to_add_y);	//	add a point to a certain waveform
		
};
#endif
