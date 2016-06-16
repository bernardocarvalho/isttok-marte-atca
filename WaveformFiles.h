/*
	ivoc - ivoc@ipfn.ist.utl.pt
	
	Description: reads a .waveform file and retrieves the short_name and the vector containing x/y points of a waveform, it also retrieves the vector_dim
				writes a .waveform containing the vectors with x/y points, also writes the short_name and vector size
				 contains a method to obtaion the vector size before reading the file completely
*/
#if !defined WAVEFORMFILES_H
#define WAVEFORMFILES_H

#include "File.h"

bool GetWaveformSize(char FilePath[], int * vector_dim){
	
	File temp_file;
	int vectordim;
		
	ConfigurationDataBase cdb;

   if(!temp_file.OpenRead(FilePath)){
        CStaticAssertErrorCondition(InitialisationError, "GetWaveformSize: Failed opening File %s", FilePath );
//		temp_file.Close();
        return False;
    }

	cdb->ReadFromStream(temp_file);
	CDBExtended cdbe(cdb);
  
	if(!cdbe->Move("description"))
	{
		CStaticAssertErrorCondition(InitialisationError,"GetWaveformSize: Could not read description, filepath = %s",FilePath);
		temp_file.Close();
		return False;
	}
//	else CStaticAssertErrorCondition(Information, "GetWaveformSize: move to description ok");

		if(!cdbe.ReadInt32(vectordim, "vector_size"))
		{
			CStaticAssertErrorCondition(InitialisationError,"GetWaveformSize: Could not read vector_size, filepath = %s", FilePath);
			temp_file.Close();
			return False;
		}
//		else	CStaticAssertErrorCondition(Information, "GetWaveformSize: %s vector_size = %d", FilePath, vectordim);


	cdbe->MoveToFather();
	temp_file.Close();
	*vector_dim = vectordim;
	if (vectordim >= 2)	return True;
	return False;
}

//	remark, make sure to set the correct size for vector_x and vector_y before calling this function...else => segmentation fault
bool ReadWaveformFile(FString * short_name_out,char FilePath[],int * vector_dim,double * vector_x,double * vector_y) {
	
	double *vectorx;
	double *vectory;
	int vectordim;
	int i;
	File temp_file;
	FString temp_fstring;

	ConfigurationDataBase cdb;

   if(!temp_file.OpenRead(FilePath)){
        CStaticAssertErrorCondition(InitialisationError, "ReadWaveformFiles: Failed opening File %s", FilePath );
        return False;
    }
	
	cdb->ReadFromStream(temp_file);
	CDBExtended cdbe(cdb);
	
	if(!cdbe->Move("description"))
	{
		CStaticAssertErrorCondition(InitialisationError,"ReadWaveformFiles::Initialise: Could not read description");
		temp_file.Close();
		return False;
	}
//	else CStaticAssertErrorCondition(Information, "ReadWaveformFiles: move to description ok");
  
		if(!cdbe.ReadFString(temp_fstring, "short_name"))
		{
			CStaticAssertErrorCondition(InitialisationError,"ReadWaveformFiles: Could not read short_name");
			temp_file.Close();
			return False;
		}
//		else	CStaticAssertErrorCondition(Information, "ReadWaveformFiles: %s short_name = %s", FilePath, temp_fstring.Buffer());
		*short_name_out = temp_fstring.Buffer();


		if(!cdbe.ReadInt32(vectordim, "vector_size"))
		{
			CStaticAssertErrorCondition(InitialisationError,"WReadWaveformFiles: Could not read vector_size");
			temp_file.Close();
			return False;
		}
//		else	CStaticAssertErrorCondition(Information, "ReadWaveformFiles: %s vector_size = %d", FilePath, vectordim);


	cdbe->MoveToFather();

	if(!cdbe->Move("data"))
	{
		CStaticAssertErrorCondition(InitialisationError,"ReadWaveformFiles: error moving to data");
		temp_file.Close();
		return False;
	}
// 	else CStaticAssertErrorCondition(Information, "ReadWaveformFiles: move to data ok");
  
		vectorx =new double[vectordim];
		vectory =new double[vectordim];


		if(!cdbe.ReadDoubleArray(vectorx, (int *)(&vectordim), 1, "index_vector"))
		{
			CStaticAssertErrorCondition(InitialisationError,"ReadWaveformFiles: Could not read index_vector");
			temp_file.Close();
			return False;
		}
//		else CStaticAssertErrorCondition(InitialisationError, "read index_vector ok");
 
		if(!cdbe.ReadDoubleArray(vectory, (int *)(&vectordim), 1, "data_vector"))
		{
			CStaticAssertErrorCondition(InitialisationError,"ReadWaveformFiles: Could not read data_vector");
			temp_file.Close();
			return False;
		}
//		else CStaticAssertErrorCondition(Information, "ReadWaveformFiles: read data_vector ok");
 
	cdbe->MoveToFather();

	temp_file.Close();

		for (i=0;i<vectordim;i++){
			*(vector_x+i) = vectorx[i];
			*(vector_y+i) = vectory[i];
		}

	*vector_dim = vectordim;
	
	return True;
}
bool WriteWaveformFile(char short_name[],char FilePath[],int vector_dim,double * vector_x,double * vector_y) {
	
	int i;
	File temp_file;

   if(!temp_file.OpenWrite(FilePath)){
        CStaticAssertErrorCondition(InitialisationError, "WriteWaveformFiles: Failed opening File %s", FilePath );
        return False;
    }
	
	temp_file.Printf("description =\n{\n\tshort_name = \"%s\"\n\tvector_size = %d\n}\ndata =\n{\n\tindex_vector = {",short_name, vector_dim);
	for (i=0;i<vector_dim-1;i++){
		temp_file.Printf(" %f",*(vector_x+i));
	}
	temp_file.Printf(" %f }\n\tdatavector = {",*(vector_x+vector_dim-1));
	for (i=0;i<vector_dim-1;i++){
		temp_file.Printf(" %f",*(vector_y+i));
	}
	temp_file.Printf(" %f }\n}\n",*(vector_y+vector_dim-1));	

	temp_file.Close();
	
	return True;
}

#endif