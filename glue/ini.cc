/* xmpvaglue library initializer */

#if defined(WIN32) || defined(_WIN32)
#include <Windows.h>
#endif

#include <mex.h>
#include <pvaClient.h>
#include <string.h>
#include <stdarg.h>
#include <stdio.h>

#if BASE_IS_MIN_VERSION(3,14,7)
#include <stdlib.h>
#include <epicsExit.h>
#endif

#include <errlog.h>

#if defined(WIN32) || defined(_WIN32)
typedef int pid_t;
#define getpid _getpid
extern "C" int _getpid();
#else
#include <unistd.h>
#endif

extern "C" const char *gitRevisionString;

#ifdef SCILAB_APP
#include <signal.h>
#include <errno.h>
#include <version.h>
#endif

#ifdef MATLAB_APP
#include <mglue.h>
#endif

// Configurable Parameters
#define DEBUG_FINA  1
#undef  USE_DLOPEN           // Use dlopen to lock scimpva in memory
#undef  USE_SCIWINEXITHACK   // Special hack for scilab/win32 (read below)
// End Of Configurable Parameters

#if defined(WIN32) || defined(_WIN32) || BASE_IS_MIN_VERSION(3,14,7)
#undef USE_DLOPEN            // ulink unloads the interface anyways
                             // so just locking the lib is probably worse
                             // than just a leak...
#endif

#if !(defined(WIN32) || defined(_WIN32)) || defined(MATLAB_APP)
#undef USE_SCIWINEXITHACK
#else
#ifdef USE_SCIWINEXITHACK
#include <wsci/wgnuplib.h>
extern "C" LPTW GetTextWinScilab(void);
#endif
extern "C" void C2F(xscion)(int*);
#endif


#if defined(USE_DLOPEN)
/* 
 * Use dlopen to lock libraries in memory and prevent from unlinking
 */
#include <dlfcn.h>
#endif

#define epicsExportSharedSymbols
#include <shareLib.h>
#include <multiEzcaCtrlC.h>
#include <multiEzca.h>

class multiEzcaInitializer {
private:
	int         initDone;
	int         inFiniFlag;
	int         isMccFlag;
	const char *matlabVersionString;

	multiEzcaInitializer(const multiEzcaInitializer &);
	multiEzcaInitializer &operator=(const multiEzcaInitializer &);

	void msgPrintf(const char *fmt, ...);
	
public:
	multiEzcaInitializer();

	int initialize();

	int inFini()
	{
		return inFiniFlag;
	}

	~multiEzcaInitializer();
#ifdef MATLAB_APP
	const char *getMatlabVersionString();
	int   getMatlabVersion();
	void mexInit();
	int  is2020b();

	int isMcc(int ini);
#endif
};

/* Our PID -- distinguish ourself from a forked caRepeater */
static	pid_t thepid;

#ifdef MATLAB_APP
int
multiEzcaInitializer::isMcc(int ini)
{
	/* Avoid execution of matlab code during finalization! */

	if ( ! isMccFlag ) {
		/* Check if this is executed from an mcc run
		 *
		 * (Thanks to Jim Sebek who suggested this fix
		 * on 4/10/07).
		 */
		mxArray *lhs = NULL;

		if ( 0 == mexCallMATLAB(1, &lhs, 0, NULL, "ismcc") && mxGetScalar(lhs) != 0. ) {
			msgPrintf((char*)"skip execution of multiEzca%s in ini.cc during mcc compilation\n", ini ? "Initializer" : "Finalizer");
			isMccFlag = 1;
		} else {
			isMccFlag = -1;
		}
	}

	return isMccFlag > 0 ? 1 : 0;
}
#endif

