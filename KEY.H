// --------------------------------------------------------------------------
// --
// -- KEY -- Keyboard handler.
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

typedef int  KeyCommand;


// -- Absolutes
#define  KEY_MAX_TRANSLATION	128
#define  KEY_NO_COMMAND		   0		// NOTHING can translate to this
										// it indicates NO command


// -- Errors
enum  KeyError {
   KEY_NO_ERROR = 0,
   KEY_OUT_OF_RANGE,
   KEY_TOO_MANY

};

// -- keyStockKeys
enum  KeyStockKeys {

	KEY_ESC			= 27,
	KEY_A			= 97,
	KEY_Z   		= 122,
	KEY_SHIFT_A     = 65,
	KEY_SHIFT_Z		= 90,
	KEY_ENTER		= 13

};

// -- keyStockKeysExt
enum  KeyStockKeysExt {

	KEY_F1			= 59,
	KEY_F2			= 60,
	KEY_F3			= 61,
	KEY_F4			= 62,
	KEY_F5			= 63,
	KEY_F6			= 64,
	KEY_F7			= 65,
	KEY_F8			= 66,
	KEY_F9			= 67,
	KEY_F10			= 68,
	KEY_HOME		= 71,
	KEY_UP			= 0,
	KEY_PGUP		= 73,
	KEY_LEFT		= 0,
	KEY_RIGHT		= 0,
	KEY_END			= 79,
	KEY_DOWN		= 0,
	KEY_PGDOWN		= 81,
	KEY_INS			= 82,
	KEY_DEL			= 83

};



// --------------------------------------------------------------------------
// -- Classes
// --------------------------------------------------------------------------

// -- key element.  To make tables to register easier
struct KeyElement {

   KeyCommand    theCommand;
   unsigned int  keyCode;

};


// -- Basic key handler
class KeyHandler {

   KeyCommand baseTranslation[KEY_MAX_TRANSLATION];
   KeyCommand extTranslation[KEY_MAX_TRANSLATION];

 public:

   KeyHandler(void);


   KeyError registerCommand(KeyCommand    theCommand,
							unsigned int  keyCode,
							Boolean       extended    );
   KeyError registerCommand(KeyElement	  element,
							Boolean 	  extended    );

   KeyError registerTable(KeyElement   *theTable,
						  int			number,
						  Boolean       extended      );

   KeyError unregisterCommand(unsigned int  keyCode,
							  Boolean		extended  );
   void invalidateAll(void);

   void flush();
   Boolean    poll(unsigned char &theChar,
				   KeyCommand    &theCommand);   // non-blocking
   KeyCommand get( unsigned char &theChar);      // blocking


};
