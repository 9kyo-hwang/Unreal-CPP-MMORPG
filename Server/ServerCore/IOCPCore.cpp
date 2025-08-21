#include "pch.h"
#include "IocpCore.h"
#include "IocpEvent.h"

/*--------------
	FSocketIOEventQueue
---------------*/

FSocketIOEventQueue::FSocketIOEventQueue()
{
	Handle = ::CreateIoCompletionPort(INVALID_HANDLE_VALUE, 0, 0, 0);
	check(Handle != INVALID_HANDLE_VALUE);
}

FSocketIOEventQueue::~FSocketIOEventQueue()
{
	::CloseHandle(Handle);
}

bool FSocketIOEventQueue::Register(ISocketIOEventableRef iocpObject)
{
	return ::CreateIoCompletionPort(iocpObject->GetHandle(), Handle, /*key*/0, 0);
}

bool FSocketIOEventQueue::Dispatch(uint32 timeoutMs)
{
	DWORD numOfBytes = 0;
	ULONG_PTR key = 0;	
	FSocketIOEvent* iocpEvent = nullptr;

	if (::GetQueuedCompletionStatus(Handle, OUT &numOfBytes, OUT &key, OUT reinterpret_cast<LPOVERLAPPED*>(&iocpEvent), timeoutMs))
	{
		ISocketIOEventableRef iocpObject = iocpEvent->Owner;
		iocpObject->Dispatch(iocpEvent, numOfBytes);
	}
	else
	{
		int32 errCode = ::WSAGetLastError();
		switch (errCode)
		{
		case WAIT_TIMEOUT:
			return false;
		default:
			// TODO : ·Î±× Âï±â
			ISocketIOEventableRef iocpObject = iocpEvent->Owner;
			iocpObject->Dispatch(iocpEvent, numOfBytes);
			break;
		}
	}

	return true;
}
