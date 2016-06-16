/*
 * File:   LookupTable.h
 * Author: ivoc, ipfn
 *
 * Created on August 26, 2010
 * last modified on August 26, 2010
 * version: 0.1
 */

#ifndef _MACHINEPROTECTIONGAM_H
#define	_MACHINEPROTECTIONGAM_H

//#include <dirent.h>
#include "DDBInputInterface.h"
#include "DDBOutputInterface.h"
#include "GAM.h"
#include "HtmlStream.h"

OBJECT_DLL(MachineProtectionGAM)


class MachineProtectionGAM : public GAM, public HttpInterface {
private:

	DDBInputInterface *SignalsInputInterface;
	DDBOutputInterface *SignalsOutputInterface;

	struct InputInterfaceStruct {
		float PlasmaCurrent;
		float HorizontalCurrent;
		float VerticalCurrent;
		float PrimaryCurrent;
		float IronCoreSaturation;
		float VLoop;
		float Density;
		float HAlfa;
		int32 usecTime;
		int32 SoftStopPrimaryPS;
		int32 SoftStopVerticalPS;
		int32 SoftStopHorizontalPS;
	};
	struct OutputInterfaceStruct {
		int32 InSaturation;
		int32 SlowStopBool;
		int32 HardStopBool;
	};

	float calculated_saturation_value;
	float iron_core_saturation_value;
	float iron_core_dangerous_value;
	
	float integration_accumulator;
	
	int assert_saturation_limit1;
	int assert_saturation_limit2;
	int assert_number_of_samples_in_saturation_1;
	int assert_number_of_samples_in_saturation_2;
	
		
	bool view_input_variables;


public:

	// Default constructor
	MachineProtectionGAM();

	// Destructor
	virtual ~MachineProtectionGAM();

	// Initialise the module
	virtual bool Initialise(ConfigurationDataBase& cdbData);

	// Execute the module functionalities
	virtual bool Execute(GAM_FunctionNumbers functionNumber);

	virtual bool ProcessHttpMessage(HttpStream &hStream);

	OBJECT_DLL_STUFF(MachineProtectionGAM)
};



#endif	/* _LOOKUPTABLE_H */

