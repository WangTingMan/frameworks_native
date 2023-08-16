#pragma once

#ifndef pid_t
#define pid_t int
#endif

#define SELINUX_CB_LOG		0
#define SELINUX_CB_AUDIT	1
#define SELINUX_CB_VALIDATE	2
#define SELINUX_CB_SETENFORCE	3
#define SELINUX_CB_POLICYLOAD	4

/* file contexts */
#define SELABEL_CTX_FILE	0
/* media contexts */
#define SELABEL_CTX_MEDIA	1
/* x contexts */
#define SELABEL_CTX_X		2
/* db objects */
#define SELABEL_CTX_DB		3
/* Android property service contexts */
#define SELABEL_CTX_ANDROID_PROP 4
/* Android service contexts */
#define SELABEL_CTX_ANDROID_SERVICE 5
/* Android keystore key contexts */
#define SELABEL_CTX_ANDROID_KEYSTORE2_KEY 6

typedef unsigned short security_class_t;

struct selabel_handle
{

};

/* Callback facilities */
union selinux_callback
{
	/* log the printf-style format and arguments,
	   with the type code indicating the type of message */
	int
#ifdef __GNUC__
		__attribute__((format(printf, 2, 3)))
#endif
		(*func_log) (int type, const char* fmt, ...);
	/* store a string representation of auditdata (corresponding
	   to the given security class) into msgbuf. */
	int (*func_audit) (void* auditdata, security_class_t cls,
		char* msgbuf, size_t msgbufsize);
	/* validate the supplied context, modifying if necessary */
	int (*func_validate) (char** ctx);
	/* netlink callback for setenforce message */
	int (*func_setenforce) (int enforcing);
	/* netlink callback for policyload message */
	int (*func_policyload) (int seqno);
};

/* Free the memory allocated for a context by any of the below get* calls. */
void freecon(char* con);

/* Get context of process identified by pid, and
   set *con to refer to it.  Caller must free via freecon. */
int getpidcon(pid_t pid, char** con);
int getpidcon_raw(pid_t pid, char** con);

int getcon(char** con);

/* selinux_status_updated - Inform us whether the kernel status has been updated */
int selinux_status_updated(void);

/**
 * selabel_close - Close a labeling handle.
 * @handle: specifies handle to close
 *
 * Destroy the specified handle, closing files, freeing allocated memory,
 * etc.  The handle may not be further used after it has been closed.
 */
void selabel_close(struct selabel_handle* handle);

struct selabel_handle* selinux_android_vendor_service_context_handle(void);

struct selabel_handle* selinux_android_service_context_handle(void);

void selinux_set_callback(int type, union selinux_callback cb);

int selinux_vendor_log_callback(int type, const char* fmt, ...);

int selinux_log_callback(int type, const char* fmt, ...);

/**
 * selinux_status_open - Open and map SELinux kernel status page
 *
 */
int selinux_status_open(int fallback);

int selinux_check_access(const char* scon, const char* tcon, const char* class_, const char* perm, void* aux);

int selabel_lookup(struct selabel_handle* rec, char** con,
	const char* key, int type);

