#ifndef _WAVEFORMGAM_H
#define	_WAVEFORMGAM_H

//#include <dirent.h>
#include "DDBInputInterface.h"
#include "DDBOutputInterface.h"
#include "GAM.h"
#include "HtmlStream.h"
#include "IWaveform.h"

OBJECT_DLL(WaveformGAM)


class WaveformGAM : public GAM, public HttpInterface {
private:
	DDBInputInterface *SignalsInputInterface;
	DDBOutputInterface *SignalsOutputInterface;

	struct InputInterfaceStruct {
		int32 usecDischargeTime;
		int32 WaveformMode;
		int32 PlasmaDirection;
		int32 DischargeStatus;
	};
	struct OutputInterfaceStruct {
		float WaveformOutput;
	};

	IWaveform *waveform_1_p;
	IWaveform *waveform_2_p;
	IWaveform *waveform_breakdown;
	IWaveform *waveform_breakdown_negative;
	IWaveform *waveform_1_n;
	IWaveform *waveform_2_n;
	IWaveform *waveform_inversion_positive_to_negative;
	IWaveform *waveform_inversion_negative_to_positive;

	int waveform_1_p_vector_size;
	float *waveform_1_p_index_vector;
	float *waveform_1_p_data_vector;
	float waveform_1_p_max_value;
	float waveform_1_p_min_value;
	bool waveform_1_p_available;
//		waveform_mode_1_negative  
	int waveform_1_n_vector_size;
	float *waveform_1_n_index_vector;
	float *waveform_1_n_data_vector;
	bool waveform_1_n_available;
//		waveform_mode_2_positive 
	int waveform_2_p_vector_size;
	float *waveform_2_p_index_vector;
	float *waveform_2_p_data_vector;
	float waveform_2_p_max_value;
	float waveform_2_p_min_value;
	bool waveform_2_p_available;
//		waveform_mode_2_negative 
	int waveform_2_n_vector_size;
	float *waveform_2_n_index_vector;
	float *waveform_2_n_data_vector;
	bool waveform_2_n_available;
//		waveform_breakdown 
	int waveform_breakdown_vector_size;
	float *waveform_breakdown_index_vector;
	float *waveform_breakdown_data_vector;
	bool waveform_breakdown_available;
//		waveform_breakdown_negative
	int waveform_breakdown_negative_vector_size;
	float *waveform_breakdown_negative_index_vector;
	float *waveform_breakdown_negative_data_vector;
	bool waveform_breakdown_negative_available;
//		waveform_inversion_positive_to_negative 
	int waveform_inversion_positive_to_negative_vector_size;
	float *waveform_inversion_positive_to_negative_index_vector;
	float *waveform_inversion_positive_to_negative_data_vector;
	bool waveform_inversion_positive_to_negative_available;
//		waveform_inversion_negative_to_positive 
	int waveform_inversion_negative_to_positive_vector_size;
	float *waveform_inversion_negative_to_positive_index_vector;
	float *waveform_inversion_negative_to_positive_data_vector;
	bool waveform_inversion_negative_to_positive_available;

	bool view_input_variables;

public:

	// Default constructor
	WaveformGAM();

	// Destructor
	virtual ~WaveformGAM();

	// Initialise the module
	virtual bool Initialise(ConfigurationDataBase& cdbData);

	// Execute the module functionalities
	virtual bool Execute(GAM_FunctionNumbers functionNumber);

	virtual bool ProcessHttpMessage(HttpStream &hStream);

	OBJECT_DLL_STUFF(WaveformGAM)
};



#endif	

