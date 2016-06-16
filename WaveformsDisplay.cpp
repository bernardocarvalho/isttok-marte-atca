//	Object properties
#define OBJECTNAME "Waveforms Display"
#define VERSION "1.00"
#define LASTDATE "2011-02-21"
#define AUTHOR "Ivo Carvalho - ivoc@ipfn.ist.utl.pt"
#define DESCRIPTION "Visualise all waveforms stored on one folder"

#define HEADERIMAGELINK "http://www.ipfn.ist.utl.pt/ipfnPortalLayout/themes/ipfn/_img_/logoIPFN_Topo_officialColours.png"
#define FOOTERIMAGELINK "http://backoffice.ipfn.ist.utl.pt/mgallery/default.asp?obj=1"

#include "WaveformsDisplay.h"
#include "MessageEnvelope.h"
#include "Message.h"


OBJECTLOADREGISTER(WaveformsDisplay,"$Id: WaveformsDisplay.cpp,v 1.5 2009/05/22 10:40:56 aneto Exp $")


   /**  initialise an object from a set of configs */
bool WaveformsDisplay::ObjectLoadSetup(ConfigurationDataBase & info, StreamInterface * err){

        GCReferenceContainer::ObjectLoadSetup(info,err);

        CDBExtended &cdbx = (CDBExtended &)info;

        cdbx.ReadFString(configFileID, "configFileID", "cfgFile");
/*		
	// Move to the graphics branch
	if(!cdbx->Move("graphics"))
	{
		CStaticAssertErrorCondition(InitialisationError,"WaveformsDisplay::ObjectLoadSetup: %s Could not move to \"graphics\"",this->Name());
		return False;
	}
	
		if(!cdbx.ReadInt32(screen_resolution_x, "screen_resolution_x"))
		{
			CStaticAssertErrorCondition(InitialisationError,"WaveformsDisplay::ObjectLoadSetup: %s Could not read graph_width",this->Name());
			return False;
		}
		if(!cdbx.ReadInt32(screen_resolution_y, "screen_resolution_y"))
		{
			CStaticAssertErrorCondition(InitialisationError,"WaveformsDisplay::ObjectLoadSetup: %s Could not read graph_height",this->Name());
			return False;
		}

	// Move back to the GAM main branch
	cdbx->MoveToFather();
	
//			CStaticAssertErrorCondition(InitialisationError,"WaveformsDisplay::ObjectLoadSetup: %d %d",screen_resolution_x,screen_resolution_y);
	

	// Move to the file branch
	if(!cdbx->Move("colours"))
	{
		CStaticAssertErrorCondition(InitialisationError,"WaveformsDisplay::ObjectLoadSetup: %s Could not move to \"colours\"",this->Name());
		return False;
	}
		if(!cdbx.ReadFString(header_colour, "header_colour"))
		{
			CStaticAssertErrorCondition(InitialisationError,"WaveformsDisplay::ObjectLoadSetup: %s Could not read header_colour",this->Name());
			return False;
		}
		if(!cdbx.ReadFString(diagnostics_parameters_colour, "diagnostics_parameters_colour"))
		{
			CStaticAssertErrorCondition(InitialisationError,"WaveformsDisplay::ObjectLoadSetup: %s Could not read diagnostics_parameters_colour",this->Name());
			return False;
		}
		if(!cdbx.ReadFString(control_parameters_colour, "control_parameters_colour"))
		{
			CStaticAssertErrorCondition(InitialisationError,"WaveformsDisplay::ObjectLoadSetup: %s Could not read control_parameters_colour",this->Name());
			return False;
		}
		if(!cdbx.ReadFString(time_windows_colour, "time_windows_colour"))
		{
			CStaticAssertErrorCondition(InitialisationError,"WaveformsDisplay::ObjectLoadSetup: %s Could not read time_windows_colour",this->Name());
			return False;
		}
		if(!cdbx.ReadFString(edit_colour, "edit_colour"))
		{
			CStaticAssertErrorCondition(InitialisationError,"WaveformsDisplay::ObjectLoadSetup: %s Could not read edit_colour",this->Name());
			return False;
		}
		if(!cdbx.ReadFString(footer_colour, "footer_colour"))
		{
			CStaticAssertErrorCondition(InitialisationError,"WaveformsDisplay::ObjectLoadSetup: %s Could not read footer_colour",this->Name());
			return False;
		}
	// Move back to the GAM main branch
	cdbx->MoveToFather();
	
//	CStaticAssertErrorCondition(InitialisationError,"WaveformsDisplay::ObjectLoadSetup: %s %s %s %s %s %s",header_colour.Buffer(),diagnostics_parameters_colour.Buffer(),control_parameters_colour.Buffer(),time_windows_colour.Buffer(),edit_colour.Buffer(),footer_colour.Buffer());
	
	
	if(!cdbx->Move("default_directories"))
	{
		CStaticAssertErrorCondition(InitialisationError,"WaveformsDisplay::ObjectLoadSetup: %s Could not move to \"default_directories\"",this->Name());
		return False;
	}
		if(!cdbx.ReadFString(default_config_file_path, "default_config_file_path"))
		{
			CStaticAssertErrorCondition(InitialisationError,"WaveformsDisplay::ObjectLoadSetup: %s Could not read default_config_file_path",this->Name());
			return False;
		}
		if(!cdbx.ReadFString(config_files_directory, "config_files_directory"))
		{
			CStaticAssertErrorCondition(InitialisationError,"WaveformsDisplay::ObjectLoadSetup: %s Could not read config_files_directory",this->Name());
			return False;
		}
		if(!cdbx.ReadFString(primary_current_directory, "primary_current_directory"))
		{
			CStaticAssertErrorCondition(InitialisationError,"WaveformsDisplay::ObjectLoadSetup: %s Could not read primary_current_directory",this->Name());
			return False;
		}
		if(!cdbx.ReadFString(primary_plasma_current_directory, "primary_plasma_current_directory"))
		{
			CStaticAssertErrorCondition(InitialisationError,"WaveformsDisplay::ObjectLoadSetup: %s Could not read primary_plasma_current_directory",this->Name());
			return False;
		}
		if(!cdbx.ReadFString(primary_scenarios_directory, "primary_scenarios_directory"))
		{
			CStaticAssertErrorCondition(InitialisationError,"WaveformsDisplay::ObjectLoadSetup: %s Could not read primary_scenarios_directory",this->Name());
			return False;
		}
		if(!cdbx.ReadFString(vertical_current_directory, "vertical_current_directory"))
		{
			CStaticAssertErrorCondition(InitialisationError,"WaveformsDisplay::ObjectLoadSetup: %s Could not read vertical_current_directory",this->Name());
			return False;
		}
		if(!cdbx.ReadFString(vertical_position_directory, "vertical_position_directory"))
		{
			CStaticAssertErrorCondition(InitialisationError,"WaveformsDisplay::ObjectLoadSetup: %s Could not read vertical_position_directory",this->Name());
			return False;
		}
		if(!cdbx.ReadFString(vertical_scenarios_directory, "vertical_scenarios_directory"))
		{
			CStaticAssertErrorCondition(InitialisationError,"WaveformsDisplay::ObjectLoadSetup: %s Could not read vertical_scenarios_directory",this->Name());
			return False;
		}
		if(!cdbx.ReadFString(horizontal_current_directory, "horizontal_current_directory"))
		{
			CStaticAssertErrorCondition(InitialisationError,"WaveformsDisplay::ObjectLoadSetup: %s Could not read horizontal_current_directory",this->Name());
			return False;
		}
		if(!cdbx.ReadFString(horizontal_position_directory, "horizontal_position_directory"))
		{
			CStaticAssertErrorCondition(InitialisationError,"WaveformsDisplay::ObjectLoadSetup: %s Could not read horizontal_position_directory",this->Name());
			return False;
		}
		if(!cdbx.ReadFString(horizontal_scenarios_directory, "horizontal_scenarios_directory"))
		{
			CStaticAssertErrorCondition(InitialisationError,"WaveformsDisplay::ObjectLoadSetup: %s Could not read horizontal_scenarios_directory",this->Name());
			return False;
		}
	// Move back to the GAM main branch
	cdbx->MoveToFather();

//	CStaticAssertErrorCondition(InitialisationError,"WaveformsDisplay::ObjectLoadSetup: %s %s %s %s %s %s %s %s %s %s %s", default_config_file_path.Buffer(), config_files_directory.Buffer(), primary_current_directory.Buffer(), primary_plasma_current_directory.Buffer(), primary_scenarios_directory.Buffer(), vertical_current_directory.Buffer(), vertical_position_directory.Buffer(), vertical_scenarios_directory.Buffer(), horizontal_current_directory.Buffer(), horizontal_position_directory.Buffer(), horizontal_scenarios_directory.Buffer());
		
	this->ReadConfigurationFile((char *)default_config_file_path.Buffer());	
		
		
		
		
		
	cdbx->MoveToRoot();	
	if(!cdbx->Move("+MARTe.+ISTTOK_RTTh.+DDB"))
	{
		CStaticAssertErrorCondition(InitialisationError,"WaveformsDisplay::ObjectLoadSetup: %s Could not move to \"+MARTe.+ISTTOK_RTTh.+waveform_primary\"",this->Name());
		return False;
	}
		
*/		
		
		

	if(!cdbx.ReadFString(marteLocation, "MARTeLocation", "MARTe")){
		AssertErrorCondition(Warning, "ObjectLoadSetup::MARTe location wasn not specified. Using default: %s", marteLocation.Buffer());
	}

	return HttpInterface::ObjectLoadSetup(info,err);
}
	