// Getting this to work on all platforms was hard work...
multiEzcaInitializer::~multiEzcaInitializer()
{
int skipCaContextDestroy = 0;

	if ( ! initDone )
		return;

	inFiniFlag = 1;

#ifdef MATLAB_APP
	/* Skip execution of finalizer if this is a mcc compilation run
	 * (J. Sebek, 6/2007)
	 */
	if ( isMcc(0) ) {
		return;
	}
#endif

	multi_ezca_ctrlC_finalize();

#if (defined(WIN32) || defined(_WIN32)) && !defined(MATLAB_APP)
	{
	// Scilab seems to kill threads cold when exiting (but not
    // as a result of 'ulink'). In this case, we must not try
	// to clean up since any attempt to synchronize with another
	// thread cleaning up [as the errlog exit handler does] would
	// deadlock (because all threads are already dead).
	// This is further complicated by the asynchronous nature
	// of scilab with GUI. I don't really know who ends up calling
	// ExitProcess() but it seems it is not the same thread that's
	// executing this code. When we get here things still look good
	// but at the time the errlog exit handler blocks for the errlog
	// thread to terminate the system seems to schedule what's
	// then doing ExitProcess() so that we never return from waiting
	// for errlog...
	// 
	// We implement two ways for detecting if we are exiting or
	// only unlinking:
	//   a) inspect hWndParent of the textwin struct. This is
	//      set to NULL by WinExit(). Drawback: sacrifice of
	//      binary compatibility. We know about the workings
	//      of WinExit() and must use knowledge abou the textwin
	//      layout (wgnuplib.h).
	//   b) Just wait for some time; eventually the threads will
	//      be dead and we detect them as 'suspended'.
	//      Drawback: we don't really know how long to wait nor
	//      how to synchronize.
	//
	// Another problem: msgPrintf()ing from an exiting wscilab.exe
	// crashes (but a [nogui] scilab.exe works fine). We handle that
	// here, too.
	int           dontPrint;
	// We rely on the errlog thread having this name!
	epicsThreadId errlogid = epicsThreadGetId("errlog");

		C2F(xscion)(&dontPrint);

		if ( dontPrint ) {
			// In graphical mode; we need to find out
			// if we are exiting or just unloading
#ifdef USE_SCIWINEXITHACK
			if ( 0 == GetTextWinScilab()->hWndParent ) {
				// bail out right away
				return;
			}
			// at this point we have either a normal hWndParent
			// and are not exiting. However, just in case we were
			// compiled for a different version of scilab and
			// the hWndParent member is actually bogus we resort
			// to the 'delay' method anyways...
#endif
			// delay a bit so we'll detect dead threads below.
			epicsThreadSleep(2.0);
		}

#if DEBUG_FINA > 0
		if ( ! dontPrint ) {
			msgPrintf((char*)"Errlog thread has ID 0x%08x\n", errlogid);
		}
#endif
		// win32 'IsSuspended' routine calls GetExitCodeThread and
		// declares the thread suspended if it's dead already...
		if ( !errlogid || epicsThreadIsSuspended(errlogid) ) {
#if DEBUG_FINA > 0
			if ( ! dontPrint )
				msgPrintf((char*)"aborting cleanup; bailing out\n");
#endif
			return; // don't attempt any cleanup
		}
	}
#endif

	// If we make it here, it's safe to print...

#if DEBUG_FINA > 0
	msgPrintf((char*)"Entering mpva finalizer\n");
#endif

	if ( thepid != getpid() ) {
		/* DONT run this if a forked process exits */
#if DEBUG_FINA > 0
		msgPrintf((char*)"mpva finalizer: PID mismatch\n");
#endif
		_exit(-1);
		return; //never get here
	}

	// Shutdown PVAccess client

#if DEBUG_FINA > 1
	msgPrintf((char*)"shutting down PVAccess client...\n");
#endif

#ifdef MATLAB_APP
	if ( is2020b() ) {
		/* 2020b deadlocks if EPICS joins threads during shutdown */
		skipCaContextDestroy = 0;
	}
#endif

	if ( ! skipCaContextDestroy ) {
		pvaClientShutdown();
	}

#if DEBUG_FINA > 1
	msgPrintf("%s\n", skipCaContextDestroy ? "skipped" : "done");
#endif

#if BASE_IS_MIN_VERSION(3,14,7)
#if DEBUG_FINA > 1
	msgPrintf((char*)"calling exits...\n");
#endif
	/* replicate epicsExit w/o calling exit() */
	epicsExitCallAtExits();
#if DEBUG_FINA > 1
	msgPrintf((char*)"done\n");
#endif
	epicsThreadSleep(1.);
#endif

#ifdef MATLAB_APP
	::free((void*) matlabVersionString);
#endif

#if DEBUG_FINA > 0
	msgPrintf((char*)"Leaving mpva finalizer\n");
#endif
}

multiEzcaInitializer::multiEzcaInitializer()
: initDone           ( 0 ),
  inFiniFlag         ( 0 ),
  isMccFlag          ( 0 ),
  matlabVersionString( 0 )
{
#ifdef SCILAB_APP
	initialize();
#endif
	/* under matlab initialize from mex-file context */
}

