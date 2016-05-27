///////////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2014 Rick Taylor
//
// This file is part of the Indigo boardgame engine.
//
// OOGL is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// OOGL is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with OOGL.  If not, see <http://www.gnu.org/licenses/>.
//
///////////////////////////////////////////////////////////////////////////////////

#ifndef INDIGO_SHADER_H_INCLUDED
#define INDIGO_SHADER_H_INCLUDED

#include <OOBase/String.h>
#include <OOBase/Table.h>

#include "OOGL.h"

namespace OOGL
{
	class Shader : public OOBase::NonCopyable
	{
		friend class Program;

	public:
		Shader(GLenum shaderType);
		~Shader();

		bool valid() const;

		bool compile(const char* sz, GLint len = 0);
		bool compile(const char *const *strings, GLsizei count);
		bool compile(const char *const *strings, const GLint* lengths, GLsizei count);

		template <GLsizei S>
		bool compile(const char *const strings[S], const GLint lengths[S])
		{
			return compile(strings,lengths,S);
		}

		template <GLsizei S>
		bool compile(const char *const strings[S])
		{
			return compile(strings,S);
		}

		OOBase::SharedString<OOBase::ThreadLocalAllocator> info_log() const;

	private:
		GLuint m_id;
	};

	class Program : public OOBase::NonCopyable, public OOBase::EnableSharedFromThis<Program>
	{
		friend class State;
		friend class StateFns;

	public:
		Program();
		~Program();

		bool link(const OOBase::SharedPtr<Shader>* shaders, size_t count);

		template <size_t S>
		bool link(const OOBase::SharedPtr<Shader> shaders[S])
		{
			return link(shaders,S);
		}

		OOBase::SharedString<OOBase::ThreadLocalAllocator> info_log() const;

		void attribute_location(const char* name, GLuint index);
		GLint attribute_location(const char* name) const;

		void fragdata_location(const char* name, GLuint color);
		GLint fragdata_location(const char* name) const;

		GLint uniform_location(const char* name) const;

		template <typename T>
		void uniform(const char* name, const T& v)
		{
			GLint l = uniform_location(name);
			if (l != -1)
				uniform(l,v);
		}

		template <typename T>
		void uniform(const char* name, const T& v, bool transpose)
		{
			GLint l = uniform_location(name);
			if (l != -1)
				uniform(l,v,transpose);
		}

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
		typedef OOBase::Table<OOBase::SharedString<OOBase::ThreadLocalAllocator>,GLint,OOBase::Less<OOBase::SharedString<OOBase::ThreadLocalAllocator> >,OOBase::ThreadLocalAllocator> map_t;
		mutable map_t m_mapUniforms;
		mutable map_t m_mapAttributes;

		void internal_use() const;
	};
}

#endif // INDIGO_SHADER_H_INCLUDED