bool WaveformsDisplay::PrintHTTPForm(HtmlStream &hmStream){
    hmStream.SSPrintf(HtmlTagStreamMode, "form enctype=\"multipart/form-data\" method=\"post\"");
    hmStream.SSPrintf(HtmlTagStreamMode, "p");
    hmStream.SSPrintf(HtmlTagStreamMode, "input type=\"checkbox\" name=\"displayCDB\" CHECKED");
    hmStream.Printf("Display CDB");
    hmStream.SSPrintf(HtmlTagStreamMode, "/p");
    hmStream.SSPrintf(HtmlTagStreamMode, "p");
    hmStream.SSPrintf(HtmlTagStreamMode, "input type=\"checkbox\" name=\"waitReply\"");
    hmStream.Printf("Wait reply");
    hmStream.SSPrintf(HtmlTagStreamMode, "/p");
    hmStream.SSPrintf(HtmlTagStreamMode, "p");
    hmStream.SSPrintf(HtmlTagStreamMode, "p");
    hmStream.SSPrintf(HtmlTagStreamMode, "input type=\"checkbox\" name=\"rebuildAll\"");
    hmStream.Printf("Rebuild All");
    hmStream.SSPrintf(HtmlTagStreamMode, "/p");
    hmStream.SSPrintf(HtmlTagStreamMode, "p");
    hmStream.Printf("Configuration file location");
    hmStream.SSPrintf(HtmlTagStreamMode, "br");
    hmStream.SSPrintf(HtmlTagStreamMode, "input type=\"file\" name=\"cfgFile\" size=\"40\"");
    hmStream.SSPrintf(HtmlTagStreamMode, "/p");
    hmStream.SSPrintf(HtmlTagStreamMode, "/div");
    hmStream.SSPrintf(HtmlTagStreamMode, "input type=\"submit\" value=\"Send\"");
    hmStream.SSPrintf(HtmlTagStreamMode, "/form");
    return True;
}

