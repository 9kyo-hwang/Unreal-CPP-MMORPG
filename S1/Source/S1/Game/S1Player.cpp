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

	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
	// are set in the derived blueprint asset named ThirdPersonCharacter (to avoid direct content references in C++)
	Position = new Protocol::PlayerInfo();
}

AS1Player::~AS1Player()
{
	delete Position;
	Position = nullptr;
}


// Called when the game starts or when spawned
void AS1Player::BeginPlay()
{
	Super::BeginPlay();
}

void AS1Player::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	FVector Location = GetActorLocation();

	Position->set_x(Location.X);
	Position->set_y(Location.Y);
	Position->set_z(Location.Z);
	Position->set_yaw(GetControlRotation().Yaw);
}

void AS1Player::SetPosition(const Protocol::PlayerInfo& InInfo)
{
	if (Position->object_id() != 0)
	{
		check(Position->object_id() == InInfo.object_id());
	}

	Position->CopyFrom(InInfo);

	FVector Location(InInfo.x(), InInfo.y(), InInfo.z());
	SetActorLocation(Location);
}

bool AS1Player::IsMyPlayer() const
{
	return IsA(AS1MyPlayer::StaticClass());
}
