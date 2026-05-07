
/* scilab Ctrl-C handling */

/* Author: Till Straumann <strauman@slac.stanford.edu>, 2004
 * This file is subject to the EPICS open license, consult the LICENSE file for
 * more information. This header must not be removed.
 */

#include <multiEzcaCtrlC.h>

#include <pvaClient.h>

#ifdef MATLAB_APP

#include <mex.h>

extern unsigned char utHandlePendingInterrupt();

static int poll_cb()
{
	if ( utHandlePendingInterrupt() ) {
		ezcaAbort();
		return 1;
	}
	return 0;
}

#else

#include <version.h>
#include <api_scilab.h>
#include <configvariable_interface.h>

static int poll_cb()
{
int rval = isExecutionBreak();
	/* sciprint("polling: isExecutionBreak() %i\n", rval); */
	if (rval) {
		ezcaAbort();
	}
	return rval;
}

#endif

void
multi_ezca_ctrlC_prologue(CtrlCState psave)
{
}

void
multi_ezca_ctrlC_epilogue(CtrlCState prest)
{
}

void
multi_ezca_ctrlC_initialize()
{
double   tottime = ezcaGetTimeout() * ezcaGetRetryCount();
unsigned cnt;

	/* use longer timeout to reduce polling load
	 * however: this is also the minimal latency,
	 * i.e., mpvaGet() never takes less than the
	 * ezca timeout!!!
	 */
	ezcaSetTimeout((float)0.1);

	cnt = (unsigned)(tottime/ezcaGetTimeout());
	if ( cnt < 1 )
		cnt = 1;
	ezcaSetRetryCount(cnt);

	ezcaPollCbInstall(poll_cb);
}

void
multi_ezca_ctrlC_finalize()
{
	ezcaPollCbInstall( 0 );
}
