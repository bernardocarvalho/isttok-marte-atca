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
//#include "WaveformFiles.h"
//#include "VectorSupport.h"




#include <dirent.h> //to load files located on the atca
#include <string.h> // to compare strings



OBJECT_DLL(MainConfigurator)
class MainConfigurator: public GCReferenceContainer, public HttpInterface, public MessageHandler{
OBJECT_DLL_STUFF(MainConfigurator)

private:
/** The id of the configuration file entry as received from the http request*/
    FString configFileID;

/** The location of MARTe*/
    FString marteLocation;
    FString cdbString; //string to be uploaded
	
	float Ip_max_threshold_value;
	float max_value;

	bool *magnetic_probes_bool_vector;
	int *timewindows_dropdown_vector;
	float *timewindows_time;
	bool *timewindows_bool_vector;
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

	bool edit_weights;
	bool edit_control;
	bool edit_timewindows;
	
	bool focus_on_t_form_bool;
	bool focus_on_y_form_bool;
	
	float box_display_point_x;
	float box_display_point_y;
	FString temp_box_string;

	FString Description;
	int screen_resolution_x;
	int screen_resolution_y;

	FString header_colour;
	FString diagnostics_parameters_colour;
	FString control_parameters_colour;
	FString time_windows_colour;
	FString edit_colour;
	FString footer_colour;

	FString default_config_file_path;
	FString config_files_directory;

	FString selected_load_directory;
	FString selected_load_file;

	FString selected_save_directory;
	FString selected_save_file;
	
