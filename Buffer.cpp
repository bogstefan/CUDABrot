#include "Buffer.h"

namespace mb
{
	Buffer::Buffer(int size)
	{
		glGenBuffersARB(1, &id);
		glBindBuffer(GL_PIXEL_PACK_BUFFER, id);
		glBufferData(GL_PIXEL_PACK_BUFFER, size, nullptr, GL_DYNAMIC_DRAW);
		glBindBuffer(GL_PIXEL_PACK_BUFFER, 0);
	}

	Buffer::~Buffer()
	{
		glBindBuffer(GL_TEXTURE_2D, 0);
		glDeleteBuffers(1, &id);
		id = 0;
	}

	void Buffer::Bind() const
	{
		glBindBuffer(GL_PIXEL_UNPACK_BUFFER, id);
	}

	void Buffer::Unbind() const
	{
		glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);
	}

	GLuint Buffer::GetId() const
	{
		return id;
	}
}
