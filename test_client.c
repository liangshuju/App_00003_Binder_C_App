#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <stdbool.h>
#include <linux/types.h>
#include <private/android_filesystem_config.h>

#include "binder.h"
#include "test_server.h"

#if 1
#define ALOGI(x...) fprintf(stderr, "binder_c_test_client : " x)
#define ALOGE(x...) fprintf(stderr, "binder_c_test_client : " x)
#else
#define ALOGI(x...)
#define ALOGE(x...)
#endif

uint32_t svcmgr_lookup(struct binder_state *bs, uint32_t target, const char *name)
{
	uint32_t handle;
	unsigned iodata[512/4];
	struct binder_io msg;
	struct binder_io reply;
	//struct binder_io msg, reply;

	bio_init(&msg, iodata, sizeof(iodata), 4);
	bio_put_uint32(&msg, 0); // strict mode header
	bio_put_string16_x(&msg, SVC_MGR_NAME);
	bio_put_string16_x(&msg, name);

	if (binder_call(bs, &msg, &reply, target, SVC_MGR_CHECK_SERVICE))
		return 0;

	handle = bio_get_ref(&reply);
	if (handle)
		binder_acquire(bs, handle);

	binder_done(bs, &msg, &reply);

	return handle;
}

void say_hello(struct binder_state *bs, uint32_t handle)
{
	unsigned iodata[512/4];	
	struct binder_io msg, reply;

	/* 1 构造binder_io */
	/* 1 constructor 'binder_io' */
	bio_init(&msg, iodata,sizeof(iodata), 4);
	bio_put_uint32(&msg, 0);   // strict mode header
	bio_put_string16_x(&msg, (char *)HELLO_SERVER_DESCRIPTOR); /* compatible iservice_name */ 
	
	/* 2 放入参数 */
	/* 2 put parameter */
	
	/* 3 调用 binder_call */
	/* 3 you can call 'binder_call' */
	if (binder_call(bs, &msg, &reply, handle, HELLO_SVC_CMD_SAY_HELLO))
		return ;

	/* 4 从reply中解析出返回值 */
	/* 4 parse return value form 'reply' */

	binder_done(bs, &msg, &reply);
}

int say_hello_to(struct binder_state *bs, uint32_t handle, char *name)
{
	unsigned iodata[512/4];
	struct binder_io msg, reply;
	int ret = -1;

	/* 1 构造binder_io */
	/* 1 constructor 'binder_io' */
	bio_init(&msg, iodata, sizeof(iodata), 4);
	bio_put_uint32(&msg, 0); // strict mode header
	bio_put_string16_x(&msg, (char *)HELLO_SERVER_DESCRIPTOR); /* compatible iservice_name */ 

	/* 2 放入参数 */
	/* 2 you can put parameter */
	bio_put_string16_x(&msg, name);

	/* 3 binder_call */
	if (binder_call(bs, &msg, &reply, handle, HELLO_SVC_CMD_SAY_HELLO_TO))
		return 0;

	/* 4 从reply中解析出返回值 */
	/* 4 parse return value form 'reply' */

	int java_flag_execption = bio_get_uint32(&reply);
	if (java_flag_execption) 
	{
		ALOGE("sayhell_to get error : java_flag_execption = %d.\n", java_flag_execption);
		return -1;
	}
	else
	{
		ret = bio_get_uint32(&reply);
	}
	
	binder_done(bs, &msg, &reply);
	return ret;
}

void say_goodbye(struct binder_state *bs, uint32_t handle)
{
	unsigned iodata[512/4];	
	struct binder_io msg, reply;

	/* 1 构造binder_io */
	/* 2 constructor 'binder_io' */
	bio_init(&msg, iodata,sizeof(iodata), 4);
	bio_put_uint32(&msg, 0);   // strict mode header
	bio_put_string16_x(&msg, (char *)GOODBYE_SERVICE_DESCRIPTOR); /* compatible iservice_name */ 

	/* 2 放入参数 */
	/* 2 put parameter */

	/* 3 调用 binder_call */
	/* 3 call 'binder_call' */
	if (binder_call(bs, &msg, &reply, handle, GOODBYE_SVC_CMD_SAY_GOODBYE))
		return ;

	/* 4 从reply中解析出返回值 */
	/* 4 parse return value form 'reply' */


	binder_done(bs, &msg, &reply);
}

