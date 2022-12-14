#ifndef _HALPHAGAM_H
#define	_HALPHAGAM_H

//#include <dirent.h>
#include "DDBInputInterface.h"
#include "DDBOutputInterface.h"
#include "GAM.h"
#include "HtmlStream.h"

OBJECT_DLL(HAlphaGAM)


class HAlphaGAM : public GAM, public HttpInterface {
private:
	DDBInputInterface *SignalsInputInterface;
	DDBOutputInterface *SignalsOutputInterface;

	struct InputInterfaceStruct {
		float ADC_H_alpha;
		int usectime;
	};
	struct OutputInterfaceStruct {
		float HAlfaOutput;
	};

	int n_samples;
	float accumulator;
	float remove_offset;
	int usectime_to_wait_for_starting_operation;
	
	bool view_input_variables;

public:

	// Default constructor
	HAlphaGAM();

	// Destructor
	virtual ~HAlphaGAM();

	// Initialise the module
	virtual bool Initialise(ConfigurationDataBase& cdbData);

	// Execute the module functionalities
	virtual bool Execute(GAM_FunctionNumbers functionNumber);

	virtual bool ProcessHttpMessage(HttpStream &hStream);

	OBJECT_DLL_STUFF(HAlphaGAM)
};



#endif	
