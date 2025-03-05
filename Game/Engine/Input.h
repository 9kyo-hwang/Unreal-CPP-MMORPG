#pragma once

enum class EKeyCode : uint8
{
	None,

	LeftMouseButton = VK_LBUTTON,
	RightMouseButton = VK_RBUTTON,

	UpArrow = VK_UP,
	DownArrow = VK_DOWN,
	LeftArrow = VK_LEFT,
	RightArrow = VK_RIGHT,
	Space = VK_SPACE,

	Alpha0 = '0',
	Alpha1 = '1',
	Alpha2 = '2',
	Alpha3 = '3',
	Alpha4 = '4',
	Alpha5 = '5',
	Alpha6 = '6',
	Alpha7 = '7',
	Alpha8 = '8',
	Alpha9 = '9',

	W = 'W',
	A = 'A',
	S = 'S',
	D = 'D',
	Q = 'Q',
	E = 'E'
};

enum class EKeyState : uint8
{
	None,
	Down,
	Hold,
	Up,
	End
};

constexpr int32 KeyCodeCount = UINT8_MAX;
constexpr int32 KeyStateCount = static_cast<int32>(EKeyState::End);

class FInput
{
public:
	void Initialize(HWND InWindow);
	void Update();

	bool GetButton(EKeyCode Key) { return GetState(Key) == EKeyState::Hold; }
	bool GetButtonDown(EKeyCode Key) { return GetState(Key) == EKeyState::Down; }
	bool GetButtonUp(EKeyCode Key) { return GetState(Key) == EKeyState::Up; }

private:
	EKeyState GetState(EKeyCode Key) { return KeyStates[static_cast<uint8>(Key)]; }

private:
	HWND Window;
	vector<EKeyState> KeyStates;
};

