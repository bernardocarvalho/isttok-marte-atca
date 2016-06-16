#ifndef _MAINROGOWSKIGAM_H
#define	_MAINROGOWSKIGAM_H

//#include <dirent.h>
#include "DDBInputInterface.h"
#include "DDBOutputInterface.h"
#include "GAM.h"
#include "HtmlStream.h"

OBJECT_DLL(MainRogowskiGAM)


class MainRogowskiGAM : public GAM, public HttpInterface {
private:
	DDBInputInterface *SignalsInputInterface;
	DDBOutputInterface *SignalsOutputInterface;

	struct InputInterfaceStruct {
		float ADC_main_rogowski;
		int usectime;
	};
	struct OutputInterfaceStruct {
		float RogowskiPlasmaCurrent;
	};

    int n_samples;
	float remove_offset;
	float accumulator;
	int usectime_to_wait_for_starting_operation;
 
public:

	// Default constructor
	MainRogowskiGAM();

	// Destructor
	virtual ~MainRogowskiGAM();

	// Initialise the module
	virtual bool Initialise(ConfigurationDataBase& cdbData);

	// Execute the module functionalities
	virtual bool Execute(GAM_FunctionNumbers functionNumber);

	virtual bool ProcessHttpMessage(HttpStream &hStream);

	OBJECT_DLL_STUFF(MainRogowskiGAM)
};



#endif	

