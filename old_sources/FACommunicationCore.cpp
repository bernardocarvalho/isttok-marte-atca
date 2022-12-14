/*
 * File:   FACommunicationCore.cpp
 * Author: danielv
 *
 * Created on April 17, 2009, 11:46 AM
 */

#include "FACommunicationCore.h"


OBJECTLOADREGISTER(FACommunicationCore, "$Id: $")


// ******************************************************************
// Initialise the module
// ******************************************************************
bool FACommunicationCore::Initialise(ConfigurationDataBase& cdbData)
{
	CDBExtended cdb(cdbData);

	//
	// Call parent Initialise method
	//
	BasicGAM::Initialise(cdb);

	//
	// Create the FA serial UART object
	//
	int32 uartPortAddress;
	if(cdb.ReadInt32(uartPortAddress, "UARTPortAddress"))
		this->FaUART = new UFSerialUART(uartPortAddress);
	else
		this->FaUART = new UFSerialUART(0xD000);

	//
	// Check port initialisation
	//
	if(this->FaUART->IsUARTInitialised())
		AssertErrorCondition(Information,
							 "FACommunicationCore::Initialise: Using UART port address %d.",
							 this->FaUART->GetUARTPortAddress());
	else
	{
		AssertErrorCondition(InitialisationError,
							 "FACommunicationCore::Initialise: %s failed get permission to use the UART at address %d.",
							 this->Name(),
							 this->FaUART->GetUARTPortAddress());

		return False;
	}

	//
	// Configure the UART
	//
	if(!this->ConfigureUART())
	{
		AssertErrorCondition(InitialisationError,
							 "FACommunicationCore::Initialise: %s failed to configure the UART.",
							 this->Name());

		return False;
	}

	//
	// Read the calibration factors for the power supply
	//
	float zeroCurrent;
	if(cdb.ReadFloat(zeroCurrent, "PointOfZeroCurrent"))
		this->pointOfZeroCurrent = (double) zeroCurrent;
	else
		this->pointOfZeroCurrent = 511.5;
	AssertErrorCondition(Information,
						 "FAPowerSupplyCommunicatorGAM::Initialise: Point of zero current: %f",
						 this->pointOfZeroCurrent);
	float stepCurrent;
	if(cdb.ReadFloat(stepCurrent, "CurrentStep"))
		this->currentStep = (double) stepCurrent;
	else
		this->currentStep = 1.0f;
	AssertErrorCondition(Information,
						 "FAPowerSupplyCommunicatorGAM::Initialise: Current step: %f",
						 this->currentStep);

	return True;
}
// ******************************************************************


// ******************************************************************
// Execute the module functionalities
// ******************************************************************
bool FACommunicationCore::Execute(GAM_FunctionNumbers functionNumber)
{
	//
	// Call parent Execute method
	// (read and put the current time on the time output of the GAM)
	//
	BasicGAM::Execute(functionNumber);

	return True;
}
// ******************************************************************


// ******************************************************************
// Configure the FA UART
// ******************************************************************
bool FACommunicationCore::ConfigureUART()
{
	this->FaUART->Select950Mode();
	this->FaUART->Disable9BitMode();
	this->FaUART->SetFrequencyDivider(0x0001);
	this->FaUART->SetEightBitsLength();
	this->FaUART->SetOddParity();
	this->FaUART->SelectTwoStopBits();
	this->FaUART->EnableFifos();

	return True;
}
// ******************************************************************


// ******************************************************************
// Send a message to the FA
// ******************************************************************
void FACommunicationCore::SendMessage(unsigned char packet1, unsigned char packet2)
{
	//
	// Send message packets
	//
	this->FaUART->SendValue(packet1);
	this->FaUART->SendValue(packet2);

	//
	// Update sent counters
	//
	this->statisticsSentPackets += 2;
	this->statisticsSentMessages++;
}
// ******************************************************************


