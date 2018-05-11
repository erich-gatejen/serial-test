// --------------------------------------------------------------------------
// --
// -- MASTER - Master for LINE.
// --
// --
// --------------------------------------------------------------------------

// --------------------------------------------------------------------------
// -- Includes
// --------------------------------------------------------------------------
#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include "defines.h"
#include "console.h"
#include "fbuffer.h"
#include "serial.h"
#include "key.h"


// --------------------------------------------------------------------------
// -- Defines
// --------------------------------------------------------------------------

// --- Port window defines ---
#define MPW_PORT1X		1
#define MPW_PORT2X 	    40
#define MPW_PORTY		40
#define MPW_PORT_YSIZE	11

// --- Primary window defines
#define MPRIW_X		1
#define MPRIW_Y		1
#define MPRIW_YSIZE 38

#define MPRIW_SEP	39


#define GRAYED_COLOR    YELLOW
#define GRAYED_BACK		BLUE

#define HIGHLITE_COLOR	WHITE
#define HIGHLITE_BACK   GREEN

#define MFS_X		14
#define MFS_Y		13
#define MMS_X		33
#define MFCS_X		54

#define PORT_Y		4
#define BPS_X		3
#define STOP_X      11
#define PARITY_X    19
#define BITS_X      29

// --- Master screen commands ---
enum  MasterScreenCommands {

   MSC_EXIT = (KEY_NO_COMMAND+1),
   MSC_GO,
   MSC_PORT1_BPS,
   MSC_PORT2_BPS,
   MSC_PORT1_BITS,
   MSC_PORT2_BITS,
   MSC_PORT1_SBITS,
   MSC_PORT2_SBITS,
   MSC_PORT1_PARITY,
   MSC_PORT2_PARITY,
   MSC_FRAME_UP,
   MSC_FRAME_DOWN,
   MSC_MODE_UP,
   MSC_MODE_DOWN,
   MSC_FC_UP,
   MSC_FC_DOWN

};

// --- Selector states
enum MasterFramingState {

   MFS_SERIAL = 0,
   MFS_PPP,
   MFS_PCCOM,
   MFS_IRLAP
};
#define MFS_LAST_STATE (MFS_IRLAP + 1)
#define MFS_MAX		   MFS_IRLAP

enum MasterModeState {

   MMS_SNOOP 	= 0,
   MMS_INTRUDE

};
#define MMS_LAST_STATE (MMS_INTRUDE + 1)
#define MMS_MAX		   MMS_INTRUDE

enum MasterFCState {

   MFCS_NONE	  = 0,
   MFCS_HARDWARE
};
#define MFCS_LAST_STATE (MFCS_HARDWARE + 1)
#define MFCS_MAX	    MFCS_HARDWARE

// --------------------------------------------------------------------------
// -- Definitions
// --------------------------------------------------------------------------

// --- Port class -----------------------------------------------------------
class  MasterPort : public ConFreeWindow {

   Boolean		  active;

 protected:

   void refreshPortData(void);

 public:

   PortParameters  myParam;

   MasterPort(SS_PORT_T	thisPort);

   void refresh(void) { refreshPortData(); };
   void redraw(void);

};


// --- Main screen class ----------------------------------------------------
class MasterScreen {

   // Windows used
   MasterPort		port1;
   MasterPort		port2;

   ConFreeWindow    primary;

   ConSepLine		separator;

   // Key controler
   KeyHandler		keyBoard;

   // Private methods
   void  refreshMFS(void);
   void  refreshMMS(void);
   void  refreshMFCS(void);
   void  refreshPort1(void) { port1.refresh(); };
   void  refreshPort2(void) { port2.refresh(); };

 public:

   // Current states
   MasterFramingState	framingState;
   MasterModeState		modeState;
   MasterFCState		fcState;

   // Methods
   MasterScreen(void);
   MasterScreenCommands use(void);

   void refresh(void);


};

// --------------------------------------------------------------------------
// -- Data
// --------------------------------------------------------------------------

// Helpers

// --------------------------------------------------------------------------
// -- Data
// --------------------------------------------------------------------------

