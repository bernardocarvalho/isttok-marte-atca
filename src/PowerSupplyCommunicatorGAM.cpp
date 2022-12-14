/***************************************************************************************************
*
*	PowerSupplyCommunicatorGAM - ivoc - 6/5/2011, based on danielv FAPowerSupplyCommunicatorGAM + communicationscore
*
****************************************************************************************************/


#include "PowerSupplyCommunicatorGAM.h"

OBJECTLOADREGISTER(PowerSupplyCommunicatorGAM, "$Id: $")


//  ******** Default constructor ***********************************
PowerSupplyCommunicatorGAM::PowerSupplyCommunicatorGAM(){
	this->SignalsInputInterface = NULL;
	this->SignalsOutputInterface = NULL;
	
	this->FaUART = NULL;
	this->receivedStartingPacket = False;
	this->numMessagePackets = 0;

	this->statisticsReceivedPackets = 0;
	this->statisticsSentPackets = 0;

	this->statisticsReceivedMessages = 0;
	this->statisticsSentMessages = 0;
	this->statisticsIncompleteMessages = 0;

	this->statisticsWrongMessagesReceived = 0;

	this->CurrentStep = 0.0;
	this->PointOfZeroCurrent = 0.0;
}

// ********* Destructor ********************************************
PowerSupplyCommunicatorGAM::~PowerSupplyCommunicatorGAM()
{
//	if(this->SignalsInputInterface != NULL) delete[] this->SignalsInputInterface ;
}



