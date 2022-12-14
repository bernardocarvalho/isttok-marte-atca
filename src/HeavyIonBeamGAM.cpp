/*
 * File:   HeavyIonBeamGAM.cpp (based on MagneticsGAM.cpp)
 * Author: Rafael Henriques
 * Date:   6th April 2016
 * 
 */

#include "HeavyIonBeamGAM.h"

OBJECTLOADREGISTER(HeavyIonBeamGAM, "$Id: $")


//  ******** Default constructor ***********************************
HeavyIonBeamGAM::HeavyIonBeamGAM(){
	this->SignalsInputInterface = NULL;
	this->SignalsOutputInterface = NULL;
}

// ********* Destructor ********************************************
HeavyIonBeamGAM::~HeavyIonBeamGAM()
{
	//if(this->SignalsInputInterface != NULL) delete[] this->SignalsInputInterface ;
	//if(this->SignalsOutputInterface != NULL) delete[] this->SignalsOutputInterface;
}



//{ ********* Initialise the module ********************************
bool HeavyIonBeamGAM::Initialise(ConfigurationDataBase& cdbData){
   
	CDBExtended cdb(cdbData);
	
	int i;
	
//read MARTe config file section: hibd
//booleans
	if(!cdb.ReadInt32(hibd_radial_bool, "hibd_radial_bool"))
	{
		AssertErrorCondition(InitialisationError, "HeavyIonBeamGAM::Initialise: %s hibd_radial_bool", this->Name());
		return False;
	}
	else AssertErrorCondition(Information, "HeavyIonBeamGAM::Initialise: hibd_radial_bool = %d", hibd_radial_bool);
	if(!cdb.ReadInt32(hibd_vertical_bool, "hibd_vertical_bool"))
	{
		AssertErrorCondition(InitialisationError,"HeavyIonBeamGAM::Initialise: %s hibd_vertical_bool",this->Name());
		return False;
	}
	else AssertErrorCondition(Information, "HeavyIonBeamGAM::Initialise: hibd_vertical_bool = %d", hibd_vertical_bool);	
	if(!cdb.ReadInt32(hibd_pos_from_isec_bool, "hibd_pos_from_isec_bool"))
	{
		AssertErrorCondition(InitialisationError,"HeavyIonBeamGAM::Initialise: %s hibd_pos_from_isec_bool",this->Name());
		return False;
	}
	else AssertErrorCondition(Information, "HeavyIonBeamGAM::Initialise: hibd_pos_from_isec_bool = %d", hibd_pos_from_isec_bool);
	if(!cdb.ReadInt32(hibd_pos_from_nesigmasimple_bool, "hibd_pos_from_nesigmasimple_bool"))
	{
		AssertErrorCondition(InitialisationError,"HeavyIonBeamGAM::Initialise: %s hibd_pos_from_nesigmasimple_bool",this->Name());
		return False;
	}
	else AssertErrorCondition(Information, "HeavyIonBeamGAM::Initialise: hibd_pos_from_nesigmasimple_bool = %d", hibd_pos_from_nesigmasimple_bool);
//number of summed samples from the FPGA
	if(!cdb.ReadInt32(hibd_nav, "hibd_nav"))
	{
		AssertErrorCondition(InitialisationError, "HeavyIonBeamGAM::Initialise: %s hibd_nav", this->Name());
		return False;
	}
	else AssertErrorCondition(Information, "HeavyIonBeamGAM::Initialise: hibd_nav = %d", hibd_nav);
		if(hibd_nav < 1){
			AssertErrorCondition(InitialisationError,"HeavyIonBeamGAM::Initialise: %s hibd_nav lower than 1",this->Name());
			return False;
		}
		  
		  
//HIBD detector description
	if(!cdb->Move("hibd_detector_description"))
	{
		AssertErrorCondition(InitialisationError,"HeavyIonBeamGAM::Initialise: %s Could not move to \"+MARTe.+ISTTOK_RTTh.+hibd.hibd_detector_description\"",this->Name());
		return False;
	}
		if(!cdb.ReadInt32(no_of_sec_hibd_chs, "no_of_sec_hibd_chs"))
		{
			AssertErrorCondition(InitialisationError,"HeavyIonBeamGAM::Initialise: %s no_of_sec_hibd_chs",this->Name());
			return False;
		}
		else	AssertErrorCondition(Information,"HeavyIonBeamGAM::Initialise: no_of_sec_hibd_chs = %d",no_of_sec_hibd_chs);
		
		if (no_of_sec_hibd_chs > 0){
			hibd_sec_chs_Zs = new float[no_of_sec_hibd_chs];
			hibd_sec_chs_dls = new float[no_of_sec_hibd_chs];

			if(!cdb.ReadFloatArray(hibd_sec_chs_Zs, (int *)(&no_of_sec_hibd_chs), 1, "positions"))
			{
				AssertErrorCondition(InitialisationError,"HeavyIonBeamGAM: Could not read hibd_sec_chs_Zs");
				return False;
			}
			else for(i=0;i<no_of_sec_hibd_chs;i++) AssertErrorCondition(Information,"HeavyIonBeamGAM::Initialise: hibd_sec_chs_Zs[%d] = %f",i, hibd_sec_chs_Zs[i]);
			if(!cdb.ReadFloatArray(hibd_sec_chs_dls, (int *)(&no_of_sec_hibd_chs), 1, "dls"))
			{
				AssertErrorCondition(InitialisationError,"HeavyIonBeamGAM: Could not read hibd_sec_chs_dls");
				return False;
			}
			else for(i=0;i<no_of_sec_hibd_chs;i++) AssertErrorCondition(Information,"HeavyIonBeamGAM::Initialise: hibd_sec_chs_dls[%d] = %f",i, hibd_sec_chs_dls[i]);
		}
		else {
			AssertErrorCondition(InitialisationError,"HeavyIonBeamGAM::Initialise: %s no_of_sec_hibd_chs lower than 1",this->Name());
			return False;
		}		
	cdb->MoveToFather();
//HIB primary current characteristics
	if(!cdb.ReadFloat(hibd_iprim_i, "hibd_iprim_i"))
	{
		AssertErrorCondition(InitialisationError, "HeavyIonBeamGAM::Initialise: %s hibd_iprim_i", this->Name());
		return False;
	}
	else AssertErrorCondition(Information, "HeavyIonBeamGAM::Initialise: hibd_iprim_i = %f", hibd_iprim_i);
		if(hibd_iprim_i <= 0){
			AssertErrorCondition(InitialisationError,"HeavyIonBeamGAM::Initialise: %s hibd_iprim_i lower than or equal to 0",this->Name());
			return False;
		}
	if(!cdb.ReadFloat(hibd_iprim_f, "hibd_iprim_f"))
	{
		AssertErrorCondition(InitialisationError, "HeavyIonBeamGAM::Initialise: %s hibd_iprim_f", this->Name());
		return False;
	}
	else AssertErrorCondition(Information, "HeavyIonBeamGAM::Initialise: hibd_iprim_f = %f", hibd_iprim_f);
		if(hibd_iprim_f <= 0){
			AssertErrorCondition(InitialisationError,"HeavyIonBeamGAM::Initialise: %s hibd_iprim_f lower than or equal to 0",this->Name());
			return False;
		}
	if(!cdb.ReadFloat(hibd_iprim_dt, "hibd_iprim_dt"))
	{
		AssertErrorCondition(InitialisationError, "HeavyIonBeamGAM::Initialise: %s hibd_iprim_dt", this->Name());
		return False;
	}
	else AssertErrorCondition(Information, "HeavyIonBeamGAM::Initialise: hibd_iprim_dt = %f", hibd_iprim_dt);
		if(hibd_iprim_dt <= 0){
			AssertErrorCondition(InitialisationError,"HeavyIonBeamGAM::Initialise: %s hibd_iprim_dt lower than or equal to 0",this->Name());
			return False;
		}
//Calibration values
	if(!cdb.ReadFloat(hibd_radial_constant, "hibd_radial_constant"))
	{
		AssertErrorCondition(InitialisationError, "HeavyIonBeamGAM::Initialise: %s hibd_radial_constant", this->Name());
		return False;
	}
	else AssertErrorCondition(Information, "HeavyIonBeamGAM::Initialise: hibd_radial_constant = %f", hibd_radial_constant);
	if(!cdb.ReadFloat(hibd_radial_offset, "hibd_radial_offset"))
	{
		AssertErrorCondition(InitialisationError, "HeavyIonBeamGAM::Initialise: %s hibd_radial_offset", this->Name());
		return False;
	}
	else AssertErrorCondition(Information, "HeavyIonBeamGAM::Initialise: hibd_radial_offset = %f", hibd_radial_offset);
	if(!cdb.ReadFloat(hibd_vertical_constant, "hibd_vertical_constant"))
	{
		AssertErrorCondition(InitialisationError, "HeavyIonBeamGAM::Initialise: %s hibd_vertical_constant", this->Name());
		return False;
	}
	else AssertErrorCondition(Information, "HeavyIonBeamGAM::Initialise: hibd_vertical_constant = %f", hibd_vertical_constant);
	if(!cdb.ReadFloat(hibd_vertical_offset, "hibd_vertical_offset"))
	{
		AssertErrorCondition(InitialisationError, "HeavyIonBeamGAM::Initialise: %s hibd_vertical_offset", this->Name());
		return False;
	}
	else AssertErrorCondition(Information, "HeavyIonBeamGAM::Initialise: hibd_vertical_offset = %f", hibd_vertical_offset);
//Thresold values
	if(!cdb.ReadFloat(hibd_isec_total_threshold, "hibd_isec_total_threshold"))
	{
		AssertErrorCondition(InitialisationError, "HeavyIonBeamGAM::Initialise: %s hibd_isec_total_threshold", this->Name());
		return False;
	}
	else AssertErrorCondition(Information, "HeavyIonBeamGAM::Initialise: hibd_isec_total_threshold = %f", hibd_isec_total_threshold);
		if(hibd_isec_total_threshold < 0){
			AssertErrorCondition(InitialisationError,"HeavyIonBeamGAM::Initialise: %s hibd_isec_total_threshold lower than 0",this->Name());
			return False;
		}
	if(!cdb.ReadFloat(hibd_nesigmasimple_total_threshold, "hibd_nesigmasimple_total_threshold"))
	{
		AssertErrorCondition(InitialisationError, "HeavyIonBeamGAM::Initialise: %s hibd_nesigmasimple_total_threshold", this->Name());
		return False;
	}
	else AssertErrorCondition(Information, "HeavyIonBeamGAM::Initialise: hibd_nesigmasimple_total_threshold = %f", hibd_nesigmasimple_total_threshold);
		if(hibd_nesigmasimple_total_threshold < 0){
			AssertErrorCondition(InitialisationError,"HeavyIonBeamGAM::Initialise: %s hibd_nesigmasimple_total_threshold lower than 0",this->Name());
			return False;
		}

//Create the signal interfaces
	if(!AddInputInterface(this->SignalsInputInterface, "HeavyIonBeamGAMInputInterface"))
	{
		AssertErrorCondition(InitialisationError, "HeavyIonBeamGAM::Initialise: %s failed to add the HeavyIonBeamGAMInputInterface", this->Name());
		return False;
	}
	if(!AddOutputInterface(this->SignalsOutputInterface, "HeavyIonBeamGAMOutputInterface"))
	{
		AssertErrorCondition(InitialisationError, "HeavyIonBeamGAM::Initialise: %s failed to add the HeavyIonBeamGAMOutputInterface", this->Name());
		return False;
	}

//INPUT SIGNALS (interface)
	if(!cdb->Move("input_signals"))
	{
		AssertErrorCondition(InitialisationError,"HeavyIonBeamGAM::Initialise: %s Could not move to \"input_signals\"",this->Name());
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
				AssertErrorCondition(InitialisationError, "HeavyIonBeamGAM::Initialise: %s Could not move to \"%s\"", this->Name(), CDB_move_to[i].Buffer());
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
				AssertErrorCondition(Information,"HeavyIonBeamGAM::Initialise: Added signal = %s", SignalName.Buffer());
				
				if(!this->SignalsInputInterface->AddSignal(SignalName.Buffer(), SignalType[i].Buffer()))
				{
					AssertErrorCondition(InitialisationError,"HeavyIonBeamGAM::Initialise: %s failed to add signal", this->Name());
					return False;
				}
			}
			cdb->MoveToFather();
		}
	cdb->MoveToFather();


