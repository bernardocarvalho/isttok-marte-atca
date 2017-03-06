
#include "MagneticsGAM.h"
#include "math.h"

OBJECTLOADREGISTER(MagneticsGAM, "$Id: $")


//  ******** Default constructor ***********************************
MagneticsGAM::MagneticsGAM(){
	this->SignalsInputInterface = NULL;
	this->SignalsOutputInterface = NULL;
}


// ********* Destructor ********************************************
MagneticsGAM::~MagneticsGAM()
{
	
}


//{ ********* Initialise the module ********************************
bool MagneticsGAM::Initialise(ConfigurationDataBase& cdbData){
   
	CDBExtended cdb(cdbData);
	
	int i;

// read config file section: magnetic_probes
		if(!cdb.ReadInt32(usectime_to_wait_for_starting_operation, "usectime_to_wait_for_starting_operation"))
		{
			AssertErrorCondition(InitialisationError,"MagneticsGAM::Initialise: %s usectime_to_wait_for_starting_operation",this->Name());
			return False;
		}
		else AssertErrorCondition(Information,"MagneticsGAM::Initialise: usectime_to_wait_for_starting_operation = %d",usectime_to_wait_for_starting_operation);
		if(!cdb.ReadInt32(i, "magnetic_radial_bool"))
		{
			AssertErrorCondition(InitialisationError,"MagneticsGAM::Initialise: %s magnetic_radial_bool",this->Name());
			return False;
		}
		else 
		{
			magnetic_radial_bool = (bool)i;
			AssertErrorCondition(Information,"MagneticsGAM::Initialise: magnetic_radial_bool = %d",magnetic_radial_bool);
		}	
		if(!cdb.ReadInt32(i, "magnetic_vertical_bool"))
		{
			AssertErrorCondition(InitialisationError,"MagneticsGAM::Initialise: %s magnetic_vertical_bool",this->Name());
			return False;
		}
		else 
		{
			magnetic_vertical_bool = (bool)i;
			AssertErrorCondition(Information,"MagneticsGAM::Initialise: magnetic_vertical_bool = %d",magnetic_vertical_bool);
		}	
		if(!cdb.ReadInt32(i, "magnetic_module_correction_bool"))
		{
			AssertErrorCondition(InitialisationError,"MagneticsGAM::Initialise: %s magnetic_module_correction_bool",this->Name());
			return False;
		}
		else 
		{
			magnetic_module_correction_bool = (bool)i;
			AssertErrorCondition(Information,"MagneticsGAM::Initialise:  = %d",magnetic_module_correction_bool);
		}	


	if(!cdb->Move("MirnovArrayDescription"))
	{
		AssertErrorCondition(InitialisationError,"MagneticsGAM::Initialise: %s Could not move to \"+MARTe.+ISTTOK_RTTh.+magnetic_probes.MirnovArrayDescription\"",this->Name());
		return False;
	}
		if(!cdb.ReadInt32(NumberOfProbes, "NumberOfProbes"))
		{
			AssertErrorCondition(InitialisationError,"MagneticsGAM::Initialise: %s NumberOfProbes",this->Name());
			return False;
		}
		else	AssertErrorCondition(Information,"MagneticsGAM::Initialise: NumberOfProbes = %d",NumberOfProbes);
		
		if (NumberOfProbes > 0){
			magnetic_Angles =new float[NumberOfProbes];
			magnetic_Calibration =new float[NumberOfProbes];

			if(!cdb.ReadFloatArray(magnetic_Angles, (int *)(&NumberOfProbes), 1, "Angles"))
			{
				AssertErrorCondition(InitialisationError,"ReadWaveformFiles: Could not read magnetic_Angles");
				return False;
			}
			else for(i=0;i<NumberOfProbes;i++) AssertErrorCondition(Information,"MagneticsGAM::Initialise: magnetic_Angles[%d] = %f",i, magnetic_Angles[i]);
			if(!cdb.ReadFloatArray(magnetic_Calibration, (int *)(&NumberOfProbes), 1, "Calibration"))
			{
				AssertErrorCondition(InitialisationError,"ReadWaveformFiles: Could not read magnetic_Calibration");
				return False;
			}
			else for(i=0;i<NumberOfProbes;i++) AssertErrorCondition(Information,"MagneticsGAM::Initialise: magnetic_Calibration[%d] = %f",i, magnetic_Calibration[i]);
		}
		else {
			AssertErrorCondition(InitialisationError,"MagneticsGAM::Initialise: %s NumberOfProbes lower than 1",this->Name());
			return False;
		}
		
	cdb->MoveToFather();

	if(!cdb->Move("Measurements"))
	{
		AssertErrorCondition(InitialisationError,"MagneticsGAM::Initialise: %s Could not move to \"+MARTe.+ISTTOK_RTTh.+magnetic_probes.Measurements\"",this->Name());
		return False;
	}
		if(!cdb.ReadInt32(NumberOfMeasurements, "NumberOfMeasurements"))
		{
			AssertErrorCondition(InitialisationError,"MagneticsGAM::Initialise: %s NumberOfMeasurements",this->Name());
			return False;
		}
		else	AssertErrorCondition(Information,"MagneticsGAM::Initialise: NumberOfMeasurements = %d",NumberOfMeasurements);

		if (NumberOfMeasurements > 0){
			ProbeNumbers =new int[NumberOfMeasurements];

			if(!cdb.ReadInt32Array(ProbeNumbers, (int *)(&NumberOfMeasurements), 1, "ProbeNumbers"))
			{
				AssertErrorCondition(InitialisationError,"ReadWaveformFiles: Could not read ProbeNumbers");
				return False;
			}
			else for(i=0;i<NumberOfMeasurements;i++) AssertErrorCondition(Information,"MagneticsGAM::Initialise: ProbeNumbers[%d] = %d",i, ProbeNumbers[i]);
		}
		else {
			AssertErrorCondition(InitialisationError,"MagneticsGAM::Initialise: %s NumberOfMeasurements lower than 1",this->Name());
			return False;
		}

	cdb->MoveToFather();
	
//Added for module offset correction
	if(!cdb->Move("ModuleOffsetCorrectionLSBusec"))
	{
		AssertErrorCondition(InitialisationError,"MagneticsGAM::Initialise: %s Could not move to \"+MARTe.+ISTTOK_RTTh.+magnetic_probes.ModuleOffsetCorrectionLSBusec\"",this->Name());
		return False;
	}
		if(!cdb.ReadInt32(NumberOfModules, "NumberOfModules"))
		{
			AssertErrorCondition(InitialisationError,"MagneticsGAM::Initialise: %s NumberOfModules",this->Name());
			return False;
		}
		else	AssertErrorCondition(Information,"MagneticsGAM::Initialise: NumberOfModules = %d",NumberOfMeasurements);

		if (NumberOfModules > 0){
			magnetic_Offset_slope =new float[NumberOfModules];

			if(!cdb.ReadFloatArray(magnetic_Offset_slope, (int *)(&NumberOfModules), 1, "OffsetCalibration"))
			{
				AssertErrorCondition(InitialisationError,"ReadWaveformFiles: Could not OffsetCalibration");
				return False;
			}
			else for(i=0;i<NumberOfModules;i++) AssertErrorCondition(Information,"MagneticsGAM::Initialise: magnetic_Offset_slope[%d] = %f",i, magnetic_Offset_slope[i]);
		}
		else {
			AssertErrorCondition(InitialisationError,"MagneticsGAM::Initialise: %s NumberOfModules lower than 1",this->Name());
			return False;
		}

		if (NumberOfModules > 0){
			magnetic_Polarity_calibration =new float[NumberOfModules];

			if(!cdb.ReadFloatArray(magnetic_Polarity_calibration, (int *)(&NumberOfModules), 1, "PolarityCalibration"))
			{
				AssertErrorCondition(InitialisationError,"ReadWaveformFiles: Could not read PolarityCalibration");
				return False;
			}
			else for(i=0;i<NumberOfModules;i++) AssertErrorCondition(Information,"MagneticsGAM::Initialise: magnetic_Polarity_calibration[%d] = %f",i, magnetic_Polarity_calibration[i]);
		}
		else {
			AssertErrorCondition(InitialisationError,"MagneticsGAM::Initialise: %s NumberOfModules lower than 1",this->Name());
			return False;
		}

	cdb->MoveToFather();

// Create the signal interfaces
	if(!AddInputInterface(this->SignalsInputInterface, "MagneticsGAMInputInterface"))
	{
		AssertErrorCondition(InitialisationError, "MagneticsGAM::Initialise: %s failed to add the MagneticsGAMInputInterface", this->Name());
		return False;
	}
	if(!AddOutputInterface(this->SignalsOutputInterface, "MagneticsGAMOutputInterface"))
	{
		AssertErrorCondition(InitialisationError, "MagneticsGAM::Initialise: %s failed to add the MagneticsGAMOutputInterface", this->Name());
		return False;
	}
	
//	INPUT SIGNALS (interface)
	if(!cdb->Move("input_signals"))
	{
		AssertErrorCondition(InitialisationError,"MagneticsGAM::Initialise: %s Could not move to \"input_signals\"",this->Name());
		return False;
	}
		int number_of_signals_to_read = 13;
		FString *CDB_move_to;
		FString *SignalType;
		CDB_move_to = new FString[number_of_signals_to_read];
		SignalType = new FString[number_of_signals_to_read];
		for(i=0;i<number_of_signals_to_read-1;i++) CDB_move_to[i].Printf("Channel_%d",i);
		CDB_move_to[number_of_signals_to_read-1].Printf("time");
		for (i=0;i<number_of_signals_to_read;i++){
			
			if(!cdb->Move(CDB_move_to[i].Buffer()))
			{
				AssertErrorCondition(InitialisationError,"MagneticsGAM::Initialise: %s Could not move to \"%s\"",this->Name(),CDB_move_to[i].Buffer());
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
				AssertErrorCondition(Information,"MagneticsGAM::Initialise: Added signal = %s", SignalName.Buffer());
				
				if(!this->SignalsInputInterface->AddSignal(SignalName.Buffer(), SignalType[i].Buffer()))
				{
					AssertErrorCondition(InitialisationError,"MagneticsGAM::Initialise: %s failed to add signal", this->Name());
					return False;
				}
			}
			cdb->MoveToFather();
		}

	cdb->MoveToFather();

//	OUTPUT SIGNALS (interface)
	if(!cdb->Move("output_signals"))
	{
		AssertErrorCondition(InitialisationError,"MagneticsGAM::Initialise: %s Could not move to \"output_signals\"",this->Name());
		return False;
	}
	
		number_of_signals_to_read = 3;
		CDB_move_to = new FString[number_of_signals_to_read];
		SignalType = new FString[number_of_signals_to_read];
		CDB_move_to[0].Printf("magnetic_probes_r");
		CDB_move_to[1].Printf("magnetic_probes_z");
		CDB_move_to[2].Printf("magnetic_probes_plasma_current");
		for (i=0;i<number_of_signals_to_read;i++){
			
			if(!cdb->Move(CDB_move_to[i].Buffer()))
			{
				AssertErrorCondition(InitialisationError,"MagneticsGAM::Initialise: %s Could not move to \"%s\"",this->Name(),CDB_move_to[i].Buffer());
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
				AssertErrorCondition(Information,"MagneticsGAM::Initialise: Added signal = %s", SignalName.Buffer());
				
				if(!this->SignalsOutputInterface->AddSignal(SignalName.Buffer(), SignalType[i].Buffer()))
				{
					AssertErrorCondition(InitialisationError,"MagneticsGAM::Initialise: %s failed to add signal", this->Name());
					return False;
				}
			}
			
			cdb->MoveToFather();
		}

	cdb->MoveToFather();

// Initialise the accumulators
	this->ADC_values = new float[this->NumberOfProbes];
	this->corrected_probes = new float[this->NumberOfMeasurements];
	this->magnetic_Offset_zero = new float[this->NumberOfModules];
	for(i = 0 ; i < this->NumberOfProbes ; i++){
		this->ADC_values[i] = 0.0;
		this->corrected_probes[i] = 0.0;
		this->magnetic_Offset_zero[i] = 0.0;
	}
	magnetic_field_sum = 0.0;

//ACHTUNG ACHTUNG!!! 0.1 if 100us and 0.01 if 1000us
// Correct Offsets factor - values Bits/ms -> bits/100us
	for(i = 0 ; i < this->NumberOfMeasurements ; i++){
		this->magnetic_Offset_slope[i] = this->magnetic_Offset_slope[i]*0.1;
	}

// Initialise the auxiliary probe position values
	this->n_samples = 0;
	this->major_radius = 0.46;
	this->probe_radius = 0.0935;
	this->clip_limit = 0.085; // -clip_limit < output r and z position < +clip_limit
	
	this->radial_coeficients = new float[this->NumberOfProbes];
	this->vertical_coeficients = new float[this->NumberOfProbes];
	for(i = 0 ; i < this->NumberOfProbes ; i++){
		
	this->radial_coeficients[i] = this->probe_radius * cos(this->magnetic_Angles[i] * M_PI / 180);
	this->vertical_coeficients[i] = this->probe_radius * sin(this->magnetic_Angles[i] * M_PI / 180);
	}
	
	if(NumberOfMeasurements == NumberOfProbes){
		
		this->points_x = new float[NumberOfProbes/4];
		this->points_y = new float[NumberOfProbes/4];
		this->m_x = new float[NumberOfProbes];
		this->m_y = new float[NumberOfProbes];
		
		for(i = 0 ; i < this->NumberOfProbes  ; i++){
			m_x[i] = this->radial_coeficients[i] / this->probe_radius;
			m_y[i] = this->vertical_coeficients[i] / this->probe_radius;
		}		
	}
	
	this->plasma_current_convertion_factor = 4300 * 2.0 * M_PI * this->probe_radius / this->NumberOfMeasurements;
	
	return True;
}
//} ******************************************************************


