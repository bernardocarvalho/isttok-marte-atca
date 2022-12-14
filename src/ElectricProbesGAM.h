#ifndef _ELECTRICPROBESGAM_H
#define	_ELECTRICPROBESGAM_H

//#include <dirent.h>
#include "DDBInputInterface.h"
#include "DDBOutputInterface.h"
#include "GAM.h"
#include "HtmlStream.h"

OBJECT_DLL(ElectricProbesGAM)


class ElectricProbesGAM : public GAM, public HttpInterface {
private:

	DDBInputInterface *SignalsInputInterface;
	DDBOutputInterface *SignalsOutputInterface;

	struct InputInterfaceStruct {
		float ADC_electric_top_near;
		float ADC_electric_inner_near;
		float ADC_electric_outer_near;
		float ADC_electric_bottom_near;
		int usectime;
	};
	struct OutputInterfaceStruct {
		float ElectricProbesR;
		float ElectricProbesZ;
	};

    int electric_radial_bool;
    int electric_vertical_bool;
	int usectime_to_wait_for_starting_operation;

	int n_samples;
	float accumulator_1;
	float accumulator_2;
	float accumulator_3;
	float accumulator_4;
	float remove_offset_1;
	float remove_offset_2;
	float remove_offset_3;
	float remove_offset_4;
	
	float out_of_bounds_limit;
	float distance;
	float RadialConstant;
	float VerticalConstant;
	bool view_input_variables;

public:

	// Default constructor
	ElectricProbesGAM();

	// Destructor
	virtual ~ElectricProbesGAM();

	// Initialise the module
	virtual bool Initialise(ConfigurationDataBase& cdbData);

	// Execute the module functionalities
	virtual bool Execute(GAM_FunctionNumbers functionNumber);

	virtual bool ProcessHttpMessage(HttpStream &hStream);

	OBJECT_DLL_STUFF(ElectricProbesGAM)
};



#endif

