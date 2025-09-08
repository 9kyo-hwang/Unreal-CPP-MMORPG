// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/S1Player.h"

#include "S1MyPlayer.h"
#include "Struct.pb.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

// Sets default values
AS1Player::AS1Player()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f); // ...at this rotation rate

	// Note: For faster iteration times these variables, and many more, can be tweaked in the Character Blueprint
	// instead of recompiling to adjust them
	GetCharacterMovement()->JumpZVelocity = 700.f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MaxWalkSpeed = 500.f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;
	GetCharacterMovement()->BrakingDecelerationFalling = 1500.0f;

	// 캐릭터 컨트롤러가 없을 때 이동이 되지 않는 현상을 해결하기 위해 추가 세팅
	GetCharacterMovement()->bRunPhysicsWithNoController = true;

	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
	// are set in the derived blueprint asset named ThirdPersonCharacter (to avoid direct content references in C++)
	CurrentInfo = new Protocol::PlayerInfo();
	DestinationLocation = new Protocol::PlayerInfo();
}

AS1Player::~AS1Player()
{
	delete CurrentInfo;
	delete DestinationLocation;
	CurrentInfo = nullptr;
	DestinationLocation = nullptr;
}


// Called when the game starts or when spawned
void AS1Player::BeginPlay()
{
	Super::BeginPlay();

	FVector Location = GetActorLocation();
	DestinationLocation->set_x(Location.X);
	DestinationLocation->set_y(Location.Y);
	DestinationLocation->set_z(Location.Z);
	DestinationLocation->set_yaw(GetControlRotation().Yaw);

	SetMoveState(Protocol::MOVE_STATE_IDLE);
}

void AS1Player::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	FVector Current = GetActorLocation();
	CurrentInfo->set_x(Current.X);
	CurrentInfo->set_y(Current.Y);
	CurrentInfo->set_z(Current.Z);
	CurrentInfo->set_yaw(GetControlRotation().Yaw);

	// MyPlayer인 경우 입력에 따라 변화, 아니라면 목적지 - 현재 위치 간 보정 수행
	if (!IsMyPlayer())
	{
		//FVector Destination = FVector(DestinationLocation->x(), DestinationLocation->y(), DestinationLocation->z());
		//FVector MoveDirection = Destination - Current;
		//const float DirectionLength = MoveDirection.Length();
		//MoveDirection.Normalize();

		//// 600.0f: 이동 속도
		//float Distance = FMath::Min((MoveDirection * 600.f * DeltaSeconds).Length(), DirectionLength);
		//FVector NextLocation = Current + MoveDirection * Distance;

		//SetActorLocation(NextLocation);

		switch (GetMoveState())
		{
		case Protocol::MOVE_STATE_RUN:
			// 애니메이션이 적용되도록 아래 코드로 변경
			SetActorRotation(FRotator(0, DestinationLocation->yaw(), 0));
			AddMovementInput(GetActorForwardVector());
			break;
		default: break;
		}
	}
}

void AS1Player::SetCurrentLocation(const Protocol::PlayerInfo& InInfo)
{
	if (CurrentInfo->object_id() != 0)
	{
		check(CurrentInfo->object_id() == InInfo.object_id());
	}

	CurrentInfo->CopyFrom(InInfo);

	FVector Location(InInfo.x(), InInfo.y(), InInfo.z());
	SetActorLocation(Location);
}

void AS1Player::SetDestinationLocation(const Protocol::PlayerInfo& InInfo) const
{
	if (CurrentInfo->object_id() != 0)
	{
		check(CurrentInfo->object_id() == InInfo.object_id());
	}

	// 세팅은 하되 이동을 하지는 않음
	DestinationLocation->CopyFrom(InInfo);
	SetMoveState(InInfo.state());	// 상태만 별도로 즉시 적용
}

bool AS1Player::IsMyPlayer() const
{
	return IsA(AS1MyPlayer::StaticClass());
}

void AS1Player::SetMoveState(Protocol::MoveState NextState) const
{
	if (GetMoveState() == NextState)
	{
		return;
	}

	CurrentInfo->set_state(NextState);
}
