/********************************************************************************
* FakeSignalSourceGAM developed for ISTTOK ATCA Slave Board Emulation		*
* FakeSignalSourceGAM.cpp							*
*										*
* SVN keywords									*
* $Date:  $									*
* $Revision: 0.2 $								*
* $URL:  $									*
*										*
********************************************************************************/

/****************************************************************
*								*
* Copyright 2011 EFDA | European Fusion Development Agreement	*
*								*
* Licensed under the EUPL, Version 1.1 or - as soon they	*
* will be approved by the European Commission - subsequent	*
* versions of the EUPL (the "Licence"); 			*
* You may not use this work except in compliance with the	*
* Licence.							*
* You may obtain a copy of the Licence at:			*
*								*
* http://ec.europa.eu/idabc/eupl				*
*								*
* Unless required by applicable law or agreed to in		*
* writing, software distributed under the Licence is		*
* distributed on an "AS IS" basis,				*
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either		*
* express or implied.						*
* See the Licence for the specific language governing		*
* permissions and limitations under the Licence.		*
*								*
* $Id: FakeSignalSourceGAM.cpp $				*
*								*
****************************************************************/

/************************************************
* IPFN-IST PDL 2019				*
* plourenco@ipfn.tecnico.ulisboa.pt		*
* N. B.: Nothing to comment			*
************************************************/

#include "FakeSignalSourceGAM.h"

/**
* GAM Initialise Function
**/
bool FakeSignalSourceGAM::Initialise(ConfigurationDataBase& cdbData){

	CDBExtended cdb(cdbData);

	////////////////////////////////////////////////////
	//                Add interfaces to DDB           //
	////////////////////////////////////////////////////
	if(!AddInputInterface(this->SignalsInputInterface, "FakeSignalSourceGAMInputInterface")){
		AssertErrorCondition(InitialisationError, "FakeSignalSourceGAM::Initialise: %s failed to add the FakeSignalSourceGAMInputInterface", this->Name());
		return False;
	}
	if(!AddOutputInterface(this->SignalsOutputInterface, "FakeSignalSourceGAMOutputInterface")){
		AssertErrorCondition(InitialisationError, "FakeSignalSourceGAM::Initialise: %s failed to add the FakeSignalSourceGAMOutputInterface", this->Name());
		return False;
	}

	////////////////////////////////////////////////////
	//                Add input signals               //
	////////////////////////////////////////////////////
	if(!cdb->Move("input_signals")){
		AssertErrorCondition(InitialisationError,"FakeSignalSourceGAM::Initialise: %s did not specify \"input_signals\" entry", Name());
		return False;
	}
	signalNumber = (uint32)cdb->NumberOfChildren();
	if(signalNumber != 1){
		AssertErrorCondition(InitialisationError, "FakeSignalSourceGAM::Initialise: %s FakeSignalSourceGAM expects the usecTime", Name());
		return False;
	}
	if(!SignalsInputInterface->ObjectLoadSetup(cdb,NULL)){
		AssertErrorCondition(InitialisationError,"FakeSignalSourceGAM::Initialise: %s ObjectLoadSetup Failed DDBInterface %s ", Name(), SignalsInputInterface->InterfaceName());
		return False;
	}
	cdb->MoveToFather();

	////////////////////////////////////////////////////
	//                Add output signals              //
	////////////////////////////////////////////////////
	if(!cdb->Move("output_signals")){
		AssertErrorCondition(InitialisationError,"FakeSignalSourceGAM::Initialise: %s did not specify \"output_signals\" entry",Name());
		return False;
	}
	signalNumber = (uint32)cdb->NumberOfChildren();
	if(signalNumber < 1){
		AssertErrorCondition(InitialisationError,"FakeSignalSourceGAM::Initialise: %s At least one output is provided by this GAM ",Name());
		return False;
	}
	if(!SignalsOutputInterface->ObjectLoadSetup(cdb,NULL)){
		AssertErrorCondition(InitialisationError,"FakeSignalSourceGAM::Initialise: %s ObjectLoadSetup Failed DDBInterface %s ",Name(),SignalsOutputInterface->InterfaceName());
		return False;
	}
	cdb->MoveToFather();

	////////////////////////////////////////////////////
	//            Configuration parameters            //
	////////////////////////////////////////////////////

	//signalType
	if(!cdb.ReadFString(signalType, "SignalType")){
		AssertErrorCondition(InitialisationError,"FakeSignalSourceGAM::Initialise: %s SignalType must be specified", Name());
		return False;
	}
	if(signalType != "float" && signalType != "int32" &&  signalType != "uint32"){
		AssertErrorCondition(InitialisationError,"FakeSignalSourceGAM::Initialise: %s Unsupported SignalType requested: %s, use float, inst32 or uint32", Name(), signalType.Buffer());
		return False;
	}

	//signalValue
	if(signalType == "float"){
		if(!cdb.ReadFloat( signalValueFloat, "SignalValue")){
			AssertErrorCondition(InitialisationError, "FakeSignalSourceGAM::Initialise: %s No SignalValue was set", Name());
			return False;
		}
		nBytes = signalNumber*sizeof(float);
		arrayDataFloat = (float *)STDMalloc(nBytes);
		for (uint32 i = 0; i < signalNumber; i++){
			*(arrayDataFloat+i) = signalValueFloat;
		}
	}
	else if (signalType == "int32") {
		if(!cdb.ReadInt32( signalValueInt32, "SignalValue")){
			AssertErrorCondition(InitialisationError, "FakeSignalSourceGAM::Initialise: %s No SignalValue was set", Name());
			return False;
		}
		nBytes = signalNumber*sizeof(int32);
		arrayDataInt32 = (int32 *)STDMalloc(nBytes);
		for (uint32 i = 0; i < signalNumber; i++){
			*(arrayDataInt32+i) = signalValueInt32;
		}
	}
	else {
		if(!cdb.ReadUint32( signalValueUint32, "SignalValue")){
			AssertErrorCondition(InitialisationError, "FakeSignalSourceGAM::Initialise: %s No SignalValue was set", Name());
			return False;
		}
		nBytes = signalNumber*sizeof(uint32);
		arrayDataUint32 = (uint32 *)STDMalloc(nBytes);
		for (uint32 i = 0; i < signalNumber; i++){
			*(arrayDataUint32+i) = signalValueUint32;
		}
	}

	AssertErrorCondition(Information,"FakeSignalSourceGAM::Initialise: %s GAM initialization finished.", Name());
	return True;
}

