#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <stdbool.h>
#include <linux/types.h>

#include <private/android_filesystem_config.h>

#include "binder.h"
#include "test_server.h"

#if 1
#define ALOGI(x...) fprintf(stderr, "binder_c_test_server : " x)
#define ALOGE(x...) fprintf(stderr, "binder_c_test_server : " x)
#else
#define ALOGI(x...)
#define ALOGE(x...)
#endif


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

	if (binder_call(bs, &msg, &reply, target, SVC_MGR_ADD_SERVICE)) {
		ALOGE("binder_call failed : %s !", strerror(errno));
		return -1;
	}

	status = bio_get_uint32(&reply);
	binder_done(bs, &msg, &reply);

	return status;
}

void say_hello(void)
{
	static int cnt = 0;
	ALOGI("say_hello : %d .\n", ++ cnt);
}

int say_hello_to(char *name)
{
	static int cnt = 0;
	ALOGI("say_hello_to %s : %d .\n", name, ++ cnt);
	return cnt;
}

int hello_service_handle(struct binder_state *bs, struct binder_transaction_data *txn, struct binder_io *msg, struct binder_io *reply)
{
	/*
	 * 根据txn->code 知道要调用哪一个函数
	 * 如果需要参数，可以从msg中取出
	 * 如果要返回结果， 可以把结果放入到reply
	 */

	/**
	 * throught 'txn->code', yon can know call different function
	 *
	 * when need parameter, you can get from 'msg' 
	 * 
	 * when nedd reply result, you can put return reslut to 'reply'
	 *
	 ******/

	/***
	* say_hello
	* say_hello_to
	*
	******/

	uint16_t *s = NULL;
	uint16_t *s_iservice_name = NULL;
	char name[512] = {0};
	char iservice_name[512] = {0};
	size_t len = 0;
	uint32_t handle = 0;
	uint32_t strict_policy = 0;
	size_t i = 0;

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
			/* 从 msg 里面取出字符串 */
			/* get string for 'msg' */
			s_iservice_name = bio_get_string16(msg, &len);
			if (s_iservice_name == NULL)
			{
				ALOGE("hello service handle : sayhelle_to get iservice name failed : s_iservice_name = NULL!!\n");
				return -1;
			}
			for (i = 0; i < len; i ++)
				iservice_name[i] = s_iservice_name[i];
			iservice_name[i] = '\0';
			ALOGI("hello service handle : sayhello_to get iservice name = %s.\n", iservice_name);
			
			s = bio_get_string16(msg, &len);
			if (s == NULL) 
			{
				ALOGE("hello service handle : sayhelle_to get service name failed : name = NULL!!\n");
				return -1;
			}
			for (i = 0; i < len; i ++) 
				name[i] = s[i];
			name[i] = '\0';

			/* 处理 */
			/* handler data */
			i = say_hello_to(name);

			/* compatible java os : you can put no exception '0' */
			bio_put_uint32(reply, 0);
			
			/* 把结果放入 reply */
			/* put result 'reply' */
			bio_put_uint32(reply, i);

			break;

		default:
			ALOGE("unknow code %d !\n", txn->code);
			return -1;

	}

	return 0;
}


void say_goodbye(void)
{
	static int cnt = 0;
	ALOGI("say_goodbye : %d .\n", ++ cnt);
}

int say_goodbye_to(char *name)
{
	static int cnt = 0;
	ALOGI("say_goodbye_to :  %s : %d .\n", name, ++ cnt);
	return cnt;
}

