/*
 * If not stated otherwise in this file or this component's Licenses.txt file the
 * following copyright and licenses apply:
 *
 * Copyright 2015 RDK Management
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
*/

/**********************************************************************
   Copyright [2014] [Cisco Systems, Inc.]
 
   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at
 
       http://www.apache.org/licenses/LICENSE-2.0
 
   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License.
**********************************************************************/

#ifdef __GNUC__
#if (!defined _BUILD_ANDROID) && (!defined _NO_EXECINFO_H_)
#include <execinfo.h>
#endif
#endif

#include "ssp_global.h"
#include "stdlib.h"
#include "ccsp_dm_api.h"
#include "ccsp_custom_logs.h"
#include "ccsp_WifiLog_wrapper.h"

/* Legacy Devices Like XB3 have systemd on the side with WiFi Agent, but don't use Service Files */
#if defined(ENABLE_SD_NOTIFY) && (defined(_XB6_PRODUCT_REQ_) || defined(_COSA_BCM_MIPS_)|| defined(_COSA_BCM_ARM_) || defined(_PLATFORM_IPQ_))
#include <systemd/sd-daemon.h>
#endif

#ifdef INCLUDE_BREAKPAD
#include "breakpad_wrapper.h"
#endif

#define DEBUG_INI_NAME  "/etc/debug.ini"
PDSLH_CPE_CONTROLLER_OBJECT     pDslhCpeController      = NULL;
PCOMPONENT_COMMON_DM            g_pComponent_Common_Dm  = NULL;
char                            g_Subsystem[32]         = {0};
PCCSP_COMPONENT_CFG             gpWifiStartCfg           = NULL;
PCCSP_FC_CONTEXT                pWifiFcContext           = (PCCSP_FC_CONTEXT           )NULL;
PCCSP_CCD_INTERFACE             pWifiCcdIf               = (PCCSP_CCD_INTERFACE        )NULL;
PCCC_MBI_INTERFACE              pWifiMbiIf               = (PCCC_MBI_INTERFACE         )NULL;
BOOL                            g_bActive               = FALSE;
static BOOL                     g_running               = TRUE;
int gChannelSwitchingCount = 0;
int  cmd_dispatch(int  command)
{
    ULONG                           ulInsNumber        = 0;
    parameterValStruct_t            val[3]             = {0};
    char*                           pParamNames[]      = {"Device.X_CISCO_COM_DDNS."};
    parameterValStruct_t**          ppReturnVal        = NULL;
    parameterInfoStruct_t**         ppReturnValNames   = NULL;
    parameterAttributeStruct_t**    ppReturnvalAttr    = NULL;
    ULONG                           ulReturnValCount   = 0;
    ULONG                           i                  = 0;

    switch ( command )
    {
            case	'e' :

#ifdef _ANSC_LINUX
                CcspTraceInfo(("Connect to bus daemon...\n"));

            {
                char                            CName[256];

                if ( g_Subsystem[0] != 0 )
                {
                    _ansc_sprintf(CName, "%s%s", g_Subsystem, gpWifiStartCfg->ComponentId);
                }
                else
                {
                    _ansc_sprintf(CName, "%s", gpWifiStartCfg->ComponentId);
                }

                ssp_WifiMbi_MessageBusEngage
                    ( 
                        CName,
                        CCSP_MSG_BUS_CFG,
                        gpWifiStartCfg->DbusPath
                    );
            }

#endif

                ssp_create_wifi(gpWifiStartCfg);
                ssp_engage_wifi(gpWifiStartCfg);

                g_bActive = TRUE;

                CcspTraceInfo(("Wifi Agent loaded successfully...\n"));

            break;

            case    'r' :

            CcspCcMbi_GetParameterValues
                (
                    DSLH_MPA_ACCESS_CONTROL_ACS,
                    pParamNames,
                    1,
                    &ulReturnValCount,
                    &ppReturnVal,
                    NULL
                );



            for ( i = 0; i < ulReturnValCount; i++ )
            {
                CcspTraceWarning(("Parameter %d name: %s value: %s \n", i+1, ppReturnVal[i]->parameterName, ppReturnVal[i]->parameterValue));
            }

			break;

        case    'm':
                AnscPrintComponentMemoryTable(pComponentName);

                break;

        case    't':
                AnscTraceMemoryTable();

                break;

        case    'c':
                ssp_cancel_wifi(gpWifiStartCfg);

                break;

        default:
            break;
    }

    return 0;
}

