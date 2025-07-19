#pragma once
#include "IOCPCore.h"

class FInternetAddr;
class FSocket;
class FOverlapped_Accept;

class FListener : public ICompletion
{
public:
	FListener();
	~FListener();

	HANDLE GetHandle() override;
	void Dispatch(FOverlapped* Event, int32 NumBytes = 0) override;

	bool Run(const FInternetAddr& Addr);
	void Stop();

private:
	void RegisterAccept(FOverlapped_Accept* Event);
	void ProcessAccept(FOverlapped_Accept* Event);

protected:
	unique_ptr<FSocket> Socket;
	TArray<FOverlapped_Accept*> AcceptEvents;
};

