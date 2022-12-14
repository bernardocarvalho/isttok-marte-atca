
#include "ElectricProbesGAM.h"

OBJECTLOADREGISTER(ElectricProbesGAM, "$Id: $")


//  ******** Default constructor ***********************************
ElectricProbesGAM::ElectricProbesGAM(){
	this->SignalsInputInterface = NULL;
	this->SignalsOutputInterface = NULL;
}




// ********* Destructor ********************************************
ElectricProbesGAM::~ElectricProbesGAM()
{
	//if(this->SignalsInputInterface != NULL) delete[] this->SignalsInputInterface ;
	//if(this->SignalsOutputInterface != NULL) delete[] this->SignalsOutputInterface;
}



//{ ********* Initialise the module ********************************
bool ElectricProbesGAM::Initialise(ConfigurationDataBase& cdbData){
   
	CDBExtended cdb(cdbData);
	
	int i;
	
	if(!cdb.ReadInt32(electric_radial_bool, "electric_radial_bool"))
	{
		AssertErrorCondition(InitialisationError,"ElectricProbesGAM::Initialise: %s electric_radial_bool",this->Name());
		return False;
	}
	else AssertErrorCondition(Information, "ElectricProbesGAM::Initialise: electric_radial_bool = %d",electric_radial_bool);

	if(!cdb.ReadInt32(electric_vertical_bool, "electric_vertical_bool"))
	{
		AssertErrorCondition(InitialisationError,"ElectricProbesGAM::Initialise: %s electric_vertical_bool",this->Name());
		return False;
	}
	else AssertErrorCondition(Information,"ElectricProbesGAM::Initialise: electric_vertical_bool = %d",electric_vertical_bool);
	if(!cdb.ReadInt32(usectime_to_wait_for_starting_operation, "usectime_to_wait_for_starting_operation"))
	{
		AssertErrorCondition(InitialisationError,"CosineProbeGAM::Initialise: %s usectime_to_wait_for_starting_operation",this->Name());
		return False;
	}
	else AssertErrorCondition(Information,"CosineProbeGAM::Initialise: usectime_to_wait_for_starting_operation = %d",usectime_to_wait_for_starting_operation);

//	sleep(1); test proposes
// Create the signal interfaces
	if(!AddInputInterface(this->SignalsInputInterface, "ElectricProbesGAMInputInterface"))
	{
		AssertErrorCondition(InitialisationError, "ElectricProbesGAM::Initialise: %s failed to add the ElectricProbesGAMInputInterface", this->Name());
		return False;
	}
	if(!AddOutputInterface(this->SignalsOutputInterface, "ElectricProbesGAMOutputInterface"))
	{
		AssertErrorCondition(InitialisationError, "ElectricProbesGAM::Initialise: %s failed to add the ElectricProbesGAMOutputInterface", this->Name());
		return False;
	}

//	INPUT SIGNALS (interface)
	if(!cdb->Move("input_signals"))
	{
		AssertErrorCondition(InitialisationError,"ElectricProbesGAM::Initialise: %s Could not move to \"input_signals\"",this->Name());
		return False;
	}
		int number_of_signals_to_read = 5;
		FString *CDB_move_to;
		FString *SignalType;
		CDB_move_to = new FString[number_of_signals_to_read];
		SignalType = new FString[number_of_signals_to_read];
		for(i=0;i<number_of_signals_to_read-1;i++) CDB_move_to[i].Printf("Channel_%d",i);
		CDB_move_to[number_of_signals_to_read-1].Printf("system_time");
		for (i=0;i<number_of_signals_to_read;i++){
			
			if(!cdb->Move(CDB_move_to[i].Buffer()))
			{
				AssertErrorCondition(InitialisationError,"ElectricProbesGAM::Initialise: %s Could not move to \"%s\"",this->Name(),CDB_move_to[i].Buffer());
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
				AssertErrorCondition(Information,"ElectricProbesGAM::Initialise: Added signal = %s", SignalName.Buffer());
				
				if(!this->SignalsInputInterface->AddSignal(SignalName.Buffer(), SignalType[i].Buffer()))
				{
					AssertErrorCondition(InitialisationError,"ElectricProbesGAM::Initialise: %s failed to add signal", this->Name());
					return False;
				}
			}
			cdb->MoveToFather();
		}

	cdb->MoveToFather();


//	OUTPUT SIGNALS (interface)
	if(!cdb->Move("output_signals"))
	{
		AssertErrorCondition(InitialisationError,"ElectricProbesGAM::Initialise: %s Could not move to \"output_signals\"",this->Name());
		return False;
	}
	
		number_of_signals_to_read = 2;
		CDB_move_to = new FString[number_of_signals_to_read];
		SignalType = new FString[number_of_signals_to_read];
		CDB_move_to[0].Printf("electric_probes_r");
		CDB_move_to[1].Printf("electric_probes_z");
		for (i=0;i<number_of_signals_to_read;i++){
			
			if(!cdb->Move(CDB_move_to[i].Buffer()))
			{
				AssertErrorCondition(InitialisationError,"ElectricProbesGAM::Initialise: %s Could not move to \"%s\"",this->Name(),CDB_move_to[i].Buffer());
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
				AssertErrorCondition(Information,"ElectricProbesGAM::Initialise: Added signal = %s", SignalName.Buffer());
				
				if(!this->SignalsOutputInterface->AddSignal(SignalName.Buffer(), SignalType[i].Buffer()))
				{
					AssertErrorCondition(InitialisationError,"ElectricProbesGAM::Initialise: %s failed to add signal", this->Name());
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
	
//	2*distance_from_center_to_near in mm
	this->distance = 0.08;
	
	this->out_of_bounds_limit = 0.085;
	
	//constants for converting volt difference to mm;
	//ADJUST THIS!!!!!!!!
	this->RadialConstant = 0.001;
	this->VerticalConstant = 0.001;
	

	return True;
}
//} ******************************************************************



//{ ********* Execute the module functionalities *******************
bool ElectricProbesGAM::Execute(GAM_FunctionNumbers functionNumber){
 
//	AssertErrorCondition(InitialisationError,"MainConfigurator::WriteConfigurationFile: discharge_time = %f",discharge_time);


	InputInterfaceStruct *inputstruct = (InputInterfaceStruct *) this->SignalsInputInterface->Buffer();
	this->SignalsInputInterface->Read();
//	AssertErrorCondition(InitialisationError,"ElectricProbesGAM:: %s inputstruct = %f %f %f %f %f %f %f %f",this->Name(), inputstruct[0].ADC_electric_top_far , inputstruct[0].ADC_electric_top_near , inputstruct[0].ADC_electric_inner_far , inputstruct[0].ADC_electric_inner_near , inputstruct[0].ADC_electric_outer_far , inputstruct[0].ADC_electric_outer_near , inputstruct[0].ADC_electric_bottom_far , inputstruct[0].ADC_electric_bottom_near );

	OutputInterfaceStruct *outputstruct = (OutputInterfaceStruct *) this->SignalsOutputInterface->Buffer();
		
/*		old code with far and near electric probes
		auxiliary_calculation[0] = constants[0]*(float)inputstruct[0].ADC_electric_inner_near - constants[1]*(float)inputstruct[0].ADC_electric_inner_far - constants[0]*(float)inputstruct[0].ADC_electric_outer_near + constants[1]*(float)inputstruct[0].ADC_electric_outer_far; 
		auxiliary_calculation[1] = (float)inputstruct[0].ADC_electric_inner_far +  (float)inputstruct[0].ADC_electric_outer_far - (float)inputstruct[0].ADC_electric_inner_near - (float)inputstruct[0].ADC_electric_outer_near ; 
		auxiliary_calculation[2] = constants[0]*(float)inputstruct[0].ADC_electric_bottom_near - constants[1]*(float)inputstruct[0].ADC_electric_bottom_far - constants[0]*(float)inputstruct[0].ADC_electric_top_near + constants[1]*(float)inputstruct[0].ADC_electric_top_far; 
		auxiliary_calculation[3] = (float)inputstruct[0].ADC_electric_bottom_far +  (float)inputstruct[0].ADC_electric_top_far - (float)inputstruct[0].ADC_electric_bottom_near - (float)inputstruct[0].ADC_electric_top_near ; 

		if (auxiliary_calculation[1] != 0 && electric_radial_bool) {
			outputstruct[0].ElectricProbesR = auxiliary_calculation[0] / auxiliary_calculation[1];
			if (outputstruct[0].ElectricProbesR > out_of_bounds_limit)outputstruct[0].ElectricProbesR = out_of_bounds_limit;
			if (outputstruct[0].ElectricProbesR < -out_of_bounds_limit)outputstruct[0].ElectricProbesR = -out_of_bounds_limit;
			
		}
		else outputstruct[0].ElectricProbesR = 0;
		
		if (auxiliary_calculation[1] != 0 && electric_vertical_bool) {
			outputstruct[0].ElectricProbesZ = auxiliary_calculation[2] / auxiliary_calculation[3];
			if (outputstruct[0].ElectricProbesZ > out_of_bounds_limit)outputstruct[0].ElectricProbesZ = out_of_bounds_limit;
			if (outputstruct[0].ElectricProbesZ < -out_of_bounds_limit)outputstruct[0].ElectricProbesZ = -out_of_bounds_limit;
			
		}
		else outputstruct[0].ElectricProbesZ = 0;
*/
// new code with just 4 electric probes

	if(functionNumber == GAMOnline){
		// Determine the ADC offset
		if(inputstruct[0].usectime > 0 && inputstruct[0].usectime < usectime_to_wait_for_starting_operation){
			n_samples++;

			this->accumulator_1 += (float) inputstruct[0].ADC_electric_top_near;
			this->accumulator_2 += (float) inputstruct[0].ADC_electric_inner_near;
			this->accumulator_3 += (float) inputstruct[0].ADC_electric_outer_near;
			this->accumulator_4 += (float) inputstruct[0].ADC_electric_bottom_near;
			this->remove_offset_1 = this->accumulator_1  / (float) this->n_samples;
			this->remove_offset_2 = this->accumulator_2  / (float) this->n_samples;
			this->remove_offset_3 = this->accumulator_3  / (float) this->n_samples;
			this->remove_offset_4 = this->accumulator_4  / (float) this->n_samples;
		outputstruct[0].ElectricProbesR = 0;
		outputstruct[0].ElectricProbesZ = 0;
		}
		else{
			//send offset corrections to logger once
			if (this->n_samples >0 ){

				AssertErrorCondition(Information,"ElectricProbesGAM::Execute: %s OFFSETS = %f,%f,%f,%f, number of samples = %d", this->Name(), this->remove_offset_1, this->remove_offset_2, this->remove_offset_3, this->remove_offset_4, n_samples);
				n_samples = 0;
			}

			if (electric_radial_bool) {
				outputstruct[0].ElectricProbesR = RadialConstant * ( (inputstruct[0].ADC_electric_inner_near - this->remove_offset_2) - (inputstruct[0].ADC_electric_outer_near - this->remove_offset_3));
				if (outputstruct[0].ElectricProbesR > out_of_bounds_limit)outputstruct[0].ElectricProbesR = out_of_bounds_limit;
				if (outputstruct[0].ElectricProbesR < -out_of_bounds_limit)outputstruct[0].ElectricProbesR = -out_of_bounds_limit;
				
			}
			else outputstruct[0].ElectricProbesR = 0;
			
			if (electric_vertical_bool) {
				outputstruct[0].ElectricProbesZ = VerticalConstant * ((inputstruct[0].ADC_electric_bottom_near - this->remove_offset_4) - (inputstruct[0].ADC_electric_top_near - this->remove_offset_1));
				if (outputstruct[0].ElectricProbesZ > out_of_bounds_limit)outputstruct[0].ElectricProbesZ = out_of_bounds_limit;
				if (outputstruct[0].ElectricProbesZ < -out_of_bounds_limit)outputstruct[0].ElectricProbesZ = -out_of_bounds_limit;
				
			}
			else outputstruct[0].ElectricProbesZ = 0;

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
		outputstruct[0].ElectricProbesR = 0;
		outputstruct[0].ElectricProbesZ = 0;
	}
		
//		AssertErrorCondition(InitialisationError,"ElectricProbesGAM:: %s outputstruct = %f %f",this->Name(), outputstruct[0].ElectricProbesR , outputstruct[0].ElectricProbesZ );
		this->SignalsOutputInterface->Write();
	
	return True;		
}
bool ElectricProbesGAM::ProcessHttpMessage(HttpStream &hStream){
	
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
		hmStream.SSPrintf(HtmlTagStreamMode, "br><br>electric_radial_bool = %d\n\
		<br>electric_vertical_bool = %d\n\
		<br><br",electric_radial_bool, electric_vertical_bool);
	}
	hmStream.SSPrintf(HtmlTagStreamMode, "/form");
  
	hmStream.SSPrintf(HtmlTagStreamMode, "/body>\n</html");
	hStream.SSPrintf("OutputHttpOtions.Content-Type","text/html;charset=utf-8");
	hStream.WriteReplyHeader(True);


	return True;		
}
