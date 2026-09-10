#include <stdio.h>
#include <string.h>

#include <exec/io.h>
#include <exec/memory.h>
#include <exec/ports.h>
#include <proto/exec.h>
#include <devices/sana2.h>

static void report(const char *name, struct IOSana2Req *req)
{
    printf("COMMAND=%s IOERR=%ld WIREERR=%lu\n", name,
           (long)req->ios2_Req.io_Error,
           (unsigned long)req->ios2_WireError);
    fflush(stdout);
}

int main(int argc, char **argv)
{
    struct MsgPort *port;
    struct IOSana2Req *req;
    struct Sana2DeviceQuery query;
    const char *command = argc > 1 ? argv[1] : "open";
    const char *device = argc > 2 ? argv[2] : "DEVS:Networks/a2065.device";
    ULONG open_error;

    puts("BEFORE_OPEN");
    fflush(stdout);
    port = CreateMsgPort();
    if (port == 0) { puts("CREATE_PORT=FAIL"); return 20; }
    req = (struct IOSana2Req *)CreateIORequest(port, sizeof(*req));
    if (req == 0) { puts("CREATE_REQUEST=FAIL"); DeleteMsgPort(port); return 20; }

    open_error = OpenDevice((CONST_STRPTR)device, 0,
                            (struct IORequest *)req, 0);
    printf("OPENERR=%lu\n", (unsigned long)open_error);
    fflush(stdout);
    if (open_error != 0) {
        DeleteIORequest((struct IORequest *)req);
        DeleteMsgPort(port);
        return 10;
    }
    if (!strcmp(command, "open")) goto close;

    req->ios2_Req.io_Command = S2_DEVICEQUERY;
    req->ios2_Req.io_Flags = IOF_QUICK;
    req->ios2_Data = (APTR)&query;
    req->ios2_DataLength = sizeof(query);
    DoIO((struct IORequest *)req);
    report("S2_DEVICEQUERY", req);
    printf("SIZE=%lu SUPPLIED=%lu ADDR_BITS=%u MTU=%lu HW=%lu\n",
           (unsigned long)query.SizeAvailable,
           (unsigned long)query.SizeSupplied,
           (unsigned)query.AddrFieldSize,
           (unsigned long)query.MTU,
           (unsigned long)query.HardwareType);
    fflush(stdout);
    if (!strcmp(command, "devicequery")) goto close;

    req->ios2_Req.io_Command = S2_GETSTATIONADDRESS;
    req->ios2_Req.io_Flags = IOF_QUICK;
    req->ios2_Data = 0;
    req->ios2_DataLength = 0;
    DoIO((struct IORequest *)req);
    report("S2_GETSTATIONADDRESS", req);
    if (!strcmp(command, "station")) goto close;

    req->ios2_Req.io_Command = S2_CONFIGINTERFACE;
    req->ios2_Req.io_Flags = IOF_QUICK;
    DoIO((struct IORequest *)req);
    report("S2_CONFIGINTERFACE", req);
    if (!strcmp(command, "config")) goto close;

    req->ios2_Req.io_Command = S2_ONLINE;
    req->ios2_Req.io_Flags = IOF_QUICK;
    DoIO((struct IORequest *)req);
    report("S2_ONLINE", req);

close:
    CloseDevice((struct IORequest *)req);
    puts("CLOSED");
    DeleteIORequest((struct IORequest *)req);
    DeleteMsgPort(port);
    return 0;
}
