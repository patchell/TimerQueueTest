#include <windows.h>
#include <stdio.h>

constexpr auto WAIT_MY_HANDLE_ERROR = -2;

class CMyEvent
{
	HANDLE m_hHandle;
public:
	CMyEvent()
	{
		m_hHandle = 0;
	}

	virtual ~CMyEvent()
	{
		::CloseHandle(m_hHandle);
	}

	virtual BOOL Create(WCHAR* name)
	{
		BOOL rV = FALSE;
		m_hHandle = ::CreateEventW(NULL, FALSE, FALSE, name);
		if (m_hHandle) rV = TRUE;
		return rV;
	}

	bool Post()
	{
		//----------------------------------
		//	Post
		//		This function is used to
		//	the object to the signaled state.
		//
		// return value
		//	TRUE if succesfull
		//	FALSE on fail
		//--------------------------------------

		if (m_hHandle) {
			if (::SetEvent(m_hHandle))
				return TRUE;
		}
		return FALSE;
	}
	int Pend(DWORD timeout = INFINITE)
	{
		//-----------------------------------
		// Pend
		//	This function will wait for an
		//	Event to be signaled.
		// Parameter:
		//	timeout---Time, in milliseconds
		//	that the method will wait for
		//	the object to be signalled.
		//	The default is Infinate
		//
		//	return value:
		//		WAIT_OBJECT_0....Success
		//		WAIT_TIMEOUT.....Timeout
		//		WAIT_FALED.......ERROR
		//		WAIT_MY_HANDLE...ERROR, band handle
		//------------------------------------

		int rV = WAIT_MY_HANDLE_ERROR;	//object not created error
		if (m_hHandle)
			rV = ::WaitForSingleObject(m_hHandle, timeout);
		return rV;
	}
};

class CTimerQueue
{
	HANDLE m_hTimerQueue;
public:
	CTimerQueue() {
		m_hTimerQueue = 0;
	}
	virtual ~CTimerQueue() {
		::DeleteTimerQueueEx(m_hTimerQueue, NULL);
	}
	BOOL Create() {
		BOOL rV = TRUE;
		m_hTimerQueue = CreateTimerQueue();
		if (!m_hTimerQueue) rV = FALSE;
		return rV;
	}
	BOOL CreateTimerQueueTimer(
		HANDLE* phTimer,	//pointer to timer Handle
		WAITORTIMERCALLBACK CallbackFunction, 
		VOID* pvParam,	//user data
		DWORD DueTime,
		DWORD Period,
		ULONG Flags
	) {
		return ::CreateTimerQueueTimer(
			phTimer,
			this->m_hTimerQueue,
			CallbackFunction,
			pvParam,
			DueTime,
			Period,
			Flags
		);
	}
	BOOL DeleteTimeQueueTimer(HANDLE hTimer, HANDLE hCompletionEvent)
	{
		return ::DeleteTimerQueueTimer(
			this->m_hTimerQueue,
			hTimer,
			hCompletionEvent
		);
	}
	BOOL ChangeTimerQueueTimer(
		HANDLE hTimer, 
		DWORD DueTime,
		DWORD Period
	) {
		return ::ChangeTimerQueueTimer(
			this->m_hTimerQueue,
			hTimer,
			DueTime,
			Period
		);
	}
	HANDLE GetQueueHandle() {
		return m_hTimerQueue;;
	}
};

DWORD Count;
CMyEvent CountDone;

void CALLBACK TimerCallback(PVOID lpParam, BOOLEAN TimerOrWaitFired)
{
	++Count;
	if (Count == 100)
		CountDone.Post();
}

int main()
{
	CTimerQueue TimerQueue;
	HANDLE TheTimer;

	CountDone.Create(NULL);

	TimerQueue.Create();
	TimerQueue.CreateTimerQueueTimer(
		&TheTimer,
		TimerCallback,
		NULL,
		1,
		1,
		0
	);
	TimerQueue.ChangeTimerQueueTimer(
		TheTimer,
		1,
		1
	);
	printf("Start Count\n");
	Count = 0;
	CountDone.Pend();
	printf("Done\n");
	return 0;
}
