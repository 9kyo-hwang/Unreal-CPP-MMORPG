#include "pch.h"
#include "InstanceManager.h"

#include <ranges>

#include "Actor.h"
#include "CameraComponent.h"
#include "InstanceBuffer.h"
#include "MeshComponent.h"
#include "SceneComponent.h"

/**
 * 기존에 카메라에서 셰이더 종류에 따라 렌더하던 것을 InstanceManager에게 위임
 */
void UInstanceManager::Render(vector<TSharedPtr<AActor>>& InActors)
{
    unordered_map<uint64, vector<TSharedPtr<AActor>>> Cache;
    for (TSharedPtr<AActor> Actor : InActors)
    {
        const uint64 InstanceID = Actor->GetMeshComponent()->GetInstanceID();
        Cache[InstanceID].emplace_back(Actor);
    }

    for(auto& [InstanceID, Actors] : Cache)
    {
        if(Actors.size() == 1)  // Instance로 그리는 경우가 아닌 경우
        {
            Actors.front()->GetMeshComponent()->Render();
            continue;
        }

        for(const TSharedPtr<AActor>& Actor : Actors)
        {
            FMatrix WorldMatrix = Actor->GetSceneComponent()->GetLocalToWorldMatrix();
            FInstanceParameters Parameters
            {
                .WorldMatrix = WorldMatrix,
                .WorldViewMatrix = WorldMatrix * UCameraComponent::StaticViewMatrix,
                .WorldViewProjectionMatrix = WorldMatrix * UCameraComponent::StaticViewMatrix * UCameraComponent::StaticProjectionMatrix,
            };
            AddInstance(InstanceID, Parameters);
        }

        Actors.front()->GetMeshComponent()->Render(BufferMap[InstanceID]);
    }
}

void UInstanceManager::ClearBuffer()
{
    for(const auto& Buffer : BufferMap | views::values)
    {
        Buffer->Empty();
    }
}

void UInstanceManager::AddInstance(uint64 InstanceID, FInstanceParameters& Parameters)
{
    if(!BufferMap.contains(InstanceID))
    {
        BufferMap[InstanceID] = MakeShared<FInstanceBuffer>();
    }

    BufferMap[InstanceID]->Add(Parameters);
}