bool WaveformsDisplay::ProcessHttpMessage(HttpStream &hStream){    
    HtmlStream hmStream(hStream);

/*	FString temp_string_1;
	
	HeaderSVG(&temp_string_1,(char *) OBJECTNAME, 90, 500, 0,422,87,(char *) HEADERIMAGELINK,10,52, (char *) this->Name());
	hmStream.SSPrintf(HtmlTagStreamMode, temp_string_1.Buffer());



// !!!! FIRST BLOCK - DIAGNOSTICS
	
	hmStream.SSPrintf(HtmlTagStreamMode, "table bgcolor=\"Wheat\" width=\"100&#37;\"");
	hmStream.SSPrintf(HtmlTagStreamMode, "td");
	hmStream.SSPrintf(HtmlTagStreamMode, "h2 style=\"text-align:center\">Diagnostics Settings</h2><br><br><br><br><br><br><br");


	hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"top: 150;left: 20;position: absolute;z-index: 1;visibility: show;\">Horizontal position diagnostics:</div");
	hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"top: 150;left: 420;position: absolute;z-index: 1;visibility: show;\">Vertical position diagnostics:</div");
	hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"top: 150;left: 820;position: absolute;z-index: 1;visibility: show;\">Plasma current diagnostics:</div");

//	<div style="top: 390;left: 300;position: absolute;z-index: 1;visibility: show;"><form><input type="checkbox" name="vehicle" value="Bike" />magnetic<br><form><input type="checkbox" name="vehicle" value="Bike" />magnetic2</form></div>

	hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"top: 170;left: 20;position: absolute;z-index: 1;visibility: show;\"><form enctype=\"multipart/form-data\" method=\"post\"><select name=\"dropdownmenu\"><option value=1>waveform1</option><option value=2>\"barco = 2\"</option><option value=3>\"carro = 3\"</option><input type=\"submit\" value=\"V\"></form></div");
	hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"top: 190;left: 20;position: absolute;z-index: 1;visibility: show;\"><form enctype=\"multipart/form-data\" method=\"post\"><select name=\"dropdownmenu\"><option value=1>waveform1</option><option value=2>\"barco = 2\"</option><option value=3>\"carro = 3\"</option><input type=\"submit\" value=\"V\"></form></div");
	hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"top: 210;left: 20;position: absolute;z-index: 1;visibility: show;\"><form enctype=\"multipart/form-data\" method=\"post\"><select name=\"dropdownmenu\"><option value=1>waveform1</option><option value=2>\"barco = 2\"</option><option value=3>\"carro = 3\"</option><input type=\"submit\" value=\"V\"></form></div");
	hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"top: 230;left: 20;position: absolute;z-index: 1;visibility: show;\"><form enctype=\"multipart/form-data\" method=\"post\"><select name=\"dropdownmenu\"><option value=1>waveform1</option><option value=2>\"barco = 2\"</option><option value=3>\"carro = 3\"</option><input type=\"submit\" value=\"V\"></form></div");
	hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"top: 250;left: 20;position: absolute;z-index: 1;visibility: show;\"><form enctype=\"multipart/form-data\" method=\"post\"><select name=\"dropdownmenu\"><option value=1>waveform1</option><option value=2>\"barco = 2\"</option><option value=3>\"carro = 3\"</option><input type=\"submit\" value=\"V\"></form></div");

	hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"top: 170;left: 420;position: absolute;z-index: 1;visibility: show;\"><form enctype=\"multipart/form-data\" method=\"post\"><select name=\"dropdownmenu\"><option value=1>waveform1</option><option value=2>\"barco = 2\"</option><option value=3>\"carro = 3\"</option><input type=\"submit\" value=\"V\"></form></div");
	hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"top: 190;left: 420;position: absolute;z-index: 1;visibility: show;\"><form enctype=\"multipart/form-data\" method=\"post\"><select name=\"dropdownmenu\"><option value=1>waveform1</option><option value=2>\"barco = 2\"</option><option value=3>\"carro = 3\"</option><input type=\"submit\" value=\"V\"></form></div");
	hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"top: 210;left: 420;position: absolute;z-index: 1;visibility: show;\"><form enctype=\"multipart/form-data\" method=\"post\"><select name=\"dropdownmenu\"><option value=1>waveform1</option><option value=2>\"barco = 2\"</option><option value=3>\"carro = 3\"</option><input type=\"submit\" value=\"V\"></form></div");
	hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"top: 230;left: 420;position: absolute;z-index: 1;visibility: show;\"><form enctype=\"multipart/form-data\" method=\"post\"><select name=\"dropdownmenu\"><option value=1>waveform1</option><option value=2>\"barco = 2\"</option><option value=3>\"carro = 3\"</option><input type=\"submit\" value=\"V\"></form></div");

	hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"top: 170;left: 820;position: absolute;z-index: 1;visibility: show;\"><form enctype=\"multipart/form-data\" method=\"post\"><select name=\"dropdownmenu\"><option value=1>waveform1</option><option value=2>\"barco = 2\"</option><option value=3>\"carro = 3\"</option><input type=\"submit\" value=\"V\"></form></div");
	hmStream.SSPrintf(HtmlTagStreamMode, "div style=\"top: 190;left: 820;position: absolute;z-index: 1;visibility: show;\"><form enctype=\"multipart/form-data\" method=\"post\"><select name=\"dropdownmenu\"><option value=1>waveform1</option><option value=2>\"barco = 2\"</option><option value=3>\"carro = 3\"</option><input type=\"submit\" value=\"V\"></form></div");






	hmStream.SSPrintf(HtmlTagStreamMode, "/td");
	hmStream.SSPrintf(HtmlTagStreamMode, "/table");
	
	
// !!!! SECOND BLOCK - CONTROLLER SETTINGS
	
	hmStream.SSPrintf(HtmlTagStreamMode, "table bgcolor=\"MistyRose\" width=\"100&#37;\"");
	hmStream.SSPrintf(HtmlTagStreamMode, "td");
	hmStream.SSPrintf(HtmlTagStreamMode, "h2 style=\"text-align:center\">Controller Settings</h2><br><br><br><br><br><br");

	











	hmStream.SSPrintf(HtmlTagStreamMode, "/td");
	hmStream.SSPrintf(HtmlTagStreamMode, "/table");

// !!!! THIRD BLOCK - TIME WINDOWS
	hmStream.SSPrintf(HtmlTagStreamMode, "table bgcolor=\"LightSkyBlue\" width=\"100&#37;\"");
	hmStream.SSPrintf(HtmlTagStreamMode, "td");
	hmStream.SSPrintf(HtmlTagStreamMode, "h2 style=\"text-align:center\">Time Windows</h2><br><br><br><br><br><br><br><br><br><br><br><br><br><br><br><br><br><br><br><br><br");

	











	hmStream.SSPrintf(HtmlTagStreamMode, "/td");
	hmStream.SSPrintf(HtmlTagStreamMode, "/table");



	//Design the Footer with the GAM description
	FooterSVG(&temp_string_1, 133, 0, 0, 283, 133,(char *) FOOTERIMAGELINK, 6, 100, 200,(char *) OBJECTNAME,(char *) VERSION,(char *) LASTDATE,(char *) AUTHOR,(char *) DESCRIPTION);
	hmStream.SSPrintf(HtmlTagStreamMode, temp_string_1.Buffer());

*/
    FString cfgFile;
    cfgFile.SetSize(0);
    if (hStream.Switch("InputCommands.cfgFile")){
        hStream.Seek(0);
        hStream.GetToken(cfgFile, "");
        hStream.Switch((uint32)0);
    }

    FString displayCDB;
    displayCDB.SetSize(0);
    if (hStream.Switch("InputCommands.displayCDB")){
        hStream.Seek(0);
        hStream.GetToken(displayCDB, "");
        hStream.Switch((uint32)0);
    }

    FString waitReply;
    waitReply.SetSize(0);
    if (hStream.Switch("InputCommands.waitReply")){
        hStream.Seek(0);
        hStream.GetToken(waitReply, "");
        hStream.Switch((uint32)0);
    }

    FString rebuildAll;
    rebuildAll.SetSize(0);
    if (hStream.Switch("InputCommands.rebuildAll")){
        hStream.Seek(0);
        hStream.GetToken(rebuildAll, "");
        hStream.Switch((uint32)0);
    }    

    if(cfgFile.Size() > 0){        
        //Try to upload the file to MARTe
        GCRTemplate<MessageEnvelope> envelope(GCFT_Create);
        GCRTemplate<Message>         message(GCFT_Create);                
        message->Init(0, "ChangeConfigFile");
        
	/*GCNamedObject level1Sender;
        level1Sender.SetObjectName("LEVEL1");
        envelope->SetSender(level1Sender);*/

        //Insert the cdb
        ConfigurationDataBase level1CDB;
        cfgFile.Seek(0);
        level1CDB->ReadFromStream(cfgFile);
        if(rebuildAll.Size() > 0){
            FString value = "True";
            FString key   = "RebuildAll";
            level1CDB->MoveToRoot();
            CDBExtended cdbe(level1CDB);
            cdbe.WriteFString(value, key.Buffer());
        }        
        if(displayCDB.Size() > 0){
            hmStream.SSPrintf(HtmlTagStreamMode, "h2");
            hmStream.Printf("Going to upload the following CDB");
            hmStream.SSPrintf(HtmlTagStreamMode, "/h2");            
            level1CDB->WriteToStream(hmStream);
        }

        message->Insert(level1CDB);
        //Send the message
        if(waitReply.Size() > 0){
            envelope->PrepareMessageEnvelope(message, marteLocation.Buffer(), MDRF_ManualReply, this);
        }
        else{
            envelope->PrepareMessageEnvelope(message, marteLocation.Buffer());
        }

        GCRTemplate<MessageEnvelope>   reply;
        //prepare the reply
        if(waitReply.Size() > 0){
            SendMessageAndWait(envelope, reply, TimeoutType(10000));
            if(!reply.IsValid()){
                AssertErrorCondition(Warning, "Reply from %s isn't valid!", marteLocation.Buffer());
                hmStream.SSPrintf(HtmlTagStreamMode, "h1");
                hmStream.Printf("Reply from %s isn't valid!\n", marteLocation.Buffer());
                hmStream.SSPrintf(HtmlTagStreamMode, "/h1");
            }
            else{
                GCRTemplate<Message> replyMessage = reply->GetMessage();
                if(!replyMessage.IsValid()){
                    AssertErrorCondition(Warning, "The reply message from %s is not valid", marteLocation.Buffer());
                }
                else{
                    hmStream.SSPrintf(HtmlTagStreamMode, "h1");
                    hmStream.Printf("%s replied: %s", marteLocation.Buffer(), replyMessage->Content());
                    hmStream.SSPrintf(HtmlTagStreamMode, "/h1");
                }
            }
        }
        else{
            SendMessage(envelope);
            hmStream.SSPrintf(HtmlTagStreamMode, "h2");
            hmStream.Printf("File sent!");
			hmStream.SSPrintf(HtmlTagStreamMode, "/h2");
        }
        hStream.Printf("<a href=\".\">BACK</a><br>");
    }
    else{
        PrintHTTPForm(hmStream);
    }
    hmStream.SSPrintf(HtmlTagStreamMode, "/body");
    hmStream.SSPrintf(HtmlTagStreamMode, "/html");


    hStream.SSPrintf("OutputHttpOtions.Content-Type","text/html");
    hStream.WriteReplyHeader(True);
    return True;
}

