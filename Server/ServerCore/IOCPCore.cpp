#include "pch.h"
#include "IOCPCore.h"

FCompletionPort GCompletionPort;

FCompletionPort::FCompletionPort()
{
	Handle = ::CreateIoCompletionPort(INVALID_HANDLE_VALUE, nullptr, 0, 0);
	check(Handle != INVALID_HANDLE_VALUE);
}

FCompletionPort::~FCompletionPort()
{
	::CloseHandle(Handle);
}

// ϰ ϴ ( == Handle) CP 
bool FCompletionPort::Enqueue(ICompletion* Completion)
{
	return ::CreateIoCompletionPort(
		Completion->GetHandle(), 
		Handle, 
		reinterpret_cast<ULONG_PTR>(Completion), 
		0
	);
}

// Worker Thread ش ޼带  CP  Task  õ
bool FCompletionPort::Dequeue(uint32 TimeoutMilliseconds)
{
	DWORD NumberOfBytesTransferred = 0;
	ICompletion* Completion = nullptr;
	FOverlapped* Overlapped = nullptr;

	if (::GetQueuedCompletionStatus(
		Handle,
		&NumberOfBytesTransferred,
		reinterpret_cast<PULONG_PTR>(&Completion),
		reinterpret_cast<LPOVERLAPPED*>(&Overlapped),
		TimeoutMilliseconds))
	{
		Completion->Dispatch(Overlapped, NumberOfBytesTransferred);
	}
	else
	{
		if (::WSAGetLastError() == WAIT_TIMEOUT)
		{
			return false; // ŸӾƿ
		}
		else
		{
			if (Completion)
			{
				// TODO: α 
				Completion->Dispatch(Overlapped, NumberOfBytesTransferred);
			}
		}
	}

	return true;
}