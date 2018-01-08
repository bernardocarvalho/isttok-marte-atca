
#include "TimeWindowsGAM.h"

OBJECTLOADREGISTER(TimeWindowsGAM, "$Id: $")


//  ******** Default constructor ***********************************
TimeWindowsGAM::TimeWindowsGAM(){
	this->SignalsInputInterface = NULL;
	this->SignalsOutputInterface = NULL;
}


// ********* Destructor ********************************************
TimeWindowsGAM::~TimeWindowsGAM()
{
//	delete the interfaces
//	if(this->SignalsInputInterface != NULL) delete[] this->SignalsInputInterface ;
//	if(this->SignalsOutputInterface != NULL) delete[] this->SignalsOutputInterface;

}
//{ ********* Initialise the module ********************************
bool TimeWindowsGAM::Initialise(ConfigurationDataBase& cdbData){
   
	CDBExtended cdb(cdbData);

	int i;


	if(!cdb.ReadFloat(discharge_time, "discharge_time"))
	{
		AssertErrorCondition(InitialisationError,"TimeWindowsGAM::Initialise: %s discharge_time",this->Name());
		return False;
	}
	else	AssertErrorCondition(Information,"TimeWindowsGAM::Initialise: discharge_time = %f",discharge_time);
	if(!cdb.ReadInt32(number_of_cycles, "number_of_cycles"))
	{
		AssertErrorCondition(InitialisationError,"TimeWindowsGAM::Initialise: %s number_of_cycles",this->Name());
		return False;
	}
	else	AssertErrorCondition(Information,"TimeWindowsGAM::Initialise: number_of_cycles = %d",number_of_cycles);
	if(!cdb.ReadInt32(i, "first_cycle_positive_bool"))
	{
		AssertErrorCondition(InitialisationError,"TimeWindowsGAM::Initialise: %s first_cycle_positive_bool",this->Name());
		return False;
	}
	else 
	{
		first_cycle_positive_bool = (bool)i;
		AssertErrorCondition(Information,"TimeWindowsGAM::Initialise: first_cycle_positive_bool = %d",first_cycle_positive_bool);
	}

	if(!cdb.ReadInt32(i, "auto_breakdown"))
	{
		AssertErrorCondition(InitialisationError,"TimeWindowsGAM::Initialise: %s auto_breakdown",this->Name());
		return False;
	}
	else 
	{
		auto_breakdown = (bool)i;
		AssertErrorCondition(Information,"TimeWindowsGAM::Initialise: auto_breakdown = %d",auto_breakdown);
	}
	if(!cdb.ReadInt32(usecthread_cycle_time, "usecthread_cycle_time"))
	{
		AssertErrorCondition(InitialisationError,"TimeWindowsGAM::Initialise: %s usecthread_cycle_time",this->Name());
		return False;
	}
	else AssertErrorCondition(Information,"TimeWindowsGAM::Initialise: usecthread_cycle_time = %d",usecthread_cycle_time);
	if(!cdb.ReadInt32(usec_pre_pulse_time, "usec_pre_pulse_time"))
	{
		AssertErrorCondition(InitialisationError,"TimeWindowsGAM::Initialise: %s usec_pre_pulse_time",this->Name());
		return False;
	}
	else AssertErrorCondition(Information,"TimeWindowsGAM::Initialise: usec_pre_pulse_time = %d",usec_pre_pulse_time);
	if(!cdb.ReadInt32(maximum_inversion_usectime, "maximum_inversion_usectime"))
	{
		AssertErrorCondition(InitialisationError,"TimeWindowsGAM::Initialise: %s maximum_inversion_usectime",this->Name());
		return False;
	}
	else AssertErrorCondition(Information,"TimeWindowsGAM::Initialise: maximum_inversion_usectime = %d",maximum_inversion_usectime);
	if(!cdb.ReadInt32(usectime_to_wait_for_starting_operation, "usectime_to_wait_for_starting_operation"))
	{
		AssertErrorCondition(InitialisationError,"TimeWindowsGAM::Initialise: %s usectime_to_wait_for_starting_operation",this->Name());
		return False;
	}
	else AssertErrorCondition(Information,"TimeWindowsGAM::Initialise: usectime_to_wait_for_starting_operation = %d",usectime_to_wait_for_starting_operation);
	if(!cdb.ReadInt32(puffing_mode, "puffing_mode"))
	{
		AssertErrorCondition(InitialisationError,"TimeWindowsGAM::Initialise: %s puffing_mode",this->Name());
		return False;
	}
	else AssertErrorCondition(Information,"TimeWindowsGAM::Initialise: puffing_mode = %d",puffing_mode);
	if(!cdb.ReadInt32(i, "end_discharge_after_unsuccess_bool"))
	{
		CStaticAssertErrorCondition(InitialisationError,"TimeWindowsGAM::ReadConfigurationFile: %s end_discharge_after_unsuccess_bool",this->Name());
		return False;
	}
	else 
	{
		end_discharge_after_unsuccess_bool = (bool)i;
		CStaticAssertErrorCondition(Information,"TimeWindowsGAM::ReadConfigurationFile: end_discharge_after_unsuccess_bool = %d",end_discharge_after_unsuccess_bool);
	}	
	if(!cdb.ReadInt32(time_between_online_and_discharge, "time_between_online_and_discharge"))
	{
		AssertErrorCondition(InitialisationError,"TimeWindowsGAM::Initialise: %s time_between_online_and_discharge",this->Name());
		return False;
	}
	else AssertErrorCondition(Information,"TimeWindowsGAM::Initialise: time_between_online_and_discharge = %d",time_between_online_and_discharge);

	if(!cdb->Move("positive_time_windows"))
	{
		AssertErrorCondition(InitialisationError,"TimeWindowsGAM::Initialise: %s Could not move to \"+MARTe.+ISTTOK_RTTh.+time_windows.positive_time_windows\"",this->Name());
		return False;
	}
		if(!cdb.ReadInt32(positive_number_of_time_windows, "number_of_time_windows"))
		{
			AssertErrorCondition(InitialisationError,"TimeWindowsGAM::Initialise: %s positive_number_of_time_windows",this->Name());
			return False;
		}
		else	AssertErrorCondition(Information,"TimeWindowsGAM::Initialise: positive_number_of_time_windows = %d",positive_number_of_time_windows);
		if (positive_number_of_time_windows > 0){
			positive_time_windows_values =new float[positive_number_of_time_windows];
			positive_primary_mode =new int[positive_number_of_time_windows];
			positive_horizontal_mode =new int[positive_number_of_time_windows];
			positive_vertical_mode =new int[positive_number_of_time_windows];

			if(!cdb.ReadFloatArray(positive_time_windows_values, (int *)(&positive_number_of_time_windows), 1, "time_windows_values"))
			{
				AssertErrorCondition(InitialisationError,"TimeWindowsGAM: Could not read positive_time_windows_values");
				return False;
			}
			else for(i=0;i<positive_number_of_time_windows;i++) AssertErrorCondition(Information,"TimeWindowsGAM::Initialise: positive_time_windows_values[%d] = %d",i, positive_time_windows_values[i]);
			if(!cdb.ReadInt32Array(positive_primary_mode, (int *)(&positive_number_of_time_windows), 1, "primary_mode"))
			{
				AssertErrorCondition(InitialisationError,"TimeWindowsGAM: Could not positive_primary_mode");
				return False;
			}
			else for(i=0;i<positive_number_of_time_windows;i++) AssertErrorCondition(Information,"TimeWindowsGAM::Initialise: positive_primary_mode[%d] = %d",i, positive_primary_mode[i]);
			if(!cdb.ReadInt32Array(positive_horizontal_mode, (int *)(&positive_number_of_time_windows), 1, "horizontal_mode"))
			{
				AssertErrorCondition(InitialisationError,"TimeWindowsGAM: Could not read positive_horizontal_mode");
				return False;
			}
			else for(i=0;i<positive_number_of_time_windows;i++) AssertErrorCondition(Information,"TimeWindowsGAM::Initialise: positive_horizontal_mode[%d] = %d",i, positive_horizontal_mode[i]);
			if(!cdb.ReadInt32Array(positive_vertical_mode, (int *)(&positive_number_of_time_windows), 1, "vertical_mode"))
			{
				AssertErrorCondition(InitialisationError,"TimeWindowsGAM: Could not read positive_vertical_mode");
				return False;
			}
			else for(i=0;i<positive_number_of_time_windows;i++) AssertErrorCondition(Information,"TimeWindowsGAM::Initialise: positive_vertical_mode[%d] = %d",i, positive_vertical_mode[i]);
		}
	cdb->MoveToFather();


	if(!cdb->Move("negative_time_windows"))
	{
		AssertErrorCondition(InitialisationError,"TimeWindowsGAM::Initialise: %s Could not move to \"+MARTe.+ISTTOK_RTTh.+time_windows.negative_time_windows\"",this->Name());
		return False;
	}
		if(!cdb.ReadInt32(negative_number_of_time_windows, "number_of_time_windows"))
		{
			AssertErrorCondition(InitialisationError,"TimeWindowsGAM::Initialise: %s negative_number_of_time_windows",this->Name());
			return False;
		}
		else	AssertErrorCondition(Information,"TimeWindowsGAM::Initialise: negative_number_of_time_windows = %d",negative_number_of_time_windows);
		
		if (positive_number_of_time_windows + negative_number_of_time_windows < 1){
				AssertErrorCondition(InitialisationError,"TimeWindowsGAM: TIMEWINDOWS WERE NOT DEFINED!!! positive_number_of_time_windows + negative_number_of_time_windows < 1");
				return False;
		}

		if (negative_number_of_time_windows > 0){

			negative_time_windows_values =new float[negative_number_of_time_windows];
			negative_primary_mode =new int[negative_number_of_time_windows];
			negative_horizontal_mode =new int[negative_number_of_time_windows];
			negative_vertical_mode =new int[negative_number_of_time_windows];

			if(!cdb.ReadFloatArray(negative_time_windows_values, (int *)(&negative_number_of_time_windows), 1, "time_windows_values"))
			{
				AssertErrorCondition(InitialisationError,"TimeWindowsGAM: Could not read negative_time_windows_values");
				return False;
			}
			else for(i=0;i<negative_number_of_time_windows;i++) AssertErrorCondition(Information,"TimeWindowsGAM::Initialise: negative_time_windows_values[%d] = %d",i, negative_time_windows_values[i]);
			if(!cdb.ReadInt32Array(negative_primary_mode, (int *)(&negative_number_of_time_windows), 1, "primary_mode"))
			{
				AssertErrorCondition(InitialisationError,"TimeWindowsGAM: Could not negative_primary_mode");
				return False;
			}
			else for(i=0;i<negative_number_of_time_windows;i++) AssertErrorCondition(Information,"TimeWindowsGAM::Initialise: negative_primary_mode[%d] = %d",i, negative_primary_mode[i]);
			if(!cdb.ReadInt32Array(negative_horizontal_mode, (int *)(&negative_number_of_time_windows), 1, "horizontal_mode"))
			{
				AssertErrorCondition(InitialisationError,"TimeWindowsGAM: Could not read negative_horizontal_mode");
				return False;
			}
			else for(i=0;i<negative_number_of_time_windows;i++) AssertErrorCondition(Information,"TimeWindowsGAM::Initialise: negative_horizontal_mode[%d] = %d",i, negative_horizontal_mode[i]);
			if(!cdb.ReadInt32Array(negative_vertical_mode, (int *)(&negative_number_of_time_windows), 1, "vertical_mode"))
			{
				AssertErrorCondition(InitialisationError,"TimeWindowsGAM: Could not read negative_vertical_mode");
				return False;
			}
			else for(i=0;i<negative_number_of_time_windows;i++) AssertErrorCondition(Information,"TimeWindowsGAM::Initialise: negative_vertical_mode[%d] = %d",i, negative_vertical_mode[i]);
		}
	cdb->MoveToFather();

//	sleep(5);
// Create the signal interfaces
	if(!AddInputInterface(this->SignalsInputInterface, "TimewindowsGAMInputInterface"))
	{
		AssertErrorCondition(InitialisationError, "TimeWindowsGAM::Initialise: %s failed to add the TimewindowsGAMInputInterface", this->Name());
		return False;
	}
	if(!AddOutputInterface(this->SignalsOutputInterface, "TimewindowsGAMOutputInterface"))
	{
		AssertErrorCondition(InitialisationError, "TimeWindowsGAM::Initialise: %s failed to add the TimewindowsGAMOutputInterface", this->Name());
		return False;
	}

//	INPUT SIGNALS (interface)
	if(!cdb->Move("input_signals"))
	{
		AssertErrorCondition(InitialisationError,"TimeWindowsGAM::Initialise: %s Could not move to \"input_signals\"",this->Name());
		return False;
	}
		int number_of_signals_to_read = 5;
		FString *CDB_move_to;
		FString *SignalType;
		CDB_move_to = new FString[number_of_signals_to_read];
		SignalType = new FString[number_of_signals_to_read];
        CDB_move_to[0].Printf("plasma_current");
		CDB_move_to[1].Printf("system_time");
		CDB_move_to[2].Printf("is_saturated_bool");
		CDB_move_to[3].Printf("slow_stop");
		CDB_move_to[4].Printf("hard_stop");
		for (i=0;i<number_of_signals_to_read;i++){
			
			if(!cdb->Move(CDB_move_to[i].Buffer()))
			{
				AssertErrorCondition(InitialisationError,"TimeWindowsGAM::Initialise: %s Could not move to \"%s\"",this->Name(),CDB_move_to[i].Buffer());
				return False;
			}
			
			if(cdb->Exists("SignalType"))
			{
				FString signalName;
				cdb.ReadFString(SignalType[i], "SignalType");
			}
			if(cdb->Exists("SignalName"))
			{
				FString SignalName;
				cdb.ReadFString(SignalName, "SignalName");
				AssertErrorCondition(Information,"TimeWindowsGAM::Initialise: Added signal = %s", SignalName.Buffer());
				
				if(!this->SignalsInputInterface->AddSignal(SignalName.Buffer(), SignalType[i].Buffer()))
				{
					AssertErrorCondition(InitialisationError,"TimeWindowsGAM::Initialise: %s failed to add signal", this->Name());
					return False;
				}
			}
			
			
			cdb->MoveToFather();
		}

	cdb->MoveToFather();


//	OUTPUT SIGNALS (interface)
	if(!cdb->Move("output_signals"))
	{
		AssertErrorCondition(InitialisationError,"TimeWindowsGAM::Initialise: %s Could not move to \"output_signals\"",this->Name());
		return False;
	}
	
		number_of_signals_to_read = 10;
		CDB_move_to = new FString[number_of_signals_to_read];
		SignalType = new FString[number_of_signals_to_read];
		CDB_move_to[0].Printf("primary_mode");
		CDB_move_to[1].Printf("horizontal_mode");
		CDB_move_to[2].Printf("vertical_mode");
		CDB_move_to[3].Printf("time_to_waveforms");
		CDB_move_to[4].Printf("discharge_direction");
		CDB_move_to[5].Printf("discharge_status");
		CDB_move_to[6].Printf("toroidal_mode");
		CDB_move_to[7].Printf("toroidal_status");
		CDB_move_to[8].Printf("puffing_mode");
		CDB_move_to[9].Printf("puffing_status");
		for (i=0;i<number_of_signals_to_read;i++){
			
			if(!cdb->Move(CDB_move_to[i].Buffer()))
			{
				AssertErrorCondition(InitialisationError,"TimeWindowsGAM::Initialise: %s Could not move to \"%s\"",this->Name(),CDB_move_to[i].Buffer());
				return False;
			}
			
			if(cdb->Exists("SignalType"))
			{
				FString signalName;
				cdb.ReadFString(SignalType[i], "SignalType");
			}
			if(cdb->Exists("SignalName"))
			{
				FString SignalName;
				cdb.ReadFString(SignalName, "SignalName");
				AssertErrorCondition(Information,"TimeWindowsGAM::Initialise: Added signal = %s", SignalName.Buffer());
				
				if(!this->SignalsOutputInterface->AddSignal(SignalName.Buffer(), SignalType[i].Buffer()))
				{
					AssertErrorCondition(InitialisationError,"TimeWindowsGAM::Initialise: %s failed to add signal", this->Name());
					return False;
				}
			}
			cdb->MoveToFather();
		}

	cdb->MoveToFather();

	discharge_time = discharge_time * 1000; //ms to us conversion
	saved_usec_time = 10000000;
	
	//additional validations
	if (number_of_cycles < 1 ){
		AssertErrorCondition(InitialisationError,"TimeWindowsGAM: ERROR on the number semi-cycles on a discharge!!! number_of_cycles < 1");
		return False;
	}
	if (positive_number_of_time_windows < 1 && (first_cycle_positive_bool || number_of_cycles > 1)){
			AssertErrorCondition(InitialisationError,"TimeWindowsGAM: ERROR on the number of positive timewindows!!! positive_number_of_time_windows < 1 && (first_cycle_positive_bool || number_of_cycles > 1)");
			return False;
	}
	if (negative_number_of_time_windows < 1 && (!first_cycle_positive_bool || number_of_cycles > 1)){
			AssertErrorCondition(InitialisationError,"TimeWindowsGAM: ERROR on the number of negative timewindows!!! negative_number_of_time_windows < 1 && (!first_cycle_positive_bool || number_of_cycles > 1)");
			return False;
	}
	
	// transform the waveform ms in us (x1000)
	corrected_positive_time_windows_values = new int[positive_number_of_time_windows];
	corrected_negative_time_windows_values = new int[negative_number_of_time_windows];
	for (i=0;i<positive_number_of_time_windows;i++)	corrected_positive_time_windows_values[i] = int (positive_time_windows_values[i] * 1000);
	for (i=0;i<negative_number_of_time_windows;i++)	corrected_negative_time_windows_values[i] = int (negative_time_windows_values[i] * 1000);
	
	this->vertical_positive_timewindows = new IntegerSequentialControl(&corrected_positive_time_windows_values[0], &positive_vertical_mode[0], positive_number_of_time_windows);
	this->horizontal_positive_timewindows = new IntegerSequentialControl(&corrected_positive_time_windows_values[0], &positive_horizontal_mode[0], positive_number_of_time_windows);
	this->primary_positive_timewindows = new IntegerSequentialControl(&corrected_positive_time_windows_values[0], &positive_primary_mode[0], positive_number_of_time_windows);
	this->vertical_negative_timewindows = new IntegerSequentialControl(&corrected_negative_time_windows_values[0], &negative_vertical_mode[0], negative_number_of_time_windows);
	this->horizontal_negative_timewindows = new IntegerSequentialControl(&corrected_negative_time_windows_values[0], &negative_horizontal_mode[0], negative_number_of_time_windows);
	this->primary_negative_timewindows = new IntegerSequentialControl(&corrected_negative_time_windows_values[0], &negative_primary_mode[0], negative_number_of_time_windows);
	
	this->vertical_positive_timewindows->DefineDefaultValue(0);
	this->horizontal_positive_timewindows->DefineDefaultValue(0);
	this->primary_positive_timewindows->DefineDefaultValue(0);
	this->vertical_negative_timewindows->DefineDefaultValue(0);
	this->horizontal_negative_timewindows->DefineDefaultValue(0);
	this->primary_negative_timewindows->DefineDefaultValue(0);
	
	if (puffing_mode == 3) puffing_in_timewindows_mode = False;
	else puffing_in_timewindows_mode = True;
	
	return True;
}
//} ******************************************************************



