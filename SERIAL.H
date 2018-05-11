// --------------------------------------------------------------------------
// --
// -- Serial - Interfacing header.
// --
// --
// --------------------------------------------------------------------------

// --------------------------------------------------------------------------
// -- Defines
// --------------------------------------------------------------------------

// ------------------------------------
// -- Enumerate the port data
enum  SS_PORT_T { SS_PORT_1  = 0, SS_PORT_2 = 1 };
enum  SS_BPS_T  { SS_BPS_300 = 0, SS_BPS_2400, SS_BPS_9600,
				  SS_BPS_19200, SS_BPS_38400            };
#define SS_BPS_MIN	    SS_BPS_300
#define SS_BPS_MAX	    SS_BPS_38400
#define SS_BPS_DEFAULT  SS_BPS_19200

enum  SS_BITS_T   { SS_BITS_5      = 0, SS_BITS_7,     SS_BITS_8      };
#define SS_BITS_MIN	    SS_BITS_5
#define SS_BITS_MAX	    SS_BITS_8
#define SS_BITS_DEFAULT SS_BITS_8

enum  SS_SBITS_T  { SS_SBITS_1,         SS_SBITS_2     };
#define SS_SBITS_MIN  	  SS_SBITS_1
#define SS_SBITS_MAX  	  SS_SBITS_2
#define SS_SBITS_DEFAULT  SS_SBITS_1

enum  SS_PARITY_T { SS_PARITY_NONE = 0, SS_PARITY_ODD, SS_PARITY_EVEN };
#define SS_PARITY_MIN  	   SS_PARITY_NONE
#define SS_PARITY_MAX  	   SS_PARITY_EVEN
#define SS_PARITY_DEFAULT  SS_PARITY_NONE

enum  SS_ERROR_T  { SS_NO_ERROR = 0, SS_ERROR, SS_BAD_PORT };

#define SS_NO_CHARACTER	-1

#define SS_BUFFER_SIZE	256

// --------------------------------------------------------------------------
// -- Class definitions
// --------------------------------------------------------------------------

// -----------------------------------
// -- Port parameters class
class PortParameters {

   friend   Serial;

   SS_BPS_T		   bps;
   SS_BITS_T	   bits;
   SS_SBITS_T	   sBits;
   SS_PARITY_T	   parity;

 public:

   void bpsNext(   void);
   void bitsNext(  void);
   void sBitsNext( void);
   void parityNext(void);

   unsigned int  bpsIs(   void) { return bps;    };
   unsigned int  bitsIs(  void) { return bits;   };
   unsigned int  sBitsIs( void) { return sBits;  };
   unsigned int  parityIs(void) { return parity; };

   PortParameters(void);

};


// -----------------------------------
// -- Basic serial class.  Each instance will run a port.
class Serial
{

   SS_PORT_T	thisPort;
   int  		portbase;

   Boolean  flowControl;

   FBuffer  txFIFO;		// FIFOs for this object
   FBuffer  rxFIFO;

   SS_ERROR_T setFraming( SS_PARITY_T parity, SS_BITS_T bits,
						  SS_SBITS_T  stopBit                  );
   SS_ERROR_T setSpeed(SS_BPS_T    speed   );
   SS_ERROR_T setPort( SS_PORT_T   port    );

 public:

   Boolean  enabled;
   Boolean  up;

   Serial(SS_PORT_T port, PortParameters portP,
		  Boolean  hwFlowControl);

   Serial &operator<<( char octet   );
   Serial &operator<<( char *string );
   Serial &operator>>( unsigned char  &octet  );

   SS_ERROR_T lineUp(  void);
   SS_ERROR_T lineDown(void);

   // Exceptions
   class XsOUT_OF_MEMORY { };
   class XsBAD_PARAM  { };
   class XsOVERRUN  { };
   
   ~Serial();

};
