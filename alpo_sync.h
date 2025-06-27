/***************************************************************************
 * This file is part of alpo, a collection of single-header public domain  *
 * libraries. For more details, see <https://github.com/auul/alpo>.        *
 *                                                                         *
 * This is free and unencumbered software released into the public domain. *
 *                                                                         *
 * Anyone is free to copy, modify, publish, use, compile, sell, or         *
 * distribute this software, either in source code form or as a compiled   *
 * binary, for any purpose, commercial or non-commercial, and by any       *
 * means.                                                                  *
 *                                                                         *
 * In jurisdictions that recognize copyright laws, the author or authors   *
 * of this software dedicate any and all copyright interest in the         *
 * software to the public domain. We make this dedication for the benefit  *
 * of the public at large and to the detriment of our heirs and            *
 * successors. We intend this dedication to be an overt act of             *
 * relinquishment in perpetuity of all present and future rights to this   *
 * software under copyright law.                                           *
 *                                                                         *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,         *
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF      *
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  *
 * IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR       *
 * OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,   *
 * ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR   *
 * OTHER DEALINGS IN THE SOFTWARE.                                         *
 *                                                                         *
 * For more information, please refer to <https://unlicense.org>           *
 ***************************************************************************/

#ifndef ALPO_SYNC_H
#define ALPO_SYNC_H

/**
    \file alpo_sync.h
**/

/**
    \defgroup alpo_sync alpo_sync.h
    \brief A cross-platform synchronization library

    This header implements various synchronization primitives for coordinating
  concurrent threads within a single process. The goal of this library is to
  cover the majority of simple concurrency needs by providing a wide but shallow
  collection of features for as many platforms as possible behind a single,
  streamlined API; as such, certain features, like recursive mutexes, are not
  available even on platforms which implement them, but every available feature
  should behave as identically as possible on every supported platform.

    This file is part of `alpo`, a collection of single-header public domain
libraries. See github.com/auul/alpo for more details.

    @{
**/

// Namespace Aliasing

#define ALPO_SYNC_CONCAT_(a, b) a##b
#define ALPO_SYNC_CONCAT(a, b)  ALPO_SYNC_CONCAT_(a, b)

/**
    \defgroup namespace Namespace Aliasing

    The default prefix for functions and constants provided by this library is
  `alpo_sync_`. This prefix can be changed to any desired value (or no prefix at
  all), by defining the value of `ALPO_SYNC_ALIAS` prior to including this
  header file. This must be done in every source file in which this header is
  included.

    Example:

        #define ALPO_SYNC_ALIAS dogfood_
        #include "alpo_sync.h"

    The function `alpo_sync_mutex_init()` will now be compiled with the name
  `dogfood_mutex_init()`, and the original name will be undefined.

    @{
**/

#ifdef ALPO_SYNC_ALIAS
#	define ALPO_SYNC_API(name) ALPO_SYNC_CONCAT(ALPO_SYNC_ALIAS, name)
#else
#	define ALPO_SYNC_API(name) ALPO_SYNC_CONCAT(alpo_sync_, name)
#endif

/** @} */

// Define Booleans

#if defined(__has_include) && __has_include(<stdbool.h>)
#	include <stdbool.h>
#elif defined(__STDC_VERSION__) && __STDC_VERSION__ >= 199901L
#	include <stdbool.h>
#else
typedef int bool;
#	define true  1
#	define false 0
#endif

// Define Integer Types

#if defined(__has_include) && __has_include(<stdint.h>)
#	include <stdint.h>
#elif defined(__STDC_VERSION__) && __STDC_VERSION__ >= 199901L
#	include <stdint.h>
#else
typedef unsigned short uint16_t;
#	define UINT16_MAX 0xffff
#endif

// Detect POSIX Features

#if defined(__has_include) && __has_include(<pthread.h>)
#	define ALPO_SYNC_HAS_POSIX
#elif defined(__unix__)
#	include <unistd.h>
#	if defined(_POSIX_THREADS)
#		define ALPO_SYNC_HAS_POSIX
#	endif
#endif

#if defined(ALPO_SYNC_HAS_POSIX)
#	include <errno.h>
#	include <pthread.h>

#	define ALPO_SYNC_HAS_POSIX_MUTEXES
#	define ALPO_SYNC_HAS_POSIX_CONDS

#	if defined(_POSIX_BARRIERS)
#		define ALPO_SYNC_HAS_POSIX_BARRIERS
#	endif

#	if defined(_POSIX_READER_WRITER_LOCKS)
#		define ALPO_SYNC_HAS_POSIX_RWLOCKS
#	elif _POSIX_VERSION >= 200112L
#		define ALPO_SYNC_HAS_POSIX_RWLOCKS
#	endif

#	if defined(__has_include) && __has_include(<semaphore.h>)
#		define ALPO_SYNC_HAS_POSIX_SEMAPHORES
#	elif defined(_POSIX_SEMAPHORES)
#		define ALPO_SYNC_HAS_POSIX_SEMAPHORES
#	endif

#	if defined(_POSIX_SPIN_LOCKS)
#		define ALPO_SYNC_HAS_POSIX_SPINLOCKS
#	endif

#	if defined(PTHREAD_ONCE_INIT)
#		define ALPO_SYNC_HAS_POSIX_ONCE
#	endif
#endif

#if defined(ALPO_SYNC_HAS_POSIX_SEMAPHORES)
#	include <semaphore.h>
#endif

// Detect Windows Availability

#if defined(__has_include) && __has_include(<windows.h>)
#	define ALPO_SYNC_HAS_WIN
#elif defined(_WIN32) || defined(_WIN64)
#	define ALPO_SYNC_HAS_WIN
#endif

#if defined(ALPO_SYNC_HAS_WIN)
#	if _WIN32_WINNT >= _WIN32_WINNT_WIN8
#		define ALPO_SYNC_HAS_WIN_BARRIERS
#	endif

#	if _WIN32_WINNT >= _WIN32_WINNT_VISTA
#		define ALPO_SYNC_HAS_WIN_RWLOCKS
#		define ALPO_SYNC_HAS_WIN_CONDS
#		define ALPO_SYNC_HAS_WIN_ONCE
#	endif

#	if _WIN32_WINNT >= _WIN32_WINNT_WINXP
#		define ALPO_SYNC_HAS_WIN_CRITICALSECTIONS
#		define ALPO_SYNC_HAS_WIN_SEMAPHORES
#		define ALPO_SYNC_HAS_WIN_EVENTS
#	endif
#endif

// Detect C11 Availability

#if defined(__has_include) && __has_include(<threads.h>)
#	define ALPO_SYNC_HAS_C11
#elif defined(__STDC_VERSION__) && __STDC_VERSION__ >= 201112L \
	&& !defined(__STDC_NO_THREADS__)
#	define ALPO_SYNC_HAS_C11
#endif

#if defined(ALPO_SYNC_HAS_C11)
#	include <threads.h>
#	define ALPO_SYNC_HAS_C11_MUTEXES
#	define ALPO_SYNC_HAS_C11_CONDS
#	define ALPO_SYNC_HAS_C11_ONCE
#endif

// Detect Atomics Availability

#define ALPO_SYNC_HAS_C11_ATOMICS   6
#define ALPO_SYNC_HAS_GNUC_ATOMICS  5
#define ALPO_SYNC_HAS_WIN8_ATOMICS  4
#define ALPO_SYNC_HAS_VISTA_ATOMICS 3
#define ALPO_SYNC_HAS_SYNC_ATOMICS  2
#define ALPO_SYNC_HAS_WINXP_ATOMICS 1

#if defined(__has_include) && __has_include(<stdatomic.h>)
#	define ALPO_SYNC_HAS_ATOMICS ALPO_SYNC_HAS_C11_ATOMICS
#elif defined(__STDC_VERSION__) && __STDC_VERSION__ >= 201112L \
	&& !defined(__STDC_NO_ATOMICS__)
#	define ALPO_SYNC_HAS_ATOMICS ALPO_SYNC_HAS_C11_ATOMICS
#endif

#if defined(ALPO_SYNC_HAS_ATOMICS)
#elif defined(__clang__)
#	if __has_extension(__atomic__)
#		define ALPO_SYNC_HAS_ATOMICS ALPO_SYNC_HAS_GNUC_ATOMICS
#	elif __has_builtin(__sync_synchronize)
#		define ALPO_SYNC_HAS_ATOMICS ALPO_SYNC_HAS_SYNC_ATOMICS
#	endif
#endif

#if defined(ALPO_SYNC_HAS_ATOMICS)
#elif defined(__GNUC__)
#	if __GNUC__ > 4
#		define ALPO_SYNC_HAS_ATOMICS ALPO_SYNC_HAS_GNUC_ATOMICS
#	elif __GNUC__ == 4
#		if __GNUC_MINOR__ >= 7
#			define ALPO_SYNC_HAS_ATOMICS ALPO_SYNC_HAS_GNUC_ATOMICS
#		elif __GNUC_MINOR__ >= 1
#			define ALPO_SYNC_HAS_SYNC_ATOMICS
#		endif
#	endif
#endif

#if defined(ALPO_SYNC_HAS_ATOMICS) \
	&& ALPO_SYNC_HAS_ATOMICS >= ALPO_SYNC_HAS_GNUC_ATOMICS
#elif defined(_WIN32_WINNT)
#	if _WIN32_WINNT >= _WIN32_WINNT_WIN8
#		undef ALPO_SYNC_HAS_ATOMICS
#		define ALPO_SYNC_HAS_ATOMICS ALPO_SYNC_HAS_WIN8_ATOMICS
#	elif _WIN32_WINNT >= _WIN32_WINNT_VISTA
#		undef ALPO_SYNC_HAS_ATOMICS
#		define ALPO_SYNC_HAS_ATOMICS ALPO_SYNC_HAS_VISTA_ATOMICS
#	elif !defined(ALPO_SYNC_HAS_ATOMICS) && _WIN32_WINNT >= _WIN32_WINNT_WINXP
#		define ALPO_SYNC_HAS_ATOMICS ALPO_SYNC_HAS_WINXP_ATOMICS
#	endif
#endif

// Atomics Wrappers

#if !defined(ALPO_SYNC_IMPLEMENT) || !defined(ALPO_SYNC_HAS_ATOMICS)
#elif ALPO_SYNC_HAS_ATOMICS >= ALPO_SYNC_HAS_C11_ATOMICS
#	include <stdatomic.h>

#	define alpo_sync__atomic_flag atomic_flag
#	define alpo_sync__atomic_flag_acquire(ptr) \
		atomic_flag_test_and_set_explicit(ptr, memory_order_acquire)
#	define alpo_sync__atomic_flag_release(ptr) \
		atomic_flag_clear_explicit(ptr, memory_order_release)
#	define alpo_sync__atomic_flag_fence(ptr) \
		atomic_thread_fence(memory_order_release)

#elif ALPO_SYNC_HAS_ATOMICS >= ALPO_SYNC_HAS_GNUC_ATOMICS

#	define alpo_sync__atomic_flag volatile unsigned char
#	define alpo_sync__atomic_flag_acquire(ptr) \
		__atomic_test_and_set(ptr, __ATOMIC_ACQUIRE)
#	define alpo_sync__atomic_flag_release(ptr) \
		__atomic_clear(ptr, __ATOMIC_RELEASE)
#	define alpo_sync__atomic_flag_fence(ptr) \
		__atomic_thread_fence(__ATOMIC_RELEASE)

#elif ALPO_SYNC_HAS_ATOMICS >= ALPO_SYNC_HAS_WIN8_ATOMICS

#	define alpo_sync__atomic_flag              volatile unsigned char
#	define alpo_sync__atomic_flag_acquire(ptr) InterlockedOr8Acquire(ptr, 1)
#	define alpo_sync__atomic_flag_release(ptr) InterlockedAnd8Release(ptr, 0)
#	define alpo_sync__atomic_flag_fence(ptr)   InterlockedAnd8Release(ptr, 1)

#elif ALPO_SYNC_HAS_ATOMICS >= ALPO_SYNC_HAS_VISTA_ATOMICS

