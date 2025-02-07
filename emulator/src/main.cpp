#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>

#include "MemoryBus.h"
#include "CPU.h"

int main()
{
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	
	GLFWwindow* window = glfwCreateWindow(160, 144, "Gameboy", nullptr, nullptr);
	if(window == nullptr)
	{
		glfwTerminate();
		std::cout << "Failed to initialize GLFW window" << std::endl;
		return -1;
	}

	glfwMakeContextCurrent(window);

	if(!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	MemoryBus::Initialize();
	CPU::Initialize();

	while(glfwWindowShouldClose(window) == false)
	{
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	CPU::Shutdown();
	MemoryBus::Shutdown();

	glfwTerminate();
	return 0;
}
