#include "InterferometryGAM.h"

OBJECTLOADREGISTER(InterferometryGAM, "$Id: $")


//  ******** Default constructor ***********************************
InterferometryGAM::InterferometryGAM(){
	this->SignalsInputInterface = NULL;
	this->SignalsOutputInterface = NULL;
}




// ********* Destructor ********************************************
InterferometryGAM::~InterferometryGAM()
{
//	if(this->SignalsInputInterface != NULL) delete[] this->SignalsInputInterface ;
//	if(this->SignalsOutputInterface != NULL) delete[] this->SignalsOutputInterface;
}



//{ ********* Initialise the module ********************************
bool InterferometryGAM::Initialise(ConfigurationDataBase& cdbData){
   
	CDBExtended cdb(cdbData);
	int i;

	if(!cdb.ReadInt32(i, "interferometry_radial_control_bool"))
	{
		AssertErrorCondition(InitialisationError,"MainConfigurator::ReadConfigurationFile: %s interferometry_radial_control_bool",this->Name());
		return False;
	}
	else 
	{
		interferometry_radial_control_bool = (bool)i;
		AssertErrorCondition(Information,"MainConfigurator::ReadConfigurationFile: interferometry_radial_control_bool = %d",interferometry_radial_control_bool);
	}	


//	sleep(3);
// Create the signal interfaces
	if(!AddInputInterface(this->SignalsInputInterface, "SineProbeGAMInputInterface"))
	{
		AssertErrorCondition(InitialisationError, "InterferometryGAM::Initialise: %s failed to add the TimewindowsGAMInputInterface", this->Name());
		return False;
	}
	if(!AddOutputInterface(this->SignalsOutputInterface, "SineProbeGAMOutputInterface"))
	{
		AssertErrorCondition(InitialisationError, "InterferometryGAM::Initialise: %s failed to add the TimewindowsGAMOutputInterface", this->Name());
		return False;
	}

//	INPUT SIGNALS (interface)
	if(!cdb->Move("input_signals"))
	{
		AssertErrorCondition(InitialisationError,"InterferometryGAM::Initialise: %s Could not move to \"input_signals\"",this->Name());
		return False;
	}
	
		int number_of_signals_to_read = 4;
		FString *CDB_move_to;
		FString *SignalType;
		CDB_move_to = new FString[number_of_signals_to_read];
		SignalType = new FString[number_of_signals_to_read];
		CDB_move_to[0].Printf("interferometry_sine_signal");
		CDB_move_to[1].Printf("interferometry_cosine_signal");
		CDB_move_to[2].Printf("H_alpha");
		CDB_move_to[3].Printf("discharge_status");
		for (i=0;i<number_of_signals_to_read;i++){
			
			if(!cdb->Move(CDB_move_to[i].Buffer()))
			{
				AssertErrorCondition(InitialisationError,"InterferometryGAM::Initialise: %s Could not move to \"%s\"",this->Name(),CDB_move_to[i].Buffer());
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
				AssertErrorCondition(Information,"InterferometryGAM::Initialise: Added signal = %s", SignalName.Buffer());
				
				if(!this->SignalsInputInterface->AddSignal(SignalName.Buffer(), SignalType[i].Buffer()))
				{
					AssertErrorCondition(InitialisationError,"InterferometryGAM::Initialise: %s failed to add signal", this->Name());
					return False;
				}
			}
			
			
			cdb->MoveToFather();
		}

	cdb->MoveToFather();


//	OUTPUT SIGNALS (interface)
	if(!cdb->Move("output_signals"))
	{
		AssertErrorCondition(InitialisationError,"InterferometryGAM::Initialise: %s Could not move to \"output_signals\"",this->Name());
		return False;
	}
	
		number_of_signals_to_read = 2;
		CDB_move_to = new FString[number_of_signals_to_read];
		SignalType = new FString[number_of_signals_to_read];
		CDB_move_to[0].Printf("density");
		CDB_move_to[1].Printf("radial_position");
		for (i=0;i<number_of_signals_to_read;i++){
			
			if(!cdb->Move(CDB_move_to[i].Buffer()))
			{
				AssertErrorCondition(InitialisationError,"InterferometryGAM::Initialise: %s Could not move to \"%s\"",this->Name(),CDB_move_to[i].Buffer());
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
				AssertErrorCondition(Information,"InterferometryGAM::Initialise: Added signal = %s", SignalName.Buffer());
				
				if(!this->SignalsOutputInterface->AddSignal(SignalName.Buffer(), SignalType[i].Buffer()))
				{
					AssertErrorCondition(InitialisationError,"InterferometryGAM::Initialise: %s failed to add signal", this->Name());
					return False;
				}
			}
			cdb->MoveToFather();
		}

	cdb->MoveToFather();
	
	constant_PI = 2*acos(0.);
	constant_phase_to_density = -8e17;
	minimum_density_for_centred_plasma = 2e18;

	return True;
}
//} ******************************************************************



