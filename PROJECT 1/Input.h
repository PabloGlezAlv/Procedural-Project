#pragma once

struct InputCommands
{
	bool movForward;
	bool movBack;
	bool movLeft;
	bool movRight;
	bool movUp;
	bool movDown;

	bool mouseLeft;
	bool mouseRight;

	float deltaX;
	float deltaY;

	bool B;
	bool H;
	bool R;
	bool F;
	bool V;

	bool rightArrow;
	bool leftArrow;
	bool upArrow;
	bool downArrow;
};


class Input
{
public:
	Input();
	~Input();
	void Initialise(HWND window);
	void Update();
	bool Quit();
	InputCommands getGameInput();


private:
	bool m_quitApp;

	std::unique_ptr<DirectX::Keyboard>	m_keyboard;
	std::unique_ptr<DirectX::Mouse>		m_mouse;
	DirectX::Keyboard::KeyboardStateTracker m_KeyboardTracker;
	DirectX::Mouse::ButtonStateTracker		m_MouseTracker;
	InputCommands							m_GameInput;
};

