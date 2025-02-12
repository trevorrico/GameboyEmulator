#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "Application.h"

int main()
{
	if(!glfwInit())
	{
		return -1;
	}
	
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	
	Application* app = new Application();
	if(app->Initialize() >= 0)
	{
		app->Run();
	}
	else
	{
		std::cerr << "Failed to initialize application!" << std::endl;
	}

	delete app;

	glfwTerminate();
	return 0;
}
