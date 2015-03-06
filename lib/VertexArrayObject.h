#ifndef INDIGO_VERTEXARRAYOBJECT_H_INCLUDED
#define INDIGO_VERTEXARRAYOBJECT_H_INCLUDED

#include "BufferObject.h"

namespace Indigo
{
	class VertexArrayObject : public OOBase::NonCopyable
	{
		friend class OOBase::AllocateNewStatic<OOBase::ThreadLocalAllocator>;
		friend class State;
		friend class StateFns;

	public:
		VertexArrayObject();
		~VertexArrayObject();

		void attribute(GLuint index, const OOBase::SharedPtr<BufferObject>& buffer, GLint components, GLenum type, bool normalized, GLsizei stride, const void *pointer);
		void attributeI(GLuint index, const OOBase::SharedPtr<BufferObject>& buffer, GLint components, GLenum type, GLsizei stride, const void *pointer);

		void attribute_divisor(GLuint index, GLuint divisor);

		void enable(GLuint index, bool enable = true);
		void disable(GLuint index)
		{
			return enable(index,false);
		}

		void draw(GLenum mode, GLint first, GLsizei count);
		void draw(GLenum mode, GLint first, GLsizei count, GLsizei instances, GLuint baseinstance = 0);
		void draw(GLenum mode, const GLint* firsts, const GLsizei* counts, GLsizei primcount);

		void draw_elements(GLenum mode, GLsizei count, GLenum type, GLsizeiptr offset = 0, GLint basevertex = 0);
		void draw_elements(GLenum mode, GLsizei count, GLenum type, GLsizeiptr offset, GLsizei instances, GLint basevertex = 0, GLuint baseinstance = 0);
		void draw_elements(GLenum mode, GLuint start, GLuint end, GLsizei count, GLenum type, GLsizeiptr offset = 0, GLint basevertex = 0);
		void draw_elements(GLenum mode, const GLsizei* counts, GLenum type, const GLsizeiptr* offsets, GLsizei primcount, const GLint* basevertices = NULL);

	private:
		GLuint m_array;

		void bind();
	};
}

#endif /// INDIGO_VERTEXARRAYOBJECT_H_INCLUDED