int say_goodbye_to(struct binder_state *bs, uint32_t handle, char *name)
{
	unsigned iodata[512/4];
	struct binder_io msg, reply;
	int ret = 0;

	/* 1 构造binder_io */
	/* 2 constructor 'binder_io' */
	bio_init(&msg, iodata, sizeof(iodata), 4);
	bio_put_uint32(&msg, 0); // strict mode header
	bio_put_string16_x(&msg, (char *)GOODBYE_SERVICE_DESCRIPTOR); /* compatible iservice_name */ 

	/* 2 放入参数 */
	/* 2 put parameter */
	bio_put_string16_x(&msg, name);

	/* 3 binder_call */
	if (binder_call(bs, &msg, &reply, handle, GOODBYE_SVC_CMD_SAY_GOODBYE_TO))
		return 0;

	/* 4 从reply中解析出返回值 */
	/* 4 parse return value form 'reply' */
	int java_flag_execption = bio_get_uint32(&reply);
	if (java_flag_execption)
	{
		ALOGE("saygoodbye_to get java_flag_execption = %d.\n", java_flag_execption);
		return -1;
	}
	else
	{
		ret = bio_get_uint32(&reply);
	}
	
	binder_done(bs, &msg, &reply);

	return ret;
}


/*
 * ./test_client hello
 * ./test_client hello <name>
 */
int main(int argc, char **argv)
{
	int fd = 0;
	struct binder_state *bs = NULL;
	uint32_t svcmgr = BINDER_SERVICE_MANAGER;
	uint32_t handle = 0;
	uint32_t helloHandle = 0;
	uint32_t goodbyeHandle = 0;
	int ret = 0;

	if (argc < 2) {
		ALOGE("Usage : ");
		ALOGE("%s <hello|goodbye> ,", argv[0]);
		ALOGE("or %s <hello|goodbye> <name> \n", argv[0]);
		return -1;
	}

	bs = binder_open(128*1024);
	if (!bs) {
		ALOGE("failed to ope binder driver!\n");
		return -1;
	}

	/* get service */
	handle = svcmgr_lookup(bs, svcmgr, (char *)"hello");
	if (!handle) 
	{
		ALOGE("failed to get hello service !\n");
		return -1;
	}
	helloHandle = handle;
	//ALOGI("hello client : helloHandler = %d.\n", helloHandle);

#if 1
	/* get service */
	handle = svcmgr_lookup(bs, svcmgr, (char *)"goodbye");
	if (!handle) 
	{
		ALOGE("failed to get hello service !\n");
		return -1;
	}
	goodbyeHandle = handle;
	//ALOGI("goodbye client : goodbyeHandle = %d.\n", goodbyeHandle);
#endif

	// handle value : get service befor/after 

	/* send data to server */
	if (!strcmp(argv[1], "hello")) 
	{
		if (argc == 2) 
		{
			say_hello(bs, helloHandle);
			ALOGI("test client call say_hello.\n");
		} 
		else if (argc == 3) 
		{
			ret = say_hello_to(bs, helloHandle, argv[2]);
			ALOGI("get return of say_hello_to = %d . \n", ret);
		}
	}
	else if (!strcmp(argv[1], "goodbye"))
	{
		if (argc == 2) 
		{
			say_goodbye(bs, goodbyeHandle);
			ALOGI("test client call say_goodbye.\n");
		} 
		else if (argc == 3) 
		{
			ret = say_goodbye_to(bs, goodbyeHandle, argv[2]);
			ALOGI("get return of say_hello_to = %d . \n", ret);
		}
	}
	else
	{
		ALOGE("Usage input error!\n");
		ALOGE("Usage : ");
		ALOGE("%s <hello|goodbye> ,", argv[0]);
		ALOGE("or %s <hello|goodbye> <name> \n", argv[0]);
	}

	binder_release(bs, helloHandle);
	//binder_release(bs, goodbyeHandle);
	
	return 0;
}
