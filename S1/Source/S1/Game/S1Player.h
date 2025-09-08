// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Struct.pb.h"
#include "GameFramework/Character.h"
#include "S1Player.generated.h"

class USpringArmComponent;
class UCameraComponent;
class UInputMappingContext;
class UInputAction;
struct FInputActionValue;

UCLASS()
class S1_API AS1Player : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AS1Player();
	virtual ~AS1Player() override;

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;

public:
	Protocol::PlayerInfo* GetCurrentLocation() const { return CurrentInfo; }
	void SetCurrentLocation(const Protocol::PlayerInfo& InInfo);
	void SetDestinationLocation(const Protocol::PlayerInfo& InInfo) const;

	bool IsMyPlayer() const;

	Protocol::MoveState GetMoveState() const { return CurrentInfo->state(); }
	void SetMoveState(Protocol::MoveState NextState) const;

protected:
	Protocol::PlayerInfo* CurrentInfo;
	Protocol::PlayerInfo* DestinationLocation;
};
