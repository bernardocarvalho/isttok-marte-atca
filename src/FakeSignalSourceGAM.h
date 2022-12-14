/********************************************************************************
* FakeSignalSourceGAM developed for ISTTOK ATCA Slave Board Emulation		*
* FakeSignalSourceGAM.h								*
*										*
* SVN keywords									*
* $Date:  $									*
* $Revision: 0.2 $								*
* $URL:  $									*
*										*
********************************************************************************/

/****************************************************************
*								*
* Copyright 2011 EFDA | European Fusion Development Agreement	*
*								*
* Licensed under the EUPL, Version 1.1 or - as soon they	*
* will be approved by the European Commission - subsequent	*
* versions of the EUPL (the "Licence"); 			*
* You may not use this work except in compliance with the	*
* Licence.							*
* You may obtain a copy of the Licence at:			*
*								*
* http://ec.europa.eu/idabc/eupl				*
*								*
* Unless required by applicable law or agreed to in		*
* writing, software distributed under the Licence is		*
* distributed on an "AS IS" basis,				*
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either		*
* express or implied.						*
* See the Licence for the specific language governing		*
* permissions and limitations under the Licence.		*
*								*
* $Id: FakeSignalSourceGAM.cpp $				*
*								*
****************************************************************/

/************************************************
* IPFN-IST PDL 2019				*
* plourenco@ipfn.tecnico.ulisboa.pt		*
* N. B.: Nothing to comment			*
************************************************/

#ifndef _FAKESIGNALSOURCEGAM_H
#define _FAKESIGNALSOURCEGAM_H

#include "DDBInputInterface.h"
#include "DDBOutputInterface.h"
#include "GAM.h"
#include "HttpInterface.h"

OBJECT_DLL(FakeSignalSourceGAM)
class FakeSignalSourceGAM : public GAM, public HttpInterface {
OBJECT_DLL_STUFF(FakeSignalSourceGAM)

private:
	/** Input interface to read data from */
	DDBInputInterface			*SignalsInputInterface;

	/** Output interface to write data to */
	DDBOutputInterface 			*SignalsOutputInterface;

	/** Private Variables */
	FString					signalType;
	float					signalValueFloat;
	int32					signalValueInt32;
	uint32					signalValueUint32;
	size_t					nBytes;
	float					*arrayDataFloat;
	int32					*arrayDataInt32;
	uint32					*arrayDataUint32;
	uint32					signalNumber;

public:

	/** Constructor */
	FakeSignalSourceGAM(){
		this->SignalsInputInterface 	= NULL;
		this->SignalsOutputInterface	= NULL;

		signalType.SetSize(0);
		signalValueFloat		= 0.0;
		signalValueInt32		= 0;
		signalValueUint32		= 0;
		nBytes				= 0;
		arrayDataFloat			= NULL;
		arrayDataInt32			= NULL;
		arrayDataUint32			= NULL;
		signalNumber			= 0;
	}

	/** Deconstructor */
	virtual ~FakeSignalSourceGAM(){
		//Clear all allocated memory
		/*if(arrayDataFloat != NULL){
			free((void *&)arrayDataFloat);
		}
		if(arrayDataInt32 != NULL){
			free((void *&)arrayDataInt32);
		}
		if(arrayDataUint32 != NULL){
			free((void *&)arrayDataUint32);
		}*/

		//Clear IO Interface pointers
		/*this->SignalsInputInterface = NULL;
		this->SignalsOutputInterface = NULL;*/
	}

	/** Initialise the module */
	virtual bool Initialise(ConfigurationDataBase& cdbData);

	/** Execute the module functionalities */
	virtual bool Execute(GAM_FunctionNumbers functionNumber);

	/** Saves parameetrs to CDB */
	virtual bool ObjectSaveSetup(ConfigurationDataBase &info, StreamInterface *err){ return True; };

	/** HTTP interface process message*/
	virtual bool ProcessHttpMessage(HttpStream &hStream);

};

#endif  /* _FAKESIGNALSOURCEGAM_H */

