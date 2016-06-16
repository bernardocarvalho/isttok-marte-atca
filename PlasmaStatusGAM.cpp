
#include "PlasmaStatusGAM.h"

OBJECTLOADREGISTER(PlasmaStatusGAM, "$Id: $")


//  ******** Default constructor ***********************************
PlasmaStatusGAM::PlasmaStatusGAM(){
	this->SignalsInputInterface = NULL;
	this->SignalsOutputInterface = NULL;
}


// ********* Destructor ********************************************
PlasmaStatusGAM::~PlasmaStatusGAM()
{

}


//{ ********* Initialise the module ********************************
bool PlasmaStatusGAM::Initialise(ConfigurationDataBase& cdbData){
   
	CDBExtended cdb(cdbData);

	int i;

	if(!cdb.ReadFloat(high_current_threshold_value, "high_current_threshold_value"))
	{
		AssertErrorCondition(InitialisationError,"PlasmaStatusGAM::Initialise: %s high_current_threshold_value",this->Name());
		return False;
	}
	else AssertErrorCondition(Information,"PlasmaStatusGAM::Initialise: high_current_threshold_value = %f",high_current_threshold_value);

	if(!cdb->Move("tomography"))
	{
		AssertErrorCondition(InitialisationError,"PlasmaStatusGAM::Initialise: %s Could not move to \"+MARTe.+ISTTOK_RTTh.+plasma_parameters.tomography\"",this->Name());
		return False;
	}
		if(!cdb.ReadInt32(i, "tomography_radial_bool"))
		{
			AssertErrorCondition(InitialisationError,"PlasmaStatusGAM::Initialise: %s tomography_radial_bool",this->Name());
			return False;
		}
		else 
		{
			tomography_radial_bool = (bool)i;
			AssertErrorCondition(Information,"PlasmaStatusGAM::Initialise: tomography_radial_bool = %d",tomography_radial_bool);
		}	
		if(!cdb.ReadInt32(i, "tomography_vertical_bool"))
		{
			AssertErrorCondition(InitialisationError,"PlasmaStatusGAM::Initialise: %s tomography_vertical_bool",this->Name());
			return False;
		}
		else 
		{
			tomography_vertical_bool = (bool)i;
			AssertErrorCondition(Information,"PlasmaStatusGAM::Initialise: tomography_vertical_bool = %d",tomography_vertical_bool);
		}	

		if(!cdb.ReadFloat(tomography_radial_high_current_weight, "radial_high_current_weight"))
		{
			AssertErrorCondition(InitialisationError,"PlasmaStatusGAM::Initialise: %s tomography_radial_high_current_weight",this->Name());
			return False;
		}
		else AssertErrorCondition(Information,"PlasmaStatusGAM::Initialise: tomography_radial_high_current_weight = %f",tomography_radial_high_current_weight);
		if(!cdb.ReadFloat(tomography_radial_low_current_weight, "radial_low_current_weight"))
		{
			AssertErrorCondition(InitialisationError,"PlasmaStatusGAM::Initialise: %s tomography_radial_low_current_weight",this->Name());
			return False;
		}
		else AssertErrorCondition(Information,"PlasmaStatusGAM::Initialise: tomography_radial_low_current_weight = %f",tomography_radial_low_current_weight);
		if(!cdb.ReadFloat(tomography_vertical_high_current_weight, "vertical_high_current_weight"))
		{
			AssertErrorCondition(InitialisationError,"PlasmaStatusGAM::Initialise: %s tomography_vertical_high_current_weight",this->Name());
			return False;
		}
		else AssertErrorCondition(Information,"PlasmaStatusGAM::Initialise: tomography_vertical_high_current_weight = %f",tomography_vertical_high_current_weight);
		if(!cdb.ReadFloat(tomography_vertical_low_current_weight, "vertical_low_current_weight"))
		{
			AssertErrorCondition(InitialisationError,"PlasmaStatusGAM::Initialise: %s tomography_vertical_low_current_weight",this->Name());
			return False;
		}
		else AssertErrorCondition(Information,"PlasmaStatusGAM::Initialise: tomography_vertical_low_current_weight = %f",tomography_vertical_low_current_weight);

	cdb->MoveToFather();

	if(!cdb->Move("electric_probes"))
	{
		AssertErrorCondition(InitialisationError,"PlasmaStatusGAM::Initialise: %s Could not move to \"+MARTe.+ISTTOK_RTTh.+plasma_parameters.electric_probes\"",this->Name());
		return False;
	}
		if(!cdb.ReadInt32(i, "electric_radial_bool"))
		{
			AssertErrorCondition(InitialisationError,"PlasmaStatusGAM::Initialise: %s electric_radial_bool",this->Name());
			return False;
		}
		else 
		{
			electric_radial_bool = (bool)i;
			AssertErrorCondition(Information,"PlasmaStatusGAM::Initialise: electric_radial_bool = %d",electric_radial_bool);
		}	
		if(!cdb.ReadInt32(i, "electric_vertical_bool"))
		{
			AssertErrorCondition(InitialisationError,"PlasmaStatusGAM::Initialise: %s electric_vertical_bool",this->Name());
			return False;
		}
		else 
		{
			electric_vertical_bool = (bool)i;
			AssertErrorCondition(Information,"PlasmaStatusGAM::Initialise: electric_vertical_bool = %d",electric_vertical_bool);
		}	
		if(!cdb.ReadFloat(electric_radial_high_current_weight, "radial_high_current_weight"))
		{
			AssertErrorCondition(InitialisationError,"PlasmaStatusGAM::Initialise: %s electric_radial_high_current_weight",this->Name());
			return False;
		}
		else AssertErrorCondition(Information,"PlasmaStatusGAM::Initialise: electric_radial_high_current_weight = %f",electric_radial_high_current_weight);
		if(!cdb.ReadFloat(electric_radial_low_current_weight, "radial_low_current_weight"))
		{
			AssertErrorCondition(InitialisationError,"PlasmaStatusGAM::Initialise: %s electric_radial_low_current_weight",this->Name());
			return False;
		}
		else AssertErrorCondition(Information,"PlasmaStatusGAM::Initialise: electric_radial_low_current_weight = %f",electric_radial_low_current_weight);
		if(!cdb.ReadFloat(electric_vertical_high_current_weight, "vertical_high_current_weight"))
		{
			AssertErrorCondition(InitialisationError,"PlasmaStatusGAM::Initialise: %s electric_vertical_high_current_weight",this->Name());
			return False;
		}
		else AssertErrorCondition(Information,"PlasmaStatusGAM::Initialise: electric_vertical_high_current_weight = %f",electric_vertical_high_current_weight);
		if(!cdb.ReadFloat(electric_vertical_low_current_weight, "vertical_low_current_weight"))
		{
			AssertErrorCondition(InitialisationError,"PlasmaStatusGAM::Initialise: %s electric_vertical_low_current_weight",this->Name());
			return False;
		}
		else AssertErrorCondition(Information,"PlasmaStatusGAM::Initialise: electric_vertical_low_current_weight = %f",electric_vertical_low_current_weight);

	cdb->MoveToFather();

	if(!cdb->Move("magnetic_probes"))
	{
		AssertErrorCondition(InitialisationError,"PlasmaStatusGAM::Initialise: %s Could not move to \"+MARTe.+ISTTOK_RTTh.+plasma_parameters.magnetic_probes\"",this->Name());
		return False;
	}
		if(!cdb.ReadInt32(i, "magnetic_radial_bool"))
		{
			AssertErrorCondition(InitialisationError,"PlasmaStatusGAM::Initialise: %s magnetic_radial_bool",this->Name());
			return False;
		}
		else 
		{
			magnetic_radial_bool = (bool)i;
			AssertErrorCondition(Information,"PlasmaStatusGAM::Initialise: magnetic_radial_bool = %d",magnetic_radial_bool);
		}	
		if(!cdb.ReadInt32(i, "magnetic_vertical_bool"))
		{
			AssertErrorCondition(InitialisationError,"PlasmaStatusGAM::Initialise: %s magnetic_vertical_bool",this->Name());
			return False;
		}
		else 
		{
			magnetic_vertical_bool = (bool)i;
			AssertErrorCondition(Information,"PlasmaStatusGAM::Initialise: magnetic_vertical_bool = %d",magnetic_vertical_bool);
		}	
		if(!cdb.ReadFloat(magnetic_radial_high_current_weight, "radial_high_current_weight"))
		{
			AssertErrorCondition(InitialisationError,"PlasmaStatusGAM::Initialise: %s magnetic_radial_high_current_weight",this->Name());
			return False;
		}
		else AssertErrorCondition(Information,"PlasmaStatusGAM::Initialise: magnetic_radial_high_current_weight = %f",magnetic_radial_high_current_weight);
		if(!cdb.ReadFloat(magnetic_radial_low_current_weight, "radial_low_current_weight"))
		{
			AssertErrorCondition(InitialisationError,"PlasmaStatusGAM::Initialise: %s magnetic_radial_low_current_weight",this->Name());
			return False;
		}
		else AssertErrorCondition(Information,"PlasmaStatusGAM::Initialise: magnetic_radial_low_current_weight = %f",magnetic_radial_low_current_weight);
		if(!cdb.ReadFloat(magnetic_vertical_high_current_weight, "vertical_high_current_weight"))
		{
			AssertErrorCondition(InitialisationError,"PlasmaStatusGAM::Initialise: %s magnetic_vertical_high_current_weight",this->Name());
			return False;
		}
		else AssertErrorCondition(Information,"PlasmaStatusGAM::Initialise: magnetic_vertical_high_current_weight = %f",magnetic_vertical_high_current_weight);
		if(!cdb.ReadFloat(magnetic_vertical_low_current_weight, "vertical_low_current_weight"))
		{
			AssertErrorCondition(InitialisationError,"PlasmaStatusGAM::Initialise: %s magnetic_vertical_low_current_weight",this->Name());
			return False;
		}
		else AssertErrorCondition(Information,"PlasmaStatusGAM::Initialise: magnetic_vertical_low_current_weight = %f",magnetic_vertical_low_current_weight);

	cdb->MoveToFather();

	if(!cdb->Move("sine_probe"))
	{
		AssertErrorCondition(InitialisationError,"PlasmaStatusGAM::Initialise: %s Could not move to \"+MARTe.+ISTTOK_RTTh.+plasma_parameters.sine_probe\"",this->Name());
		return False;
	}
		if(!cdb.ReadInt32(i, "sine_vertical_bool"))
		{
			AssertErrorCondition(InitialisationError,"PlasmaStatusGAM::Initialise: %s sine_vertical_bool",this->Name());
			return False;
		}
		else 
		{
			sine_vertical_bool = (bool)i;
			AssertErrorCondition(Information,"PlasmaStatusGAM::Initialise: sine_vertical_bool = %d",sine_vertical_bool);
		}	
		if(!cdb.ReadFloat(sine_vertical_high_current_weight, "vertical_high_current_weight"))
		{
			AssertErrorCondition(InitialisationError,"PlasmaStatusGAM::Initialise: %s sine_vertical_high_current_weight",this->Name());
			return False;
		}
		else AssertErrorCondition(Information,"PlasmaStatusGAM::Initialise: sine_vertical_high_current_weight = %f",sine_vertical_high_current_weight);
		if(!cdb.ReadFloat(sine_vertical_low_current_weight, "vertical_low_current_weight"))
		{
			AssertErrorCondition(InitialisationError,"PlasmaStatusGAM::Initialise: %s sine_vertical_low_current_weight",this->Name());
			return False;
		}
		else AssertErrorCondition(Information,"PlasmaStatusGAM::Initialise: sine_vertical_low_current_weight = %f",sine_vertical_low_current_weight);

	cdb->MoveToFather();

	if(!cdb->Move("cosine_probe"))
	{
		AssertErrorCondition(InitialisationError,"PlasmaStatusGAM::Initialise: %s Could not move to \"+MARTe.+ISTTOK_RTTh.+plasma_parameters.cosine_probe\"",this->Name());
		return False;
	}
		if(!cdb.ReadInt32(i, "cosine_radial_bool"))
		{
			AssertErrorCondition(InitialisationError,"PlasmaStatusGAM::Initialise: %s cosine_radial_bool",this->Name());
			return False;
		}
		else 
		{
			cosine_radial_bool = (bool)i;
			AssertErrorCondition(Information,"PlasmaStatusGAM::Initialise: cosine_radial_bool = %d",cosine_radial_bool);
		}	
		if(!cdb.ReadFloat(cosine_radial_high_current_weight, "radial_high_current_weight"))
		{
			AssertErrorCondition(InitialisationError,"PlasmaStatusGAM::Initialise: %s cosine_radial_high_current_weight",this->Name());
			return False;
		}
		else AssertErrorCondition(Information,"PlasmaStatusGAM::Initialise: cosine_radial_high_current_weight = %f",cosine_radial_high_current_weight);
		if(!cdb.ReadFloat(cosine_radial_low_current_weight, "radial_low_current_weight"))
		{
			AssertErrorCondition(InitialisationError,"PlasmaStatusGAM::Initialise: %s cosine_radial_low_current_weight",this->Name());
			return False;
		}
		else AssertErrorCondition(Information,"PlasmaStatusGAM::Initialise: cosine_radial_low_current_weight = %f",cosine_radial_low_current_weight);

	cdb->MoveToFather();

	if(!cdb->Move("hibd"))
	{
		AssertErrorCondition(InitialisationError,"PlasmaStatusGAM::Initialise: %s Could not move to \"+MARTe.+ISTTOK_RTTh.+plasma_parameters.hibd\"",this->Name());
		return False;
	}
		if(!cdb.ReadInt32(i, "hibd_radial_bool"))
		{
			AssertErrorCondition(InitialisationError,"PlasmaStatusGAM::Initialise: %s hibd_radial_bool",this->Name());
			return False;
		}
		else 
		{
			hibd_radial_bool = (bool)i;
			AssertErrorCondition(Information,"PlasmaStatusGAM::Initialise: hibd_radial_bool = %d",hibd_radial_bool);
		}	
		if(!cdb.ReadInt32(i, "hibd_vertical_bool"))
		{
			AssertErrorCondition(InitialisationError,"PlasmaStatusGAM::Initialise: %s hibd_vertical_bool",this->Name());
			return False;
		}
		else 
		{
			hibd_vertical_bool = (bool)i;
			AssertErrorCondition(Information,"PlasmaStatusGAM::Initialise: hibd_vertical_bool = %d",hibd_vertical_bool);
		}	
		if(!cdb.ReadFloat(hibd_radial_high_current_weight, "radial_high_current_weight"))
		{
			AssertErrorCondition(InitialisationError,"PlasmaStatusGAM::Initialise: %s hibd_radial_high_current_weight",this->Name());
			return False;
		}
		else AssertErrorCondition(Information,"PlasmaStatusGAM::Initialise: hibd_radial_high_current_weight = %f",hibd_radial_high_current_weight);
		if(!cdb.ReadFloat(hibd_radial_low_current_weight, "radial_low_current_weight"))
		{
			AssertErrorCondition(InitialisationError,"PlasmaStatusGAM::Initialise: %s hibd_radial_low_current_weight",this->Name());
			return False;
		}
		else AssertErrorCondition(Information,"PlasmaStatusGAM::Initialise: hibd_radial_low_current_weight = %f",hibd_radial_low_current_weight);
		if(!cdb.ReadFloat(hibd_vertical_high_current_weight, "vertical_high_current_weight"))
		{
			AssertErrorCondition(InitialisationError,"PlasmaStatusGAM::Initialise: %s hibd_vertical_high_current_weight",this->Name());
			return False;
		}
		else AssertErrorCondition(Information,"PlasmaStatusGAM::Initialise: hibd_vertical_high_current_weight = %f",hibd_vertical_high_current_weight);
		if(!cdb.ReadFloat(hibd_vertical_low_current_weight, "vertical_low_current_weight"))
		{
			AssertErrorCondition(InitialisationError,"PlasmaStatusGAM::Initialise: %s hibd_vertical_low_current_weight",this->Name());
			return False;
		}
		else AssertErrorCondition(Information,"PlasmaStatusGAM::Initialise: hibd_vertical_low_current_weight = %f",hibd_vertical_low_current_weight);

	cdb->MoveToFather();

//	sleep(3);
// Create the signal interfaces
	if(!AddInputInterface(this->SignalsInputInterface, "PlasmaStatusGAMInputInterface"))
	{
		AssertErrorCondition(InitialisationError, "PlasmaStatusGAM::Initialise: %s failed to add the TimewindowsGAMInputInterface", this->Name());
		return False;
	}
	if(!AddOutputInterface(this->SignalsOutputInterface, "PlasmaStatusGAMOutputInterface"))
	{
		AssertErrorCondition(InitialisationError, "PlasmaStatusGAM::Initialise: %s failed to add the TimewindowsGAMOutputInterface", this->Name());
		return False;
	}

//	INPUT SIGNALS (interface)
	if(!cdb->Move("input_signals"))
	{
		AssertErrorCondition(InitialisationError,"PlasmaStatusGAM::Initialise: %s Could not move to \"input_signals\"",this->Name());
		return False;
	}
		int number_of_signals_to_read = 14;
		FString *CDB_move_to;
		FString *SignalType;
		CDB_move_to = new FString[number_of_signals_to_read];
		SignalType = new FString[number_of_signals_to_read];
		CDB_move_to[0].Printf("rogowski_coil");
		CDB_move_to[1].Printf("density");
		CDB_move_to[2].Printf("hibd_r");
		CDB_move_to[3].Printf("hibd_z");
		CDB_move_to[4].Printf("sine_probe_z");
		CDB_move_to[5].Printf("cosine_probe_r");
		CDB_move_to[6].Printf("magnetic_probes_r");
		CDB_move_to[7].Printf("magnetic_probes_z");
		CDB_move_to[8].Printf("magnetic_probes_plasma_current");
		CDB_move_to[9].Printf("electric_probes_r");
		CDB_move_to[10].Printf("electric_probes_z");
		CDB_move_to[11].Printf("tomography_r");
		CDB_move_to[12].Printf("tomography_z");
		CDB_move_to[13].Printf("time");
		for (i=0;i<number_of_signals_to_read;i++){
			
			if(!cdb->Move(CDB_move_to[i].Buffer()))
			{
				AssertErrorCondition(InitialisationError,"PlasmaStatusGAM::Initialise: %s Could not move to \"%s\"",this->Name(),CDB_move_to[i].Buffer());
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
				AssertErrorCondition(Information,"PlasmaStatusGAM::Initialise: Added signal = %s", SignalName.Buffer());
				
				if(!this->SignalsInputInterface->AddSignal(SignalName.Buffer(), SignalType[i].Buffer()))
				{
					AssertErrorCondition(InitialisationError,"PlasmaStatusGAM::Initialise: %s failed to add signal", this->Name());
					return False;
				}
			}
			
			
			cdb->MoveToFather();
		}

	cdb->MoveToFather();


//	OUTPUT SIGNALS (interface)
	if(!cdb->Move("output_signals"))
	{
		AssertErrorCondition(InitialisationError,"PlasmaStatusGAM::Initialise: %s Could not move to \"output_signals\"",this->Name());
		return False;
	}
	
		number_of_signals_to_read = 4;
		CDB_move_to = new FString[number_of_signals_to_read];
		SignalType = new FString[number_of_signals_to_read];
		CDB_move_to[0].Printf("plasma_current");
		CDB_move_to[1].Printf("position_r");
		CDB_move_to[2].Printf("position_z");
		CDB_move_to[3].Printf("density");
		for (i=0;i<number_of_signals_to_read;i++){
			
			if(!cdb->Move(CDB_move_to[i].Buffer()))
			{
				AssertErrorCondition(InitialisationError,"PlasmaStatusGAM::Initialise: %s Could not move to \"%s\"",this->Name(),CDB_move_to[i].Buffer());
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
				AssertErrorCondition(Information,"PlasmaStatusGAM::Initialise: Added signal = %s", SignalName.Buffer());
				
				if(!this->SignalsOutputInterface->AddSignal(SignalName.Buffer(), SignalType[i].Buffer()))
				{
					AssertErrorCondition(InitialisationError,"PlasmaStatusGAM::Initialise: %s failed to add signal", this->Name());
					return False;
				}
			}
			cdb->MoveToFather();
		}

	cdb->MoveToFather();

	return True;
}
//} ******************************************************************



