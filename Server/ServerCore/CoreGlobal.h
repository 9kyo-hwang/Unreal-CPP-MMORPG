#pragma once

extern class FThreadManager* GThreadManager;

// 매니저 간 호출 순서를 조정해주는 역할
class CoreGlobal
{
public:
	CoreGlobal();
	~CoreGlobal();
};