//{ ********* Execute the module functionalities *******************
bool InterferometryGAM::Execute(GAM_FunctionNumbers functionNumber){
 
	InputInterfaceStruct *inputstruct = (InputInterfaceStruct *) this->SignalsInputInterface->Buffer();
	this->SignalsInputInterface->Read();
//	AssertErrorCondition(InitialisationError,"InterferometryGAM:: %s inputstruct = %f %f %d",this->Name(), inputstruct[0].ADC_interferometry_sine, inputstruct[0].ADC_interferometry_cosine, inputstruct[0].DischargeStatus);
	OutputInterfaceStruct *outputstruct = (OutputInterfaceStruct *) this->SignalsOutputInterface->Buffer();
	
	float interf;
	float delta;
	float ha;
	
	if(functionNumber == GAMOnline){
		
		if(inputstruct[0].DischargeStatus == -2){
			outputstruct[0].InterferometryDensity = 0;
			outputstruct[0].InterferometryR = 0;
			i=0;
			old_phase = 0;
		}
		if(inputstruct[0].DischargeStatus == -1){
			i++;
			offset_sine = (offset_sine * (i-1)/(i) + ((float) inputstruct[0].ADC_interferometry_sine)/(i) );
			offset_cosine = (offset_cosine * (i-1)/(i) + ((float) inputstruct[0].ADC_interferometry_cosine)/(i) );
			old_phase = 0;
		}
		if(inputstruct[0].DischargeStatus >= 0){
			sine = (float) inputstruct[0].ADC_interferometry_sine;
			cosine = (float) inputstruct[0].ADC_interferometry_cosine;
			InterferencePhase = atan2(sine,cosine);
			InterferenceAmplitude=sqrt(cosine*cosine+sine*sine);
			HAmplitude=(float)inputstruct[0].HAlpha;
			PhaseShift=2*acos(InterferenceAmplitude/(2*HAmplitude));
			outputstruct[0].InterferometryDensity = InterferencePhase-PhaseShift;
		}

		if (interferometry_radial_control_bool){
			if (outputstruct[0].InterferometryDensity > minimum_density_for_centred_plasma) outputstruct[0].InterferometryR = outputstruct[0].InterferometryDensity;
			else outputstruct[0].InterferometryR = 0;
		}
		else outputstruct[0].InterferometryR = 0;
		
	}
	
//	outputstruct[0].InterferometryDensity = (float) inputstruct[0].ADC_interferometry_sine;
//	AssertErrorCondition(InitialisationError,"InterferometryGAM:: %s outputstruct = %f %f",this->Name(), outputstruct[0].InterferometryDensity, outputstruct[0].InterferometryR);
	this->SignalsOutputInterface->Write();

	return True;		
}
bool InterferometryGAM::ProcessHttpMessage(HttpStream &hStream){
	
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
		hmStream.SSPrintf(HtmlTagStreamMode, "br><br>interferometry_radial_control_bool = %d\n\
		<br><br", interferometry_radial_control_bool);
	}
	hmStream.SSPrintf(HtmlTagStreamMode, "/form");

	
	hmStream.SSPrintf(HtmlTagStreamMode, "/body>\n</html");
			
	hStream.SSPrintf("OutputHttpOtions.Content-Type","text/html;charset=utf-8");
	hStream.WriteReplyHeader(True);


	return True;		
}
