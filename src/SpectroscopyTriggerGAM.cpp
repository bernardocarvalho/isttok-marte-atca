/*
 * File:   SpectroscopyTriggerGAM.cpp (based on SpectroscopyTriggerGAM.cpp)
 * Author: Hugo Alves
 * Date:   20th Feb 2017
 * 
 */

#include "SpectroscopyTriggerGAM.h"

OBJECTLOADREGISTER(SpectroscopyTriggerGAM, "$Id: $")


//  ******** Default constructor ***********************************
SpectroscopyTriggerGAM::SpectroscopyTriggerGAM(){
	this->SignalsInputInterface = NULL;
	this->SignalsOutputInterface = NULL;
}

// ********* Destructor ********************************************
SpectroscopyTriggerGAM::~SpectroscopyTriggerGAM()
{
	//if(this->SignalsInputInterface != NULL) delete[] this->SignalsInputInterface ;
	//if(this->SignalsOutputInterface != NULL) delete[] this->SignalsOutputInterface;
}

//{ ********* Initialise the module ********************************
bool SpectroscopyTriggerGAM::Initialise(ConfigurationDataBase& cdbData){
   
	CDBExtended cdb(cdbData);
	
	int i;
//Read config
	//if(!cdb->Move("trigger_config"))
	//{
	//	AssertErrorCondition(InitialisationError,"SpectroscopyTriggerGAM::Initialise: %s Could not move to \"trigger_config\"",this->Name());
	//	return False;
	//}

	if(!cdb.ReadInt32(min_time_between_triggers_usec, "min_time_between_triggers_usec"))
	{
		AssertErrorCondition(InitialisationError, "SpectroscopyTriggerGAM::Initialise: %s min_time_between_triggers_usec", this->Name());
		return False;
	}
	else AssertErrorCondition(Information, "SpectroscopyTriggerGAM::Initialise: min_time_between_triggers_usec = %d", min_time_between_triggers_usec);
//Create the signal interfaces
	if(!AddInputInterface(this->SignalsInputInterface, "SpectroscopyTriggerGAMInputInterface"))
	{
		AssertErrorCondition(InitialisationError, "SpectroscopyTriggerGAM::Initialise: %s failed to add the SpectroscopyTriggerGAMInputInterface", this->Name());
		return False;
	}
	if(!AddOutputInterface(this->SignalsOutputInterface, "SpectroscopyTriggerGAMOutputInterface"))
	{
		AssertErrorCondition(InitialisationError, "SpectroscopyTriggerGAM::Initialise: %s failed to add the SpectroscopyTriggerGAMOutputInterface", this->Name());
		return False;
	}

//INPUT SIGNALS (interface)
	if(!cdb->Move("input_signals"))
	{
		AssertErrorCondition(InitialisationError,"SpectroscopyTriggerGAM::Initialise: %s Could not move to \"input_signals\"",this->Name());
		return False;
	}
	
		int number_of_signals_to_read = 3;
		FString *CDB_move_to;
		FString *SignalType;
		CDB_move_to = new FString[number_of_signals_to_read];
		SignalType = new FString[number_of_signals_to_read];
		CDB_move_to[0].Printf("DischargeStatus");
		CDB_move_to[1].Printf("usecDischargeTime");
		CDB_move_to[2].Printf("usecTime");
		for (i=0;i<number_of_signals_to_read;i++){
			
			if(!cdb->Move(CDB_move_to[i].Buffer()))
			{
				AssertErrorCondition(InitialisationError, "SpectroscopyTriggerGAM::Initialise: %s Could not move to \"%s\"", this->Name(), CDB_move_to[i].Buffer());
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
				AssertErrorCondition(Information,"SpectroscopyTriggerGAM::Initialise: Added signal = %s", SignalName.Buffer());
				
				if(!this->SignalsInputInterface->AddSignal(SignalName.Buffer(), SignalType[i].Buffer()))
				{
					AssertErrorCondition(InitialisationError,"SpectroscopyTriggerGAM::Initialise: %s failed to add signal", this->Name());
					return False;
				}
			}
			cdb->MoveToFather();
		}
	cdb->MoveToFather();


//OUTPUT SIGNALS (interface)
	if(!cdb->Move("output_signals"))
	{
		AssertErrorCondition(InitialisationError,"SpectroscopyTriggerGAM::Initialise: %s Could not move to \"output_signals\"",this->Name());
		return False;
	}
	
		int number_of_signals_to_write = 1;
		CDB_move_to = new FString[number_of_signals_to_write];
		SignalType = new FString[number_of_signals_to_write];
		CDB_move_to[0].Printf("spectr_trigger");
		for (i=0;i<number_of_signals_to_write;i++){
			
			if(!cdb->Move(CDB_move_to[i].Buffer()))
			{
				AssertErrorCondition(InitialisationError,"SpectroscopyTriggerGAM::Initialise: %s Could not move to \"%s\"",this->Name(),CDB_move_to[i].Buffer());
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
				AssertErrorCondition(Information,"SpectroscopyTriggerGAM::Initialise: Added signal = %s", SignalName.Buffer());
				
				if(!this->SignalsOutputInterface->AddSignal(SignalName.Buffer(), SignalType[i].Buffer()))
				{
					AssertErrorCondition(InitialisationError,"SpectroscopyTriggerGAM::Initialise: %s failed to add signal", this->Name());
					return False;
				}
			}
			cdb->MoveToFather();
		}
	cdb->MoveToFather();
	
	// Initialization
	old_elapsedtime = -100000;  // Do not skip first semicycle
	
	return True;
}
//} ****************************************************************


//{ ********* Execute the module functionalities *******************
bool SpectroscopyTriggerGAM::Execute(GAM_FunctionNumbers functionNumber){
 
	InputInterfaceStruct *inputstruct = (InputInterfaceStruct *) this->SignalsInputInterface->Buffer();
	this->SignalsInputInterface->Read();

	OutputInterfaceStruct *outputstruct = (OutputInterfaceStruct *) this->SignalsOutputInterface->Buffer();
	
	int dischargestatus;    		// 0 : breakdown, 1 : normal, 2 : inversion
	int disch_usec;				// usec since last change in dischargestatus
	int usec;				// total usec since plasma beginning
	int usec_since_last_trigger;		// usec since last trigger (no, really.)
	
	if(functionNumber == GAMOnline){
		
		dischargestatus = inputstruct[0].DischargeStatus;
		disch_usec = inputstruct[0].usecDischargeTime;
		usec = inputstruct[0].usecTime;
		usec_since_last_trigger = usec - old_elapsedtime;

		// falling edge
		if(dischargestatus == 1 && usec_since_last_trigger > 2000) outputstruct[0].SpectroscopyTrigger = 0.0;

		// send a trigger in the beginning of a semicycle if it's not too early 
		if(dischargestatus == 1 && disch_usec < 2000 && usec_since_last_trigger > this->min_time_between_triggers_usec ){
			outputstruct[0].SpectroscopyTrigger = 100.0;
			old_elapsedtime = usec;
		}
		if(dischargestatus != 1) outputstruct[0].SpectroscopyTrigger = 0.0;
		
	}
	else {
		
		outputstruct[0].SpectroscopyTrigger = 0.0;
		old_elapsedtime = -100000;
	}
		
	this->SignalsOutputInterface->Write();
	
	return True;		
}
