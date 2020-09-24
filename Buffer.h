#pragma once
#include <GL/glew.h>

namespace mb
{
	class Buffer
	{
	public:
		Buffer(int size);
		~Buffer();

		void Bind() const;
		void Unbind() const;
		GLuint GetId() const;

	private:
		GLuint id = 0;
	};
}
