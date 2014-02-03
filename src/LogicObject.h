///////////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2013 Rick Taylor
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

#ifndef INDIGO_LOGICOBJECT_H_INCLUDED
#define INDIGO_LOGICOBJECT_H_INCLUDED

#include "Common.h"

namespace Indigo
{
	class LogicObject :	public OOBase::NonCopyable
	{
	public:
		LogicObject() : m_active(0)
		{}

	protected:
		bool call_fn(OOBase::CDRStream& input, OOBase::CDRStream& output)
		{
			if (!m_active)
			{
				bool (*callback)(OOBase::CDRStream& input, OOBase::CDRStream& output);
				if (!input.read(callback))
					LOG_ERROR_RETURN(("Failed to read callback: %s",OOBase::system_error_text(input.last_error())),false);

				return (callback && (*callback)(input,output));
			}
			return false;
		}

	private:
		bool (*m_render_fn)(OOBase::CDRStream&, OOBase::CDRStream&);
		OOBase::Atomic<int> m_active;
	};


}

#endif /* INDIGO_LOGICOBJECT_H_INCLUDED */
