#pragma once
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "Buffer.h"
#include "Texture.h"
#include "Utility.h"

namespace mb
{
	class Mandelbrot
	{
	public:
		Mandelbrot(int width, int height, int iterations);
		~Mandelbrot();

		void Loop();

	private:
		void Update() const;
		void SetPalette() const;
		void SetPalette2() const;
		static void ScrollCallbackCaller(GLFWwindow* window, double xoffset, double yoffset);
		static void MouseButtonCallbackCaller(GLFWwindow* window, int button, int action, int mods);

		void ScrollCallback(GLFWwindow* window, double xoffset, double yoffset);
		void MouseButtonCallback(GLFWwindow* window, int button, int action, int mods);

		void HandleMouse();

		int width, height = 0;
		int iterations = 500;
		Buffer* buffer = nullptr;
		Texture* texture = nullptr;
		Bounds bounds = {-5, 5, -5, 5};
		Cords startCords = {};
		bool mouseDown = false;
	};
}
