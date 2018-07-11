#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

#include "binder.h"
#include "test_server.h"

int svcmgr_publish(struct binder_state *bs, uint32_t target, const char *name, void *ptr)
{
	int status;
	unsigned iodata[512/4];
	struct binder_io msg, reply;

	bio_init(&msg, iodata, sizeof(iodata), 4);
	bio_put_uint32(&msg, 0); // struct mode header
	bio_put_string16_x(&msg, SVC_MGR_NAME);
	bio_put_string16_x(&msg, name);
	bio_put_obj(&msg, ptr);

	if (binder_call(bs, &msg, &reply, target, SVC_MGR_ADD_SERVICE))
		return -1;

	status = bio_get_uint32(&reply);
	binder_done(bs, &msg, &reply);

	return status;
}

void say_hello(void)
{
	static int cnt = 0;
	fprintf(stderr, "say_hello : %d .\n", cnt ++);
}

int say_hello_to(char *name)
{
	static int cnt = 0;
	fprintf(stderr, "say_hello_to %s : %d .\n", name, cnt++);
	return cnt;
}

int hello_service_handle(struct binder_state *bs, struct binder_transaction_data *txn, 
		struct binder_io *msg, struct binder_io *reply)
{
	/*
	 * ����txn->code ֪��Ҫ������һ������
	 * �����Ҫ���������Դ�msg��ȡ��
	 * ���Ҫ���ؽ���� ���԰ѽ�����뵽reply
	 */

	/***
	* say_hello
	* say_hello_to
	*
	******/

	uint16_t *s;
	char name[512];
	size_t len;
	uint32_t handle;
	uint32_t strict_policy;
	int i;

	// Equivalent to Parcel::enforceInterface(), reading the RPC    
	// header with the strict mode policy mask and the interface name.    
	// Note that we ignore the strict_policy and don't propagate it    
	// further (since we do no outbound RPCs anyway).
	strict_policy = bio_get_uint32(msg);

	switch(txn->code) {
		case HELLO_SVC_CMD_SAY_HELLO:
			say_hello();
			return 0;

		case HELLO_SVC_CMD_SAY_HELLO_TO:
			/* �� msg ����ȡ���ַ��� */
			s = bio_get_string16(msg, &len);
			if (s == NULL) {
				return -1;
			}
			for (i = 0; i < len; i ++) 
				name[i] = s[i];
			name[i] = '\0';

			/* ���� */
			i = say_hello_to(name);

			/* �ѽ������ reply */
			bio_put_uint32(reply, i);

			break;

		default:
			ALOGE("unknow code %d !\n", txn->code);
			return -1;

	}

	return 0;
}

int main(int argc, char **argv)
{
	int fd;
	struct binder_state *bs;
	uint32_t svcmgr = BINDER_SERVICE_MANAGER;
	uint32_t handle;
	int ret;

	bs = binder_open(128*1024);
	if (!bs) {
		fprintf(stderr, "Failed to binder_open binder driver!\n");
		return -1;
	}

	/* add service */
	ret = svcmgr_publish(bs, svcmgr, "hello", "123");
	if (!ret) {
		fprintf(stderr, "failed to publish hello sevice!\n");
		return -1;
	}
	ret svcmgr_publish(bs, svcmgr, "goodbye", "123");
	if (!ret) {
		fprintf(stderr, "failed to publish goodbye service!\n");
		return -1;
	}

	#if 0
	while(1)
	{
		/* read data */
		/* parse data, and process */
		/* reply */
	}
	#endif
	binder_loop(bs, hello_service_handle);

	return 0;
	
}
