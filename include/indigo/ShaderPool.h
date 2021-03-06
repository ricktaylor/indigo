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

#ifndef INDIGO_CORE_SHADERPOOL_H_
#define INDIGO_CORE_SHADERPOOL_H_

#include <OOGL/Shader.h>

#include "Resource.h"

namespace Indigo
{
	namespace ShaderPool
	{
		OOBase::SharedPtr<OOGL::Shader> add_shader(const char* name, GLenum shaderType, ResourceBundle& resource, const char* res_name = NULL);

		OOBase::SharedPtr<OOGL::Program> add_program(const char* name, const OOBase::SharedPtr<OOGL::Shader>* shaders, size_t count);
	}
}

#endif /* INDIGO_CORE_SHADERPOOL_H_ */
