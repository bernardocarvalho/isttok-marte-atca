//******************************************************************************
//
//		IntegerSequentialControl.cpp - waveform container able to perform basic waveform operations
//		Ivo Carvalho 27/4/2011
//
//******************************************************************************


#include "IntegerSequentialControl.h"

//OBJECTLOADREGISTER(IntegerSequentialControl,"$Id: IntegerSequentialControl.cpp,v 1.0 27/4/2011 14:22:36 ivoc Exp $")

IntegerSequentialControl::IntegerSequentialControl(int * received_index_vector,int * received_data_vector, int NumberOfPoints){
	
	if( NumberOfPoints >=1) {
		number_of_points = NumberOfPoints;
		index_vector = new int[NumberOfPoints];
		data_vector = new int[NumberOfPoints];
		
		int i;
		for (i = 0; i < NumberOfPoints ; i++ ){
			index_vector[i] = *(received_index_vector+i);
			data_vector[i] = *(received_data_vector+i);
		}
		this->SortWaveform();
		this->RemoveRepeatedValues();
	}
}

// ********* Destructor ********************************************
IntegerSequentialControl::~IntegerSequentialControl(){
	
//	if(this->index_vector != NULL) delete this->index_vector;
//	if(this->data_vector != NULL) delete this->data_vector;
}

int IntegerSequentialControl::GetWaveformValue(int index_to_search){
	
	if (this->number_of_points == 0) return 0;
	if (this->number_of_points == 1 && index_to_search >= this->index_vector[0]) return this->data_vector[0];
	//if out of bounds
	if (this->index_vector[0] > index_to_search ) return this->OutsideValueDefaultOutput;
	if( this->index_vector[this->number_of_points-1] <= index_to_search) return this->data_vector[this->number_of_points-1];
	// search algorithm based on a regular SAR ADC model - bisection routine - numerical receips in C chapter 3.4
	this->jl = 0;
	this->ju = this->number_of_points-1;
	while (ju-jl > 1){
		this->jm = (this->ju+this->jl) >> 1;	//(shift 1 bit to the left same as divide by 2 and then cast to int)
		if (index_to_search >= this->index_vector[jm]) this->jl = this->jm;
		else this->ju = this->jm;
	}// return linear interpolation between the upper and the lower point
	
//	CStaticAssertErrorCondition(InitialisationError,"jl = %d",jl);
	return (this->data_vector[jl]);	
}

bool IntegerSequentialControl::SaveWaveform(int * received_index_vector,int * received_data_vector, int NumberOfPoints){
	
	if (NumberOfPoints != number_of_points) {
	
		index_vector = new int[NumberOfPoints];
		data_vector = new int[NumberOfPoints];
		number_of_points = NumberOfPoints;
	}
	if (NumberOfPoints < 1) return False;
	
	int i;
	for (i = 0; i < NumberOfPoints ; i++ ){
		index_vector[i] = *(received_index_vector+i);
		data_vector[i] = *(received_data_vector+i);
	}
	
	if(this->SortWaveform()) return (this->RemoveRepeatedValues());
}

int IntegerSequentialControl::GetFirstIndex(){

	if (this->number_of_points > 0){
		return index_vector[0];
	}
	else return -1;
}

int IntegerSequentialControl::GetLastIndex(){
	
	if (this->number_of_points > 0){
		return index_vector[number_of_points-1];
	}
	else return -1;
}

/* not done yet
bool WaveformGAM::AddPoint(float point_index, float point_data){
	
	
	return True;
}

bool WaveformGAM::RemovePoint(int index_to_remove){
	
	
	return True;
}

bool WaveformGAM::GetDataVector(float &received_data_vector){
	
	
	
}
*/
bool IntegerSequentialControl::DefineDefaultValue(int outside_value_default_output){
	
	this->OutsideValueDefaultOutput = outside_value_default_output;
}

bool IntegerSequentialControl::RemoveRepeatedValues(){
	
	if (this->number_of_points < 1) return False;
	if (this->number_of_points == 1) return True;
	
	int i = 0;
	int alfa;
	
	for (alfa = 0; alfa < this->number_of_points-1; alfa++){

		if ( this->index_vector[alfa] == this->index_vector[alfa+1]){
			
			for (i = alfa; i < this->number_of_points-2; i++){
				this->index_vector[i+1] = this->index_vector[i+2];
				this->data_vector[i+1] = this->data_vector[i+2];
			}
			alfa--;
			this->number_of_points--;
		}
	}
	return True;
}

bool IntegerSequentialControl::SortWaveform(){
	
	if (this->number_of_points < 1) return False;
	if (this->number_of_points == 1) return True;

	int i;
	int alfa;
	int temp;
	int min = this->index_vector[0];
	int pos;
	
	for(alfa = 0; alfa < this->number_of_points-1; alfa++){	
		pos = alfa;
		min = this->index_vector[alfa];
		
		for (i = alfa;i < this->number_of_points; i++){
			if ( this->index_vector[i] <= min){
				pos = i;
				min = this->index_vector[i];
			}	
		}	

		temp = this->index_vector[alfa];
		this->index_vector[alfa] = this->index_vector[pos];
		this->index_vector[pos] = temp;
		temp = this->data_vector[alfa];
		this->data_vector[alfa] = this->data_vector[pos];
		this->data_vector[pos] = temp;
	}
	
	return True;
}
