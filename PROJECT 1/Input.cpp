#include "pch.h"
#include "Input.h"
#include <iostream>


Input::Input()
{
}

Input::~Input()
{
}

void Input::Initialise(HWND window)
{
	m_keyboard = std::make_unique<DirectX::Keyboard>();
	m_mouse = std::make_unique<DirectX::Mouse>();
	m_mouse->SetWindow(window);

	//-------MOUSE-------------------
	ShowCursor(FALSE);
	m_mouse->SetMode(DirectX::Mouse::Mode::MODE_RELATIVE);

	//-------BUTTONS------------------
	m_quitApp = false;

	m_GameInput.movForward = false;
	m_GameInput.movBack = false;
	m_GameInput.movRight = false;
	m_GameInput.movLeft = false;
	m_GameInput.movUp = false;
	m_GameInput.movDown = false;

	m_GameInput.deltaX = 0;
	m_GameInput.deltaY = 0;

	m_GameInput.mouseLeft = false;
	m_GameInput.mouseRight = false;

	m_GameInput.B = false;
	m_GameInput.H = false;
	m_GameInput.R = false;	
	m_GameInput.F = false;
	m_GameInput.V = false;
	m_GameInput.rightArrow = false;
	m_GameInput.leftArrow = false;
	m_GameInput.upArrow = false;
	m_GameInput.downArrow = false;
}



void Input::Update()
{
	auto kb = m_keyboard->GetState();	//updates the basic keyboard state
	m_KeyboardTracker.Update(kb);		//updates the more feature filled state. Press / release etc. 
	auto mouse = m_mouse->GetState();   //updates the basic mouse state
	m_MouseTracker.Update(mouse);		//updates the more advanced mouse state. 


	if (kb.Escape)// check has escape been pressed.  if so, quit out. 
	{
		m_quitApp = true;
	}

	//-----------------MOVEMENT--------------------
	//W key
	if (kb.W)	m_GameInput.movForward = true;
	else		m_GameInput.movForward = false;

	//S key
	if (kb.S)	m_GameInput.movBack = true;
	else		m_GameInput.movBack = false;

	//D key
	if (kb.D)	m_GameInput.movRight = true;
	else		m_GameInput.movRight = false;

	//A key
	if (kb.A)	m_GameInput.movLeft = true;
	else		m_GameInput.movLeft = false;


	//SPACE key
	if (kb.Space)	m_GameInput.movUp = true;
	else		m_GameInput.movUp = false;

	//LEFT SHIFT key
	if (kb.LeftShift)	m_GameInput.movDown = true;
	else		m_GameInput.movDown = false;

	//----------------ROTATION-------------------

	//--------YAW--------------------
	if (mouse.x != 0)
		m_GameInput.deltaX = mouse.x;
	else
		m_GameInput.deltaX = 0;
	//-------PITCH-----------------------
	if (mouse.y != 0)
		m_GameInput.deltaY = -mouse.y;
	else
		m_GameInput.deltaY = 0;

	//-----------------CHANGE SPEED-----------------
	//
	//LeftButton 
	if (mouse.leftButton)
		m_GameInput.mouseLeft = true;
	else
		m_GameInput.mouseLeft = false;
	//RightButton
	if (mouse.rightButton)
		m_GameInput.mouseRight = true;
	else
		m_GameInput.mouseRight = false;

	//-----------------MORE BUTTONS---------------

	if (kb.B)	m_GameInput.B = true;
	else		m_GameInput.B = false;

	if (kb.H)	m_GameInput.H = true;
	else		m_GameInput.H = false;

	if (kb.R)	m_GameInput.R = true;
	else		m_GameInput.R = false;

	if (kb.F)	m_GameInput.F = true;
	else		m_GameInput.F = false;

	if (kb.V)	m_GameInput.V = true;
	else		m_GameInput.V = false;

	//--------------PLAYER MOVEMENT---------------

	if (kb.Up)	m_GameInput.upArrow = true;
	else		m_GameInput.upArrow = false;

	if (kb.Down)	m_GameInput.downArrow = true;
	else		m_GameInput.downArrow = false;

	if (kb.Right)	m_GameInput.rightArrow = true;
	else		m_GameInput.rightArrow = false;

	if (kb.Left)	m_GameInput.leftArrow = true;
	else		m_GameInput.leftArrow = false;
}

bool Input::Quit()
{
	return m_quitApp;
}

InputCommands Input::getGameInput()
{
	return m_GameInput;
}
