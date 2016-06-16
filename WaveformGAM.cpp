
#include "WaveformGAM.h"

OBJECTLOADREGISTER(WaveformGAM, "$Id: $")


//  ******** Default constructor ***********************************
WaveformGAM::WaveformGAM(){
	this->SignalsOutputInterface = NULL;
	this->SignalsInputInterface = NULL;
	
}




// ********* Destructor ********************************************
WaveformGAM::~WaveformGAM()
{
//	if(this->SignalsInputInterface != NULL) delete[] this->SignalsInputInterface ;
//	if(this->SignalsOutputInterface != NULL) delete[] this->SignalsOutputInterface;
}



//{ ********* Initialise the module ********************************
bool WaveformGAM::Initialise(ConfigurationDataBase& cdbData){
   
	CDBExtended cdb(cdbData);
	int i;
	
	if(!cdb->Move("waveform_mode_1_positive"))
	{
		AssertErrorCondition(InitialisationError,"WaveformGAM::Initialise: %s Could not move to \"waveform_mode_1_positive\"",this->Name());
		return False;
	}
		if(!cdb.ReadInt32(waveform_1_p_vector_size, "vector_size"))
		{
			AssertErrorCondition(InitialisationError,"WaveformGAM::Initialise: %s waveform_1_p_vector_size",this->Name());
			return False;
		}
		else	AssertErrorCondition(Information,"WaveformGAM::Initialise: %s waveform_1_p_vector_size = %d", this->Name(),waveform_1_p_vector_size);
		if (waveform_1_p_vector_size > 1){
			waveform_1_p_available = True;
			waveform_1_p_index_vector =new float[waveform_1_p_vector_size];
			waveform_1_p_data_vector =new float[waveform_1_p_vector_size];

			if(!cdb.ReadFloatArray(waveform_1_p_index_vector, (int *)(&waveform_1_p_vector_size), 1, "index_vector"))
			{
				AssertErrorCondition(InitialisationError,"ReadWaveformFiles: %s Could not read waveform_1_p_index_vector", this->Name());
				return False;
			}
			else for(i=0;i<waveform_1_p_vector_size;i++) AssertErrorCondition(Information,"WaveformGAM::Initialise: %s waveform_1_p_index_vector[%d] = %f", this->Name(),i, waveform_1_p_index_vector[i]);
			if(!cdb.ReadFloatArray(waveform_1_p_data_vector, (int *)(&waveform_1_p_vector_size), 1, "data_vector"))
			{
				AssertErrorCondition(InitialisationError,"ReadWaveformFiles: Could not read waveform_1_p_data_vector");
				return False;
			}
			else for(i=0;i<waveform_1_p_vector_size;i++) AssertErrorCondition(Information,"WaveformGAM::Initialise: %s waveform_1_p_data_vector[%d] = %f", this->Name(),i, waveform_1_p_data_vector[i]);
		}
		else waveform_1_p_available = False;
			if(!cdb.ReadFloat(waveform_1_p_max_value, "max_value"))
			{
				AssertErrorCondition(InitialisationError,"WaveformGAM::Initialise: %s waveform_1_p_max_value",this->Name());
				return False;
			}
			else	AssertErrorCondition(Information,"WaveformGAM::Initialise: %s waveform_1_p_max_value = %f", this->Name(),waveform_1_p_max_value);
			if(!cdb.ReadFloat(waveform_1_p_min_value, "min_value"))
			{
				AssertErrorCondition(InitialisationError,"WaveformGAM::Initialise: %s waveform_1_p_min_value",this->Name());
				return False;
			}
			else	AssertErrorCondition(Information,"WaveformGAM::Initialise: %s waveform_1_p_min_value = %f", this->Name(),waveform_1_p_min_value);

	cdb->MoveToFather();	
	
	if(!cdb->Move("waveform_mode_1_negative"))
	{
		AssertErrorCondition(InitialisationError,"WaveformGAM::Initialise: %s Could not move to \"+MARTe.+ISTTOK_RTTh.+waveform_waveform.waveform_mode_1_negative\"",this->Name());
		return False;
	}
		if(!cdb.ReadInt32(waveform_1_n_vector_size, "vector_size"))
		{
			AssertErrorCondition(InitialisationError,"WaveformGAM::Initialise: %s waveform_1_n_vector_size",this->Name());
			return False;
		}
		else	AssertErrorCondition(Information,"WaveformGAM::Initialise: %s waveform_1_n_vector_size = %d", this->Name(),waveform_1_n_vector_size);
		if (waveform_1_n_vector_size > 1){
			waveform_1_n_available = True;
			waveform_1_n_index_vector =new float[waveform_1_n_vector_size];
			waveform_1_n_data_vector =new float[waveform_1_n_vector_size];

			if(!cdb.ReadFloatArray(waveform_1_n_index_vector, (int *)(&waveform_1_n_vector_size), 1, "index_vector"))
			{
				AssertErrorCondition(InitialisationError,"ReadWaveformFiles: %s Could not read waveform_1_n_index_vector", this->Name());
				return False;
			}
			else for(i=0;i<waveform_1_n_vector_size;i++) AssertErrorCondition(Information,"WaveformGAM::Initialise: %s waveform_1_n_index_vector[%d] = %f", this->Name(),i, waveform_1_n_index_vector[i]);
			if(!cdb.ReadFloatArray(waveform_1_n_data_vector, (int *)(&waveform_1_n_vector_size), 1, "data_vector"))
			{
				AssertErrorCondition(InitialisationError,"ReadWaveformFiles: Could not read waveform_1_n_data_vector");
				return False;
			}
			else for(i=0;i<waveform_1_n_vector_size;i++) AssertErrorCondition(Information,"WaveformGAM::Initialise: %s waveform_1_n_data_vector[%d] = %f", this->Name(),i, waveform_1_n_data_vector[i]);
		}
		else waveform_1_n_available = False;
	cdb->MoveToFather();
	
	if(!cdb->Move("waveform_mode_2_positive"))
	{
		AssertErrorCondition(InitialisationError,"WaveformGAM::Initialise: %s Could not move to \"+MARTe.+ISTTOK_RTTh.+waveform_waveform.waveform_mode_2_positive\"",this->Name());
		return False;
	}
		if(!cdb.ReadInt32(waveform_2_p_vector_size, "vector_size"))
		{
			AssertErrorCondition(InitialisationError,"WaveformGAM::Initialise: %s waveform_2_p_vector_size",this->Name());
			return False;
		}
		else	AssertErrorCondition(Information,"WaveformGAM::Initialise: %s waveform_2_p_vector_size = %d", this->Name(),waveform_2_p_vector_size);
		if (waveform_2_p_vector_size > 1){
			waveform_2_p_available = True;
			waveform_2_p_index_vector =new float[waveform_2_p_vector_size];
			waveform_2_p_data_vector =new float[waveform_2_p_vector_size];

			if(!cdb.ReadFloatArray(waveform_2_p_index_vector, (int *)(&waveform_2_p_vector_size), 1, "index_vector"))
			{
				AssertErrorCondition(InitialisationError,"ReadWaveformFiles: %s Could not read waveform_2_p_index_vector", this->Name());
				return False;
			}
			else for(i=0;i<waveform_2_p_vector_size;i++) AssertErrorCondition(Information,"WaveformGAM::Initialise: %s waveform_2_p_index_vector[%d] = %f", this->Name(),i, waveform_2_p_index_vector[i]);
			if(!cdb.ReadFloatArray(waveform_2_p_data_vector, (int *)(&waveform_2_p_vector_size), 1, "data_vector"))
			{
				AssertErrorCondition(InitialisationError,"ReadWaveformFiles: Could not read waveform_1_p_data_vector");
				return False;
			}
			else for(i=0;i<waveform_2_p_vector_size;i++) AssertErrorCondition(Information,"WaveformGAM::Initialise: %s waveform_2_p_data_vector[%d] = %f", this->Name(),i, waveform_2_p_data_vector[i]);
		}
		else waveform_2_p_available = False;
			if(!cdb.ReadFloat(waveform_2_p_max_value, "max_value"))
			{
				AssertErrorCondition(InitialisationError,"WaveformGAM::Initialise: %s waveform_2_p_max_value",this->Name());
				return False;
			}
			else	AssertErrorCondition(Information,"WaveformGAM::Initialise: %s waveform_2_p_max_value = %f", this->Name(),waveform_2_p_max_value);
			if(!cdb.ReadFloat(waveform_2_p_min_value, "min_value"))
			{
				AssertErrorCondition(InitialisationError,"WaveformGAM::Initialise: %s waveform_2_p_min_value",this->Name());
				return False;
			}
			else	AssertErrorCondition(Information,"WaveformGAM::Initialise: %s waveform_2_p_min_value = %f", this->Name(),waveform_2_p_min_value);

	cdb->MoveToFather();	
	
	if(!cdb->Move("waveform_mode_2_negative"))
	{
		AssertErrorCondition(InitialisationError,"WaveformGAM::Initialise: %s Could not move to \"+MARTe.+ISTTOK_RTTh.+waveform_waveform.waveform_mode_2_negative\"",this->Name());
		return False;
	}
		if(!cdb.ReadInt32(waveform_2_n_vector_size, "vector_size"))
		{
			AssertErrorCondition(InitialisationError,"WaveformGAM::Initialise: %s waveform_2_n_vector_size",this->Name());
			return False;
		}
		else	AssertErrorCondition(Information,"WaveformGAM::Initialise: %s waveform_2_n_vector_size = %d", this->Name(),waveform_2_n_vector_size);

		if (waveform_2_n_vector_size > 1){
			waveform_2_n_available = True;
			waveform_2_n_index_vector =new float[waveform_2_n_vector_size];
			waveform_2_n_data_vector =new float[waveform_2_n_vector_size];

			if(!cdb.ReadFloatArray(waveform_2_n_index_vector, (int *)(&waveform_2_n_vector_size), 1, "index_vector"))
			{
				AssertErrorCondition(InitialisationError,"ReadWaveformFiles: %s Could not read waveform_2_n_index_vector", this->Name());
				return False;
			}
			else for(i=0;i<waveform_2_n_vector_size;i++) AssertErrorCondition(Information,"WaveformGAM::Initialise: %s waveform_2_n_index_vector[%d] = %f", this->Name(),i, waveform_1_n_index_vector[i]);
			if(!cdb.ReadFloatArray(waveform_2_n_data_vector, (int *)(&waveform_2_n_vector_size), 1, "data_vector"))
			{
				AssertErrorCondition(InitialisationError,"ReadWaveformFiles: Could not read waveform_2_n_data_vector");
				return False;
			}
			else for(i=0;i<waveform_2_n_vector_size;i++) AssertErrorCondition(Information,"WaveformGAM::Initialise: %s waveform_2_n_data_vector[%d] = %f", this->Name(),i, waveform_2_n_data_vector[i]);
		}
		else waveform_2_n_available = False;
	cdb->MoveToFather();
	
	if(!cdb->Move("waveform_breakdown"))
	{
		AssertErrorCondition(InitialisationError,"WaveformGAM::Initialise: %s Could not move to \"+MARTe.+ISTTOK_RTTh.+waveform_waveform.waveform_mode_3_positive\"",this->Name());
		return False;
	}
		if(!cdb.ReadInt32(waveform_breakdown_vector_size, "vector_size"))
		{
			AssertErrorCondition(InitialisationError,"WaveformGAM::Initialise: %s waveform_breakdown_vector_size",this->Name());
			return False;
		}
		else	AssertErrorCondition(Information,"WaveformGAM::Initialise: %s waveform_breakdown_vector_size = %d", this->Name(),waveform_breakdown_vector_size);

		if (waveform_breakdown_vector_size > 1){
			waveform_breakdown_available = True;
			waveform_breakdown_index_vector =new float[waveform_breakdown_vector_size];
			waveform_breakdown_data_vector =new float[waveform_breakdown_vector_size];

			if(!cdb.ReadFloatArray(waveform_breakdown_index_vector, (int *)(&waveform_breakdown_vector_size), 1, "index_vector"))
			{
				AssertErrorCondition(InitialisationError,"ReadWaveformFiles: %s Could not read waveform_breakdown_index_vector", this->Name());
				return False;
			}
			else for(i=0;i<waveform_breakdown_vector_size;i++) AssertErrorCondition(Information,"WaveformGAM::Initialise: %swaveform_breakdown_index_vector[%d] = %f", this->Name(),i, waveform_breakdown_index_vector[i]);
			if(!cdb.ReadFloatArray(waveform_breakdown_data_vector, (int *)(&waveform_breakdown_vector_size), 1, "data_vector"))
			{
				AssertErrorCondition(InitialisationError,"ReadWaveformFiles: %s Could not read waveform_breakdown_data_vector", this->Name());
				return False;
			}
			else for(i=0;i<waveform_breakdown_vector_size;i++) AssertErrorCondition(Information,"WaveformGAM::Initialise: %s waveform_breakdown_data_vector[%d] = %f", this->Name(),i, waveform_breakdown_data_vector[i]);
		}
		else waveform_breakdown_available = False;
	cdb->MoveToFather();	

	if(!cdb->Move("waveform_breakdown_negative"))
	{
		AssertErrorCondition(InitialisationError,"WaveformGAM::Initialise: %s Could not move to \"+MARTe.+ISTTOK_RTTh.+waveform_waveform.waveform_breakdown_negative\"",this->Name());
		return False;
	}
		if(!cdb.ReadInt32(waveform_breakdown_negative_vector_size, "vector_size"))
		{
			AssertErrorCondition(InitialisationError,"WaveformGAM::Initialise: %s waveform_breakdown_negative_vector_size",this->Name());
			return False;
		}
		else	AssertErrorCondition(Information,"WaveformGAM::Initialise: %s waveform_breakdown_negative_vector_size = %d", this->Name(),waveform_breakdown_negative_vector_size);

		if (waveform_breakdown_negative_vector_size > 1){
			waveform_breakdown_negative_available = True;
			waveform_breakdown_negative_index_vector =new float[waveform_breakdown_negative_vector_size];
			waveform_breakdown_negative_data_vector =new float[waveform_breakdown_negative_vector_size];

			if(!cdb.ReadFloatArray(waveform_breakdown_negative_index_vector, (int *)(&waveform_breakdown_negative_vector_size), 1, "index_vector"))
			{
				AssertErrorCondition(InitialisationError,"ReadWaveformFiles: %s Could not read waveform_breakdown_negative_index_vector", this->Name());
				return False;
			}
			else for(i=0;i<waveform_breakdown_negative_vector_size;i++) AssertErrorCondition(Information,"WaveformGAM::Initialise: %swaveform_breakdown_negative_index_vector[%d] = %f", this->Name(),i, waveform_breakdown_negative_index_vector[i]);
			if(!cdb.ReadFloatArray(waveform_breakdown_negative_data_vector, (int *)(&waveform_breakdown_negative_vector_size), 1, "data_vector"))
			{
				AssertErrorCondition(InitialisationError,"ReadWaveformFiles: %s Could not read waveform_breakdown_negative_data_vector", this->Name());
				return False;
			}
			else for(i=0;i<waveform_breakdown_negative_vector_size;i++) AssertErrorCondition(Information,"WaveformGAM::Initialise: %s waveform_breakdown_negative_data_vector[%d] = %f", this->Name(),i, waveform_breakdown_negative_data_vector[i]);
		}
		else waveform_breakdown_negative_available = False; 
	cdb->MoveToFather();	
	
	if(!cdb->Move("waveform_inversion_positive_to_negative"))
	{
		AssertErrorCondition(InitialisationError,"WaveformGAM::Initialise: %s Could not move to \"+MARTe.+ISTTOK_RTTh.+waveform_waveform.waveform_inversion_positive_to_negative\"",this->Name());
		return False;
	}
		if(!cdb.ReadInt32(waveform_inversion_positive_to_negative_vector_size, "vector_size"))
		{
			AssertErrorCondition(InitialisationError,"WaveformGAM::Initialise: %s waveform_inversion_positive_to_negative_vector_size",this->Name());
			return False;
		}
		else	AssertErrorCondition(Information,"WaveformGAM::Initialise: %s waveform_inversion_positive_to_negative_vector_size = %d", this->Name(),waveform_inversion_positive_to_negative_vector_size);

		if (waveform_inversion_positive_to_negative_vector_size > 1){
			waveform_inversion_positive_to_negative_available = True;
			waveform_inversion_positive_to_negative_index_vector = new float[waveform_inversion_positive_to_negative_vector_size];
			waveform_inversion_positive_to_negative_data_vector = new float[waveform_inversion_positive_to_negative_vector_size];

			if(!cdb.ReadFloatArray(waveform_inversion_positive_to_negative_index_vector, (int *)(&waveform_inversion_positive_to_negative_vector_size), 1, "index_vector"))
			{
				AssertErrorCondition(InitialisationError,"ReadWaveformFiles: %s Could not read waveform_inversion_positive_to_negative_index_vector", this->Name());
				return False;
			}
			else for(i=0;i<waveform_inversion_positive_to_negative_vector_size;i++) AssertErrorCondition(Information,"WaveformGAM::Initialise: %s waveform_inversion_positive_to_negative_index_vector[%d] = %f", this->Name(),i, waveform_inversion_positive_to_negative_index_vector[i]);
			if(!cdb.ReadFloatArray(waveform_inversion_positive_to_negative_data_vector, (int *)(&waveform_inversion_positive_to_negative_vector_size), 1, "data_vector"))
			{
				AssertErrorCondition(InitialisationError,"ReadWaveformFiles: Could not read waveform_inversion_positive_to_negative_data_vector");
				return False;
			}
			else for(i=0;i<waveform_inversion_positive_to_negative_vector_size;i++) AssertErrorCondition(Information,"WaveformGAM::Initialise: %s waveform_inversion_positive_to_negative_data_vector[%d] = %f", this->Name(),i, waveform_inversion_positive_to_negative_data_vector[i]);
		}
		else waveform_inversion_positive_to_negative_available = False;
	cdb->MoveToFather();


	if(!cdb->Move("waveform_inversion_negative_to_positive"))
	{
		AssertErrorCondition(InitialisationError,"WaveformGAM::Initialise: %s Could not move to \"+MARTe.+ISTTOK_RTTh.+waveform_waveform.waveform_inversion_negative_to_positive\"",this->Name());
		return False;
	}
		if(!cdb.ReadInt32(waveform_inversion_negative_to_positive_vector_size, "vector_size"))
		{
			AssertErrorCondition(InitialisationError,"WaveformGAM::Initialise: %s waveform_inversion_negative_to_positive_vector_size",this->Name());
			return False;
		}
		else	AssertErrorCondition(Information,"WaveformGAM::Initialise: %s waveform_inversion_negative_to_positive_vector_size = %d", this->Name(),waveform_inversion_negative_to_positive_vector_size);

		if (waveform_inversion_negative_to_positive_vector_size > 1){
			waveform_inversion_negative_to_positive_available = True;
			waveform_inversion_negative_to_positive_index_vector =new float[waveform_inversion_negative_to_positive_vector_size];
			waveform_inversion_negative_to_positive_data_vector =new float[waveform_inversion_negative_to_positive_vector_size];

			if(!cdb.ReadFloatArray(waveform_inversion_negative_to_positive_index_vector, (int *)(&waveform_inversion_negative_to_positive_vector_size), 1, "index_vector"))
			{
				AssertErrorCondition(InitialisationError,"ReadWaveformFiles: %s Could not read waveform_inversion_negative_to_positive_index_vector", this->Name());
				return False;
			}
			else for(i=0;i<waveform_inversion_negative_to_positive_vector_size;i++) AssertErrorCondition(Information,"WaveformGAM::Initialise: %s waveform_inversion_index_negative_to_positive_vector[%d] = %f", this->Name(),i, waveform_inversion_negative_to_positive_index_vector[i]);
			if(!cdb.ReadFloatArray(waveform_inversion_negative_to_positive_data_vector, (int *)(&waveform_inversion_negative_to_positive_vector_size), 1, "data_vector"))
			{
				AssertErrorCondition(InitialisationError,"ReadWaveformFiles: Could not read waveform_inversion_negative_to_positive_data_vector");
				return False;
			}
			else for(i=0;i<waveform_inversion_negative_to_positive_vector_size;i++) AssertErrorCondition(Information,"WaveformGAM::Initialise: %s waveform_inversion_negative_to_positive_data_vector[%d] = %f", this->Name(),i, waveform_inversion_negative_to_positive_data_vector[i]);
		}
		else waveform_inversion_negative_to_positive_available = False;
	cdb->MoveToFather();

//	sleep(1);
// Create the signal interfaces
	if(!AddInputInterface(this->SignalsInputInterface, "WaveformGAMInputInterface"))
	{
		AssertErrorCondition(InitialisationError, "WaveformGAM::Initialise: %s failed to add the WaveformGAMInputInterface", this->Name());
		return False;
	}
	if(!AddOutputInterface(this->SignalsOutputInterface, "WaveformGAMOutputInterface"))
	{
		AssertErrorCondition(InitialisationError, "WaveformGAM::Initialise: %s failed to add the WaveformGAMOutputInterface", this->Name());
		return False;
	}

//	INPUT SIGNALS (interface)
	if(!cdb->Move("input_signals"))
	{
		AssertErrorCondition(InitialisationError,"WaveformGAM::Initialise: %s Could not move to \"input_signals\"",this->Name());
		return False;
	}
		int number_of_signals_to_read = 4;
		FString *CDB_move_to;
		FString *SignalType;
		CDB_move_to = new FString[number_of_signals_to_read];
		SignalType = new FString[number_of_signals_to_read];
		CDB_move_to[0].Printf("input_time");
		CDB_move_to[1].Printf("input_mode");
		CDB_move_to[2].Printf("input_AC_cycle");
		CDB_move_to[3].Printf("discharge_status");
		for (i=0;i<number_of_signals_to_read;i++){
			
			if(!cdb->Move(CDB_move_to[i].Buffer()))
			{
				AssertErrorCondition(InitialisationError,"WaveformGAM::Initialise: %s Could not move to \"%s\"",this->Name(),CDB_move_to[i].Buffer());
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
				AssertErrorCondition(Information,"WaveformGAM::Initialise:%s Added signal = %s", this->Name(), SignalName.Buffer());
				
				if(!this->SignalsInputInterface->AddSignal(SignalName.Buffer(), SignalType[i].Buffer()))
				{
					AssertErrorCondition(InitialisationError,"WaveformGAM::Initialise: %s failed to add signal", this->Name());
					return False;
				}
			}
			
			
			cdb->MoveToFather();
		}

	cdb->MoveToFather();


//	OUTPUT SIGNALS (interface)
	if(!cdb->Move("output_signals"))
	{
		AssertErrorCondition(InitialisationError,"WaveformGAM::Initialise: %s Could not move to \"output_signals\"",this->Name());
		return False;
	}
	
		number_of_signals_to_read = 1;
		CDB_move_to = new FString[number_of_signals_to_read];
		SignalType = new FString[number_of_signals_to_read];
		CDB_move_to[0].Printf("output_waveform");
		for (i=0;i<number_of_signals_to_read;i++){
			
			if(!cdb->Move(CDB_move_to[i].Buffer()))
			{
				AssertErrorCondition(InitialisationError,"WaveformGAM::Initialise: %s Could not move to \"%s\"",this->Name(),CDB_move_to[i].Buffer());
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
				AssertErrorCondition(Information,"WaveformGAM::Initialise: Added signal = %s", SignalName.Buffer());
				
				if(!this->SignalsOutputInterface->AddSignal(SignalName.Buffer(), SignalType[i].Buffer()))
				{
					AssertErrorCondition(InitialisationError,"WaveformGAM::Initialise: %s failed to add signal", this->Name());
					return False;
				}
			}
			cdb->MoveToFather();
		}

	cdb->MoveToFather();
	
	
	//	keep waveforms within the limits and transform time in ms to time in us
	if (waveform_1_p_available) {
		for (i = 0; i < waveform_1_p_vector_size; i++){
			if (waveform_1_p_data_vector[i] > waveform_1_p_max_value) waveform_1_p_data_vector[i] = waveform_1_p_max_value;
			if (waveform_1_p_data_vector[i] < waveform_1_p_min_value) waveform_1_p_data_vector[i] = waveform_1_p_min_value;
			waveform_1_p_index_vector[i] = waveform_1_p_index_vector[i] * 1000;
		}
	}
	if (waveform_2_p_available) {
		for (i = 0; i < waveform_2_p_vector_size; i++){
			if (waveform_2_p_data_vector[i] > waveform_1_p_max_value) waveform_2_p_data_vector[i] = waveform_2_p_max_value;
			if (waveform_2_p_data_vector[i] < waveform_1_p_min_value) waveform_2_p_data_vector[i] = waveform_2_p_min_value;
			waveform_2_p_index_vector[i] = waveform_2_p_index_vector[i] * 1000;
		}
	}
	if (waveform_1_n_available) {
		for (i = 0; i < waveform_1_n_vector_size; i++){
			if (waveform_1_n_data_vector[i] > waveform_1_p_max_value) waveform_1_n_data_vector[i] = waveform_1_p_max_value;
			if (waveform_1_n_data_vector[i] < waveform_1_p_min_value) waveform_1_n_data_vector[i] = waveform_1_p_min_value;
			waveform_1_n_index_vector[i] = waveform_1_n_index_vector[i] * 1000;
		}
	}
	if (waveform_2_n_available) {
		for (i = 0; i < waveform_2_n_vector_size; i++){
			if (waveform_2_n_data_vector[i] > waveform_2_p_max_value) waveform_2_n_data_vector[i] = waveform_2_p_max_value;
			if (waveform_2_n_data_vector[i] < waveform_2_p_min_value) waveform_2_n_data_vector[i] = waveform_2_p_min_value;
			waveform_2_n_index_vector[i] = waveform_2_n_index_vector[i] * 1000;
		}
	}
	if (waveform_breakdown_available) {
		for (i = 0; i < waveform_breakdown_vector_size; i++){
			waveform_breakdown_index_vector[i] = waveform_breakdown_index_vector[i] * 1000;
		}
	}
	if (waveform_breakdown_negative_available) {
		for (i = 0; i < waveform_breakdown_negative_vector_size; i++){
			waveform_breakdown_negative_index_vector[i] = waveform_breakdown_negative_index_vector[i] * 1000;
		}
	}
	if (waveform_inversion_positive_to_negative_available) {
		for (i = 0; i < waveform_inversion_positive_to_negative_vector_size; i++){
			waveform_inversion_positive_to_negative_index_vector[i] = waveform_inversion_positive_to_negative_index_vector[i] * 1000;
		}
	}
	if (waveform_inversion_negative_to_positive_available) {
		for (i = 0; i < waveform_inversion_negative_to_positive_vector_size; i++){
			waveform_inversion_negative_to_positive_index_vector[i] = waveform_inversion_negative_to_positive_index_vector[i] * 1000;
		}
	}
	
	
	//	waveforms
	if (waveform_1_p_available) this->waveform_1_p = new IWaveform(&waveform_1_p_index_vector[0], &waveform_1_p_data_vector[0], waveform_1_p_vector_size);
	if (waveform_1_p_available) this->waveform_1_p->DefineOutsideValueDefaultValue(0);
	if (waveform_2_p_available) this->waveform_2_p = new IWaveform(&waveform_2_p_index_vector[0], &waveform_2_p_data_vector[0], waveform_2_p_vector_size );
	if (waveform_2_p_available) this->waveform_2_p->DefineOutsideValueDefaultValue(0);
	if (waveform_breakdown_available) this->waveform_breakdown = new IWaveform(&waveform_breakdown_index_vector[0], &waveform_breakdown_data_vector[0], waveform_breakdown_vector_size );
	if (waveform_breakdown_available) this->waveform_breakdown->DefineOutsideValueDefaultValue(0);
	if (waveform_breakdown_negative_available) this->waveform_breakdown_negative = new IWaveform(&waveform_breakdown_negative_index_vector[0], &waveform_breakdown_negative_data_vector[0], waveform_breakdown_negative_vector_size );
	if (waveform_breakdown_negative_available) this->waveform_breakdown_negative->DefineOutsideValueDefaultValue(0);
	if (waveform_1_n_available) this->waveform_1_n = new IWaveform(&waveform_1_n_index_vector[0], &waveform_1_n_data_vector[0], waveform_1_n_vector_size );
	if (waveform_1_n_available)this->waveform_1_n->DefineOutsideValueDefaultValue(0);
	if (waveform_2_n_available) this->waveform_2_n = new IWaveform(&waveform_2_n_index_vector[0], &waveform_2_n_data_vector[0], waveform_2_n_vector_size );
	if (waveform_2_n_available) this->waveform_2_n->DefineOutsideValueDefaultValue(0);
	if (waveform_inversion_positive_to_negative_available) this->waveform_inversion_positive_to_negative = new IWaveform(&waveform_inversion_positive_to_negative_index_vector[0], &waveform_inversion_positive_to_negative_data_vector[0], waveform_inversion_positive_to_negative_vector_size );
	if (waveform_inversion_positive_to_negative_available) this->waveform_inversion_positive_to_negative->DefineOutsideValueDefaultValue(0);
	if (waveform_inversion_negative_to_positive_available) this->waveform_inversion_negative_to_positive = new IWaveform(&waveform_inversion_negative_to_positive_index_vector[0], &waveform_inversion_negative_to_positive_data_vector[0], waveform_inversion_negative_to_positive_vector_size );
	if (waveform_inversion_negative_to_positive_available) this->waveform_inversion_negative_to_positive->DefineOutsideValueDefaultValue(0);
	
	//delete unnecessary vectors		(if deleted the web page will not have the correct values)
/*	if (waveform_1_p_available) delete waveform_1_p_index_vector;
	if (waveform_1_p_available) delete waveform_1_p_data_vector;
	if (waveform_1_n_available) delete waveform_1_n_index_vector;
	if (waveform_1_n_available) delete waveform_1_n_data_vector;
	if (waveform_2_p_available) delete waveform_2_p_index_vector;
	if (waveform_2_p_available) delete waveform_2_p_data_vector;
	if (waveform_2_n_available) delete waveform_2_n_index_vector;
	if (waveform_2_n_available) delete waveform_2_n_data_vector;
	if (waveform_breakdown_available) delete waveform_breakdown_index_vector;
	if (waveform_breakdown_available) delete waveform_breakdown_data_vector;
	if (waveform_breakdown_negative_available) delete waveform_breakdown_negative_index_vector;
	if (waveform_breakdown_negative_available) delete waveform_breakdown_negative_data_vector;
	if (waveform_inversion_positive_to_negative_available) delete waveform_inversion_positive_to_negative_index_vector;
	if (waveform_inversion_positive_to_negative_available) delete waveform_inversion_positive_to_negative_data_vector;
	if (waveform_inversion_negative_to_positive_available) delete waveform_inversion_negative_to_positive_index_vector;
	if (waveform_inversion_negative_to_positive_available) delete waveform_inversion_negative_to_positive_data_vector;
*/	
	return True;
}
//} ******************************************************************



