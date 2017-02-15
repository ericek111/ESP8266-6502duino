#include <FS.h>
#include <ESP8266WiFi.h>
#include "user_interface.h"

#if 0
#define AP_SSID		"BBCBasic"
#define AP_PASSWORD	""
#define AP_CHANNEL	6
#else
#define SSID		"Ty Penguin"
#define PASSWORD	"dgtp2ymc"
#endif

/* Choose one. Normal inline seems to be fastest.
 */
//#define INLINE inline __attribute__((always_inline))
#define INLINE inline
//#define INLINE

#include "processor.h"
#include "memory.h"
