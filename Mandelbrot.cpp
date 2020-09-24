#include "Mandelbrot.h"
#include "Kernels.cuh"
#include <array>
#include <iostream>

extern GLFWwindow* window;

namespace mb
{
	Mandelbrot::Mandelbrot(int width, int height, int iterations) : width(width), height(height), iterations(iterations)
	{
		const double ar = 1.0 * width / height;
		bounds.left *= ar;
		bounds.right *= ar;

		glfwSetWindowSize(window, width, height);
		glfwSetWindowTitle(window, "Mandelbrot");

		glfwSetWindowUserPointer(window, this);
		glfwSetScrollCallback(window, ScrollCallbackCaller);
		glfwSetMouseButtonCallback(window, MouseButtonCallbackCaller);

		glViewport(0, 0, width, height);
		glClearColor(0.3f, 0.3f, 0.3f, 0.5f);
		glEnable(GL_TEXTURE_2D);
		glLoadIdentity();

		SetPalette();
		//SetPalette2();

		buffer = new Buffer(width * height * sizeof(int));
		texture = new Texture(width, height);

		InitCuda(*buffer);
	}

	Mandelbrot::~Mandelbrot()
	{
		glfwTerminate();
	}

	void Mandelbrot::Loop()
	{
		while (!glfwWindowShouldClose(window))
		{
			HandleMouse();
			glfwPollEvents();
			glClear(GL_COLOR_BUFFER_BIT);
			Update();

			glfwSwapBuffers(window);
		}
	}

	void Mandelbrot::Update() const
	{
		unsigned int* pixels = nullptr;
		texture->Bind();
		Calculate(pixels, bounds, {width, height}, iterations, *buffer);

		buffer->Bind();
		glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
		buffer->Unbind();

		glBegin(GL_QUADS);
		glTexCoord2f(0.0f, 0.0f);
		glVertex2f(-1.0f, -1.0f);

		glTexCoord2f(1.0f, 0.0f);
		glVertex2f(1.0f, -1.0f);

		glTexCoord2f(1.0f, 1.0f);
		glVertex2f(1.0f, 1.0f);

		glTexCoord2f(0.0f, 1.0f);
		glVertex2f(-1.0f, 1.0f);
		glEnd();
	}



	void Mandelbrot::SetPalette() const
	{
		std::array<Color, 256> palette{};

		int i;
		int ofs = 0;

		for (i = 0; i < 16; i++)
		{
			palette[i + ofs].r = 16 * (16 - abs(i - 16));
			palette[i + ofs].g = 0;
			palette[i + ofs].b = 16 * abs(i - 16);
		}
		ofs = 16;
		for (i = 0; i < 16; i++)
		{
			palette[i + ofs].r = 0;
			palette[i + ofs].g = 16 * (16 - abs(i - 16));
			palette[i + ofs].b = 0;
		}
		ofs = 32;
		for (i = 0; i < 16; i++)
		{
			palette[i + ofs].r = 0;
			palette[i + ofs].g = 0;
			palette[i + ofs].b = 16 * (16 - abs(i - 16));
		}
		ofs = 48;
		for (i = 0; i < 16; i++)
		{
			palette[i + ofs].r = 16 * (16 - abs(i - 16));
			palette[i + ofs].g = 16 * (16 - abs(i - 16));
			palette[i + ofs].b = 0;
		}
		ofs = 64;
		for (i = 0; i < 16; i++)
		{
			palette[i + ofs].r = 0;
			palette[i + ofs].g = 16 * (16 - abs(i - 16));
			palette[i + ofs].b = 16 * (16 - abs(i - 16));
		}
		ofs = 80;
		for (i = 0; i < 16; i++)
		{
			palette[i + ofs].r = 16 * (16 - abs(i - 16));
			palette[i + ofs].g = 0;
			palette[i + ofs].b = 16 * (16 - abs(i - 16));
		}
		ofs = 96;
		for (i = 0; i < 16; i++)
		{
			palette[i + ofs].r = 16 * (16 - abs(i - 16));
			palette[i + ofs].g = 16 * (16 - abs(i - 16));
			palette[i + ofs].b = 16 * (16 - abs(i - 16));
		}
		ofs = 112;
		for (i = 0; i < 16; i++)
		{
			palette[i + ofs].r = 16 * (8 - abs(i - 8));
			palette[i + ofs].g = 16 * (8 - abs(i - 8));
			palette[i + ofs].b = 16 * (8 - abs(i - 8));
		}

		SetLookup(palette);
	}

	void Mandelbrot::SetPalette2() const
	{
		std::array<Color, 256> palette{};

		for(int i = 0; i < 256; ++i)
		{
			double t = 1.0 * i / iterations;
			int r = static_cast<int>(9 * (1 - t) * t * t * t * 255);
			int g = static_cast<int>(15 * (1 - t) * (1 - t) * t * t * 255);
			int b = static_cast<int>(8.5 * (1 - t) * (1 - t) * (1 - t) * t * 255);
			palette[i].r = r;
			palette[i].g = g;
			palette[i].b = b;
			palette[i].a = 255;
		}
		SetLookup(palette);
	}

	void Mandelbrot::ScrollCallbackCaller(GLFWwindow* window, double xoffset, double yoffset)
	{
		auto brot = static_cast<Mandelbrot*>(glfwGetWindowUserPointer(window));
		brot->ScrollCallback(window, xoffset, yoffset);
	}

	void Mandelbrot::MouseButtonCallbackCaller(GLFWwindow* window, int button, int action, int mods)
	{
		auto brot = static_cast<Mandelbrot*>(glfwGetWindowUserPointer(window));
		brot->MouseButtonCallback(window, button, action, mods);
	}

	void Mandelbrot::ScrollCallback(GLFWwindow* window, double xoffset, double yoffset)
	{
		const double xLen = abs(bounds.left - bounds.right);
		const double yLen = abs(bounds.down - bounds.up);

		if (yoffset > 0)
		{
			bounds.up -= yLen / 4;
			bounds.down += yLen / 4;
			bounds.left += xLen / 4;
			bounds.right -= xLen / 4;
		}
		else
		{
			bounds.up += yLen / 4;
			bounds.down -= yLen / 4;
			bounds.left -= xLen / 4;
			bounds.right += xLen / 4;
		}
	}

	void Mandelbrot::MouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
	{
		if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
		{
			mouseDown = true;
			double xpos, ypos;
			glfwGetCursorPos(window, &xpos, &ypos);
			startCords.x = xpos;
			startCords.y = ypos;
		}
		else if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE)
		{
			mouseDown = false;
		}
		else if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS)
		{
			bounds = {-2, 2, -2, 2};
		}
	}

	void Mandelbrot::HandleMouse()
	{
		if (mouseDown)
		{
			double xpos, ypos;
			glfwGetCursorPos(window, &xpos, &ypos);

			const int deltaX = xpos - startCords.x;
			const int deltaY = (ypos - startCords.y) * -1;

			const double graphX = deltaX * abs(bounds.left - bounds.right) / width;
			const double graphY = deltaY * abs(bounds.up - bounds.down) / height;

			bounds.left -= graphX;
			bounds.right -= graphX;
			bounds.up -= graphY;
			bounds.down -= graphY;

			startCords.x = xpos;
			startCords.y = ypos;
		}
	}
}