// ******************************************************************
// Get messages in the buffer
// ******************************************************************
void FACommunicationCore::GetMessages()
{
	unsigned int packetCounter = 0;
	unsigned short packet;

	while(this->FaUART->IsDataAvailable())
	{
		// Increment packet counter and check if maximum allowed packets were attained
		if(++packetCounter > FA_COMMUNICATION_MAX_PACKETS)
			break;

		// DEBUG
		//AssertErrorCondition(InitialisationError,
		//			"Aqui");

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
				// TODO: Checking

				// Interpret message
				this->InterpretMessage((unsigned char) messagePackets[0],
									   (unsigned char) messagePackets[1]);
			}

			this->statisticsReceivedMessages++;

			// Clear packet counter and flags
			this->numMessagePackets = 0;
			this->receivedStartingPacket = False;
		}
	} // while(this->FaUART->IsDataAvailable())
}
// ******************************************************************


// ******************************************************************
// Interpret a received message
// Note: must be overriden by descendent classes to provide behaviour
// ******************************************************************
void FACommunicationCore::InterpretMessage(unsigned char packet1, unsigned char packet2)
{
}
// ******************************************************************


// ******************************************************************
// Create/Decode current packets
// ******************************************************************
bool FACommunicationCore::CreateCurrentPackets(double current, unsigned char &packet1, unsigned char &packet2)
{
	//
	// Calculate the point in the scale of the current
	//
	short pointOfCurrent = (short)(this->pointOfZeroCurrent + current / this->currentStep);

	//
	// Saturate current
	//
	if(pointOfCurrent < FA_SCALE_MIN)
		pointOfCurrent = FA_SCALE_MIN;
	if(pointOfCurrent > FA_SCALE_MAX)
		pointOfCurrent = FA_SCALE_MAX;

	//
	// Build packets
	//
	unsigned short pc = (unsigned short) pointOfCurrent;
	unsigned short nc = ~pc;
	packet1 = (unsigned char)(0x0000 |
				((nc & 0x03C0) >> 5) |
				((pc & 0x0007) << 5));
	packet2 = (unsigned char)(0x0001 |
				((pc & 0x03F8) >> 2));

	// TODO: Delete these lines
/*
	packet1 = packet2 = 0;
	packet1 = (unsigned char)(0x000	 |
				 (nc & 0x0040)		 |
				((nc & 0x0080) >> 2) |
				((nc & 0x0100) >> 4) |
				((nc & 0x0200) >> 6) |
				((pc & 0x0001) << 2) |
				 (pc & 0x0002)		 |
				((pc & 0x0004) >> 2));
	packet2 = (unsigned char)(0x080	 |
				((pc & 0x0008) << 3) |
				((pc & 0x0010) << 1) |
				((pc & 0x0020) >> 1) |
				((pc & 0x0040) >> 3) |
				((pc & 0x0080) >> 5) |
				((pc & 0x0100) >> 7) |
				((pc & 0x0200) >> 9));
*/

	return True;
}


bool FACommunicationCore::DecodeCurrentPackets(double &current, unsigned char packet1, unsigned char packet2)
{
	//
	// Validate packets
	//
	unsigned char validation = (packet2 & 0xF0) ^ ((packet1 & 0x1E) << 3);
	if(validation != 0xF0)
	{
		this->statisticsWrongMessagesReceived++;
		return False;
	}

	// TODO: Delete these lines
/*
	unsigned char validation = (packet1 & 0x78) >> 3 ^ (packet2 & 0x0F);
	if(validation != 0x0F)
		return False;
*/

	//
	// Decode packets
	//
	short pointOfCurrent = (short)(	(((unsigned short) packet1 & 0x00E0) >> 5)	|
									(((unsigned short) packet2 & 0x00FE) << 2));

	// TODO: Delete these lines
/*
	short pointOfCurrent = (short)(	(((unsigned short) packet2 & 0x0001) << 9)	|
									(((unsigned short) packet2 & 0x0002) << 7)	|
									(((unsigned short) packet2 & 0x0004) << 5)	|
									(((unsigned short) packet2 & 0x0008) << 3)	|
									(((unsigned short) packet2 & 0x0010) << 1)	|
									(((unsigned short) packet2 & 0x0020) >> 1)	|
									(((unsigned short) packet2 & 0x0040) >> 3)	|
									(((unsigned short) packet1 & 0x0001) << 2)	|
									 ((unsigned short) packet1 & 0x0002)		|
									(((unsigned short) packet1 & 0x0004) >> 2));
*/

	//
	// Calculate the current
	//
	current = ((double) pointOfCurrent - this->pointOfZeroCurrent) * this->currentStep;

	return True;
}
// ******************************************************************
