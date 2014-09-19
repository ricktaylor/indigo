///////////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2014 Rick Taylor
//
// This file is part of the Indigo boardgame engine.
//
// Indigo is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// Indigo is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with Indigo.  If not, see <http://www.gnu.org/licenses/>.
//
///////////////////////////////////////////////////////////////////////////////////

#ifndef INDIGO_SHADER_H_INCLUDED
#define INDIGO_SHADER_H_INCLUDED

#include "OOGL.h"

namespace Indigo
{
	class Shader : public OOBase::EnableSharedFromThis<Shader>
	{
		friend class Program;

	public:
		Shader(GLenum shaderType);
		virtual ~Shader();

		void compile(const GLchar* sz, GLint len = 0);
		void compile(const GLchar *const *strings, GLsizei count);
		void compile(const GLchar *const *strings, const GLint* lengths, GLsizei count);

		template <GLsizei S>
		void compile(const GLchar *const strings[S], const GLint lengths[S])
		{
			compile(strings,lengths,S);
		}

		template <GLsizei S>
		void compile(const GLchar *const strings[S])
		{
			compile(strings,S);
		}

		static const GLchar* get_gstap();

		bool compile_status() const;
		OOBase::String info_log() const;

	private:
		GLuint m_id;
	};

	class Program : public OOBase::EnableSharedFromThis<Program>
	{
		friend class State;

	public:
		Program();

		void link(const OOBase::SharedPtr<Shader>* shaders, size_t count);
		bool link_status() const;
		OOBase::String info_log() const;

		bool in_use() const;

		void attribute_location(const char* name, GLuint index);
		GLint attribute_location(const char* name) const;

		void fragdata_location(const char* name, GLuint color);
		GLint fragdata_location(const char* name) const;

		GLint uniform_location(const char* name) const;

		void uniform(GLint location, GLfloat v);
		void uniform(GLint location, GLint v);
		void uniform(GLint location, GLuint v);
		void uniform(GLint location, const glm::vec2& v);
		void uniform(GLint location, const glm::vec3& v);
		void uniform(GLint location, const glm::vec4& v);
		void uniform(GLint location, const glm::ivec2& v);
		void uniform(GLint location, const glm::ivec3& v);
		void uniform(GLint location, const glm::ivec4& v);
		void uniform(GLint location, const glm::uvec2& v);
		void uniform(GLint location, const glm::uvec3& v);
		void uniform(GLint location, const glm::uvec4& v);
		void uniform(GLint location, const glm::mat2& v, bool transpose = false);
		void uniform(GLint location, const glm::mat2x3& v, bool transpose = false);
		void uniform(GLint location, const glm::mat2x4& v, bool transpose = false);
		void uniform(GLint location, const glm::mat3& v, bool transpose = false);
		void uniform(GLint location, const glm::mat3x2& v, bool transpose = false);
		void uniform(GLint location, const glm::mat3x4& v, bool transpose = false);
		void uniform(GLint location, const glm::mat4& v, bool transpose = false);
		void uniform(GLint location, const glm::mat4x2& v, bool transpose = false);
		void uniform(GLint location, const glm::mat4x3& v, bool transpose = false);

	private:
		GLuint m_id;

		void use();
	};
}

#endif // INDIGO_SHADER_H_INCLUDED
