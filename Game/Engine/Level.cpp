#include "pch.h"
#include "Level.h"

#include "CameraComponent.h"
#include "ConstantBuffer.h"
#include "Engine.h"
#include "Actor.h"
#include "LightComponent.h"
#include "Resources.h"

void ULevel::Awake()
{
	// shared_ptr 복사가 일어나지 않도록 const&로 순회
	for (const TSharedPtr<AActor>& GameObject : Actors )
	{
		GameObject->Initialize();
	}
}

void ULevel::Start()
{
	for (const TSharedPtr<AActor>& GameObject : Actors )
	{
		GameObject->BeginPlay();
	}
}

void ULevel::Update()
{
	for (const TSharedPtr<AActor>& GameObject : Actors )
	{
		GameObject->Tick();
	}
}

void ULevel::LateUpdate()
{
	for (const TSharedPtr<AActor>& GameObject : Actors )
	{
		GameObject->LateUpdate();
	}
}

void ULevel::FinalUpdate()
{
	for (const TSharedPtr<AActor>& GameObject : Actors )
	{
		GameObject->FinalUpdate();
	}
}

void ULevel::Render()
{
	PushLightData();	// 프레임 당 1번만
	ClearRTV();
	RenderShadow();
	RenderDeferred();
	RenderLights();
	RenderFinal();
	RenderForward();
}

void ULevel::SpawnActor(TSharedPtr<AActor> Actor)
{
	if (auto CameraComponent = Actor->GetCameraComponent())
	{
		Cameras.push_back(CameraComponent);
	}
	else if (auto LightComponent = Actor->GetLightComponent())
	{
		Lights.push_back(LightComponent);
	}

	Actors.push_back(Actor);
}

void ULevel::RemoveActor(TSharedPtr<AActor> Actor)
{
	if (auto CameraComponent = Actor->GetCameraComponent())
	{
		if (auto Iterator = ranges::find(Cameras, CameraComponent);
			Iterator != Cameras.end())
		{
			Cameras.erase(Iterator);
		}
	}
	else if (auto LightComponent = Actor->GetLightComponent())
	{
		if (auto Iterator = ranges::find(Lights, LightComponent);
			Iterator != Lights.end())
		{
			Lights.erase(Iterator);
		}
	}

	if (auto Iterator = ranges::find(Actors, Actor);
		Iterator != Actors.end())
	{
		Actors.erase(Iterator);
	}
}

void ULevel::PushLightData()
{
	FLightParameters Parameters{};
	for (auto& LightComponent : Lights)
	{
		LightComponent->SetIndex(Parameters.LightCount);	// Light를 Render할 때 Index를 통해 구분하기 위함
		Parameters.Lights[Parameters.LightCount++] = LightComponent->GetInfo();
	}

	CONSTANT_BUFFER(EConstantBufferType::Global)->SetGlobalGraphicsData(&Parameters, sizeof(Parameters));
}

void ULevel::ClearRTV()
{
	// TODO: CommandQueue에서 수행하던 ClearRenderTargetView를 이곳에서 수행
	int8 BackBufferIndex = GEngine->GetSwapChain()->GetBackBufferIndex();

	// ClearRenderTargetView로 인해 STATE_COMMON -> STATE_RENDER_TARGET
	GEngine->GetMultipleRenderTarget(ERenderTargetType::SwapChain)->ClearRTV(BackBufferIndex);
	GEngine->GetMultipleRenderTarget(ERenderTargetType::Shadow)->ClearRTV();
	GEngine->GetMultipleRenderTarget(ERenderTargetType::GeometryBuffer)->ClearRTV();
	GEngine->GetMultipleRenderTarget(ERenderTargetType::Lighting)->ClearRTV();
}

void ULevel::RenderShadow()
{
	GEngine->GetMultipleRenderTarget(ERenderTargetType::Shadow)->OMSetRenderTargets();

	for (auto& LightComponent : Lights)
	{
		if (LightComponent->GetLightType() == ELightType::Directional)
		{
			LightComponent->RenderShadow();
		}
	}

	GEngine->GetMultipleRenderTarget(ERenderTargetType::Shadow)->WaitForUseAsAResource();
}

void ULevel::RenderDeferred()
{
	GEngine->GetMultipleRenderTarget(ERenderTargetType::GeometryBuffer)->OMSetRenderTargets();	// Deferred OMSet

	/**
	 *	메인 카메라에 대해서 우선적으로 연산 수행
	 *	Sort - Deferred - Light - Final - Forward
	 *	그리고 나머지 카메라에 대해 Sort - Forward 2단계만 수행
	 */
	TSharedPtr<UCameraComponent> MainCamera = Cameras[0];	// 0번 카메라를 메인 카메라로 가정
	MainCamera->SortActors();
	MainCamera->RenderDeferred();
	GEngine->GetMultipleRenderTarget(ERenderTargetType::GeometryBuffer)->WaitForUseAsAResource();
}

void ULevel::RenderLights()
{
	TSharedPtr<UCameraComponent> MainCamera = Cameras[0];
	UCameraComponent::StaticViewMatrix = MainCamera->GetViewMatrix();
	UCameraComponent::StaticProjectionMatrix = MainCamera->GetProjectionMatrix();

	GEngine->GetMultipleRenderTarget(ERenderTargetType::Lighting)->OMSetRenderTargets();

	for ( auto& LightComponent : Lights )
	{
		LightComponent->Render();
	}

	GEngine->GetMultipleRenderTarget(ERenderTargetType::Lighting)->WaitForUseAsAResource();
}

void ULevel::RenderFinal()
{
	// 모든 정보를 가지고 취합시키는 단계 -> BackBuffer 세팅, Final 데이터를 통해 Render

	int8 BackBufferIndex = GEngine->GetSwapChain()->GetBackBufferIndex();
	GEngine->GetMultipleRenderTarget(ERenderTargetType::SwapChain)->OMSetRenderTargets(1, BackBufferIndex);

	Resources::Get()->Get<FMaterial>(L"Final")->PushGraphicsData();
	Resources::Get()->Get<UMesh>(L"Rectangle")->Render();
}

void ULevel::RenderForward()
{
	TSharedPtr<UCameraComponent> MainCamera = Cameras[0];
	MainCamera->RenderForward();

	for ( auto& CameraComponent : Cameras )
	{
		if ( CameraComponent == MainCamera ) continue;

		CameraComponent->SortActors();
		CameraComponent->RenderForward();
	}
}