//{ ********* Initialise the module ********************************
bool PowerSupplyCommunicatorGAM::Initialise(ConfigurationDataBase& cdbData){
   
	CDBExtended cdb(cdbData);

	int i;

	if(!cdb.ReadInt32(UARTPortAddress, "UARTPortAddress"))
	{
		AssertErrorCondition(InitialisationError,"PowerSupplyCommunicatorGAM::Initialise: %s Could not UARTPortAddress",this->Name());
		return False;
	}
	else {
		this->FaUART = new UFSerialUART(UARTPortAddress);
		AssertErrorCondition(Information,"PowerSupplyCommunicatorGAM::Initialise: UARTPortAddress = %X",this->UARTPortAddress);
	}
	// Check port initialisation
	if(this->FaUART->IsUARTInitialised())
		AssertErrorCondition(Information, "PowerSupplyCommunicatorGAM::Initialise: %s, Using UART port address %d.",this->Name(), this->FaUART->GetUARTPortAddress());
	else
	{
		AssertErrorCondition(InitialisationError, "PowerSupplyCommunicatorGAM::Initialise: %s failed get permission to use the UART at address %d.", this->Name(),this->FaUART->GetUARTPortAddress());
		return False;
	}
	// Configure the UART
	if(!this->ConfigureUART())
	{
		AssertErrorCondition(InitialisationError, "PowerSupplyCommunicatorGAM::Initialise: %s failed to configure the UART.", this->Name());
		return False;
	}
	if(!cdb.ReadFloat(PointOfZeroCurrent, "PointOfZeroCurrent"))
	{
		AssertErrorCondition(InitialisationError,"PowerSupplyCommunicatorGAM::Initialise: %s PointOfZeroCurrent",this->Name());
		return False;
	}
	else AssertErrorCondition(Information,"PowerSupplyCommunicatorGAM::Initialise: PointOfZeroCurrent = %f",PointOfZeroCurrent);
	if(!cdb.ReadFloat(CurrentStep, "CurrentStep"))
	{
		AssertErrorCondition(InitialisationError,"PowerSupplyCommunicatorGAM::Initialise: %s CurrentStep",this->Name());
		return False;
	}
	else AssertErrorCondition(Information,"PowerSupplyCommunicatorGAM::Initialise: CurrentStep = %f",CurrentStep);
	if(!cdb.ReadInt32(usecGlobalPeriod, "usecGlobalPeriod"))
	{
		AssertErrorCondition(InitialisationError,"PowerSupplyCommunicatorGAM::Initialise: %s usecGlobalPeriod",this->Name());
		return False;
	}
	else AssertErrorCondition(Information,"PowerSupplyCommunicatorGAM::Initialise: usecGlobalPeriod = %d",usecGlobalPeriod);


//	sleep(3);
// Create the signal interfaces
	if(!AddInputInterface(this->SignalsInputInterface, "PowerSupplyCommunicatorGAMInputInterface"))
	{
		AssertErrorCondition(InitialisationError, "PowerSupplyCommunicatorGAM::Initialise: %s failed to add the PowerSupplyCommunicatorGAMInputInterface", this->Name());
		return False;
	}
	if(!AddOutputInterface(this->SignalsOutputInterface, "PowerSupplyCommunicatorGAMOutputInterface"))
	{
		AssertErrorCondition(InitialisationError, "PowerSupplyCommunicatorGAM::Initialise: %s failed to add the PowerSupplyCommunicatorGAMOutputInterface", this->Name());
		return False;
	}

//	INPUT SIGNALS (interface)
	if(!cdb->Move("input_signals"))
	{
		AssertErrorCondition(InitialisationError,"PowerSupplyCommunicatorGAM::Initialise: %s Could not move to \"input_signals\"",this->Name());
		return False;
	}
	
		int number_of_signals_to_read = 3;
		FString *CDB_move_to;
		FString *SignalType;
		CDB_move_to = new FString[number_of_signals_to_read];
		SignalType = new FString[number_of_signals_to_read];
		CDB_move_to[0].Printf("CurrentSignal");
		CDB_move_to[1].Printf("GlobalTime");
		CDB_move_to[2].Printf("discharge_status");
		for (i=0;i<number_of_signals_to_read;i++){
			
			if(!cdb->Move(CDB_move_to[i].Buffer()))
			{
				AssertErrorCondition(InitialisationError,"PowerSupplyCommunicatorGAM::Initialise: %s Could not move to \"%s\"",this->Name(),CDB_move_to[i].Buffer());
				return False;
			}
			
			if(cdb->Exists("SignalType"))
			{
				FString signalName;
				cdb.ReadFString(SignalType[i], "SignalType");
			}
			if(cdb->Exists("SignalName"))
			{
				FString SignalName;
				cdb.ReadFString(SignalName, "SignalName");
				AssertErrorCondition(Information,"PowerSupplyCommunicatorGAM::Initialise: Added signal = %s", SignalName.Buffer());
				
				if(!this->SignalsInputInterface->AddSignal(SignalName.Buffer(), SignalType[i].Buffer()))
				{
					AssertErrorCondition(InitialisationError,"PowerSupplyCommunicatorGAM::Initialise: %s failed to add signal", this->Name());
					return False;
				}
			}
			
			
			cdb->MoveToFather();
		}

	cdb->MoveToFather();
	
//	OUTPUT SIGNALS (interface)
	if(!cdb->Move("output_signals"))
	{
		AssertErrorCondition(InitialisationError,"owerSupplyCommunicatorGAM::Initialise: %s Could not move to \"output_signals\"",this->Name());
		return False;
	}
	
		number_of_signals_to_read = 2;
		CDB_move_to = new FString[number_of_signals_to_read];
		SignalType = new FString[number_of_signals_to_read];
		CDB_move_to[0].Printf("soft_stop");
		CDB_move_to[1].Printf("ReceiveValue");
		for (i=0;i<number_of_signals_to_read;i++){
			
			if(!cdb->Move(CDB_move_to[i].Buffer()))
			{
				AssertErrorCondition(InitialisationError,"owerSupplyCommunicatorGAM::Initialise: %s Could not move to \"%s\"",this->Name(),CDB_move_to[i].Buffer());
				return False;
			}
			
			if(cdb->Exists("SignalType"))
			{
				FString signalName;
				cdb.ReadFString(SignalType[i], "SignalType");
			}
			if(cdb->Exists("SignalName"))
			{
				FString SignalName;
				cdb.ReadFString(SignalName, "SignalName");
				AssertErrorCondition(Information,"owerSupplyCommunicatorGAM::Initialise: Added signal = %s", SignalName.Buffer());
				
				if(!this->SignalsOutputInterface->AddSignal(SignalName.Buffer(), SignalType[i].Buffer()))
				{
					AssertErrorCondition(InitialisationError,"owerSupplyCommunicatorGAM::Initialise: %s failed to add signal", this->Name());
					return False;
				}
			}
			cdb->MoveToFather();
		}

	cdb->MoveToFather();


	this->isCharged = False;
	this->isStarted = False;
	this->IsTriggered = False;
	this->temperatureFailure = False;
	this->voltageFailure = False;
	this->stopFailure = False;
	this->logReceivedPointer=0;
	this->logSentPointer=0;
	this->communicationFailure = False;

	this->communicatorOnlineStage = FA_COMMUNICATOR_ONLINE_IDLE;

	this->logReceivedPointer = -1;

	this->communicatorOnlineStartOperationAttempts = 0;
	this->communicatorOnlineStopOperationAttempts = 0;
	this->communicatorOnlineIdleCount=0;
	this->communicatorOnlineWaitTriggerCount=0;
	this->communicatorOnlineDischargeCount=0;
	this->communicatorOnlineStopCount=0;
	this->communicatorOnlineErrorCount=0;
	this->statisticsCurrentValue=0;
	this->statisticsTemperatureFault=0;
	this->statisticsCharged=0;
	this->statisticsNotCharged=0;
	this->statisticsVoltageFault=0;
	this->statisticsStartOk=0;
	this->statisticsStopped=0;
	this->statisticsStopFault=0;
	this->statisticsCommunicationFault=0;
	this->statisticsReceivedPackets=0;
	this->statisticsSentPackets=0;
	this->statisticsReceivedMessages=0;
	this->statisticsSentMessages=0;
	this->statisticsIncompleteMessages=0;
	this->statisticsWrongMessagesReceived=0;

	for (i=0;i<NLOGMESSAGES;i++){
		logReceivedMessages[i] = 0;
		logSentMessages[i] = 0;
		logReceivedCurrents[i] = 0;
		logSentCurrents[i] = 0;
		logTimeReceivedMessages[i] = 0;
		logTimeSentMessages[i] = 0;
	}

	return True;
}
//} ******************************************************************