bool WaveformsDisplay::EditWaveform(HtmlStream &hmStream){
	


 
	
	return True;
}

bool WaveformsDisplay::ReadConfigurationFile(char FilePath[]){
	
//	CStaticAssertErrorCondition(InitialisationError,"WaveformsDisplay::ObjectLoadSetup: FilePath = %s",FilePath);
	File temp_file;
	FString temp_fstring;

/*	ConfigurationDataBase cdb;

   if(!temp_file.OpenRead(FilePath)){
        CStaticAssertErrorCondition(InitialisationError, "ReadWaveformFiles: Failed opening File %s", FilePath );
        return False;
    }
	
	cdb->ReadFromStream(temp_file);
	CDBExtended cdbe(cdb);
	
	if(!cdbe->Move("+MARTe.+ISTTOK_RTTh.+waveform_primary.waveform_mode_1_positive"))
	{
		CStaticAssertErrorCondition(InitialisationError,"WaveformsDisplay::ObjectLoadSetup: %s Could not move to \"+MARTe.+ISTTOK_RTTh.+waveform_primary\"",this->Name());
		temp_file.Close();
		return False;
	}
	int vector_size = 0;
	
		if(!cdbe.ReadInt32(vector_size, "vector_size"))
		{
			CStaticAssertErrorCondition(InitialisationError,"WaveformsDisplay::ObjectLoadSetup: %s vector_size",this->Name());
			return False;
		}
		else	CStaticAssertErrorCondition(InitialisationError,"WaveformsDisplay::ObjectLoadSetup: vector_size = %d",vector_size);
	
//	
	
	cdbe->MoveToRoot();
	
	temp_file.Close();
	
	*/
	return True;
}

