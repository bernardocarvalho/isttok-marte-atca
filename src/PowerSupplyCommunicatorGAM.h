#ifndef _POWERSUPPLYCOMMUNICATORGAM_H
#define	_POWERSUPPLYCOMMUNICATORGAM_H


//#define __FA_COM_LOG_RECEIVED_MESSAGES
//#define ___FA_COM_LOG_SENT_MESSAGES

//#include <dirent.h>
#include "DDBInputInterface.h"
#include "DDBOutputInterface.h"
#include "GAM.h"
#include "HtmlStream.h"
#include "UFSerialUART.h"


#define FA_CHARGE_MESSAGE_1				0x6C
#define FA_CHARGE_MESSAGE_2				0x6D
#define FA_SHUTDOWN_MESSAGE_1			0x92
#define FA_SHUTDOWN_MESSAGE_2			0x93
#define FA_STARTOP_MESSAGE_1			0xFE
#define FA_STARTOP_MESSAGE_2			0xFF
#define FA_STOPOP_MESSAGE_1				0x00
#define FA_STOPOP_MESSAGE_2				0x01

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

// Logging #defines
//#define __FA_COM_LOG_RECEIVED_MESSAGES
//#define ___FA_COM_LOG_SENT_MESSAGES
#define __FA_COM_LOG_LEVEL	InitialisationError


// Communicator Online Stages
#define FA_COMMUNICATOR_ONLINE_IDLE					0
#define FA_COMMUNICATOR_ONLINE_WAIT_CODAC_TRIGGER	1
#define FA_COMMUNICATOR_ONLINE_DISCHARGE			2
#define FA_COMMUNICATOR_ONLINE_STOP_OPERATION		3
#define FA_COMMUNICATOR_ONLINE_ERROR				4

#define FA_COMMUNICATOR_MAXIMUM_ATTEMPTS			5


OBJECT_DLL(PowerSupplyCommunicatorGAM)


class PowerSupplyCommunicatorGAM : public GAM, public HttpInterface {
private:
	DDBInputInterface *SignalsInputInterface;
	DDBOutputInterface *SignalsOutputInterface;

	struct InputInterfaceStruct {
        float SendValue;
		int32 usecTime;
		int32 DischargeStatus;
	};
	
	struct OutputInterfaceStruct {
		int32 SoftStop;
		float ReceiveValue;
	};


	int32 usectimecopy;
	float CurrentToSendCopy;

	int32 UARTPortAddress;
	float PointOfZeroCurrent;
	float CurrentStep;
	int usecGlobalPeriod;

	bool view_input_variables;

	// Status
	bool temperatureFailure;
	bool voltageFailure;
	bool communicationFailure;
	bool stopFailure;
	bool isCharged;
	bool isStarted;
	
	bool PlasmaEnded;
	bool IsTriggered;

	// Communicator online behaviour
	int communicatorOnlineStage;
	int communicatorOnlineStartOperationAttempts;
	int communicatorOnlineStopOperationAttempts;

	int communicatorOnlineIdleCount;
	int communicatorOnlineWaitTriggerCount;
	int communicatorOnlineDischargeCount;
	int communicatorOnlineStopCount;
	int communicatorOnlineErrorCount;


	// Communication Statistics
	uint64 statisticsCurrentValue;
	uint64 statisticsTemperatureFault;
	uint64 statisticsCharged;
	uint64 statisticsNotCharged;
	uint64 statisticsVoltageFault;
	uint64 statisticsStartOk;
	uint64 statisticsStopped;
	uint64 statisticsStopFault;
	uint64 statisticsCommunicationFault;

	// Logging
#define NLOGMESSAGES	1000
	uint16 logReceivedMessages[NLOGMESSAGES];
	uint16 logSentMessages[NLOGMESSAGES];
	float logReceivedCurrents[NLOGMESSAGES];
	float logSentCurrents[NLOGMESSAGES];
	uint16 logTimeReceivedMessages[NLOGMESSAGES];
	uint16 logTimeSentMessages[NLOGMESSAGES];
	int logReceivedPointer;
	int logSentPointer;

	// Structures
	struct CurrentSignals
	{
		uint32 TimingSignal;
		float CurrentSignal;
	};

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


public:


	// Default constructor
	PowerSupplyCommunicatorGAM();

	// Destructor
	virtual ~PowerSupplyCommunicatorGAM();

	// Initialise the module
	virtual bool Initialise(ConfigurationDataBase& cdbData);

	// Execute the module functionalities
	virtual bool Execute(GAM_FunctionNumbers functionNumber);

	// Builds the HTTP page with information about the power supplies
	virtual bool ProcessHttpMessage(HttpStream &hStream);

	// Implement message interpretation functionalities
	void InterpretMessage(unsigned char packet1, unsigned char packet2);

	// Behaviour for the communicator
	bool CommunicatorOnline();
	bool CommunicatorOffline();

	// Configure the FA UART
	bool ConfigureUART();

	// Send a message to the FA
	void SendMessage(unsigned char packet1, unsigned char packet2);

	// Get messages in the buffer
	void GetMessages();

	// Create current packets
	bool CreateCurrentPackets(float current, unsigned char &packet1, unsigned char &packet2);

	// Decode current packets
	bool DecodeCurrentPackets(float &current, unsigned char packet1, unsigned char packet2);




	
	OBJECT_DLL_STUFF(PowerSupplyCommunicatorGAM)
};



#endif	


