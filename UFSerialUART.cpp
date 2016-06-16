#include "UFSerialUART.h"

#include <stdio.h>


// ******************************************************************
// Default constructor/destructor
// ******************************************************************
UFSerialUART::UFSerialUART(unsigned int address)
{
	this->UARTPortAddress = address;
	this->NineBitMode = false;

	//
	// Get permission from the kernel to access the UART ports
	//
#if !defined(_RTAI)
	if(ioperm(this->UARTPortAddress, 8, 1))
		this->KernelPermission = false;
	else
		this->KernelPermission = true;
#else
		this->KernelPermission = true;
#endif
}


UFSerialUART::~UFSerialUART()
{
	//
	// Release permission from the kernel to access the UART ports
	//
#if !defined(_RTAI)
	ioperm(this->UARTPortAddress, 8, 0);
#endif
}
// ******************************************************************


// ******************************************************************
// Get UART initialisation status
// ******************************************************************
bool UFSerialUART::IsUARTInitialised()
{
	return this->KernelPermission;
}
// ******************************************************************


// ******************************************************************
// Set/Get UART port address
// ******************************************************************
void UFSerialUART::SetUARTPortAddress(unsigned int address)
{
	this->UARTPortAddress = address;
}


unsigned int UFSerialUART::GetUARTPortAddress()
{
	return this->UARTPortAddress;
}
// ******************************************************************


// ******************************************************************
// DLAB Set/Clear
// ******************************************************************
void UFSerialUART::SetDLAB()
{
	unsigned char LcrValue = inb(LCR);
	outb(LcrValue | 0x80, LCR);
}


void UFSerialUART::ClearDLAB()
{
	unsigned char LcrValue = inb(LCR);
	outb(LcrValue & 0x7F, LCR);
}
// ******************************************************************


// ******************************************************************
// Set Frequency Divider
// IMPORTANT: only call when DLAB = 0
// ******************************************************************
void UFSerialUART::SetFrequencyDivider(unsigned short newDivider)
{
	unsigned char Low = (unsigned char)(newDivider & 0x00FF);
	unsigned char High = (unsigned char)((newDivider & 0xFF00) >> 8);

	SetDLAB();
	outb(Low, DLL);
	outb(High, DLM);
	ClearDLAB();
}
// ******************************************************************


// ******************************************************************
// Configure LCR: 8-bits, 1 stop bit, keep the rest of the byte
// TODO: erase this method
// ******************************************************************
void UFSerialUART::ConfigureLCR()
{
	unsigned char LcrValue = inb(LCR);
	LcrValue |= 0x03;
	LcrValue &= 0xFB;
	outb(LcrValue, LCR);
}
// ******************************************************************


// ******************************************************************
// Select operation mode
// ******************************************************************
void UFSerialUART::Select950Mode()
{
	unsigned char FcrValue = inb(FCR);
	FcrValue |= 0x01;
	outb(FcrValue, FCR);

	unsigned char LcrValue = inb(LCR);
	outb(0xBF, LCR);
	unsigned char EfrValue = inb(EFR);
	EfrValue |= 0x10;
	outb(LcrValue, LCR);
}
// ******************************************************************


// ******************************************************************
// Enable/Disable 9-bit mode
// ******************************************************************
void UFSerialUART::Enable9BitMode()
{
	outb(0x0D, SPR);
	outb(0x01, ICR);

	this->NineBitMode = true;
}


void UFSerialUART::Disable9BitMode()
{
	outb(0x0D, SPR);
	outb(0x00, ICR);

	this->NineBitMode = false;
}
// ******************************************************************


// ******************************************************************
// Set Low/High Parity
// ******************************************************************
void UFSerialUART::DisableParity()
{
	unsigned char LcrValue = inb(LCR);
	outb(LcrValue & 0xF7, LCR);
}


void UFSerialUART::SetLowParity()
{
	unsigned char LcrValue = inb(LCR);
	outb(LcrValue | 0x38, LCR);
}


void UFSerialUART::SetHighParity()
{
	unsigned char LcrValue = inb(LCR);
	LcrValue |= 0x28;
	LcrValue &= 0xEF;
	outb(LcrValue, LCR);
}