//{ ********* Execute the module functionalities *******************
bool PowerSupplyCommunicatorGAM::Execute(GAM_FunctionNumbers functionNumber){

	InputInterfaceStruct *inputstruct = (InputInterfaceStruct *) this->SignalsInputInterface->Buffer();
	this->SignalsInputInterface->Read();
//	AssertErrorCondition(InitialisationError,"PowerSupplyCommunicatorGAM:: %s inputstruct = %f %d %d ",this->Name(), inputstruct[0].SendValue, inputstruct[0].usecTime, inputstruct[0].DischargeStatus);
	OutputInterfaceStruct *outputstruct = (OutputInterfaceStruct *) this->SignalsOutputInterface->Buffer();


	usectimecopy = inputstruct[0].usecTime;
	CurrentToSendCopy = (float) inputstruct[0].SendValue;
	
//	PlasmaEnded			-> alterar esta variavel
//	IsTriggered			-> alterar tambem	
	
	

		
		if (inputstruct[0].DischargeStatus >=0 ){
			this->GetMessages();
			this->IsTriggered = True;
			this->PlasmaEnded = False;
			outputstruct[0].ReceiveValue = this->logReceivedCurrents[this->logReceivedPointer];
		}
		else if (IsTriggered){
		    this->GetMessages();
			this->PlasmaEnded = True;
		} 
			
			
				switch(functionNumber){
					// Called once to verify if the parameter configuration is acceptable

					// Called once before going online
					case GAMPrepulse:

						this->communicatorOnlineStage = FA_COMMUNICATOR_ONLINE_WAIT_CODAC_TRIGGER;

						this->communicatorOnlineIdleCount = 0;
						this->communicatorOnlineWaitTriggerCount = 0;
						this->communicatorOnlineDischargeCount = 0;
						this->communicatorOnlineStopCount = 0;
						this->communicatorOnlineErrorCount = 0;
						this->communicatorOnlineStopOperationAttempts = 0;
						this->communicatorOnlineStartOperationAttempts = 0;
						
						PlasmaEnded = False;
					break; // GAMPrepulse

					// Called continuously when online after data has been read
					case GAMOnline:

						if(!this->CommunicatorOnline())
							return False;

					break; // GAMOnline

					// Called once before going offline
					case GAMPostpulse:
						outputstruct[0].SoftStop = 0;
//						this->ResetTrigger();

					break; // GAMPostpulse

					// Called continuously while offline
					case GAMOffline:
						outputstruct[0].SoftStop = 0;
						if(!this->CommunicatorOffline())
							return False;

					break; // GAMOffline

					// Called continuously after a fault has been detected
					case GAMSafety:
						break; // GAMSafety

					// None of the above
					default:
						break; // default
				}


	this->SignalsOutputInterface->Write();

	return True;		
}

// Behaviour for the communicator
bool PowerSupplyCommunicatorGAM::CommunicatorOffline(){
	return True;
}