// -- Key maps for Master screen
#define 	MASTER_KEY_MAP_SIZE	2
KeyElement  masterKeyMap[MASTER_KEY_MAP_SIZE] = {

   { MSC_EXIT, KEY_ESC    },
   { MSC_GO,   KEY_ENTER  }

};

#define 	MASTER_KEY_MAP_EXT_SIZE	14
KeyElement  masterKeyMapExt[MASTER_KEY_MAP_EXT_SIZE] = {

   { MSC_PORT1_BPS,		KEY_F1	   },
   { MSC_PORT1_SBITS,	KEY_F2	   },
   { MSC_PORT1_PARITY,	KEY_F3	   },
   { MSC_PORT1_BITS,	KEY_F4	   },
   { MSC_PORT2_BPS,		KEY_F5	   },
   { MSC_PORT2_SBITS,	KEY_F6	   },
   { MSC_PORT2_PARITY,	KEY_F7	   },
   { MSC_PORT2_BITS,	KEY_F8	   },
   { MSC_FRAME_UP,   	KEY_INS    },
   { MSC_FRAME_DOWN,	KEY_DEL    },
   { MSC_MODE_UP,   	KEY_HOME   },
   { MSC_MODE_DOWN,		KEY_END    },
   { MSC_FC_UP,     	KEY_PGUP   },
   { MSC_FC_DOWN,		KEY_PGDOWN }

};


// -- Text for various selectors
char	mfsText[4][10] = {

   " SERIAL  ",
   " PPP     ",
   " pcCOMM  ",
   " irLAP   "
};

char	mmsText[2][12] = {

   "  SNOOP    ",
   "  INTRUDE  "
};

char	mfcsText[2][15] = {

   "   NONE       ",
   "   HARDWARE   "
};

char    portBPSText[5][7] = {

   "   300",
   "  2400",
   "  9600",
   " 19200",
   " 38400"
};

char	portSBITSText[2][7] = {

   "    1 ",
   "    2 "
};

char	portParityText[3][9] = {

   "   NONE ",
   "    ODD ",
   "   EVEN "
};

char    portBitsText[3][7] = {

   "    5 ",
   "    7 ",
   "    8 "
};

// --------------------------------------------------------------------------
// -- Methods
// --------------------------------------------------------------------------

// --- port screen class --------------------------------------------------

// --- Constructor ------------
MasterPort::MasterPort(SS_PORT_T	thisPort) :
			ConFreeWindow(MPW_PORTY, 1+(thisPort*MPW_PORT2X), MPW_PORT_YSIZE )
{

}

// --- Do a port refresh ---
void  MasterPort::refreshPortData( void ) {

   int  step;

   // put the BPS first
   for (step = 0; step <= SS_BPS_38400; step++) {
	  if (myParam.bpsIs() == step)
		 put(BPS_X, PORT_Y+step, portBPSText[step],
			 HIGHLITE_COLOR, HIGHLITE_BACK);
	  else
		 put(BPS_X, PORT_Y+step, portBPSText[step]);
   }

   if (myParam.sBitsIs() == SS_SBITS_1) {
		 put(STOP_X, PORT_Y,   portSBITSText[0],
			 HIGHLITE_COLOR, HIGHLITE_BACK);
		 put(STOP_X, PORT_Y+1, portSBITSText[1]);
   } else {
		 put(STOP_X, PORT_Y,   portSBITSText[0]);
		 put(STOP_X, PORT_Y+1, portSBITSText[1],
			 HIGHLITE_COLOR, HIGHLITE_BACK);
   }

   for (step = 0; step <= SS_PARITY_EVEN; step++) {
	  if (myParam.parityIs() == step)
		 put(PARITY_X, PORT_Y+step, portParityText[step],
			 HIGHLITE_COLOR, HIGHLITE_BACK);
	  else
		 put(PARITY_X, PORT_Y+step, portParityText[step]);
   }

   for (step = 0; step <= SS_BITS_8; step++) {
	  if (myParam.bitsIs() == step)
		 put(BITS_X, PORT_Y+step, portBitsText[step],
			 HIGHLITE_COLOR, HIGHLITE_BACK);
	  else
		 put(BITS_X, PORT_Y+step, portBitsText[step]);
   }

};

