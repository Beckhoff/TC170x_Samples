#include <windows.h>
#include <stdlib.h>
#include <stdio.h>
#include "C:\Program Files (x86)\Beckhoff\TwinCAT\AdsApi\TcAdsDll\Include\TcAdsDef.h"
#include "C:\Program Files (x86)\Beckhoff\TwinCAT\AdsApi\TcAdsDll\Include\TcAdsApi.h"

//#pragma comment(lib, "C:\\Twincat\\AdsApi\\TcAdsDll\\x64\\lib\\TcAdsDll.lib")

#define	RTADSGRP_SYSDATA							0x0001
   #define	RTADSOFFS_SYSDATA_TICKNOW					0x0031
   #define	RTADSOFFS_SYSDATA_TICK_MODE				0x0032

int main()
{
	long nPort = AdsPortOpenEx();
	if (nPort == 0)
	{
		printf("Failed to open ads port\n");
		return -1;
	}
	
	AmsNetId netId = { 192, 168, 40, 24, 1, 1 };
	AmsAddr addr;

	addr.port = AMSPORT_R0_RTIME;
	addr.netId = netId;

	LONG nRes;
	ULONG mode = 0;
	ULONG cbRead;
	for (;;)
	{
		nRes = AdsSyncReadReqEx2(nPort, &addr, RTADSGRP_SYSDATA, RTADSOFFS_SYSDATA_TICK_MODE, sizeof(ULONG), &mode, &cbRead);
		if (nRes == ADSERR_NOERR && mode == 3)
		{
			break;
		}
		printf("waiting for rt mode changed to mode 3 ( current mode=%d err=0x%x).\n", mode, nRes);
		Sleep(1000);
	}


	for (;;)
	{
		ULONG nTicks = 100;
		nRes = AdsSyncWriteReqEx(nPort, &addr, RTADSGRP_SYSDATA, RTADSOFFS_SYSDATA_TICKNOW, sizeof(ULONG), &nTicks);

		if (nRes == ADSERR_NOERR)
		{
			printf("Successfully advanced RTime by %d ticks.\n", nTicks);
		}
		else
		{
			printf("Advancing RTime by %d ticks failed(err=0x%x).\n", nTicks, nRes);
			break;
		}

		if ((nRes = AdsSyncReadReqEx2(nPort, &addr, RTADSGRP_SYSDATA, RTADSOFFS_SYSDATA_TICK_MODE, sizeof(ULONG), &mode, &cbRead)) == ADSERR_NOERR && mode == 4)
		{
			printf("rt mode changed to mode=%d.\n", mode);
			break;
		}

		Sleep(500);
	}

}

