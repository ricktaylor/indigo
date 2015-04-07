#ifndef INDIGO_VERTEXARRAYOBJECT_H_INCLUDED
#define INDIGO_VERTEXARRAYOBJECT_H_INCLUDED

#include "OOGL.h"

namespace OOGL
{
	class BufferObject;

	class VertexArrayObject : public OOBase::NonCopyable, public OOBase::EnableSharedFromThis<VertexArrayObject>
	{
		friend class OOBase::AllocateNewStatic<OOBase::ThreadLocalAllocator>;
		friend class State;
		friend class StateFns;

	public:
		VertexArrayObject();
		~VertexArrayObject();

		static OOBase::SharedPtr<VertexArrayObject> none();

		void attribute(GLuint index, const OOBase::SharedPtr<BufferObject>& buffer, GLint components, GLenum type, GLsizei stride = 0, GLsizeiptr offset = 0);
		void attribute(GLuint index, const OOBase::SharedPtr<BufferObject>& buffer, GLint components, GLenum type, bool normalized, GLsizei stride = 0, GLsizeiptr offset = 0);

		void attribute_divisor(GLuint index, GLuint divisor);

		void enable_attribute(GLuint index, bool enable = true);
		void disable_attribute(GLuint index)
		{
			return enable_attribute(index,false);
		}

		void draw(GLenum mode, GLint first, GLsizei count);
		void draw(GLenum mode, const GLint* firsts, const GLsizei* counts, GLsizei primcount);

		void draw_instanced(GLenum mode, GLint first, GLsizei count, GLsizei instances);
		void draw_instanced(GLenum mode, GLint first, GLsizei count, GLsizei instances, GLuint baseinstance);

		void draw_elements(GLenum mode, GLsizei count, GLenum type);
		void draw_elements(GLenum mode, GLsizei count, GLenum type, GLsizeiptr offset);
		void draw_elements(GLenum mode, GLsizei count, GLenum type, GLsizeiptr offset, GLint basevertex);
		void draw_elements(GLenum mode, GLuint start, GLuint end, GLsizei count, GLenum type);
		void draw_elements(GLenum mode, GLuint start, GLuint end, GLsizei count, GLenum type, GLsizeiptr offset);
		void draw_elements(GLenum mode, GLuint start, GLuint end, GLsizei count, GLenum type, GLsizeiptr offset, GLint basevertex);
		void draw_elements(GLenum mode, const GLsizei* counts, GLenum type, const GLsizeiptr* offsets, GLsizei primcount);
		void draw_elements(GLenum mode, const GLsizei* counts, GLenum type, const GLsizeiptr* offsets, GLsizei primcount, const GLint* basevertices);

		void draw_elements_instanced(GLenum mode, GLsizei count, GLenum type, GLsizeiptr offset, GLsizei instances);
		void draw_elements_instanced(GLenum mode, GLsizei count, GLenum type, GLsizeiptr offset, GLsizei instances, GLint basevertex);
		void draw_elements_instanced(GLenum mode, GLsizei count, GLenum type, GLsizeiptr offset, GLsizei instances, GLint basevertex, GLuint baseinstance);

	private:
		GLuint m_array;

		VertexArrayObject(GLuint array);

		void bind();
	};
}

#endif /// INDIGO_VERTEXARRAYOBJECT_H_INCLUDED
