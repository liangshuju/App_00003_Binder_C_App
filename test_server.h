
#ifndef __TEST_SERVER_H__
#define __TEST_SERVER_H__

#if 0
#define HELLO_SVC_CMD_SAY_HELLO   0
#define HELLO_SVC_CMD_SAY_HELLO_TO 1

#define GOODBYE_SVC_CMD_SAY_GOODBYE   0
#define GOODBYE_SVC_CMD_SAY_GOODBYE_TO 1
#else //compatible java bind os
#define HELLO_SVC_CMD_SAY_HELLO   1
#define HELLO_SVC_CMD_SAY_HELLO_TO 2

#define GOODBYE_SVC_CMD_SAY_GOODBYE   1
#define GOODBYE_SVC_CMD_SAY_GOODBYE_TO 2

#define HELLO_SERVER_DESCRIPTOR    "IHelloService"
#define GOODBYE_SERVICE_DESCRIPTOR "IGoodbyeService"
#endif

#endif