int goodbye_service_handle(struct binder_state *bs, struct binder_transaction_data *txn, struct binder_io *msg, struct binder_io *reply)
{
	/*
	 * 根据txn->code 知道要调用哪一个函数
	 * 如果需要参数，可以从msg中取出
	 * 如果要返回结果， 可以把结果放入到reply
	 */


	/**
	 * throught 'txn->code', yon can know call different function
	 *
	 * when need parameter, you can get from 'msg' 
	 * 
	 * when nedd reply result, you can put return reslut to 'reply'
	 *
	 ******/
	 

	/***
	* say_hello
	* say_hello_to
	*
	******/

	uint16_t *s = NULL;
	uint16_t *s_iservice_name = NULL;
	char iservice_name[512] = {0};
	char name[512] = {0};
	size_t len = 0;
	uint32_t handle = 0;
	uint32_t strict_policy = 0;
	size_t i = 0;

	// Equivalent to Parcel::enforceInterface(), reading the RPC    
	// header with the strict mode policy mask and the interface name.    
	// Note that we ignore the strict_policy and don't propagate it    
	// further (since we do no outbound RPCs anyway).
	strict_policy = bio_get_uint32(msg);

	switch(txn->code) {
		case GOODBYE_SVC_CMD_SAY_GOODBYE:
			say_goodbye();
			return 0;

		case GOODBYE_SVC_CMD_SAY_GOODBYE_TO:
			/* 从 msg 里面取出字符串 */
			/* get string form msg */
			s_iservice_name = bio_get_string16(msg, &len); /* IGoodbyeService */
			if (s_iservice_name == NULL) 
			{
				ALOGE("goodbye service handler saygoodbye_to bio_get_string16 iservice_name error : s_iservice_name = NULL!!\n");
				return -1;
			}
			for (i = 0; i < len; i ++ )
				iservice_name[i] = s_iservice_name[i];
			iservice_name[i] = '\0';
			ALOGI("service handler saygoodbye_to : iservice_name = %s. \n", iservice_name);
			
			s = bio_get_string16(msg, &len); /* name */
			if (s == NULL) 
			{
				ALOGE("goodbye service handler saygoodbye_to bio_get_string16 name error : s = NULL!!\n");
				return -1;
			}
			for (i = 0; i < len; i ++) 
				name[i] = s[i];
			name[i] = '\0';

			/* 处理 */
			/* handle data */
			i = say_goodbye_to(name);

			/* compatible java os : you can put no exception '0' */
			bio_put_uint32(reply, 0);

			/* 把结果放入 reply */
			/* put reply data result */
			bio_put_uint32(reply, i);

			break;

		default:
			ALOGE("unknow code %d !\n", txn->code);
			return -1;

	}

	return 0;
}


int test_server_handle(struct binder_state *bs, struct binder_transaction_data *txn, struct binder_io *msg, struct binder_io *reply)
{
#if 0
	if (txn->target.ptr == 123)
	{
		return hello_service_handle(bs, txn, msg, reply);
	}
	else if (txn->target.ptr == 124) 
	{
		return goodbye_service_handle(bs, txn, msg, reply);
	}
	else 
	{
		ALOGI("test_server_handle error ! : txn->target.ptr = %d.\n", txn->target.ptr);
		return -1;
	}
#else
	int (*handle) (struct binder_state *bs, struct binder_transaction_data *txn, struct binder_io *msg, struct binder_io *reply);

	handle = (int (*) (struct binder_state *bs, struct binder_transaction_data *txn, struct binder_io *msg, struct binder_io *reply))txn->target.ptr;

	return handle(bs, txn, msg, reply);
#endif
}


//int main(int argc, char **argv)
int main(void)
{
	int fd;
	struct binder_state *bs;
	uint32_t svcmgr = BINDER_SERVICE_MANAGER;
	uint32_t handle;
	int ret;

	bs = binder_open(128*1024);
	if (!bs) {
		ALOGE("Failed to binder_open binder driver!\n");
		return -1;
	}

	#if 0
	/* add service */
	ret = svcmgr_publish(bs, svcmgr, "hello", (void *)123);
	if (ret) {
		fprintf(stderr, "failed to publish hello sevice!\n");
		return -1;
	}
	ret = svcmgr_publish(bs, svcmgr, "goodbye", (void *)124); // must common and test_server_handle
	if (ret) {
		fprintf(stderr, "failed to publish goodbye service!\n");
		return -1;
	}
	#else
	/* add service */
	ret = svcmgr_publish(bs, svcmgr, "hello", hello_service_handle);
	if (ret) {
		ALOGE("failed to publish hello sevice!\n");
		return -1;
	}
	#if 1 // mask goodbye server analyze data
	ret = svcmgr_publish(bs, svcmgr, "goodbye", goodbye_service_handle);
	if (ret) {
		ALOGE("failed to publish goodbye service!\n");
		return -1;
	}
	#endif
	#endif

	#if 0
	while(1)
	{
		/* read data */
		/* parse data, and process data */
		/* reply */
	}
	#endif

	binder_set_maxthreads(bs, 10);
	

	binder_loop(bs, test_server_handle);

	return 0;
	
}