// --- Do a port redraw ---
void MasterPort::redraw(void) {

   reset();
   put(1,1,"  PORT 1  (COM1)                      ºº  PORT 2  (COM2)");
   put(1,2,"                                      ºº");
   put(1,3,"  ÚBPSÄ¿  ÚSTOP¿  ÚPARITY¿  ÚBITS¿    ºº  ÚBPSÄ¿  ÚSTOP¿  ÚPARITY¿  ÚBITS¿");
   put(1,4,"                                      ºº");
   put(1,5,"                                      ºº");
   put(1,6,"          ÀÄÄÄÄÙ                      ºº          ÀÄÄÄÄÙ");
   put(1,7,"                  ÀÄÄÄÄÄÄÙ  ÀÄÄÄÄÙ    ºº                  ÀÄÄÄÄÄÄÙ  ÀÄÄÄÄÙ");
   put(1,8,"                                      ºº");
   put(1,9,"  ÀÄÄÄÄÙ                              ºº  ÀÄÄÄÄÙ");
   put(1,10,"                                      ºº");
}

// --- Operators ---



// --- master screen class --------------------------------------------------

// --- Constructor ------------
MasterScreen::MasterScreen(void) :
			  port1(SS_PORT_1),
			  port2(SS_PORT_2),
			  primary(MPRIW_Y, MPRIW_X, MPRIW_YSIZE),
			  separator(MPRIW_SEP, YELLOW, BLUE),
			  keyBoard()						       {



   // Setup windows
   port1.setTextColor(YELLOW);
   port2.setTextColor(YELLOW);
   port1.setBackgroundColor(BLUE);
   port2.setBackgroundColor(BLUE);

   primary.setTextColor(YELLOW);
   primary.setBackgroundColor(BLUE);

   // setup key handler
   keyBoard.registerTable( masterKeyMap,    MASTER_KEY_MAP_SIZE,     FALSE);
   keyBoard.registerTable( masterKeyMapExt, MASTER_KEY_MAP_EXT_SIZE, TRUE );

   // Init states
   framingState = MFS_SERIAL;
   modeState    = MMS_SNOOP;
   fcState	    = MFCS_NONE;


}


// --- Use ------------
MasterScreenCommands  MasterScreen::use( void ) {

   MasterScreenCommands     command;
   char					    dummy;

   // Reset all the windows
   port1.redraw();
   port1.refresh();
   port2.refresh();
   refresh();

   separator.reset();

   // loop on command.
   command = keyBoard.get(dummy);
   while(command != MSC_EXIT) {

	  switch (command) {

		 case  MSC_PORT1_BPS:
			   port1.myParam.bpsNext();
			   port1.refresh();
			   break;

		 case  MSC_PORT1_SBITS:
			   port1.myParam.sBitsNext();
			   port1.refresh();
			   break;

		 case  MSC_PORT1_PARITY:
			   port1.myParam.parityNext();
			   port1.refresh();
			   break;

		 case  MSC_PORT1_BITS:
			   port1.myParam.bitsNext();
			   port1.refresh();
			   break;

		 case  MSC_PORT2_BPS:
			   port2.myParam.bpsNext();
			   port2.refresh();
			   break;

		 case  MSC_PORT2_SBITS:
			   port2.myParam.sBitsNext();
			   port2.refresh();
			   break;

		 case  MSC_PORT2_PARITY:
			   port2.myParam.parityNext();
			   port2.refresh();
			   break;

		 case  MSC_PORT2_BITS:
			   port2.myParam.bitsNext();
			   port2.refresh();
			   break;

		 case  MSC_FRAME_DOWN:
			   if (framingState == MFS_MAX)
				  framingState = MFS_SERIAL;
			   else
				  framingState++;
			   refreshMFS();
			   break;

		 case  MSC_FRAME_UP:
			   if (framingState == MFS_SERIAL)
				  framingState = MFS_MAX;
			   else
				  framingState--;
			   refreshMFS();
			   break;

		 case  MSC_MODE_DOWN:
		 case  MSC_MODE_UP:
			   if (modeState == MMS_INTRUDE)
				  modeState = MMS_SNOOP;
			   else
				  modeState = MMS_INTRUDE;
			   refreshMMS();
			   break;


		 case  MSC_FC_UP:
		 case  MSC_FC_DOWN:
			   if (fcState == MFCS_NONE)
				  fcState = MFCS_HARDWARE;
			   else
				  fcState = MFCS_NONE;
			   refreshMFCS();
			   break;


		 case  MSC_GO:
			   return MSC_GO;

		 case  MSC_EXIT:
			   return MSC_EXIT;

	  } // end case

	  command = keyBoard.get(dummy);

   } // end while

   return MSC_EXIT;
}


