#include <exec/types.h>
#include <exec/libraries.h>
#include <exec/ports.h>
#include <proto/exec.h>
#include <proto/rexxsyslib.h>
#include <rexx/storage.h>
#include <rexx/rxslib.h>
#include <string.h>

#include "amintp/arexx.h"
#include "amintp/platform.h"

struct Library *RexxSysBase;

static void reply_rexx(struct RexxMsg *msg, int rc, const char *result)
{
    msg->rm_Result1 = rc;
    msg->rm_Result2 = 0;

    if ((msg->rm_Action & RXFF_RESULT) != 0 && result != 0 && *result != '\0') {
        msg->rm_Result2 = (LONG)CreateArgstring((STRPTR)result, (LONG)strlen(result));
    }
    ReplyMsg((struct Message *)msg);
}

int amintp_arexx_run(void)
{
    struct MsgPort *port;
    ULONG signals;
    int running = 1;

    RexxSysBase = OpenLibrary(RXSNAME, 36);
    if (RexxSysBase == 0) return 20;

    if (FindPort((STRPTR)AMINTP_AREXX_PORT) != 0) {
        CloseLibrary(RexxSysBase);
        RexxSysBase = 0;
        return 20;
    }

    port = CreateMsgPort();
    if (port == 0) {
        CloseLibrary(RexxSysBase);
        RexxSysBase = 0;
        return 20;
    }

    port->mp_Node.ln_Name = (STRPTR)AMINTP_AREXX_PORT;
    AddPort(port);
    signals = 1UL << port->mp_SigBit;

    while (running) {
        ULONG got = Wait(signals | SIGBREAKF_CTRL_C);

        if ((got & SIGBREAKF_CTRL_C) != 0) running = 0;
        if ((got & signals) != 0) {
            struct RexxMsg *msg;
            while ((msg = (struct RexxMsg *)GetMsg(port)) != 0) {
                char result[AMINTP_AREXX_RESULT_SIZE];
                int rc;

                if (!IsRexxMsg(msg) || msg->rm_Args[0] == 0) {
                    reply_rexx(msg, 20, "ERROR CODE=INVALID_MESSAGE");
                    continue;
                }

                rc = amintp_arexx_dispatch((const char *)msg->rm_Args[0],
                                           result, sizeof(result));
                reply_rexx(msg, rc, result);
            }
        }
    }

    RemPort(port);
    DeleteMsgPort(port);
    CloseLibrary(RexxSysBase);
    RexxSysBase = 0;
    return 0;
}