/**
* GAM Execute Function
**/
bool FakeSignalSourceGAM::Execute(GAM_FunctionNumbers functionNumber){

	// Get input and output data pointers
	this->SignalsInputInterface->Read();
	int32 usecTime			= *((int32*)SignalsInputInterface->Buffer());
	float *outputBuffFloat		= (float*)SignalsOutputInterface->Buffer();
	int32 *outputBuffInt32		= (int32*)SignalsOutputInterface->Buffer();
	uint32 *outputBuffUint32	= (uint32*)SignalsOutputInterface->Buffer();

	//Copy pre calculated data
	if(signalType == "float"){
		memcpy(outputBuffFloat, arrayDataFloat, nBytes);
	}
	else if (signalType == "int32"){
		memcpy(outputBuffInt32, arrayDataInt32, nBytes);
	}
	else{
		memcpy(outputBuffUint32, arrayDataUint32, nBytes);
	}

	// Update the data output buffer
	this->SignalsOutputInterface->Write();

	return True;
}

/**
* GAM HTTP Messaging Function
**/
bool FakeSignalSourceGAM::ProcessHttpMessage(HttpStream &hStream) {
	hStream.SSPrintf("OutputHttpOtions.Content-Type","text/html");
	hStream.keepAlive = False;
	hStream.WriteReplyHeader(False);
	hStream.Printf("<html><head><title>%s</title>", Name());
	hStream.Printf( "</style></head><body>\n" );
	hStream.Printf("<table class=\"bltable\">\n");
	hStream.Printf("<tr><th style=\"text-align:center;background-color:orange\" colspan=\"2\">FakeSignalSourceGAM</th></tr>\n");
	hStream.Printf("<tr><th style=\"text-align:center\">Parameter</th><th style=\"text-align:center\">Value</th></tr>\n");
	hStream.Printf("<tr><td>SignalType</td><td>%s</td></tr>\n", signalType.Buffer());
	hStream.Printf("<tr><td>SignalNumber</td><td>%d</td></tr>\n", signalNumber);
	if(signalType == "float"){
    	hStream.Printf("<tr><td>SignalValue</td><td>%.3f</td></tr>\n", signalValueFloat);
	}
	else if (signalType == "int32"){
    	hStream.Printf("<tr><td>SignalValue</td><td>%d</td></tr>\n", signalValueInt32);
	}
	else{
    	hStream.Printf("<tr><td>SignalValue</td><td>%u</td></tr>\n", signalValueUint32);
	}
	hStream.Printf("<tr><td>Number Bytes</td><td>%u</td></tr>\n", nBytes);
    hStream.Printf("</body></html>");
    hStream.WriteReplyHeader(True);
    return True;
}

OBJECTLOADREGISTER(FakeSignalSourceGAM, "$Id: FakeSignalSourceGAM$")