int
multiEzcaInitializer::initialize()
{
CtrlCStateRec saved;

	if ( initDone )
		return 1;

	initDone = 1;

#ifdef MATLAB_APP
	/* Skip execution of initializer if this is a mcc compilation run
	 * (J. Sebek, 4/2007)
	 */
	if ( isMcc(1) )
		return 0;
#endif


#if defined( SCILAB_APP ) && SCI_VERSION_MAJOR < 5
	/* uninstall scilabs recovery method; I'd rather have
	 * a coredump to debug...
	 * However, do this only for old scilab -- java installs
	 * a sigaction handler and apparently doesn't like us messing
	 * with a SIGSEGV handler -- I actually get segmentation faults
	 * IF I change the handler under scilab-5!
	 */
	signal(SIGSEGV, SIG_DFL);
#endif

	/* don't print to stderr because that
	 * doesn't go to scilab's main window...
	 */
	msgPrintf((char*)"Initializing MPVA Release '%s'...\n", gitRevisionString);
	msgPrintf((char*)"Author: Till Straumann <till.straumann@psi.ch>\n");

#if defined( MATLAB_APP ) && 0
	/* UPDATE: this mexLock() crashes matlab 2017 -- hence we remove
	 * it for now. Here's a relevant email (thanks to J. Sebek)
	 *
	 * 12/9/2017
	 *
	 * Hello Jim, 
	 *
	 * Thank you for your patience. I collaborated with my colleagues to work
	 * on this further and following are our observations.
	 *
	 * We were able to reproduce the crash on Windows with the sample
	 * application you provided and get the same crash error.  After
	 * speaking with the development team, it looks like this is probably
	 * a bug from a change in the code in R2017b.  The best guess we have
	 * as to why this started failing in R2017b is that there was probably
	 * a change in scoping of a variable or structure that is now initialized
	 * either when the MEX file is compiled or at run-time.  If mexLock is
	 * called in a library linked to the MEX file before the MEX file is run,
	 * the failure occurs.
	 *
	 * While we want to avoid any crashes and will investigate this issue,
	 * from talking with the development team, mexLock wasn't intended
	 * to be called at the shared library level.  While we don't specifically
	 * forbid it in our documentation, our examples there show how to use it
	 * in a MEX file. We tested the workflow by moving the mexLock command
	 * from the shared library object (libthelib.so through ini.o) to the
	 * calling MEX function (themex.cc), and the program ran successfully
	 * without crashing.  
	 *
	 * We then verified a workflow similar to what you mentioned, multiple
	 * MEX files accessing a shared library without the library being
	 * released until specifically unlock called.  We created a MEX file
	 * where we call a function from the shared library as well as MEX
	 * function calls mexLock.  When mexLock is called, the MEX can't
	 * be released from memory by typing clear <mexFile>.  This can be
	 * tested by trying to recompile the MEX file, which will explicitly say
	 * that it is locked. This still is same as the workflow you intended to
	 * have (even though mexLock is in the MEX file), as the MEX file is
	 * locked, the linked library is also locked as well, and this can be
	 * verified by duplicating the MEX file and calling it mexFile2.  Each
	 * MEX file can be locked or unlocked, but the library won't be freed
	 * unless all MEX files that load that library are unlocked.
	 *
	 * I'm not sure if this fulfills your exact workflow, perhaps if want
	 * to load the library once at the start of their program and never release
	 * it, no matter what happens to the MEX file.  In that case, you may want
	 * to add a dummy MEX file that calls a function in the shared
	 * library as well as mexLock.  You can then unlock the MEX file when
	 * exiting the application so that the MEX file and the library it loads
	 * remains throughout the lifetime of the application.
	 *
	 * Developers are already aware of this behavior (crash when mexLock
	 * called from shared library) and they may consider fixing this in one of
	 * the future releases of MATLAB.
	 *
	 * Please go through the above information and let me know if  the
	 * workaround satisfies your requirements or not. I look forward to hearing
	 * from you.
	 *
	 * Sincerely, 
	 * Manish Annappa
	 */
	/* Under matlab, always lock the library in memory - even though we
	 * now do a pretty good job cleaning up (and as a matter of fact
	 * I now can clear and reload scimpva and matmpva under unix [but
	 * not win$] w/o crashing) but more work in ezcamt is needed until
	 * we can release this...
	 */
	mexLock();
	/* NOTE: (2021/3/3) we now use the 'APPLY_MEXLOCK()' macro to
	 *       ensure mexLock() is executed from a mex file.
	 */
#endif

#if defined(USE_DLOPEN) && !defined(MATLAB_APP)
	if ( !dlopen("libsezcaglue.so",RTLD_NOW) ) {
		msgPrintf((char*)"Locking library in memory failed: %s\n",dlerror());
	}
#endif

	multi_ezca_ctrlC_initialize();

	multi_ezca_ctrlC_prologue(&saved);
	pvaClientInit(); /* initialize PVAccess provider */
	multi_ezca_ctrlC_epilogue(&saved);

	/* MUST initialize errlog -- otherwise it is never initialized
	 * until after epicsCallAtExits(), i.e., from some class destructor.
	 * At that point it is too late however and it won't be shut
	 * down anymore.
	 */
	errlogInit(0);	// FIXME: should move that to ezca initialization

	/* Track PID to detect forked processes */
	thepid = getpid();

	return 0;
}

