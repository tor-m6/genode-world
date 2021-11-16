/*
 * \brief  Implementation of some auxilary functions for golang runtime
 * \author Alexander Tormasov
 * \date   2020-11-17
 */

extern "C" {

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/uio.h>
int sendfile(int fd, int s, off_t offset, size_t nbytes,
             struct sf_hdtr *hdtr, off_t *sbytes, int flags) { return 0; }

#include <unistd.h>
pid_t getpgid(pid_t pid) { return 0; }

int lchown(const char *path, uid_t owner, gid_t group) { return 0; }

#include <sys/mount.h>
int mount(const char *source, const char *target, int f, void *data) { return 0; }

#include <sys/time.h>
int settimeofday(const struct timeval *tv , const struct timezone *tz) { return 0; }

#include <sys/mman.h>
int mlock(const void *addr, size_t len) { return 0; }
int munlock(const void *addr, size_t len) { return 0; }
int mlockall(int flags) { return 0; }
int munlockall(void) { return 0; }

#include <signal.h>
int sigaltstack(const stack_t *ss, stack_t *oss) { return 0; }

void __outgo_genode_log();
}

/* to produce call stack and halt */
#include <spec/x86_64/os/backtrace.h>
extern "C" int raise(int sig)
{
	Genode::log("raise ", sig);
	Genode::backtrace();
	return 0;
}

extern "C" void Log(const char * b) { Genode::log("Log: ",b); }

#include <util/string.h>

/* print to log from go using go string */
extern "C"
ssize_t __go_genode_write(int fd, const void *Buf, size_t count)
{
	static unsigned char b[1024];
	static unsigned int ptr=0; /* point to free byte */

	/* not safe in concurrent access */

	/* rest of buffer */
	const unsigned int size = sizeof(b)-1; /* available for data */
	unsigned int rest_buf;
	unsigned char *buf = (unsigned char *)Buf;
	again:	rest_buf = size - ptr;
	if (rest_buf) {
		while (count>0) {
			size_t to_copy = count > rest_buf ? rest_buf : count;
			Genode::memcpy(b+ptr,buf,to_copy);

			/* increment pointers */
			ptr += to_copy;
			count -= to_copy;
			buf += to_copy;
			rest_buf -= to_copy;

			/* print only if we have "\n" or no buf space */
			b[ptr] = 0; /* terminate string - safe because of size above */
			char *c;
			while ((c=(char*)__builtin_memchr(b,'\n',ptr)))	/* found, print */ {
				*c = 0; /* terminate string for ::log - replace \n */
				Genode::log("fd(", fd, "): ", (const char*)b);

				/* squeeze buffer */
				ptr -= c+1 - (char *)(&b[0]);
				rest_buf += c+1 - (char *)(&b[0]);
				Genode::memmove((void *)(&b[0]), c+1, ptr);
				b[ptr] = 0;
			}
		}
	} else {
		Genode::log("fd(", fd, "): ", (const char*)b);
		ptr = 0;
		goto again;
	}
	return count;
}

#include <base/thread.h>

using namespace Genode;

/* Sleep for some number of microseconds.  */
#include <timer_session/connection.h>

static Timer::Connection *connection_ptr = 0;

extern "C" void
my_usleep(uint32_t us)
{
	connection_ptr->usleep(us);
}
/******************
 ** Startup code **
 ******************/

#include <libc/component.h>
#include <libc/args.h>

extern char **genode_argv;
extern int genode_argc;
extern char **genode_envp;

/* initial environment for the FreeBSD libc implementation */
extern char **environ;

/* provided by the application */
extern "C" int main(int argc, char **argv, char **envp);

static void construct_component(Libc::Env &env)
{
	populate_args_and_env(env, genode_argc, genode_argv, genode_envp);

	environ = genode_envp;

	exit(main(genode_argc, genode_argv, genode_envp));
}


namespace Libc {

	void anon_init_file_operations(Genode::Env &env,
								   Xml_node const &config_accessor);
}

extern "C" void wait_for_continue();

void Libc::Component::construct(Libc::Env &env)
{
	static ::Timer::Connection connection(env);
	connection_ptr = &connection;
	// wait_for_continue();

	Libc::anon_init_file_operations(env, env.libc_config());

	Libc::with_libc([&]() { construct_component(env); });
}

#include <sys/ucontext.h>

/* base-internal includes */
#include <base/internal/stack_area.h>

/* to be executed on new stack after setcontext */
extern "C"
void othercontext(void **pmyself)
{
	Thread *myself = Thread::myself();
	*pmyself = myself;
}

/*
 * before actual call of setcontext copy over Stack and utcb/native_thread objects
 */
