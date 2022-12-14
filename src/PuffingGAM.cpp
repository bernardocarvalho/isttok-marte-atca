
#include "PuffingGAM.h"

OBJECTLOADREGISTER(PuffingGAM, "$Id: $")


//  ******** Default constructor ***********************************
PuffingGAM::PuffingGAM(){
	this->SignalsInputInterface = NULL;
	this->FaUART = NULL;
	this->receivedStartingPacket = False;
	this->numMessagePackets = 0;

	this->statisticsReceivedPackets = 0;
	this->statisticsSentPackets = 0;

	this->statisticsReceivedMessages = 0;
	this->statisticsSentMessages = 0;
	this->statisticsIncompleteMessages = 0;

	this->statisticsWrongMessagesReceived = 0;

	this->Step = 0.0;
	this->PointOfZero = 0.0;
}


// ********* Destructor ********************************************
PuffingGAM::~PuffingGAM()
{
	//if(this->SignalsInputInterface != NULL) delete[] this->SignalsInputInterface ;
}


//{ ********* Initialise the module ********************************
bool PuffingGAM::Initialise(ConfigurationDataBase& cdbData){
   
	CDBExtended cdb(cdbData);

	int i;

/*	if(!cdb.ReadInt32(UARTPortAddress, "UARTPortAddress"))
	{
		AssertErrorCondition(InitialisationError,"PuffingGAM::Initialise: %s Could not UARTPortAddress",this->Name());
		return False;
	}
	else {
		this->FaUART = new UFSerialUART(UARTPortAddress);
		AssertErrorCondition(Information,"PuffingGAM::Initialise: UARTPortAddress = %X",this->UARTPortAddress);
	}
	// Check port initialisation
	if(this->FaUART->IsUARTInitialised())
		AssertErrorCondition(Information, "PuffingGAM::Initialise: %s, Using UART port address %d.",this->Name(), this->FaUART->GetUARTPortAddress());
	else
	{
		AssertErrorCondition(InitialisationError, "PuffingGAM::Initialise: %s failed get permission to use the UART at address %d.", this->Name(),this->FaUART->GetUARTPortAddress());
		return False;
	}
	// Configure the UART
	if(!this->ConfigureUART())
	{
		AssertErrorCondition(InitialisationError, "PuffingGAM::Initialise: %s failed to configure the UART.", this->Name());
		return False;
	}
*/
	if(!cdb.ReadFloat(PointOfZero, "PointOfZero"))
	{
		AssertErrorCondition(InitialisationError,"PowerSupplyCommunicatorGAM::Initialise: %s PointOfZero",this->Name());
		return False;
	}
	else AssertErrorCondition(Information,"PowerSupplyCommunicatorGAM::Initialise: PointOfZero = %f",PointOfZero);
	if(!cdb.ReadFloat(Step, "Step"))
	{
		AssertErrorCondition(InitialisationError,"PowerSupplyCommunicatorGAM::Initialise: %s Step",this->Name());
		return False;
	}
	else AssertErrorCondition(Information,"PowerSupplyCommunicatorGAM::Initialise: Step = %f",Step);
	if(!cdb.ReadInt32(usecGlobalPeriod, "usecGlobalPeriod"))
	{
		AssertErrorCondition(InitialisationError,"PowerSupplyCommunicatorGAM::Initialise: %s usecGlobalPeriod",this->Name());
		return False;
	}
	else AssertErrorCondition(Information,"PowerSupplyCommunicatorGAM::Initialise: usecGlobalPeriod = %d",usecGlobalPeriod);



//	sleep(3);
// Create the signal interfaces
	if(!AddInputInterface(this->SignalsInputInterface, "PuffingGAMInputInterface"))
	{
		AssertErrorCondition(InitialisationError, "PuffingGAM::Initialise: %s failed to add the PuffingGAMInputInterface", this->Name());
		return False;
	}

//	INPUT SIGNALS (interface)
	if(!cdb->Move("input_signals"))
	{
		AssertErrorCondition(InitialisationError,"PuffingGAM::Initialise: %s Could not move to \"input_signals\"",this->Name());
		return False;
	}
		int number_of_signals_to_read = 3;
		FString *CDB_move_to;
		FString *SignalType;
		CDB_move_to = new FString[number_of_signals_to_read];
		SignalType = new FString[number_of_signals_to_read];
		CDB_move_to[0].Printf("input_puffing");
		CDB_move_to[1].Printf("GlobalTime");
		CDB_move_to[2].Printf("discharge_status");
		for (i=0;i<number_of_signals_to_read;i++){
			
			if(!cdb->Move(CDB_move_to[i].Buffer()))
			{
				AssertErrorCondition(InitialisationError,"PuffingGAM::Initialise: %s Could not move to \"%s\"",this->Name(),CDB_move_to[i].Buffer());
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
				AssertErrorCondition(Information,"PuffingGAM::Initialise: Added signal = %s", SignalName.Buffer());
				
				if(!this->SignalsInputInterface->AddSignal(SignalName.Buffer(), SignalType[i].Buffer()))
				{
					AssertErrorCondition(InitialisationError,"PuffingGAM::Initialise: %s failed to add signal", this->Name());
					return False;
				}
			}
			
			
			cdb->MoveToFather();
		}

	cdb->MoveToFather();


	return True;
}
//} ******************************************************************



