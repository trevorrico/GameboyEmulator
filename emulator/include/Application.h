#ifndef APPLICATION_H_
#define APPLICATION_H_

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include <portable-file-dialogs.h>

#include "GameBoy.h"

#include <iostream>

class Application
{
public:
	Application();
	~Application();

	int Initialize();

	void Run();
private:
	void ConfigureImGui();

	void RenderGUI();

	GLFWwindow* window;

	// Gameboy variables
	GameBoy* gameboy;

	// UI variables
	bool show_cpu_debug = true;
	bool paused = false;
};

#endif