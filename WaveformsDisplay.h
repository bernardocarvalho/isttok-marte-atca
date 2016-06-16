/**
 * @file allows to configure ISTTOK and upload configuration file to MARTe
 */
#ifndef WAVEFORMSDISPLAY_H
#define WAVEFORMSDISPLAY_H

#include "Level0.h"
#include "Level1.h"
#include "Level2.h"
#include "Level3.h"

#include "System.h"
#include "HttpStream.h"
#include "HttpInterface.h"
#include "HtmlStream.h"
#include "FString.h"
#include "GCReferenceContainer.h"
#include "CDBExtended.h"
#include "GlobalObjectDataBase.h"
#include "MessageHandler.h"


OBJECT_DLL(WaveformsDisplay)
class WaveformsDisplay: public GCReferenceContainer, public HttpInterface, public MessageHandler{
OBJECT_DLL_STUFF(WaveformsDisplay)

private:
/** The id of the configuration file entry as received from the http request*/
    FString configFileID;

/** The location of MARTe*/
    FString marteLocation;

	bool initialised;
	
    FString cdbString; //string to be uploaded
	
	int screen_resolution_x;
	int screen_resolution_y;

	FString header_colour;
	FString diagnostics_parameters_colour;
	FString control_parameters_colour;
	FString time_windows_colour;
	FString edit_colour;
	FString footer_colour;

	FString default_config_file_path;
	FString config_files_directory;
	FString primary_current_directory;
	FString primary_plasma_current_directory;
	FString primary_scenarios_directory;
	FString vertical_current_directory;
	FString vertical_position_directory;
	FString vertical_scenarios_directory;
	FString horizontal_current_directory;
	FString horizontal_position_directory;
	FString horizontal_scenarios_directory;


public:

    /** the main entry point for HttpInterface */
    virtual     bool    ProcessHttpMessage(HttpStream &hStream);

    /** the default constructor */
    WaveformsDisplay(){
    }

    ~WaveformsDisplay(){
    }

	virtual bool ObjectLoadSetup(ConfigurationDataBase & info, StreamInterface * err);

    /** save an object content into a set of configs */
    virtual bool ObjectSaveSetup( ConfigurationDataBase & info, StreamInterface * err){

        GCReferenceContainer::ObjectSaveSetup(info,err);
        return HttpInterface::ObjectSaveSetup(info,err);
    }

 
    
private:
    /**Utility method to print the form*/
	bool Initialise();
 
	bool PrintHTTPForm(HtmlStream &hmStream);
	
	bool EditWaveform(HtmlStream &hmStream);
	
	bool WriteConfigurationFileWithChanges(char BaseFileFilePath[], char TargetFilePath[]);
	
	bool ReadConfigurationFile(char FilePath[]);
	
};
#endif
