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

#ifndef INDIGO_PARSER_H_INCLUDED
#define INDIGO_PARSER_H_INCLUDED

namespace Indigo
{
	class Image;
	class Font;
	class NinePatch;
	class ResourceBundle;
	
	class Parser
	{
	public:
		bool load(const OOBase::SharedPtr<ResourceBundle>& resource, const char* resource_name);

	protected:
		OOBase::SharedPtr<ResourceBundle> m_resource;

		struct error_pos_t
		{
			unsigned int m_line;
			unsigned int m_col;
		} m_error_pos;

		void syntax_error(const char* fmt, ...) OOBASE_FORMAT(printf,2,3);

		void skip_whitespace(const char*& p, const char* pe);
		const char*& inc_p(const char*& p, const char* pe);
		bool character(const char*& p, const char* pe, char c);
		bool ident(const char*& p, const char* pe, OOBase::ScopedString& i);
		bool type_name(const char*& p, const char* pe, OOBase::ScopedString& i);
		bool parse_uint(const char*& p, const char* pe, unsigned int& u);
		bool parse_uvec2(const char*& p, const char* pe, glm::uvec2& u);
		bool parse_uvec4(const char*& p, const char* pe, glm::uvec4& u);
		bool parse_int(const char*& p, const char* pe, int& i);
		bool parse_ivec2(const char*& p, const char* pe, glm::ivec2& i);
		bool parse_string(const char*& p, const char* pe, OOBase::ScopedString& s);
		bool parse_float(const char*& p, const char* pe, float& i);
		bool parse_colour(const char*& p, const char* pe, glm::vec4& c);

		OOBase::SharedPtr<ResourceBundle> cd_resource(const char* res_name, OOBase::ScopedString& filename);
		
		OOBase::SharedPtr<Image> load_image(const char*& p, const char* pe, const OOBase::ScopedString& image_name);
		OOBase::SharedPtr<Font> load_font(const char*& p, const char* pe, const OOBase::ScopedString& font_name);

		virtual bool load_top_level(const char*& p, const char* pe, const OOBase::ScopedString& type) = 0;
		
	private:
		typedef OOBase::HashTable<size_t,OOBase::SharedPtr<Image>,OOBase::ThreadLocalAllocator> image_hash_t;
		image_hash_t m_hashImages;

		typedef OOBase::HashTable<size_t,OOBase::SharedPtr<Font>,OOBase::ThreadLocalAllocator> font_hash_t;
		font_hash_t m_hashFonts;
	};
}

#define SYNTAX_ERROR_RETURN(expr,ret_val) return (syntax_error expr,ret_val)

#endif // INDIGO_PARSER_H_INCLUDED
