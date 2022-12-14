//******************************************************************************
//
//		IWaveform.cpp - waveform container able to perform basic waveform operations
//		Ivo Carvalho 27/4/2011
//
//******************************************************************************

#if !defined (IWAVEFORM_H)
#define IWAVEFORM_H

/** @file 
     waveform container able to perform basic waveform operations */


#include "Level0.h"
#include "Level1.h"
#include "Level2.h"
//OBJECT_DLL(IWaveform)

/** waveform container able to perform basic waveform operations */
class IWaveform {

//OBJECT_DLL_STUFF(IWaveform)
private:

	int number_of_points;
	float *index_vector;
	float *data_vector;
	float OutsideValueDefaultOutput;
	int jl;
	int ju;
	int jm;

public:

	
	float GetWaveformValue(float index_to_search);
	
	bool SaveWaveform(float * received_index_vector,float * received_data_vector, int NumberOfPoints);
	
//	bool AddPoint(float point_index, float point_data);
	
//	bool RemovePoint(int index_to_remove);
	
//	bool GetIndexVector(float &recived_index_vector);
	
//	bool GetDataVector(float &received_data_vector);
	
	int GetSize(){
		return number_of_points;
	}
	
	float GetFirstIndex();
	
	float GetLastIndex();
	
	bool DefineOutsideValueDefaultValue(float outside_value_default_output);
	
	IWaveform (float * received_index_vector,float * received_data_vector, int NumberOfPoints);
	
	~IWaveform();

private:

	bool SortWaveform();
	bool RemoveRepeatedValues();

};


#endif











