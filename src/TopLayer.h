///////////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2015 Rick Taylor
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

#ifndef INDIGO_TOPLAYER_H_INCLUDED
#define INDIGO_TOPLAYER_H_INCLUDED

#include "GUILayer.h"

namespace Indigo
{
	class MainWindow;
	class TopLayer;

	namespace Render
	{
		class MainWindow;

		class TopLayer : public GUILayer, public OOBase::EnableSharedFromThis<TopLayer>
		{
			friend class Indigo::TopLayer;

		public:
			TopLayer(Indigo::TopLayer* owner);

		private:
			Indigo::TopLayer* m_owner;

			bool create(const OOBase::SharedPtr<Render::MainWindow>& wnd);
		};
	}

	class TopLayer
	{
		friend class MainWindow;
	public:
		
	private:
		OOBase::SharedPtr<Render::TopLayer> m_layer;

		TopLayer();

		bool create(const OOBase::SharedPtr<Render::MainWindow>& wnd);
		void destroy();
	};
}

#endif // INDIGO_TOPLAYER_H_INCLUDED