#	define alpo_sync__atomic_flag volatile unsigned int
#	define alpo_sync__atomic_flag_acquire(ptr) \
		InterlockedCompareExchangeAcquire(ptr, 1, 0)
#	define alpo_sync__atomic_flag_release(ptr) \
		InterlockedCompareExchangeRelease(ptr, 0, 1)
#	define alpo_sync__atomic_flag_fence(ptr) \
		InterlockedCompareExchangeRelease(ptr, 1, 0)

#elif ALPO_SYNC_HAS_ATOMICS >= ALPO_SYNC_HAS_SYNC_ATOMICS

#	define alpo_sync__atomic_flag              volatile unsigned char
#	define alpo_sync__atomic_flag_acquire(ptr) __sync_lock_test_and_set(ptr, 1)
#	define alpo_sync__atomic_flag_release(ptr) __sync_lock_release(ptr)
#	define alpo_sync__atomic_flag_fence(ptr)   __sync_synchronize()

#elif ALPO_SYNC_HAS_ATOMICS >= ALPO_SYNC_HAS_WINXP_ATOMICS

#	define alpo_sync__atomic_flag              volatile unsigned int
#	define alpo_sync__atomic_flag_acquire(ptr) InterlockedExchange(ptr, 1)
#	define alpo_sync__atomic_flag_release(ptr) InterlockedExchange(ptr, 0)
#	define alpo_sync__atomic_flag_fence(ptr)   InterlockedExchange(ptr, 1)

#endif

#if defined(ALPO_SYNC_HAS_ATOMICS) && !defined(ATOMIC_FLAG_INIT)
#	define ATOMIC_FLAG_INIT 0
#endif

// Status Codes

/**
    \defgroup status Status Codes
    \brief Return values used by this library

    @{
**/

typedef enum alpo_sync_status ALPO_SYNC_API(status);
const char *ALPO_SYNC_API(status_str)(enum alpo_sync_status status);

#if defined(ALPO_SYNC_IMPLEMENT) || defined(DOXYGEN)

/**
    \brief Values representing the outcome of an operation

    When returned from a function, these values represent the outcome of the
  operation, either success or a specific error indicator.
**/
enum alpo_sync_status {
	ALPO_SYNC_API(success), /**< Operation was successful */
	ALPO_SYNC_API(busy),    /**< Resource is busy */
	ALPO_SYNC_API(eagain),  /**< Resource is unavailable */
	ALPO_SYNC_API(enomem),  /**< Operation failed due to lack of memory */
	ALPO_SYNC_API(eperm),   /**< Operation was not permitted */
	ALPO_SYNC_API(einval),  /**< Operation failed due to invalid argument */
	ALPO_SYNC_API(edeadlk), /**< Resource is deadlocked */
	ALPO_SYNC_API(error)    /**< Operation failed due to some other error */
};

/**
    \brief Converts a status code to a user-friendly string

    This function converts the return value from a library function to a
  string explaining the outcome of the operation, suitable for printing to
  output.
    \param status The return value from a function
    \return A string explaining the status value
**/
const char *ALPO_SYNC_API(status_str)(enum alpo_sync_status status)
{
	switch (status) {
	case ALPO_SYNC_API(success):
		return "Success";
	case ALPO_SYNC_API(busy):
		return "Resource is busy";
	case ALPO_SYNC_API(eagain):
		return "Resource is unavailable";
	case ALPO_SYNC_API(enomem):
		return "Cannot allocate memory";
	case ALPO_SYNC_API(eperm):
		return "Operation not permitted";
	case ALPO_SYNC_API(einval):
		return "Invalid argument";
	case ALPO_SYNC_API(edeadlk):
		return "Resource deadlock";
	default:
		return "Unspecified error";
	}
}

#	if defined(ALPO_SYNC_HAS_POSIX)

static inline enum alpo_sync_status alpo_sync__posix_status(int status)
{
	switch (status) {
	case 0:
		return ALPO_SYNC_API(success);
	case EBUSY:
		return ALPO_SYNC_API(busy);
	case EAGAIN:
		return ALPO_SYNC_API(eagain);
	case ENOMEM:
		return ALPO_SYNC_API(enomem);
	case EPERM:
		return ALPO_SYNC_API(eperm);
	case EINVAL:
		return ALPO_SYNC_API(einval);
	case EDEADLK:
		return ALPO_SYNC_API(edeadlk);
	default:
		return ALPO_SYNC_API(error);
	}
}

#	endif
#	if defined(ALPO_SYNC_HAS_WIN)

static inline enum alpo_sync_status alpo_sync__win_status(int status)
{
	switch (status) {
	case ERROR_INVALID_HANDLE:
		return ALPO_SYNC_API(einval);
	case ERROR_ACCESS_DENIED:
		return ALPO_SYNC_API(eperm);
	case ERROR_INVALID_PARAMETER:
		return ALPO_SYNC_API(einval);
	case ERROR_NOT_ENOUGH_MEMORY:
		return ALPO_SYNC_API(enomem);
	default:
		return ALPO_SYNC_API(error);
	}
}

#	endif
#	if defined(ALPO_SYNC_HAS_C11)

static inline enum alpo_sync_status alpo_sync__c11_status(int status)
{
	switch (status) {
	case thrd_success:
		return ALPO_SYNC_API(success);
	case thrd_busy:
		return ALPO_SYNC_API(busy);
	default:
		return ALPO_SYNC_API(error);
	}
}

#	endif
#endif

/** @} */

// Exponential Backoff Macros

#if defined(ALPO_SYNC_IMPLEMENT)

#	ifndef ALPO_SYNC_MAX_SPIN_DELAY
#		define ALPO_SYNC_MAX_SPIN_DELAY 1024
#	endif

#	if defined(__SSE2__)
#		include <emmintrin.h>
#		define alpo_sync__pause() _mm_pause()
#	elif defined(__i386__) || defined(__x86_64__)
#		if defined(__has_builtin) && __has_builtin(__builtin_ia32_pause())
#			define alpo_sync__pause()
#		endif
#	endif

#	if !defined(alpo_sync__pause)
#		define alpo_sync__pause() ((void)0)
#	endif

#	define alpo_sync__spin(test)                       \
		{                                               \
			int delay = 1;                              \
			while (test) {                              \
				for (int i = 0; i < delay; i++) {       \
					alpo_sync__pause();                 \
				}                                       \
				delay *= 2;                             \
				if (delay > ALPO_SYNC_MAX_SPIN_DELAY) { \
					delay = ALPO_SYNC_MAX_SPIN_DELAY;   \
				}                                       \
			}                                           \
		}

#endif

// Mutex Typedef

/**
    \defgroup mutex Mutexes

    Mutexes allow for restricting access to a block of code to a single thread.
  Each thread must acquire sole ownership of the mutex object before proceeding;
  all other contending threads will wait until the mutex is released by the
  owning thread before attempting to acquire again (unless using
  `alpo_sync_mutex_trylock()`, which returns a value of `alpo_sync_busy`).

    As all concurrency primitives can be simulated using mutexes to guard
  non-concurrent objects, the ability to implement a mutex is the bare-minimum
  requirement for this library. Therefore, an attempt has been made to use the
  lowest-possible overhead version of a mutex on every supported platform. As a
  result, common features like recursive and timed locking are not supported,
  but this allows the library to behave more or less identically on systems with
  wildly different operating systems, or no operating system at all, as long as
  atomic intrinsics are available.

    @{
**/

#if defined(DOXYGEN)

/**
    \brief An opaque handle to a mutex object

    \note This is an opaque handle, and should be accessed only using API
  functions, as the underlying structure may change from platform to platform.
**/
typedef struct alpo_sync_mutex alpo_sync_mutex;

#elif defined(ALPO_SYNC_HAS_POSIX_MUTEXES)

typedef pthread_mutex_t alpo_sync_mutex;

#elif defined(ALPO_SYNC_HAS_WIN_RWLOCKS)

typedef SRWLOCK alpo_sync_mutex;

#elif defined(ALPO_SYNC_HAS_WIN_CRITICALSECTIONS)

typedef CRITICALSECTION alpo_sync_mutex;

#elif defined(ALPO_SYNC_HAS_C11_MUTEXES)

typedef mtx_t alpo_sync_mutex;

#elif defined(ALPO_SYNC_HAS_ATOMICS)

typedef alpo_sync__atomic_flag alpo_sync_mutex;

#else
#	error "No synchronization objects found"
#endif

typedef alpo_sync_mutex ALPO_SYNC_API(mutex);

// Mutex Functions

/**
    \brief Initializes a mutex object for use

    This function initializes a mutex object to its unlocked state and makes it
  available for use. This function must be called exactly once before any other
  functions on the mutex object; attempting to use an uninitialized mutex will
  result in undefined behavior, as will initializing an already initialized
  mutex.

    \param mutex A pointer to the mutex object
    \return One of `enum alpo_sync_status` indicating success or failure
**/
int ALPO_SYNC_API(mutex_init)(alpo_sync_mutex *mutex);

/**
    \brief Destroys a mutex object

    This function releases any resources allocated for a mutex object and makes
  it unavailable for use. This function should be called exactly once after the
  mutex object is no longer needed; failure to do so may result in a resource
  leak. Destroying an already destroyed mutex object will result in undefined
  behavior.

    \param mutex A pointer to the mutex object
    \return One of `enum alpo_sync_status` indicating success or failure
**/
int ALPO_SYNC_API(mutex_destroy)(alpo_sync_mutex *mutex);

/**
    \brief Locks a mutex object

    This function allows a single thread to acquire sole ownership of a mutex
  object. All threads contending for ownership will block until the mutex
  becomes available. This function should be called only once before calling
  `alpo_sync_mutex_unlock()`; locking an already locked or uninitialized mutex
  results in undefined behavior.

    \param mutex A pointer to the mutex object
    \return One of `enum alpo_sync_status` indicating success or failure
**/
int ALPO_SYNC_API(mutex_lock)(alpo_sync_mutex *mutex);

/**
    \brief Attempts to lock a mutex object

    This function allows a single thread to acquire sole ownership of a mutex
  object, but unlike `alpo_sync_mutex_lock()`, the current thread will not block
  if the mutex is in use. Check the return value to determine whether or not the
  mutex was acquired. Calling this function on an uninitialized mutex or one
  locked by the calling thread will result in undefined behavior.

    \param mutex A pointer to the mutex object
    \return `alpo_sync_success` on success, `alpo_sync_busy` if the mutex is
  currently in use by another thread, or one of `enum alpo_sync_status` to
  indicate other reason for failure.
**/
int ALPO_SYNC_API(mutex_trylock)(alpo_sync_mutex *mutex);

/**
    \brief Unlocks a mutex object

    This function allows a thread to release a mutex object back to other
  contending threads. This function should be called exactly once for every
  successful call to `alpo_sync_mutex_lock()` or `alpo_sync_mutex_trylock()`.
  Attempting to unlock an uninitialized mutex or one not owned by the calling
  thread will result in undefined behavior.

    \param mutex A pointer to the mutex object
    \return One of `enum alpo_sync_status` indicating success or failure
**/
int ALPO_SYNC_API(mutex_unlock)(alpo_sync_mutex *mutex);

#if !defined(ALPO_SYNC_IMPLEMENT)
#elif defined(ALPO_SYNC_HAS_POSIX_MUTEXES)
#	define ALPO_SYNC_USE_POSIX_MUTEXES

int ALPO_SYNC_API(mutex_init)(alpo_sync_mutex *mutex)
{
	return alpo_sync__posix_status(pthread_mutex_init(mutex, NULL));
}

int ALPO_SYNC_API(mutex_destroy)(alpo_sync_mutex *mutex)
{
	return alpo_sync__posix_status(pthread_mutex_destroy(mutex));
}

int ALPO_SYNC_API(mutex_lock)(alpo_sync_mutex *mutex)
{
	return alpo_sync__posix_status(pthread_mutex_lock(mutex));
}

int ALPO_SYNC_API(mutex_trylock)(alpo_sync_mutex *mutex)
{
	if (pthread_mutex_trylock(mutex) == 0) {
		return ALPO_SYNC_API(success);
	}
	return alpo_sync__posix_status(errno);
}

