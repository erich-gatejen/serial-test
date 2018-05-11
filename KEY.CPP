// --------------------------------------------------------------------------
// --
// -- FBUFFERS - FIFO buffers.
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
#include "key.h"

// --------------------------------------------------------------------------
// -- Definitions
// --------------------------------------------------------------------------

#define KEY_EXTENDED_KEY		0

// --------------------------------------------------------------------------
// -- Methods
// --------------------------------------------------------------------------

// --- Constructor ------------------
KeyHandler::KeyHandler(void) {

   invalidateAll();

}


// --- Destructor -------------------
// NONE



// --- Register a single command to the table
KeyError KeyHandler::registerCommand(KeyElement	  element,
									 Boolean 	  extended ) {

   if (element.keyCode >= KEY_MAX_TRANSLATION) return KEY_OUT_OF_RANGE;

   if (extended) {
	  extTranslation[element.keyCode] = element.theCommand;

   } else {
	  baseTranslation[element.keyCode] = element.theCommand;

   }

   return KEY_NO_ERROR;
}


// --- Register a single command to the table, simple way
KeyError KeyHandler::registerCommand(KeyCommand    theCommand,
									 unsigned int  keyCode,
									 Boolean 	   extended    ) {

   if (keyCode >= KEY_MAX_TRANSLATION) return KEY_OUT_OF_RANGE;

   if (extended) {
	  extTranslation[keyCode] = theCommand;

   } else {
	  baseTranslation[keyCode] = theCommand;

   }

   return KEY_NO_ERROR;
}


// -- Register an entire set of the table, as a key element
KeyError KeyHandler::registerTable(KeyElement   *theTable,
								   int			 number,
								   Boolean       extended      ) {

   int        step   = 0;
   KeyElement *rover = theTable;

   for (step = 0; step < number; step++) {

	  if (rover->keyCode>= KEY_MAX_TRANSLATION) return KEY_OUT_OF_RANGE;

	  if (extended) {
		 extTranslation[rover->keyCode]  = rover->theCommand;

	  } else {
		 baseTranslation[rover->keyCode] = rover->theCommand;

	  }

	  rover++;

   };

   return KEY_NO_ERROR;

}


// -- Unregister a specific command
KeyError KeyHandler::unregisterCommand(unsigned int  keyCode,
									   Boolean       extended ) {

   if (keyCode >= KEY_MAX_TRANSLATION) return KEY_OUT_OF_RANGE;

   if (extended) {
	  extTranslation[keyCode]  = KEY_NO_COMMAND;

   } else {
	  baseTranslation[keyCode] = KEY_NO_COMMAND;

   }

   return KEY_NO_ERROR;

}


// -- Dump the entire contents of both tables
void KeyHandler::invalidateAll(void) {

   for (int step = 0; step < KEY_MAX_TRANSLATION; step++ ) {
	  baseTranslation[step] = KEY_NO_COMMAND;
	  extTranslation[step]  = KEY_NO_COMMAND;
   }
}


// -- Flush the keyboard ------------
void KeyHandler::flush(void) {

   unsigned char  key;

   while( kbhit() ) {

	  key = getch();
	  if (key == KEY_EXTENDED_KEY) getch();

   }
}


// -- Poll, a NON-blocking char get
Boolean  KeyHandler::poll(unsigned char &theChar,
						  KeyCommand    &theCommand) {

   if (kbhit()) {

	  theChar = getch();
	  if (theChar == KEY_EXTENDED_KEY) {
		 theChar    = getch();
		 theCommand = extTranslation[theChar];

	  } else {
		 theCommand = baseTranslation[theChar];

	  }

	  return TRUE;

   } else {

	  theCommand = KEY_NO_COMMAND;
	  return FALSE;
   }


}


// -- get, a BLOCKING char get
KeyCommand KeyHandler::get( unsigned char &theChar) {

   theChar = getch();
   if (theChar == KEY_EXTENDED_KEY) {
	  theChar    = getch();
	  return (extTranslation[theChar]);

   } else {
	  return (baseTranslation[theChar]);

   }

}


