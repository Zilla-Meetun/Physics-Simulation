// SOFApp.cpp : Defines the entry point for the console application.
//
#include "stdafx.h"
#include "Week19App.h"

int main()
{
	Week19App *app = new Week19App();
	SOF::Window::CreateWindow(1024*2, 2*768, "Physics 3: Collision Resolution", app);
	SOF::Window::SetBackground(glm::vec4(0, 0, 0, 0));
	SOF::Window::Run();

    return 0;
}

