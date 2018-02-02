
#include "MachineProtectionGAM.h"

OBJECTLOADREGISTER(MachineProtectionGAM, "$Id: $")


//  ******** Default constructor ***********************************
MachineProtectionGAM::MachineProtectionGAM(){
	this->SignalsInputInterface = NULL;
	this->SignalsOutputInterface = NULL;
}




// ********* Destructor ********************************************
MachineProtectionGAM::~MachineProtectionGAM()
{
}



//{ ********* Initialise the module ********************************
bool MachineProtectionGAM::Initialise(ConfigurationDataBase& cdbData){
   
	CDBExtended cdb(cdbData);
	int i;
	
	if(!cdb.ReadFloat(iron_core_saturation_value, "iron_core_saturation_value"))
	{
		CStaticAssertErrorCondition(InitialisationError,"MachineProtectionGAM::Initialise: %s iron_core_saturation_value",this->Name());
		return False;
	}
	else CStaticAssertErrorCondition(Information,"MachineProtectionGAM::Initialise: iron_core_saturation_value = %f",iron_core_saturation_value);
	if(!cdb.ReadFloat(iron_core_dangerous_value, "iron_core_dangerous_value"))
	{
		CStaticAssertErrorCondition(InitialisationError,"MachineProtectionGAM::Initialise: %s iron_core_dangerous_value",this->Name());
		return False;
	}
	else CStaticAssertErrorCondition(Information,"MachineProtectionGAM::Initialise: iron_core_dangerous_value = %f",iron_core_dangerous_value);


//	sleep(3);
// Create the signal interfaces
	if(!AddInputInterface(this->SignalsInputInterface, "MachineProtectionGAMInputInterface"))
	{
		AssertErrorCondition(InitialisationError, "MachineProtectionGAM::Initialise: %s failed to add the TimewindowsGAMInputInterface", this->Name());
		return False;
	}
	if(!AddOutputInterface(this->SignalsOutputInterface, "MachineProtectionGAMOutputInterface"))
	{
		AssertErrorCondition(InitialisationError, "MachineProtectionGAM::Initialise: %s failed to add the TimewindowsGAMOutputInterface", this->Name());
		return False;
	}

//	INPUT SIGNALS (interface)
	if(!cdb->Move("input_signals"))
	{
		AssertErrorCondition(InitialisationError,"MachineProtectionGAM::Initialise: %s Could not move to \"input_signals\"",this->Name());
		return False;
	}
		int number_of_signals_to_read = 12;
		FString *CDB_move_to;
		FString *SignalType;
		CDB_move_to = new FString[number_of_signals_to_read];
		SignalType = new FString[number_of_signals_to_read];
		CDB_move_to[0].Printf("plasma_current");
		CDB_move_to[1].Printf("horizontal_current");
		CDB_move_to[2].Printf("vertical_current");
		CDB_move_to[3].Printf("primary_current");
		CDB_move_to[4].Printf("iron_core_saturation");
		CDB_move_to[5].Printf("v_loop");
		CDB_move_to[6].Printf("density");
		CDB_move_to[7].Printf("h_alpha");
		CDB_move_to[8].Printf("time");
		CDB_move_to[9].Printf("soft_stop_primary");
		CDB_move_to[10].Printf("soft_stop_vertical");
		CDB_move_to[11].Printf("soft_stop_horizontal");
		for (i=0;i<number_of_signals_to_read;i++){
			
			if(!cdb->Move(CDB_move_to[i].Buffer()))
			{
				AssertErrorCondition(InitialisationError,"MachineProtectionGAM::Initialise: %s Could not move to \"%s\"",this->Name(),CDB_move_to[i].Buffer());
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
				AssertErrorCondition(Information,"MachineProtectionGAM::Initialise: Added signal = %s", SignalName.Buffer());
				
				if(!this->SignalsInputInterface->AddSignal(SignalName.Buffer(), SignalType[i].Buffer()))
				{
					AssertErrorCondition(InitialisationError,"MachineProtectionGAM::Initialise: %s failed to add signal", this->Name());
					return False;
				}
			}
			
			cdb->MoveToFather();
		}

	cdb->MoveToFather();


//	OUTPUT SIGNALS (interface)
	if(!cdb->Move("output_signals"))
	{
		AssertErrorCondition(InitialisationError,"MachineProtectionGAM::Initialise: %s Could not move to \"output_signals\"",this->Name());
		return False;
	}
	
		number_of_signals_to_read = 3;
		CDB_move_to = new FString[number_of_signals_to_read];
		SignalType = new FString[number_of_signals_to_read];
		CDB_move_to[0].Printf("saturated_bool");
		CDB_move_to[1].Printf("slow_stop");
		CDB_move_to[2].Printf("hard_stop");
		for (i=0;i<number_of_signals_to_read;i++){
			
			if(!cdb->Move(CDB_move_to[i].Buffer()))
			{
				AssertErrorCondition(InitialisationError,"MachineProtectionGAM::Initialise: %s Could not move to \"%s\"",this->Name(),CDB_move_to[i].Buffer());
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
				AssertErrorCondition(Information,"MachineProtectionGAM::Initialise: Added signal = %s", SignalName.Buffer());
				
				if(!this->SignalsOutputInterface->AddSignal(SignalName.Buffer(), SignalType[i].Buffer()))
				{
					AssertErrorCondition(InitialisationError,"MachineProtectionGAM::Initialise: %s failed to add signal", this->Name());
					return False;
				}
			}
			cdb->MoveToFather();
		}

	cdb->MoveToFather();

	calculated_saturation_value = 0;
	assert_number_of_samples_in_saturation_1 = 0;
	assert_number_of_samples_in_saturation_2 = 0;

	// number os samples necessary to assert InSaturation
	assert_saturation_limit1 = 6;
	assert_saturation_limit2 = 6;

	return True;
}
//} ******************************************************************



