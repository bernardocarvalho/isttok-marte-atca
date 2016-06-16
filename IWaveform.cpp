//******************************************************************************
//
//		IWaveform.cpp - waveform container able to perform basic waveform operations
//		Ivo Carvalho 27/4/2011
//
//******************************************************************************


#include "IWaveform.h"

//OBJECTLOADREGISTER(IWaveform,"$Id: IWaveform.cpp,v 1.0 27/4/2011 14:22:36 ivoc Exp $")

IWaveform::IWaveform(float * received_index_vector,float * received_data_vector, int NumberOfPoints){
	
	if( NumberOfPoints >1) {
		number_of_points = NumberOfPoints;
		index_vector = new float[NumberOfPoints];
		data_vector = new float[NumberOfPoints];
		
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
IWaveform::~IWaveform(){
	
	if(this->index_vector != NULL) delete this->index_vector;
	if(this->data_vector != NULL) delete this->data_vector;
}

float IWaveform::GetWaveformValue(float index_to_search){
	
	if (this->number_of_points == 0) return 0.;
	if (this->number_of_points == 1 && index_to_search == this->index_vector[0]) return this->data_vector[0];
	//if out of bounds
	if (this->index_vector[0] > index_to_search || this->index_vector[this->number_of_points-1] < index_to_search) return this->OutsideValueDefaultOutput;
	// search algorithm based on a regular SAR ADC model - bisection routine - numerical receips in C chapter 3.4
	this->jl = 0;
	this->ju = this->number_of_points-1;
	while (ju-jl > 1){
		this->jm = (this->ju+this->jl) >> 1;	//(shift 1 bit to the left same as divide by 2 and then cast to int)
		if (index_to_search >= this->index_vector[jm]) this->jl = this->jm;
		else this->ju = this->jm;
	}// return linear interpolation between the upper and the lower point
	return (this->data_vector[jl] + (this->data_vector[jl+1] - this->data_vector[jl])*(index_to_search - this->index_vector[jl])/(this->index_vector[jl+1] - this->index_vector[jl]));	
}

bool IWaveform::SaveWaveform(float * received_index_vector,float * received_data_vector, int NumberOfPoints){
	
	if (NumberOfPoints != number_of_points) {
	
		index_vector = new float[NumberOfPoints];
		data_vector = new float[NumberOfPoints];
		number_of_points = NumberOfPoints;
	}
	if (NumberOfPoints < 2) return False;
	
	int i;
	for (i = 0; i < NumberOfPoints ; i++ ){
		index_vector[i] = *(received_index_vector+i);
		data_vector[i] = *(received_data_vector+i);
	}
	
	if(this->SortWaveform()) return (this->RemoveRepeatedValues());
}

float IWaveform::GetFirstIndex(){

	if (this->number_of_points > 0){
		return index_vector[0];
	}
	else return (float)NULL;
}

float IWaveform::GetLastIndex(){
	
	if (this->number_of_points > 0){
		return index_vector[number_of_points-1];
	}
	else return (float)NULL;
}

/* not done yet
bool WaveformGAM::AddPoint(float point_index, float point_data){
	
	
	return True;
}

bool WaveformGAM::RemovePoint(int index_to_remove){
	
	
	return True;
}

bool WaveformGAM::GetIndexVector(float &recived_index_vector){
	
}

bool WaveformGAM::GetDataVector(float &received_data_vector){
	
	
	
}
*/
bool IWaveform::DefineOutsideValueDefaultValue(float outside_value_default_output){
	
	this->OutsideValueDefaultOutput = outside_value_default_output;
}

bool IWaveform::RemoveRepeatedValues(){
	
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

bool IWaveform::SortWaveform(){
	
	if (this->number_of_points < 1) return False;
	if (this->number_of_points == 1) return True;

	int i;
	int alfa;
	float temp;
	float min = this->index_vector[0];
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