bool WaveformsDisplay::WriteConfigurationFileWithChanges(char BaseFileFilePath[], char TargetFilePath[]){
	
	CDBExtended cdb;
    File configIn;
    if(!configIn.OpenRead((char *)BaseFileFilePath)){
       printf("WaveformsDisplay:: Failed opening file %s\n", BaseFileFilePath);
       return 0;
    }
    FileEraseFile((char *)TargetFilePath);
    File configOut;
    if(!configOut.OpenWrite((char *)TargetFilePath)){
       printf("WaveformsDisplay:: Failed open write file %s\n", TargetFilePath);
       return 0;
    }

    cdb->ReadFromStream(configIn);
    configIn.Close();
/*
    bool test = cdb->Move("+MARTe.+Thread_1.+WaterTank");
    printf("Moved ok = %d\n", test);
    //Change the voltage value
    float maxVoltage = 0;
    cdb.ReadFloat(maxVoltage, "MaxVoltage");
    printf("maxVoltage = %f\n", maxVoltage);
    //Change the max voltage to 200
    cdb.WriteFloat(200, "MaxVoltage");
    cdb->MoveToRoot();
    //e arrays??
    test = cdb->Move("+MARTe.+Thread_1.+WaveformGen.+zeroSignal");
    printf("Moved ok = %d\n", test);
    int32 arraySize = 10;
    int32 *timeArr  = new int32[arraySize];
    //Should if this failed
    float *dataArr  = new float[arraySize];
    //Should if this failed
    for(int32 i=0; i<arraySize; i++){
        timeArr[i] = i;
        dataArr[i] = i*1e-3;
    }
    cdb->WriteArray(timeArr, CDBTYPE_int32, NULL, arraySize, "TimeVector");
    cdb->WriteArray(dataArr, CDBTYPE_float, NULL, arraySize, "ValueVector");
    delete []timeArr;
    delete []dataArr;
    //voltar sempre 'a root..
    cdb->MoveToRoot();
*/
    //Write cdb to string. Tu vais utilizar esta versao (nao vais guardar num ficheiro, claro). Isto e' o que vai depois ser enviado ao MARTe
 //   FString cdbString;
    cdb->WriteToStream(cdbString);
    cdbString.Seek(0);
    //Write to file...
    cdb->WriteToStream(configOut);
    configOut.Close();
 
	return True;
}
