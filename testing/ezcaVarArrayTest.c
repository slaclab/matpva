/* #include <tsDefs.h> */
#include <stdio.h>
#include <string.h>
#include <pvaClient.h>
#include <cantProceed.h>
#include <db_access.h>
#include <epicsTypes.h>
#include "pvaClient.h"

#define WV_NAME_MAX	100
#define WV_SIZE_MAX	(100*1000000)	/* Arbitrary 100M limit */

int main( int argc, char * argv[] )
{
	int				i;
	int				status;
	char		*	pvName	= "mywaveform";
	char			waveform_NELM[WV_NAME_MAX];
	char			waveform_NORD[WV_NAME_MAX];
	epicsInt32		count;
	epicsInt32		nElemWaveform;
	epicsInt32		nOrdWaveform;
	epicsInt32	*	pBufferWaveform	= NULL;

	if ( argc < 2 )
	{
		printf( "Usage: %s waveform_name\n", argv[0] );
		printf( "waveform_name should be the EPICS PV name for a partially filled waveform record.\n" );
		return -1;
	}

	pvName	= argv[1];
	snprintf( waveform_NELM, WV_NAME_MAX, "%.*s%5s", WV_NAME_MAX-6, pvName, ".NELM" );
	snprintf( waveform_NORD, WV_NAME_MAX, "%.*s%5s", WV_NAME_MAX-6, pvName, ".NORD" );

	status = ezcaGet( waveform_NELM,	ezcaLong, 1, &nElemWaveform	);
	if ( status != EZCA_OK )
	{
		printf( "Error %d: Unable to read NELM PV %s\n", status, waveform_NELM );
		return -1;
	}
	if ( nElemWaveform <= 0 || nElemWaveform > WV_SIZE_MAX )
	{
		printf( "Error: Unsupported waveform size %d\n", nElemWaveform );
		return -1;
	}

	status = ezcaGet( waveform_NORD,	ezcaLong, 1, &nOrdWaveform	);
	if ( status != EZCA_OK )
	{
		printf( "Error %d: Unable to read NORD PV %s\n", status, waveform_NORD );
		return -1;
	}
	if ( nOrdWaveform <= 0 || nOrdWaveform > nElemWaveform )
	{
		printf( "Error: Invalid waveform NORD %d\n", nOrdWaveform );
		return -1;
	}

	pBufferWaveform = callocMustSucceed( nElemWaveform, dbr_value_size[DBR_LONG], "Unable to allocate waveform buffer!" );
	memset( pBufferWaveform, 255, nElemWaveform * dbr_value_size[DBR_LONG] );

	count = nElemWaveform;
	if ( argc >= 3 )
	{
		status = sscanf( argv[2], "%d", &count );
		if ( status != 1 )
			count = nElemWaveform;
	}

	printf( "Reading %d elements from  waveform PV %s ...\n", count, pvName );
	status = ezcaGet( pvName,	ezcaLong, count, pBufferWaveform );
	if ( status != EZCA_OK )
	{
		printf( "Error %d: Unable to read %d elements from  waveform PV %s\n", status, count, pvName );
		return -1;
	}
	printf( "waveform PV %s: NELM = %d, NORD = %d, nRead = %d\n", pvName, nElemWaveform, nOrdWaveform, count );
	if ( count < 12 )
		count = 12;
	for ( i = 0; i < count; i++ )
	{
		/* Show the first 25 and the last 5 */
		if ( i >= 25 )
		{
			if ( i == 25 )
				printf( "...\n" );
			if ( i < (count - 5) )
				continue;
		}

		printf( "%d	", pBufferWaveform[i] );
		if ( (i % 20) == 19 )
			putchar( '\n' );
	}
	putchar( '\n' );
	printf( "Done.\n" );

	return 0;
}
