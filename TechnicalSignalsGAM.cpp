
#include "TechnicalSignalsGAM.h"

OBJECTLOADREGISTER(TechnicalSignalsGAM, "$Id: $")


//  ******** Default constructor ***********************************
TechnicalSignalsGAM::TechnicalSignalsGAM(){
	this->SignalsInputInterface = NULL;
	this->SignalsOutputInterface = NULL;
}




// ********* Destructor ********************************************
TechnicalSignalsGAM::~TechnicalSignalsGAM()
{
//	if(this->SignalsInputInterface != NULL) delete[] this->SignalsInputInterface ;
//	if(this->SignalsOutputInterface != NULL) delete[] this->SignalsOutputInterface;
}



//{ ********* Initialise the module ********************************
bool TechnicalSignalsGAM::Initialise(ConfigurationDataBase& cdbData){
   
	CDBExtended cdb(cdbData);

	if(!cdb.ReadInt32(usectime_to_wait_for_starting_operation, "usectime_to_wait_for_starting_operation"))
	{
		AssertErrorCondition(InitialisationError,"CosineProbeGAM::Initialise: %s usectime_to_wait_for_starting_operation",this->Name());
		return False;
	}
	else AssertErrorCondition(Information,"CosineProbeGAM::Initialise: usectime_to_wait_for_starting_operation = %d",usectime_to_wait_for_starting_operation);

//	sleep(3);
// Create the signal interfaces
	if(!AddInputInterface(this->SignalsInputInterface, "TechnicalSignalsGAMInputInterface"))
	{
		AssertErrorCondition(InitialisationError, "TechnicalSignalsGAM::Initialise: %s failed to add the TimewindowsGAMInputInterface", this->Name());
		return False;
	}
	if(!AddOutputInterface(this->SignalsOutputInterface, "TechnicalSignalsGAMOutputInterface"))
	{
		AssertErrorCondition(InitialisationError, "TechnicalSignalsGAM::Initialise: %s failed to add the TimewindowsGAMOutputInterface", this->Name());
		return False;
	}

//	INPUT SIGNALS (interface)
	if(!cdb->Move("input_signals"))
	{
		AssertErrorCondition(InitialisationError,"TechnicalSignalsGAM::Initialise: %s Could not move to \"input_signals\"",this->Name());
		return False;
	}
		int number_of_signals_to_read = 6;
		FString *CDB_move_to;
		FString *SignalType;
		CDB_move_to = new FString[number_of_signals_to_read];
		SignalType = new FString[number_of_signals_to_read];
		CDB_move_to[0].Printf("horizontal_current_transducer");
		CDB_move_to[1].Printf("vertical_current_transducer");
		CDB_move_to[2].Printf("primary_current_transducer");
		CDB_move_to[3].Printf("v_loop");
		CDB_move_to[4].Printf("h_alfa_output");
		CDB_move_to[5].Printf("system_time");
		for (i=0;i<number_of_signals_to_read;i++){
			
			if(!cdb->Move(CDB_move_to[i].Buffer()))
			{
				AssertErrorCondition(InitialisationError,"TechnicalSignalsGAM::Initialise: %s Could not move to \"%s\"",this->Name(),CDB_move_to[i].Buffer());
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
				AssertErrorCondition(Information,"TechnicalSignalsGAM::Initialise: Added signal = %s", SignalName.Buffer());
				
				if(!this->SignalsInputInterface->AddSignal(SignalName.Buffer(), SignalType[i].Buffer()))
				{
					AssertErrorCondition(InitialisationError,"TechnicalSignalsGAM::Initialise: %s failed to add signal", this->Name());
					return False;
				}
			}
			
			
			cdb->MoveToFather();
		}

	cdb->MoveToFather();


//	OUTPUT SIGNALS (interface)
	if(!cdb->Move("output_signals"))
	{
		AssertErrorCondition(InitialisationError,"TechnicalSignalsGAM::Initialise: %s Could not move to \"output_signals\"",this->Name());
		return False;
	}
	
		number_of_signals_to_read = 6;
		CDB_move_to = new FString[number_of_signals_to_read];
		SignalType = new FString[number_of_signals_to_read];
		CDB_move_to[0].Printf("horizontal_current");
		CDB_move_to[1].Printf("vertical_current");
		CDB_move_to[2].Printf("primary_current");
		CDB_move_to[3].Printf("iron_core_saturation");
		CDB_move_to[4].Printf("v_loop");
		CDB_move_to[5].Printf("h_alpha");
		for (i=0;i<number_of_signals_to_read;i++){
			
			if(!cdb->Move(CDB_move_to[i].Buffer()))
			{
				AssertErrorCondition(InitialisationError,"TechnicalSignalsGAM::Initialise: %s Could not move to \"%s\"",this->Name(),CDB_move_to[i].Buffer());
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
				AssertErrorCondition(Information,"TechnicalSignalsGAM::Initialise: Added signal = %s", SignalName.Buffer());
				
				if(!this->SignalsOutputInterface->AddSignal(SignalName.Buffer(), SignalType[i].Buffer()))
				{
					AssertErrorCondition(InitialisationError,"TechnicalSignalsGAM::Initialise: %s failed to add signal", this->Name());
					return False;
				}
			}
			cdb->MoveToFather();
		}

	cdb->MoveToFather();

		this->n_samples = 0;
		this->accumulator_1 = 0;
		this->accumulator_2 = 0;
		this->accumulator_3 = 0;
		this->accumulator_4 = 0;
		this->remove_offset_1 = 0;
		this->remove_offset_2 = 0;
		this->remove_offset_3 = 0;
		this->remove_offset_4 = 0;

		number_of_points_for_average = 8;

		this->old_v_loop_value_storage = new float[number_of_points_for_average];
		for (i=0;i<number_of_points_for_average;i++){
			old_v_loop_value_storage[i] = 0;
		}

	return True;
}
//} ******************************************************************