void UFSerialUART::SetEvenParity()
{
	unsigned char LcrValue = inb(LCR);
	LcrValue |= 0x18;
	LcrValue &= 0xDF;
	outb(LcrValue, LCR);
}


void UFSerialUART::SetOddParity()
{
	unsigned char LcrValue = inb(LCR);
	LcrValue |= 0x08;
	LcrValue &= 0xCF;
	outb(LcrValue, LCR);
}
// ******************************************************************


// ******************************************************************
// Set Data Length
// ******************************************************************
void UFSerialUART::SetFiveBitsLength()
{
	unsigned char LcrValue = inb(LCR);
	LcrValue &= 0xFC;
	outb(LcrValue, LCR);
}


void UFSerialUART::SetSixBitsLength()
{
	unsigned char LcrValue = inb(LCR);
	LcrValue &= 0xFD;
	LcrValue |= 0x01;
	outb(LcrValue, LCR);
}


void UFSerialUART::SetSevenBitsLength()
{
	unsigned char LcrValue = inb(LCR);
	LcrValue &= 0xFE;
	LcrValue |= 0x02;
	outb(LcrValue, LCR);
}


void UFSerialUART::SetEightBitsLength()
{
	unsigned char LcrValue = inb(LCR);
	LcrValue |= 0x03;
	outb(LcrValue, LCR);
}
// ******************************************************************


// ******************************************************************
// Enable/disable usage of FIFOs
// ******************************************************************
void UFSerialUART::EnableFifos()
{
	outb(0xF7, FCR);
}


void UFSerialUART::DisableFifos()
{
	outb(0xF6, FCR);
}
// ******************************************************************


// ******************************************************************
// Select number of stop bits
// ******************************************************************
void UFSerialUART::SelectOneStopBit()
{
	unsigned char LcrValue = inb(LCR);
	LcrValue &= 0xFB;
	outb(LcrValue, LCR);
}


void UFSerialUART::SelectTwoStopBits()
{
	unsigned char LcrValue = inb(LCR);
	LcrValue |= 0x04;
	outb(LcrValue, LCR);
}
// ******************************************************************


// ******************************************************************
// Clear FIFOs
// Note: I don't know if these methods work with the 950 UARTs
// ******************************************************************
void UFSerialUART::ClearTxFifo()
{
	unsigned char FcrValue = inb(FCR);
	outb(FcrValue | 0x04, FCR);
}


void UFSerialUART::ClearRxFifo()
{
	unsigned char FcrValue = inb(FCR);
	outb(FcrValue | 0x02, FCR);
}
// ******************************************************************


// ******************************************************************
// Check if there is data waiting to be read
// ******************************************************************
bool UFSerialUART::IsDataAvailable()
{
	if((inb(LSR) & 0x01) == 0)
		return false;

	return true;
}
// ******************************************************************


// ******************************************************************
// Send/receive value
// ******************************************************************
bool UFSerialUART::SendValue(unsigned short value)
{
	// TODO: delete these comments
	//unsigned char result = inb(LSR);
	//if((result & 0x20))
	//	outb(value, THR);
	//else
	//	return -1;

	// Place the 9th bit
	if(this->NineBitMode)
	{
		if(value & 0x0100)
			outb(0x01, SPR);
		else
			outb(0x00, SPR);
	}
	outb((unsigned char)(value & 0x00FF), THR);

	return true;
}


// TODO:
bool UFSerialUART::ReadValue(unsigned short &value)
{
	bool Is9BitAsserted = false;

	//
	// Get the 9th bit
	//
	if(this->NineBitMode)
	{
		if(inb(LSR) & 0x04)
			Is9BitAsserted = true;
		else
			Is9BitAsserted = false;
	}

	//
	// Read the remainder of the value
	//
	value = (unsigned short) inb(RHR);
	value &= 0x00FF;

	//
	// Mix the 9 bits
	//
	if(Is9BitAsserted)
		value |= 0x0100;

	return true;
}
// ******************************************************************
