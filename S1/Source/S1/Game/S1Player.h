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
	Protocol::PositionData* GetCurrentPosition() const { return CurrentPosition; }
	void SetCurrentPosition(const Protocol::PositionData& NewPosition);
	void SetDestinationPosition(const Protocol::PositionData& NewPosition) const;

	bool IsMyPlayer() const;

	Protocol::EMoveState GetMoveState() const { return CurrentPosition->move_state(); }
	void SetMoveState(Protocol::EMoveState NextState) const;

protected:
	Protocol::PositionData* CurrentPosition;
	Protocol::PositionData* DestinationPosition;
};