//{ ********* Execute the module functionalities *******************
bool TimeWindowsGAM::Execute(GAM_FunctionNumbers functionNumber){
 
	
//	AssertErrorCondition(InitialisationError,"TimeWindowsGAM::Execute: discharge_time = %f",discharge_time);


	InputInterfaceStruct *inputstruct = (InputInterfaceStruct *) this->SignalsInputInterface->Buffer();
	this->SignalsInputInterface->Read();
//    AssertErrorCondition(InitialisationError,"TimeWindowsGAM:: %s inputstruct = %f %d %d %d %d",this->Name(), inputstruct[0].PlasmaCurrent, inputstruct[0].usecTime, inputstruct[0].InSaturation, inputstruct[0].SlowStopBool, inputstruct[0].HardStopBool);
	OutputInterfaceStruct *outputstruct = (OutputInterfaceStruct *) this->SignalsOutputInterface->Buffer();

/*
	*** Operation modes ***
	0 -> off
	1 -> current control
	2 -> position / plasma current control - soft PID
	3 -> position / plasma current control - medium PID
	4 -> position / plasma current control - hard PID
	5 -> position / plasma current control - Auto PID (with adaptative gain)
	6 -> *integrated control (scenario)
	7 -> *Invert (scenario)
*/
/*
	*** Discharge Status ***
	-3 -> error
	-2 -> offline
	0 -> breakdown
	1 -> normal operation
	2 -> inverting
*/
/*
	*** puffing_mode ***
	0 -> off
	1 -> time-windows, out percentage
	2 -> time windows density feedback
	3 -> absolute time, out percentage
	4 -> preprogrammed on breakdown, feedback on time-windows
*/

// !!!!! falta usar time_between_online_and_discharge e initial_online_time... para pre-puffing e pre-campo toroidal

	if (functionNumber == GAMOffline){
		
		inputstruct[0].SlowStopBool = False;
		inputstruct[0].HardStopBool = False;

		saved_usec_time = 10000000;
		outputstruct[0].PrimaryWaveformMode = 0;
		outputstruct[0].HorizontalWaveformMode = 0;
		outputstruct[0].VerticalWaveformMode = 0;
		outputstruct[0].usecDischargeTime = -20000;
		outputstruct[0].DischargeStatus = -2;
		outputstruct[0].ToroidalMode = 0;
		outputstruct[0].ToroidalStatus = -2;
		outputstruct[0].PuffingMode = 0; 
		outputstruct[0].PuffingStatus = -2;
		last_dischage_status = -2;
		n_cycles_temp = number_of_cycles;
		power_supplies_started = False;

		if( first_cycle_positive_bool ){ 
			outputstruct[0].PlasmaDirection = 1;
			last_plasma_direction = 1;
		}
		else {
			outputstruct[0].PlasmaDirection = 0;
			last_plasma_direction = 0;
		}
		in_positive_breakdown_scenario = False;
		in_negative_breakdown_scenario = False;
		in_inversion_from_positive_to_negative_scenario = False;
		in_inversion_from_negative_to_positive_scenario = False;
	}
	if (functionNumber == GAMPrepulse){
		
		saved_usec_time = inputstruct[0].usecTime;
		outputstruct[0].PrimaryWaveformMode = 0;
		outputstruct[0].HorizontalWaveformMode = 0;
		outputstruct[0].VerticalWaveformMode = 0;
		outputstruct[0].usecDischargeTime = -20000;
		outputstruct[0].DischargeStatus = -1;
		outputstruct[0].ToroidalMode = 0;
		outputstruct[0].ToroidalStatus = -1;
		outputstruct[0].PuffingMode = 0; 
		outputstruct[0].PuffingStatus = -1;
		last_dischage_status = -1;
		n_cycles_temp = number_of_cycles;
		power_supplies_started = False;
		if( first_cycle_positive_bool ){ 
			outputstruct[0].PlasmaDirection = 1;
			last_plasma_direction = 1;
			//check if breakdown scenario is going to be used mode = 7
			if (auto_breakdown){
				in_positive_breakdown_scenario = True;
				in_negative_breakdown_scenario = False;
				in_inversion_from_positive_to_negative_scenario = False;
				in_inversion_from_negative_to_positive_scenario = False;
			} 
		}
		else {
			//check if breakdown scenario is going to be used
			if (auto_breakdown){
				in_negative_breakdown_scenario = True;
				in_positive_breakdown_scenario = False;
				in_inversion_from_positive_to_negative_scenario = False;
				in_inversion_from_negative_to_positive_scenario = False;
			}
			outputstruct[0].PlasmaDirection = 0;
			last_plasma_direction = 0;
		}
		
		initial_online_time = inputstruct[0].usecTime; //saved  time
		
	}
	if (functionNumber == GAMOnline){
		
		if (inputstruct[0].SlowStopBool && last_dischage_status != -3){
			outputstruct[0].PrimaryWaveformMode = 0;
			outputstruct[0].HorizontalWaveformMode = 0;
			outputstruct[0].VerticalWaveformMode = 0;
			outputstruct[0].ToroidalMode = 0;
			outputstruct[0].PuffingMode = 0;
			AssertErrorCondition(InitialisationError,"TimeWindowsGAM:: %s SLOW STOP WAS ASSIGNED!!!",this->Name());
			last_dischage_status = -3;
		}
		else if (inputstruct[0].HardStopBool && last_dischage_status != -3){
			outputstruct[0].PrimaryWaveformMode = 0;
			outputstruct[0].HorizontalWaveformMode = 0;
			outputstruct[0].VerticalWaveformMode = 0;
			outputstruct[0].ToroidalMode = 0;
			outputstruct[0].PuffingMode = 0;
			AssertErrorCondition(InitialisationError,"TimeWindowsGAM:: %s HARD STOP WAS ASSIGNED!!!",this->Name());
			last_dischage_status = -3;
		}
		else if (last_dischage_status == -3){
			outputstruct[0].PrimaryWaveformMode = 0;
			outputstruct[0].HorizontalWaveformMode = 0;
			outputstruct[0].VerticalWaveformMode = 0;
			outputstruct[0].ToroidalMode = 0;
			outputstruct[0].PuffingMode = 0; 
		}
		else {	// if no stops are issued
			
			if(inputstruct[0].usecTime >= usectime_to_wait_for_starting_operation && inputstruct[0].usecTime <= (usectime_to_wait_for_starting_operation + discharge_time) && n_cycles_temp > 0){
				if (!power_supplies_started){
					power_supplies_started = True;
					AssertErrorCondition(InitialisationError,"TimeWindowsGAM:: %s power_supplies_started at %d us",this->Name(),inputstruct[0].usecTime);
					saved_usec_time = inputstruct[0].usecTime;					
					
				}
				if (in_positive_breakdown_scenario){
					
					
					if (puffing_in_timewindows_mode && puffing_mode != 4) outputstruct[0].PuffingStatus = -2;
					
					
					if (usec_pre_pulse_time > inputstruct[0].usecTime - usectime_to_wait_for_starting_operation){
						if (inputstruct[0].PlasmaCurrent > 750){
							in_positive_breakdown_scenario = False; // breakdown successful
							if (puffing_mode == 4 ) outputstruct[0].PuffingMode = 2;  //end of breakdown -> if puffing mode == 4 -> change to puffing feedback in time windows
							outputstruct[0].usecDischargeTime = 0;
							saved_usec_time = inputstruct[0].usecTime;
							outputstruct[0].DischargeStatus = 1;
							last_dischage_status = 0;
						}
						else {
							outputstruct[0].PrimaryWaveformMode = 1;
							outputstruct[0].HorizontalWaveformMode = 1;
							outputstruct[0].VerticalWaveformMode = 1;
							outputstruct[0].usecDischargeTime = inputstruct[0].usecTime - usectime_to_wait_for_starting_operation;
							outputstruct[0].PlasmaDirection = 1;
							outputstruct[0].DischargeStatus = 0;
						}
					}
					else if (end_discharge_after_unsuccess_bool){
						AssertErrorCondition(InitialisationError,"TimeWindowsGAM::Execute: Breakdown to positive Ip not successful at %d us - ending dishcarge!!!", inputstruct[0].usecTime);
						outputstruct[0].PrimaryWaveformMode = 0;
						outputstruct[0].HorizontalWaveformMode = 0;
						outputstruct[0].VerticalWaveformMode = 0;
						outputstruct[0].PuffingMode = 0;
						outputstruct[0].ToroidalMode = 0;
						outputstruct[0].DischargeStatus = -3;
						last_dischage_status = -3;
					}
					else {
						AssertErrorCondition(InitialisationError,"TimeWindowsGAM::Execute: Breakdown to positive Ip not successful at %d us- skip phase !!!", inputstruct[0].usecTime);
						in_positive_breakdown_scenario = False;
						if (puffing_mode == 4 ) outputstruct[0].PuffingMode = 2;  //end of breakdown -> if puffing mode == 4 -> change to puffing feedback in time windows
						outputstruct[0].usecDischargeTime = 0;
						saved_usec_time = inputstruct[0].usecTime;
						outputstruct[0].DischargeStatus = 1;
						last_dischage_status = 0;						
					}
				}
				if (in_negative_breakdown_scenario){					
					
					if (puffing_in_timewindows_mode && puffing_mode != 4) outputstruct[0].PuffingStatus = -2;
					
					
					if (usec_pre_pulse_time > inputstruct[0].usecTime - usectime_to_wait_for_starting_operation){
						if (inputstruct[0].PlasmaCurrent < -750){
							in_negative_breakdown_scenario = False;
							if (puffing_mode == 4 ) outputstruct[0].PuffingMode = 2;  //end of breakdown -> if puffing mode == 4 -> change to puffing feedback in time windows
							outputstruct[0].usecDischargeTime = 0;
							saved_usec_time = inputstruct[0].usecTime;
							outputstruct[0].DischargeStatus = 1;
							last_dischage_status = 0;
						}
						else {
							outputstruct[0].PrimaryWaveformMode = 1;
							outputstruct[0].HorizontalWaveformMode = 1;
							outputstruct[0].VerticalWaveformMode = 1;
							outputstruct[0].usecDischargeTime = inputstruct[0].usecTime - usectime_to_wait_for_starting_operation;
							outputstruct[0].PlasmaDirection = 0;
							outputstruct[0].DischargeStatus = 0;
						}
					}
					else if (end_discharge_after_unsuccess_bool){
						AssertErrorCondition(InitialisationError,"TimeWindowsGAM::Execute: Breakdown to negative Ip not successful at %d us - ending dishcarge!!!", inputstruct[0].usecTime);
						outputstruct[0].PrimaryWaveformMode = 0;
						outputstruct[0].HorizontalWaveformMode = 0;
						outputstruct[0].VerticalWaveformMode = 0;
						outputstruct[0].PuffingMode = 0;
						outputstruct[0].ToroidalMode = 0;
						outputstruct[0].DischargeStatus = -3;
						last_dischage_status = -3;
					}
					else {
						AssertErrorCondition(InitialisationError,"TimeWindowsGAM::Execute: Breakdown to negative Ip not successful at %d us - skip phase !!!", inputstruct[0].usecTime);
						in_negative_breakdown_scenario = False;
						if (puffing_mode == 4 ) outputstruct[0].PuffingMode = 2;  //end of breakdown -> if puffing mode == 4 -> change to puffing feedback in time windows
						outputstruct[0].usecDischargeTime = 0;
						saved_usec_time = inputstruct[0].usecTime;
						outputstruct[0].DischargeStatus = 1;
						last_dischage_status = 0;
					}
				}
				if (in_inversion_from_positive_to_negative_scenario){
					
					
					if (puffing_in_timewindows_mode) outputstruct[0].PuffingStatus = -2;
					
					
					if (maximum_inversion_usectime > inputstruct[0].usecTime - saved_usec_time){
						if (inputstruct[0].PlasmaCurrent < -750){
							in_inversion_from_positive_to_negative_scenario = False;
							outputstruct[0].usecDischargeTime = 0;
							saved_usec_time = inputstruct[0].usecTime;
							outputstruct[0].PlasmaDirection = 0;
							outputstruct[0].DischargeStatus = 1;
							last_dischage_status = 2;
							n_cycles_temp--;
						}
						else {
							outputstruct[0].PrimaryWaveformMode = 1;
							outputstruct[0].HorizontalWaveformMode = 1;
							outputstruct[0].VerticalWaveformMode = 1;
							outputstruct[0].usecDischargeTime = inputstruct[0].usecTime - saved_usec_time;
							outputstruct[0].PlasmaDirection = 1;
							outputstruct[0].DischargeStatus = 2;
							last_dischage_status = 2;
						}
					}
					else  if (end_discharge_after_unsuccess_bool){
						AssertErrorCondition(InitialisationError,"TimeWindowsGAM::Execute: inversion_from_positive_to_negative not successful at %d us - ending dishcarge!!!", inputstruct[0].usecTime);
						outputstruct[0].PrimaryWaveformMode = 0;
						outputstruct[0].HorizontalWaveformMode = 0;
						outputstruct[0].VerticalWaveformMode = 0;
						outputstruct[0].PuffingMode = 0;
						outputstruct[0].ToroidalMode = 0;
						outputstruct[0].DischargeStatus = -3;
						last_dischage_status = -3;
					}
					else {
						AssertErrorCondition(InitialisationError,"TimeWindowsGAM::Execute: inversion_from_positive_to_negative not successful at %d us - skip phase !!!",inputstruct[0].usecTime);
						in_inversion_from_positive_to_negative_scenario = False;
						outputstruct[0].usecDischargeTime = 0;
						saved_usec_time = inputstruct[0].usecTime;
						outputstruct[0].PlasmaDirection = 0;
						outputstruct[0].DischargeStatus = 1;
						last_dischage_status = 2;
						n_cycles_temp--;
					}
				}
				if (in_inversion_from_negative_to_positive_scenario){
					
					
					if (puffing_in_timewindows_mode) outputstruct[0].PuffingStatus = -2;
					
					
					if (maximum_inversion_usectime > inputstruct[0].usecTime - saved_usec_time){
						if (inputstruct[0].PlasmaCurrent > 750){
							in_inversion_from_negative_to_positive_scenario = False;
							outputstruct[0].usecDischargeTime = 0;
							saved_usec_time = inputstruct[0].usecTime;
							outputstruct[0].PlasmaDirection = 1;
							outputstruct[0].DischargeStatus = 1;
							last_dischage_status = 2;
							n_cycles_temp--;
						}
						else {
							outputstruct[0].PrimaryWaveformMode = 1;
							outputstruct[0].HorizontalWaveformMode = 1;
							outputstruct[0].VerticalWaveformMode = 1;
							outputstruct[0].usecDischargeTime = inputstruct[0].usecTime - saved_usec_time;
							outputstruct[0].PlasmaDirection = 0;
							outputstruct[0].DischargeStatus = 2;
							last_dischage_status = 2;
						}
					}
					else if (end_discharge_after_unsuccess_bool){
						AssertErrorCondition(InitialisationError,"TimeWindowsGAM::Execute: inversion_from_negative_to_positive not successful at %d us - ending dishcarge!!!", inputstruct[0].usecTime);
						outputstruct[0].PrimaryWaveformMode = 0;
						outputstruct[0].HorizontalWaveformMode = 0;
						outputstruct[0].VerticalWaveformMode = 0;
						outputstruct[0].PuffingMode = 0;
						outputstruct[0].ToroidalMode = 0;
						outputstruct[0].DischargeStatus = -3;
						last_dischage_status = -3;
					}
					else {
						AssertErrorCondition(InitialisationError,"TimeWindowsGAM::Execute: inversion_from_negative_to_positive not successful at %d us - skip phase !!!",inputstruct[0].usecTime);
						in_inversion_from_negative_to_positive_scenario = False;
						outputstruct[0].usecDischargeTime = 0;
						saved_usec_time = inputstruct[0].usecTime;
						outputstruct[0].PlasmaDirection = 1;
						outputstruct[0].DischargeStatus = 1;
						last_dischage_status = 2;
						n_cycles_temp--;
					}
				}
				if (!in_positive_breakdown_scenario  && !in_negative_breakdown_scenario  && !in_inversion_from_positive_to_negative_scenario  && !in_inversion_from_negative_to_positive_scenario && inputstruct[0].InSaturation == 1){
					if (n_cycles_temp == 1) n_cycles_temp = 0; // end dischage
					else{
						saved_usec_time = inputstruct[0].usecTime;
						if (outputstruct[0].PlasmaDirection == 0) in_inversion_from_negative_to_positive_scenario = True;
						else if (outputstruct[0].PlasmaDirection == 1) in_inversion_from_positive_to_negative_scenario = True;
					}   //save usec_time for starting inversion waveform at 0s;
					AssertErrorCondition(InitialisationError,"TimeWindowsGAM::Execute: IRON CORE SATURATION ACHIVED at %d us !!!",inputstruct[0].usecTime );
					
				}
				if ( !in_positive_breakdown_scenario  && !in_negative_breakdown_scenario  && !in_inversion_from_positive_to_negative_scenario  && !in_inversion_from_negative_to_positive_scenario && inputstruct[0].InSaturation == 0 ){
					//normal timewindows sequence starting  at 0
					outputstruct[0].ToroidalStatus = 1;
					
					if (puffing_in_timewindows_mode) outputstruct[0].PuffingStatus = 1;
					else outputstruct[0].PuffingStatus = 0;
					
					outputstruct[0].usecDischargeTime = (inputstruct[0].usecTime - saved_usec_time);
					if (outputstruct[0].PlasmaDirection == 1){
						outputstruct[0].PrimaryWaveformMode = this->primary_positive_timewindows->GetWaveformValue(outputstruct[0].usecDischargeTime);
						outputstruct[0].HorizontalWaveformMode = this->horizontal_positive_timewindows->GetWaveformValue(outputstruct[0].usecDischargeTime);
						outputstruct[0].VerticalWaveformMode = this->vertical_positive_timewindows->GetWaveformValue(outputstruct[0].usecDischargeTime);
			
						if (outputstruct[0].PrimaryWaveformMode >2 && outputstruct[0].PrimaryWaveformMode <7) outputstruct[0].PrimaryWaveformMode = 2;
						if (outputstruct[0].HorizontalWaveformMode >2 && outputstruct[0].HorizontalWaveformMode <7) outputstruct[0].HorizontalWaveformMode = 2;
						if (outputstruct[0].VerticalWaveformMode >2 && outputstruct[0].VerticalWaveformMode  <7) outputstruct[0].VerticalWaveformMode = 2;
						
						outputstruct[0].DischargeStatus = 1;
						outputstruct[0].PlasmaDirection = 1;
						if (outputstruct[0].PrimaryWaveformMode == 7 && outputstruct[0].HorizontalWaveformMode == 7 && outputstruct[0].VerticalWaveformMode == 7){
							if (n_cycles_temp == 1) n_cycles_temp = 0; // end dischage
							else{
								saved_usec_time = inputstruct[0].usecTime;
								in_inversion_from_positive_to_negative_scenario = True; // invert
							}   //save usec_time for starting inversion waveform at 0s;
						}
						last_dischage_status = 1;
					}
					if (outputstruct[0].PlasmaDirection == 0){
						outputstruct[0].PrimaryWaveformMode = this->primary_negative_timewindows->GetWaveformValue(outputstruct[0].usecDischargeTime);
						outputstruct[0].HorizontalWaveformMode = this->horizontal_negative_timewindows->GetWaveformValue(outputstruct[0].usecDischargeTime);
						outputstruct[0].VerticalWaveformMode = this->vertical_negative_timewindows->GetWaveformValue(outputstruct[0].usecDischargeTime);
						outputstruct[0].DischargeStatus = 1;
						outputstruct[0].PlasmaDirection = 0;
						if (outputstruct[0].PrimaryWaveformMode == 7 && outputstruct[0].HorizontalWaveformMode == 7 && outputstruct[0].VerticalWaveformMode == 7){
							if (n_cycles_temp == 1) n_cycles_temp =0; // end dischage
							else {
								saved_usec_time = inputstruct[0].usecTime;  //save usec_time for starting inversion waveform at 0s;
								in_inversion_from_negative_to_positive_scenario = True;// invert
							}  
						}
						last_dischage_status = 1;
					}
				}
			}
			
			
/*			//apagar depois - testar tempo entre pre e shot
			if (inputstruct[0].usecTime < 0 || inputstruct[0].usecTime > 10000000) {
				apagar_depois_time_between_pre_and_shot = inputstruct[0].usecTime - initial_online_time;
			}
*/			
/*			
			//trial - prepuffing and toroidal before 0 seconds (trigger)
			if (inputstruct[0].usecTime < 0 || inputstruct[0].usecTime > 10000000){
				
				
				
			}
*/			
			
			
			if (inputstruct[0].usecTime < usectime_to_wait_for_starting_operation) {
				// use this to previously start operation of slow systems ex.: toroidal field, pre-puffing (special overide place left blank for now)
				
					if (puffing_in_timewindows_mode && puffing_mode != 4){
						outputstruct[0].PuffingMode = puffing_mode;
						outputstruct[0].PuffingStatus = -2;
					} 
					else {
						outputstruct[0].PuffingMode = 1;
						outputstruct[0].PuffingStatus = 0;
					}
				
			}
			if (inputstruct[0].usecTime > usectime_to_wait_for_starting_operation + discharge_time || n_cycles_temp <= 0){
				// discharge ended
//				AssertErrorCondition(InitialisationError,"TimeWindowsGAM::Execute: discharge ended at %d us !!!",inputstruct[0].usecTime );
				
				saved_usec_time = 10000000;
				outputstruct[0].PrimaryWaveformMode = 0;
				outputstruct[0].HorizontalWaveformMode = 0;
				outputstruct[0].VerticalWaveformMode = 0;
				outputstruct[0].PuffingMode = 0;
				outputstruct[0].ToroidalMode = 0;
				outputstruct[0].DischargeStatus = -2;
				outputstruct[0].ToroidalStatus = -2;
				outputstruct[0].PuffingStatus = -2;
				last_dischage_status = -2;
				outputstruct[0].usecDischargeTime = -20000;
			}
			else {
				outputstruct[0].ToroidalMode = 1; // toroidal in absolute time always active between gamonline and end of discharge time
				//if (!puffing_in_timewindows_mode) outputstruct[0].PuffingMode = 3;
			}
		}
	}
	if (functionNumber == GAMPostpulse){
		
		saved_usec_time = 10000000;
		outputstruct[0].PrimaryWaveformMode = 0;
		outputstruct[0].HorizontalWaveformMode = 0;
		outputstruct[0].VerticalWaveformMode = 0;
		outputstruct[0].PuffingMode = 0;
		outputstruct[0].ToroidalMode = 0;
		outputstruct[0].usecDischargeTime = -20000;
		outputstruct[0].DischargeStatus = -2;
		last_dischage_status = -2;
	}
//	AssertErrorCondition(InitialisationError,"TimeWindowsGAM:: %s OUTPUTSTRUCT = %d %d %d %d %d %d",this->Name(), outputstruct[0].PrimaryWaveformMode , outputstruct[0].HorizontalWaveformMode , outputstruct[0].VerticalWaveformMode , outputstruct[0].usecDischargeTime , outputstruct[0].PlasmaDirection , outputstruct[0].DischargeStatus );
	this->SignalsOutputInterface->Write();
	
	return True;		
}