//{ ********* Execute the module functionalities *******************
bool MagneticsGAM::Execute(GAM_FunctionNumbers functionNumber){

	InputInterfaceStruct *inputstruct = (InputInterfaceStruct *) this->SignalsInputInterface->Buffer();
	this->SignalsInputInterface->Read();
	OutputInterfaceStruct *outputstruct = (OutputInterfaceStruct *) this->SignalsOutputInterface->Buffer();
	
	int i;
	
	ADC_values[0] = (float)inputstruct[0].ADC_magnetic_0;
	ADC_values[1] = (float)inputstruct[0].ADC_magnetic_1;
	ADC_values[2] = (float)inputstruct[0].ADC_magnetic_2;
	ADC_values[3] = (float)inputstruct[0].ADC_magnetic_3;
	ADC_values[4] = (float)inputstruct[0].ADC_magnetic_4;
	ADC_values[5] = (float)inputstruct[0].ADC_magnetic_5;
	ADC_values[6] = (float)inputstruct[0].ADC_magnetic_6;
	ADC_values[7] = (float)inputstruct[0].ADC_magnetic_7;
	ADC_values[8] = (float)inputstruct[0].ADC_magnetic_8;
	ADC_values[9] = (float)inputstruct[0].ADC_magnetic_9;
	ADC_values[10] = (float)inputstruct[0].ADC_magnetic_10;
	ADC_values[11] = (float)inputstruct[0].ADC_magnetic_11;

	
	//Apply coil polarity factor - OK
	for(i = 0 ; i < this->NumberOfMeasurements ; i++){
		ADC_values[i] = ADC_values[i]*magnetic_Polarity_calibration[i];
	}
		
	if(functionNumber == GAMOnline){
		// Determine the ADC Module offset "b" as "y(n)=a*n+b"
		
		if(inputstruct[0].usectime > 0 && inputstruct[0].usectime < usectime_to_wait_for_starting_operation){
			
			//For now we do not use this step (under optimization)
					
			//Determine "b" by knowing "a" and "y(-100us)"
			//if(inputstruct[0].usectime==900){
			//	for(i = 0 ; i < this->NumberOfMeasurements ; i++){
			//		this->magnetic_Offset_zero[i] = ADC_values[i] + this->magnetic_Offset_slope[i]; // b = y(-100us) - a*(-100us) = y(10) + a*(1)
			//	}				
			//}			

			outputstruct[0].MagneticProbesR = 0.;
			outputstruct[0].MagneticProbesZ = 0.;
			outputstruct[0].MagneticProbesPlasmaCurrent = 0.;
		}
		else{
			
			//Take offset at t=0
			if(inputstruct[0].usectime==usectime_to_wait_for_starting_operation){
				for(i = 0 ; i < this->NumberOfMeasurements ; i++){
					this->magnetic_Offset_zero[i] = ADC_values[i];
					magnetic_field_sum = 0.0;
				}
			}
			
			//Correct using ADC[n]-(m*x+b)
			for(i = 0 ; i < this->NumberOfMeasurements ; i++){
				corrected_probes[i] = ADC_values[i] - ( this->magnetic_Offset_slope[i]*((inputstruct[0].usectime - usectime_to_wait_for_starting_operation)/100) + this->magnetic_Offset_zero[i]);
			}

			// Calculate Ip
			magnetic_field_sum = 0.0;  //this->NumberOfMeasurements
			for(i = 0 ; i < this->NumberOfMeasurements ; i++){
				magnetic_field_sum = corrected_probes[i];
			}
					
			outputstruct[0].MagneticProbesPlasmaCurrent = magnetic_field_sum;//corrected_probes[11]; //magnetic_field_sum * this->plasma_current_convertion_factor;

			// Estimate radial_position and vertical_position
			radial_position = 0.0;
			vertical_position = 0.0;
			/*
			if(NumberOfMeasurements == NumberOfProbes){
				// WARNING: this code divides by zero fairly often. Many IGBTs died to bring us this information
				for (i = 0 ; i < this->NumberOfProbes/4  ; i++){
					r_a = this->probe_radius * 2 * corrected_probes[int(i+NumberOfProbes/2)] / (corrected_probes[int(i+NumberOfProbes/2)] + corrected_probes[i]);
					r_b = this->probe_radius * 2 * corrected_probes[int(i+NumberOfProbes/2+NumberOfProbes/4)] / (corrected_probes[int(i+NumberOfProbes/2+NumberOfProbes/4)] + corrected_probes[int(i+NumberOfProbes/4)]);
					x_a = this->radial_coeficients[this->ProbeNumbers[i]] - m_x[i] * r_a;
					x_b = this->radial_coeficients[this->ProbeNumbers[int(i+this->NumberOfProbes/4)]] - m_x[int(i+this->NumberOfProbes/4)] * r_b;
					y_a = this->vertical_coeficients[this->ProbeNumbers[i]] - m_y[i] * r_a;
					y_b = this->vertical_coeficients[this->ProbeNumbers[int(i+this->NumberOfProbes/4)]] - m_y[int(i+this->NumberOfProbes/4)] * r_b;
//					if (m_x[i] != 0 && m_x[int(i+NumberOfProbes/4)] != 0) { 
						m_b = m_y[i]/m_x[i];
						m_a = m_y[int(i+this->NumberOfProbes/4)]/m_x[int(i+this->NumberOfProbes/4)];
						points_x[i] = (m_b*x_b-y_b-m_a*x_a+y_a) / (m_b-m_a);
						points_y[i] = m_a*(points_x[i]-x_a)+y_a;
//					}
					radial_position += points_x[i];
					vertical_position += points_y[i];
				}
				
			}
			else {
				for(i = 0 ; i < this->NumberOfMeasurements ; i++){
					
					radial_position += corrected_probes[i] * this->radial_coeficients[this->ProbeNumbers[i]];
					vertical_position += corrected_probes[i] * this->vertical_coeficients[this->ProbeNumbers[i]];
				}
				
				if (magnetic_field_sum !=0) {
					radial_position = radial_position / magnetic_field_sum;
					vertical_position = vertical_position / magnetic_field_sum;
				}
				else {
					radial_position = 0;
					vertical_position = 0;
				} 
			}
			// Hard clip position
			if(radial_position < -this->clip_limit) radial_position = -this->clip_limit;
			if(radial_position > this->clip_limit) radial_position = this->clip_limit;
			
			if(vertical_position < -this->clip_limit) vertical_position = -this->clip_limit;
			if(vertical_position > this->clip_limit) vertical_position = this->clip_limit;
			
			*/
			
			outputstruct[0].MagneticProbesR = radial_position;
			outputstruct[0].MagneticProbesZ = vertical_position;

		} 
	}
	else {
		//this->n_samples = 0;
		//for(i = 0 ; i < (this->NumberOfMeasurements) ; i++){
		//	this->magnetic_Offset_zero[i] = 0;
		//}
		outputstruct[0].MagneticProbesPlasmaCurrent = 0;
		outputstruct[0].MagneticProbesR = 0;
		outputstruct[0].MagneticProbesZ = 0;
	}

	this->SignalsOutputInterface->Write();

	return True;		
}
// ******************************************************************

