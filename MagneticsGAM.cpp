
#include "MagneticsGAM.h"

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
	this->accumulator = new float[this->NumberOfMeasurements];
	this->remove_offset = new float[this->NumberOfMeasurements];
	for(i = 0 ; i < this->NumberOfMeasurements ; i++){
		this->accumulator[i] = 0.0;
		this->remove_offset[i] = 0.0;
	} 

	// Initialise the auxiliary probe position values
	this->n_samples = 0;
	this->major_radius = 0.46;
	this->probe_radius = 0.0935;
	this->clip_limit = 0.085; // -clip_limit < output r and z position < +clip_limit
	
	this->radial_coeficients = new float[this->NumberOfProbes];
	this->vertical_coeficients = new float[this->NumberOfProbes];
	for(i = 0 ; i < this->NumberOfProbes ; i++){
		
//		this->radial_coeficients[i] = this->major_radius + this->probe_radius * cos(this->magnetic_Angles[i] * M_PI / 180);
		this->radial_coeficients[i] = this->probe_radius * cos(this->magnetic_Angles[i] * M_PI / 180);
		this->vertical_coeficients[i] = this->probe_radius * sin(this->magnetic_Angles[i] * M_PI / 180);
		//AssertErrorCondition(InitialisationError,"MagneticsGAM:: probe number = %d position x = %f, y = %f",i,this->radial_coeficients[i],this->vertical_coeficients[i]);
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
/*
	float *teste;
	teste = new float[NumberOfProbes];
	teste[0] = 1;
	teste[1] = 1;
	teste[2] = 1.2;
	teste[3] = 1.2;
	teste[4] = 1;
	teste[5] = 1;
	teste[6] = 1;
	teste[7] = 1;
	teste[8] = 0.8;
	teste[9] = 0.8;
	teste[10] = 1;
	teste[11] = 1;
	
	
	for (i = 0 ; i < this->NumberOfProbes/4  ; i++){
		r_a = this->probe_radius * 2 * teste[int(i+NumberOfProbes/2)] / (teste[int(i+NumberOfProbes/2)] + teste[i]);
		r_b = this->probe_radius * 2 * teste[int(i+NumberOfProbes/2+NumberOfProbes/4)] / (teste[int(i+NumberOfProbes/2+NumberOfProbes/4)] + teste[int(i+NumberOfProbes/4)]);
		x_a = this->radial_coeficients[this->ProbeNumbers[i]] - m_x[i] * r_a;
		x_b = this->radial_coeficients[this->ProbeNumbers[int(i+this->NumberOfProbes/4)]] - m_x[int(i+this->NumberOfProbes/4)] * r_b;
		y_a = this->vertical_coeficients[this->ProbeNumbers[i]] - m_y[i] * r_a;
		y_b = this->vertical_coeficients[this->ProbeNumbers[int(i+this->NumberOfProbes/4)]] - m_y[int(i+this->NumberOfProbes/4)] * r_b;
	
		AssertErrorCondition(InitialisationError,"MagneticsGAM:: i = %d r_a = %f, r_b = %f, x_a = %f, y_a = %f, x_b = %f, y_b = %f",i,r_a,r_b,x_a,y_a,x_b,y_b);
		
//					if (m_x[i] != 0 && m_x[int(i+NumberOfProbes/4)] != 0) { 
			m_b = m_y[i]/m_x[i];
			m_a = m_y[int(i+this->NumberOfProbes/4)]/m_x[int(i+this->NumberOfProbes/4)];
			points_x[i] = (m_b*x_b-y_b-m_a*x_a+y_a) / (m_b-m_a);
			points_y[i] = m_a*(points_x[i]-x_a)+y_a;
//					}
		radial_position += points_x[i];
		vertical_position += points_y[i];
	}
		AssertErrorCondition(InitialisationError,"MagneticsGAM:: position x = %f, y = %f",this->radial_position,this->vertical_position);
*/

	
	
	return True;
}
//} ******************************************************************