int ALPO_SYNC_API(mutex_unlock)(alpo_sync_mutex *mutex)
{
	return alpo_sync__posix_status(pthread_mutex_unlock(mutex));
}

#elif defined(ALPO_SYNC_HAS_WIN_RWLOCKS)
#	define ALPO_SYNC_USE_SRW_MUTEXES

int ALPO_SYNC_API(mutex_init)(alpo_sync_mutex *mutex)
{
	InitializeSRWLock(mutex);
	return ALPO_SYNC_API(success);
}

int ALPO_SYNC_API(mutex_destroy)(alpo_sync_mutex *mutex)
{
	return ALPO_SYNC_API(success);
}

int ALPO_SYNC_API(mutex_lock)(alpo_sync_mutex *mutex)
{
	return AcquireSRWLockExclusive(mutex);
}

int ALPO_SYNC_API(mutex_trylock)(alpo_sync_mutex *mutex)
{
	if (TryAcquireSRWLockExclusive(mutex)) {
		return ALPO_SYNC_API(success);
	}
	return ALPO_SYNC_API(busy);
}

int ALPO_SYNC_API(mutex_unlock)(alpo_sync_mutex *mutex)
{
	ReleaseSRWLockExclusive(mutex);
	return ALPO_SYNC_API(success);
}

#elif defined(ALPO_SYNC_HAS_WIN_CRITICALSECTIONS)
#	define ALPO_SYNC_USE_CS_MUTEXES

int ALPO_SYNC_API(mutex_init)(alpo_sync_mutex *mutex)
{
	InitializeCriticalSection(mutex);
	return ALPO_SYNC_API(success);
}

int ALPO_SYNC_API(mutex_destroy)(alpo_sync_mutex *mutex)
{
	DeleteCriticalSection(mutex);
	return ALPO_SYNC_API(success);
}

int ALPO_SYNC_API(mutex_lock)(alpo_sync_mutex *mutex)
{
	EnterCriticalSection(mutex);
	return ALPO_SYNC_API(success);
}

int ALPO_SYNC_API(mutex_trylock)(alpo_sync_mutex *mutex)
{
	if (TryEnterCriticalSection(mutex)) {
		return ALPO_SYNC_API(success);
	}
	return ALPO_SYNC_API(busy);
}

int ALPO_SYNC_API(mutex_unlock)(alpo_sync_mutex *mutex)
{
	LeaveCriticalSection(mutex);
	return ALPO_SYNC_API(success);
}

#elif defined(ALPO_SYNC_HAS_C11_MUTEXES)
#	define ALPO_SYNC_USE_C11_MUTEXES

int ALPO_SYNC_API(mutex_init)(alpo_sync_mutex *mutex)
{
	return alpo_sync__c11_status(mtx_init(mutex, mtx_plain));
}

int ALPO_SYNC_API(mutex_destroy)(alpo_sync_mutex *mutex)
{
	mtx_destroy(mutex);
	return ALPO_SYNC_API(success);
}

int ALPO_SYNC_API(mutex_lock)(alpo_sync_mutex *mutex)
{
	return alpo_sync__c11_status(mtx_lock(mutex));
}

int ALPO_SYNC_API(mutex_trylock)(alpo_sync_mutex *mutex)
{
	return alpo_sync__c11_status(mtx_trylock(mutex));
}

int ALPO_SYNC_API(mutex_unlock)(alpo_sync_mutex *mutex)
{
	return alpo_sync__c11_status(mtx_unlock(mutex));
}

#elif defined(ALPO_SYNC_HAS_ATOMICS)

int ALPO_SYNC_API(mutex_init)(alpo_sync_mutex *mutex)
{
	*mutex = (alpo_sync_mutex)ATOMIC_FLAG_INIT;
	return ALPO_SYNC_API(success);
}

int ALPO_SYNC_API(mutex_destroy)(alpo_sync_mutex *mutex)
{
	return ALPO_SYNC_API(success);
}

int ALPO_SYNC_API(mutex_lock)(alpo_sync_mutex *mutex)
{
	alpo_sync__spin(alpo_sync__atomic_flag_acquire(mutex));
	return ALPO_SYNC_API(success);
}

int ALPO_SYNC_API(mutex_trylock)(alpo_sync_mutex *mutex)
{
	if (alpo_sync__atomic_flag_acquire(mutex)) {
		return ALPO_SYNC_API(success);
	}
	return ALPO_SYNC_API(busy);
}

int ALPO_SYNC_API(mutex_unlock)(alpo_sync_mutex *mutex)
{
	alpo_sync__atomic_flag_release(mutex);
	return ALPO_SYNC_API(success);
}

#endif

/** @} */

// Spinlock Typedef

/**
    \defgroup spinlock Spinlocks

    Spinlocks allow for restricting access to a block of code to a single
  thread, similarly to an `alpo_sync_mutex`. Unlike mutexes, which may incur
  expensive system calls (depending on implementation), spinlocks are
  implemented with atomic operations and busy waiting. Because of this, they are
  unsuitable for guarding long blocks of code, as busy waiting wastes many CPU
  cycles, but they are better suited than mutexes for guarding very short blocks
  of code, where the time needed to make the necessary system calls may be as
  long or longer than the total time spent in the critical section.

    Compilation targets lacking OS support for mutexes may implement them as
  spinlocks internally, or vice-versa; targets lacking detectable atomics
  intrinsics, but possessing OS support for mutexes, may implement spinlocks as
  mutexes internally. Semantically, the two devices are identical in every
  aspect but performance, so this is acceptable from a compatibility standpoint.
  In general, choosing spinlocks when guarding very short sections and mutexes
  when guarding longer sections will maximize performance, regardless of
  platform.

    @{
**/

#if defined(DOXYGEN)

/**
    \brief An opaque handle to a spinlock object

    \note This is an opaque handle, and should be accessed only using API
  functions, as the underlying structure may change from platform to platform.
**/
typedef struct alpo_sync_spinlock alpo_sync_spinlock;

#elif defined(ALPO_SYNC_HAS_POSIX_SPINLOCKS)

typedef pthread_spinlock_t alpo_sync_spinlock;

#elif defined(ALPO_SYNC_HAS_ATOMICS)

typedef alpo_sync__atomic_flag alpo_sync_spinlock;

#else

typedef alpo_sync_mutex alpo_sync_spinlock;

#endif

typedef alpo_sync_spinlock ALPO_SYNC_API(spinlock);

// Spinlock Functions

/**
    \brief Initializes a spinlock object for use

    This function initializes a spinlock object to its unlocked state and makes
  it available for use. This function must be called exactly once before any
  other functions on the spinlock object; attempting to use an uninitialized
  spinlock will result in undefined behavior, as will initializing an already
  initialized spinlock.

    \param spinlock A pointer to the spinlock object
    \return One of `enum alpo_sync_status` indicating success or failure
**/
int ALPO_SYNC_API(spinlock_init)(alpo_sync_spinlock *spinlock);

/**
    \brief Destroys a spinlock object

    This function releases any resources allocated for a spinlock object and
  makes it unavailable for use. This function should be called exactly once
  after the spinlock object is no longer needed; failure to do so may result in
  a resource leak. Destroying an already destroyed spinlock object will result
  in undefined behavior.

    \param spinlock A pointer to the spinlock object
    \return One of `enum alpo_sync_status` indicating success or failure
**/
int ALPO_SYNC_API(spinlock_destroy)(alpo_sync_spinlock *spinlock);

/**
    \brief Locks a spinlock object

    This function allows a single thread to acquire sole ownership of a spinlock
  object. All threads contending for ownership will block until the spinlock
  becomes available. This function should be called only once before calling
  `alpo_sync_spinlock_unlock()`; locking an already locked or uninitialized
  spinlock results in undefined behavior.

    \param spinlock A pointer to the spinlock object
    \return One of `enum alpo_sync_status` indicating success or failure
**/
int ALPO_SYNC_API(spinlock_lock)(alpo_sync_spinlock *spinlock);

/**
    \brief Attempts to lock a spinlock object

    This function allows a single thread to acquire sole ownership of a spinlock
  object, but unlike `alpo_sync_spinlock_lock()`, the current thread will not
  block if the spinlock is in use. Check the return value to determine whether
  or not the spinlock was acquired. Calling this function on an uninitialized
  spinlock or one locked by the calling thread will result in undefined
  behavior.

    \param spinlock A pointer to the spinlock object
    \return `alpo_sync_success` on success, `alpo_sync_busy` if the spinlock is
  currently in use by another thread, or one of `enum alpo_sync_status` to
  indicate other reason for failure.
**/
int ALPO_SYNC_API(spinlock_trylock)(alpo_sync_spinlock *spinlock);

/**
    \brief Unlocks a spinlock object

    This function allows a thread to release a spinlock object back to other
  contending threads. This function should be called exactly once for every
  successful call to `alpo_sync_spinlock_lock()` or
  `alpo_sync_spinlock_trylock()`. Attempting to unlock an uninitialized spinlock
  or one not owned by the calling thread will result in undefined behavior.

    \param spinlock A pointer to the mutex object
    \return One of `enum alpo_sync_status` indicating success or failure
**/
int ALPO_SYNC_API(spinlock_unlock)(alpo_sync_spinlock *spinlock);

#if !defined(ALPO_SYNC_IMPLEMENT)
#elif defined(ALPO_SYNC_HAS_POSIX_SPINLOCKS)

int ALPO_SYNC_API(spinlock_init)(alpo_sync_spinlock *spinlock)
{
	return alpo_sync__posix_status(pthread_spin_init(spinlock, 0));
}

int ALPO_SYNC_API(spinlock_destroy)(alpo_sync_spinlock *spinlock)
{
	return alpo_sync__posix_status(pthread_spin_destroy(spinlock));
}

int ALPO_SYNC_API(spinlock_lock)(alpo_sync_spinlock *spinlock)
{
	return alpo_sync__posix_status(pthread_spin_lock(spinlock));
}

int ALPO_SYNC_API(spinlock_trylock)(alpo_sync_spinlock *spinlock)
{
	return alpo_sync__posix_status(pthread_spin_trylock(spinlock));
}

int ALPO_SYNC_API(spinlock_unlock)(alpo_sync_spinlock *spinlock)
{
	return alpo_sync__posix_status(pthread_spin_unlock(spinlock));
}

#elif defined(ALPO_SYNC_HAS_ATOMICS)

int ALPO_SYNC_API(spinlock_init)(alpo_sync_spinlock *spinlock)
{
	*spinlock = (alpo_sync_spinlock)ATOMIC_FLAG_INIT;
	return ALPO_SYNC_API(success);
}

int ALPO_SYNC_API(spinlock_destroy)(alpo_sync_spinlock *spinlock)
{
	return ALPO_SYNC_API(success);
}

int ALPO_SYNC_API(spinlock_lock)(alpo_sync_spinlock *spinlock)
{
	alpo_sync__spin(alpo_sync__atomic_flag_acquire(spinlock));
	return ALPO_SYNC_API(success);
}

int ALPO_SYNC_API(spinlock_trylock)(alpo_sync_spinlock *spinlock)
{
	if (alpo_sync__atomic_flag_acquire(spinlock)) {
		return ALPO_SYNC_API(busy);
	}
	return ALPO_SYNC_API(success);
}

int ALPO_SYNC_API(spinlock_unlock)(alpo_sync_spinlock *spinlock)
{
	alpo_sync__atomic_flag_release(spinlock);
	return ALPO_SYNC_API(success);
}

#else

int ALPO_SYNC_API(spinlock_init)(alpo_sync_spinlock *spinlock)
{
	return ALPO_SYNC_API(mutex_init)(spinlock);
}

int ALPO_SYNC_API(spinlock_destroy)(alpo_sync_spinlock *spinlock)
{
	return ALPO_SYNC_API(mutex_destroy)(spinlock);
}

int ALPO_SYNC_API(spinlock_lock)(alpo_sync_spinlock *spinlock)
{
	return ALPO_SYNC_API(mutex_lock)(spinlock);
}

int ALPO_SYNC_API(spinlock_trylock)(alpo_sync_spinlock *spinlock)
{
	return ALPO_SYNC_API(mutex_trylock)(spinlock);
}

