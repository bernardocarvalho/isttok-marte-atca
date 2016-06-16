
#include "TomographyGAM.h"


OBJECTLOADREGISTER(TomographyGAM, "$Id: $")


//  ******** Default constructor ***********************************
TomographyGAM::TomographyGAM(){
	this->SignalsInputInterface = NULL;
	this->SignalsOutputInterface = NULL;
}




// ********* Destructor ********************************************
TomographyGAM::~TomographyGAM()
{
//	if(this->SignalsInputInterface != NULL) delete[] this->SignalsInputInterface ;
//	if(this->SignalsOutputInterface != NULL) delete[] this->SignalsOutputInterface;
}



// ********* Initialise the module ********************************
bool TomographyGAM::Initialise(ConfigurationDataBase& cdbData){
   
	CDBExtended cdb(cdbData);
	int i;
	
	
		if(!cdb.ReadInt32(nch, "nch"))
		{
			AssertErrorCondition(InitialisationError,"TomographyGAM::Initialise: %s nch",this->Name());
			return False;
		}
		else AssertErrorCondition(Information,"TomographyGAM::Initialise: nch = %d",nch);
		
		onlineChannels = new int[nch];
		
		if(!cdb.ReadInt32Array(onlineChannels, (int *)(&nch), 1, "onlineChannels"))
		{
			AssertErrorCondition(InitialisationError,"TomographyGAM::Initialise: Could not onlineChannels");
			return False;
		}
		else for(i=0;i<nch;i++) AssertErrorCondition(Information,"TomographyGAM::Initialise: onlineChannels[%d] = %d",i, onlineChannels[i]);
		
	
		if(!cdb.ReadInt32(i, "tomography_radial_bool"))
		{
			AssertErrorCondition(InitialisationError,"TomographyGAM::Initialise: %s tomography_radial_bool",this->Name());
			return False;
		}
		else 
		{
			tomography_radial_bool = (bool)i;
			AssertErrorCondition(Information,"TomographyGAM::Initialise: tomography_radial_bool = %d",tomography_radial_bool);
		}	
		if(!cdb.ReadInt32(i, "tomography_vertical_bool"))
		{
			AssertErrorCondition(InitialisationError,"TomographyGAM::Initialise: %s tomography_vertical_bool",this->Name());
			return False;
		}
		else 
		{
			tomography_vertical_bool = (bool)i;
			AssertErrorCondition(Information,"TomographyGAM::Initialise: tomography_vertical_bool = %d",tomography_vertical_bool);
		}	
		if(!cdb.ReadInt32(usectime_to_wait_for_starting_operation, "usectime_to_wait_for_starting_operation"))
		{
			AssertErrorCondition(InitialisationError,"TomographyGAM::Initialise: %s usectime_to_wait_for_starting_operation",this->Name());
			return False;
		}
		else AssertErrorCondition(Information,"TomographyGAM::Initialise: usectime_to_wait_for_starting_operation = %d",usectime_to_wait_for_starting_operation);
	
// Create the signal interfaces
	if(!AddInputInterface(this->SignalsInputInterface, "TomographyGAMInputInterface"))
	{
		AssertErrorCondition(InitialisationError, "TomographyGAM::Initialise: %s failed to add the TomographyGAMInputInterface", this->Name());
		return False;
	}
	if(!AddOutputInterface(this->SignalsOutputInterface, "TomographyGAMOutputInterface"))
	{
		AssertErrorCondition(InitialisationError, "TomographyGAM::Initialise: %s failed to add the TomographyGAMOutputInterface", this->Name());
		return False;
	}

//	INPUT SIGNALS (interface)
	if(!cdb->Move("input_signals"))
	{
		AssertErrorCondition(InitialisationError,"TomographyGAM::Initialise: %s Could not move to \"input_signals\"",this->Name());
		return False;
	}
		int number_of_signals_to_read = 25;
		FString *CDB_move_to;
		FString *SignalType;
		CDB_move_to = new FString[number_of_signals_to_read];
		SignalType = new FString[number_of_signals_to_read];
		for(i=0;i<(number_of_signals_to_read-1);i++) CDB_move_to[i].Printf("Channel_%d",i);
		CDB_move_to[number_of_signals_to_read-1].Printf("system_time");
		for (i=0;i<number_of_signals_to_read;i++){
			
			if(!cdb->Move(CDB_move_to[i].Buffer()))
			{
				AssertErrorCondition(InitialisationError,"TomographyGAM::Initialise: %s Could not move to \"%s\"",this->Name(),CDB_move_to[i].Buffer());
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
				AssertErrorCondition(Information,"TomographyGAM::Initialise: Added signal = %s", SignalName.Buffer());
				
				if(!this->SignalsInputInterface->AddSignal(SignalName.Buffer(), SignalType[i].Buffer()))
				{
					AssertErrorCondition(InitialisationError,"TomographyGAM::Initialise: %s failed to add signal", this->Name());
					return False;
				}
			}
			cdb->MoveToFather();
		}

	cdb->MoveToFather();

//	OUTPUT SIGNALS (interface)
	if(!cdb->Move("output_signals"))
	{
		AssertErrorCondition(InitialisationError,"TomographyGAM::Initialise: %s Could not move to \"output_signals\"",this->Name());
		return False;
	}
	
		number_of_signals_to_read = 3;
		CDB_move_to = new FString[number_of_signals_to_read];
		SignalType = new FString[number_of_signals_to_read];
		CDB_move_to[0].Printf("tomography_r");
		CDB_move_to[1].Printf("tomography_z");
		CDB_move_to[2].Printf("tomography_intensity");
		for (i=0;i<number_of_signals_to_read;i++){
			
			if(!cdb->Move(CDB_move_to[i].Buffer()))
			{
				AssertErrorCondition(InitialisationError,"TomographyGAM::Initialise: %s Could not move to \"%s\"",this->Name(),CDB_move_to[i].Buffer());
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
				AssertErrorCondition(Information,"TomographyGAM::Initialise: Added signal = %s", SignalName.Buffer());
				
				if(!this->SignalsOutputInterface->AddSignal(SignalName.Buffer(), SignalType[i].Buffer()))
				{
					AssertErrorCondition(InitialisationError,"TomographyGAM::Initialise: %s failed to add signal", this->Name());
					return False;
				}
			}
			
			cdb->MoveToFather();
		}

	cdb->MoveToFather();
	
//	READ tomography file	
	File temp_file;
	FString file_to_read;
	ConfigurationDataBase file_cdb;

	if(!cdb.ReadFString(file_to_read, "file_to_load"))
	{
		AssertErrorCondition(InitialisationError,"TomographyGAM::Initialise: %s Could not get file_to_load",this->Name());
		return False;
	}
	else AssertErrorCondition(Information,"TomographyGAM::Initialise: file_to_load = %s",file_to_read.Buffer());

   if(!temp_file.OpenRead(file_to_read.Buffer())){
        AssertErrorCondition(InitialisationError, "TomographyGAM::Initialise: Failed opening File %s", file_to_read.Buffer() );
//		temp_file.Close();
        return False;
    }

	file_cdb->ReadFromStream(temp_file);
	CDBExtended cdbe(file_cdb);
	
	if(!cdbe.ReadInt32(nchd, "nchd"))
	{
		AssertErrorCondition(InitialisationError,"TomographyGAM::Initialise: %s nchd",this->Name());
		temp_file.Close();
		return False;
	}
	else	AssertErrorCondition(Information,"TomographyGAM::Initialise: nchd = %d",nchd);
	if(!cdbe.ReadInt32(nbf, "nbf"))
	{
		AssertErrorCondition(InitialisationError,"TomographyGAM::Initialise: %s nbf",this->Name());
		temp_file.Close();
		return False;
	}
	else	AssertErrorCondition(Information,"TomographyGAM::Initialise: nbf = %d",nbf);
	i=2;
	if(!cdbe.ReadInt32Array(griddim, (int *) &i, 1, "griddim"))
	{
		AssertErrorCondition(InitialisationError,"TomographyGAM::Initialise: Could not read griddim");
		temp_file.Close();
		return False;
	}
	else for(i=0;i<2;i++) AssertErrorCondition(Information,"TomographyGAM::Initialise: griddim[%d] = %d",i, griddim[i]);

	xx = new float[griddim[1]];
	yy = new float[griddim[1]];
	svsolW = new float[nbf];
	
	if(!cdbe.ReadFloatArray(xx, (int *)(&griddim[1]), 1, "xx"))
	{
		AssertErrorCondition(InitialisationError,"TomographyGAM::Initialise: Could not read xx");
		temp_file.Close();
		return False;
	}
	else for(i=0;i<nbf;i++) AssertErrorCondition(Information,"TomographyGAM::Initialise: xx[%d] = %f",i, xx[i]);
	if(!cdbe.ReadFloatArray(yy, (int *)(&griddim[1]), 1, "yy"))
	{
		AssertErrorCondition(InitialisationError,"TomographyGAM::Initialise: Could not read yy");
		temp_file.Close();
		return False;
	}
	else for(i=0;i<nbf;i++) AssertErrorCondition(Information,"TomographyGAM::Initialise: yy[%d] = %f",i, yy[i]);
	if(!cdbe.ReadFloatArray(svsolW, (int *)(&nbf), 1, "yy"))
	{
		AssertErrorCondition(InitialisationError,"TomographyGAM::Initialise: Could not read svsolW");
		temp_file.Close();
		return False;
	}
	else for(i=0;i<nbf;i++) AssertErrorCondition(Information,"TomographyGAM::Initialise: svsolW[%d] = %f",i, svsolW[i]);

	temp_max_dim = 2;
	
	if (!cdbe->GetArrayDims(temp_dims,temp_max_dim,"gmask",CDBAIM_Strict)){
		AssertErrorCondition(InitialisationError,"TomographyGAM::Initialise: Could not get gmask dimension");
		temp_file.Close();
		return False;
	} 
	if (temp_max_dim != 2 || temp_dims[0] == 0 || temp_dims[1] == 0){
		AssertErrorCondition(InitialisationError,"TomographyGAM::Initialise: gmask dimension != 2");
		temp_file.Close();
		return False;
	}
	gmask.ReSize(temp_dims[0],temp_dims[1]);
	if (!cdbe.ReadInt32Array((int *)gmask.data,temp_dims,2,"gmask")){
		AssertErrorCondition(InitialisationError,"TomographyGAM::Initialise: could not read gmask matrix");
		temp_file.Close();
		return False;
	}
	else AssertErrorCondition(Information,"TomographyGAM::Initialise: successfully loaded gmask matrix");	 
	gmask = ~gmask;	//transpose

	if (!cdbe->GetArrayDims(temp_dims,temp_max_dim,"svsolV",CDBAIM_Strict)){
		AssertErrorCondition(InitialisationError,"TomographyGAM::Initialise: Could not get svsolV dimension");
		temp_file.Close();
		return False;
	} 
	if (temp_max_dim != 2 || temp_dims[0] == 0 || temp_dims[1] == 0){
		AssertErrorCondition(InitialisationError,"TomographyGAM::Initialise: svsolV dimension != 2");
		temp_file.Close();
		return False;
	}
	svsolV.ReSize(temp_dims[0],temp_dims[1]);
	if (!cdbe.ReadFloatArray((float *)svsolV.data,temp_dims,2,"svsolV")){
		AssertErrorCondition(InitialisationError,"TomographyGAM::Initialise: could not read svsolV matrix");
		temp_file.Close();
		return False;
	}
	else AssertErrorCondition(Information,"TomographyGAM::Initialise: successfully loaded svsolV matrix");	 
	svsolV = ~svsolV;	//transpose

	if (!cdbe->GetArrayDims(temp_dims,temp_max_dim,"svsolU",CDBAIM_Strict)){
		AssertErrorCondition(InitialisationError,"TomographyGAM::Initialise: Could not get svsolU dimension");
		temp_file.Close();
		return False;
	} 
	if (temp_max_dim != 2 || temp_dims[0] == 0 || temp_dims[1] == 0){
		AssertErrorCondition(InitialisationError,"TomographyGAM::Initialise: svsolU dimension != 2");
		temp_file.Close();
		return False;
	}
	svsolU.ReSize(temp_dims[0],temp_dims[1]);
	if (!cdbe.ReadFloatArray((float *)svsolU.data,temp_dims,2,"svsolU")){
		AssertErrorCondition(InitialisationError,"TomographyGAM::Initialise: could not read svsolU matrix");
		temp_file.Close();
		return False;
	}
	else AssertErrorCondition(Information,"TomographyGAM::Initialise: successfully loaded svsolU matrix");	 
	svsolU = ~svsolU;	//transpose

	if (!cdbe->GetArrayDims(temp_dims,temp_max_dim,"grnl00c",CDBAIM_Strict)){
		AssertErrorCondition(InitialisationError,"TomographyGAM::Initialise: Could not get grnl00c dimension");
		temp_file.Close();
		return False;
	} 
	if (temp_max_dim != 2 || temp_dims[0] == 0 || temp_dims[1] == 0){
		AssertErrorCondition(InitialisationError,"TomographyGAM::Initialise: grnl00c dimension != 2");
		temp_file.Close();
		return False;
	}
	grnl00c.ReSize(temp_dims[0],temp_dims[1]);
	if (!cdbe.ReadFloatArray((float *)grnl00c.data,temp_dims,2,"grnl00c")){
		AssertErrorCondition(InitialisationError,"TomographyGAM::Initialise: could not read grnl00c matrix");
		temp_file.Close();
		return False;
	}
	else AssertErrorCondition(Information,"TomographyGAM::Initialise: successfully loaded grnl00c matrix");	 
	grnl00c = ~grnl00c;	//transpose
	
	if (!cdbe->GetArrayDims(temp_dims,temp_max_dim,"grnl01c",CDBAIM_Strict)){
		AssertErrorCondition(InitialisationError,"TomographyGAM::Initialise: Could not get grnl01c dimension");
		temp_file.Close();
		return False;
	} 
	if (temp_max_dim != 2 || temp_dims[0] == 0 || temp_dims[1] == 0){
		AssertErrorCondition(InitialisationError,"TomographyGAM::Initialise: grnl01c dimension != 2");
		temp_file.Close();
		return False;
	}
	grnl01c.ReSize(temp_dims[0],temp_dims[1]);
	if (!cdbe.ReadFloatArray((float *)grnl01c.data,temp_dims,2,"grnl01c")){
		AssertErrorCondition(InitialisationError,"TomographyGAM::Initialise: could not read grnl01c matrix");
		temp_file.Close();
		return False;
	}
	else AssertErrorCondition(Information,"TomographyGAM::Initialise: successfully loaded grnl01c matrix");	 
	grnl01c = ~grnl01c;	//transpose

	if (!cdbe->GetArrayDims(temp_dims,temp_max_dim,"grnl02c",CDBAIM_Strict)){
		AssertErrorCondition(InitialisationError,"TomographyGAM::Initialise: Could not get grnl02c dimension");
		temp_file.Close();
		return False;
	} 
	if (temp_max_dim != 2 || temp_dims[0] == 0 || temp_dims[1] == 0){
		AssertErrorCondition(InitialisationError,"TomographyGAM::Initialise: grnl02c dimension != 2");
		temp_file.Close();
		return False;
	}
	grnl02c.ReSize(temp_dims[0],temp_dims[1]);
	if (!cdbe.ReadFloatArray((float *)grnl02c.data,temp_dims,2,"grnl02c")){
		AssertErrorCondition(InitialisationError,"TomographyGAM::Initialise: could not read grnl02c matrix");
		temp_file.Close();
		return False;
	}
	else AssertErrorCondition(Information,"TomographyGAM::Initialise: successfully loaded grnl02c matrix");	 
	grnl02c = ~grnl02c;	//transpose
	
	if (!cdbe->GetArrayDims(temp_dims,temp_max_dim,"grnl10c",CDBAIM_Strict)){
		AssertErrorCondition(InitialisationError,"TomographyGAM::Initialise: Could not get grnl10c dimension");
		temp_file.Close();
		return False;
	} 
	if (temp_max_dim != 2 || temp_dims[0] == 0 || temp_dims[1] == 0){
		AssertErrorCondition(InitialisationError,"TomographyGAM::Initialise: grnl10c dimension != 2");
		temp_file.Close();
		return False;
	}
	grnl10c.ReSize(temp_dims[0],temp_dims[1]);
	if (!cdbe.ReadFloatArray((float *)grnl10c.data,temp_dims,2,"grnl10c")){
		AssertErrorCondition(InitialisationError,"TomographyGAM::Initialise: could not read grnl10c matrix");
		temp_file.Close();
		return False;
	}
	else AssertErrorCondition(Information,"TomographyGAM::Initialise: successfully loaded grnl10c matrix");	 
	grnl10c = ~grnl10c;	//transpose
	
	if (!cdbe->GetArrayDims(temp_dims,temp_max_dim,"grnl11c",CDBAIM_Strict)){
		AssertErrorCondition(InitialisationError,"TomographyGAM::Initialise: Could not get grnl11c dimension");
		temp_file.Close();
		return False;
	} 
	if (temp_max_dim != 2 || temp_dims[0] == 0 || temp_dims[1] == 0){
		AssertErrorCondition(InitialisationError,"TomographyGAM::Initialise: grnl11c dimension != 2");
		temp_file.Close();
		return False;
	}
	grnl11c.ReSize(temp_dims[0],temp_dims[1]);
	if (!cdbe.ReadFloatArray((float *)grnl11c.data,temp_dims,2,"grnl11c")){
		AssertErrorCondition(InitialisationError,"TomographyGAM::Initialise: could not read grnl11c matrix");
		temp_file.Close();
		return False;
	}
	else AssertErrorCondition(Information,"TomographyGAM::Initialise: successfully loaded grnl11c matrix");	 
	grnl11c = ~grnl11c;	//transpose
	
	if (!cdbe->GetArrayDims(temp_dims,temp_max_dim,"grnl12c",CDBAIM_Strict)){
		AssertErrorCondition(InitialisationError,"TomographyGAM::Initialise: Could not get grnl12c dimension");
		temp_file.Close();
		return False;
	} 
	if (temp_max_dim != 2 || temp_dims[0] == 0 || temp_dims[1] == 0){
		AssertErrorCondition(InitialisationError,"TomographyGAM::Initialise: grnl12c dimension != 2");
		temp_file.Close();
		return False;
	}
	grnl12c.ReSize(temp_dims[0],temp_dims[1]);
	if (!cdbe.ReadFloatArray((float *)grnl12c.data,temp_dims,2,"grnl12c")){
		AssertErrorCondition(InitialisationError,"TomographyGAM::Initialise: could not read grnl12c matrix");
		temp_file.Close();
		return False;
	}
	else AssertErrorCondition(Information,"TomographyGAM::Initialise: successfully loaded grnl12c matrix");	 
	grnl12c = ~grnl12c;	//transpose
	
	if (!cdbe->GetArrayDims(temp_dims,temp_max_dim,"grnl20c",CDBAIM_Strict)){
		AssertErrorCondition(InitialisationError,"TomographyGAM::Initialise: Could not get grnl20c dimension");
		temp_file.Close();
		return False;
	} 
	if (temp_max_dim != 2 || temp_dims[0] == 0 || temp_dims[1] == 0){
		AssertErrorCondition(InitialisationError,"TomographyGAM::Initialise: grnl20c dimension != 2");
		temp_file.Close();
		return False;
	}
	grnl20c.ReSize(temp_dims[0],temp_dims[1]);
	if (!cdbe.ReadFloatArray((float *)grnl20c.data,temp_dims,2,"grnl20c")){
		AssertErrorCondition(InitialisationError,"TomographyGAM::Initialise: could not read grnl20c matrix");
		temp_file.Close();
		return False;
	}
	else AssertErrorCondition(Information,"TomographyGAM::Initialise: successfully loaded grnl20c matrix");	 
	grnl20c = ~grnl20c;	//transpose
	
	if (!cdbe->GetArrayDims(temp_dims,temp_max_dim,"grnl21c",CDBAIM_Strict)){
		AssertErrorCondition(InitialisationError,"TomographyGAM::Initialise: Could not get grnl21c dimension");
		temp_file.Close();
		return False;
	} 
	if (temp_max_dim != 2 || temp_dims[0] == 0 || temp_dims[1] == 0){
		AssertErrorCondition(InitialisationError,"TomographyGAM::Initialise: grnl21c dimension != 2");
		temp_file.Close();
		return False;
	}
	grnl21c.ReSize(temp_dims[0],temp_dims[1]);
	if (!cdbe.ReadFloatArray((float *)grnl21c.data,temp_dims,2,"grnl21c")){
		AssertErrorCondition(InitialisationError,"TomographyGAM::Initialise: could not read grnl21c matrix");
		temp_file.Close();
		return False;
	}
	else AssertErrorCondition(Information,"TomographyGAM::Initialise: successfully loaded grnl21c matrix");	 
	grnl21c = ~grnl21c;	//transpose
	
	if (!cdbe->GetArrayDims(temp_dims,temp_max_dim,"grnl22c",CDBAIM_Strict)){
		AssertErrorCondition(InitialisationError,"TomographyGAM::Initialise: Could not get grnl22c dimension");
		temp_file.Close();
		return False;
	} 
	if (temp_max_dim != 2 || temp_dims[0] == 0 || temp_dims[1] == 0){
		AssertErrorCondition(InitialisationError,"TomographyGAM::Initialise: grnl22c dimension != 2");
		temp_file.Close();
		return False;
	}
	grnl22c.ReSize(temp_dims[0],temp_dims[1]);
	if (!cdbe.ReadFloatArray((float *)grnl22c.data,temp_dims,2,"grnl22c")){
		AssertErrorCondition(InitialisationError,"TomographyGAM::Initialise: could not read grnl22c matrix");
		temp_file.Close();
		return False;
	}
	else AssertErrorCondition(Information,"TomographyGAM::Initialise: successfully loaded grnl22c matrix");	 
	grnl22c = ~grnl22c;	//transpose

	if (!cdbe->GetArrayDims(temp_dims,temp_max_dim,"grnl10s",CDBAIM_Strict)){
		AssertErrorCondition(InitialisationError,"TomographyGAM::Initialise: Could not get grnl10s dimension");
		temp_file.Close();
		return False;
	} 
	if (temp_max_dim != 2 || temp_dims[0] == 0 || temp_dims[1] == 0){
		AssertErrorCondition(InitialisationError,"TomographyGAM::Initialise: grnl10s dimension != 2");
		temp_file.Close();
		return False;
	}
	grnl10s.ReSize(temp_dims[0],temp_dims[1]);
	if (!cdbe.ReadFloatArray((float *)grnl10s.data,temp_dims,2,"grnl10s")){
		AssertErrorCondition(InitialisationError,"TomographyGAM::Initialise: could not read grnl10s matrix");
		temp_file.Close();
		return False;
	}
	else AssertErrorCondition(Information,"TomographyGAM::Initialise: successfully loaded grnl10s matrix");	 
	grnl10s = ~grnl10s;	//transpose
	
	if (!cdbe->GetArrayDims(temp_dims,temp_max_dim,"grnl11s",CDBAIM_Strict)){
		AssertErrorCondition(InitialisationError,"TomographyGAM::Initialise: Could not get grnl11s dimension");
		temp_file.Close();
		return False;
	} 
	if (temp_max_dim != 2 || temp_dims[0] == 0 || temp_dims[1] == 0){
		AssertErrorCondition(InitialisationError,"TomographyGAM::Initialise: grnl11s dimension != 2");
		temp_file.Close();
		return False;
	}
	grnl11s.ReSize(temp_dims[0],temp_dims[1]);
	if (!cdbe.ReadFloatArray((float *)grnl11s.data,temp_dims,2,"grnl11s")){
		AssertErrorCondition(InitialisationError,"TomographyGAM::Initialise: could not read grnl11s matrix");
		temp_file.Close();
		return False;
	}
	else AssertErrorCondition(Information,"TomographyGAM::Initialise: successfully loaded grnl11s matrix");	 
	grnl11s = ~grnl11s;	//transpose
	
	if (!cdbe->GetArrayDims(temp_dims,temp_max_dim,"grnl12s",CDBAIM_Strict)){
		AssertErrorCondition(InitialisationError,"TomographyGAM::Initialise: Could not get grnl12s dimension");
		temp_file.Close();
		return False;
	} 
	if (temp_max_dim != 2 || temp_dims[0] == 0 || temp_dims[1] == 0){
		AssertErrorCondition(InitialisationError,"TomographyGAM::Initialise: grnl12s dimension != 2");
		temp_file.Close();
		return False;
	}
	grnl12s.ReSize(temp_dims[0],temp_dims[1]);
	if (!cdbe.ReadFloatArray((float *)grnl12s.data,temp_dims,2,"grnl12s")){
		AssertErrorCondition(InitialisationError,"TomographyGAM::Initialise: could not read grnl12s matrix");
		temp_file.Close();
		return False;
	}
	else AssertErrorCondition(Information,"TomographyGAM::Initialise: successfully loaded grnl12s matrix");	 
	grnl12s = ~grnl12s;	//transpose
	
	if (!cdbe->GetArrayDims(temp_dims,temp_max_dim,"grnl20s",CDBAIM_Strict)){
		AssertErrorCondition(InitialisationError,"TomographyGAM::Initialise: Could not get grnl20s dimension");
		temp_file.Close();
		return False;
	} 
	if (temp_max_dim != 2 || temp_dims[0] == 0 || temp_dims[1] == 0){
		AssertErrorCondition(InitialisationError,"TomographyGAM::Initialise: grnl20s dimension != 2");
		temp_file.Close();
		return False;
	}
	grnl20s.ReSize(temp_dims[0],temp_dims[1]);
	if (!cdbe.ReadFloatArray((float *)grnl20s.data,temp_dims,2,"grnl20s")){
		AssertErrorCondition(InitialisationError,"TomographyGAM::Initialise: could not read grnl20s matrix");
		temp_file.Close();
		return False;
	}
	else AssertErrorCondition(Information,"TomographyGAM::Initialise: successfully loaded grnl20s matrix");	 
	grnl20s = ~grnl20s;	//transpose
	
	if (!cdbe->GetArrayDims(temp_dims,temp_max_dim,"grnl21s",CDBAIM_Strict)){
		AssertErrorCondition(InitialisationError,"TomographyGAM::Initialise: Could not get grnl21s dimension");
		temp_file.Close();
		return False;
	} 
	if (temp_max_dim != 2 || temp_dims[0] == 0 || temp_dims[1] == 0){
		AssertErrorCondition(InitialisationError,"TomographyGAM::Initialise: grnl21s dimension != 2");
		temp_file.Close();
		return False;
	}
	grnl21s.ReSize(temp_dims[0],temp_dims[1]);
	if (!cdbe.ReadFloatArray((float *)grnl21s.data,temp_dims,2,"grnl21s")){
		AssertErrorCondition(InitialisationError,"TomographyGAM::Initialise: could not read grnl21s matrix");
		temp_file.Close();
		return False;
	}
	else AssertErrorCondition(Information,"TomographyGAM::Initialise: successfully loaded grnl21s matrix");	 
	grnl21s = ~grnl21s;	//transpose
	
	if (!cdbe->GetArrayDims(temp_dims,temp_max_dim,"grnl22s",CDBAIM_Strict)){
		AssertErrorCondition(InitialisationError,"TomographyGAM::Initialise: Could not get grnl22s dimension");
		temp_file.Close();
		return False;
	} 
	if (temp_max_dim != 2 || temp_dims[0] == 0 || temp_dims[1] == 0){
		AssertErrorCondition(InitialisationError,"TomographyGAM::Initialise: grnl22s dimension != 2");
		temp_file.Close();
		return False;
	}
	grnl22s.ReSize(temp_dims[0],temp_dims[1]);
	if (!cdbe.ReadFloatArray((float *)grnl22s.data,temp_dims,2,"grnl22s")){
		AssertErrorCondition(InitialisationError,"TomographyGAM::Initialise: could not read grnl22s matrix");
		temp_file.Close();
		return False;
	}
	else AssertErrorCondition(Information,"TomographyGAM::Initialise: successfully loaded grnl22s matrix");	 
	grnl22s = ~grnl22s;	//transpose
	

	reconstruction.ReSize(griddim[0],griddim[1]);
	int nchOn = 0;
	for (i=0; i<this->nch; i++) if (this->onlineChannels[i]) nchOn++;
	
	usedSignals = new float[nch];
	remove_offset = new float[nch];
	accumulator = new float[nch];
	tempArrayN = new float[nbf];
	aFit = new float[nbf];

	temp_file.Close();
	
	return True;
}
// ******************************************************************