static void _print_stack_backtrace(void)
{
#ifdef __GNUC__
#if (!defined _BUILD_ANDROID) && (!defined _NO_EXECINFO_H_)
        void* tracePtrs[100];
        char** funcNames = NULL;
        int i, count = 0;

        count = backtrace( tracePtrs, 100 );
        backtrace_symbols_fd( tracePtrs, count, 2 );

        funcNames = backtrace_symbols( tracePtrs, count );

        if ( funcNames ) {
            // Print the stack trace
            for( i = 0; i < count; i++ )
                printf("%s\n", funcNames[i] );

            // Free the string pointers
            free( funcNames );
        }
#endif
#endif
}

#if defined(_ANSC_LINUX)
static void daemonize(void) {
	int fd;
	switch (fork()) {
	case 0:
		break;
	case -1:
		// Error
		CcspTraceInfo(("Error daemonizing (fork)! %d - %s\n", errno, strerror(
				errno)));
		exit(0);
		break;
	default:
		_exit(0);
	}

	if (setsid() < 	0) {
		CcspTraceInfo(("Error demonizing (setsid)! %d - %s\n", errno, strerror(errno)));
		exit(0);
	}

    /*
     *  What is the point to change current directory?
     *
    chdir("/");
     */

#ifndef  _DEBUG

	fd = open("/dev/null", O_RDONLY);
	if (fd != 0) {
		dup2(fd, 0);
		close(fd);
	}
	fd = open("/dev/null", O_WRONLY);
	if (fd != 1) {
		dup2(fd, 1);
		close(fd);
	}
	fd = open("/dev/null", O_WRONLY);
	if (fd != 2) {
		dup2(fd, 2);
		close(fd);
	}
#endif
}

void sig_handler(int sig)
{

    if ( sig == SIGINT ) {
#ifdef INCLUDE_GPERFTOOLS
        g_running = FALSE;
#else
    	signal(SIGINT, sig_handler); /* reset it to this function */
    	CcspTraceInfo(("SIGINT received!\n"));
        exit(0);
#endif
    }
    else if ( sig == SIGUSR1 ) {
    	signal(SIGUSR1, sig_handler); /* reset it to this function */
    	CcspTraceInfo(("SIGUSR1 received!\n"));
    }
    else if ( sig == SIGUSR2 ) {
    	CcspTraceInfo(("SIGUSR2 received!\n"));
    }
    else if ( sig == SIGCHLD ) {
    	signal(SIGCHLD, sig_handler); /* reset it to this function */
    	CcspTraceInfo(("SIGCHLD received!\n"));
    }
    else if ( sig == SIGPIPE ) {
    	signal(SIGPIPE, sig_handler); /* reset it to this function */
    	CcspTraceInfo(("SIGPIPE received!\n"));
    }
	else if ( sig == SIGALRM ) {

    	signal(SIGALRM, sig_handler); /* reset it to this function */
    	CcspTraceInfo(("SIGALRM received!\n"));
		gChannelSwitchingCount = 0;
    }
    else if ( sig == SIGTERM )
    {
#ifdef INCLUDE_GPERFTOOLS
        g_running = FALSE;
#else
        CcspTraceInfo(("SIGTERM received!\n"));
        exit(0);
#endif
    }
    else if ( sig == SIGKILL )
    {
        CcspTraceInfo(("SIGKILL received!\n"));
        exit(0);
    }
    else {
    	/* get stack trace first */
    	_print_stack_backtrace();
    	CcspTraceInfo(("Signal %d received, exiting!\n", sig));
    	exit(0);
    }
}

