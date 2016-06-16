#ifndef _SINEPROBEGAM_H
#define	_SINEPROBEGAM_H

//#include <dirent.h>
#include "DDBInputInterface.h"
#include "DDBOutputInterface.h"
#include "GAM.h"
#include "HtmlStream.h"

OBJECT_DLL(SineProbeGAM)


class SineProbeGAM : public GAM, public HttpInterface {
private:
	DDBInputInterface *SignalsInputInterface;
	DDBOutputInterface *SignalsOutputInterface;

	struct InputInterfaceStruct {
		float ADC_sine_probe;
		int usectime;
	};
	struct OutputInterfaceStruct {
		float SineProbeZ;
	};

	bool sine_vertical_bool;
	int n_samples;
	float accumulator;
	float remove_offset;
	int usectime_to_wait_for_starting_operation;
	
	bool view_input_variables;

public:

	// Default constructor
	SineProbeGAM();

	// Destructor
	virtual ~SineProbeGAM();

	// Initialise the module
	virtual bool Initialise(ConfigurationDataBase& cdbData);

	// Execute the module functionalities
	virtual bool Execute(GAM_FunctionNumbers functionNumber);

	virtual bool ProcessHttpMessage(HttpStream &hStream);

	OBJECT_DLL_STUFF(SineProbeGAM)
};



#endif	