int ALPO_SYNC_API(spinlock_unlock)(alpo_sync_spinlock *spinlock)
{
	return ALPO_SYNC_API(mutex_unlock)(spinlock);
}

#endif

/** @} */

// Condition Variable Typedef

/**
    \defgroup cond Condition Variables

    Condition variables allow threads to wait until a condition has been met
  before proceeding. Before waiting on a condition variable, the thread must
  have ownership of a mutex, which is passed as a parameter to
  `alpo_sync_cond_wait()`; this mutex is released while the thread is waiting,
  then reacquired before proceeding.

    @{
**/

#if defined(DOXYGEN)

/**
    \brief An opaque handle to a condition variable object

    \note This is an opaque handle, and should be accessed only using API
  functions, as the underlying structure may change from platform to platform.
**/
typedef struct alpo_sync_cond alpo_sync_cond;

#elif defined(ALPO_SYNC_HAS_POSIX_CONDS) && defined(ALPO_SYNC_USE_POSIX_MUTEXES)

typedef pthread_cond_t alpo_sync_cond;

#elif defined(ALPO_SYNC_HAS_WIN_CONDS)     \
	&& (defined(ALPO_SYNC_USE_SRW_MUTEXES) \
        || defined(ALPO_SYNC_USE_CS_MUTEXES))

typedef CONDITION_VARIABLE alpo_sync_cond;

#elif defined(ALPO_SYNC_HAS_C11_CONDS) && defined(ALPO_SYNC_USE_C11_MUTEXES)

typedef cnd_t alpo_sync_cond;

#else

typedef struct {
	alpo_sync_mutex mutex;
	uint16_t wait_id;
	uint16_t signal_id;
} alpo_sync_cond;

#endif

typedef alpo_sync_cond ALPO_SYNC_API(cond);

// Condition Variable Functions

/**
    \brief Initializes a condition variable object for use

    This function initializes a condition variable object and makes it available
  for use. This function must be called exactly once before any other functions
  on the condition variable object; attempting to use an uninitialized condition
  variable will result in undefined behavior, as will initializing an already
  initialized condition variable.

    \param cond A pointer to the condition variable object
    \return One of `enum alpo_sync_status` indicating success or failure
**/
int ALPO_SYNC_API(cond_init)(alpo_sync_cond *cond);

/**
    \brief Destroys a condition variable object

    This function releases any resources allocated for a condition variable
  object and makes it unavailable for use. This function should be called
  exactly once after the condition variable is no longer needed; failure to do
  so may result in a resource leak. Destroying an already destroyed condition
  variable will result in undefined behavior.

  \param cond A pointer to the condition variable object
  \return One of `enum alpo_sync_status` indicating success or failure
**/
int ALPO_SYNC_API(cond_destroy)(alpo_sync_cond *cond);

/**
    \brief Causes the calling thread to wait on the condition variable

    This function puts the calling thread to sleep until signaled via the
  condition varible. The mutex passed must already be locked by the calling
  thread; it will be unlocked for the duration of the waiting period, then
  relocked upon waking when signaled. Attempting to wait on an uninitialized
  condition variable or using a mutex which is not initialized and owned by the
  calling thread will result in undefined behavior.

    Spurious wake-ups are possible, depending on platform. Because of this,
  calls to this function should be wrapped in a conditional loop to allow the
  thread to reenter the wait call.
        while (condition != true) {
            alpo_sync_cond_wait(&cond, &mutex);
        }
    If waiting for a signal without a specific condition is desired, see
  `alpo_sync_event`.

    \param cond A pointer to the condition variable object
    \param mutex A pointer to a mutex object which must be already locked by the
  calling thread
    \return One of `enum alpo_sync_status` indicating success or failure
**/
int ALPO_SYNC_API(cond_wait)(alpo_sync_cond *cond, alpo_sync_mutex *mutex);

/**
    \brief Signals a single waiting thread to wake up

    This function wakes a single thread waiting on the condition variable, which
  will reacquire its mutex and proceed. To signal all threads currently waiting
  on the condition variable, use `alpo_sync_cond_broadcast()`. If no thread is
  currently waiting on the condition variable, `alpo_sync_success` is returned
  but no state will change. Attempting to signal an uninitialized condition
  variable results in undefined behavior.

    \param cond A pointer to the condition variable object
    \return One of `enum alpo_sync_status` indicating success or failure
**/
int ALPO_SYNC_API(cond_signal)(alpo_sync_cond *cond);

/**
    \brief Signals all waiting threads to wake up

    This function wakes all threads waiting on the condition variable, which
  will acquire their mutexes and proceed. To signal a single thread currently
  waiting on the condition variable, use `alpo_sync_cond_signal()`. If no thread
  is currently waiting on the condition variable, `alpo_sync_success` is
  returned but no state will change. Attempting to signal an uninitialized
  condition variable results in undefined behavior.

    If multiple waiting threads are using the same mutex, please note that the
  contention to reacquire the mutex will occur within the
  `alpo_sync_cond_wait()` function call. If that call is wrapped in a
  conditional loop, then it is possible for the condition to change before some
  threads have been allowed to proceed, in which case it will appear to be a
  spurious wake-up. As spurious wake-ups are potentially unavoidable, care must
  be taken to craft the conditional loop if it is desired that all threads
  observe the condition at the moment they were *signaled* rather than the
  moment they acquired the mutex.

    \param cond A pointer to the condition variable object
    \return One of `enum alpo_sync_status` indicating success or failure
**/
int ALPO_SYNC_API(cond_broadcast)(alpo_sync_cond *cond);

#if !defined(ALPO_SYNC_IMPLEMENT)
#elif defined(ALPO_SYNC_HAS_POSIX_CONDS) && defined(ALPO_SYNC_USE_POSIX_MUTEXES)

int ALPO_SYNC_API(cond_init)(alpo_sync_cond *cond)
{
	return alpo_sync__posix_status(pthread_cond_init(cond, NULL));
}

int ALPO_SYNC_API(cond_destroy)(alpo_sync_cond *cond)
{
	return alpo_sync__posix_status(pthread_cond_destroy(cond));
}

int ALPO_SYNC_API(cond_wait)(alpo_sync_cond *cond, alpo_sync_mutex *mutex)
{
	return alpo_sync__posix_status(pthread_cond_wait(cond, mutex));
}

int ALPO_SYNC_API(cond_signal)(alpo_sync_cond *cond)
{
	return alpo_sync__posix_status(pthread_cond_signal(cond));
}

int ALPO_SYNC_API(cond_broadcast)(alpo_sync_cond *cond)
{
	return alpo_sync__posix_status(pthread_cond_broadcast(cond));
}

#elif defined(ALPO_SYNC_HAS_WIN_CONDS)     \
	&& (defined(ALPO_SYNC_USE_SRW_MUTEXES) \
        || defined(ALPO_SYNC_USE_CS_MUTEXES))

int ALPO_SYNC_API(cond_init)(alpo_sync_cond *cond)
{
	InitializeConditionVariable(cond);
	return ALPO_SYNC_API(success);
}

int ALPO_SYNC_API(cond_destroy)(alpo_sync_cond *cond)
{
	return ALPO_SYNC_API(success);
}

#	if defined(ALPO_SYNC_USE_SRW_MUTEXES)
int ALPO_SYNC_API(cond_wait)(alpo_sync_cond *cond, alpo_sync_mutex *mutex)
{
	if (SleepConditionVariableSRW(cond, mutex, INFINITE, 0)) {
		return ALPO_SYNC_API(success);
	}
	return alpo_sync__win_status(GetLastError());
}

#	else

int ALPO_SYNC_API(cond_wait)(alpo_sync_cond *cond, alpo_sync_mutex *mutex)
{
	if (SleepConditionVariableCS(cond, mutex, INFINITE)) {
		return ALPO_SYNC_API(success);
	}
	return alpo_sync__win_status(GetLastError());
}
#	endif

int ALPO_SYNC_API(cond_signal)(alpo_sync_cond *cond)
{
	WakeConditionVariable(cond);
	return ALPO_SYNC_API(success);
}

int ALPO_SYNC_API(cond_broadcast)(alpo_sync_cond *cond)
{
	WakeAllConditionVariable(cond);
	return ALPO_SYNC_API(success);
}

#elif defined(ALPO_SYNC_HAS_C11_CONDS) && defined(ALPO_SYNC_USE_C11_MUTEXES)

int ALPO_SYNC_API(cond_init)(alpo_sync_cond *cond)
{
	return alpo_sync__c11_status(cnd_init(cond));
}

int ALPO_SYNC_API(cond_destroy)(alpo_sync_cond *cond)
{
	cnd_destroy(cond);
	return ALPO_SYNC_API(success);
}

int ALPO_SYNC_API(cond_wait)(alpo_sync_cond *cond, alpo_sync_mutex *mutex)
{
	return alpo_sync__c11_status(cnd_wait(cond, mutex));
}

int ALPO_SYNC_API(cond_signal)(alpo_sync_cond *cond)
{
	return alpo_sync__c11_status(cnd_signal(cond));
}

int ALPO_SYNC_API(cond_broadcast)(alpo_sync_cond *cond)
{
	return alpo_sync__c11_status(cnd_broadcast(cond));
}

#else

int ALPO_SYNC_API(cond_init)(alpo_sync_cond *cond)
{
	int retval = ALPO_SYNC_API(mutex_init)(&cond->mutex);
	if (retval != ALPO_SYNC_API(success)) {
		return retval;
	}

	cond->wait_id = 0;
	cond->signal_id = 0;

	return ALPO_SYNC_API(success);
}

int ALPO_SYNC_API(cond_destroy)(alpo_sync_cond *cond)
{
	return ALPO_SYNC_API(mutex_destroy)(&cond->mutex);
}

static inline int alpo_sync__cond_wait_loop(alpo_sync_cond *cond,
                                            alpo_sync_mutex *mutex,
                                            unsigned id)
{
	int retval = ALPO_SYNC_API(mutex_unlock)(&cond->mutex);
	if (retval != ALPO_SYNC_API(success)) {
		return retval;
	}

	retval = ALPO_SYNC_API(mutex_lock)(&cond->mutex);
	if (retval != ALPO_SYNC_API(success)) {
		return retval;
	}

	if (id - cond->signal_id > 0x8000) {
		retval = ALPO_SYNC_API(mutex_unlock)(&cond->mutex);
		if (retval != ALPO_SYNC_API(success)) {
			return retval;
		}
		return ALPO_SYNC_API(mutex_lock)(mutex);
	}
	return ALPO_SYNC_API(busy);
}

int ALPO_SYNC_API(cond_wait)(alpo_sync_cond *cond, alpo_sync_mutex *mutex)
{
	int retval = ALPO_SYNC_API(mutex_lock)(&cond->mutex);
	if (retval != ALPO_SYNC_API(success)) {
		return retval;
	}

	retval = ALPO_SYNC_API(mutex_unlock)(mutex);
	if (retval != ALPO_SYNC_API(success)) {
		ALPO_SYNC_API(mutex_unlock)(&cond->mutex);
		return retval;
	}

	unsigned id = cond->wait_id;
	cond->wait_id += 1;

	alpo_sync__spin((retval = alpo_sync__cond_wait_loop(cond, mutex, id))
	                != ALPO_SYNC_API(busy));
	return retval;
}

int ALPO_SYNC_API(cond_signal)(alpo_sync_cond *cond)
{
	int retval = ALPO_SYNC_API(mutex_lock)(&cond->mutex);
	if (retval != ALPO_SYNC_API(success)) {
		return retval;
	}

	if (cond->wait_id == cond->signal_id) {
		return ALPO_SYNC_API(mutex_unlock)(&cond->mutex);
	}
	cond->signal_id += 1;

	return ALPO_SYNC_API(mutex_unlock)(&cond->mutex);
}

int ALPO_SYNC_API(cond_broadcast)(alpo_sync_cond *cond)
{
	int retval = ALPO_SYNC_API(mutex_lock)(&cond->mutex);
	if (retval != ALPO_SYNC_API(success)) {
		return retval;
	}
	cond->signal_id = cond->wait_id;

	return ALPO_SYNC_API(mutex_unlock)(&cond->mutex);
}

#endif

/** @} */