static int is_core_dump_opened(void)
{
    FILE *fp;
    char path[256];
    char line[1024];
    char *start, *tok, *sp;
#define TITLE   "Max core file size"

    snprintf(path, sizeof(path), "/proc/%d/limits", getpid());
    if ((fp = fopen(path, "rb")) == NULL)
        return 0;

    while (fgets(line, sizeof(line), fp) != NULL) {
        if ((start = strstr(line, TITLE)) == NULL)
            continue;

        start += strlen(TITLE);
        if ((tok = strtok_r(start, " \t\r\n", &sp)) == NULL)
            break;

        fclose(fp);

        if (strcmp(tok, "0") == 0)
            return 0;
        else
            return 1;
    }

    fclose(fp);
    return 0;
}

#endif

int main(int argc, char* argv[])
{
    ANSC_STATUS                     returnStatus       = ANSC_STATUS_SUCCESS;
    int                             cmdChar            = 0;
    BOOL                            bRunAsDaemon       = TRUE;
    int                             idx                = 0;
    char                            cmd[1024]          = {0};
    FILE                           *fd                 = NULL;
    DmErr_t                         err;
    char                            *subSys            = NULL;
    extern ANSC_HANDLE bus_handle;

    /*
     *  Load the start configuration
     */
#if defined(FEATURE_SUPPORT_RDKLOG)
        RDK_LOGGER_INIT();
#endif

    gpWifiStartCfg = (PCCSP_COMPONENT_CFG)AnscAllocateMemory(sizeof(CCSP_COMPONENT_CFG));
    
    if ( gpWifiStartCfg )
    {
        CcspComponentLoadCfg(CCSP_WIFI_START_CFG_FILE, gpWifiStartCfg);
    }
    else
    {
        printf("Insufficient resources for start configuration, quit!\n");
        exit(1);
    }
    
    /* Set the global pComponentName */
    pComponentName = gpWifiStartCfg->ComponentName;

#if defined(_DEBUG) && defined(_COSA_SIM_)
    AnscSetTraceLevel(CCSP_TRACE_LEVEL_INFO);
#endif

    for (idx = 1; idx < argc; idx++)
    {
        if ( (strcmp(argv[idx], "-subsys") == 0) )
        {
            AnscCopyString(g_Subsystem, argv[idx+1]);
            CcspTraceWarning(("\nSubsystem is %s\n", g_Subsystem));
        }
        else if ( strcmp(argv[idx], "-c" ) == 0 )
        {
            bRunAsDaemon = FALSE;
        }
    }

#if  defined(_ANSC_WINDOWSNT)

    AnscStartupSocketWrapper(NULL);

    display_info();

    cmd_dispatch('e');

    while ( cmdChar != 'q' )
    {
        cmdChar = getchar();

        cmd_dispatch(cmdChar);
    }
#elif defined(_ANSC_LINUX)
    if ( bRunAsDaemon )
        daemonize();

/* Legacy Devices Like XB3 have systemd on the side with WiFi Agent, but don't use Service Files */
#if defined(ENABLE_SD_NOTIFY) && (defined(_XB6_PRODUCT_REQ_) || defined(_COSA_BCM_MIPS_)|| defined(_COSA_BCM_ARM_) || defined(_PLATFORM_IPQ_))
    /*This is used for systemd */
    fd = fopen("/var/tmp/CcspWifiAgent.pid", "w+");
    if ( !fd )
    {
        CcspTraceWarning(("Create /var/tmp/CcspWifiAgent.pid error. \n"));
        return 1;
    }
    else
    {
        sprintf(cmd, "%d", getpid());
        fputs(cmd, fd);
        fclose(fd);
    }
#endif

#ifdef INCLUDE_BREAKPAD
    breakpad_ExceptionHandler();

	struct sigaction new_action, old_action;

	// we want to ignore SIGCHLD so that there wont be zombies
	new_action.sa_handler = SIG_IGN;
	new_action.sa_flags   = 0;
	sigemptyset (&new_action.sa_mask);
	if ( -1 == sigaction ( SIGCHLD, NULL, &old_action )) 
	{
		  CcspTraceWarning(("Problem getting original signal handler for SIGCHLD. Reason (%d) %s\n", errno, strerror(errno)));
	} 
	else 
	{ 
	   if ( SIG_IGN != old_action.sa_handler ) 
	   {
		 if ( -1 == sigaction ( SIGCHLD, &new_action, NULL ) ) 
		 {
			   CcspTraceWarning(("Problem setting signal handler for SIGCHLD. Reason (%d) %s\n", errno, strerror(errno)));
		 }
	   }
	}
#endif
    
#if defined(INCLUDE_GPERFTOOLS)
    signal(SIGINT, sig_handler);
    signal(SIGTERM, sig_handler);
#elif !defined(INCLUDE_BREAKPAD)
    if (is_core_dump_opened())
    {
        signal(SIGUSR1, sig_handler);
        CcspTraceWarning(("Core dump is opened, do not catch signal\n"));
    }
    else
    {
        CcspTraceWarning(("Core dump is NOT opened, backtrace if possible\n"));
    	signal(SIGTERM, sig_handler);
    	signal(SIGINT, sig_handler);
    	signal(SIGUSR1, sig_handler);
    	signal(SIGUSR2, sig_handler);

    	signal(SIGSEGV, sig_handler);
    	signal(SIGBUS, sig_handler);
    	signal(SIGKILL, sig_handler);
    	signal(SIGFPE, sig_handler);
    	signal(SIGILL, sig_handler);
    	signal(SIGQUIT, sig_handler);
    	signal(SIGHUP, sig_handler);
		signal(SIGALRM, sig_handler);
    }

#endif
    cmd_dispatch('e');

    // printf("Calling Docsis\n");

    // ICC_init();
    // DocsisIf_StartDocsisManager();

#ifdef _COSA_SIM_
    subSys = "";        /* PC simu use empty string as subsystem */
#else
    subSys = NULL;      /* use default sub-system */
#endif
    err = Cdm_Init(bus_handle, subSys, NULL, NULL, pComponentName);
    if (err != CCSP_SUCCESS)
    {
        fprintf(stderr, "Cdm_Init: %s\n", Cdm_StrError(err));
        exit(1);
    }
	
/* Legacy Devices Like XB3 have systemd on the side with WiFi Agent, but don't use Service Files */
#if defined(ENABLE_SD_NOTIFY) && (defined(_XB6_PRODUCT_REQ_) || defined(_COSA_BCM_MIPS_)|| defined(_COSA_BCM_ARM_) || defined(_PLATFORM_IPQ_))
    sd_notifyf(0, "READY=1\n"
              "STATUS=CcspWifiAgent is Successfully Initialized\n"
              "MAINPID=%lu", (unsigned long) getpid());
  
    CcspTraceInfo(("RDKB_SYSTEM_BOOT_UP_LOG : CcspWifiAgent sd_notify Called\n"));
#endif

    /* For some reason, touching the file via system command was not working consistently.
     * We'll fopen the file and dump in a value */
    if ((fd = fopen ("/tmp/wifi_initialized", "w+")) != NULL) {
        fprintf(fd,"1");
        fclose(fd);
    }

#ifdef _XB6_PRODUCT_REQ_
	system("/bin/sh /etc/webgui.sh");
#endif

    printf("Entering Wifi loop\n");
    CcspTraceWarning(("RDKB_SYSTEM_BOOT_UP_LOG : Entering Wifi loop \n"));
    if ( bRunAsDaemon )
    {
        while(g_running)
        {
            sleep(30);
        }
    }
    else
    {
        while ( cmdChar != 'q' && g_running)
        {
            cmdChar = getchar();

            cmd_dispatch(cmdChar);
        }
    }
#endif

    err = Cdm_Term();
    if (err != CCSP_SUCCESS)
    {
        fprintf(stderr, "Cdm_Term: %s\n", Cdm_StrError(err));
        exit(1);
    }

    if ( g_bActive )
    {
        ssp_cancel_wifi(gpWifiStartCfg);

        g_bActive = FALSE;
    }

    return 0;
}