//{ ********* Execute the module functionalities *******************
bool PuffingGAM::Execute(GAM_FunctionNumbers functionNumber){
 
	InputInterfaceStruct *inputstruct = (InputInterfaceStruct *) this->SignalsInputInterface->Buffer();
	this->SignalsInputInterface->Read();
//	AssertErrorCondition(InitialisationError,"PuffingGAM:: %s inputstruct = %f %d %d ",this->Name(), inputstruct[0].SendValue, inputstruct[0].usecTime, inputstruct[0].DischargeStatus);

/*
	usectimecopy = inputstruct[0].usecTime;
	CurrentToSendCopy = (float) inputstruct[0].SendValue;


	if(functionNumber == GAMOnline){
		
		if (inputstruct[0].DischargeStatus >=0 ){
	
			this->GetMessages();
			
			
				switch(functionNumber){
					// Called once to verify if the parameter configuration is acceptable
					case GAMCheck:
					break; // GAMCheck

					// Called once before going online
					case GAMPrepulse:

						this->communicatorOnlineStage = FA_COMMUNICATOR_ONLINE_WAIT_CODAC_TRIGGER;

						this->communicatorOnlineIdleCount = 0;
						this->communicatorOnlineWaitTriggerCount = 0;
						this->communicatorOnlineDischargeCount = 0;
						this->communicatorOnlineStopCount = 0;
						this->communicatorOnlineErrorCount = 0;

						PlasmaEnded = False;
					break; // GAMPrepulse

					// Called continuously when online after data has been read
					case GAMOnline:

						if(!this->CommunicatorOnline())
							return False;

					break; // GAMOnline

					// Called once before going offline
					case GAMPostpulse:

//						this->ResetTrigger();

					break; // GAMPostpulse

					// Called continuously while offline
					case GAMOffline:

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

		}
	}
*/

	return True;		
}


// Behaviour for the communicator
bool PuffingGAM::CommunicatorOffline(){
	return True;
}


bool PuffingGAM::CommunicatorOnline(){
	
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

/*		TODO:compatibilizar isto com a minha gam
		case FA_COMMUNICATOR_ONLINE_ERROR:

			// Trigger Plasma Ended flag
			plasmaEndedTriggerOutput = (int *) this->PlasmaEndOutputInterface->Buffer();
			*plasmaEndedTriggerOutput = 1;
			this->PlasmaEndOutputInterface->Write();
			return False;

			break;
*/
		default:
			break;
	}

	return True;
}

