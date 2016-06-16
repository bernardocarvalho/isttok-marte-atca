/*
 * File:   LookupTable.h
 * Author: ivoc, ipfn
 *
 * Created on August 26, 2010
 * last modified on August 26, 2010
 * version: 0.1
 */

#ifndef _TIMEWINDOWSGAM_H
#define	_TIMEWINDOWSGAM_H

//#include <dirent.h>
#include "DDBInputInterface.h"
#include "DDBOutputInterface.h"
#include "HtmlStream.h"
#include "GAM.h"
#include "IntegerSequentialControl.h"

OBJECT_DLL(TimeWindowsGAM)


class TimeWindowsGAM : public GAM, public HttpInterface  {
private:

	DDBInputInterface *SignalsInputInterface;
	DDBOutputInterface *SignalsOutputInterface;

	struct InputInterfaceStruct {
		float PlasmaCurrent;
		int usecTime;
		int InSaturation;
		int SlowStopBool;
		int HardStopBool;
	};
	struct OutputInterfaceStruct {
		int PrimaryWaveformMode;
		int HorizontalWaveformMode;
		int VerticalWaveformMode;
		int usecDischargeTime;
		int PlasmaDirection;
		int DischargeStatus;
		int ToroidalMode;
		int ToroidalStatus;
		int PuffingMode;
		int PuffingStatus;
	};
	
	IntegerSequentialControl *vertical_positive_timewindows;
	IntegerSequentialControl *horizontal_positive_timewindows;
	IntegerSequentialControl *primary_positive_timewindows;
	IntegerSequentialControl *vertical_negative_timewindows;
	IntegerSequentialControl *horizontal_negative_timewindows;
	IntegerSequentialControl *primary_negative_timewindows;
	
	int usec_pre_pulse_time;
	int last_dischage_status;
	int last_plasma_direction;
	int maximum_inversion_usectime;
	int usectime_to_wait_for_starting_operation;
	int time_between_online_and_discharge;
	int saved_usec_time;
	int n_cycles_temp;
	int initial_online_time;
	
	
	int apagar_depois_time_between_pre_and_shot;
	
	
	bool in_positive_breakdown_scenario;
	bool in_negative_breakdown_scenario;
	bool in_inversion_from_positive_to_negative_scenario;
	bool in_inversion_from_negative_to_positive_scenario;
	bool power_supplies_started;
	bool end_discharge_after_unsuccess_bool;

	bool auto_breakdown;
	
	int usecthread_cycle_time;
	
//	FString Directory;
	float discharge_time;
	int number_of_cycles;
	bool first_cycle_positive_bool;
//		positive_time_windows
	int positive_number_of_time_windows;
	float *positive_time_windows_values;
	int *corrected_positive_time_windows_values;
	int *positive_primary_mode;
	int *positive_horizontal_mode;
	int *positive_vertical_mode;
//		negative_time_windows
	int negative_number_of_time_windows;
	float *negative_time_windows_values;
	int *corrected_negative_time_windows_values;
	int *negative_primary_mode;
	int *negative_horizontal_mode;
	int *negative_vertical_mode;
	
	int puffing_mode;
	bool puffing_in_timewindows_mode;
	
	bool view_input_variables;
	
public:

	// Default constructor
	TimeWindowsGAM();

	// Destructor
	virtual ~TimeWindowsGAM();

	// Initialise the module
	virtual bool Initialise(ConfigurationDataBase& cdbData);

	// Execute the module functionalities
	virtual bool Execute(GAM_FunctionNumbers functionNumber);

	virtual bool ProcessHttpMessage(HttpStream &hStream);


	OBJECT_DLL_STUFF(TimeWindowsGAM)
};



#endif