// Reader/Writer Lock Typedef

/**
    \defgroup rwlock Reader-Writer Locks

    Reader-writer locks allow restricting access either to a single thread
  (referred to as "exclusive" mode) or to multiple non-exclusive threads
  (referred to as "shared" mode). When locked in exclusive mode, the
  reader-writer lock behaves similarly to a mutex; all threads attempting to
  acquire the lock, regardless of mode, will wait until the owning thread has
  released it. When locked in shared mode, only threads attempting to acquire
  the lock in exclusive mode will block; threads attempting to acquire the lock
  in shared mode may be allowed to proceed. Releasing the lock from shared mode
  only occurs once every one of these threads has called
  `alpo_sync_rwlock_read_unlock()`.

    These locks are primarily intended for single-writer/multiple-reader
  scenarios, where multiple threads in shared mode are allowed to read from the
  same memory, but only threads in exclusive mode are allowed to modify this
  memory. Allowing threads in shared mode to modify shared memory may result in
  race conditions if other steps are not taken to protect the shared memory.

    An attempt has been made to align reader/writer sharing behavior across
  platforms, but it would be impossible to guarantee so while using
  platform-specific implementations. As such, there may be a significant
  discrepancy in performance across platforms. When possible, rwlocks should be
  configured to prevent writer starvation, which can occur if reader threads are
  allowed to enter an open shared mode while writer threads are waiting. The way
  in which this is done differs by implementation, and the specific details of
  the resulting behavior are therefore outside of the scope of this library. The
  built-in implementation uses a writer-preferred but mode-switching algorithm.
  If a writer thread is waiting, no new reader threads may acquire a lock
  already in shared mode, which helps to prevent writer starvation. Upon
  unlocking, the rwlock will prefer to alternate modes, which prevents rwlocks
  in high contention from degenerating into single-thread mutexes.

    @{
**/

#if defined(DOXYGEN)

/**
    \brief An opaque handle to a reader-writer lock object

    \note This is an opaque handle, and should be accessed only using API
  functions, as the underlying structure may change from platform to platform.
**/
typedef struct alpo_sync_rwlock alpo_sync_rwlock;

#elif defined(ALPO_SYNC_HAS_POSIX_RWLOCKS)

typedef pthread_rwlock_t alpo_sync_rwlock;

#elif defined(ALPO_SYNC_HAS_WIN_RWLOCKS)

typedef SRWLOCK alpo_sync_rwlock;

#else

typedef struct {
	alpo_sync_mutex mutex;
	alpo_sync_cond read_cond;
	alpo_sync_cond write_cond;
	uint16_t read_count;
	unsigned write_waiting : 15;
	bool write_mode        : 1;
} alpo_sync_rwlock;

#endif

typedef alpo_sync_rwlock ALPO_SYNC_API(rwlock);

// Reader/Writer Lock Functions

/**
    \brief Initializes a reader-writer lock object for use

    This function initializes an rwlock object to its unlocked state and makes
  it available for use. This function must be called exactly once before any
  other functions on the rwlock object; attempting to use an uninitialized
  rwlock will result in undefined behavior, as will initializing an already
  initialized rwlock.

    \param rwlock A pointer to the rwlock object
    \return One of `enum alpo_sync_status` indicating success or failure
**/
int ALPO_SYNC_API(rwlock_init)(alpo_sync_rwlock *rwlock);

/**
    \brief Destroys a reader-writer lock object

    This function releases any resources allocated for an rwlock object and
  makes it unavailable for use. This function should be called exactly once
  after the rwlock object is no longer needed; failure to do so may result in a
  resource leak. Destroying an already destroyed rwlock object will result in
  undefined behavior.

    \param rwlock A pointer to the rwlock object
    \return One of `enum alpo_sync_status` indicating success or failure
**/
int ALPO_SYNC_API(rwlock_destroy)(alpo_sync_rwlock *rwlock);

/**
    \brief Acquires a reader-writer lock object in shared mode

    This function allows a calling thread to acquire the rwlock object in shared
  mode. If the lock is currently in exclusive mode, the calling thread will
  block until it becomes available; if the lock is currently in shared mode, the
  calling thread may be allowed to proceed, or may block if writer threads are
  waiting for access, depending on platform-specfic implementation. This
  function must be called only once before calling
  `alpo_sync_rwlock_read_unlock()`; attempting to acquire an rwlock twice within
  the same thread, even in shared mode, will result in undefined behavior.
  Locking an uninitialized mutex will also result in undefined behavior.

    \param rwlock A pointer to the rwlock object
    \return One of `enum alpo_sync_status` indicating success or failure
**/
int ALPO_SYNC_API(rwlock_read_lock)(alpo_sync_rwlock *rwlock);

/**
    \brief Attempts to acquire a reader-writer lock object in shared mode

    This function allows a calling thread to acquire the rwlock object in shared
  mode, but unlike `alpo_sync_rwlock_read_lock()`, the calling thread will not
  block if the rwlock is not immediately available. Check the return value to
  determine whether or not the rwlock was acquired. Calling this function on an
  uninitialized rwlock or one already locked by the calling thread will result
  in undefined behavior.

    \param rwlock A pointer to the rwlock object
    \return `alpo_sync_success` on success, `alpo_sync_busy` if the rwlock is
  currently in use by another thread, or one of `enum alpo_sync_status` to
  indicate other reason for failure.
**/
int ALPO_SYNC_API(rwlock_read_trylock)(alpo_sync_rwlock *rwlock);

/**
    \brief Releases a reader-writer lock object from shared mode

    This function allows a calling thread to release an rwlock object that it
  currently has shared access to. All reader threads with access to this object
  must release it with this function before it will actually become available to
  waiting threads. This function should be called once for every call to
  `alpo_sync_rwlock_read_lock()` or `alpo_sync_rwlock_read_trylock()`; failure
  to do so may result in threads blocking indefinitely. Attempting to unlock an
  uninitialized rwlock or one not previously acquired by the calling thread will
  result in undefined behavior.

    \param rwlock A pointer to the rwlock object
    \return One of `enum alpo_sync_status` indicating success or failure
**/
int ALPO_SYNC_API(rwlock_read_unlock)(alpo_sync_rwlock *rwlock);

/**
    \brief Acquires a reader-writer lock object in exclusive mode

    This function allows a calling thread to acquire the rwlock object in
  exclusive mode. If the lock is currently owned in either mode, the calling
  thread will block until it becomes available. This function must be called
  only once before calling `alpo_sync_rwlock_read_unlock()`; attempting to
  acquire an uninitialized rwlock or one already acquired by the calling thread
  will result in undefined behavior.

    \param rwlock A pointer to the rwlock object
    \return One of `enum alpo_sync_status` indicating success or failure
**/
int ALPO_SYNC_API(rwlock_write_lock)(alpo_sync_rwlock *rwlock);

/**
    \brief Attempts to acquire a reader-writer lock object in exclusive mode

    This function allows a calling thread to acquire the rwlock object in
  exclusive mode, but unlike `alpo_sync_rwlock_write_lock()`, the calling thread
  will not block if the rwlock is not immediately available. Check the return
  value to determine whether or not the rwlock was acquired. Calling this
  function on an uninitialized rwlock or one already locked by the calling
  thread will result in undefined behavior.

    \param rwlock A pointer to the rwlock object
    \return `alpo_sync_success` on success, `alpo_sync_busy` if the rwlock is
  currently in use by another thread, or one of `enum alpo_sync_status` to
  indicate other reason for failure.
**/
int ALPO_SYNC_API(rwlock_write_trylock)(alpo_sync_rwlock *rwlock);

/**
    \brief Releases a reader-writer lock object from exclusive mode

    This function allows a calling thread to release an rwlock object that it
  currently has exclusive access to. This function should be called once for
  every call to `alpo_sync_rwlock_write_lock()` or
  `alpo_sync_rwlock_write_trylock()`; failure to do so may result in threads
  blocking indefinitely. Attempting to unlock an uninitialized rwlock or one not
  previously acquired by the calling thread will result in undefined behavior.

    \param rwlock A pointer to the rwlock objcet
    \return One of `enum alpo_sync_status` indicating success or failure
**/
int ALPO_SYNC_API(rwlock_write_unlock)(alpo_sync_rwlock *rwlock);

#if !defined(ALPO_SYNC_IMPLEMENT)
#elif defined(ALPO_SYNC_HAS_POSIX_RWLOCKS)

int ALPO_SYNC_API(rwlock_init)(alpo_sync_rwlock *rwlock)
{
	pthread_rwlockattr_t attr;

	int retval = alpo_sync__posix_status(pthread_rwlockattr_init(&attr));
	if (retval != ALPO_SYNC_API(success)) {
		return retval;
	}

	retval = alpo_sync__posix_status(pthread_rwlockattr_setkind_np(
		&attr, PTHREAD_RWLOCK_PREFER_WRITER_NONRECURSIVE_NP));
	if (retval != ALPO_SYNC_API(success)) {
		return retval;
	}

	retval = alpo_sync__posix_status(pthread_rwlock_init(rwlock, &attr));
	if (retval != ALPO_SYNC_API(success)) {
		return retval;
	}

	return alpo_sync__posix_status(pthread_rwlockattr_destroy(&attr));
}

int ALPO_SYNC_API(rwlock_destroy)(alpo_sync_rwlock *rwlock)
{
	return alpo_sync__posix_status(pthread_rwlock_destroy(rwlock));
}

int ALPO_SYNC_API(rwlock_read_lock)(alpo_sync_rwlock *rwlock)
{
	return alpo_sync__posix_status(pthread_rwlock_rdlock(rwlock));
}

int ALPO_SYNC_API(rwlock_read_trylock)(alpo_sync_rwlock *rwlock)
{
	return alpo_sync__posix_status(pthread_rwlock_tryrdlock(rwlock));
}

int ALPO_SYNC_API(rwlock_read_unlock)(alpo_sync_rwlock *rwlock)
{
	return alpo_sync__posix_status(pthread_rwlock_unlock(rwlock));
}

int ALPO_SYNC_API(rwlock_write_lock)(alpo_sync_rwlock *rwlock)
{
	return alpo_sync__posix_status(pthread_rwlock_wrlock(rwlock));
}

int ALPO_SYNC_API(rwlock_write_trylock)(alpo_sync_rwlock *rwlock)
{
	return alpo_sync__posix_status(pthread_rwlock_trywrlock(rwlock));
}

int ALPO_SYNC_API(rwlock_write_unlock)(alpo_sync_rwlock *rwlock)
{
	return alpo_sync__posix_status(pthread_rwlock_unlock(rwlock));
}

#elif defined(ALPO_SYNC_HAS_WIN_RWLOCKS)

int ALPO_SYNC_API(rwlock_init)(alpo_sync_rwlock *rwlock)
{
	InitializeSRWLock(rwlock);
	return ALPO_SYNC_API(success);
}

int ALPO_SYNC_API(rwlock_destroy)(alpo_sync_rwlock *rwlock)
{
	return ALPO_SYNC_API(success);
}

int ALPO_SYNC_API(rwlock_read_lock)(alpo_sync_rwlock *rwlock)
{
	AcquireSRWLockShared(rwlock);
	return ALPO_SYNC_API(success);
}

int ALPO_SYNC_API(rwlock_read_trylock)(alpo_sync_rwlock *rwlock)
{
	if (TryAcquireSRWLockShared(rwlock)) {
		return ALPO_SYNC_API(success);
	}
	return ALPO_SYNC_API(busy);
}

int ALPO_SYNC_API(rwlock_read_unlock)(alpo_sync_rwlock *rwlock)
{
	ReleaseSRWLockShared(rwlock);
	return ALPO_SYNC_API(success);
}

int ALPO_SYNC_API(rwlock_write_lock)(alpo_sync_rwlock *rwlock)
{
	AcquireSRWLockExclusive(rwlock);
	return ALPO_SYNC_API(success);
}

int ALPO_SYNC_API(rwlock_write_trylock)(alpo_sync_rwlock *rwlock)
{
	if (TryAcquireSRWLockExclusive(rwlock)) {
		return ALPO_SYNC_API(success);
	}
	return ALPO_SYNC_API(busy);
}

