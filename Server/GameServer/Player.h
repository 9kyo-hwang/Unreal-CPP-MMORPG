#pragma once
#include "Enum.pb.h"

class UPlayer
{
	using EPlayerType = Protocol::EPlayerType;

public:
	UPlayer();
	~UPlayer();

	uint64 PlayerId;
	string Name;
	EPlayerType Type;
	shared_ptr<class FClientSession> OwnerSession;	// TODO: Session <-> Player 순환 참조 해제
};