bool PowerSupplyCommunicatorGAM::CommunicatorOnline(){
	
	unsigned char packet1, packet2;

	// Iterate the online stages of the communicator state machine
	switch(this->communicatorOnlineStage)
	{
		case FA_COMMUNICATOR_ONLINE_IDLE:
			// Log the entry on this stage
			if(this->communicatorOnlineIdleCount++ == 0) AssertErrorCondition(Information, "[FACom] COMMUNICATOR_ONLINE_IDLE");
		break;

		// Wait for the CODAC trigger
		case FA_COMMUNICATOR_ONLINE_WAIT_CODAC_TRIGGER:

			// Log the entry on this stage
			if(this->communicatorOnlineWaitTriggerCount++ == 0)	AssertErrorCondition(Information, "[FACom] COMMUNICATOR_ONLINE_WAIT_CODAC_TRIGGER");

			if(IsTriggered)
			{
				// Send Start Operation message
				this->SendMessage(FA_STARTOP_MESSAGE_1, FA_STARTOP_MESSAGE_2);

				// Increase attempts counter
				this->communicatorOnlineStartOperationAttempts++;

				// Change online state
				this->communicatorOnlineStage = FA_COMMUNICATOR_ONLINE_DISCHARGE;
				this->communicatorOnlineWaitTriggerCount = 0;
			}

		break;

		case FA_COMMUNICATOR_ONLINE_DISCHARGE:

			// Log the entry on this stage
			//this->communicatorOnlineDischargeCount++;
			if(this->communicatorOnlineDischargeCount++ == 0)
				AssertErrorCondition(Information, "[FACom] COMMUNICATOR_ONLINE_DISCHARGE");

			if(!this->isStarted)
			{
				// DEBUG
				//AssertErrorCondition(Information,
				//			"[FACom] COMMUNICATOR_ONLINE_DISCHARGE: is NOT started");

				// Check how many attemps were made
				if(this->communicatorOnlineStartOperationAttempts >= FA_COMMUNICATOR_MAXIMUM_ATTEMPTS)
				{
					AssertErrorCondition(Timeout, "[FACom] Start Operation Timeout: %d attemps were made", this->communicatorOnlineStartOperationAttempts);

					this->communicatorOnlineStage = FA_COMMUNICATOR_ONLINE_ERROR;
					this->communicatorOnlineDischargeCount = 0;

					return True;
				}

				// Send Start Operation message
				this->SendMessage(FA_STARTOP_MESSAGE_1, FA_STARTOP_MESSAGE_2);

				// Increase attempts counter
				this->communicatorOnlineStartOperationAttempts++;
			}
			else
			{
				// DEBUG
				//AssertErrorCondition(Information,"[FACom] COMMUNICATOR_ONLINE_DISCHARGE: is started");

				if(PlasmaEnded)
				{
					this->communicatorOnlineStage = FA_COMMUNICATOR_ONLINE_STOP_OPERATION;
					this->communicatorOnlineDischargeCount = 0;
					break;
				}

				// Generate packets
				this->CreateCurrentPackets(CurrentToSendCopy, packet1, packet2);

				// Send packets
				this->SendMessage(packet1, packet2);
			}

		break;

		case FA_COMMUNICATOR_ONLINE_STOP_OPERATION:

			// Log the entry on this stage
			if(this->communicatorOnlineStopCount++ == 0)
				AssertErrorCondition(Information, "[FACom] COMMUNICATOR_ONLINE_STOP_OPERATION");

			if(this->isStarted)
			{
				// Check how many attemps were made
				if(this->communicatorOnlineStopOperationAttempts >= FA_COMMUNICATOR_MAXIMUM_ATTEMPTS)
				{
					AssertErrorCondition(Timeout, "[FACom] Stop Operation Timeout: %d attemps were made", this->communicatorOnlineStopOperationAttempts);

					this->communicatorOnlineStage = FA_COMMUNICATOR_ONLINE_ERROR;
					this->communicatorOnlineStopCount = 0;

					return True;
				}

				// Send Stop Operation message
				this->SendMessage(FA_STOPOP_MESSAGE_1, FA_STOPOP_MESSAGE_2);

				// Increase attempts counter
				this->communicatorOnlineStopOperationAttempts++;
			}
			else
			{
				// Trigger Plasma Ended flag
//				plasmaEndedTriggerOutput = (int *) this->PlasmaEndOutputInterface->Buffer();
//				*plasmaEndedTriggerOutput = 1;
//				this->PlasmaEndOutputInterface->Write();

				this->communicatorOnlineStage = FA_COMMUNICATOR_ONLINE_IDLE;
				this->communicatorOnlineStopCount = 0;
			}

		break;

		case FA_COMMUNICATOR_ONLINE_ERROR:
			
			AssertErrorCondition(InitialisationError, "[FACom]::%s Power supplies timeout: after %d attemps, FA_COMMUNICATOR_ONLINE_ERROR, RETURN FALSE", this->Name(), this->communicatorOnlineStopOperationAttempts);

			return False;

		break;

		default:
		break;
	}

	return True;
}