	FString dummy_fstring;	

//	MARTe advanced settings
	int marte_usec_cycle_time;
	int usectime_to_wait_for_starting_operation;
	int datacollection_n_of_samples;

//	+waveform_primary
//	waveform_mode_1_positive 
	int primary_1_p_vector_size;
	float *primary_1_p_index_vector;
	float *primary_1_p_data_vector;
	float primary_1_p_max_value;
	float primary_1_p_min_value;
//	waveform_mode_1_negative  
	int primary_1_n_vector_size;
	float *primary_1_n_index_vector;
	float *primary_1_n_data_vector;
//	waveform_mode_2_positive 
	int primary_2_p_vector_size;
	float *primary_2_p_index_vector;
	float *primary_2_p_data_vector;
	float primary_2_p_max_value;
	float primary_2_p_min_value;
//	waveform_mode_2_negative 
	int primary_2_n_vector_size;
	float *primary_2_n_index_vector;
	float *primary_2_n_data_vector;
//	waveform_mode_3_positive 
	int primary_breakdown_vector_size;
	float *primary_breakdown_index_vector;
	float *primary_breakdown_data_vector;
//	waveform_mode_3_negative 
	int primary_inversion_vector_size;
	float *primary_inversion_index_vector;
	float *primary_inversion_data_vector;

//	+waveform_vertical
//		waveform_mode_1_positive 
	int vertical_1_p_vector_size;
	float *vertical_1_p_index_vector;
	float *vertical_1_p_data_vector;
	float vertical_1_p_max_value;
	float vertical_1_p_min_value;
//		waveform_mode_1_negative  
	int vertical_1_n_vector_size;
	float *vertical_1_n_index_vector;
	float *vertical_1_n_data_vector;
//		waveform_mode_2_positive 
	int vertical_2_p_vector_size;
	float *vertical_2_p_index_vector;
	float *vertical_2_p_data_vector;
	float vertical_2_p_max_value;
	float vertical_2_p_min_value;
//		waveform_mode_2_negative 
	int vertical_2_n_vector_size;
	float *vertical_2_n_index_vector;
	float *vertical_2_n_data_vector;
//		waveform_mode_3_positive 
	int vertical_breakdown_vector_size;
	float *vertical_breakdown_index_vector;
	float *vertical_breakdown_data_vector;
//		waveform_mode_3_negative 
	int vertical_inversion_vector_size;
	float *vertical_inversion_index_vector;
	float *vertical_inversion_data_vector;
	
//	+waveform_horizontal
//		waveform_mode_1_positive 
	int horizontal_1_p_vector_size;
	float *horizontal_1_p_index_vector;
	float *horizontal_1_p_data_vector;
	float horizontal_1_p_max_value;
	float horizontal_1_p_min_value;
//		waveform_mode_1_negative  
	int horizontal_1_n_vector_size;
	float *horizontal_1_n_index_vector;
	float *horizontal_1_n_data_vector;
//		waveform_mode_2_positive 
	int horizontal_2_p_vector_size;
	float *horizontal_2_p_index_vector;
	float *horizontal_2_p_data_vector;
	float horizontal_2_p_max_value;
	float horizontal_2_p_min_value;
//		waveform_mode_2_negative 
	int horizontal_2_n_vector_size;
	float *horizontal_2_n_index_vector;
	float *horizontal_2_n_data_vector;
//		waveform_mode_3_positive 
	int horizontal_breakdown_vector_size;
	float *horizontal_breakdown_index_vector;
	float *horizontal_breakdown_data_vector;
//		waveform_mode_3_negative 
	int horizontal_inversion_vector_size;
	float *horizontal_inversion_index_vector;
	float *horizontal_inversion_data_vector;

//	+time_windows
	float discharge_time;
	int number_of_cycles;
	bool first_cycle_positive_bool;
	bool auto_breakdown;
//		positive_time_windows
	int positive_number_of_time_windows;
	float *positive_time_windows_values;
	int *positive_primary_mode;
	int *positive_horizontal_mode;
	int *positive_vertical_mode;
//		negative_time_windows
	int negative_number_of_time_windows;
	float *negative_time_windows_values;
	int *negative_primary_mode;
	int *negative_horizontal_mode;
	int *negative_vertical_mode;

//	+tomography
	bool tomography_radial_bool;
	bool tomography_vertical_bool;

//	+electric_probes
	bool electric_radial_bool;
	bool electric_vertical_bool;

//	+magnetic_probes
	bool magnetic_radial_bool;
	bool magnetic_vertical_bool;
//		MirnovArrayDescription
	int NumberOfProbes;
	float *magnetic_Angles;
	float *magnetic_Calibration;
//		Measurements
	int NumberOfMeasurements;
	int *ProbeNumbers;

//	+sine_probe =
	bool sine_vertical_bool;

//	+coseno_probe =
	bool cosine_radial_bool;
	
//	+hibd =
	bool hibd_radial_bool;
	bool hibd_vertical_bool;

//	+interferometry
	bool interferometry_radial_control_bool;

//	+machine_protection
	float iron_core_saturation_value;
	float iron_core_dangerous_value;

//	+plasma_parameters
	float high_current_threshold_value;
//		tomography
	float tomography_radial_high_current_weight;
	float tomography_radial_low_current_weight;
	float tomography_vertical_high_current_weight;
	float tomography_vertical_low_current_weight;
//		electric_probes
	float electric_radial_high_current_weight;
	float electric_radial_low_current_weight;
	float electric_vertical_high_current_weight;
	float electric_vertical_low_current_weight;
//		magnetic_probes
	float magnetic_radial_high_current_weight;
	float magnetic_radial_low_current_weight;
	float magnetic_vertical_high_current_weight;
	float magnetic_vertical_low_current_weight;
//		sine_probe
	float sine_vertical_high_current_weight;
	float sine_vertical_low_current_weight;
//		cosine_probe
	float cosine_radial_high_current_weight;
	float cosine_radial_low_current_weight;
//		hibd
	float hibd_radial_high_current_weight;
	float hibd_radial_low_current_weight;
	float hibd_vertical_high_current_weight;
	float hibd_vertical_low_current_weight;

//	+controller
//		PID_horizontal
	float PID_horizontal_proportional_soft;
	float PID_horizontal_proportional_normal;
	float PID_horizontal_proportional_hard;
	float PID_horizontal_integral_soft;
	float PID_horizontal_integral_normal; 
	float PID_horizontal_integral_hard;
	float PID_horizontal_derivative_soft;
	float PID_horizontal_derivative_normal;
	float PID_horizontal_derivative_hard;
//		PID_vertical =
	float PID_vertical_proportional_soft;
	float PID_vertical_proportional_normal;
	float PID_vertical_proportional_hard;
	float PID_vertical_integral_soft;
	float PID_vertical_integral_normal;
	float PID_vertical_integral_hard;
	float PID_vertical_derivative_soft;
	float PID_vertical_derivative_normal;
	float PID_vertical_derivative_hard;
//		PID_primary 
	float PID_primary_proportional_soft;
	float PID_primary_proportional_normal;
	float PID_primary_proportional_hard;
	float PID_primary_integral_soft;
	float PID_primary_integral_normal;
	float PID_primary_integral_hard;
	float PID_primary_derivative_soft;
	float PID_primary_derivative_normal;
	float PID_primary_derivative_hard;

//	+FACommunicator_vertical
	FString	verticalPS_UARTPortAddress;
	int verticalPS_SimulationTestNumber;
	float verticalPS_PointOfZeroCurrent;
	float verticalPS_CurrentStep;

//	+FACommunicator_horizontal
	FString	horizontalPS_UARTPortAddress;
	int horizontalPS_SimulationTestNumber;
	float horizontalPS_PointOfZeroCurrent;
	float horizontalPS_CurrentStep;

//	+FACommunicator_primary
	FString	primaryPS_UARTPortAddress;
	int primaryPS_SimulationTestNumber;
	float primaryPS_PointOfZeroCurrent;
	float primaryPS_CurrentStep;

	SVGGraphicSupport *graphics_support;


public:

    /** the main entry point for HttpInterface */
    virtual     bool    ProcessHttpMessage(HttpStream &hStream);

    /** the default constructor */
    MainConfigurator(){
    }

    ~MainConfigurator(){
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
	
	bool SortTimeWindows();		// sort and remove repeated submited timewindows
		
};
#endif