// Configure the FA UART
bool PuffingGAM::ConfigureUART(){
	
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
void PuffingGAM::SendMessage(unsigned char packet1, unsigned char packet2){
	// Send message packets
	this->FaUART->SendValue(packet1);
	this->FaUART->SendValue(packet2);

	// Update sent counters
	this->statisticsSentPackets += 2;
	this->statisticsSentMessages++;
}
// Create/Decode current packets
bool PuffingGAM::CreateCurrentPackets(float current, unsigned char &packet1, unsigned char &packet2){

	// Calculate the point in the scale of the current
	short pointOfCurrent = (short)(this->PointOfZero + current / this->Step);

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
void PuffingGAM::GetMessages(){
	
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
		//AssertErrorCondition(InitialisationError,"Packet received (%d): 0x%02x   %d", packetCounter, packet, packet);


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

bool PuffingGAM::DecodeCurrentPackets(float &current, unsigned char packet1, unsigned char packet2){
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
	current = ((float) pointOfCurrent - this->PointOfZero) * this->Step;

	return True;
}


// Implement message interpretation functionalities
void PuffingGAM::InterpretMessage(unsigned char packet1, unsigned char packet2){
	
	// Increment log pointer
	this->logReceivedPointer++;
	if(this->logReceivedPointer >= NLOGMESSAGES)
		this->logReceivedPointer = 0;

	this->logTimeReceivedMessages[this->logReceivedPointer] = this->usectimecopy;

	// Operation Started message
	if((packet1 == FA_STARTED_MESSAGE_1) && (packet2 == FA_STARTED_MESSAGE_2))
	{
		#if defined(__FA_COM_LOG_RECEIVED_MESSAGES)
			AssertErrorCondition(__FA_COM_LOG_LEVEL, "[FACom] Operation Started Message");
		#endif

		this->isStarted = True;
		this->statisticsStartOk++;
		this->logReceivedMessages[this->logReceivedPointer] = LOG_STARTED;

		return;
	}

	// Operation Stopped message
	if((packet1 == FA_STOPPED_MESSAGE_1) && (packet2 == FA_STOPPED_MESSAGE_2)){

		#if defined(__FA_COM_LOG_RECEIVED_MESSAGES) 
			AssertErrorCondition(__FA_COM_LOG_LEVEL, "[FACom] Operation Stopped Message");
		#endif

		this->isStarted = False;
		this->statisticsStopped++;
		this->logReceivedMessages[this->logReceivedPointer] = LOG_STOPPED;

		return;
	}

	// Charged message
	if((packet1 == FA_CHARGED_MESSAGE_1) && (packet2 == FA_CHARGED_MESSAGE_2)){
		
		#if defined(__FA_COM_LOG_RECEIVED_MESSAGES) 
			AssertErrorCondition(__FA_COM_LOG_LEVEL, "[FACom] Charged Message");
		#endif

		this->isCharged = True;
		this->statisticsCharged++;
		this->logReceivedMessages[this->logReceivedPointer] = LOG_CHARGED;

		return;
	}

	// Not charged message
	if((packet1 == FA_NOT_CHARGED_MESSAGE_1) && (packet2 == FA_NOT_CHARGED_MESSAGE_2)){
		
		#if defined(__FA_COM_LOG_RECEIVED_MESSAGES) 
			AssertErrorCondition(__FA_COM_LOG_LEVEL,"[FACom] Not Charged Message");
		#endif

		this->isCharged = False;
		this->statisticsNotCharged++;
		this->logReceivedMessages[this->logReceivedPointer] = LOG_NOT_CHARGED;

		return;
	}

	// Temperature Fault message
	if((packet1 == FA_TEMPERATURE_FAULT_MESSAGE_1) && (packet2 == FA_TEMPERATURE_FAULT_MESSAGE_2)){
		
		#if defined(__FA_COM_LOG_RECEIVED_MESSAGES) 
			AssertErrorCondition(__FA_COM_LOG_LEVEL, "[FACom] Temperature Fault Message");
		#endif

		this->temperatureFailure = True;
		this->statisticsTemperatureFault++;
		this->logReceivedMessages[this->logReceivedPointer] = LOG_TEMPERATURE_FAULT;

		return;
	}


	// 24V Failure message
	if((packet1 == FA_24V_FAULT_MESSAGE_1) && (packet2 == FA_24V_FAULT_MESSAGE_2)){

		#if defined(__FA_COM_LOG_RECEIVED_MESSAGES)
			AssertErrorCondition(__FA_COM_LOG_LEVEL, "[FACom] 24V Failure Message");
		#endif

		this->voltageFailure = True;
		this->statisticsVoltageFault++;
		this->logReceivedMessages[this->logReceivedPointer] = LOG_24V_FAULT;

		return;
	}

	// Stop Error message
	if((packet1 == FA_STOP_ERROR_MESSAGE_1) && (packet2 == FA_STOP_ERROR_MESSAGE_2)){

		#if defined(__FA_COM_LOG_RECEIVED_MESSAGES)
			AssertErrorCondition(__FA_COM_LOG_LEVEL, "[FACom] Stop Error Message");
		#endif

		this->stopFailure = True;
		this->statisticsStopFault++;
		this->logReceivedMessages[this->logReceivedPointer] = LOG_STOP_FAULT;

		return;
	}


	// Communication Error message
	if((packet1 == FA_COMM_ERROR_MESSAGE_1) && (packet2 == FA_COMM_ERROR_MESSAGE_2)) {

		#if defined(__FA_COM_LOG_RECEIVED_MESSAGES)
			AssertErrorCondition(__FA_COM_LOG_LEVEL, "[FACom] Communication Error Message");
		#endif

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
	#if defined(__FA_COM_LOG_RECEIVED_MESSAGES)
		AssertErrorCondition(__FA_COM_LOG_LEVEL, "[FACom] Other message received: 0x%02x  0x%02x", packet1, packet2);
	#endif
}


bool PuffingGAM::ProcessHttpMessage(HttpStream &hStream){
	
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
		hmStream.SSPrintf(HtmlTagStreamMode, "br><br>UARTPortAddress = %d\n\
		<br><br",UARTPortAddress);
	}
	hmStream.SSPrintf(HtmlTagStreamMode, "/form");
	
	hmStream.SSPrintf(HtmlTagStreamMode, "/body>\n</html");
	hStream.SSPrintf("OutputHttpOtions.Content-Type","text/html;charset=utf-8");
	hStream.WriteReplyHeader(True);

	return True;		
}

