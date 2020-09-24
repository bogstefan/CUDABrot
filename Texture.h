#pragma once
#include <GL/glew.h>

namespace mb
{
	class Texture
	{
	public:
		Texture(int width, int height);
		~Texture();

		void Bind() const;

	private:
		GLuint id = 0;
	};
}
