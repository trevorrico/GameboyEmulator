#ifndef APPLICATION_H_
#define APPLICATION_H_

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include <portable-file-dialogs.h>

#include "GameBoy.h"
#include "Renderer.h"

#include <iostream>

class Application
{
public:
	Application();
	~Application();

	int Initialize();

	void Run();
private:
	void AddBreakpoint(uint16_t address);
	void RemoveBreakpoint(uint16_t index);

	void ConfigureImGui();

	void RenderGUI();

	GLFWwindow* window;

	Renderer* renderer;

	// Gameboy variables
	GameBoy* gameboy;

	// UI variables
	bool show_cpu_debug = true;
	bool show_vram_view = false;
	bool show_breakpoints = false;
	uint32_t current_breakpoint_item = 0;
	std::string hex_val = "0000";
	bool paused = false;

	// Breakpoints
	std::vector<uint16_t> breakpoints;
};

#endif