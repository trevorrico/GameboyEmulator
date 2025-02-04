#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>

int main()
{
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	
	GLFWwindow* window = glfwCreateWindow(200, 200, "Gameboy", nullptr, nullptr);
	if(window == nullptr)
	{
		glfwTerminate();
		return -1;
	}

	while(glfwWindowShouldClose(window) == false)
	{
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glfwTerminate();
	return 0;
}