//{ ********* Execute the module functionalities *******************
bool MagneticsGAM::Execute(GAM_FunctionNumbers functionNumber){

	InputInterfaceStruct *inputstruct = (InputInterfaceStruct *) this->SignalsInputInterface->Buffer();
	this->SignalsInputInterface->Read();
//	AssertErrorCondition(InitialisationError,"MagneticsGAM:: %s inputstruct = %f %f %f %f %f %f %f %f %f %f %f %f %d",this->Name(), inputstruct[0].ADC_magnetic_0 , inputstruct[0].ADC_magnetic_1 , inputstruct[0].ADC_magnetic_2 , inputstruct[0].ADC_magnetic_3 , inputstruct[0].ADC_magnetic_4 , inputstruct[0].ADC_magnetic_5 , inputstruct[0].ADC_magnetic_6 , inputstruct[0].ADC_magnetic_7 , inputstruct[0].ADC_magnetic_8 , inputstruct[0].ADC_magnetic_9 , inputstruct[0].ADC_magnetic_10 , inputstruct[0].ADC_magnetic_11, inputstruct[0].usectime );
	OutputInterfaceStruct *outputstruct = (OutputInterfaceStruct *) this->SignalsOutputInterface->Buffer();
	
	int i;
	
	this->ADC_values[0] = inputstruct[0].ADC_magnetic_0;
	this->ADC_values[1] = inputstruct[0].ADC_magnetic_1;
	this->ADC_values[2] = inputstruct[0].ADC_magnetic_2;
	this->ADC_values[3] = inputstruct[0].ADC_magnetic_3;
	this->ADC_values[4] = inputstruct[0].ADC_magnetic_4;
	this->ADC_values[5] = inputstruct[0].ADC_magnetic_5;
	this->ADC_values[6] = inputstruct[0].ADC_magnetic_6;
	this->ADC_values[7] = inputstruct[0].ADC_magnetic_7;
	this->ADC_values[8] = inputstruct[0].ADC_magnetic_8;
	this->ADC_values[9] = inputstruct[0].ADC_magnetic_9;
	this->ADC_values[10] = inputstruct[0].ADC_magnetic_10;
	this->ADC_values[11] = inputstruct[0].ADC_magnetic_11;
	
	
	if(functionNumber == GAMOnline){
		// Determine the ADC offset
		if(inputstruct[0].usectime > 0 && inputstruct[0].usectime < usectime_to_wait_for_starting_operation){
			n_samples++;
			
			for(i = 0 ; i < this->NumberOfMeasurements ; i++){
				
				this->accumulator[i] += ADC_values[ProbeNumbers[i]];
				this->remove_offset[i] = this->accumulator [i] / (float) this->n_samples;
			}

			outputstruct[0].MagneticProbesR = 0.;
			outputstruct[0].MagneticProbesZ = 0.;
			outputstruct[0].MagneticProbesPlasmaCurrent = 0.;
		}
		else{
			//send offset corrections to logger once
			if (this->n_samples >0 ){

				for(i = 0 ; i < this->NumberOfMeasurements ; i++) AssertErrorCondition(Information,"MagneticsGAM::Execute: %s OFFSETS %d = %f, number of samples = %d", this->Name(), i, this->remove_offset[i], n_samples);
				n_samples = 0;
			}
			
			for(i = 0 ; i < this->NumberOfMeasurements ; i++){
				corrected_probes[i] = ADC_values[ProbeNumbers[i]] - remove_offset[i];
			}
			// Calculate H from ADC values (with integral correction)
//			for(int i = 0 ; i < this->NumberOfMeasurements ; i++)
//				Hp[i] = ((float) adcValues[ProbeNumbers[this->i]] - adcOffset[ProbeNumbers[this->i]] + this->integral_correction_constant * (this->adcAccumulator[this->ProbeNumbers[i]] - (float) this->n_samples * adcOffset[this->ProbeNumbers[i]])) * this->magnetic_Calibration[this->ProbeNumbers[i]];

			// Calculate Ip
			magnetic_field_sum = 0.0;
			for(i = 0 ; i < this->NumberOfMeasurements ; i++) magnetic_field_sum += corrected_probes[i];
			
			outputstruct[0].MagneticProbesPlasmaCurrent = magnetic_field_sum * this->plasma_current_convertion_factor;

			// Estimate radial_position and vertical_position
			radial_position = 0.0;
			vertical_position = 0.0;
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

			outputstruct[0].MagneticProbesR = radial_position;
			outputstruct[0].MagneticProbesZ = vertical_position;

			// Convert position to mm
			//	radial_position *= 1000.0;
			//	vertical_position *= 1000.0;
		} 
	}
	else {
		this->n_samples = 0;
		for(i = 0 ; i < (this->NumberOfMeasurements) ; i++){
			this->accumulator[i] = 0;
			this->remove_offset[i] = 0;
		}
		outputstruct[0].MagneticProbesPlasmaCurrent = 0;
		outputstruct[0].MagneticProbesR = 0;
		outputstruct[0].MagneticProbesZ = 0;
	}
	
	
	
	
//	outputstruct[0].MagneticProbesR = (float) inputstruct[0].ADC_magnetic_0;
//	AssertErrorCondition(InitialisationError,"MagneticsGAM:: %s outputstruct = %f %f %f",this->Name(), outputstruct[0].MagneticProbesR , outputstruct[0].MagneticProbesZ, outputstruct[0].MagneticProbesPlasmaCurrent);
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
		<br>NumberOfProbes = %d\n\
		<br>NumberOfMeasurements = %d\n\
		<br><br",magnetic_radial_bool,magnetic_vertical_bool,NumberOfProbes,NumberOfMeasurements);

		hmStream.SSPrintf(HtmlTagStreamMode, "table border=\"1\"><tr><td>magnetic_Angles</td");
		for (i=0;i<NumberOfProbes;i++)hmStream.SSPrintf(HtmlTagStreamMode, "td>%.2f</td",magnetic_Angles[i]);
		hmStream.SSPrintf(HtmlTagStreamMode, "/tr><tr><td>magnetic_Calibration</td");
		for (i=0;i<NumberOfProbes;i++)hmStream.SSPrintf(HtmlTagStreamMode, "td>%.2f</td",magnetic_Calibration[i]);
		hmStream.SSPrintf(HtmlTagStreamMode, "/tr></table><br");

		hmStream.SSPrintf(HtmlTagStreamMode, "table border=\"1\"><tr><td>ProbeNumbers</td");
		for (i=0;i<NumberOfMeasurements;i++)hmStream.SSPrintf(HtmlTagStreamMode, "td>%d</td",ProbeNumbers[i]);
		hmStream.SSPrintf(HtmlTagStreamMode, "/tr></table><br");
	}
	hmStream.SSPrintf(HtmlTagStreamMode, "/form");
	
	
	hmStream.SSPrintf(HtmlTagStreamMode, "/body>\n</html");
	hStream.SSPrintf("OutputHttpOtions.Content-Type","text/html;charset=utf-8");
	hStream.WriteReplyHeader(True);


	return True;		
}
