#ifndef _TECHNICALSIGNALSGAM_H
#define	_TECHNICALSIGNALSGAM_H

//#include <dirent.h>
#include "DDBInputInterface.h"
#include "DDBOutputInterface.h"
#include "GAM.h"
#include "HtmlStream.h"

OBJECT_DLL(TechnicalSignalsGAM)


class TechnicalSignalsGAM : public GAM, public HttpInterface {
private:

	DDBInputInterface *SignalsInputInterface;
	DDBOutputInterface *SignalsOutputInterface;

	struct InputInterfaceStruct {
        float ADC_horizontal_current;
		float ADC_vertical_current;
        float ADC_primary_current;
		float ADC_vloop;
		float HAlfaOutput;
		int usectime;
	};
	struct OutputInterfaceStruct {
		float HorizontalCurrent;
		float VerticalCurrent;
		float PrimaryCurrent;
		float IronCoreSaturation;
		float VLoop;
		float HAlfa;
	};

    bool calculate_h_alpha_bool;
	int n_samples;
	float accumulator_1;
	float accumulator_2;
	float accumulator_3;
	float accumulator_4;
	float remove_offset_1;
	float remove_offset_2;
	float remove_offset_3;
	float remove_offset_4;
	int usectime_to_wait_for_starting_operation;
	
	float *old_v_loop_value_storage;
	float filtered_v_loop;
	int number_of_points_for_average;

	int i;

	bool view_input_variables;

public:

	// Default constructor
	TechnicalSignalsGAM();

	// Destructor
	virtual ~TechnicalSignalsGAM();

	// Initialise the module
	virtual bool Initialise(ConfigurationDataBase& cdbData);

	// Execute the module functionalities
	virtual bool Execute(GAM_FunctionNumbers functionNumber);

	virtual bool ProcessHttpMessage(HttpStream &hStream);

	OBJECT_DLL_STUFF(TechnicalSignalsGAM)
};



#endif	
