// --------------------------------------------------------------------------
// --
// -- CONSOLE -- Main cosole handler.
// --
// --
// --------------------------------------------------------------------------

// --------------------------------------------------------------------------
// -- Includes
// --------------------------------------------------------------------------

// --------------------------------------------------------------------------
// -- Definitions
// --------------------------------------------------------------------------

// -- Basic types
typedef int			  ColorT;   // stupid weak typing...
typedef unsigned char ColorTB;  // these must be different types

// -- Absolutes
#define CON_MAX_X_SIZE	80
#define CON_MAX_Y_SIZE  80

#define CON_START_X		1
#define CON_END_X		80
#define CON_START_Y		1

// --------------------------------------------------------------------------
// -- Classes
// --------------------------------------------------------------------------

// ---------------- Console windows -----------------------------------------

// -- The basic window
class ConWindow  {

   int  startY;
   int  endY;

   void use(void)   { window(CON_START_X, startY, CON_END_X, endY); };
   void clear(void) { use(); clrscr(); };

 public:

   ConWindow(int Y, int sizeY);

   void putit(int X, int Y, char character   );
   void putit(int X, int Y, char *text       );
   void putit(int X, int Y, char *text, Boolean lineBased	 );

   void up(void);
   void down(void);

   virtual void reset(void);

};


// -- Color control
class ConColorControl {

   ColorT   defaultTextColor;
   ColorTB  defaultBkgColor;

 protected:
   void assertText(void) { textcolor(defaultTextColor);     };
   void assertBack(void) { textbackground(defaultBkgColor); };

   void assertText(ColorT  textColor) {textcolor(textColor);      };
   void assertBack(ColorTB textColor) {textbackground(textColor); };

 public:

   ConColorControl( ColorT textColor, ColorTB bkgColor );

   void setTextColor( ColorT textColor)  { defaultTextColor = textColor; };
   void setBackgroundColor( ColorTB backgroundColor)
							 { defaultBkgColor = backgroundColor; };

   void colAssert(void);
};


// -- Line based scrolling window.
class ConLBSWindow : ConWindow, public ConColorControl {

   int      cursorY;  // must be saved at end of every public method

 public:

   ConLBSWindow( int yStart, int ySize );
   
   void put(char *text);
   void put(char *text, ColorT  textColor);
   void put(char *text, ColorTB backgroundColor);
   void put(char *text, ColorT  textColor,  ColorTB backgroundColor);

   void reset(void);

   void scrollUp(char *text);
   void scrollUp(char *text, ColorT  textColor,  ColorTB backgroundColor);

};


// -- Character based chaser window
class ConCBCWindow : ConWindow, public ConColorControl  {
   
   int   sizeY;

   int   cursorX;  // must be saved at end of every public method
   int   cursorY;  // must be saved at end of every public method

   void  puntCursor( int &X, int &Y);

 public:

   ConCBCWindow( int yStart, int ySize );

   void reset(void);
   void put(char theCharacter);
   void put(char theCharacter, ColorT  textColor,  ColorTB backgroundColor);

};


// -- Freestyle window
class ConFreeWindow : ConWindow, public ConColorControl  {

   int startX;

 public:

   ConFreeWindow( int yStart, int xStart, int ySize );

   void reset(void);

   void put( int  atX, int  atY, char   character );
   void put( int  atX, int  atY, char  *text );
   void put( int  atX, int  atY, char   character,
			 ColorT  textColor, ColorTB backgroundColor	);
   void put( int  atX, int  atY, char  *text,
			 ColorT  textColor, ColorTB backgroundColor	 );


};


// -- Separator line
class ConSepLine : ConColorControl {

   int	Y;

 public:

   ConSepLine( int  atY, ColorT  textColor,  ColorTB backgroundColor );

   void reset(void);
};


// -------- Console Objects -----------------------------------------------

class Console {

 public:

   Console(void);
  ~Console();

};