// Configure the FA UART
bool PowerSupplyCommunicatorGAM::ConfigureUART(){
	
	this->FaUART->Select950Mode();
	this->FaUART->Disable9BitMode();
	this->FaUART->SetFrequencyDivider(0x0001);
	this->FaUART->SetEightBitsLength();
	this->FaUART->SetOddParity();
	this->FaUART->SelectTwoStopBits();
	this->FaUART->EnableFifos();

	return True;
}

// Send a message to the power supplies
void PowerSupplyCommunicatorGAM::SendMessage(unsigned char packet1, unsigned char packet2){
	// Send message packets
	this->FaUART->SendValue(packet1);
	this->FaUART->SendValue(packet2);

	// Update sent counters
	this->statisticsSentPackets += 2;
	this->statisticsSentMessages++;
}
// Create/Decode current packets
bool PowerSupplyCommunicatorGAM::CreateCurrentPackets(float current, unsigned char &packet1, unsigned char &packet2){

	// Calculate the point in the scale of the current
	short pointOfCurrent = (short)(this->PointOfZeroCurrent + current / this->CurrentStep);

	// Saturate current
	if(pointOfCurrent < FA_SCALE_MIN)
		pointOfCurrent = FA_SCALE_MIN;
	if(pointOfCurrent > FA_SCALE_MAX)
		pointOfCurrent = FA_SCALE_MAX;

	// Build packets
	unsigned short pc = (unsigned short) pointOfCurrent;
	unsigned short nc = ~pc;
	packet1 = (unsigned char)(0x0000 |
				((nc & 0x03C0) >> 5) |
				((pc & 0x0007) << 5));
	packet2 = (unsigned char)(0x0001 |
				((pc & 0x03F8) >> 2));

	return True;
}


// Get messages in the buffer
void PowerSupplyCommunicatorGAM::GetMessages(){
	
	unsigned int packetCounter = 0;
	unsigned short packet;

	while(this->FaUART->IsDataAvailable())
	{
		// Increment packet counter and check if maximum allowed packets were attained
		if(++packetCounter > FA_COMMUNICATION_MAX_PACKETS)
			break;

		// Get the next packet
		this->FaUART->ReadValue(packet);

		// Increment total packet counter
		this->statisticsReceivedPackets++;

		// DEBUG
		//AssertErrorCondition(InitialisationError,
		//			"Packet received (%d): 0x%02x   %d", packetCounter, packet, packet);


		// Received a starting packet, clear the packet buffer
		if((packet & FA_FRAMING_BIT_MASK) == 0)
		{
			// Update count if no complete message is received
			if(this->numMessagePackets != 0)
				this->statisticsIncompleteMessages++;

			this->numMessagePackets = 0;
			this->receivedStartingPacket = True;
		}

		// Add packet to the buffer and increment packet counter
		this->messagePackets[this->numMessagePackets] = packet;
		this->numMessagePackets++;

		// Message complete, process message
		if(this->numMessagePackets == 2)
		{
			// Guarantee that a starting packet was received
			if(this->receivedStartingPacket)
			{
				// Interpret message
				this->InterpretMessage((unsigned char) messagePackets[0], (unsigned char) messagePackets[1]);
			}

			this->statisticsReceivedMessages++;

			// Clear packet counter and flags
			this->numMessagePackets = 0;
			this->receivedStartingPacket = False;
		}
	} // while(this->FaUART->IsDataAvailable())
}

bool PowerSupplyCommunicatorGAM::DecodeCurrentPackets(float &current, unsigned char packet1, unsigned char packet2){
	// Validate packets
	unsigned char validation = (packet2 & 0xF0) ^ ((packet1 & 0x1E) << 3);
	if(validation != 0xF0)
	{
		this->statisticsWrongMessagesReceived++;
		return False;
	}
	else this->statisticsWrongMessagesReceived=0;

	// Decode packets
	short pointOfCurrent = (short)(	(((unsigned short) packet1 & 0x00E0) >> 5)	| (((unsigned short) packet2 & 0x00FE) << 2));

	// Calculate the current
	current = ((float) pointOfCurrent - this->PointOfZeroCurrent) * this->CurrentStep;

	return True;
}


