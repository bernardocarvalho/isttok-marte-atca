#ifndef _SPECTROSCOPYTRIGGERGAM_H
#define _SPECTROSCOPYTRIGGERGAM_H

//#include <dirent.h>
#include "DDBInputInterface.h"
#include "DDBOutputInterface.h"
#include "GAM.h"
// #include "HtmlStream.h"

OBJECT_DLL(SpectroscopyTriggerGAM)


class SpectroscopyTriggerGAM : public GAM {
private:

	DDBInputInterface *SignalsInputInterface;
	DDBOutputInterface *SignalsOutputInterface;

	struct InputInterfaceStruct {
		int DischargeStatus;
		int usecDischargeTime;
		int usecTime;
	};
	struct OutputInterfaceStruct {
		float SpectroscopyTrigger;
	};
	int old_elapsedtime;
	int min_time_between_triggers_usec;	

public:

	// Default constructor
	SpectroscopyTriggerGAM();

	// Destructor
	virtual ~SpectroscopyTriggerGAM();

	// Initialise the module
	virtual bool Initialise(ConfigurationDataBase& cdbData);

	// Execute the module functionalities
	virtual bool Execute(GAM_FunctionNumbers functionNumber);

	OBJECT_DLL_STUFF(SpectroscopyTriggerGAM)
};


#endif

