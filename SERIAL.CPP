// --------------------------------------------------------------------------
// --
// -- Serial - Methods and functions.
// --
// -- Ver. 1 - Hard-code support for COM1 and COM2, only.
// --
// --------------------------------------------------------------------------

// --------------------------------------------------------------------------
// -- Includes
// --------------------------------------------------------------------------

#include <stdlib.h>
#include <string.h>
#include <dos.h>
#include "defines.h"
#include "fbuffer.h"
#include "serial-i.h"
#include "serial.h"

// --------------------------------------------------------------------------
// -- Defines
// --------------------------------------------------------------------------

// --------------------------------------------------------------------------
// -- Data
// --------------------------------------------------------------------------

// -- Basic vars ------------------------
int            portbaseCOM1    = 0;
int  		   portbaseCOM2	   = 0;

// -- Interrupt vectors to save ---------
void           interrupt(*origVectors[2])(...);

// -- Buffers ---------------------------
FBuffer 	   *rxCOM1Buffer;
FBuffer        *txCOM1Buffer;
FBuffer        *rxCOM2Buffer;
FBuffer		   *txCOM2Buffer;


// --------------------------------------------------------------------------
// -- Functions
// --------------------------------------------------------------------------

// -- Speed is REALLY important in these.   so...  we will trade size
// -- for speed.  There is a handler for each port for hardware flow
// -- control and no hardware flow control.

// -- COM1 interrupt handler w/Hardware Flow Control --------------------
void interrupt com_int1HFC(...)
{
	unsigned char  IIRb;
	unsigned char  MSRb;
	unsigned char  characterb;

	Boolean		   EOISent;

	// Re-enable interrupts.  The serial chip wont interrupt us until
	// we've cleaned out it's pending.
	enable();

	// OK, find which kind of interrupt caused this and switch it
	// Do this as long as there are no interrupts pending
	while( !( (IIRb = inportb(portbaseCOM1 + IIR)) & IIR_PENDING ) ) {

	   IIRb &= IIR_MASK;
	   switch(IIRb) {

		  // --- Modem Status register changed
		  case IIR_MSR:
			   MSRb = inportb(portbaseCOM1 + MSR);

			   // See if we just got a Clear to Send
			   // If so, try and send a character if there is one queued
			   // AND the transmitter hold register is clear
			   if (MSRb & CTS_ASSERTED) {
				  if (txCOM1Buffer->Out(characterb) == FB_NO_ERROR) {
					 if (inportb(portbaseCOM1 + LSR) & XMTRDY)
						outportb(portbaseCOM1 + TXR, characterb);
				  } else
					 outportb(portbaseCOM1 + MCR, FLOW_RNR); // Drop DTR/RTS
			   }

			   break;

		  // --- The Transmitter went empty.  See if we can send more.
		  case IIR_TX:
			   // Send if character is availible AND CTS is raised
			   if (txCOM1Buffer->Out(characterb) == FB_NO_ERROR) {
				   if (inportb(portbaseCOM1 + MSR) & CTS)
					  outportb(portbaseCOM1 + TXR, characterb);
			   } else
				  outportb(portbaseCOM1 + MCR, FLOW_RNR);
			   break;

		  // --- An incoming byte.
		  case IIR_RX:
			   // Cool, a byte.  Do error processing later...
			   rxCOM1Buffer->In(characterb);
			   break;

		  // --- Line error.  Probibly a framing error.
		  case IIR_ERROR:
			   break;
		  // --- A world of hurt!

		  default:
			   break;

	   } // end case

	   // Send the non-specific EOI the first go, only
	   if (!EOISent) {
		  outportb(ICR, EOI);
		  EOISent = TRUE;
	   };

	} // end while

}