//{ ********* Execute the module functionalities *******************
bool MachineProtectionGAM::Execute(GAM_FunctionNumbers functionNumber){

	InputInterfaceStruct *inputstruct = (InputInterfaceStruct *) this->SignalsInputInterface->Buffer();
	this->SignalsInputInterface->Read();
//	AssertErrorCondition(InitialisationError,"MachineProtectionGAM:: %s inputstruct = %d %d %d %d %d %f %f %f %f %f %f %f %f ",this->Name(), inputstruct[0].usecTime, inputstruct[0].SoftStopPrimaryPS, inputstruct[0].SoftStopVerticalPS, inputstruct[0].SoftStopHorizontalPS, inputstruct[0].SoftStopPuffing, inputstruct[0].PlasmaCurrent, inputstruct[0].HorizontalCurrent, inputstruct[0].VerticalCurrent, inputstruct[0].PrimaryCurrent, inputstruct[0].IronCoreSaturation, inputstruct[0].VLoop, inputstruct[0].Density, inputstruct[0].HAlfa);

	OutputInterfaceStruct *outputstruct = (OutputInterfaceStruct *) this->SignalsOutputInterface->Buffer();
	

	if(functionNumber == GAMOnline){

		//place to insert additional protections
		inputstruct[0].PlasmaCurrent;
		inputstruct[0].PrimaryCurrent;
		inputstruct[0].IronCoreSaturation;

		//if |Iprim| > 25 
		if (inputstruct[0].PrimaryCurrent * inputstruct[0].PrimaryCurrent > 625){
			// second methode for iron core saturation predictor, calculated_saturation_value = 28 * 28 * (Iprim * Iprim) / (Ip * Ip + 280 * 280), good threshold = 1.25
			calculated_saturation_value = 784 * inputstruct[0].PrimaryCurrent * inputstruct[0].PrimaryCurrent / (inputstruct[0].PlasmaCurrent * inputstruct[0].PlasmaCurrent + 280 * 280);
		}
		else calculated_saturation_value = 0;
		
		if ( inputstruct[0].IronCoreSaturation > iron_core_dangerous_value) {
			assert_number_of_samples_in_saturation_1++ ;  // HALVES
			// AssertErrorCondition(InitialisationError,"MachineProtectionGAM::Execute: Consecutive Saturated1 cycles %d/6",assert_number_of_samples_in_saturation_1);
		}
		else assert_number_of_samples_in_saturation_1 = 0;

		if ( calculated_saturation_value > iron_core_saturation_value) {
			assert_number_of_samples_in_saturation_2++;
			// AssertErrorCondition(InitialisationError,"MachineProtectionGAM::Execute: Consecutive Saturated2 cycles %d/6",assert_number_of_samples_in_saturation_2);
		}
		else assert_number_of_samples_in_saturation_2 = 0;
		
		if ( assert_number_of_samples_in_saturation_1 > assert_saturation_limit1 || assert_number_of_samples_in_saturation_2 > assert_saturation_limit2) outputstruct[0].InSaturation = 1;
		else outputstruct[0].InSaturation = 0;

		outputstruct[0].HardStopBool = False;
		outputstruct[0].SlowStopBool = False;	

	}
	else {
		outputstruct[0].InSaturation = 0;
		outputstruct[0].HardStopBool = False;
		outputstruct[0].SlowStopBool = False;	
	}



	this->SignalsOutputInterface->Write();

	return True;		
}
bool MachineProtectionGAM::ProcessHttpMessage(HttpStream &hStream){
	
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
		hmStream.SSPrintf(HtmlTagStreamMode, "br>iron_core_saturation_value = %.2f\n\
		<br>iron_core_dangerous_value = %.2f\n\
		<br><br",iron_core_saturation_value,iron_core_dangerous_value);
	}
	hmStream.SSPrintf(HtmlTagStreamMode, "/form");

	
	hmStream.SSPrintf(HtmlTagStreamMode, "/body>\n</html");
			
	hStream.SSPrintf("OutputHttpOtions.Content-Type","text/html;charset=utf-8");
	hStream.WriteReplyHeader(True);


	return True;		
}