// Implement message interpretation functionalities
void PowerSupplyCommunicatorGAM::InterpretMessage(unsigned char packet1, unsigned char packet2){
	
	// Increment log pointer
	this->logReceivedPointer++;
	if(this->logReceivedPointer >= NLOGMESSAGES)
		this->logReceivedPointer = 0;

	this->logTimeReceivedMessages[this->logReceivedPointer] = this->usectimecopy;

	// Operation Started message
	if((packet1 == FA_STARTED_MESSAGE_1) && (packet2 == FA_STARTED_MESSAGE_2))
	{

		this->isStarted = True;
		this->statisticsStartOk++;
		this->logReceivedMessages[this->logReceivedPointer] = LOG_STARTED;

		return;
	}

	// Operation Stopped message
	if((packet1 == FA_STOPPED_MESSAGE_1) && (packet2 == FA_STOPPED_MESSAGE_2)){

		this->isStarted = False;
		this->statisticsStopped++;
		this->logReceivedMessages[this->logReceivedPointer] = LOG_STOPPED;
		this->IsTriggered = False;

		return;
	}

	// Stop Error message
	if((packet1 == FA_STOP_ERROR_MESSAGE_1) && (packet2 == FA_STOP_ERROR_MESSAGE_2)){

		this->stopFailure = True;
		this->statisticsStopFault++;
		this->logReceivedMessages[this->logReceivedPointer] = LOG_STOP_FAULT;

		return;
	}


	// Communication Error message
	if((packet1 == FA_COMM_ERROR_MESSAGE_1) && (packet2 == FA_COMM_ERROR_MESSAGE_2)) {

		this->communicationFailure = True;
		this->statisticsCommunicationFault++;
		this->logReceivedMessages[this->logReceivedPointer] = LOG_COMMUNICATION_FAULT;

		return;
	}

	// Current Value
	this->logReceivedMessages[this->logReceivedPointer] = LOG_CURRENT_VALUE;
	float current;
	if(this->DecodeCurrentPackets(current, packet1, packet2))
		this->logReceivedCurrents[this->logReceivedPointer] = current;
	else
		this->logReceivedCurrents[this->logReceivedPointer] = 0.0;

	// Other messages
}