// -- COM1 interrupt handler --------------------
void interrupt com_int1(...)
{
	unsigned char  IIRb;
	unsigned char  MSRb;
	unsigned char  characterb;

	Boolean		   EOISent;

	// Re-enable interrupts.  The serial chip wont interrupt us until
	// we've cleaned out it's pending.
	enable();

	// OK, find which kind of interrupt caused this and switch it
	// Do this as long as there are no interrupts pending
	while( !( (IIRb = inportb(portbaseCOM1 + IIR)) & IIR_PENDING ) ) {

	   IIRb &= IIR_MASK;
	   switch(IIRb) {

		  // --- Modem Status register changed
		  case IIR_MSR:
			   break;

		  // --- The Transmitter went empty.  See if we can send more.
		  case IIR_TX:
			   // Send if character is availible AND CTS is raised
			   if (txCOM1Buffer->Out(characterb) == FB_NO_ERROR)
				  outportb(portbaseCOM1 + TXR, characterb);
			   else
				  outportb(portbaseCOM1 + MCR, FLOW_RNR);
			   break;

		  // --- An incoming byte.
		  case IIR_RX:
			   // Cool, a byte.  Do error processing later...
			   rxCOM1Buffer->In(characterb);
			   break;

		  // --- Line error.  Probibly a framing error.
		  case IIR_ERROR:
			   break;

		  // --- A world of hurt!
		  default:
			   break;

	   } // end case

	   // Send the non-specific EOI the first go, only
	   if (!EOISent) {
		  outportb(ICR, EOI);
		  EOISent = TRUE;
	   };

	} // end while

}

// -- COM2 interrupt handler w/Hardware Flow Control --------------------
void interrupt com_int2HFC(...)
{
	unsigned char  IIRb;
	unsigned char  MSRb;
	unsigned char  characterb;

	Boolean		   EOISent;

	// Re-enable interrupts.  The serial chip wont interrupt us until
	// we've cleaned out it's pending.
	enable();

	// OK, find which kind of interrupt caused this and switch it
	// Do this as long as there are no interrupts pending
	while( !( (IIRb = inportb(portbaseCOM2 + IIR)) & IIR_PENDING ) ) {

	   IIRb &= IIR_MASK;
	   switch(IIRb) {

		  // --- Modem Status register changed
		  case IIR_MSR:
			   MSRb = inportb(portbaseCOM2 + MSR);

			   // See if we just got a Clear to Send
			   // If so, try and send a character if there is one queued
			   // AND the transmitter hold register is clear
			   if (MSRb & CTS_ASSERTED) {
				  if (txCOM2Buffer->Out(characterb) == FB_NO_ERROR) {
					 if (inportb(portbaseCOM2 + LSR) & XMTRDY)
						outportb(portbaseCOM2 + TXR, characterb);
				  } else
					 outportb(portbaseCOM2 + MCR, FLOW_RNR); // Drop RTS
			   }

			   break;

		  // --- The Transmitter went empty.  See if we can send more.
		  case IIR_TX:
			   // Send if character is availible AND CTS is raised
			   if (txCOM2Buffer->Out(characterb) == FB_NO_ERROR) {
				   if (inportb(portbaseCOM2 + MSR) & CTS)
					  outportb(portbaseCOM2 + TXR, characterb);
			   } else
				  outportb(portbaseCOM2 + MCR, FLOW_RNR);
			   break;

		  // --- An incoming byte.
		  case IIR_RX:
			   // Cool, a byte.  Do error processing later...
			   rxCOM2Buffer->In(characterb);
			   break;

		  // --- Line error.  Probibly a framing error.
		  case IIR_ERROR:
			   break;

		  // --- A world of hurt!
		  default:
			   break;

	   } // end case

	   // Send the non-specific EOI the first go, only
	   if (!EOISent) {
		  outportb(ICR, EOI);
		  EOISent = TRUE;
	   };

	} // end while

}

// -- COM2 interrupt handler --------------------
void interrupt com_int2(...)
{
	unsigned char  IIRb;
	unsigned char  MSRb;
	unsigned char  characterb;

	Boolean		   EOISent;

	// Re-enable interrupts.  The serial chip wont interrupt us until
	// we've cleaned out it's pending.
	enable();

	// OK, find which kind of interrupt caused this and switch it
	// Do this as long as there are no interrupts pending
	while( !( (IIRb = inportb(portbaseCOM2 + IIR)) & IIR_PENDING ) ) {

	   IIRb &= IIR_MASK;
	   switch(IIRb) {

		  // --- Modem Status register changed
		  case IIR_MSR:
			   break;

		  // --- The Transmitter went empty.  See if we can send more.
		  case IIR_TX:
			   // Send if character is availible AND CTS is raised
			   if (txCOM2Buffer->Out(characterb) == FB_NO_ERROR)
				  outportb(portbaseCOM2 + TXR, characterb);
			   else
				  outportb(portbaseCOM2 + MCR, FLOW_RNR);
			   break;

		  // --- An incoming byte.
		  case IIR_RX:
			   // Cool, a byte.  Do error processing later...
			   rxCOM2Buffer->In(characterb);
			   break;

		  // --- Line error.  Probibly a framing error.
		  case IIR_ERROR:
			   break;

		  // --- A world of hurt!
		  default:
			   break;

	   } // end case

	   // Send the non-specific EOI the first go, only
	   if (!EOISent) {
		  outportb(ICR, EOI);
		  EOISent = TRUE;
	   };

	} // end while

}


