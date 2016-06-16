//******************************************************************************
//
//		IntegerSequentialControl.cpp - waveform container able to perform basic waveform operations
//		Ivo Carvalho 27/4/2011
//
//******************************************************************************

#if !defined (INTEGERSEQUENTIALCONTROL_H)
#define INTEGERSEQUENTIALCONTROL_H

/** @file 
     waveform container able to perform basic waveform operations */


#include "Level0.h"
#include "Level1.h"
#include "Level2.h"
//OBJECT_DLL(IntegerSequentialControl)

/** waveform container able to perform basic waveform operations */
class IntegerSequentialControl {

//OBJECT_DLL_STUFF(IntegerSequentialControl)
private:

	int number_of_points;
	int *index_vector;
	int *data_vector;
	int OutsideValueDefaultOutput;

	int ju;
	int jl;
	int jm;

public:

	
	int GetWaveformValue(int index_to_search);
	
	bool SaveWaveform(int * received_index_vector,int * received_data_vector, int NumberOfPoints);
	
//	bool AddPoint(float point_index, float point_data);
	
//	bool RemovePoint(int index_to_remove);
		
	int GetSize(){
		return number_of_points;
	}
	
	int GetFirstIndex();
	
	int GetLastIndex();
	
	bool DefineDefaultValue(int outside_value_default_output);
	
	IntegerSequentialControl (int * received_index_vector,int * received_data_vector, int NumberOfPoints);
	
	~IntegerSequentialControl();

private:

	bool SortWaveform();
	bool RemoveRepeatedValues();

};


#endif











