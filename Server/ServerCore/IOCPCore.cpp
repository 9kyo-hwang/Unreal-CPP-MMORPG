#include "pch.h"
#include "IOCPCore.h"
#include "IOCPEvent.h"

FSocketEventQueue::FSocketEventQueue()
	: Data(::CreateIoCompletionPort(INVALID_HANDLE_VALUE, nullptr, 0, 0))
{
	check(Data != INVALID_HANDLE_VALUE);
}

FSocketEventQueue::~FSocketEventQueue()
{
	::CloseHandle(Data);
}

// ϰ ϴ ( == Data) CP 
bool FSocketEventQueue::Enqueue(shared_ptr<ISocketEventable> Socket)
{
	return ::CreateIoCompletionPort(
		Socket->GetHandle(), 
		Data, 
		/*Key*/0,
		0
	);
}

// Worker threads pop event
bool FSocketEventQueue::Dequeue(uint32 TimeoutMilliseconds)
{
	DWORD NumberOfBytesTransferred = 0;
	ULONG_PTR CompletionKey = 0;
	FSocketEvent* Event = nullptr;

	if (::GetQueuedCompletionStatus(Data, &NumberOfBytesTransferred, &CompletionKey,
		reinterpret_cast<LPOVERLAPPED*>(&Event),
		TimeoutMilliseconds))
	{
		auto Socket = Event->Owner;
		Socket->Dispatch(Event, NumberOfBytesTransferred);
	}
	else
	{
		if (::WSAGetLastError() == WAIT_TIMEOUT)
		{
			return false;
		}
		else
		{
			if (auto Socket = Event->Owner)
			{
				// TODO: α 
				Socket->Dispatch(Event, NumberOfBytesTransferred);
			}
		}
	}

	return true;
}