//OUTPUT SIGNALS (interface)
	if(!cdb->Move("output_signals"))
	{
		AssertErrorCondition(InitialisationError,"HeavyIonBeamGAM::Initialise: %s Could not move to \"output_signals\"",this->Name());
		return False;
	}
	
		int number_of_signals_to_write = 9;
		CDB_move_to = new FString[number_of_signals_to_write];
		SignalType = new FString[number_of_signals_to_write];
		CDB_move_to[0].Printf("hibd_r");
		CDB_move_to[1].Printf("hibd_z");
		CDB_move_to[2].Printf("hibd_isec_total");
		CDB_move_to[3].Printf("hibd_r_isec");
		CDB_move_to[4].Printf("hibd_z_isec");
		CDB_move_to[5].Printf("hibd_iprim_0");
		CDB_move_to[6].Printf("hibd_nesigmasimple_total");
		CDB_move_to[7].Printf("hibd_r_nesigmasimple");
		CDB_move_to[8].Printf("hibd_z_nesigmasimple");
		for (i=0;i<number_of_signals_to_write;i++){
			
			if(!cdb->Move(CDB_move_to[i].Buffer()))
			{
				AssertErrorCondition(InitialisationError,"HeavyIonBeamGAM::Initialise: %s Could not move to \"%s\"",this->Name(),CDB_move_to[i].Buffer());
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
				AssertErrorCondition(Information,"HeavyIonBeamGAM::Initialise: Added signal = %s", SignalName.Buffer());
				
				if(!this->SignalsOutputInterface->AddSignal(SignalName.Buffer(), SignalType[i].Buffer()))
				{
					AssertErrorCondition(InitialisationError,"HeavyIonBeamGAM::Initialise: %s failed to add signal", this->Name());
					return False;
				}
			}
			cdb->MoveToFather();
		}
	cdb->MoveToFather();
	
	//creating array to store the HIBD sec. currents
	currents = new float [no_of_sec_hibd_chs];
	
	//convert values from the FPGA to currents in nA
	// 50 (1e9 / 20e+6) convert V to nA
	// 10 because ADC goes from -5V to 5V
	// 1/hibd_nav makes the average of the summed samples
	// 2^18=262144 because it is a 18bit ADC
	this->conv_to_nA = (50.0 * 10.0) / (hibd_nav * 262144.0);
	
	//HIB primary current
	this->iprim_a = (hibd_iprim_f - hibd_iprim_i) / (hibd_iprim_dt * 1.0e6);
	this->iprim_b = hibd_iprim_i;
	iprim_j = new float [no_of_sec_hibd_chs];
	
	//NeSigmaSimple array
	ne_sigma_simple = new float [no_of_sec_hibd_chs];
	
	//limiter (mm)
	this->out_of_bounds_limit = 85.0;

	return True;
}
//} ****************************************************************