//{ ********* Execute the module functionalities *******************
bool WaveformGAM::Execute(GAM_FunctionNumbers functionNumber){
 

	InputInterfaceStruct *inputstruct = (InputInterfaceStruct *) this->SignalsInputInterface->Buffer();
	this->SignalsInputInterface->Read();
//    AssertErrorCondition(InitialisationError,"WaveformGAM:: %s inputstruct = %d %d %d %d",this->Name(), inputstruct[0].usecDischargeTime, inputstruct[0].WaveformMode, inputstruct[0].PlasmaDirection, inputstruct[0].DischargeStatus);
	OutputInterfaceStruct *outputstruct = (OutputInterfaceStruct *) this->SignalsOutputInterface->Buffer();

/*
	*** Discharge Status ***
	-3 -> error
	-2 -> offline
	0 -> breakdown
	1 -> normal operation
	2 -> inverting
*/
/*
	*** WaveformMode ***
	1 -> direct
	2 -> scenario (position, plasma current etc..)
*/
/*
	*** Plasma Direction ***
	0-> negative direction.
	1-> positive and normal direction.
*/
	if(functionNumber == GAMOnline){
		
		if (inputstruct[0].WaveformMode == 1 && inputstruct[0].PlasmaDirection == 1 && inputstruct[0].DischargeStatus == 1){
			if (waveform_1_p_available) outputstruct[0].WaveformOutput = this->waveform_1_p->GetWaveformValue(inputstruct[0].usecDischargeTime);
			else {
				 AssertErrorCondition(InitialisationError,"WaveformGAM:: %s ERROR waveform_1_p was requested and is not available",this->Name());
				 outputstruct[0].WaveformOutput = 0;
			}
		}
		else if (inputstruct[0].WaveformMode == 2 && inputstruct[0].PlasmaDirection == 1 && inputstruct[0].DischargeStatus == 1){
			if (waveform_2_p_available) outputstruct[0].WaveformOutput = this->waveform_2_p->GetWaveformValue(inputstruct[0].usecDischargeTime);
			else {
				 AssertErrorCondition(InitialisationError,"WaveformGAM:: %s ERROR waveform_2_p was requested and is not available",this->Name());
				 outputstruct[0].WaveformOutput = 0;
			}
		}
		else if (inputstruct[0].WaveformMode == 1 && inputstruct[0].PlasmaDirection == 1  && inputstruct[0].DischargeStatus == 0){
			if (waveform_breakdown_available) outputstruct[0].WaveformOutput = this->waveform_breakdown->GetWaveformValue(inputstruct[0].usecDischargeTime);
			else {
				 AssertErrorCondition(InitialisationError,"WaveformGAM:: %s ERROR waveform_breakdown was requested and is not available",this->Name());
				 outputstruct[0].WaveformOutput = 0;
			}
		}
		else if (inputstruct[0].WaveformMode == 1 && inputstruct[0].PlasmaDirection == 1  && inputstruct[0].DischargeStatus == 2){
			if (waveform_inversion_positive_to_negative_available) outputstruct[0].WaveformOutput = this->waveform_inversion_positive_to_negative->GetWaveformValue(inputstruct[0].usecDischargeTime);
			else {
				 AssertErrorCondition(InitialisationError,"WaveformGAM:: %s ERROR waveform_inversion_positive_to_negative was requested and is not available",this->Name());
				 outputstruct[0].WaveformOutput = 0;
			}
		}
		else if (inputstruct[0].WaveformMode == 1 && inputstruct[0].PlasmaDirection == 0 && inputstruct[0].DischargeStatus == 1){
			if (waveform_1_n_available) outputstruct[0].WaveformOutput = this->waveform_1_n->GetWaveformValue(inputstruct[0].usecDischargeTime);
			else {
				 AssertErrorCondition(InitialisationError,"WaveformGAM:: %s ERROR waveform_1_n was requested and is not available",this->Name());
				 outputstruct[0].WaveformOutput = 0;
			}
		}
		else if (inputstruct[0].WaveformMode == 2 && inputstruct[0].PlasmaDirection == 0 && inputstruct[0].DischargeStatus == 1){
			if (waveform_2_n_available) outputstruct[0].WaveformOutput = this->waveform_2_n->GetWaveformValue(inputstruct[0].usecDischargeTime);
			else {
				 AssertErrorCondition(InitialisationError,"WaveformGAM:: %s ERROR waveform_2_n was requested and is not available",this->Name());
				 outputstruct[0].WaveformOutput = 0;
			}
		}
		else if (inputstruct[0].WaveformMode == 1 && inputstruct[0].PlasmaDirection == 0 && inputstruct[0].DischargeStatus == 0){
			if (waveform_breakdown_negative_available) outputstruct[0].WaveformOutput = this->waveform_breakdown_negative->GetWaveformValue(inputstruct[0].usecDischargeTime);
			else {
				 AssertErrorCondition(InitialisationError,"WaveformGAM:: %s ERROR waveform_breakdown_negative was requested and is not available",this->Name());
				 outputstruct[0].WaveformOutput = 0;
			}
		}
		else if (inputstruct[0].WaveformMode == 1 && inputstruct[0].PlasmaDirection == 0 && inputstruct[0].DischargeStatus == 2){
			if (waveform_inversion_negative_to_positive_available) outputstruct[0].WaveformOutput = this->waveform_inversion_negative_to_positive->GetWaveformValue(inputstruct[0].usecDischargeTime);
			else {
				 AssertErrorCondition(InitialisationError,"WaveformGAM:: %s ERROR waveform_inversion_negative_to_positive was requested and is not available",this->Name());
				 outputstruct[0].WaveformOutput = 0;
			}
		}
		else	outputstruct[0].WaveformOutput = 0;
	}
	else	outputstruct[0].WaveformOutput = 0;


//	outputstruct[0].WaveformOutput = inputstruct[0].usecDischargeTime;
//	AssertErrorCondition(InitialisationError,"WaveformGAM:: %s inputstruct = %f",this->Name(), outputstruct[0].WaveformOutput );
	this->SignalsOutputInterface->Write();

	return True;		
}
bool WaveformGAM::ProcessHttpMessage(HttpStream &hStream){
	
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
		hmStream.SSPrintf(HtmlTagStreamMode, "br><br>waveform_1_p_vector_size = %d\n\
		<br>waveform_1_p_max_value = %.2f\n\
		<br>waveform_1_p_min_value = %.2f\n\
		<br>waveform_1_n_vector_size = %d\n\
		<br>waveform_2_p_vector_size = %d\n\
		<br>waveform_2_p_max_value = %.2f\n\
		<br>waveform_2_p_min_value = %.2f\n\
		<br>waveform_2_n_vector_size = %d\n\
		<br>waveform_breakdown_vector_size = %d\n\
		<br>waveform_breakdown_negative_vector_size = %d\n\
		<br>waveform_inversion_positive_to_negative_vector_size = %d\n\
		<br>waveform_inversion_negative_to_positive_vector_size = %d\n\
		<br><br",waveform_1_p_vector_size,waveform_1_p_max_value,waveform_1_p_min_value,waveform_1_n_vector_size,waveform_2_p_vector_size,waveform_2_p_max_value,waveform_2_p_min_value,waveform_2_n_vector_size,waveform_breakdown_vector_size,waveform_breakdown_negative_vector_size,waveform_inversion_positive_to_negative_vector_size,waveform_inversion_negative_to_positive_vector_size);

		if (waveform_1_p_available){
			
			hmStream.SSPrintf(HtmlTagStreamMode, "table border=\"1\"><tr><td>waveform_1_p_index_vector</td");
			for (i=0;i<waveform_1_p_vector_size;i++)hmStream.SSPrintf(HtmlTagStreamMode, "td>%.2f</td",waveform_1_p_index_vector[i]);
			hmStream.SSPrintf(HtmlTagStreamMode, "/tr><tr><td>waveform_1_p_data_vector</td");
			for (i=0;i<waveform_1_p_vector_size;i++)hmStream.SSPrintf(HtmlTagStreamMode, "td>%.2f</td",waveform_1_p_data_vector[i]);
			hmStream.SSPrintf(HtmlTagStreamMode, "/tr></table><br");
		}
		else hmStream.SSPrintf(HtmlTagStreamMode, "/br>waveform_1_p not available<br");
		
		if (waveform_1_n_available){
			hmStream.SSPrintf(HtmlTagStreamMode, "table border=\"1\"><tr><td>waveform_1_n_index_vector</td");
			for (i=0;i<waveform_1_n_vector_size;i++)hmStream.SSPrintf(HtmlTagStreamMode, "td>%.2f</td",waveform_1_n_index_vector[i]);
			hmStream.SSPrintf(HtmlTagStreamMode, "/tr><tr><td>waveform_1_n_data_vector</td");
			for (i=0;i<waveform_1_n_vector_size;i++)hmStream.SSPrintf(HtmlTagStreamMode, "td>%.2f</td",waveform_1_n_data_vector[i]);
			hmStream.SSPrintf(HtmlTagStreamMode, "/tr></table><br");
		}
		else hmStream.SSPrintf(HtmlTagStreamMode, "/br>waveform_1_n not available<br");
			
		if (waveform_2_p_available){
			hmStream.SSPrintf(HtmlTagStreamMode, "table border=\"1\"><tr><td>waveform_2_p_index_vector</td");
			for (i=0;i<waveform_2_p_vector_size;i++)hmStream.SSPrintf(HtmlTagStreamMode, "td>%.2f</td",waveform_2_p_index_vector[i]);
			hmStream.SSPrintf(HtmlTagStreamMode, "/tr><tr><td>waveform_2_p_data_vector</td");
			for (i=0;i<waveform_2_p_vector_size;i++)hmStream.SSPrintf(HtmlTagStreamMode, "td>%.2f</td",waveform_2_p_data_vector[i]);
			hmStream.SSPrintf(HtmlTagStreamMode, "/tr></table><br");
		}
		else hmStream.SSPrintf(HtmlTagStreamMode, "/br>waveform_2_p not available<br");
 
		if (waveform_2_n_available){
			hmStream.SSPrintf(HtmlTagStreamMode, "table border=\"1\"><tr><td>waveform_2_n_index_vector</td");
			for (i=0;i<waveform_2_n_vector_size;i++)hmStream.SSPrintf(HtmlTagStreamMode, "td>%.2f</td",waveform_2_n_index_vector[i]);
			hmStream.SSPrintf(HtmlTagStreamMode, "/tr><tr><td>waveform_2_n_data_vector</td");
			for (i=0;i<waveform_2_n_vector_size;i++)hmStream.SSPrintf(HtmlTagStreamMode, "td>%.2f</td",waveform_2_n_data_vector[i]);
			hmStream.SSPrintf(HtmlTagStreamMode, "/tr></table><br");
		}
		else hmStream.SSPrintf(HtmlTagStreamMode, "/br>waveform_2_n not available<br");
		
		if (waveform_breakdown_available){
			hmStream.SSPrintf(HtmlTagStreamMode, "table border=\"1\"><tr><td>waveform_breakdown_index_vector</td");
			for (i=0;i<waveform_breakdown_vector_size;i++)hmStream.SSPrintf(HtmlTagStreamMode, "td>%.2f</td",waveform_breakdown_index_vector[i]);
			hmStream.SSPrintf(HtmlTagStreamMode, "/tr><tr><td>waveform_breakdown_data_vector</td");
			for (i=0;i<waveform_breakdown_vector_size;i++)hmStream.SSPrintf(HtmlTagStreamMode, "td>%.2f</td",waveform_breakdown_data_vector[i]);
			hmStream.SSPrintf(HtmlTagStreamMode, "/tr></table><br");
		}
		else hmStream.SSPrintf(HtmlTagStreamMode, "/br>waveform_breakdown not available<br");

		if (waveform_breakdown_negative_available){
			hmStream.SSPrintf(HtmlTagStreamMode, "table border=\"1\"><tr><td>waveform_breakdown_negative_index_vector</td");
			for (i=0;i<waveform_breakdown_negative_vector_size;i++)hmStream.SSPrintf(HtmlTagStreamMode, "td>%.2f</td",waveform_breakdown_negative_index_vector[i]);
			hmStream.SSPrintf(HtmlTagStreamMode, "/tr><tr><td>waveform_breakdown_negative_data_vector</td");
			for (i=0;i<waveform_breakdown_negative_vector_size;i++)hmStream.SSPrintf(HtmlTagStreamMode, "td>%.2f</td",waveform_breakdown_negative_data_vector[i]);
			hmStream.SSPrintf(HtmlTagStreamMode, "/tr></table><br");
		}
		else hmStream.SSPrintf(HtmlTagStreamMode, "/br>waveform_breakdown_negative not available<br");

		if (waveform_inversion_positive_to_negative_available){
			hmStream.SSPrintf(HtmlTagStreamMode, "table border=\"1\"><tr><td>waveform_inversion_positive_to_negative_index_vector</td");
			for (i=0;i<waveform_inversion_positive_to_negative_vector_size;i++)hmStream.SSPrintf(HtmlTagStreamMode, "td>%.2f</td",waveform_inversion_positive_to_negative_index_vector[i]);
			hmStream.SSPrintf(HtmlTagStreamMode, "/tr><tr><td>waveform_inversion_positive_to_negative_data_vector</td");
			for (i=0;i<waveform_inversion_positive_to_negative_vector_size;i++)hmStream.SSPrintf(HtmlTagStreamMode, "td>%.2f</td",waveform_inversion_positive_to_negative_data_vector[i]);
			hmStream.SSPrintf(HtmlTagStreamMode, "/tr></table><br");
		}
		else hmStream.SSPrintf(HtmlTagStreamMode, "/br>waveform_inveersion_positive_to_negative not available<br");

		if (waveform_inversion_negative_to_positive_available){
			hmStream.SSPrintf(HtmlTagStreamMode, "table border=\"1\"><tr><td>waveform_inversion_negative_to_positive_index_vector</td");
			for (i=0;i<waveform_inversion_negative_to_positive_vector_size;i++)hmStream.SSPrintf(HtmlTagStreamMode, "td>%.2f</td",waveform_inversion_negative_to_positive_index_vector[i]);
			hmStream.SSPrintf(HtmlTagStreamMode, "/tr><tr><td>waveform_inversion_negative_to_positive_data_vector</td");
			for (i=0;i<waveform_inversion_negative_to_positive_vector_size;i++)hmStream.SSPrintf(HtmlTagStreamMode, "td>%.2f</td",waveform_inversion_negative_to_positive_data_vector[i]);
			hmStream.SSPrintf(HtmlTagStreamMode, "/tr></table><br");
		}
		else hmStream.SSPrintf(HtmlTagStreamMode, "/br>waveform_inveersion_negative_to_positive not available<br"); 
	}
	hmStream.SSPrintf(HtmlTagStreamMode, "/form");



	hmStream.SSPrintf(HtmlTagStreamMode, "/body>\n</html");
	hStream.SSPrintf("OutputHttpOtions.Content-Type","text/html;charset=utf-8");
	hStream.WriteReplyHeader(True);


	return True;		
}
