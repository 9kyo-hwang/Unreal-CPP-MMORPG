// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
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

protected:
	virtual void BeginPlay() override;
};