int ALPO_SYNC_API(rwlock_write_unlock)(alpo_sync_rwlock *rwlock)
{
	ReleaseSRWLockExclusive(rwlock);
	return ALPO_SYNC_API(success);
}

#else

int ALPO_SYNC_API(rwlock_init)(alpo_sync_rwlock *rwlock)
{
	rwlock->write_waiting = 0;
	rwlock->write_mode = false;
	rwlock->read_count = 0;

	int retval = ALPO_SYNC_API(mutex_init)(&rwlock->mutex);
	if (retval != ALPO_SYNC_API(success)) {
		return retval;
	}

	retval = ALPO_SYNC_API(cond_init)(&rwlock->read_cond);
	if (retval != ALPO_SYNC_API(success)) {
		return retval;
	}

	return ALPO_SYNC_API(cond_init)(&rwlock->write_cond);
}

int ALPO_SYNC_API(rwlock_destroy)(alpo_sync_rwlock *rwlock)
{
	int retval = ALPO_SYNC_API(mutex_destroy)(&rwlock->mutex);
	if (retval != ALPO_SYNC_API(success)) {
		return retval;
	}

	retval = ALPO_SYNC_API(cond_destroy)(&rwlock->read_cond);
	if (retval != ALPO_SYNC_API(success)) {
		return retval;
	}

	return ALPO_SYNC_API(cond_destroy)(&rwlock->write_cond);
}

int ALPO_SYNC_API(rwlock_read_lock)(alpo_sync_rwlock *rwlock)
{
	int retval = ALPO_SYNC_API(mutex_lock)(&rwlock->mutex);
	if (retval != ALPO_SYNC_API(success)) {
		return retval;
	}

	if (rwlock->write_mode || rwlock->write_waiting > 0) {
		do {
			retval =
				ALPO_SYNC_API(cond_wait)(&rwlock->read_cond, &rwlock->mutex);
			if (retval != ALPO_SYNC_API(success)) {
				return retval;
			}
		} while (rwlock->write_mode);
	}
	rwlock->read_count += 1;

	return ALPO_SYNC_API(mutex_unlock)(&rwlock->mutex);
}

int ALPO_SYNC_API(rwlock_read_trylock)(alpo_sync_rwlock *rwlock)
{
	int retval = ALPO_SYNC_API(mutex_lock)(&rwlock->mutex);
	if (retval != ALPO_SYNC_API(success)) {
		return retval;
	}

	if (rwlock->write_mode || rwlock->write_waiting > 0) {
		retval = ALPO_SYNC_API(mutex_unlock)(&rwlock->mutex);
		if (retval != ALPO_SYNC_API(success)) {
			return retval;
		}
		return ALPO_SYNC_API(busy);
	}
	rwlock->read_count += 1;

	return ALPO_SYNC_API(mutex_unlock)(&rwlock->mutex);
}

int ALPO_SYNC_API(rwlock_read_unlock)(alpo_sync_rwlock *rwlock)
{
	int retval = ALPO_SYNC_API(mutex_lock)(&rwlock->mutex);
	if (retval) {
		return retval;
	}

	rwlock->read_count -= 1;

	if (rwlock->read_count == 0) {
		if (rwlock->write_waiting) {
			retval = ALPO_SYNC_API(cond_signal)(&rwlock->write_cond);
		} else {
			retval = ALPO_SYNC_API(cond_broadcast)(&rwlock->read_cond);
		}

		if (retval != ALPO_SYNC_API(success)) {
			ALPO_SYNC_API(mutex_unlock)(&rwlock->mutex);
			return retval;
		}
	}

	return ALPO_SYNC_API(mutex_unlock)(&rwlock->mutex);
}

int ALPO_SYNC_API(rwlock_write_lock)(alpo_sync_rwlock *rwlock)
{
	int retval = ALPO_SYNC_API(mutex_lock)(&rwlock->mutex);
	if (retval != ALPO_SYNC_API(success)) {
		return retval;
	}

	rwlock->write_waiting += 1;

	while (rwlock->write_mode || rwlock->read_count > 0) {
		retval = ALPO_SYNC_API(cond_wait)(&rwlock->write_cond, &rwlock->mutex);
		if (retval != ALPO_SYNC_API(success)) {
			ALPO_SYNC_API(mutex_unlock)(&rwlock->mutex);
			return retval;
		}
	}

	rwlock->write_waiting -= 1;
	rwlock->write_mode = true;

	return ALPO_SYNC_API(mutex_unlock)(&rwlock->mutex);
}

int ALPO_SYNC_API(rwlock_write_trylock)(alpo_sync_rwlock *rwlock)
{
	int retval = ALPO_SYNC_API(mutex_lock)(&rwlock->mutex);
	if (retval != ALPO_SYNC_API(success)) {
		return retval;
	}

	if (rwlock->write_mode || rwlock->read_count > 0) {
		retval = ALPO_SYNC_API(mutex_unlock)(&rwlock->mutex);
		if (retval != ALPO_SYNC_API(success)) {
			return retval;
		}
		return ALPO_SYNC_API(busy);
	}

	rwlock->write_mode = true;

	return ALPO_SYNC_API(mutex_unlock)(&rwlock->mutex);
}

int ALPO_SYNC_API(rwlock_write_unlock)(alpo_sync_rwlock *rwlock)
{
	int retval = ALPO_SYNC_API(mutex_lock)(&rwlock->mutex);
	if (retval != ALPO_SYNC_API(success)) {
		return retval;
	}

	rwlock->write_mode = false;

	retval = ALPO_SYNC_API(cond_broadcast)(&rwlock->read_cond);
	if (retval != ALPO_SYNC_API(success)) {
		ALPO_SYNC_API(mutex_unlock)(&rwlock->mutex);
		return retval;
	}

	retval = ALPO_SYNC_API(cond_signal)(&rwlock->write_cond);
	if (retval != ALPO_SYNC_API(success)) {
		ALPO_SYNC_API(mutex_unlock)(&rwlock->mutex);
		return retval;
	}

	return ALPO_SYNC_API(mutex_unlock)(&rwlock->mutex);
}

#endif

/** @} */

// Semaphore Typedef

/**
    \defgroup sem Semaphores

    Semaphores allow a set number of threads access to a resource. The number of
  available slots is set upon initialization; when a thread attempts to acquire
  a slot using `alpo_sync_sem_wait()`, the thread will wait until at least one
  slot is available, then acquire that slot and proceed. When the thread is done
  accessing the resource, it can release its slot for use by another thread
  using `alpo_sync_sem_post()`.

    @{
**/

#if defined(DOXYGEN)

/**
    \brief An opaque handle to a semaphore object

    \note This is an opaque handle, and should be accessed only using API
  functions, as the underlying structure may change from platform to platform.
**/
typedef struct alpo_sync_sem alpo_sync_sem;

#elif defined(ALPO_SYNC_HAS_POSIX_SEMAPHORES)

typedef sem_t alpo_sync_sem;

#elif defined(ALPO_SYNC_HAS_WIN_SEMAPHORES)

typedef HANDLE alpo_sync_sem;

#else

typedef struct {
	alpo_sync_mutex mutex;
	alpo_sync_cond cond;
	uint16_t count;
} alpo_sync_sem;

#endif

typedef alpo_sync_sem ALPO_SYNC_API(sem);

// Semaphore Functions

/**
    \brief Initializes a semaphore object with a specified number of slots

    This function initializes a semaphore object in its "full" state, with the
  number of slots given by `count`, and makes it available for use. The number
  of slots is unchangeable during use; re-initializing with a new count requires
  the semaphore to be released by all threads, destroyed using
  `alpo_sync_sem_destroy()` and then re-initialized with this function. This
  function should be called exactly once before using the semaphore object;
  attempting to use an uninitialized semaphore or initialize an already
  initialized semaphore results in undefined behavior.

    \param sem A pointer to the semaphore object
    \return One of `enum alpo_sync_status` indicating success or failure
**/
int ALPO_SYNC_API(sem_init)(alpo_sync_sem *sem, uint16_t count);

/**
    \brief Destroys a semaphore object

    This function releases all allocated resources used by a semaphore object,
  and leaves it in its uninitialized state. This function should be called
  exactly once on every semaphore once it is no longer needed, or before
  re-initializing with a new slot count. Attempting to destroy an uninitialized
  semaphore results in undefined behavior.

    \param sem A pointer to the semaphore object
    \return One of `enum alpo_sync_status` indicating success or failure
**/
int ALPO_SYNC_API(sem_destroy)(alpo_sync_sem *sem);

/**
    \brief Acquires an available semaphore slot

    This function allows a thread to acquire an available slot in a semaphore.
  If no slots are available, the thread will block until one is. Care should be
  taken before allowing threads to acquire multiple slots; although this is not
  undefined behavior, multiple threads contending for multiple slots may result
  in deadlock, where slots are split equally amongst threads and no single
  thread can acquire enough to proceed. All calls to this function should be
  followed by a matching call to `alpo_sync_sem_post()` to release the acquired
  slot; failure to do so may result in some threads blocking indefinitely.
  Attempting call this function on an uninitialized semaphore results in
  undefined behavior.

    \param sem A pointer to the semaphore object
    \return One of `enum alpo_sync_status` indicating success or failure
**/
int ALPO_SYNC_API(sem_wait)(alpo_sync_sem *sem);

/**
    \brief Releases a slot back to a semaphore object

    This function allows a thread to release its slot back to a semaphore,
  making it available for other threads to use. The calling thread must have
  previously acquired a slot using `alpo_sync_sem_post()`; calling this function
  on a semaphore object from which the thread does not own a slot, or upon an
  uninitialized semaphore, results in undefined behavior.

    \param sem A pointer to the semaphore object
    \return One of `enum alpo_sync_status` indicating success or failure
**/
int ALPO_SYNC_API(sem_post)(alpo_sync_sem *sem);

#if !defined(ALPO_SYNC_IMPLEMENT)
#elif defined(ALPO_SYNC_HAS_POSIX_SEMAPHORES)

int ALPO_SYNC_API(sem_init)(alpo_sync_sem *sem, uint16_t count)
{
	return alpo_sync__posix_status(sem_init(sem, 0, count));
}

int ALPO_SYNC_API(sem_destroy)(alpo_sync_sem *sem)
{
	return alpo_sync__posix_status(sem_destroy(sem));
}

int ALPO_SYNC_API(sem_wait)(alpo_sync_sem *sem)
{
	return alpo_sync__posix_status(sem_wait(sem));
}

int ALPO_SYNC_API(sem_post)(alpo_sync_sem *sem)
{
	return alpo_sync__posix_status(sem_post(sem));
}

#elif defined(ALPO_SYNC_HAS_WIN_SEMAPHORES)

int ALPO_SYNC_API(sem_init)(alpo_sync_sem *sem, uint16_t count)
{
	*sem = CreateSemaphoreA(NULL, count, UINT16_MAX, NULL);
	if (!*sem) {
		return alpo_sync__win_status(GetLastError());
	}
	return ALPO_SYNC_API(success);
}

int ALPO_SYNC_API(sem_destroy)(alpo_sync_sem *sem)
{
	if (!CloseHandle(sem)) {
		return alpo_sync__win_status(GetLastError());
	}
	return ALPO_SYNC_API(success);
}

int ALPO_SYNC_API(sem_wait)(alpo_sync_sem *sem)
{
	if (WaitForSingleObject(sem) == WAIT_FAILED) {
		return alpo_sync__win_status(GetLastError());
	}
	return ALPO_SYNC_API(success);
}

int ALPO_SYNC_API(sem_post)(alpo_sync_sem *sem)
{
	if (!ReleaseSemaphore(sem, 1, NULL)) {
		return alpo_sync__win_status(GetLastError());
	}
	return ALPO_SYNC_API(success);
}

#else

int ALPO_SYNC_API(sem_init)(alpo_sync_sem *sem, uint16_t count)
{
	int retval = ALPO_SYNC_API(mutex_init)(&sem->mutex);
	if (retval != ALPO_SYNC_API(success)) {
		return retval;
	}
	sem->count = count;

	return ALPO_SYNC_API(cond_init)(&sem->cond);
}

