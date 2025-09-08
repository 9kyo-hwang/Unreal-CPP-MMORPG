// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/S1MyPlayer.h"

#include "ClientPacketHandler.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"
#include "Protocol.pb.h"
#include "S1GameInstance.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Kismet/KismetMathLibrary.h"

// Sets default values
AS1MyPlayer::AS1MyPlayer()
{
	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 400.0f; // The camera follows at this distance behind the character	
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
	// are set in the derived blueprint asset named ThirdPersonCharacter (to avoid direct content references in C++)
}

void AS1MyPlayer::Move(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D MovementVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);

		// get right vector 
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		// add movement
		// Don't use GetActorForwardVector, GetActorRightVector: 액터 방향이 아닌 마우스 방향!
		AddMovementInput(ForwardDirection, MovementVector.Y);
		AddMovementInput(RightDirection, MovementVector.X);

		// Caching 
		DesiredInput = MovementVector;

		// GetActorRotation().Yaw: 실시간으로 변하는 액터의 회전값 -> 서버는 최종 회전값 1개만 얻고 싶음!
		DesiredMoveDirection = FVector::ZeroVector;
		DesiredMoveDirection += ForwardDirection * MovementVector.Y;
		DesiredMoveDirection += RightDirection * MovementVector.X;
		DesiredMoveDirection.Normalize();

		const FVector Location = GetActorLocation();
		const FRotator Rotator = UKismetMathLibrary::FindLookAtRotation(Location, Location + DesiredMoveDirection);
		DesiredYaw = Rotator.Yaw;
	}
}

void AS1MyPlayer::Look(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// add yaw and pitch input to controller
		AddControllerYawInput(LookAxisVector.X);
		AddControllerPitchInput(LookAxisVector.Y);
	}
}

// Called when the game starts or when spawned
void AS1MyPlayer::BeginPlay()
{
	Super::BeginPlay();

	if (const APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}
}

// Called every frame
void AS1MyPlayer::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	bool bForceSendPacket = false;
	// 급격하게 방향을 튼 경우 강제로 Packet을 전송하도록 세팅(조건 많아질 예정)
	if (LastDesiredInput != DesiredInput)
	{
		bForceSendPacket = true;
		LastDesiredInput = DesiredInput;
	}

	// 키보드 입력 유무에 따라 상태 설정
	if (DesiredInput == FVector2D::Zero())
	{
		SetMoveState(Protocol::MOVE_STATE_IDLE);
	}
	else
	{
		SetMoveState(Protocol::MOVE_STATE_RUN);
	}

	PacketSendTimer -= DeltaTime;
	if (PacketSendTimer <= 0.f || bForceSendPacket)
	{
		PacketSendTimer = PacketSendDelay;
		Protocol::C_MOVE Packet;

		Protocol::PlayerInfo* Info = Packet.mutable_info();
		Info->CopyFrom(*Super::CurrentInfo);
		Info->set_yaw(DesiredYaw);	// 보정 중인 현재 회전값이 아닌, 최종적으로 완료된 회전값
		Info->set_state(GetMoveState());	// 혹시 모르니

		Cast<US1GameInstance>(GWorld->GetGameInstance())->SendPacket(ClientPacketHandler::MakeSendBuffer(Packet));
	}
}

// Called to bind functionality to input
void AS1MyPlayer::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent)) {

		// Jumping
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &ACharacter::Jump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);

		// Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ThisClass::Move);
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Completed, this, &ThisClass::Move);

		// Looking
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &ThisClass::Look);
	}
}

