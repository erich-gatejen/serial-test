// --------------------------------------------------------------------------
// --
// -- Console classes methods
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

// --------------------------------------------------------------------------
// -- Definitions
// --------------------------------------------------------------------------

// --------------------------------------------------------------------------
// -- Methods
// --------------------------------------------------------------------------

// --- Class: conWindow -----------------------------------------------------

// --- constructor ------------------
ConWindow::ConWindow(int Y, int sizeY) {

   startY = Y;
   endY   = Y + (sizeY-1);

   clear();

}


// --- put, character --------------
void ConWindow::putit(int X, int Y, char character) {

   use();
   gotoxy(X, Y);
   putch(character);

}


// --- put, string, not line based --------------
void ConWindow::putit(int X, int Y, char *text ) {

   use();

   while(*text) {

	  gotoxy(X, Y);
	  putch(*text);
	  X++;
	  if (X > CON_MAX_X_SIZE) {
		 X = CON_START_X;
		 Y++;
	  };

	  text++;
   }

}


// --- put, string, line based --------------
void ConWindow::putit(int X, int Y, char *text, Boolean lineBased ) {

   use();
   gotoxy(X,Y);
   cputs(text);

}


// --- move everything up one line ----
void ConWindow::up(void) {

   movetext(CON_START_X,  startY+1, CON_END_X, endY,
			CON_START_X,  startY                    );
}


// --- move everything down one line --------------
void ConWindow::down(void) {

   movetext(CON_START_X,  startY, CON_END_X, endY-1,
			CON_START_X,  startY+1                   );
}


// --- reset --------------
void ConWindow::reset(void) {

   clear();

}


// --- Class: conWindow -----------------------------------------------------

// --- constructor -------------------
ConColorControl::ConColorControl( ColorT textColor, ColorTB bkgColor ) {

   defaultTextColor = textColor;
   defaultBkgColor  = bkgColor;

}


// --- assert the colors --------------
void  ConColorControl::colAssert(void) {
   assertText();
   assertBack();
}


// --- Class: conLBSWindow -------------------------------------------------

// --- constructor ------------------
ConLBSWindow::ConLBSWindow( int yStart, int ySize ) :
			  ConWindow( yStart, ySize),
			  ConColorControl( WHITE, BLACK )              {

   cursorY = CON_START_Y;
}


// --- put string, no color control --
void ConLBSWindow::put(char *text) {

   colAssert();
   ConWindow::putit(CON_START_X, cursorY, text, TRUE);
   cursorY = wherey();
}


// --- put string, assert back, set text color --
void ConLBSWindow::put(char *text, ColorT  textColor) {

   assertBack();
   textcolor( textColor );
   ConWindow::putit(CON_START_X, cursorY, text, TRUE);
   cursorY = wherey();
}


// --- put string, assert text color, set background --
void ConLBSWindow::put(char *text, ColorTB backgroundColor) {

   assertText();
   textbackground( backgroundColor );
   ConWindow::putit(CON_START_X, cursorY, text, TRUE);
   cursorY = wherey();
}


// --- put string, set text color, set background --
void ConLBSWindow::put(char *text, ColorT  textColor,
					   ColorTB backgroundColor        ) {

   textcolor( textColor );
   textbackground( backgroundColor );
   ConWindow::putit(CON_START_X, cursorY, text, TRUE);
   cursorY = wherey();
}


// --- reset window --------------
void ConLBSWindow::reset(void) {

   colAssert();
   ConWindow::reset();
   cursorY = CON_START_Y;

}


// --- scroll window up, fill in top line --------
void ConLBSWindow::scrollUp(char *text) {

   down();
   colAssert();
   ConWindow::putit(CON_START_X, 1, text);

}


// --- scroll window up, fill in top line --------
void ConLBSWindow::scrollUp(char *text, ColorT  textColor,
							ColorTB backgroundColor        ) {
   down();
   textcolor( textColor );
   textbackground( backgroundColor );
   ConWindow::putit(CON_START_X, 1, text);
}



// --- Class: conCBCWindow -------------------------------------------------