int ALPO_SYNC_API(sem_destroy)(alpo_sync_sem *sem)
{
	int retval = ALPO_SYNC_API(mutex_destroy)(&sem->mutex);
	if (retval != ALPO_SYNC_API(success)) {
		return retval;
	}
	return ALPO_SYNC_API(cond_destroy)(&sem->cond);
}

int ALPO_SYNC_API(sem_wait)(alpo_sync_sem *sem)
{
	int retval = ALPO_SYNC_API(mutex_lock)(&sem->mutex);
	if (retval != ALPO_SYNC_API(success)) {
		return retval;
	}

	while (sem->count == 0) {
		retval = ALPO_SYNC_API(cond_wait)(&sem->cond, &sem->mutex);
		if (retval != ALPO_SYNC_API(success)) {
			ALPO_SYNC_API(mutex_unlock);
			return retval;
		}
	}

	sem->count -= 1;

	return ALPO_SYNC_API(mutex_unlock)(&sem->mutex);
}

int ALPO_SYNC_API(sem_post)(alpo_sync_sem *sem)
{
	int retval = ALPO_SYNC_API(mutex_lock)(&sem->mutex);
	if (retval != ALPO_SYNC_API(success)) {
		return retval;
	}

	sem->count += 1;

	retval = ALPO_SYNC_API(cond_signal)(&sem->cond);
	if (retval != ALPO_SYNC_API(success)) {
		ALPO_SYNC_API(mutex_unlock)(&sem->mutex);
		return retval;
	}

	return ALPO_SYNC_API(mutex_unlock)(&sem->mutex);
}

#endif

// Barrier Typedef

/**
    \defgroup barrier Barriers

    Barriers allow for blocking a group of threads until all of them have
  reached a certain point before proceeding. The first thread released from the
  barrier may optionally be marked as the "leader", allowing that thread to
  perform some extra actions prior to the other threads proceeding; this may
  require an extra object, like an event, to block the non-leader threads until
  the leader has finished.

    Like semaphores, the number of threads required by the barrier is set and
  unchangeable upon initialization. Changing this value requires the barrier to
  be destroyed and re-initialized, which cannot occur while threads are waiting
on it.

    @{
**/

#if defined(DOXYGEN)

/**
    \brief An opaque handle to a barrier object

    \note This is an opaque handle, and should be accessed only using API
  functions, as the underlying structure may change from platform to platform.
**/
typedef struct alpo_sync_barrier alpo_sync_barrier;

#elif defined(ALPO_SYNC_HAS_POSIX_BARRIERS)

typedef pthread_barrier_t alpo_sync_barrier;

#elif defined(ALPO_SYNC_HAS_WIN_BARRIERS)

typedef SYNCHRONIZATION_BARRIER alpo_sync_barrier;

#else

typedef struct {
	alpo_sync_mutex mutex;
	alpo_sync_cond cond;
	uint16_t gen_size;
	uint16_t count;
	uint16_t gen         : 15;
	uint16_t exit_parity : 1;
} alpo_sync_barrier;

#endif

// Barrier Functions

/**
    \brief Initializes a barrier object

    This function initializes a barrier object, with the number of expected
  threads specified by `gen_size`. Each set of `gen_size` threads will wait
  until all threads have reached the barrier before being released. This value
  is unchangeable during use; changing it requires that no threads are waiting
  on the barrier before calling `alpo_sync_barrier_destroy()` and then this
  function to re-initialize to a new value. This function should be called
  exactly once before using a barrier; attempting to use an uninitialized
  barrier, or initialize an already-initialized barrier, results in undefined
  behavior.

    \param barrier A pointer to the barrier object
    \param gen_size The number of threads expected before a batch of threads is
  released
    \return One of `enum alpo_sync_status` indicating success or failure
**/
int ALPO_SYNC_API(barrier_init)(alpo_sync_barrier *barrier, uint16_t gen_size);

/**
    \brief Destroys a barrier object

    This function releases all allocated resources used by a barrier object and
  leaves it in its uninitialized state. This function should be called once a
  barrier object is no longer needed; failing to do so may result in resources
  becoming unrecoverable. There must be no threads currently waiting on a
  barrier when calling this function; attempting to destroy a barrier which is
  in use, or is uninitialized, results in undefined behavior.

    \param barrier A pointer to the barrier object
    \return One of `enum alpo_sync_status` indicating success or failure
**/
int ALPO_SYNC_API(barrier_destroy)(alpo_sync_barrier *barrier);

/**
    \brief Waits until all expected threads have reached the barrier before
  procedeeding

    This function causes the calling thread to block until the barrier is ready
  to release it, which occurs once the expected number of waiting threads (set
  by `alpo_sync_barrier_init()`) has been reached. Spurious wake-ups will not
  occur; all threads will block indefinitely until the expected number of
  threads have reached the barrier.

    The `is_leader` parameter allows for optionally marking the first thread to
  proceed through the barrier as the "leader". Note that this is not necessarily
  the first thread to *reach* the barrier, but the first to be awakened once the
  barrier is released. This ensures that the leader thread will be the first to
  contend for resources following the barrier, which allows the leader to block
  the other threads of its cohort and perform some special actions before
  allowing them to continue.

    The value of `is_leader` should be a pointer to an `int`, or `NULL` if
  marking the leader is unnecessary. Upon proceeding, the pointed-to `int` will
  either have a value of `1`, if the thread has been marked as the leader, or
  `0` if not.

    \param is_leader A pointer to an `int` variable which will receive `1` if
  the calling thread is the leader, or `0` if not. This parameter is optional;
  if unneeded, use the value `NULL`.
    \param barrier A pointer to the barrier object
    \return One of `enum alpo_sync_status` indicating success or failure
**/
int ALPO_SYNC_API(barrier_wait)(int *is_leader, alpo_sync_barrier *barrier);

#if !defined(ALPO_SYNC_IMPLEMENT)
#elif defined(ALPO_SYNC_HAS_POSIX_BARRIERS)

int ALPO_SYNC_API(barrier_init)(alpo_sync_barrier *barrier, uint16_t gen_size)
{
	return alpo_sync__posix_status(
		pthread_barrier_init(barrier, NULL, gen_size));
}

int ALPO_SYNC_API(barrier_destroy)(alpo_sync_barrier *barrier)
{
	return alpo_sync__posix_status(pthread_barrier_destroy(barrier));
}

int ALPO_SYNC_API(barrier_wait)(int *is_leader, alpo_sync_barrier *barrier)
{
	int retval = alpo_sync__posix_status(pthread_barrier_wait(barrier));
	switch (retval) {
	case 0:
		if (is_leader) {
			*is_leader = false;
		}
		return ALPO_SYNC_API(success);
	case PTHREAD_BARRIER_SERIAL_THREAD:
		if (is_leader) {
			*is_leader = true;
		}
		return ALPO_SYNC_API(success);
	default:
		return alpo_sync__posix_status(retval);
	}
}

#elif defined(ALPO_SYNC_HAS_WIN_BARRIERS)

int ALPO_SYNC_API(barrier_init)(alpo_sync_barrier *barrier, uint16_t gen_size)
{
	if (!InitializeSynchronizationBarrier(barrier, gen_size, -1)) {
		return alpo_sync__win_status(GetLastError());
	}
	return ALPO_SYNC_API(success);
}

int ALPO_SYNC_API(barrier_destroy)(alpo_sync_barrier *barrier)
{
	DeleteSynchronizationBarrier(barrier);
	return ALPO_SYNC_API(success);
}

int ALPO_SYNC_API(barrier_wait)(alpo_sync_barrier *barrier)
{
	bool retval = EnterSynchronizationBarrier(
		barrier, SYNCHRONIZATION_BARRIER_FLAGS_BLOCK_ONLY);
	if (is_leader) {
		*is_leader = retval;
	}
	return ALPO_SYNC_API(success);
}

#else

int ALPO_SYNC_API(barrier_init)(alpo_sync_barrier *barrier, uint16_t gen_size)
{
	int retval = ALPO_SYNC_API(mutex_init)(&barrier->mutex);
	if (retval != ALPO_SYNC_API(success)) {
		return retval;
	}

	barrier->gen_size = gen_size;
	barrier->count = 0;
	barrier->gen = 0;
	barrier->exit_parity = 0;

	return ALPO_SYNC_API(cond_init)(&barrier->cond);
}

int ALPO_SYNC_API(barrier_destroy)(alpo_sync_barrier *barrier)
{
	int retval = ALPO_SYNC_API(mutex_destroy)(&barrier->mutex);
	if (retval != ALPO_SYNC_API(success)) {
		return retval;
	}
	return ALPO_SYNC_API(cond_destroy)(&barrier->cond);
}

int ALPO_SYNC_API(barrier_wait)(int *is_leader, alpo_sync_barrier *barrier)
{
	int retval = ALPO_SYNC_API(mutex_lock)(&barrier->mutex);
	if (retval != ALPO_SYNC_API(success)) {
		return retval;
	}

	uint16_t gen = barrier->gen + 1;

	barrier->count += 1;
	if (barrier->count == barrier->gen_size) {
		barrier->count = 0;
		barrier->gen += 1;

		retval = ALPO_SYNC_API(cond_broadcast)(&barrier->cond);
		if (retval != ALPO_SYNC_API(success)) {
			ALPO_SYNC_API(mutex_unlock)(&barrier->mutex);
			return retval;
		}
	}

	while (gen != barrier->gen) {
		retval = ALPO_SYNC_API(cond_wait)(&barrier->cond, &barrier->mutex);
		if (retval != ALPO_SYNC_API(success)) {
			ALPO_SYNC_API(mutex_unlock)(&barrier->mutex);
			return retval;
		}
	}

	if (is_leader) {
		if ((gen & 1) == barrier->exit_parity) {
			*is_leader = false;
		} else {
			barrier->exit_parity = (gen & 1);
			*is_leader = true;
		}
	}

	return ALPO_SYNC_API(mutex_unlock)(&barrier->mutex);
}

#endif

/** @} */

// Event Object Typedef

/**
    \defgroup event Events

    Event objects allow one thread to control when one or more waiting threads
  is allowed to proceed, similarly to a condition variable but without requiring
  a specific condition attached to the signal.

    Event objects may be either "manual" or "automatic". Manual events, once
  signaled, allow every waiting thread to proceed until the event is reset by a
  controlling thread. Automatic events allow a single thread at a time to
  proceed, and must be signaled once for every thread waiting on the object.

    @{
**/

#if defined(DOXYGEN)

/**
    \brief An opaque handle to an event object

    \note This is an opaque handle, and should be accessed only using API
  functions, as the underlying structure may change from platform to platform.
**/
typedef struct alpo_sync_event alpo_sync_event;

#elif defined(ALPO_SYNC_HAS_WIN_EVENTS)

typedef HANDLE alpo_sync_event;

#else

typedef struct {
	alpo_sync_mutex mutex;
	alpo_sync_cond cond;
	bool manual;
	bool value;
} alpo_sync_event;

#endif

typedef alpo_sync_event ALPO_SYNC_API(event);

// Event Object Functions

/**
    \brief Initializes an event object

    This function initializes an event object and makes it available for use.
  The event object is specified as either manual or automatic; this cannot be
  changed without calling `alpo_sync_event_destroy()` and re-initializing with a
  different value. This function should be called exactly once before using an
  event object; attempting to use an uninitialized event object, or initialize
  an already initialized event object, results in undefined behavior.

    \param event A pointer to the event object
    \param manual `true` if the event object should be manually reset, and
  `false` if the event object should be reset each time a thread is signaled
    \return One of `enum alpo_sync_status` indicating success or failure
**/
int ALPO_SYNC_API(event_init)(alpo_sync_event *event, bool manual);

/**
    \brief Destroys an event object

    This function releases all resources used by an event object and leaves it
  in its uninitialized state. This function should be called exactly once when
  the event object is no longer needed; failing to do so may result in
  unrecoverable resources. This function should not be called while threads are
  waiting on the object; doing so, or destroying an uninitialized event object,
  results in undefined behavior.

    \param event A pointer to the event object
    \return One of `enum alpo_sync_status` indicating success or failure
**/
int ALPO_SYNC_API(event_destroy)(alpo_sync_event *event);