void
multiEzcaInitializer::msgPrintf(const char *fmt, ...)
{
va_list ap;
char    msg[512];
	va_start(ap, fmt);
	vsnprintf(msg, sizeof(msg), fmt, ap);
	va_end(ap);
#ifdef SCILAB_APP
	mexPrintf("%s", msg);
#endif
#ifdef MATLAB_APP
	/* Matlab 2020 doesn't like printing from shutdown */
	if ( inFini() ) {
		/* Not visible in matlab console */
		fprintf(stderr, "%s", msg);
	} else {
		mexPrintf("%s", msg);
	}
#endif
}

#ifdef MATLAB_APP
void
multiEzcaInitializer::mexInit()
{
mxArray *lhs[1];
mxArray *exc;
char    *str;
size_t   sl;

	/* Assume matlab serializes access to mexFiles */
	if ( initialize() )
		return;

	lhs[0]=0;
	exc = mexCallMATLABWithTrap(1, lhs, 0, NULL, "version");
	if ( exc ) {
		msgPrintf("WARNING: -- UNABLE TO DETERMINE MATLAB VERSION\n");
	} else {
		if ( ! mxIsChar( lhs[0] ) ) {
			msgPrintf("WARNING: -- UNABLE TO DETERMINE MATLAB VERSION (not a string?)\n");
		} else {
			sl  = mxGetN(lhs[0]);
			str = (char*)::malloc(sl + 1);
			str[sl] = 0;
			mxGetString( lhs[0], str, sl );
			matlabVersionString = str;
			str = 0;
		}
	}
	if ( lhs[0] ) {
		mxDestroyArray( lhs[0] );
	}
	if ( str ) {
		::free( (void*)str );
	}
	if ( ! matlabVersionString ) {
		matlabVersionString = "unknown";
	}

#ifdef CONFIG_MEXLOCK
	mexLock();
#else
	if ( is2020b() ) {
		/* Lock anyways -- 2020b doesn't handle unloading well */
		mexLock();
	}
#endif

}

const char *
multiEzcaInitializer::getMatlabVersionString()
{
	if ( ! matlabVersionString ) {
		msgPrintf("%s\n", "INTERNAL ERROR: must not call getMatlabVersionString() before the first mexFile was executed");
		return "ERROR";
	}
	return matlabVersionString;
}

int
multiEzcaInitializer::getMatlabVersion()
{
const char *c = getMatlabVersionString();
int         i;
int         rv = 0;

	while ( 0 == rv ) {
		while ( *c != 'R' ) {
			if ( ! *c ) {
				return -1;
			}
			c++;
		}
		c++;
		i = 0;
		while ( i < 4 ) {
			if ( ! *c ) {
				return -1;
			}
			if ( *c >= '0' && *c <= '9' ) {
				rv = 10*rv + (*c - '0');
				i++;
				c++;
			} else {
				rv = 0;
				i  = 4;
			}
		}
	}
	return rv;
}


int
multiEzcaInitializer::is2020b()
{
	return getMatlabVersion() >= 2020;
}
#endif

static multiEzcaInitializer theini;


#ifdef MATLAB_APP
extern "C" void mpvaMexGblInit()
{
	theini.mexInit();
}
#endif