//{ ********* Execute the module functionalities *******************
bool PlasmaStatusGAM::Execute(GAM_FunctionNumbers functionNumber){
	float positionRnume =0.0;
	float positionRdeno =0.0;
	float positionZnume =0.0;
	float positionZdeno =0.0;
 
	InputInterfaceStruct *inputstruct = (InputInterfaceStruct *) this->SignalsInputInterface->Buffer();
	this->SignalsInputInterface->Read();
//	AssertErrorCondition(InitialisationError,"PlasmaStatusGAM:: %s inputstruct = %f %f %f %f %f %f %f %f %f %f %f ",this->Name(), inputstruct[0].RogowskiPlasmaCurrent, inputstruct[0].InterferometryDensity, inputstruct[0].SineProbeZ, inputstruct[0].CosineProbeR, inputstruct[0].MagneticProbesR, inputstruct[0].MagneticProbesZ, inputstruct[0].MagneticProbesPlasmaCurrent, inputstruct[0].ElectricProbesR, inputstruct[0].ElectricProbesZ, inputstruct[0].TomographyR, inputstruct[0].TomographyZ);

	OutputInterfaceStruct *outputstruct = (OutputInterfaceStruct *) this->SignalsOutputInterface->Buffer();
	 
	this->currentTime = inputstruct[0].usectime;

	if(functionNumber == GAMOnline){

			if (inputstruct[0].RogowskiPlasmaCurrent < high_current_threshold_value && inputstruct[0].RogowskiPlasmaCurrent > -high_current_threshold_value){
				positionRnume=0.0;
				positionRdeno=0.0;
				
				if(tomography_radial_bool != 0) {
					positionRnume += tomography_radial_low_current_weight*inputstruct[0].TomographyR;
					positionRdeno += tomography_radial_low_current_weight;
				}
				if(electric_radial_bool != 0) {
					positionRnume += electric_radial_low_current_weight*inputstruct[0].ElectricProbesR;
					positionRdeno += electric_radial_low_current_weight;
				}
				if(magnetic_radial_bool != 0) {
					positionRnume += magnetic_radial_low_current_weight*inputstruct[0].MagneticProbesR;
					positionRdeno += magnetic_radial_low_current_weight;
				}
				if(cosine_radial_bool != 0) {
					positionRnume += cosine_radial_low_current_weight*inputstruct[0].CosineProbeR;
					positionRdeno += cosine_radial_low_current_weight;
				}
				if(hibd_radial_bool != 0) {
					positionRnume += hibd_radial_low_current_weight*inputstruct[0].HeavyIonBeamR;
					positionRdeno += hibd_radial_low_current_weight;
				}
				
				if (positionRdeno != 0) outputstruct[0].PositionR = positionRnume/positionRdeno;
				else outputstruct[0].PositionR = 0;
				
				positionZnume=0.0;
				positionZdeno=0.0;
				
				if(tomography_vertical_bool != 0) {
					positionZnume += tomography_vertical_low_current_weight*inputstruct[0].TomographyZ;
					positionZdeno += tomography_vertical_low_current_weight;
				}
				if(electric_vertical_bool != 0) {
					positionZnume += electric_vertical_low_current_weight*inputstruct[0].ElectricProbesZ;
					positionZdeno += electric_vertical_low_current_weight;
				}
				if(magnetic_vertical_bool != 0) {
					positionZnume += magnetic_vertical_low_current_weight*inputstruct[0].MagneticProbesZ;
					positionZdeno += magnetic_vertical_low_current_weight;
				}
				if(sine_vertical_bool != 0) {
					positionZnume += sine_vertical_low_current_weight*inputstruct[0].SineProbeZ;
					positionZdeno += sine_vertical_low_current_weight;
				}
				if(hibd_vertical_bool != 0) {
					positionZnume += hibd_vertical_low_current_weight*inputstruct[0].HeavyIonBeamZ;
					positionZdeno += hibd_vertical_low_current_weight;
				}
				
				if (positionZdeno != 0) outputstruct[0].PositionZ = positionZnume/positionZdeno;
				else outputstruct[0].PositionZ = 0;
			}
			
			else {
				positionRnume=0.0;
				positionRdeno=0.0;
				
				if(tomography_radial_bool != 0) {
					positionRnume += tomography_radial_high_current_weight*inputstruct[0].TomographyR;
					positionRdeno += tomography_radial_high_current_weight;
				}
				if(electric_radial_bool != 0) {
					positionRnume += electric_radial_high_current_weight*inputstruct[0].ElectricProbesR;
					positionRdeno += electric_radial_high_current_weight;
				}
				if(magnetic_radial_bool != 0) {
					positionRnume += magnetic_radial_high_current_weight*inputstruct[0].MagneticProbesR;
					positionRdeno += magnetic_radial_high_current_weight;
				}
				if(cosine_radial_bool != 0) {
					positionRnume += cosine_radial_high_current_weight*inputstruct[0].CosineProbeR;
					positionRdeno += cosine_radial_high_current_weight;
				}
				if(hibd_radial_bool != 0) {
					positionRnume += hibd_radial_high_current_weight*inputstruct[0].HeavyIonBeamR;
					positionRdeno += hibd_radial_high_current_weight;
				}
				
				if (positionRdeno != 0) outputstruct[0].PositionR = positionRnume/positionRdeno;
				else outputstruct[0].PositionR = 0;
				
				positionZnume=0.0;
				positionZdeno=0.0;
				
				if(tomography_vertical_bool != 0) {
					positionZnume += tomography_vertical_high_current_weight*inputstruct[0].TomographyZ;
					positionZdeno += tomography_vertical_high_current_weight;
				}
				if(electric_vertical_bool != 0) {
					positionZnume += electric_vertical_high_current_weight*inputstruct[0].ElectricProbesZ;
					positionZdeno += electric_vertical_high_current_weight;
				}
				if(magnetic_vertical_bool != 0) {
					positionZnume += magnetic_vertical_high_current_weight*inputstruct[0].MagneticProbesZ;
					positionZdeno += magnetic_vertical_high_current_weight;
				}
				if(sine_vertical_bool != 0) {
					positionZnume += sine_vertical_high_current_weight*inputstruct[0].SineProbeZ;
					positionZdeno += sine_vertical_high_current_weight;
				}
				if(hibd_vertical_bool != 0) {
					positionZnume += hibd_vertical_high_current_weight*inputstruct[0].HeavyIonBeamZ;
					positionZdeno += hibd_vertical_high_current_weight;
				}
				
				if (positionZdeno != 0) outputstruct[0].PositionZ = positionZnume/positionZdeno;
				else outputstruct[0].PositionZ = 0;
			}

		outputstruct[0].Density = inputstruct[0].InterferometryDensity;
		// not used for now: inputstruct[0].MagneticProbesPlasmaCurrent
		outputstruct[0].PlasmaCurrent = inputstruct[0].RogowskiPlasmaCurrent;
	}
	else {
		outputstruct[0].PlasmaCurrent = 0;
		outputstruct[0].PositionR = 0;
		outputstruct[0].PositionZ = 0;
		outputstruct[0].Density = 0;
	}
	
//	AssertErrorCondition(InitialisationError,"PlasmaStatusGAM:: %s outputstruct = %f %f %f %f",this->Name(), outputstruct[0].PlasmaCurrent, outputstruct[0].PositionR, outputstruct[0].PositionZ, outputstruct[0].Density);
	this->SignalsOutputInterface->Write();

	return True;		
}
bool PlasmaStatusGAM::ProcessHttpMessage(HttpStream &hStream){
	
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
		hmStream.SSPrintf(HtmlTagStreamMode, "br><br>high_current_threshold_value = %.2f\n\
		<br>tomography_radial_bool = %d\n\
		<br>tomography_vertical_bool = %d\n\
		<br>tomography_radial_high_current_weight = %.2f\n\
		<br>tomography_radial_low_current_weight = %.2f\n\
		<br>tomography_vertical_high_current_weight = %.2f\n\
		<br>tomography_vertical_low_current_weight = %.2f\n\
		<br>electric_radial_bool = %d\n\
		<br>electric_vertical_bool = %d\n\
		<br>electric_radial_high_current_weight = %.2f\n\
		<br>electric_radial_low_current_weight = %.2f\n\
		<br>electric_vertical_high_current_weight = %.2f\n\
		<br>electric_vertical_low_current_weight = %.2f\n\
		<br>magnetic_radial_bool = %d\n\
		<br>magnetic_vertical_bool = %d\n\
		<br>magnetic_radial_high_current_weight = %.2f\n\
		<br>magnetic_radial_low_current_weight = %.2f\n\
		<br>magnetic_vertical_high_current_weight = %.2f\n\
		<br>magnetic_vertical_low_current_weight = %.2f\n\
		<br>sine_vertical_bool = %d\n\
		<br>cosine_radial_low_current_weight = %.2f\n\
		<br>hibd_radial_bool = %d\n\
		<br>hibd_vertical_bool = %d\n\
		<br>hibd_radial_high_current_weight = %.2f\n\
		<br>hibd_radial_low_current_weight = %.2f\n\
		<br>hibd_vertical_high_current_weight = %.2f\n\
		<br>hibd_vertical_low_current_weight = %.2f\n\
		<br><br",high_current_threshold_value,tomography_radial_bool,tomography_vertical_bool,tomography_radial_high_current_weight,tomography_radial_low_current_weight,tomography_vertical_high_current_weight,tomography_vertical_low_current_weight,electric_radial_bool,electric_vertical_bool,electric_radial_high_current_weight,electric_radial_low_current_weight,electric_vertical_high_current_weight,electric_vertical_low_current_weight,magnetic_radial_bool,magnetic_vertical_bool,magnetic_radial_high_current_weight,magnetic_radial_low_current_weight,magnetic_vertical_high_current_weight,magnetic_vertical_low_current_weight,sine_vertical_bool,cosine_radial_low_current_weight, hibd_radial_bool, hibd_vertical_bool, hibd_radial_high_current_weight, hibd_radial_low_current_weight, hibd_vertical_high_current_weight, hibd_vertical_low_current_weight);
	}
	hmStream.SSPrintf(HtmlTagStreamMode, "/form");

	
	
	hmStream.SSPrintf(HtmlTagStreamMode, "/body>\n</html");
			
	hStream.SSPrintf("OutputHttpOtions.Content-Type","text/html;charset=utf-8");
	hStream.WriteReplyHeader(True);


	return True;		
}
