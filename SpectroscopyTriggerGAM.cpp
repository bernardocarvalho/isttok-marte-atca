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
	
		int number_of_signals_to_read = 2;
		FString *CDB_move_to;
		FString *SignalType;
		CDB_move_to = new FString[number_of_signals_to_read];
		SignalType = new FString[number_of_signals_to_read];
		CDB_move_to[0].Printf("discharge_status");
		CDB_move_to[1].Printf("usec_discharge_time");
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
	
	
	return True;
}
//} ****************************************************************


//{ ********* Execute the module functionalities *******************
bool SpectroscopyTriggerGAM::Execute(GAM_FunctionNumbers functionNumber){
 
	InputInterfaceStruct *inputstruct = (InputInterfaceStruct *) this->SignalsInputInterface->Buffer();
	this->SignalsInputInterface->Read();

	OutputInterfaceStruct *outputstruct = (OutputInterfaceStruct *) this->SignalsOutputInterface->Buffer();
	
	int dischargestatus;
	int usec;
	
	if(functionNumber == GAMOnline){
		
		dischargestatus = inputstruct[0].DischargeStatus;
		usec = inputstruct[0].usecDischargeTime;
		
		if(dischargestatus == 1 && usec < 2000) outputstruct[0].SpectroscopyTrigger = 100.0;
		if(dischargestatus == 1 && usec > 2000) outputstruct[0].SpectroscopyTrigger = 0.0;
		if(dischargestatus != 1) outputstruct[0].SpectroscopyTrigger = 0.0;
		
	}
	else {
		
		outputstruct[0].SpectroscopyTrigger = 0.0;
		
	}
		
	this->SignalsOutputInterface->Write();
	
	return True;		
}
