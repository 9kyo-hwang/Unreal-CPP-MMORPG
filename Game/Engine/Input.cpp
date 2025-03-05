#include "pch.h"
#include "Input.h"

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

	for (uint32 Key = 0; Key < KeyCodeCount; ++Key)
	{
		EKeyState& State = KeyStates[Key];
		if (::GetAsyncKeyState(Key) & 0x8000)
		{
			if (State == EKeyState::Hold || State == EKeyState::Down)
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