// --- punt the cursor to its next position ---
void ConCBCWindow::puntCursor(int &X, int &Y) {

   // Adjust and possibly wrap X
   X++;
   if (X > CON_END_X) {
	  Y++;
	  X = CON_START_X;
   }

   // possibly wrap Y
   if (Y > sizeY) {
	  Y = CON_START_Y;
   }

}


// --- constructor ------------
ConCBCWindow::ConCBCWindow( int yStart, int ySize ) :
			  ConWindow( yStart, ySize),
			  ConColorControl( WHITE, BLACK )              {

   cursorX = 1;
   cursorY = 1;
   sizeY   = ySize;
}


// --- put character ----------
void ConCBCWindow::put(char  theCharacter) {

   int  tempX;
   int  tempY;

   // Put this character and find next spot
   colAssert();
   ConWindow::putit(cursorX, cursorY, theCharacter);

   puntCursor(cursorX, cursorY);
   tempX = cursorX;
   tempY = cursorY;


   // Put chaser dead-space
   assertBack(BLACK);
   ConWindow::putit(tempX, tempY, ' ');
   puntCursor(tempX, tempY);
   ConWindow::putit(tempX, tempY, ' ');


}


// --- put character ----------
void ConCBCWindow::put(char    theCharacter, ColorT  textColor,
					   ColorTB backgroundColor )                {

   int  tempX;
   int  tempY;

   // Put this character and find next spot
   assertText(textColor);
   assertBack(backgroundColor);
   ConWindow::putit(cursorX, cursorY, theCharacter);

   puntCursor(cursorX, cursorY);
   tempX = cursorX;
   tempY = cursorY;


   // Put chaser dead-space
   assertBack(BLACK);
   ConWindow::putit(tempX, tempY, ' ');
   puntCursor(tempX, tempY);
   ConWindow::putit(tempX, tempY, ' ');

}


// --- reset window --------------
void ConCBCWindow::reset(void) {

   colAssert();
   ConWindow::reset();
   cursorX = CON_START_X;
   cursorY = CON_START_Y;

}


// --- Class: conFreeWindow -------------------------------------------------

// --- constructor -------------
ConFreeWindow::ConFreeWindow( int yStart, int xStart, int ySize ) :
			   ConWindow( yStart, ySize),
			   ConColorControl( WHITE, BLACK )              {

   startX = xStart;

}


// --- put character -----------
void ConFreeWindow::put(int  atX, int  atY, char  character) {

   colAssert();
   ConWindow::putit(atX+startX, atY, character);
}


// --- put character -----------
void ConFreeWindow::put(int  atX, int  atY, char  *text) {

   colAssert();
   ConWindow::putit(atX+startX, atY, text);
}


// --- put character -----------
void ConFreeWindow::put(int  atX, int  atY, char  character,
						ColorT  textColor,
						ColorTB backgroundColor				) {

   assertText(textColor);
   assertBack(backgroundColor);
   ConWindow::putit(atX+startX, atY, character);
}


// --- put character -----------
void ConFreeWindow::put(int  atX, int  atY, char  *text,
						ColorT  textColor,
						ColorTB backgroundColor				) {

   assertText(textColor);
   assertBack(backgroundColor);
   ConWindow::putit(atX+startX, atY, text);
}


// --- reset window --------------
void ConFreeWindow::reset(void) {

   colAssert();
   ConWindow::reset();
}


// --- Class: conSepLine -------------------------------------------------

// -- constructor
ConSepLine::ConSepLine( int  atY, ColorT  textColor,
						ColorTB backgroundColor ) :
			ConColorControl( textColor, backgroundColor ) {

   Y = atY;

}

// -- constructor
void ConSepLine::reset( void ) {

   colAssert();

   window( CON_START_X, Y, CON_END_X, Y+1);
   cputs("様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様");

}


// --- Class: console -------------------------------------------------

// -- constructor
Console::Console(void) {
   textmode(C4350);
   _setcursortype(_NOCURSOR);
}

// -- destructor
Console::~Console() {
   textmode(C4350);
}


