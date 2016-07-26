//============================================================================
// Name        : Airtame.cpp
// Author      : Navid Anari
// Version     :
// Copyright   : Your copyright notice
// Description : Airtame technical challenge has been writen in c++, performance and experimental values like
//               waiting time or frame size or even  buffer size has not been tested In my impression obtain
//				 the optimized valuse needs a comprehensive testing process
//
//				 code is implemented as simple as possible there are just few methods with clear decleration.
//				 because of dependencies to some predefined functions and DS like frame DS or functions to deal
//				 with frame, some parts are written as sudo code.
//============================================================================

#include <iostream>
#include <pthread.h>
#include <unistd.h>
#include <mutex>
#include <condition_variable>
#include <queue>

using namespace std;

struct Frame
{
	size_t size;
	void * data;
}myFrame;

std::queue<Frame> myQ;

pthread_cond_t cond;
pthread_mutex_t mx;

bool produce_frame (struct Frame)
{return true;}
void consume_frame (struct Frame)
{return;}


int threadsafe_SendFrame(struct Frame);
void threadSafe_SyncSpeed(int);
struct Frame threadsafe_ReceiveFrame();

int threadsafe_SendFrame(struct Frame myFrame)
{
	size_t qSize;
	pthread_mutex_lock(&mx);
	//myFrame.id = i;
	myQ.push(myFrame);
	qSize = myQ.size();
	pthread_cond_signal(&cond);
	pthread_mutex_unlock(&mx);

	return qSize;
}
void threadSafe_SyncSpeed(int qSize)
{
	if(qSize > 300)
			usleep(5000);
}
struct Frame threadsafe_ReceiveFrame()
{
	struct Frame myFrame;

	pthread_mutex_lock(&mx);

	while(myQ.empty())
		pthread_cond_wait(&cond, &mx);
	consume_frame(myQ.front());
	myQ.pop();

	pthread_mutex_unlock(&mx);

	return myFrame;
}

void * writer(void *)
{
	size_t qSize;
	Frame myFrame;
	for(int i = 0; i<5000000; i++)
	{
		if(produce_frame(myFrame))
			qSize = threadsafe_SendFrame(myFrame);

		threadSafe_SyncSpeed(qSize);
	}
	pthread_exit(0);
}

void * reader(void *)
{
	for(int i = 0; true; i++)
	{
		threadsafe_ReceiveFrame();
	}
	pthread_exit(0);
}

int main() {

	pthread_t th2,th1;

	pthread_create(&th2, NULL, writer, NULL);
	pthread_create(&th1, NULL, reader, NULL);

	pthread_join(th2, NULL);
	pthread_join(th1, NULL);

	return 0;
}