// ********* Execute the module functionalities *******************
bool TomographyGAM::Execute(GAM_FunctionNumbers functionNumber){
 
	InputInterfaceStruct *inputstruct = (InputInterfaceStruct *) this->SignalsInputInterface->Buffer();
	this->SignalsInputInterface->Read();
//	AssertErrorCondition(InitialisationError,"TomographyGAM:: %s inputstruct = %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %d",this->Name(),inputstruct[0].ADC_tomography_top_0,inputstruct[0].ADC_tomography_top_1,inputstruct[0].ADC_tomography_top_2,inputstruct[0].ADC_tomography_top_3,inputstruct[0].ADC_tomography_top_4,inputstruct[0].ADC_tomography_top_5,inputstruct[0].ADC_tomography_top_6,inputstruct[0].ADC_tomography_top_7,inputstruct[0].ADC_tomography_outer_0,inputstruct[0].ADC_tomography_outer_1,inputstruct[0].ADC_tomography_outer_2,inputstruct[0].ADC_tomography_outer_3,inputstruct[0].ADC_tomography_outer_4,inputstruct[0].ADC_tomography_outer_5,inputstruct[0].ADC_tomography_outer_6,inputstruct[0].ADC_tomography_outer_7,inputstruct[0].ADC_tomography_bottom_0,inputstruct[0].ADC_tomography_bottom_1,inputstruct[0].ADC_tomography_bottom_2,inputstruct[0].ADC_tomography_bottom_3,inputstruct[0].ADC_tomography_bottom_4,inputstruct[0].ADC_tomography_bottom_5,inputstruct[0].ADC_tomography_bottom_6,inputstruct[0].ADC_tomography_bottom_7, inputstruct[0].usectime);
	
	OutputInterfaceStruct *outputstruct = (OutputInterfaceStruct *) this->SignalsOutputInterface->Buffer();


   if(functionNumber == GAMOnline){
		
	   if(inputstruct[0].usectime > 0 && inputstruct[0].usectime < usectime_to_wait_for_starting_operation){
		    n_samples++;
			
			accumulator[0] += (float)inputstruct[0].ADC_tomography_top_0;
			accumulator[1] += (float)inputstruct[0].ADC_tomography_top_1;
			accumulator[2] += (float)inputstruct[0].ADC_tomography_top_2;
			accumulator[3] += (float)inputstruct[0].ADC_tomography_top_3;
			accumulator[4] += (float)inputstruct[0].ADC_tomography_top_4;
			accumulator[5] += (float)inputstruct[0].ADC_tomography_top_5;
			accumulator[6] += (float)inputstruct[0].ADC_tomography_top_6;
			accumulator[7] += (float)inputstruct[0].ADC_tomography_top_7;
			accumulator[8] += (float)inputstruct[0].ADC_tomography_outer_0;
			accumulator[9] += (float)inputstruct[0].ADC_tomography_outer_1;
			accumulator[10] += (float)inputstruct[0].ADC_tomography_outer_2;
			accumulator[11] += (float)inputstruct[0].ADC_tomography_outer_3;
			accumulator[12] += (float)inputstruct[0].ADC_tomography_outer_4;
			accumulator[13] += (float)inputstruct[0].ADC_tomography_outer_5;
			accumulator[14] += (float)inputstruct[0].ADC_tomography_outer_6;
			accumulator[15] += (float)inputstruct[0].ADC_tomography_outer_7;
			accumulator[16] += (float)inputstruct[0].ADC_tomography_bottom_0;
			accumulator[17] += (float)inputstruct[0].ADC_tomography_bottom_1;
			accumulator[18] += (float)inputstruct[0].ADC_tomography_bottom_2;
			accumulator[19] += (float)inputstruct[0].ADC_tomography_bottom_3;
			accumulator[20] += (float)inputstruct[0].ADC_tomography_bottom_4;
			accumulator[21] += (float)inputstruct[0].ADC_tomography_bottom_5;
			accumulator[22] += (float)inputstruct[0].ADC_tomography_bottom_6;
			accumulator[23] += (float)inputstruct[0].ADC_tomography_bottom_7;

			outputstruct[0].TomographyR = 0;
			outputstruct[0].TomographyZ = 0;
			outputstruct[0].TomographyIntensity = 0;

			for(i=0;i<nch;i++) remove_offset[i] = accumulator[i] / n_samples;
	   }
	   else{
			if (this->n_samples >0 ){

				for(i = 0 ; i < this->nch ; i++) AssertErrorCondition(Information,"MagneticsGAM::Execute: %s OFFSETS %d = %f, number of samples = %d", this->Name(), i, this->remove_offset[i], n_samples);
				n_samples = 0;
			}
		   
			usedSignals[0] =  (float) inputstruct[0].ADC_tomography_top_0 - remove_offset[0];
			usedSignals[1] =  (float) inputstruct[0].ADC_tomography_top_1 - remove_offset[1];
			usedSignals[2] =  (float) inputstruct[0].ADC_tomography_top_2 - remove_offset[2];
			usedSignals[3] =  (float) inputstruct[0].ADC_tomography_top_3 - remove_offset[3];
			usedSignals[4] =  (float) inputstruct[0].ADC_tomography_top_4 - remove_offset[4];
			usedSignals[5] =  (float) inputstruct[0].ADC_tomography_top_5 - remove_offset[5];
			usedSignals[6] =  (float) inputstruct[0].ADC_tomography_top_6 - remove_offset[6];
			usedSignals[7] =  (float) inputstruct[0].ADC_tomography_top_7 - remove_offset[7];
			usedSignals[8] =  (float) inputstruct[0].ADC_tomography_outer_0 - remove_offset[8];
			usedSignals[9] =  (float) inputstruct[0].ADC_tomography_outer_1 - remove_offset[9];
			usedSignals[10] = (float) inputstruct[0].ADC_tomography_outer_2 - remove_offset[10];
			usedSignals[11] = (float) inputstruct[0].ADC_tomography_outer_3 - remove_offset[11];
			usedSignals[12] = (float) inputstruct[0].ADC_tomography_outer_4 - remove_offset[12];
			usedSignals[13] = (float) inputstruct[0].ADC_tomography_outer_5 - remove_offset[13];
			usedSignals[14] = (float) inputstruct[0].ADC_tomography_outer_6 - remove_offset[14];
			usedSignals[15] = (float) inputstruct[0].ADC_tomography_outer_7 - remove_offset[15];
			usedSignals[16] = (float) inputstruct[0].ADC_tomography_bottom_0 - remove_offset[16];
			usedSignals[17] = (float) inputstruct[0].ADC_tomography_bottom_1 - remove_offset[17];
			usedSignals[18] = (float) inputstruct[0].ADC_tomography_bottom_2 - remove_offset[18];
			usedSignals[19] = (float) inputstruct[0].ADC_tomography_bottom_3 - remove_offset[19];
			usedSignals[20] = (float) inputstruct[0].ADC_tomography_bottom_4 - remove_offset[20];
			usedSignals[21] = (float) inputstruct[0].ADC_tomography_bottom_5 - remove_offset[21];
			usedSignals[22] = (float) inputstruct[0].ADC_tomography_bottom_6 - remove_offset[22];
			usedSignals[23] = (float) inputstruct[0].ADC_tomography_bottom_7 - remove_offset[23];
							
			//Algorithm described in Numerical Recipes in C, ch15.4
			for (j=0; j<nbf; j++){
				s=0;
				for( i=0; i<nch;i++){  //360 cycles
						
						if (onlineChannels[i]) s += svsolU[j][i]*usedSignals[i];
				}
				s *= svsolW[j];
				tempArrayN[j]=s;
			}
			for (i=0; i<nbf; i++){
				
				s = 0.;
				for (j=0; j<nbf; j++){ //225 cycles
				
					s += (svsolV[j][i] * tempArrayN[j]);
				}
				aFit[i] = s;
			}
			//drawing of the reconstruction by multiplying the fitted parameters in 'aFit' by the basis functions previously calculated and stored in the configuration file
			mass = 0.;
			x = 0.;
			y = 0.;
			for (i=0; i<griddim[0]; i++){
				for (j=0; j<griddim[1]; j++){

					if (gmask[i][j]){					//149 cycles
						
						reconstruction[i][j]= aFit[0]*grnl00c[i][j] + aFit[1]*grnl01c[i][j] + aFit[2]*grnl02c[i][j] + aFit[3]*grnl10c[i][j] + aFit[4]*grnl11c[i][j] + aFit[5]*grnl12c[i][j] + aFit[6]*grnl20c[i][j] + aFit[7]*grnl21c[i][j] + aFit[8]*grnl22c[i][j] + aFit[9]*grnl10s[i][j] + aFit[10]*grnl11s[i][j] + aFit[11]*grnl12s[i][j] + aFit[12]*grnl20s[i][j] + aFit[13]*grnl21s[i][j] + aFit[14]*grnl22s[i][j];
						mass += reconstruction[i][j];
						x += xx[i] * reconstruction[i][j];
						y += yy[j] * reconstruction[i][j];
					}
				}
			}
			
			if(mass != 0.){
				outputstruct[0].TomographyR = x / (mass);
				outputstruct[0].TomographyZ = y / (mass);
			}
			else {
				outputstruct[0].TomographyR = 0.;
				outputstruct[0].TomographyZ = 0.;
			}
			
			outputstruct[0].TomographyIntensity = 0.;
			for( i=0; i<nch;i++)	if (onlineChannels[i]) outputstruct[0].TomographyIntensity += usedSignals[i];
	   }
	   
	}
	else {
		outputstruct[0].TomographyR = 0;
		outputstruct[0].TomographyZ = 0;
		outputstruct[0].TomographyIntensity = 0;
		n_samples = 0;
		for (i=0;i<this->nch;i++){
			remove_offset[i] = 0;
			accumulator[i] = 0;
		}
	}
	
	this->SignalsOutputInterface->Write();
	
	return True;		
}
bool TomographyGAM::ProcessHttpMessage(HttpStream &hStream){
	
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
		hmStream.SSPrintf(HtmlTagStreamMode, "br><br>tomography_radial_bool = %d\n\
		<br>tomography_vertical_bool = %d\n\
		<br>nch = %d\n\
		<br>nchd = %d\n\
		<br>nbf = %d\n\
		<br>griddim[0] = %d, griddim[1] = %d\n\
		<br><br", tomography_radial_bool,tomography_vertical_bool,nch,nchd,nbf,griddim[0],griddim[1]);

		hmStream.SSPrintf(HtmlTagStreamMode, "table border=\"1\"><tr><td>onlineChannels</td");
		for (i=0;i<nch;i++)hmStream.SSPrintf(HtmlTagStreamMode, "td>%d</td",onlineChannels[i]);
		hmStream.SSPrintf(HtmlTagStreamMode, "/tr></table><br");

		hmStream.SSPrintf(HtmlTagStreamMode, "table border=\"1\"><tr><td>xx</td");
		for (i=0;i<nbf;i++)hmStream.SSPrintf(HtmlTagStreamMode, "td>%.4f</td",xx[i]);
		hmStream.SSPrintf(HtmlTagStreamMode, "/tr></table><br");

		hmStream.SSPrintf(HtmlTagStreamMode, "table border=\"1\"><tr><td>yy</td");
		for (i=0;i<nbf;i++)hmStream.SSPrintf(HtmlTagStreamMode, "td>%.4f</td",yy[i]);
		hmStream.SSPrintf(HtmlTagStreamMode, "/tr></table><br");

		hmStream.SSPrintf(HtmlTagStreamMode, "table border=\"1\"><tr><td>svsolW</td");
		for (i=0;i<nbf;i++)hmStream.SSPrintf(HtmlTagStreamMode, "td>%.4f</td",svsolW[i]);
		hmStream.SSPrintf(HtmlTagStreamMode, "/tr></table><br");


		int j;
		hmStream.SSPrintf(HtmlTagStreamMode, "br><b>gmask</b>\n<table border=\"1\"");
		for (j=0;j<griddim[0];j++){
			hmStream.SSPrintf(HtmlTagStreamMode, "tr");
			for (i=0;i<griddim[1];i++)hmStream.SSPrintf(HtmlTagStreamMode, "td> %d </td",gmask[i][j]);
			hmStream.SSPrintf(HtmlTagStreamMode, "/tr");
		}
		hmStream.SSPrintf(HtmlTagStreamMode, "/table><br");

		hmStream.SSPrintf(HtmlTagStreamMode, "br><b>svsolV</b>\n<table border=\"1\"");
		for (j=0;j<nbf;j++){
			hmStream.SSPrintf(HtmlTagStreamMode, "tr");
			for (i=0;i<nbf;i++)hmStream.SSPrintf(HtmlTagStreamMode, "td> %.4f </td",svsolV[i][j]);
			hmStream.SSPrintf(HtmlTagStreamMode, "/tr");
		}
		hmStream.SSPrintf(HtmlTagStreamMode, "/table><br");

		hmStream.SSPrintf(HtmlTagStreamMode, "br><b>svsolU</b>\n<table border=\"1\"");
		for (j=0;j<nch;j++){
			hmStream.SSPrintf(HtmlTagStreamMode, "tr");
			for (i=0;i<nbf;i++)hmStream.SSPrintf(HtmlTagStreamMode, "td> %.4f </td",svsolU[i][j]);
			hmStream.SSPrintf(HtmlTagStreamMode, "/tr");
		}
		hmStream.SSPrintf(HtmlTagStreamMode, "/table><br");
		
		hmStream.SSPrintf(HtmlTagStreamMode, "br><b>grnl00c</b>\n<table border=\"1\"");
		for (j=0;j<griddim[0];j++){
			hmStream.SSPrintf(HtmlTagStreamMode, "tr");
			for (i=0;i<griddim[1];i++)hmStream.SSPrintf(HtmlTagStreamMode, "td> %.4f </td",grnl00c[i][j]);
			hmStream.SSPrintf(HtmlTagStreamMode, "/tr");
		}
		hmStream.SSPrintf(HtmlTagStreamMode, "/table><br");
		
		hmStream.SSPrintf(HtmlTagStreamMode, "br><b>grnl01c</b>\n<table border=\"1\"");
		for (j=0;j<griddim[0];j++){
			hmStream.SSPrintf(HtmlTagStreamMode, "tr");
			for (i=0;i<griddim[1];i++)hmStream.SSPrintf(HtmlTagStreamMode, "td> %.4f </td",grnl01c[i][j]);
			hmStream.SSPrintf(HtmlTagStreamMode, "/tr");
		}
		hmStream.SSPrintf(HtmlTagStreamMode, "/table><br");
		
		hmStream.SSPrintf(HtmlTagStreamMode, "br><b>grnl02c</b>\n<table border=\"1\"");
		for (j=0;j<griddim[0];j++){
			hmStream.SSPrintf(HtmlTagStreamMode, "tr");
			for (i=0;i<griddim[1];i++)hmStream.SSPrintf(HtmlTagStreamMode, "td> %.4f </td",grnl02c[i][j]);
			hmStream.SSPrintf(HtmlTagStreamMode, "/tr");
		}
		hmStream.SSPrintf(HtmlTagStreamMode, "/table><br");
		
		hmStream.SSPrintf(HtmlTagStreamMode, "br><b>grnl10c</b>\n<table border=\"1\"");
		for (j=0;j<griddim[0];j++){
			hmStream.SSPrintf(HtmlTagStreamMode, "tr");
			for (i=0;i<griddim[1];i++)hmStream.SSPrintf(HtmlTagStreamMode, "td> %.4f </td",grnl10c[i][j]);
			hmStream.SSPrintf(HtmlTagStreamMode, "/tr");
		}
		hmStream.SSPrintf(HtmlTagStreamMode, "/table><br");
		
		hmStream.SSPrintf(HtmlTagStreamMode, "br><b>grnl11c</b>\n<table border=\"1\"");
		for (j=0;j<griddim[0];j++){
			hmStream.SSPrintf(HtmlTagStreamMode, "tr");
			for (i=0;i<griddim[1];i++)hmStream.SSPrintf(HtmlTagStreamMode, "td> %.4f </td",grnl11c[i][j]);
			hmStream.SSPrintf(HtmlTagStreamMode, "/tr");
		}
		hmStream.SSPrintf(HtmlTagStreamMode, "/table><br");
		
		hmStream.SSPrintf(HtmlTagStreamMode, "br><b>grnl12c</b>\n<table border=\"1\"");
		for (j=0;j<griddim[0];j++){
			hmStream.SSPrintf(HtmlTagStreamMode, "tr");
			for (i=0;i<griddim[1];i++)hmStream.SSPrintf(HtmlTagStreamMode, "td> %.4f </td",grnl12c[i][j]);
			hmStream.SSPrintf(HtmlTagStreamMode, "/tr");
		}
		hmStream.SSPrintf(HtmlTagStreamMode, "/table><br");
		
		hmStream.SSPrintf(HtmlTagStreamMode, "br><b>grnl20c</b>\n<table border=\"1\"");
		for (j=0;j<griddim[0];j++){
			hmStream.SSPrintf(HtmlTagStreamMode, "tr");
			for (i=0;i<griddim[1];i++)hmStream.SSPrintf(HtmlTagStreamMode, "td> %.4f </td",grnl20c[i][j]);
			hmStream.SSPrintf(HtmlTagStreamMode, "/tr");
		}
		hmStream.SSPrintf(HtmlTagStreamMode, "/table><br");
		
		hmStream.SSPrintf(HtmlTagStreamMode, "br><b>grnl21c</b>\n<table border=\"1\"");
		for (j=0;j<griddim[0];j++){
			hmStream.SSPrintf(HtmlTagStreamMode, "tr");
			for (i=0;i<griddim[1];i++)hmStream.SSPrintf(HtmlTagStreamMode, "td> %.4f </td",grnl21c[i][j]);
			hmStream.SSPrintf(HtmlTagStreamMode, "/tr");
		}
		hmStream.SSPrintf(HtmlTagStreamMode, "/table><br");
		
		hmStream.SSPrintf(HtmlTagStreamMode, "br><b>grnl22c</b>\n<table border=\"1\"");
		for (j=0;j<griddim[0];j++){
			hmStream.SSPrintf(HtmlTagStreamMode, "tr");
			for (i=0;i<griddim[1];i++)hmStream.SSPrintf(HtmlTagStreamMode, "td> %.4f </td",grnl22c[i][j]);
			hmStream.SSPrintf(HtmlTagStreamMode, "/tr");
		}
		hmStream.SSPrintf(HtmlTagStreamMode, "/table><br");
		
		hmStream.SSPrintf(HtmlTagStreamMode, "br><b>grnl10s</b>\n<table border=\"1\"");
		for (j=0;j<griddim[0];j++){
			hmStream.SSPrintf(HtmlTagStreamMode, "tr");
			for (i=0;i<griddim[1];i++)hmStream.SSPrintf(HtmlTagStreamMode, "td> %.4f </td",grnl10s[i][j]);
			hmStream.SSPrintf(HtmlTagStreamMode, "/tr");
		}
		hmStream.SSPrintf(HtmlTagStreamMode, "/table><br");
		
		hmStream.SSPrintf(HtmlTagStreamMode, "br><b>grnl11s</b>\n<table border=\"1\"");
		for (j=0;j<griddim[0];j++){
			hmStream.SSPrintf(HtmlTagStreamMode, "tr");
			for (i=0;i<griddim[1];i++)hmStream.SSPrintf(HtmlTagStreamMode, "td> %.4f </td",grnl11s[i][j]);
			hmStream.SSPrintf(HtmlTagStreamMode, "/tr");
		}
		hmStream.SSPrintf(HtmlTagStreamMode, "/table><br");
		
		hmStream.SSPrintf(HtmlTagStreamMode, "br><b>grnl12s</b>\n<table border=\"1\"");
		for (j=0;j<griddim[0];j++){
			hmStream.SSPrintf(HtmlTagStreamMode, "tr");
			for (i=0;i<griddim[1];i++)hmStream.SSPrintf(HtmlTagStreamMode, "td> %.4f </td",grnl12s[i][j]);
			hmStream.SSPrintf(HtmlTagStreamMode, "/tr");
		}
		hmStream.SSPrintf(HtmlTagStreamMode, "/table><br");
		
		hmStream.SSPrintf(HtmlTagStreamMode, "br><b>grnl20s</b>\n<table border=\"1\"");
		for (j=0;j<griddim[0];j++){
			hmStream.SSPrintf(HtmlTagStreamMode, "tr");
			for (i=0;i<griddim[1];i++)hmStream.SSPrintf(HtmlTagStreamMode, "td> %.4f </td",grnl20s[i][j]);
			hmStream.SSPrintf(HtmlTagStreamMode, "/tr");
		}
		hmStream.SSPrintf(HtmlTagStreamMode, "/table><br");
		
		hmStream.SSPrintf(HtmlTagStreamMode, "br><b>grnl21s</b>\n<table border=\"1\"");
		for (j=0;j<griddim[0];j++){
			hmStream.SSPrintf(HtmlTagStreamMode, "tr");
			for (i=0;i<griddim[1];i++)hmStream.SSPrintf(HtmlTagStreamMode, "td> %.4f </td",grnl21s[i][j]);
			hmStream.SSPrintf(HtmlTagStreamMode, "/tr");
		}
		hmStream.SSPrintf(HtmlTagStreamMode, "/table><br");
		
		hmStream.SSPrintf(HtmlTagStreamMode, "br><b>grnl22s</b>\n<table border=\"1\"");
		for (j=0;j<griddim[0];j++){
			hmStream.SSPrintf(HtmlTagStreamMode, "tr");
			for (i=0;i<griddim[1];i++)hmStream.SSPrintf(HtmlTagStreamMode, "td> %.4f </td",grnl22s[i][j]);
			hmStream.SSPrintf(HtmlTagStreamMode, "/tr");
		}
		hmStream.SSPrintf(HtmlTagStreamMode, "/table><br");
	}
	hmStream.SSPrintf(HtmlTagStreamMode, "/form");
	
	hmStream.SSPrintf(HtmlTagStreamMode, "/body>\n</html");
	hStream.SSPrintf("OutputHttpOtions.Content-Type","text/html;charset=utf-8");
	hStream.WriteReplyHeader(True);


	return True;		
}