/**
    \brief Resets a manual event object

    This function resets an event object that has been initialized as manual to
  its untriggered state, causing threads which reach the event to wait until the
  event is signaled again. This is only necessary for manual event objects, as
  automatic event objects will reset each time a thread passes through them.
  Calling this function on an uninitialized event object results in undefined
  behavior.

    \param event A pointer to the event object
    \return One of `enum alpo_sync_status` indicating success or failure
**/
int ALPO_SYNC_API(event_reset)(alpo_sync_event *event);

/**
    \brief Waits until an event has been triggered

    This function causes the calling thread to wait until the event object has
  been signaled by a controlling thread. If the event object was initialized as
  manual, and the event has already been signaled but not reset, the calling
  thread will not block, and will proceed immediately. Calling this function on
  an uninitialized event object results in undefined behavior.

    \param event A pointer to the event object
    \return One of `enum alpo_sync_status` indicating success or failure
**/
int ALPO_SYNC_API(event_wait)(alpo_sync_event *event);

/**
    \brief Signals to waiting threads that the event has been triggered

    This function sends a signal to threads waiting on an event object to wake
  up and proceed. If the event object was initialized as manual, all waiting
  threads will proceed until `alpo_sync_event_reset()` is called; otherwise,
  only one thread will proceed, and any other awakened threads will resume
  waiting until the event is signaled again. Calling this function on a manual
  event object with no waiting threads will prevent any threads from blocking
  until it is reset; calling this function on an automatic event object with no
  waiting threads will allow the next thread which reaches it to pass, and all
  further threads will block until signaled again. Calling this function on an
  already signaled event object does nothing; the object will remain signaled.
  Calling this function on an uninitialized event object will result in
  undefined behavior.

    \param event A pointer to the event object
    \return One of `enum alpo_sync_status` indicating success or failure
**/
int ALPO_SYNC_API(event_signal)(alpo_sync_event *event);

#if !defined(ALPO_SYNC_IMPLEMENT)
#elif defined(ALPO_SYNC_HAS_WIN_EVENTS)

int ALPO_SYNC_API(event_init)(alpo_sync_event *event, bool manual)
{
	*event = CreateEventA(NULL, manual, false, NULL);
	if (!*event) {
		return alpo_sync__win_status(GetLastError());
	}
	return ALPO_SYNC_API(success);
}

int ALPO_SYNC_API(event_destroy)(alpo_sync_event *event)
{
	if (!CloseHandle(event)) {
		return alpo_sync__win_status(GetLastError());
	}
	return ALPO_SYNC_API(success);
}

int ALPO_SYNC_API(event_reset)(alpo_sync_event *event)
{
	if (!ResetEvent(event)) {
		return alpo_sync__win_status(GetLastError());
	}
	return ALPO_SYNC_API(success);
}

int ALPO_SYNC_API(event_wait)(alpo_sync_event *event)
{
	if (WaitForSingleObject(event, INFINITE) == WAIT_FAILED) {
		return alpo_sync__win_status(GetLastError());
	}
	return ALPO_SYNC_API(success);
}

int ALPO_SYNC_API(event_signal)(alpo_sync_event *event)
{
	if (!SetEvent(event)) {
		return alpo_sync__win_status(GetLastError());
	}
	return ALPO_SYNC_API(success);
}

#else

int ALPO_SYNC_API(event_init)(alpo_sync_event *event, bool manual)
{
	int retval = ALPO_SYNC_API(mutex_init)(&event->mutex);
	if (retval != ALPO_SYNC_API(success)) {
		return retval;
	}

	event->manual = manual;
	event->value = true;

	return ALPO_SYNC_API(cond_init)(&event->cond);
}

int ALPO_SYNC_API(event_destroy)(alpo_sync_event *event)
{
	int retval = ALPO_SYNC_API(mutex_destroy)(&event->mutex);
	if (retval != ALPO_SYNC_API(success)) {
		return retval;
	}
	return ALPO_SYNC_API(cond_destroy)(&event->cond);
}

int ALPO_SYNC_API(event_reset)(alpo_sync_event *event)
{
	int retval = ALPO_SYNC_API(mutex_lock)(&event->mutex);
	if (retval != ALPO_SYNC_API(success)) {
		return retval;
	}

	event->value = false;

	return ALPO_SYNC_API(mutex_unlock)(&event->mutex);
}

int ALPO_SYNC_API(event_wait)(alpo_sync_event *event)
{
	int retval = ALPO_SYNC_API(mutex_lock)(&event->mutex);
	if (retval != ALPO_SYNC_API(success)) {
		return retval;
	}

	while (!event->value) {
		retval = ALPO_SYNC_API(cond_wait)(&event->cond, &event->mutex);
		if (retval != ALPO_SYNC_API(success)) {
			ALPO_SYNC_API(mutex_unlock)(&event->mutex);
			return retval;
		}
	}

	if (!event->manual) {
		event->value = false;
	}

	return ALPO_SYNC_API(mutex_unlock)(&event->mutex);
}

int ALPO_SYNC_API(event_signal)(alpo_sync_event *event)
{
	int retval = ALPO_SYNC_API(mutex_lock)(&event->mutex);
	if (retval != ALPO_SYNC_API(success)) {
		return retval;
	}

	if (event->value) {
		return ALPO_SYNC_API(mutex_unlock)(&event->mutex);
	}

	event->value = true;

	if (event->manual) {
		retval = ALPO_SYNC_API(cond_broadcast)(&event->cond);
		if (retval != ALPO_SYNC_API(success)) {
			ALPO_SYNC_API(mutex_unlock)(&event->mutex);
			return retval;
		}
	} else {
		retval = ALPO_SYNC_API(cond_signal)(&event->cond);
		if (retval != ALPO_SYNC_API(success)) {
			ALPO_SYNC_API(mutex_unlock)(&event->mutex);
			return retval;
		}
	}

	return ALPO_SYNC_API(mutex_unlock)(&event->mutex);
}

#endif

/** @} */

// Do-Once Typedef

/**
    \defgroup once Do Once

    This object allows a function to be called exactly once, regardless of the
  number of threads sharing the same code. This is useful for global
  initialization and exit logic without needing separate paths for concurrent
  threads.

    Unlike other objects in this library, do-once objects are initialized
  statically, which allows them to be used immediately at the start of global
  scope without calling a separate init function. This requires that, before
  using a do-once object, it must be initialized like so:

        alpo_sync_once once = ONCE_INIT;

    This must be present before the object can be used, but can be done at
  either global or local scope.

    @{
**/

#if defined(DOXYGEN)

/**
    \brief An opaque handle to an event object

    \note This is an opaque handle, and should be accessed only using API
  functions, as the underlying structure may change from platform to platform.
**/
typedef struct alpo_sync_once alpo_sync_once;

/**
    \def ONCE_INIT
    \brief Static initializer for do-once objects

    This is the value to which all do-once objects must be initialized before
  use. Failure to do so will result in undefined behavior.
**/
#	define ONCE_INIT (alpo_sync_once) false;

#elif defined(ALPO_SYNC_HAS_POSIX_ONCE)

typedef pthread_once_t alpo_sync_once;
#	define ONCE_INIT PTHREAD_ONCE_INIT

#elif defined(ALPO_SYNC_HAS_C11_ONCE)

typedef once_flag alpo_sync_once;
#	define ONCE_INIT ONCE_FLAG_INIT

#elif defined(ALPO_SYNC_HAS_WIN_ONCE)

typedef struct {
	SRWLOCK lock;
	bool flag;
} alpo_sync_once;
#	define ONCE_INIT                      \
		(alpo_sync_once)                   \
		{                                  \
			.lock = SRWLOCK, .flag = false \
		}

#else

typedef alpo_sync__atomic_flag alpo_sync_once;
#	define ONCE_INIT ATOMIC_FLAG_INIT

#endif

typedef alpo_sync_once ALPO_SYNC_API(once);

// Do-Once Functions

/**
    \brief Calls the provided function exactly once

    This function triggers the callback function `void exec(void)` exactly once,
  regardless of the number of threads which share this function call. Prior to
  calling this function, the do-once object pointed to by `once` must be
  initialized to the value `ONCE_INIT`; failure to do so will result in
  undefined behavior.

    \param once A pointer to the do-once object
    \param exec The function to call exactly once
    \return One of `enum alpo_sync_status` indicating success or failure
**/
int ALPO_SYNC_API(do_once)(alpo_sync_once *once, void (*exec)(void));

#if defined(ALPO_SYNC_HAS_POSIX_ONCE)

int ALPO_SYNC_API(do_once)(alpo_sync_once *once, void (*exec)(void))
{
	return alpo_sync__posix_status(pthread_once(once, exec));
}

#elif defined(ALPO_SYNC_HAS_C11_ONCE)

int ALPO_SYNC_API(do_once)(alpo_sync_once *once, void (*exec)(void))
{
	call_once(once, exec);
	return ALPO_SYNC_API(success);
}

#elif defined(ALPO_SYNC_HAS_WIN_ONCE)

int ALPO_SYNC_API(do_once)(alpo_sync_once *once, void (*exec)(void))
{
	AcquireSRWLockExclusive(&once->lock);

	if (!once->flag) {
		once->flag = true;
		exec();
	}

	ReleaseSRWLockExclusive(&once->lock);

	return ALPO_SYNC_API(success);
}

#else

int ALPO_SYNC_API(do_once)(alpo_sync_once *once, void (*exec)(void))
{
	if (alpo_sync__atomic_flag_acquire(once)) {
		return ALPO_SYNC_API(success);
	}

	exec();
	alpo_sync__atomic_flag_fence(once);

	return ALPO_SYNC_API(success);
}

#endif

/** @} */

// Cleanup

#undef ALPO_SYNC_CONCAT_
#undef ALPO_SYNC_CONCAT
#undef ALPO_SYNC_API
#undef ALPO_SYNC_HAS_POSIX
#undef ALPO_SYNC_HAS_POSIX_MUTEXES
#undef ALPO_SYNC_HAS_POSIX_CONDS
#undef ALPO_SYNC_HAS_POSIX_BARRIERS
#undef ALPO_SYNC_HAS_POSIX_RWLOCKS
#undef ALPO_SYNC_HAS_POSIX_SEMAPHORES
#undef ALPO_SYNC_HAS_POSIX_SPINLOCKS
#undef ALPO_SYNC_HAS_POSIX_ONCE
#undef ALPO_SYNC_HAS_WIN
#undef ALPO_SYNC_HAS_WIN_BARRIERS
#undef ALPO_SYNC_HAS_WIN_RWLOCKS
#undef ALPO_SYNC_HAS_WIN_CONDS
#undef ALPO_SYNC_HAS_WIN_ONCE
#undef ALPO_SYNC_HAS_WIN_CRITICALSECTIONS
#undef ALPO_SYNC_HAS_WIN_SEMAPHORES
#undef ALPO_SYNC_HAS_WIN_EVENTS
#undef ALPO_SYNC_HAS_C11
#undef ALPO_SYNC_HAS_C11_MUTEXES
#undef ALPO_SYNC_HAS_C11_CONDS
#undef ALPO_SYNC_HAS_C11_ONCE
#undef ALPO_SYNC_HAS_ATOMICS
#undef ALPO_SYNC_HAS_C11_ATOMICS
#undef ALPO_SYNC_HAS_GNUC_ATOMICS
#undef ALPO_SYNC_HAS_WIN8_ATOMICS
#undef ALPO_SYNC_HAS_VISTA_ATOMICS
#undef ALPO_SYNC_HAS_SYNC_ATOMICS
#undef ALPO_SYNC_HAS_WINXP_ATOMICS
#undef ALPO_SYNC_USE_POSIX_MUTEXES
#undef ALPO_SYNC_USE_SRW_MUTEXES
#undef ALPO_SYNC_USE_CS_MUTEXES
#undef ALPO_SYNC_USE_C11_MUTEXES
#undef alpo_sync__atomic_flag
#undef alpo_sync__atomic_flag_acquire
#undef alpo_sync__atomic_flag_release
#undef alpo_sync__atomic_flag_fence
#undef alpo_sync__pause
#undef alpo_sync__spin

/** @} */

#endif
