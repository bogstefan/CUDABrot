#include <iostream>
#include <GL/glew.h>
#include "Mandelbrot.h"

GLFWwindow* window;

void Init()
{
	if (!glfwInit())
	{
		std::cerr << "Error initializing Glew" << std::endl;
		__debugbreak();
	}
	window = glfwCreateWindow(500, 500, "Default", nullptr, nullptr);
	if (!window)
	{
		glfwTerminate();
		std::cerr << "Error creating window" << std::endl;
		__debugbreak();
	}
	glfwMakeContextCurrent(window);
	glewInit();
}

void Shutdown()
{
	glfwTerminate();
}

int main(int arc, char* argv[])
{
	Init();

	mb::Mandelbrot mandelbrot(1000, 1000, 500);
	mandelbrot.Loop();

	Shutdown();

	return 0;
}
