/*
 * Copyright 2011 EFDA | European Fusion Development Agreement
 *
 * Licensed under the EUPL, Version 1.1 or - as soon they 
   will be approved by the European Commission - subsequent  
   versions of the EUPL (the "Licence"); 
 * You may not use this work except in compliance with the 
   Licence. 
 * You may obtain a copy of the Licence at: 
 *  
 * http://ec.europa.eu/idabc/eupl
 *
 * Unless required by applicable law or agreed to in 
   writing, software distributed under the Licence is 
   distributed on an "AS IS" basis, 
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either 
   express or implied. 
 * See the Licence for the specific language governing 
   permissions and limitations under the Licence. 
 *
 * $Id$
 *
**/

#include "System.h"
#include "MessageHandler.h"
#include "GCNamedObject.h"
#include "GlobalObjectDataBase.h"
#include "Console.h"
#include "LoggerService.h"
#include "ATCAIocIntDrv.h"

#include "ConfigurationDataBase.h"
#include "CDBExtended.h"
#include "File.h"
#include "MenuContainer.h"
#include "StateMachine.h"
#include "RealTimeThread.h"

const char *menuMap=
"Title = \"TEST PROGRAM\" \n"
"Add={\n"
"   MENU1={\n"
"       Class = MenuEntry\n"
"       Title = \"Start\"\n"
"   }\n"
"   MENU2={\n"
"       Class = MenuEntry\n"
"       Title = \"Stop\"\n"
"   }\n"
"}\n"
;

char remoteLoggerAddress[] = "127.0.0.1";
bool _keepRunning;

int atca_thread(void* atca){
	ATCAIocIntDrv* atcadrv = (ATCAIocIntDrv*) atca;
	char* buffer = (char*)malloc((atcadrv->NumberOfInputs())*4);
	if(buffer==NULL){
		printf("malloc null %d \n",(atcadrv->NumberOfInputs())*4);
		return 0;
	}

 	while(_keepRunning) {
 		atcadrv->GetData(0, (int32*)buffer);
 	}
    return 0;
}

bool Start(StreamInterface &in,StreamInterface &out,void *userData){

	LoadableLibrary ll;
	if(!ll.Open("ATCAIocIntDrv.gam")){
		printf("can't load gam\n");
		return False;
	}
	
	// this flick the ncurses window 
	Object *o = OBJObjectCreateByName("ATCAIocIntDrv");
	if(o == NULL){
		printf("can't create obj\n");
		return False;
	}

	ATCAIocIntDrv* atcadrv = dynamic_cast<ATCAIocIntDrv*>(o);
	if(atcadrv == NULL){
		printf("can't create ATCAIocInt drv object\n");
		return False;
	}
	File config;
	if(!config.OpenRead("CDB.cfg")){
		printf("can't find CDB.cfg\n");
		//delete atcadrv;
		return False;		
	}
	ConfigurationDataBase cdb;
	if(!cdb->ReadFromStream(config)){
		printf("can't load CDB.cfg\n");
		//delete atcadrv;
		return False;				
	}
	if(!(atcadrv->ObjectLoadSetup(cdb, NULL))){
		printf("can't obj load setup CDB.cfg\n");
		//delete atcadrv;
		return False;
	}
	_keepRunning = True;
	Threads::BeginThread((ThreadFunctionType)atca_thread, atcadrv);

    return True;
}

bool Stop(StreamInterface &in,StreamInterface &out,void *userData){
	_keepRunning =False;
    return True;
}

int main(int argc, char *argv[]){
    LSSetUserAssembleErrorMessageFunction(LSAssembleErrorMessage);
    LSSetRemoteLogger(remoteLoggerAddress,32767);
    LSStartService();
    
    //SleepSec(5.0);
    	
	GCRTemplate<MenuContainer> mc(GCFT_Create);
	Console con;
	mc->Init(menuMap,&con );

	mc->SetupItem("MENU1",Start,NULL,NULL,NULL);
	mc->SetupItem("MENU2",Stop ,NULL,NULL,NULL);

	mc->TextMenu(con,con);

    return 1;
}
