#ifndef _ELECTRODEBIASINGGAM_H
#define _ELECTRODEBIASINGGAM_H

//#include <dirent.h>
#include "DDBInputInterface.h"
#include "DDBOutputInterface.h"
#include "GAM.h"
// #include "HtmlStream.h"

OBJECT_DLL(ElectrodeBiasingGAM)


class ElectrodeBiasingGAM : public GAM {
private:

	DDBInputInterface *SignalsInputInterface;
	DDBOutputInterface *SignalsOutputInterface;

	struct InputInterfaceStruct {
		int DischargeStatus;
	};
	struct OutputInterfaceStruct {
		float ElectrodeBiasingTrigger;
	};
	
	//internal use	
	int dischargestatus_old; //save previous value of DischargeStatus

public:

	// Default constructor
	ElectrodeBiasingGAM();

	// Destructor
	virtual ~ElectrodeBiasingGAM();

	// Initialise the module
	virtual bool Initialise(ConfigurationDataBase& cdbData);

	// Execute the module functionalities
	virtual bool Execute(GAM_FunctionNumbers functionNumber);

	OBJECT_DLL_STUFF(ElectrodeBiasingGAM)
};


#endif

