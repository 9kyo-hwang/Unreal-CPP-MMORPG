#pragma once

class FSocket;

class FSocketSubsystem
{
public:
	inline static LPFN_CONNECTEX Connect = nullptr;
	inline static LPFN_DISCONNECTEX Disconnect = nullptr;
	inline static LPFN_ACCEPTEX Accept = nullptr;

public:
	static void Init();
	static void Shutdown();
	static bool LoadSocketFunctions(SOCKET Socket);

	static std::unique_ptr<FSocket> CreateSocket();
};

template<typename T>
bool SetSockOpt(SOCKET Socket, int32 Level, int32 OptName, T OptVal)
{
	return ::setsockopt(Socket, Level, OptName, reinterpret_cast<char*>(&OptVal), sizeof(T)) == 0;
}