extern "C"
int innosetcontext(ucontext_t *uc)
{
	/*
	 * current OS thread should be non-main
	 */
	Thread *myself = Thread::myself();
	if (!myself)
	{
		error(__func__, ": main thread? base ", myself);
		throw -1;
	}

	/*
	 * technically we have 4 stacks:
	 * current stack (rsp) having Thread* _thread inside, prefix c
	 * primary OS stack _thread->_stack - whith the same OS thread info, prefix p
	 * new stack - Thread * _nthread inside to be set to _thread with data, prefix n
	 * new primary os stack _nthread->_stack - not touched
	 */
	addr_t const cstack_top = (addr_t)myself->stack_top();
	addr_t const cstack_base = (addr_t)myself->stack_base();

	/*
	 * some position inside stack after setcontext()
	 */
	addr_t nfrom_stack = (addr_t)(uc->uc_mcontext.mc_rsp);

	/* if our stack to be set is outside current stack
	 * than this could be not the same OS thread (it can have plurality of stacks)
	 */
	if (nfrom_stack < cstack_base || nfrom_stack >= cstack_top)
	{
		/* 
		 * we outside current stack - probalby we will switch to another OS thread
		 * lets check this by understanding Thread* address for new stack and
		 * compare it with myself - by running myself inside function with new stack
		 */
		Thread *new_myself = nullptr;
		ucontext_t T1, Main;
		T1.uc_mcontext.mc_len =
			Main.uc_mcontext.mc_len = sizeof(Main.uc_mcontext);

		/* execute function othercontext() in another stack using local var pointer */
		getcontext(&T1);
		T1.uc_link = &Main;
		T1.uc_stack.ss_sp = (void *)(uc->uc_mcontext.mc_rsp - MINSIGSTKSZ);
		T1.uc_stack.ss_size = MINSIGSTKSZ;
		makecontext(&T1, (void (*)()) & othercontext, 1, &new_myself);
		swapcontext(&Main, &T1);

		// log(__func__, ": cur thread* ", Hex(addr_t(myself)), " stack ", Hex_range(cstack_base, cstack_top - cstack_base),
		// 	" =>nsp ", Hex(nfrom_stack));
		if (new_myself && new_myself != myself)
		{
			/*
			 * need copy stack data because we will switch to another os thread
			 *
			 * !!! FIXME potentially dangerous because any Genode objects/caps
			 * created/accessed from original thread could be not accessible 
			 * (or accessible with different permissions);
			 * need to wrap their callers and take some measures, on case-by-case
			 * eg by redirecting operations with them to original OS thread
			 * 
			 * TBD: need to have a registry and process such objects in the OS thread context
			 */
			// log(__func__, ": old invalid thread* ", Hex(addr_t(new_myself)));

			/*
			 * copy size will be from original beginning of stack structure stored
			 * in the _stack field in the Thread class till the end of stack allocation
			 * stack_base .. stack_top .. _stack .. stack_base+area_size
			 * we need sizeof(Stack) only, while it contain per-OS native 
			 * thread/utcb structures, as well as C++ vtab
			 * fields to copy:
			 * _stack[]				no need to copy
			 * _libc_tls_pointer	no need to copy should be already the same
			 * _name				no need to copy
			 * _thread	need to copy as related to os thread
			 * _base	should not be overrided because it is from current stack
			 * _ds_cap	should not be overrided because it is from current stack
			 * _native_thread	need to copy as related to os thread
			 * _utcb			need to copy as related to os thread
			 * so, technically we have to copy everything after 
			 */
			size_t cend_stack = ((cstack_base) & ~(stack_virtual_size() - 1)) + stack_virtual_size();
			addr_t const cstack_abase = cstack_base & ~(stack_virtual_size() - 1);
			addr_t const nstack_abase = nfrom_stack & ~(stack_virtual_size() - 1);

			/*
			 * address inside primary thread stack
			 * cant use it directly - could be differ from current
			 * even for the same Thread*
			 * use it only to calculate native_off (same for all stacks)
			 */
			addr_t mnative = (addr_t)&myself->native_thread();
			size_t native_off = mnative - (mnative & ~(stack_virtual_size() - 1));

			/* adresses of native_thread field inside stack */
			addr_t cnative = cstack_abase + native_off;
			addr_t new_native = nstack_abase + native_off;

			/* use info about Stack field order from stack.h for Thread& field offset */
			addr_t thr_off = sizeof(Ram_dataspace_capability) + sizeof(addr_t) + sizeof(void *);

			/* copy Thread& field from current stack to new stack */
			*((void **)(new_native - thr_off)) = *((void **)(cnative - thr_off));

			/* copy pthread* filed from current stack to new stack */
			Thread::Stack_info info = Thread::mystack();
			*((void **)(nstack_abase + info.libc_tls_pointer_offset))
				= *((void **)(cstack_abase + info.libc_tls_pointer_offset));

			/* copy rest without last page (could be stack guard if enough space) */
			size_t size = cend_stack - size_t(cnative);
			/* FIXME: linux version does not have guard page in the end while others has */
			size = size > 0x1000 ? size - 0x1000 : size;
			Genode::memcpy((void *)new_native, (void *)cnative, size);
		}
	}

	return setcontext(uc);
}
