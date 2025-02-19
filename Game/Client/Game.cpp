#include "pch.h"
#include "Game.h"
#include "Engine.h"

void Game::Initialize(const FWindowInfo& Info)
{
	GEngine->Initialize(Info);
}

void Game::Update()
{
	GEngine->Render();
}