//{ ********* Execute the module functionalities *******************
bool TechnicalSignalsGAM::Execute(GAM_FunctionNumbers functionNumber){
 
	InputInterfaceStruct *inputstruct = (InputInterfaceStruct *) this->SignalsInputInterface->Buffer();
	this->SignalsInputInterface->Read();
//	AssertErrorCondition(InitialisationError,"TechnicalSignalsGAM:: %s inputstruct = %f %f %f %f %f ",this->Name(), inputstruct[0].ADC_horizontal_current, inputstruct[0].ADC_vertical_current, inputstruct[0].ADC_primary_current, inputstruct[0].ADC_vloop, inputstruct[0].HAlfaOutput);
	OutputInterfaceStruct *outputstruct = (OutputInterfaceStruct *) this->SignalsOutputInterface->Buffer();
	
	if(functionNumber == GAMOnline){
		// Determine the ADC offset
		if(inputstruct[0].usectime > 0 && inputstruct[0].usectime < usectime_to_wait_for_starting_operation){
			n_samples++;
				
			this->accumulator_1 += (float) inputstruct[0].ADC_horizontal_current;
			this->accumulator_2 += (float) inputstruct[0].ADC_vertical_current;
			this->accumulator_3 += (float) inputstruct[0].ADC_primary_current;
			this->accumulator_4 += (float) inputstruct[0].ADC_vloop;
			this->remove_offset_1 = this->accumulator_1  / (float) this->n_samples;
			this->remove_offset_2 = this->accumulator_2  / (float) this->n_samples;
			this->remove_offset_3 = this->accumulator_3  / (float) this->n_samples;
			this->remove_offset_4 = this->accumulator_4  / (float) this->n_samples;
			outputstruct[0].HorizontalCurrent = 0;
			outputstruct[0].VerticalCurrent = 0;
			outputstruct[0].PrimaryCurrent = 0;
			outputstruct[0].VLoop = 0;
			outputstruct[0].HAlfa = 0;
			outputstruct[0].IronCoreSaturation = 0;
		}
		else{
			//send offset corrections to logger once
			if (this->n_samples >0 ){

				AssertErrorCondition(Information,"TechnicalSignalsGAM::Execute: %s OFFSETS = %f,%f,%f,%f, number of samples = %d", this->Name(), this->remove_offset_1, this->remove_offset_2, this->remove_offset_3, this->remove_offset_4, n_samples);
				n_samples = 0;
			}
			
			old_v_loop_value_storage[0] = inputstruct[0].ADC_vloop - this->remove_offset_4; 
			filtered_v_loop = old_v_loop_value_storage[0];
			
			for (i=1;i<number_of_points_for_average;i++){
				filtered_v_loop += old_v_loop_value_storage[i];
				old_v_loop_value_storage[i] = old_v_loop_value_storage[i-1];
			}
			filtered_v_loop = filtered_v_loop / number_of_points_for_average;
			
			outputstruct[0].VLoop = filtered_v_loop;
			
			outputstruct[0].HorizontalCurrent = inputstruct[0].ADC_horizontal_current - this->remove_offset_1;
			outputstruct[0].VerticalCurrent = inputstruct[0].ADC_vertical_current - this->remove_offset_2;
			outputstruct[0].PrimaryCurrent = inputstruct[0].ADC_primary_current - this->remove_offset_3;
			if (inputstruct[0].HAlfaOutput > 0)	outputstruct[0].HAlfa = inputstruct[0].HAlfaOutput;
			else outputstruct[0].HAlfa = 0;
			
			
			//try to preview iron core saturation by this formula  Iprim ^ 2 / (Vloop ^ 2 + 0.1)
			outputstruct[0].IronCoreSaturation = ( outputstruct[0].PrimaryCurrent * outputstruct[0].PrimaryCurrent / ( outputstruct[0].VLoop * outputstruct[0].VLoop + 0.1 ) );
		} 
	}
	else {
		this->n_samples = 0;
		this->accumulator_1 = 0;
		this->accumulator_2 = 0;
		this->accumulator_3 = 0;
		this->accumulator_4 = 0;
		this->remove_offset_1 = 0;
		this->remove_offset_2 = 0;
		this->remove_offset_3 = 0;
		this->remove_offset_4 = 0;
		outputstruct[0].HorizontalCurrent = 0;
		outputstruct[0].VerticalCurrent = 0;
		outputstruct[0].PrimaryCurrent = 0;
		outputstruct[0].VLoop = 0;
		outputstruct[0].HAlfa = 0;
		outputstruct[0].IronCoreSaturation = 0;
	}

	
	
	this->SignalsOutputInterface->Write();

	return True;		
}
bool TechnicalSignalsGAM::ProcessHttpMessage(HttpStream &hStream){
	
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
	}
	hmStream.SSPrintf(HtmlTagStreamMode, "/form");
	
	hmStream.SSPrintf(HtmlTagStreamMode, "/body>\n</html");
	hStream.SSPrintf("OutputHttpOtions.Content-Type","text/html;charset=utf-8");
	hStream.WriteReplyHeader(True);


	return True;		
}