// --------------------------------------------------------------------------
// -- Private Methods
// --------------------------------------------------------------------------

// --- Set the port -----------------
SS_ERROR_T Serial::setPort(SS_PORT_T port)
{
	int      offset;
	int far *addr;

	switch (port)
	{
	  case SS_PORT_1 : // Set the offsets and attach the FIFOs
				  offset = 0x0000;
				  txCOM1Buffer = &txFIFO;
				  rxCOM1Buffer = &rxFIFO;
				  break;

	  case SS_PORT_2:
				  offset = 0x0002;
				  txCOM1Buffer = &txFIFO;
				  rxCOM1Buffer = &rxFIFO;
				  break;

	  default   : return (SS_ERROR);
	}

	addr = (int far *)MK_FP(0x0040, offset);
	if (*addr == NULL) return (SS_ERROR);
	portbase = *addr;

	return (SS_NO_ERROR);
}

// --- Set the line speed
SS_ERROR_T Serial::setSpeed(SS_BPS_T speed)
{
	char			item;
	unsigned int	divisor;
	unsigned int    numeric;

	switch( speed ) {
	   case SS_BPS_300 :	 numeric = 300;
							 break;
	   case SS_BPS_2400:	 numeric = 2400;
							 break;
	   case SS_BPS_9600:	 numeric = 9600;
							 break;
	   case SS_BPS_19200:	 numeric = 19200;
							 break;
	   case SS_BPS_38400:	 numeric = 38400;
							 break;
	   default:				 return SS_ERROR;

	}

	divisor = (int) (115200L/numeric);

	disable();
	item = inportb(portbase + LCR);
	outportb(portbase + LCR, (item | 0x80));

	outportb(portbase + DLL, (divisor & 0x00FF));
	outportb(portbase + DLH, ((divisor >> 8) & 0x00FF));

	outportb(portbase + LCR, item);
	enable();

	return (SS_NO_ERROR);
}

/* Set other communications parameters */
SS_ERROR_T Serial::setFraming(SS_PARITY_T parity, SS_BITS_T bits,
							  SS_SBITS_T stopBit                  )
{
    int                setting;

	switch (bits) {
	   case SS_BITS_5:		setting = 0x00;
							break;
	   case SS_BITS_7:		setting = 0x02;
							break;
	   case SS_BITS_8:		setting = 0x03;
							break;
	   default       :      return(SS_ERROR);

	}

	switch (stopBit) {
	   case SS_SBITS_1  :  break;
	   case SS_SBITS_2  :  setting |= 0x04;
						   break;
	   default:			   return(SS_ERROR);
	}

	switch (parity) {
	   case SS_PARITY_NONE:  break;
	   case SS_PARITY_ODD :  setting |= 0x08;
							 break;
	   case SS_PARITY_EVEN:  setting |= 0x18;
							 break;
	   default:			     return(SS_ERROR);
	}

	disable();
	outportb(portbase + LCR, setting);
	enable();

	return (SS_NO_ERROR);
}

// --------------------------------------------------------------------------
// -- Public Methods
// --------------------------------------------------------------------------

// --- Port Parameters ------------------------------------------------------

// --- constructor
PortParameters::PortParameters(void) {

   bps 		= SS_BPS_DEFAULT;
   bits     = SS_BITS_DEFAULT;
   sBits    = SS_SBITS_DEFAULT;
   parity   = SS_PARITY_DEFAULT;

}

// -- incrementors
void PortParameters::bpsNext(   void) {

   if (bps == SS_BPS_MAX)
	  bps = SS_BPS_MIN;
   else
	  bps++;

}

void PortParameters::bitsNext(  void) {

   if (bits == SS_BITS_MAX)
	  bits = SS_BITS_MIN;
   else
	  bits++;
}

void PortParameters::sBitsNext( void) {

   if (sBits == SS_SBITS_MAX)
	  sBits = SS_SBITS_MIN;
   else
	  sBits++;

}

void PortParameters::parityNext(void) {

   if (parity == SS_PARITY_MAX)
	  parity = SS_PARITY_MIN;
   else
	  parity++;
}


// --- Serial ---------------------------------------------------------------

