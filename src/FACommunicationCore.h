

#ifndef __FACOMMUNICATIONCORE_H__
#define	__FACOMMUNICATIONCORE_H__


//#include "Level0.h"

#include "GAM.h"
#include "UFSerialUART.h"


#define FA_CHARGE_MESSAGE_1				0x6C
#define FA_CHARGE_MESSAGE_2				0x6D
#define FA_SHUTDOWN_MESSAGE_1			0x92
#define FA_SHUTDOWN_MESSAGE_2			0x93
#define FA_STARTOP_MESSAGE_1			0xFE
#define FA_STARTOP_MESSAGE_2			0xFF
#define FA_STOPOP_MESSAGE_1				0x00
#define FA_STOPOP_MESSAGE_2				0x01

#define FA_TEMPERATURE_FAULT_MESSAGE_1	0xB6
#define FA_TEMPERATURE_FAULT_MESSAGE_2	0xB7
#define FA_24V_FAULT_MESSAGE_1			0x48
#define FA_24V_FAULT_MESSAGE_2			0x49
#define FA_CHARGED_MESSAGE_1			0x6C
#define FA_CHARGED_MESSAGE_2			0x6D
#define FA_NOT_CHARGED_MESSAGE_1		0x92
#define FA_NOT_CHARGED_MESSAGE_2		0x93
#define FA_STARTED_MESSAGE_1			0xFE
#define FA_STARTED_MESSAGE_2			0xFF
#define FA_STOPPED_MESSAGE_1			0x00
#define FA_STOPPED_MESSAGE_2			0x01
#define FA_STOP_ERROR_MESSAGE_1			0x24
#define FA_STOP_ERROR_MESSAGE_2			0x25
#define FA_COMM_ERROR_MESSAGE_1			0xDA
#define FA_COMM_ERROR_MESSAGE_2			0xDB


#define FA_COMMUNICATION_MAX_PACKETS	4

#define FA_FRAMING_BIT_MASK				0x01


#define FA_SCALE_MIN					0
#define FA_SCALE_MAX					1023


#define LOG_CHARGE				1
#define LOG_SHUTDOWN			2
#define LOG_STARTOP				3
#define LOG_STOPOP				4
#define	LOG_TEMPERATURE_FAULT	5
#define LOG_24V_FAULT			6
#define LOG_CHARGED				7
#define LOG_NOT_CHARGED			8
#define LOG_STARTED				9
#define LOG_STOPPED				10
#define LOG_STOP_FAULT			11
#define LOG_COMMUNICATION_FAULT	12
#define LOG_CURRENT_VALUE		13


//OBJECT_DLL(FACommunicationCore)


class FACommunicationCore : public GAM, public HttpInterface
{
public:
	FACommunicationCore()
	{
		this->FaUART = NULL;
		this->receivedStartingPacket = False;
		this->numMessagePackets = 0;

		this->statisticsReceivedPackets = 0;
		this->statisticsSentPackets = 0;

		this->statisticsReceivedMessages = 0;
		this->statisticsSentMessages = 0;
		this->statisticsIncompleteMessages = 0;

		this->statisticsWrongMessagesReceived = 0;

		this->currentStep = 0.0;
		this->pointOfZeroCurrent = 0.0;
	}

protected:

	// Serial UART
	UFSerialUART *FaUART;


	// FA message packets
	bool receivedStartingPacket;
	int numMessagePackets;
	unsigned short messagePackets[2];


	// Communication statistics
	uint64 statisticsReceivedPackets;
	uint64 statisticsSentPackets;
	uint64 statisticsReceivedMessages;
	uint64 statisticsSentMessages;
	uint64 statisticsIncompleteMessages;
	uint64 statisticsWrongMessagesReceived;


	//
	// Power supply calibration
	//
	double currentStep;
	double pointOfZeroCurrent;


	//
	// Initialise the module
	//
	virtual bool Initialise(ConfigurationDataBase& cdbData);


	//
	// Execute the module functionalities
	//
	virtual bool Execute(GAM_FunctionNumbers functionNumber);


	//
	// Builds the HTTP page with information about the power supplies
	//
	virtual bool ProcessHttpMessage(HttpStream &hStream);


	// Configure the FA UART
	bool ConfigureUART();

	// Send a message to the FA
	void SendMessage(unsigned char packet1, unsigned char packet2);

	// Get messages in the buffer
	void GetMessages();

	// Interpret a received message
	// Note: must be overriden by descendent classes to provide behaviour
	virtual void InterpretMessage(unsigned char packet1, unsigned char packet2);

	// Create current packets
	bool CreateCurrentPackets(double current, unsigned char &packet1, unsigned char &packet2);

	// Decode current packets
	bool DecodeCurrentPackets(double &current, unsigned char packet1, unsigned char packet2);


	OBJECT_DLL_STUFF(FACommunicationCore)
};


#endif	/* __FACOMMUNICATIONCORE_H__ */

