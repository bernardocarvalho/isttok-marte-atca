/* 
 * File:   FireSignalDischargeStatusGAM.h
 * Author: ivoc
 *
 */

#ifndef __FIRESIGNALDISCHARGESTATUSGAM_H__
#define	__FIRESIGNALDISCHARGESTATUSGAM_H__

#include "GAM.h"
#include "DDBInputInterface.h"
#include "HtmlStream.h"

OBJECT_DLL(FireSignalDischargeStatusGAM)


class FireSignalDischargeStatusGAM : public GAM, public HttpInterface  {
private:

	DDBInputInterface *SignalsInputInterface;

	struct InputInterfaceStruct {
		int DischargeStatus;
		int usecTime;
	};


	bool plasmaEnded;
	bool plasmaEndedTimeMarked;
	bool PlasmaStarted;
	uint32 plasmaEndedTime;
	int i;

public:

	//
	// Default constructor
	//
	FireSignalDischargeStatusGAM();

	//
	// Destructor
	//
	virtual ~FireSignalDischargeStatusGAM();

	//
	// Initialise the module
	//
	virtual bool Initialise(ConfigurationDataBase& cdbData);

	//
	// Execute the module functionalities
	//
	virtual bool Execute(GAM_FunctionNumbers functionNumber);

	//
	// Builds the HTTP page with information about the CODAC for FireSignal to catch (used this way for compatibility with the previous version)
	//
	virtual bool ProcessHttpMessage(HttpStream &hStream);

	OBJECT_DLL_STUFF(FireSignalDischargeStatusGAM)
};

#endif	

