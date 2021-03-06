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

#include "../include/indigo/ShaderPool.h"

#include "Common.h"

namespace
{
	class ShaderPool
	{
	public:
		typedef OOBase::HashTable<const char*,OOBase::SharedPtr<OOGL::Shader>,OOBase::ThreadLocalAllocator> shader_name_map_t;
		typedef OOBase::HashTable<GLenum,shader_name_map_t,OOBase::ThreadLocalAllocator> shader_map_t;
		shader_map_t m_shader_map;

		typedef OOBase::HashTable<const char*,OOBase::SharedPtr<OOGL::Program>,OOBase::ThreadLocalAllocator> program_map_t;
		program_map_t m_program_map;
	};

	typedef OOGL::ContextSingleton<ShaderPool> SHADER_POOL;
}

OOBase::SharedPtr<OOGL::Shader> Indigo::ShaderPool::add_shader(const char* name, GLenum shaderType, ResourceBundle& resource, const char* res_name)
{
	::ShaderPool::shader_map_t::iterator i = SHADER_POOL::instance().m_shader_map.find(shaderType);
	if (i)
	{
		::ShaderPool::shader_name_map_t::iterator j = i->second.find(name);
		if (j)
			return j->second;
	}

	if (!res_name)
		res_name = name;

	OOBase::SharedPtr<OOGL::Shader> shader;
	if (!resource.exists(res_name))
		LOG_ERROR_RETURN(("Failed to load shader %s, resource not found",name),shader);

	shader = OOBase::allocate_shared<OOGL::Shader,OOBase::ThreadLocalAllocator>(shaderType);
	if (!shader)
		LOG_ERROR_RETURN(("Failed to allocate shader: %s",OOBase::system_error_text()),shader);

	OOBase::SharedPtr<const char> text = Indigo::static_resources().load<char>(res_name);
	if (!text)
		return OOBase::SharedPtr<OOGL::Shader>();

	if (!shader->compile(text.get(),static_cast<GLint>(Indigo::static_resources().size(res_name))))
		LOG_ERROR_RETURN(("Failed to compile shader: %s",shader->info_log().c_str()),OOBase::SharedPtr<OOGL::Shader>());

	i = SHADER_POOL::instance().m_shader_map.find(shaderType);
	if (!i)
	{
		i = SHADER_POOL::instance().m_shader_map.insert(shaderType,::ShaderPool::shader_name_map_t());
		if (!i)
			LOG_WARNING(("Failed to insert shader into pool: %s",OOBase::system_error_text()));
	}
	if (i && !i->second.insert(name,shader))
		LOG_WARNING(("Failed to insert shader into pool: %s",OOBase::system_error_text()));

	return shader;
}

OOBase::SharedPtr<OOGL::Program> Indigo::ShaderPool::add_program(const char* name, const OOBase::SharedPtr<OOGL::Shader>* shaders, size_t count)
{
	::ShaderPool::program_map_t::iterator i = SHADER_POOL::instance().m_program_map.find(name);
	if (i)
		return i->second;

	OOBase::SharedPtr<OOGL::Program> program = OOBase::allocate_shared<OOGL::Program,OOBase::ThreadLocalAllocator>();
	if (!program)
		LOG_ERROR_RETURN(("Failed to allocate shader program: %s",OOBase::system_error_text()),program);

	if (!program->link(shaders,count))
		LOG_ERROR_RETURN(("Failed to link shader program: %s",program->info_log().c_str()),OOBase::SharedPtr<OOGL::Program>());

	if (!SHADER_POOL::instance().m_program_map.insert(name,program))
		LOG_WARNING(("Failed to insert program into pool: %s",OOBase::system_error_text()));

	return program;
}
