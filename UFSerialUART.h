/* 
 *   File:   UFSerialUart.h
 * Author: danielv
 *   Note: this class only works with the 950 UARTs
 *
 * Created on March 5, 2009, 9:30 AM
 */

#ifndef __UFSERIALUART_H__
#define	__UFSERIALUART_H__

#include <sys/io.h>


//
// UART registers
// Note: don't use outside this the UFSerialUART class
//
#define THR (this->UARTPortAddress) // Write
#define RHR (this->UARTPortAddress) // Read

#define IER (this->UARTPortAddress + 1)

#define ISR (this->UARTPortAddress + 2) // Read
#define FCR (this->UARTPortAddress + 2) // Write

#define LCR (this->UARTPortAddress + 3)
#define MCR (this->UARTPortAddress + 4)
#define LSR (this->UARTPortAddress + 5)
#define MSR (this->UARTPortAddress + 6)
#define SPR (this->UARTPortAddress + 7)

#define DLL (this->UARTPortAddress) // DLAB = 1 (LCR[7])
#define DLM (this->UARTPortAddress + 1) // DLAB = 1

#define EFR (this->UARTPortAddress + 2)
#define ICR (this->UARTPortAddress + 5)


class UFSerialUART
{
private:
	unsigned int UARTPortAddress;
	bool NineBitMode;
	bool KernelPermission;

private:

	// DLAB Set/Clear
	inline void SetDLAB();
	inline void ClearDLAB();

public:

	// Default constructor/destructor
	UFSerialUART(unsigned int address);
	virtual ~UFSerialUART();

	// Get UART initialisation status
	bool IsUARTInitialised();

	// Set/Get UART port address
	void SetUARTPortAddress(unsigned int address);
	unsigned int GetUARTPortAddress();

	// Clear FIFOs
	void ClearTxFifo();
	void ClearRxFifo();

	// Set Frequency Divider
	void SetFrequencyDivider(unsigned short newDivider);

	// Set Parity
	void DisableParity();
	void SetLowParity();
	void SetHighParity();
	void SetEvenParity();
	void SetOddParity();

	// Set Data Length
	void SetFiveBitsLength();
	void SetSixBitsLength();
	void SetSevenBitsLength();
	void SetEightBitsLength();

	// Enable/disable usage of FIFOs
	void EnableFifos();
	void DisableFifos();

	// Select number of stop bits
	void SelectOneStopBit();
	void SelectTwoStopBits();

	// Select operation mode
	void Select950Mode();

	// Enable/Disable 9-bit mode
	void Enable9BitMode();
	void Disable9BitMode();

	// Configure LCR: 8-bits, 1 stop bit, keep the rest of the byte
	// TODO: delete this method, it is not needed anymore
	void ConfigureLCR();

	// Check if there is data waiting to be read
	bool IsDataAvailable();

	// Send/receive value
	bool SendValue(unsigned short value);
	bool ReadValue(unsigned short &value);

};


#endif	/* __UFSERIALUART_H__ */

