/*
 * File:   FireSignalDischargeStatusGAM.cpp
 * Author: ivoc
 *
 */

#include "FireSignalDischargeStatusGAM.h"


OBJECTLOADREGISTER(FireSignalDischargeStatusGAM, "$Id: $")


//
// Default constructor
//
FireSignalDischargeStatusGAM::FireSignalDischargeStatusGAM(){
	this->SignalsInputInterface = NULL;

	this->plasmaEnded = False;
}


//
// Destructor
//
FireSignalDischargeStatusGAM::~FireSignalDischargeStatusGAM(){
}


//
// Initialise the module
//
bool FireSignalDischargeStatusGAM::Initialise(ConfigurationDataBase& cdbData)
{
	CDBExtended cdb(cdbData);

	if(!AddInputInterface(this->SignalsInputInterface, "FireSignalDischargeStatusGAMInputInterface"))
	{
		AssertErrorCondition(InitialisationError, "FireSignalDischargeStatusGAM::Initialise: %s failed to add the FireSignalDischargeStatusGAMInputInterface", this->Name());
		return False;
	}

//	INPUT SIGNALS (interface)
	if(!cdb->Move("input_signals"))
	{
		AssertErrorCondition(InitialisationError,"FireSignalDischargeStatusGAM::Initialise: %s Could not move to \"input_signals\"",this->Name());
		return False;
	}
		int number_of_signals_to_read = 2;
		FString *CDB_move_to;
		FString *SignalType;
		CDB_move_to = new FString[number_of_signals_to_read];
		SignalType = new FString[number_of_signals_to_read];
        CDB_move_to[0].Printf("discharge_status");
		CDB_move_to[1].Printf("system_time");
		for (i=0;i<number_of_signals_to_read;i++){
			
			if(!cdb->Move(CDB_move_to[i].Buffer()))
			{
				AssertErrorCondition(InitialisationError,"FireSignalDischargeStatusGAM::Initialise: %s Could not move to \"%s\"",this->Name(),CDB_move_to[i].Buffer());
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
				AssertErrorCondition(Information,"FireSignalDischargeStatusGAM::Initialise: Added signal = %s", SignalName.Buffer());
				
				if(!this->SignalsInputInterface->AddSignal(SignalName.Buffer(), SignalType[i].Buffer()))
				{
					AssertErrorCondition(InitialisationError,"FireSignalDischargeStatusGAM::Initialise: %s failed to add signal", this->Name());
					return False;
				}
			}
			
			
			cdb->MoveToFather();
		}

	cdb->MoveToFather();

	this->plasmaEnded = False;
	this->PlasmaStarted = False;

	return True;
}


//
// Execute the module functionalities
//
bool FireSignalDischargeStatusGAM::Execute(GAM_FunctionNumbers functionNumber)
{

	InputInterfaceStruct *inputstruct = (InputInterfaceStruct *) this->SignalsInputInterface->Buffer();
	this->SignalsInputInterface->Read();
	
	//
	// Reset plasma ended trigger
	//
	
	if(functionNumber == GAMOffline)
	{
		this->plasmaEnded = False;
		this->plasmaEndedTimeMarked = False;
		this->PlasmaStarted = False;
	}
	
	if(functionNumber == GAMPrepulse)
	{
		this->plasmaEnded = False;
		this->plasmaEndedTimeMarked = False;
		this->PlasmaStarted = False;
	}
	
	if(inputstruct[0].DischargeStatus >= 0) this->PlasmaStarted = True;
	
	
//	if (inputstruct[0].DischargeStatus >= 0) AssertErrorCondition(InitialisationError,"FireSignalDischargeStatusGAM::Execute: DischargeStatus = %d !!!",inputstruct[0].DischargeStatus );
	if((inputstruct[0].DischargeStatus < 0) && (!this->plasmaEnded) && (this->PlasmaStarted)){
		
		this->plasmaEnded = True;
		this->PlasmaStarted = False;
		AssertErrorCondition(InitialisationError,"FireSignalDischargeStatusGAM::Execute: PLASMA ENDED at %d !!!", inputstruct[0].usecTime);
		
		if(!this->plasmaEndedTimeMarked){
			
			this->plasmaEndedTime = inputstruct[0].usecTime;
			this->plasmaEndedTimeMarked = True;
		}
	}

	return True;
}


//
// Builds the HTTP page with information about the CODAC
//
bool FireSignalDischargeStatusGAM::ProcessHttpMessage(HttpStream &hStream){
	
	hStream.SSPrintf("OutputHttpOtions.Content-Type","text/html");
	hStream.keepAlive = False;
	//copy to the client
	hStream.WriteReplyHeader(False);

	hStream.Printf("<html><head><title>CODAC GAM</title></head><body>");
	hStream.Printf("<h1>Information:</h1><br />");

	//
	// Plasma ended flag
	//
	if(this->plasmaEnded)
		hStream.Printf("<p>Plasma ended: True</p>");
	else
		hStream.Printf("<p>Plasma ended: False</p>");
	hStream.Printf("<p>Plasma ended time: %d</p>", this->plasmaEndedTime);

	hStream.Printf("</body></html>");

	return True;
}