bool MagneticsGAM::ProcessHttpMessage(HttpStream &hStream){
	
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
		hmStream.SSPrintf(HtmlTagStreamMode, "br><br>magnetic_radial_bool = %d\n\
		<br>magnetic_vertical_bool = %d\n\
		<br>magnetic_module_correction_bool = %d\n\
		<br>NumberOfProbes = %d\n\
		<br>NumberOfMeasurements = %d\n\
		<br>NumberOfModules = %d\n\
		<br><br",magnetic_radial_bool,magnetic_vertical_bool,magnetic_module_correction_bool,NumberOfProbes,NumberOfMeasurements,NumberOfModules);

		hmStream.SSPrintf(HtmlTagStreamMode, "table border=\"1\"><tr><td>magnetic_Angles</td");
		for (i=0;i<NumberOfProbes;i++)hmStream.SSPrintf(HtmlTagStreamMode, "td>%.2f</td",magnetic_Angles[i]);
		hmStream.SSPrintf(HtmlTagStreamMode, "/tr><tr><td>magnetic_Calibration</td");
		for (i=0;i<NumberOfProbes;i++)hmStream.SSPrintf(HtmlTagStreamMode, "td>%.2f</td",magnetic_Calibration[i]);
		hmStream.SSPrintf(HtmlTagStreamMode, "/tr></table><br");

		hmStream.SSPrintf(HtmlTagStreamMode, "table border=\"1\"><tr><td>ProbeNumbers</td");
		for (i=0;i<NumberOfMeasurements;i++)hmStream.SSPrintf(HtmlTagStreamMode, "td>%d</td",ProbeNumbers[i]);
		hmStream.SSPrintf(HtmlTagStreamMode, "/tr></table><br");
		
		hmStream.SSPrintf(HtmlTagStreamMode, "table border=\"1\"><tr><td>magnetic_Offset_slope</td");
		for (i=0;i<NumberOfModules;i++)hmStream.SSPrintf(HtmlTagStreamMode, "td>%.2f</td",magnetic_Offset_slope[i]);
		hmStream.SSPrintf(HtmlTagStreamMode, "/tr></table><br");
		
		hmStream.SSPrintf(HtmlTagStreamMode, "table border=\"1\"><tr><td>magnetic_Polarity_calibration</td");
		for (i=0;i<NumberOfModules;i++)hmStream.SSPrintf(HtmlTagStreamMode, "td>%.2f</td",magnetic_Polarity_calibration[i]);
		hmStream.SSPrintf(HtmlTagStreamMode, "/tr></table><br");
	}
	hmStream.SSPrintf(HtmlTagStreamMode, "/form");
	
	
	hmStream.SSPrintf(HtmlTagStreamMode, "/body>\n</html");
	hStream.SSPrintf("OutputHttpOtions.Content-Type","text/html;charset=utf-8");
	hStream.WriteReplyHeader(True);


	return True;		
}
