class KifWaitMutex { 
	int guard; /* pthread_cond_signal() does not record signals. If no 
			   threads are waiting on a cond variable and the cond is 
			   signaled, nothing happens. So to avoid race conditions, 
			   we need this guard variable to record the fact that the 
			   thread should not try to delay. */ 
	pthread_mutex_t mutex; 
	pthread_cond_t cond; 
	int timetowait;

	/* Wait for the specified number of seconds. If we time 
	out, return -1, but if we are woken early, return 0. */ 

	int waiting() {
		struct timeval now; 
		struct timespec wait; 
		int rc;
		pthread_mutex_lock(&mutex); 
		if (guard) { /* Should we bother delaying? */ 
			rc = 0; /* tell calling thread we woke early. */ 
		} 
		else { 
			guard = 1; /* Mark that we are delaying. */ 
			gettimeofday(&now, NULL);
			memset(&wait, 0, sizeof(wait)); /* ensure it's initialized */ 
			wait.tv_sec = now.tv_sec + timetowait; 

			if (ETIMEDOUT == pthread_cond_timedwait(&cond,mutex,&wait))
				rc = -1; /* timeout */ 
			else
				rc = 0; /* someone woke us up */ 
		} 
		guard = 0; /* Out of delay. */ 
		pthread_mutex_unlock(&mutex); 
		return rc; 
	}

	KifWaitMutex(int seconds) { 
		guard=0;
		timetowait=seconds;
	}

	/* Wake up a thread that's sleeping. */ 
	int wake() 	{ 
		pthread_mutex_lock(&mutex); 
		if (guard) { 
			pthread_cond_signal(&cond); /* Wake thread from sleep. */ 
		} 
		else { 
			guard = 1; /* Thread not sleeping now, tell it not to try. */ 
		} 
		pthread_mutex_unlock(&mutex); 
	}
};