// --- Refresh the master screen ---
void  MasterScreen::refresh(void) {

   // clear
   primary.reset();

   // Draw screen
   primary.put(1,1,"   DOPPELGANGER     COPYRIGHT (C) GEOWORKS 1995 - All rights reserved");
   primary.put(1,4,"   FOR INTERNAL USE ONLY");

   primary.put(13, 9,"ÚÄÄÄÄÄÄÄÄÄ¿        ÚÄÄÄÄÄÄÄÄÄÄÄ¿        ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿");
   primary.put(13,10,"³ FRAMING ³        ³ LINE MODE ³        ³ FLOW CONTROL ³");
   primary.put(13,11,"³(ins/del)³        ³ (home/end)³        ³ (pgup/pgdn)  ³");
   primary.put(13,12,"ÃÄÄÄÄÄÄÄÄÄ´        ÃÄÄÄÄÄÄÄÄÄÄÄ´        ÃÄÄÄÄÄÄÄÄÄÄÄÄÄÄ´");
   primary.put(13,13,"³         ³        ³           ³        ³              ³");
   primary.put(13,14,"³         ³        ³           ³        ³              ³");
   primary.put(13,15,"³         ³        ÀÄÄÄÄÄÄÄÄÄÄÄÙ        ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ");
   primary.put(13,16,"³         ³");
   primary.put(13,17,"ÀÄÄÄÄÄÄÄÄÄÙ");

   // draw selectors
   refreshMFS();
   refreshMMS();
   refreshMFCS();

}


// --- Refresh the Master Framing mode selector ---
void  MasterScreen::refreshMFS(void) {

   for (int step = 0; step < MFS_LAST_STATE; step++) {

	  if (framingState == step)
		 primary.put(MFS_X, MFS_Y+step, mfsText[step],
					 HIGHLITE_COLOR, HIGHLITE_BACK);
	  else
		 primary.put(MFS_X, MFS_Y+step, mfsText[step]);
   }
}

// --- Refresh the Master mode selector ---
void  MasterScreen::refreshMMS(void) {

   for (int step = 0; step < MMS_LAST_STATE; step++) {

	  if (modeState == step)
		 primary.put(MMS_X, MFS_Y+step, mmsText[step],
					 HIGHLITE_COLOR, HIGHLITE_BACK);
	  else
		 primary.put(MMS_X, MFS_Y+step, mmsText[step]);
   }
}

// --- Refresh the master flow control selector ---
void  MasterScreen::refreshMFCS(void) {

   for (int step = 0; step < MFCS_LAST_STATE; step++) {

	  if (fcState == step)
		 primary.put(MFCS_X, MFS_Y+step, mfcsText[step],
					 HIGHLITE_COLOR, HIGHLITE_BACK);
	  else
		 primary.put(MFCS_X, MFS_Y+step, mfcsText[step]);
   }
}




// --------------------------------------------------------------------------
// -- Functions
// --------------------------------------------------------------------------

// --- Helpers --------------------------------------------------------------


// -- Main ------------------------------------------------------------------
void main() {

   // Make the primary screen
   Console					thisConsole;
   MasterScreen				screen;
   MasterScreenCommands     command;

   // Use the mast screen
   command = screen.use();
   while( command != MSC_EXIT) {

	  command = screen.use();
   }

}