#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <stdbool.h>
#include <linux/types.h>

#include <private/android_filesystem_config.h>


#include "binder.h"
#include "test_server.h"

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

	/* 1 ����binder_io */
	bio_init(&msg, iodata,sizeof(iodata), 4);
	bio_put_uint32(&msg, 0);   // strict mode header

	/* 2 ������� */

	/* 3 ���� binder_call */
	if (binder_call(bs, &msg, &reply, handle, HELLO_SVC_CMD_SAY_HELLO))
		return ;

	/* 4 ��reply�н���������ֵ */


	binder_done(bs, &msg, &reply);
}

int say_hello_to(struct binder_state *bs, uint32_t handle, char *name)
{
	unsigned iodata[512/4];
	struct binder_io msg, reply;
	int ret;

	/* 1 ����binder_io */
	bio_init(&msg, iodata, sizeof(iodata), 4);
	bio_put_uint32(&msg, 0); // strict mode header

	/* 2 ������� */
	bio_put_string16_x(&msg, name);

	/* 3 binder_call */
	if (binder_call(bs, &msg, &reply, handle, HELLO_SVC_CMD_SAY_HELLO_TO))
		return 0;

	/* 4 ��reply�н���������ֵ */
	ret = bio_get_uint32(&reply);

	binder_done(bs, &msg, &reply);

	return ret;
}

/*
 * ./test_client hello
 * ./test_client hello <name>
 */

int main(int argc, char **argv)
{
	int fd;
	struct binder_state *bs;
	uint32_t svcmgr = BINDER_SERVICE_MANAGER;
	uint32_t handle;
	int ret;

	if (argc < 2) {
		fprintf(stderr, "Usage : ");
		fprintf(stderr, "%s hello ,", argv[0]);
		fprintf(stderr, "or %s hello <name> \n", argv[0]);
		return -1;
	}

	bs = binder_open(128*1024);
	if (!bs) {
		fprintf(stderr, "failed to ope binder driver!\n");
		return -1;
	}

	/* get service */
	handle = svcmgr_lookup(bs, svcmgr, (char *)"hello");
	if (!handle) {
		fprintf(stderr, "failed to get hello service !\n");
		return -1;
	}

	/* send data to server */
	if (argc == 2) {
		say_hello(bs, handle);
	} else if (argc == 3) {
		ret = say_hello_to(bs, handle, argv[2]);
		fprintf(stderr, "get ret of say_hello_to = %d . \n", ret);
	}

	binder_release(bs, handle);
	
	return 0;
}
