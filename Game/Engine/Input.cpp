#include "pch.h"
#include "Input.h"

FInput::FInput()
	: Window(nullptr)
{
}

FInput::~FInput()
{
}

void FInput::Initialize(HWND InWindow)
{
	Window = InWindow;
	KeyStates.resize(KeyCodeCount, EKeyState::None);
}

void FInput::Update()
{
	if (Window != ::GetActiveWindow())
	{
		KeyStates.assign(KeyCodeCount, EKeyState::None);
		return;
	}

	BYTE ASCII[KeyCodeCount]{};
	if (!::GetKeyboardState(ASCII))  // 모든 키 상태를 한 번에 가져오는 함수
	{
		return;
	}

	for ( uint32 Key = 0; Key < KeyCodeCount; ++Key )
	{
		EKeyState& State = KeyStates[Key];
		if ( ASCII[Key] & 0x80 )	// if the high-order bit(== 16진수 80) is 1, the key is down
		{
			if ( State == EKeyState::Hold || State == EKeyState::Down )
			{
				State = EKeyState::Hold;
			}
			else
			{
				State = EKeyState::Down;
			}
		}
		else
		{
			if ( State == EKeyState::Hold || State == EKeyState::Down )
			{
				State = EKeyState::Up;
			}
			else
			{
				State = EKeyState::None;
			}
		}
	}
}
