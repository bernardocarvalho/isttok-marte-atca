/*
 * File:   ElectrodeBiasingGAM.cpp (based on HeavyIonBeamGAM.cpp)
 * Author: Rafael Henriques
 * Date:   7th May 2016
 * 
 */

#include "ElectrodeBiasingGAM.h"

OBJECTLOADREGISTER(ElectrodeBiasingGAM, "$Id: $")


//  ******** Default constructor ***********************************
ElectrodeBiasingGAM::ElectrodeBiasingGAM(){
	this->SignalsInputInterface = NULL;
	this->SignalsOutputInterface = NULL;
}

// ********* Destructor ********************************************
ElectrodeBiasingGAM::~ElectrodeBiasingGAM()
{
	//if(this->SignalsInputInterface != NULL) delete[] this->SignalsInputInterface ;
	//if(this->SignalsOutputInterface != NULL) delete[] this->SignalsOutputInterface;
}

//{ ********* Initialise the module ********************************
bool ElectrodeBiasingGAM::Initialise(ConfigurationDataBase& cdbData){
   
	CDBExtended cdb(cdbData);
	
	int i;

//Create the signal interfaces
	if(!AddInputInterface(this->SignalsInputInterface, "ElectrodeBiasingGAMInputInterface"))
	{
		AssertErrorCondition(InitialisationError, "ElectrodeBiasingGAM::Initialise: %s failed to add the ElectrodeBiasingGAMInputInterface", this->Name());
		return False;
	}
	if(!AddOutputInterface(this->SignalsOutputInterface, "ElectrodeBiasingGAMOutputInterface"))
	{
		AssertErrorCondition(InitialisationError, "ElectrodeBiasingGAM::Initialise: %s failed to add the ElectrodeBiasingGAMOutputInterface", this->Name());
		return False;
	}

//INPUT SIGNALS (interface)
	if(!cdb->Move("input_signals"))
	{
		AssertErrorCondition(InitialisationError,"ElectrodeBiasingGAM::Initialise: %s Could not move to \"input_signals\"",this->Name());
		return False;
	}
	
		int number_of_signals_to_read = 1;
		FString *CDB_move_to;
		FString *SignalType;
		CDB_move_to = new FString[number_of_signals_to_read];
		SignalType = new FString[number_of_signals_to_read];
		CDB_move_to[number_of_signals_to_read-1].Printf("discharge_status");
		for (i=0;i<number_of_signals_to_read;i++){
			
			if(!cdb->Move(CDB_move_to[i].Buffer()))
			{
				AssertErrorCondition(InitialisationError, "ElectrodeBiasingGAM::Initialise: %s Could not move to \"%s\"", this->Name(), CDB_move_to[i].Buffer());
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
				AssertErrorCondition(Information,"ElectrodeBiasingGAM::Initialise: Added signal = %s", SignalName.Buffer());
				
				if(!this->SignalsInputInterface->AddSignal(SignalName.Buffer(), SignalType[i].Buffer()))
				{
					AssertErrorCondition(InitialisationError,"ElectrodeBiasingGAM::Initialise: %s failed to add signal", this->Name());
					return False;
				}
			}
			cdb->MoveToFather();
		}
	cdb->MoveToFather();


//OUTPUT SIGNALS (interface)
	if(!cdb->Move("output_signals"))
	{
		AssertErrorCondition(InitialisationError,"ElectrodeBiasingGAM::Initialise: %s Could not move to \"output_signals\"",this->Name());
		return False;
	}
	
		int number_of_signals_to_write = 1;
		CDB_move_to = new FString[number_of_signals_to_write];
		SignalType = new FString[number_of_signals_to_write];
		CDB_move_to[0].Printf("eb_trigger");
		for (i=0;i<number_of_signals_to_write;i++){
			
			if(!cdb->Move(CDB_move_to[i].Buffer()))
			{
				AssertErrorCondition(InitialisationError,"ElectrodeBiasingGAM::Initialise: %s Could not move to \"%s\"",this->Name(),CDB_move_to[i].Buffer());
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
				AssertErrorCondition(Information,"ElectrodeBiasingGAM::Initialise: Added signal = %s", SignalName.Buffer());
				
				if(!this->SignalsOutputInterface->AddSignal(SignalName.Buffer(), SignalType[i].Buffer()))
				{
					AssertErrorCondition(InitialisationError,"ElectrodeBiasingGAM::Initialise: %s failed to add signal", this->Name());
					return False;
				}
			}
			cdb->MoveToFather();
		}
	cdb->MoveToFather();
	
	//Initialization
	dischargestatus_old = 0;
	
	return True;
}
//} ****************************************************************


//{ ********* Execute the module functionalities *******************
bool ElectrodeBiasingGAM::Execute(GAM_FunctionNumbers functionNumber){
 
	InputInterfaceStruct *inputstruct = (InputInterfaceStruct *) this->SignalsInputInterface->Buffer();
	this->SignalsInputInterface->Read();

	OutputInterfaceStruct *outputstruct = (OutputInterfaceStruct *) this->SignalsOutputInterface->Buffer();
	
	int dischargestatus;
	
	if(functionNumber == GAMOnline){
		
		dischargestatus = inputstruct[0].DischargeStatus;
		
		if(dischargestatus_old == 0 && dischargestatus == 1) outputstruct[0].ElectrodeBiasingTrigger = 1.0;
		if(dischargestatus_old == 0 && dischargestatus != 1) outputstruct[0].ElectrodeBiasingTrigger = 0.0;
		if(dischargestatus_old == 1 && dischargestatus == 1) outputstruct[0].ElectrodeBiasingTrigger = 1.0;
		if(dischargestatus_old == 1 && dischargestatus != 1 && dischargestatus != 2) outputstruct[0].ElectrodeBiasingTrigger = 0;
		if(dischargestatus_old == 1 && dischargestatus == 2) outputstruct[0].ElectrodeBiasingTrigger = 0.0;
		if(dischargestatus_old == 2 && dischargestatus == 1) outputstruct[0].ElectrodeBiasingTrigger = 1.0;
		if(dischargestatus_old == 2 && dischargestatus != 1) outputstruct[0].ElectrodeBiasingTrigger = 0.0;
		
		dischargestatus_old = dischargestatus;
		
	}
	else {
		
		outputstruct[0].ElectrodeBiasingTrigger = 0.0;
		
	}
		
	this->SignalsOutputInterface->Write();
	
	return True;		
}