// --- Get a character out to the port
Serial& Serial::operator<<( char octet )
{

	// Stop the interrupts so the handlers dont monkey with the FIFOs
	disable();

	// Output pending?
	if( txFIFO.HasData() ) {

	   // We are already going.  So, just dump it in the FIFO and move on
	   txFIFO.In(octet);
	   enable();
	   return *this;

	}

	// No data!
	// If we are doing flow control, see if we need to wait for CTS
	if (flowControl) {
	   if(!(inportb(portbase + MSR) & CTS)) {

		   // Assert RTS/DTR and wait for interrupt (return, that is)
		   outportb(portbase + MCR, FLOW_RR);
		   enable();
		   return *this;

	   }
	} // end if flow control


	// Ok, finally, if the serial chip is not busy sending something,
	// Dump this char directly on it.  If we cant, put it in the FIFO
	if (inportb(LSR) & XMTRDY) 
	   outportb(portbase + TXR, octet);
	else
	   txFIFO.In(octet);

	enable();
	return *this;

}

// --- Send a whole string.  NULL terminated
Serial& Serial::operator<<( char *string )
{
	while (*string)
	{
	   (*this) << *string;
	   string++;
	}
	return *this;
}

// --- Get a char from the buffer.  Dump into an int.
Serial &Serial::operator>>( unsigned char &octet )
{
	disable();  // disable ints to prevent FIFO trouble
	if (rxFIFO.Out(octet) != FB_NO_ERROR) octet = SS_NO_CHARACTER;
	enable();
	return *this;
}

Serial::~Serial()
{
   if (up) lineDown();
}




// --- Constructor  ----------------------------------------------
Serial::Serial(SS_PORT_T port, PortParameters  portP,
			   Boolean  hwFlowControl) :
			   txFIFO(SS_BUFFER_SIZE),
			   rxFIFO(SS_BUFFER_SIZE)

{                                        
	// Setup the port we want to use
	enabled = FALSE;
	up      = FALSE;

	flowControl = hwFlowControl;
	thisPort	= port;

	if (setPort(port)   == SS_ERROR) return;
	if (setSpeed(portP.bps) == SS_ERROR) return;
	if (setFraming(portP.parity, portP.bits,
				   portP.sBits               ) == SS_ERROR ) return;

	enabled = TRUE;

}


// --- Bring the line up -------------------
SS_ERROR_T Serial::lineUp(void) {

   int	IRQ;
   int  item;

   // Ok, determine port and grab the vector
   switch( thisPort ) {

	  case SS_PORT_1: IRQ = IRQ4;
					  origVectors[0] = getvect(0x0C);
					  if(flowControl)
						 setvect(0x0C,com_int1HFC);
					  else
						 setvect(0x0C,com_int1);
					  break;

	  case SS_PORT_2: IRQ = IRQ3;
					  origVectors[1] = getvect(0x0B);
					  if(flowControl)
						 setvect(0x0B,com_int2HFC);
					  else
						 setvect(0x0B,com_int2);
					  break;

	  default:		 return(SS_BAD_PORT);
   }

   // Enable interrupts
   disable();

   // Let the modem control and interrupt control know what we want
   item = inportb(portbase + MCR) | MC_INT;
   outportb(portbase + MCR, item);
   outportb(portbase + IER, ALL_INT);

   // Let the PIC know which to kick
   item = inportb(IMR) & IRQ;
   outportb( IMR, item );

   enable();

   up = TRUE;

   return SS_NO_ERROR;

}

// --- Bring the line Down ---------------------
SS_ERROR_T Serial::lineDown(void) {

   int	IRQ;
   int  item;

   // Ok, determine port and replace the vector
   switch( thisPort ) {

	  case SS_PORT_1: IRQ = IRQ4;
					  setvect(0x0C, origVectors[0]);
					  break;

	  case SS_PORT_2: IRQ = IRQ3;
					  setvect(0x0B, origVectors[1]);
					  break;

	  default:		 return(SS_BAD_PORT);
   }

   // Enable interrupts
   disable();

   // Let the PIC know which to stop handling
   item = inportb(IMR) | ~IRQ;
   outportb( IMR, item );

   // Let the modem control and interrupt control know what we want
   outportb(portbase + IER, 0);
   item = inportb(portbase + MCR) & ~MC_INT;
   outportb(portbase + MCR, item);

   enable();

   up = FALSE;

   return SS_NO_ERROR;


}

// --------------------------------------------------------------------------
// -- Functions
// --------------------------------------------------------------------------

