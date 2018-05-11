// --------------------------------------------------------------------------
// --
// -- FBUFFERS - FIFO buffers Header.
// --
// --
// --------------------------------------------------------------------------

// --------------------------------------------------------------------------
// -- Includes
// --------------------------------------------------------------------------

// --------------------------------------------------------------------------
// -- Definitions
// --------------------------------------------------------------------------
#define  FB_MAX_BUFFER_SIZE		   1024

enum  FB_ERROR_T { FB_NO_ERROR = 0, FB_OVERRUN, FB_EMPTY };

// --------------------------------------------------------------------------
// -- Classes
// --------------------------------------------------------------------------

class FBuffer {

   unsigned char  *buffer;

   // To avoid multiplication, we will keep pointers
   unsigned char  *roverStart;
   unsigned char  *roverEnd;
   unsigned int   currentStop;
   unsigned int   currentSize;
   unsigned int   currentStart;

 public:

   FBuffer(unsigned int  size);
  ~FBuffer();

   Boolean  HasData(void) { if(currentSize)return TRUE; return FALSE; };

   // Lets try and inline these bugs.  The interrupt handler will
   // thanks us
   FB_ERROR_T  In(unsigned char   byte) {
	  if (currentSize==FB_MAX_BUFFER_SIZE) return FB_OVERRUN;
	  currentSize++;
	  *roverEnd = byte;
	  if (currentStop == (FB_MAX_BUFFER_SIZE-1)) {
		 currentStop = 0;
		 roverEnd    = buffer;
	  } else {
		 currentStop++;
		 roverEnd++;
	  }
	  return FB_NO_ERROR;
   };

   FB_ERROR_T  Out(unsigned char  &byte) {
	  if (currentSize==0) return FB_EMPTY;
	  currentSize--;
	  byte = *roverStart;
	  if (currentStart == (FB_MAX_BUFFER_SIZE-1)) {
		 currentStart  = 0;
		 roverStart    = buffer;
	  } else {
		 currentStart++;
		 roverStart++;
	  }
	  return FB_NO_ERROR;
   };

};


