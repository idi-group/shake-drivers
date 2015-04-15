/*	Copyright (c) 2006-2009, University of Glasgow
*	All rights reserved.
*
*	Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:
*
*		* Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
*		* Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer
*			in the documentation and/or other materials provided with the distribution.
*		* Neither the name of the University of Glasgow nor the names of its contributors may be used to endorse or promote products derived 
*			from this software without specific prior written permission.
*
*	THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, 
*	THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS 
*	BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE 
*	GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT 
*	LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/


#include "shake_platform.h"
#include "shake_thread.h"

#ifndef _WIN32
/*  utility func to avoid rewriting this pthread_cond_wait handling code.
 *  Waits on dev->cmd_event, returns TRUE if returning
 *  due to signal, FALSE otherwise */
BOOL pthread_wait(shake_thread* dev, int secs, int ms, int thread) {
	struct timeval now;
	struct timespec timeout;
	int done, retval;
	BOOL signalled = FALSE;

	/* lock the mutex associated with the condition variable */
	if(thread == CMD_THREAD)
		pthread_mutex_lock(&(dev->cmd_mutex));
	else
		pthread_mutex_lock(&(dev->callback_mutex));

	gettimeofday(&now, NULL);

	/* have to set absolute time for the timeout */

	/* just add seconds to current time */
	timeout.tv_sec = now.tv_sec + secs;

	/* timespec structure uses nanoseconds, timeval uses microseconds, and
	 * we get milliseconds as a parameter. This statement just converts the milliseconds into
	 * microseconds, adds them to the current microsecond offset, and multiplies the result
	 * by 1000 to get it into nanoseconds for the timespec structure */
	timeout.tv_nsec = (now.tv_usec + (1000 * ms)) * 1000;

	/* it's possible that the wait might be ended prematurely by a
	 * Unix signal, so have to run a loop like this... */
	done = FALSE;
	while(!done) {
		if(thread == CMD_THREAD)
			retval = pthread_cond_timedwait(&(dev->cmd_event), &(dev->cmd_mutex), &timeout);
		else
			retval = pthread_cond_timedwait(&(dev->callback_event), &(dev->callback_mutex), &timeout);
		switch(retval) {
			case 0:
				/* in this case the func returned due to the cond var being signalled */
				done = TRUE;
				signalled = TRUE;
				break;
			default: 
				/* some signal was responsible for returning from func */
				if(retval == ETIMEDOUT || retval == EINVAL) {
					done = TRUE;
				}
				break;
		}
	}
	if(thread == CMD_THREAD)
		pthread_mutex_unlock(&(dev->cmd_mutex));
	else
		pthread_mutex_unlock(&(dev->callback_mutex));
	return signalled;
}
#endif /* _WIN32 */

shake_thread* shake_thread_init(shake_thread* st, SHAKE_THREAD_FUNC cmdfunc, void* cmdparam, TCHAR* cmdeventname, 
													SHAKE_THREAD_FUNC cbfunc, void* cbparam, TCHAR* cbeventname,
													SHAKE_THREAD_FUNC audiofunc, void* audioparam, TCHAR* audioeventname) {
	#ifdef _WIN32
	st->cmd_event = CreateEvent(NULL, FALSE, FALSE, cmdeventname); 
	st->rthread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)cmdfunc, cmdparam, 0, NULL);
	SetThreadPriority(st->rthread, THREAD_PRIORITY_ABOVE_NORMAL);
	st->callback_event = CreateEvent(NULL, FALSE, FALSE, cbeventname);
	st->cthread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)cbfunc, cbparam, 0, NULL);
	if(audiofunc) {
		st->audio_event = CreateEvent(NULL, FALSE, FALSE, audioeventname);
		st->audiothread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)audiofunc, audioparam, 0, NULL);
		SetThreadPriority(st->audiothread, THREAD_PRIORITY_ABOVE_NORMAL);
	}
	#else
	pthread_cond_init(&(st->cmd_event), NULL);
	pthread_mutex_init(&(st->cmd_mutex), NULL);
	pthread_create(&(st->rthread), NULL, cmdfunc, cmdparam);
	pthread_cond_init(&(st->callback_event), NULL);
	pthread_mutex_init(&(st->callback_mutex), NULL);
	pthread_create(&(st->cthread), NULL, cbfunc, cbparam);
	#endif
	return st;
}

BOOL shake_thread_wait(shake_thread* st, int secs, int ms, int thread) {
	#ifdef _WIN32
	int ret;
	if(thread == CMD_THREAD)
		ret = WaitForSingleObject(st->cmd_event, (secs * 1000) + ms);
	else
		ret = WaitForSingleObject(st->callback_event, (secs * 1000) + ms);
	
	if(ret == WAIT_OBJECT_0)
		return TRUE;
	return FALSE;
	#else
	return pthread_wait(st, secs, ms, thread);
	#endif
}

BOOL shake_thread_signal(shake_thread* st, int thread) {
	#ifdef _WIN32
	if(thread == CMD_THREAD)
		SetEvent(st->cmd_event);
	else
		SetEvent(st->callback_event);
	#else
	if(thread == CMD_THREAD)
		pthread_cond_signal(&(st->cmd_event));
	else
		pthread_cond_signal(&(st->callback_event));
	#endif
	return TRUE;
}

BOOL shake_thread_free(shake_thread* st) {
	#ifdef _WIN32
	CloseHandle(st->cmd_event);
	#else
	#endif
	return TRUE;
}

void shake_thread_exit(int value) {
	#ifdef _WIN32
	ExitThread(value);
	#else
	pthread_exit((void*)value);
	#endif
}