bool TimeWindowsGAM::ProcessHttpMessage(HttpStream &hStream){
	
	HtmlStream hmStream(hStream);
	int i;

	hmStream.SSPrintf(HtmlTagStreamMode, "html>\n\
		<head>\n\
		<title>%s</title>\n\
		</head>\n\
		<body>\n\
		<svg width=\"100&#37;\" height=\"100\" style=\"background-color: AliceBlue;\">\n\
		<image x=\"%d\" y=\"%d\" width=\"%d\" height=\"%d\" xlink:href=\"%s\" />\n\
		</svg", (char *) this->Name() ,0, 0, 422, 87, "http://www.ipfn.ist.utl.pt/ipfnPortalLayout/themes/ipfn/_img_/logoIPFN_Topo_officialColours.png");

	hmStream.SSPrintf(HtmlTagStreamMode, "br><br><text style=\"font-family:Arial;font-size:46\">%s</text><br", (char *) this->Name());

    FString submit_view;
    submit_view.SetSize(0);
    if (hStream.Switch("InputCommands.submit_view")){
        hStream.Seek(0);
        hStream.GetToken(submit_view, "");
        hStream.Switch((uint32)0);
    }
	if(submit_view.Size() > 0) view_input_variables = True;
	
	FString submit_hide;
    submit_hide.SetSize(0);
    if (hStream.Switch("InputCommands.submit_hide")){
        hStream.Seek(0);
        hStream.GetToken(submit_hide, "");
        hStream.Switch((uint32)0);
    }
	if(submit_hide.Size() > 0) view_input_variables = False;

	hmStream.SSPrintf(HtmlTagStreamMode, "form enctype=\"multipart/form-data\" method=\"post\"");
	if(!view_input_variables){
		hmStream.SSPrintf(HtmlTagStreamMode, "input type=\"submit\" name=\"submit_view\" value=\"View input variables\"");
	}
	else {
		hmStream.SSPrintf(HtmlTagStreamMode, "input type=\"submit\" name=\"submit_hide\" value=\"Hide input variables\"");		
		hmStream.SSPrintf(HtmlTagStreamMode, "br><br>discharge_time = %.1f us\n\
		<br>number_of_cycles = %d\n\
		<br>first_cycle_positive_bool = %d\n\
		<br>auto_breakdown = %d\n\
		<br>usecthread_cycle_time = %d\n\
		<br>usec_pre_pulse_time = %d\n\
		<br>maximum_inversion_usectime = %d\n\
		<br>usectime_to_wait_for_starting_operation = %d\n\
		<br><br", discharge_time,number_of_cycles,first_cycle_positive_bool,auto_breakdown,usecthread_cycle_time,usec_pre_pulse_time,maximum_inversion_usectime,usectime_to_wait_for_starting_operation);
		
		hmStream.SSPrintf(HtmlTagStreamMode, "table border=\"1\">\n<tr>\n<th>positive_number_of_time_windows</th>\n<th>%d</th>\n</tr>\n<tr>\n<td>positive_time_windows_values</td", positive_number_of_time_windows);
		for (i=0;i<positive_number_of_time_windows;i++)hmStream.SSPrintf(HtmlTagStreamMode, "td>%.1f</td",positive_time_windows_values[i]);
		hmStream.SSPrintf(HtmlTagStreamMode, "/tr><td>positive_primary_mode</td");
		for (i=0;i<positive_number_of_time_windows;i++)hmStream.SSPrintf(HtmlTagStreamMode, "td>%d</td",positive_primary_mode[i]);
		hmStream.SSPrintf(HtmlTagStreamMode, "/tr><td>positive_horizontal_mode</td");
		for (i=0;i<positive_number_of_time_windows;i++)hmStream.SSPrintf(HtmlTagStreamMode, "td>%d</td",positive_horizontal_mode[i]);
		hmStream.SSPrintf(HtmlTagStreamMode, "/tr><td>positive_vertical_mode</td");
		for (i=0;i<positive_number_of_time_windows;i++)hmStream.SSPrintf(HtmlTagStreamMode, "td>%d</td",positive_vertical_mode[i]);

		hmStream.SSPrintf(HtmlTagStreamMode, "/tr></table><br");

		hmStream.SSPrintf(HtmlTagStreamMode, "table border=\"1\">\n<tr>\n<th>negative_number_of_time_windows</th>\n<th>%d</th>\n</tr>\n<tr>\n<td>negative_time_windows_values</td", negative_number_of_time_windows);
		for (i=0;i<negative_number_of_time_windows;i++)hmStream.SSPrintf(HtmlTagStreamMode, "td>%.1f</td",negative_time_windows_values[i]);
		hmStream.SSPrintf(HtmlTagStreamMode, "/tr><td>negative_primary_mode</td");
		for (i=0;i<negative_number_of_time_windows;i++)hmStream.SSPrintf(HtmlTagStreamMode, "td>%d</td",negative_primary_mode[i]);
		hmStream.SSPrintf(HtmlTagStreamMode, "/tr><td>negative_horizontal_mode</td");
		for (i=0;i<negative_number_of_time_windows;i++)hmStream.SSPrintf(HtmlTagStreamMode, "td>%d</td",negative_horizontal_mode[i]);
		hmStream.SSPrintf(HtmlTagStreamMode, "/tr><td>negative_vertical_mode</td");
		for (i=0;i<negative_number_of_time_windows;i++)hmStream.SSPrintf(HtmlTagStreamMode, "td>%d</td",negative_vertical_mode[i]);

		hmStream.SSPrintf(HtmlTagStreamMode, "/tr></table><br><br");
	}
	hmStream.SSPrintf(HtmlTagStreamMode, "/form");
	
	hmStream.SSPrintf(HtmlTagStreamMode, "/body>\n</html");
	hStream.SSPrintf("OutputHttpOtions.Content-Type","text/html;charset=utf-8");
	hStream.WriteReplyHeader(True);

	return True;		
}
