#include <devices/timer.h>
#include <exec/io.h>
#include <exec/memory.h>
#include <exec/ports.h>
#include <proto/exec.h>

#include "amintp/clock.h"

int amintp_set_system_time(const struct amintp_amiga_time *time)
{
    struct MsgPort *port;
    struct timerequest *request;
    LONG open_rc;
    LONG io_rc;

    if (time == 0 || time->micros >= 1000000UL) {
        return 20;
    }

    port = CreateMsgPort();
    if (port == 0) {
        return 20;
    }

    request = (struct timerequest *)CreateIORequest(port, sizeof(*request));
    if (request == 0) {
        DeleteMsgPort(port);
        return 20;
    }

    open_rc = OpenDevice(TIMERNAME, UNIT_MICROHZ,
                         (struct IORequest *)request, 0);
    if (open_rc != 0) {
        DeleteIORequest((struct IORequest *)request);
        DeleteMsgPort(port);
        return 20;
    }

    request->tr_node.io_Command = TR_SETSYSTIME;
    request->tr_time.tv_secs = time->seconds;
    request->tr_time.tv_micro = time->micros;

    io_rc = DoIO((struct IORequest *)request);

    CloseDevice((struct IORequest *)request);
    DeleteIORequest((struct IORequest *)request);
    DeleteMsgPort(port);

    return io_rc == 0 ? 0 : 20;
}