bool PowerSupplyCommunicatorGAM::ProcessHttpMessage(HttpStream &hStream){
	
	HtmlStream hmStream(hStream);
	int i;
	hmStream.SSPrintf(HtmlTagStreamMode, "html>\n\
		<head>\n\
		<title>%s</title>\n\
		</head>\n\
		<body>\n\
		<svg width=\"100&#37;\" height=\"100\" style=\"background-color: AliceBlue;\">\n\
		<image x=\"%d\" y=\"%d\" width=\"%d\" height=\"%d\" xlink:href=\"%s\" />\n\
		</svg", (char *) this->Name() ,0, 0, 422, 87, "http://www.ipfn.ist.utl.pt/ipfnPortalLayout/themes/ipfn/_img_/logoIPFN_Topo_officialColours.png");

	hmStream.SSPrintf(HtmlTagStreamMode, "br><br><text style=\"font-family:Arial;font-size:46\">%s</text><br", (char *) this->Name());

    FString submit_view;
    submit_view.SetSize(0);
    if (hStream.Switch("InputCommands.submit_view")){
        hStream.Seek(0);
        hStream.GetToken(submit_view, "");
        hStream.Switch((uint32)0);
    }
	if(submit_view.Size() > 0) view_input_variables = True;
	
	FString submit_hide;
    submit_hide.SetSize(0);
    if (hStream.Switch("InputCommands.submit_hide")){
        hStream.Seek(0);
        hStream.GetToken(submit_hide, "");
        hStream.Switch((uint32)0);
    }
	if(submit_hide.Size() > 0) view_input_variables = False;

	hmStream.SSPrintf(HtmlTagStreamMode, "form enctype=\"multipart/form-data\" method=\"post\"");
	if(!view_input_variables){
		hmStream.SSPrintf(HtmlTagStreamMode, "input type=\"submit\" name=\"submit_view\" value=\"View input variables\"");
	}
	else {
		hmStream.SSPrintf(HtmlTagStreamMode, "input type=\"submit\" name=\"submit_hide\" value=\"Hide input variables\"");		
		hmStream.SSPrintf(HtmlTagStreamMode, "br><br>UARTPortAddress = %X\n\
		<br>PointOfZeroCurrent = %.2f\n\
		<br>CurrentStep = %.2f\n\
		<br>usecGlobalPeriod = %d\n\
		<br><br",this->UARTPortAddress,this->PointOfZeroCurrent,this->CurrentStep,this->usecGlobalPeriod);
		
		hmStream.SSPrintf(HtmlTagStreamMode, "h1");
		hmStream.SSPrintf(HtmlTagStreamMode, "center");
		hmStream.Printf("Fast Amplifier Communicator");
		hmStream.SSPrintf(HtmlTagStreamMode, "/center");
		hmStream.SSPrintf(HtmlTagStreamMode, "/h1");


		// ******************************************************
		// FA status
		// ******************************************************
		hmStream.SSPrintf(HtmlTagStreamMode, "h2");
		hmStream.SSPrintf(HtmlTagStreamMode, "center");
		hmStream.Printf("Fast Amplifier Status");
		hmStream.SSPrintf(HtmlTagStreamMode, "/center");
		hmStream.SSPrintf(HtmlTagStreamMode, "/h2");

		hStream.Printf("<TABLE BORDER=\"2\" BORDERCOLOR=\"#336699\" CELLPADDING=\"2\" CELLSPACING=\"2\" WIDTH=\"100%%\">\n");
		hStream.Printf("<TR><TH>Parameter</TH><TH>Value</TH></TR>\n");
		if(this->isCharged)
			hStream.Printf("<TR><TD>Charged</TD><TD><FONT COLOR=\"#00FF00\">Yes!</FONT></TD></TR>\n");
		else
			hStream.Printf("<TR><TD>Charged</TD><TD><FONT COLOR=\"#FF0000\">No!</FONT></TD></TR>\n");
		if(this->isStarted)
			hStream.Printf("<TR><TD>Operation Started</TD><TD><FONT COLOR=\"#00FF00\">Yes!</FONT></TD></TR>\n");
		else
			hStream.Printf("<TR><TD>Operation Started</TD><TD><FONT COLOR=\"#FF0000\">No!</FONT></TD></TR>\n");
		if(!this->temperatureFailure)
			hStream.Printf("<TR><TD>Temperature</TD><TD><FONT COLOR=\"#00FF00\">OK!</FONT></TD></TR>\n");
		else
			hStream.Printf("<TR><TD>Temperature</TD><TD><FONT COLOR=\"#FF0000\">Failure!</FONT></TD></TR>\n");
		if(!this->voltageFailure)
			hStream.Printf("<TR><TD>Voltage</TD><TD><FONT COLOR=\"#00FF00\">OK!</FONT></TD></TR>\n");
		else
			hStream.Printf("<TR><TD>Voltage</TD><TD><FONT COLOR=\"#FF0000\">Failure!</FONT></TD></TR>\n");
		if(!this->stopFailure)
			hStream.Printf("<TR><TD>Stopped</TD><TD><FONT COLOR=\"#00FF00\">OK!</FONT></TD></TR>\n");
		else
			hStream.Printf("<TR><TD>Stopped</TD><TD><FONT COLOR=\"#FF0000\">Failure!</FONT></TD></TR>\n");
		hStream.Printf("</TABLE>\n");

		// ******************************************************
		// FA calibration
		// ******************************************************

		// ******************************************************
		// Communication statistics
		// ******************************************************
		hmStream.SSPrintf(HtmlTagStreamMode, "h2");
		hmStream.SSPrintf(HtmlTagStreamMode, "center");
		hmStream.Printf("Communication Statistics");
		hmStream.SSPrintf(HtmlTagStreamMode, "/center");
		hmStream.SSPrintf(HtmlTagStreamMode, "/h2");

		hStream.Printf("<TABLE BORDER=\"2\" BORDERCOLOR=\"#336699\" CELLPADDING=\"2\" CELLSPACING=\"2\" WIDTH=\"100%%\">\n");
		hStream.Printf("<TR><TD></TD><TD>Number of Occurrences</TD></TR>\n");
		hStream.Printf("<TR><TD>Packets Sent</TD><TD>%d</TD></TR>\n", this->statisticsSentPackets);
		hStream.Printf("<TR><TD>Packets Received</TD><TD>%d</TD></TR>\n", this->statisticsReceivedPackets);
		hStream.Printf("<TR><TD>Messages Sent</TD><TD>%d</TD></TR>\n", this->statisticsSentMessages);
		hStream.Printf("<TR><TD>Messages Received</TD><TD>%d</TD></TR>\n", this->statisticsReceivedMessages);
		hStream.Printf("<TR><TD>Incomplete Messages</TD><TD>%d</TD></TR>\n", this->statisticsIncompleteMessages);
		hStream.Printf("<TR><TD>Wrong Messages</TD><TD>%d</TD></TR>\n", this->statisticsWrongMessagesReceived);
		hStream.Printf("</TABLE>\n");

		hStream.Printf("<TABLE BORDER=\"2\" BORDERCOLOR=\"#336699\" CELLPADDING=\"2\" CELLSPACING=\"2\" WIDTH=\"100%%\">\n");
		hStream.Printf("<TR><TD>Received Messages</TD><TD>Number of Occurrences</TD></TR>\n");
		hStream.Printf("<TR><TD>Current Value</TD><TD>%d</TD></TR>\n", this->statisticsCurrentValue);
		hStream.Printf("<TR><TD>Charged</TD><TD>%d</TD></TR>\n", this->statisticsCharged);
		hStream.Printf("<TR><TD>Not Charged</TD><TD>%d</TD></TR>\n", this->statisticsNotCharged);
		hStream.Printf("<TR><TD>Start OK</TD><TD>%d</TD></TR>\n", this->statisticsStartOk);
		hStream.Printf("<TR><TD>Stopped</TD><TD>%d</TD></TR>\n", this->statisticsStopped);
		hStream.Printf("<TR><TD>Stop Fault</TD><TD>%d</TD></TR>\n", this->statisticsStopFault);
		hStream.Printf("<TR><TD>Temperature Fault</TD><TD>%d</TD></TR>\n", this->statisticsTemperatureFault);
		hStream.Printf("<TR><TD>Voltage Fault</TD><TD>%d</TD></TR>\n", this->statisticsVoltageFault);
		hStream.Printf("<TR><TD>Communication Fault</TD><TD>%d</TD></TR>\n", this->statisticsCommunicationFault);
		hStream.Printf("</TABLE>\n");

		// ******************************************************
		// Message Logs
		// ******************************************************
		hmStream.SSPrintf(HtmlTagStreamMode, "h2");
		hmStream.SSPrintf(HtmlTagStreamMode, "center");
		hmStream.Printf("Message Logs");
		hmStream.SSPrintf(HtmlTagStreamMode, "/center");
		hmStream.SSPrintf(HtmlTagStreamMode, "/h2");

		hmStream.Printf("N Log = %d", this->logReceivedPointer);

/*
		hStream.Printf("<TABLE BORDER=\"2\" BORDERCOLOR=\"#336699\" CELLPADDING=\"2\" CELLSPACING=\"2\" WIDTH=\"100%%\">\n");
		hStream.Printf("<TR><TD>Time</TD><TD>Sent Message</TD></TR>\n");
		for(int i = 0 ; i < this->logSentPointer ; i++)
			hStream.Printf("<TR><TD>?</TD><TD>%d</TD></TR>\n", this->logTimeSentMessages[i], this->logSentMessages[i]);
		for(int i = this->logSentPointer ; i < NLOGMESSAGES ; i++)
			hStream.Printf("<TR><TD>?</TD><TD>%d</TD></TR>\n", this->logTimeSentMessages[i], this->logSentMessages[i]);
		hStream.Printf("</TABLE>\n");
*/	
		hStream.Printf("<TABLE BORDER=\"2\" BORDERCOLOR=\"#336699\" CELLPADDING=\"2\" CELLSPACING=\"2\" WIDTH=\"100%%\">\n");
		hStream.Printf("<TR><TD>Time</TD><TD>Received Message</TD></TR>\n");
		int index = this->logReceivedPointer + 1;
		while(1)
		{
			FString msgText;
			switch(this->logReceivedMessages[index])
			{
				case LOG_TEMPERATURE_FAULT:
					msgText.Printf("Temperature Fault");
					break;

				case LOG_24V_FAULT:
					msgText.Printf("24V Fault");
					break;

				case LOG_COMMUNICATION_FAULT:
					msgText.Printf("Communication Fault");
					break;

				case LOG_CHARGED:
					msgText.Printf("Charged");
					break;

				case LOG_NOT_CHARGED:
					msgText.Printf("Not Charged");
					break;

				case LOG_STARTED:
					msgText.Printf("Started");
					break;

				case LOG_STOPPED:
					msgText.Printf("Stopped");
					break;

				case LOG_STOP_FAULT:
					msgText.Printf("Stop Fault");
					break;

				case LOG_CURRENT_VALUE:
					msgText.Printf("Current Value");
					break;

				default:
					msgText.Printf("None");
					break;
			}

			if(this->logReceivedMessages[index] != LOG_CURRENT_VALUE)
				hStream.Printf("<TR><TD>%d</TD><TD>%s</TD></TR>\n",
							   this->logTimeReceivedMessages[index],
							   msgText.Buffer());
			else
				hStream.Printf("<TR><TD>%d</TD><TD>%s (%f A)</TD></TR>\n",
							   this->logTimeReceivedMessages[index],
							   msgText.Buffer(),
							   this->logReceivedCurrents[index]);

			if(++index >= NLOGMESSAGES)
				index = 0;
			if(index == this->logReceivedPointer + 1)
				break;
		}
		hStream.Printf("</TABLE>\n");

	
			
			
			
			
		}
		hmStream.SSPrintf(HtmlTagStreamMode, "/form");
		
		
		hStream.SSPrintf("OutputHttpOtions.Content-Type","text/html;charset=utf-8");
		hStream.WriteReplyHeader(True);


	return True;		
}
