#include <devices/timer.h>
#include <exec/io.h>
#include <exec/ports.h>
#include <proto/exec.h>
#include "amintp/time_source.h"
int amintp_time_now(struct amintp_wallclock *out){struct MsgPort*p; struct timerequest*r; LONG e;if(!out)return 20;p=CreateMsgPort();if(!p)return 20;r=(struct timerequest*)CreateIORequest(p,sizeof(*r));if(!r){DeleteMsgPort(p);return 20;}e=OpenDevice((CONST_STRPTR)TIMERNAME,UNIT_MICROHZ,(struct IORequest*)r,0);if(e==0){r->tr_node.io_Command=TR_GETSYSTIME;e=DoIO((struct IORequest*)r);if(e==0){out->amiga_seconds=(uint32_t)r->tr_time.tv_secs;out->microseconds=(uint32_t)r->tr_time.tv_micro;}}if(e==0)CloseDevice((struct IORequest*)r);DeleteIORequest((struct IORequest*)r);DeleteMsgPort(p);return e==0?0:20;}
