#include "pch.h"
#include "Game.h"
#include "Engine.h"
#include "SceneManager.h"

void Game::Initialize(const FWindowInfo& Info)
{
	GEngine->Initialize(Info);

	// Scene Load는 컨텐츠단이 맞음
	SceneManager::Get()->LoadLevel(L"TestScene");
}

void Game::Update()
{
	GEngine->Update();	// ActiveScene이 결정됐으므로 내부적으로 해당하는 Update, Render 자동 수행
}