//{ ********* Execute the module functionalities *******************
bool HeavyIonBeamGAM::Execute(GAM_FunctionNumbers functionNumber){
 
	InputInterfaceStruct *inputstruct = (InputInterfaceStruct *) this->SignalsInputInterface->Buffer();
	this->SignalsInputInterface->Read();
//	AssertErrorCondition(InitialisationError,"HeavyIonBeamGAM:: %s inputstruct = %f %f %f %f %f %f %f %f",this->Name(), inputstruct[0].ADC_electric_top_far , inputstruct[0].ADC_electric_top_near , inputstruct[0].ADC_electric_inner_far , inputstruct[0].ADC_electric_inner_near , inputstruct[0].ADC_electric_outer_far , inputstruct[0].ADC_electric_outer_near , inputstruct[0].ADC_electric_bottom_far , inputstruct[0].ADC_electric_bottom_near );

	OutputInterfaceStruct *outputstruct = (OutputInterfaceStruct *) this->SignalsOutputInterface->Buffer();
	
	int i; //For iteration
	
	if(functionNumber == GAMOnline){
		
		currents[0] = (float) inputstruct[0].HIBD_Sec_01 * this->conv_to_nA;
		currents[1] = (float) inputstruct[0].HIBD_Sec_02 * this->conv_to_nA;
		currents[2] = (float) inputstruct[0].HIBD_Sec_03 * this->conv_to_nA;
		currents[3] = (float) inputstruct[0].HIBD_Sec_04 * this->conv_to_nA;
		currents[4] = (float) inputstruct[0].HIBD_Sec_05 * this->conv_to_nA;
		currents[5] = (float) inputstruct[0].HIBD_Sec_06 * this->conv_to_nA;
		currents[6] = (float) inputstruct[0].HIBD_Sec_07 * this->conv_to_nA;
		currents[7] = (float) inputstruct[0].HIBD_Sec_08 * this->conv_to_nA;
		currents[8] = (float) inputstruct[0].HIBD_Sec_09 * this->conv_to_nA;
		currents[9] = (float) inputstruct[0].HIBD_Sec_10 * this->conv_to_nA;
		currents[10] = (float) inputstruct[0].HIBD_Sec_11 * this->conv_to_nA;
		currents[11] = (float) inputstruct[0].HIBD_Sec_12 * this->conv_to_nA;
		
		/////////////////////////////////////////////////
		// Radial position (currently not implemented) //
		/////////////////////////////////////////////////
		if(hibd_radial_bool){
			
			outputstruct[0].HeavyIonBeamRISec = 0.0;
			outputstruct[0].HeavyIonBeamRNeSigmaSimple = 0.0;
			
			/* convert from mm to m */
			outputstruct[0].HeavyIonBeamRISec = outputstruct[0].HeavyIonBeamRISec*0.001;
			outputstruct[0].HeavyIonBeamRNeSigmaSimple = outputstruct[0].HeavyIonBeamRNeSigmaSimple*0.001;
			
			// Output //
			/*select which algorithm is used to feedback the controller*/
			if(hibd_pos_from_isec_bool) outputstruct[0].HeavyIonBeamR = outputstruct[0].HeavyIonBeamRISec;
			if(hibd_pos_from_nesigmasimple_bool) outputstruct[0].HeavyIonBeamR = outputstruct[0].HeavyIonBeamRNeSigmaSimple;
			/*linear calibration*/
			outputstruct[0].HeavyIonBeamR = hibd_radial_constant*outputstruct[0].HeavyIonBeamR + hibd_radial_offset;
			/*limit the output*/
			if(outputstruct[0].HeavyIonBeamR > this->out_of_bounds_limit) outputstruct[0].HeavyIonBeamR = this->out_of_bounds_limit;
			if(outputstruct[0].HeavyIonBeamR < -this->out_of_bounds_limit) outputstruct[0].HeavyIonBeamR = -this->out_of_bounds_limit;
			/* equal the position output to 0 when the calculated position cannot be trusted */
// 			if(hibd_pos_from_isec_bool){
// 				if(outputstruct[0].HeavyIonBeamISecTotal < hibd_isec_total_threshold) outputstruct[0].HeavyIonBeamR = 0.0;
// 			}
// 			if(hibd_pos_from_nesigmasimple_bool){
// 				if(outputstruct[0].HeavyIonBeamNeSigmaSimpleTotal < hibd_nesigmasimple_total_threshold) outputstruct[0].HeavyIonBeamR = 0.0;
// 			}
		}
		else{
			outputstruct[0].HeavyIonBeamRISec = 0.0;
			outputstruct[0].HeavyIonBeamRNeSigmaSimple = 0.0;
			outputstruct[0].HeavyIonBeamR = 0.0;
		}
		
		///////////////////////
		// Vertical position //
		///////////////////////
		if(hibd_vertical_bool){
			
			// ISec //
			/* calculate the centre of mass */
			outputstruct[0].HeavyIonBeamISecTotal = 0.0;
			outputstruct[0].HeavyIonBeamZISec = 0.0;
			for(i=0; i < this->no_of_sec_hibd_chs; i++){
				outputstruct[0].HeavyIonBeamISecTotal += currents[i];
				outputstruct[0].HeavyIonBeamZISec += currents[i]*hibd_sec_chs_Zs[i];
			}
			/* avoid negative currents and division by zero */
			if(outputstruct[0].HeavyIonBeamISecTotal <= 0) outputstruct[0].HeavyIonBeamISecTotal = 0.001;
			outputstruct[0].HeavyIonBeamZISec = outputstruct[0].HeavyIonBeamZISec / outputstruct[0].HeavyIonBeamISecTotal;
			/* truncate the position to the out of bounds limits */
			if(outputstruct[0].HeavyIonBeamZISec > this->out_of_bounds_limit) outputstruct[0].HeavyIonBeamZISec = this->out_of_bounds_limit;
			if(outputstruct[0].HeavyIonBeamZISec < -this->out_of_bounds_limit) outputstruct[0].HeavyIonBeamZISec = -this->out_of_bounds_limit;
			/* convert from mm to m */
			outputstruct[0].HeavyIonBeamZISec = outputstruct[0].HeavyIonBeamZISec*0.001;
			
			// NeSigmaSimple //
			/* calculate the injected primary beam current (it should be always constant, but it is not during the beginning of the shot)*/
			if(inputstruct[0].usectime <= hibd_iprim_dt*1.0e6)
				outputstruct[0].HeavyIonBeamIprim0 = this->iprim_a * (float) inputstruct[0].usectime + this->iprim_b;
			else
				outputstruct[0].HeavyIonBeamIprim0 = hibd_iprim_f;
			/* calculate the ne_sigma_simple */
			iprim_j[0] = outputstruct[0].HeavyIonBeamIprim0;
			ne_sigma_simple[0] = currents[0] / (2.0*iprim_j[0]*hibd_sec_chs_dls[0]*0.001); //*0.001 convert from 1/mm to 1/m
			for(i=1; i < this->no_of_sec_hibd_chs; i++){
				iprim_j[i] = iprim_j[i-1] - currents[i-1]/2.0;
				ne_sigma_simple[i] = currents[i] / (2.0*iprim_j[i]*hibd_sec_chs_dls[i]*0.001); //*0.001 convert from 1/mm to 1/m
			}
			/* calculate the centre of mass */
			outputstruct[0].HeavyIonBeamNeSigmaSimpleTotal = 0.0;
			outputstruct[0].HeavyIonBeamZNeSigmaSimple = 0.0;
			for(i=0; i < this->no_of_sec_hibd_chs; i++){
				outputstruct[0].HeavyIonBeamNeSigmaSimpleTotal += ne_sigma_simple[i];
				outputstruct[0].HeavyIonBeamZNeSigmaSimple += ne_sigma_simple[i]*hibd_sec_chs_Zs[i];
			}
			/* avoid negative ne_sigma_simple and division by zero */
			if(outputstruct[0].HeavyIonBeamNeSigmaSimpleTotal <= 0) outputstruct[0].HeavyIonBeamNeSigmaSimpleTotal = 0.0001;
			outputstruct[0].HeavyIonBeamZNeSigmaSimple = outputstruct[0].HeavyIonBeamZNeSigmaSimple / outputstruct[0].HeavyIonBeamNeSigmaSimpleTotal;
			/*truncate the position to the out of bounds limits*/
			if(outputstruct[0].HeavyIonBeamZNeSigmaSimple > this->out_of_bounds_limit) outputstruct[0].HeavyIonBeamZNeSigmaSimple = this->out_of_bounds_limit;
			if(outputstruct[0].HeavyIonBeamZNeSigmaSimple < -this->out_of_bounds_limit) outputstruct[0].HeavyIonBeamZNeSigmaSimple = -this->out_of_bounds_limit;
			/* convert from mm to m */			
			outputstruct[0].HeavyIonBeamZNeSigmaSimple = outputstruct[0].HeavyIonBeamZNeSigmaSimple*0.001;
			
			// Output //
			/* select which algorithm is used to feedback the controller */
			if(hibd_pos_from_isec_bool) outputstruct[0].HeavyIonBeamZ = outputstruct[0].HeavyIonBeamZISec;
			if(hibd_pos_from_nesigmasimple_bool) outputstruct[0].HeavyIonBeamZ = outputstruct[0].HeavyIonBeamZNeSigmaSimple;
			/* linear calibration */
			outputstruct[0].HeavyIonBeamZ = hibd_vertical_constant*outputstruct[0].HeavyIonBeamZ + hibd_vertical_offset;
			/* equal the position output to 0 when the calculated position cannot be trusted */
			if(hibd_pos_from_isec_bool){
				if(outputstruct[0].HeavyIonBeamISecTotal < hibd_isec_total_threshold) outputstruct[0].HeavyIonBeamZ = 0.0;
			}
			if(hibd_pos_from_nesigmasimple_bool){
				if(outputstruct[0].HeavyIonBeamNeSigmaSimpleTotal < hibd_nesigmasimple_total_threshold) outputstruct[0].HeavyIonBeamZ = 0.0;
			}
		}
		else{
			outputstruct[0].HeavyIonBeamISecTotal = 0.0;
			outputstruct[0].HeavyIonBeamZISec = 0.0;
			outputstruct[0].HeavyIonBeamIprim0 = 0.0;
			outputstruct[0].HeavyIonBeamZNeSigmaSimple = 0.0;
			outputstruct[0].HeavyIonBeamNeSigmaSimpleTotal = 0.0;
			outputstruct[0].HeavyIonBeamZ = 0.0;
		}
		
	}
	else {
	  
		for(i=0; i<this->no_of_sec_hibd_chs; i++){
			currents[i] = 0.0;
			iprim_j[i] = 0.0;
			ne_sigma_simple[i] = 0.0;
		}
		outputstruct[0].HeavyIonBeamRISec = 0.0;
		outputstruct[0].HeavyIonBeamRNeSigmaSimple = 0.0;
		outputstruct[0].HeavyIonBeamR = 0.0;
		outputstruct[0].HeavyIonBeamZISec = 0.0;
		outputstruct[0].HeavyIonBeamZNeSigmaSimple = 0.0;
		outputstruct[0].HeavyIonBeamZ = 0.0;
		outputstruct[0].HeavyIonBeamISecTotal = 0.0;
		outputstruct[0].HeavyIonBeamNeSigmaSimpleTotal = 0.0;
		outputstruct[0].HeavyIonBeamIprim0 = 0.0;
		
	}
		
//		AssertErrorCondition(InitialisationError,"HeavyIonBeamGAM:: %s outputstruct = %f %f",this->Name(), outputstruct[0].HeavyIonBeamR , outputstruct[0].HeavyIonBeamZ );
	this->SignalsOutputInterface->Write();
	
	return True;		
}
bool HeavyIonBeamGAM::ProcessHttpMessage(HttpStream &hStream){
	
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
		hmStream.SSPrintf(HtmlTagStreamMode, "br><br>hibd_radial_bool = %d\n\
		<br>hibd_vertical_bool = %d\n\
		<br>hibd_pos_from_isec_bool = %d\n\
		<br>hibd_pos_from_nesigmasimple_bool = %d\n\
		<br>hibd_nav = %d\n\
		<br><br",hibd_radial_bool, hibd_vertical_bool, hibd_pos_from_isec_bool, hibd_pos_from_nesigmasimple_bool, hibd_nav);
		
		hmStream.SSPrintf(HtmlTagStreamMode, "br><br>no_of_sec_hibd_chs = %d<br><br",no_of_sec_hibd_chs);
		hmStream.SSPrintf(HtmlTagStreamMode, "table border=\"1\"><tr><td>positions</td");
		for (i=0;i<no_of_sec_hibd_chs;i++)hmStream.SSPrintf(HtmlTagStreamMode, "td>%.2f</td",hibd_sec_chs_Zs[i]);
		hmStream.SSPrintf(HtmlTagStreamMode, "/tr><tr><td>dls</td");
		for (i=0;i<no_of_sec_hibd_chs;i++)hmStream.SSPrintf(HtmlTagStreamMode, "td>%.2f</td",hibd_sec_chs_dls[i]);
		hmStream.SSPrintf(HtmlTagStreamMode, "/tr></table>\n<br");
		
		hmStream.SSPrintf(HtmlTagStreamMode, "br><br>hibd_iprim_i = %f\n\
		<br>hibd_iprim_f = %f\n\
		<br>hibd_iprim_dt = %f\n\
		<br><br",hibd_iprim_i, hibd_iprim_f, hibd_iprim_dt);
		
		hmStream.SSPrintf(HtmlTagStreamMode, "br><br>hibd_radial_constant = %f\n\
		<br>hibd_radial_offset = %f\n\
		<br>hibd_vertical_constant = %f\n\
		<br>hibd_vertical_offset = %f\n\
		<br><br",hibd_radial_constant, hibd_radial_offset, hibd_vertical_constant, hibd_vertical_offset);
		
		hmStream.SSPrintf(HtmlTagStreamMode, "br><br>hibd_isec_total_threshold = %f\n\
		<br>hibd_nesigmasimple_total_threshold = %f\
		<br><br",hibd_isec_total_threshold, hibd_nesigmasimple_total_threshold);
		
	}
	hmStream.SSPrintf(HtmlTagStreamMode, "/form");
  
	hmStream.SSPrintf(HtmlTagStreamMode, "/body>\n</html");
	hStream.SSPrintf("OutputHttpOtions.Content-Type","text/html;charset=utf-8");
	hStream.WriteReplyHeader(True);

	return True;		
}
