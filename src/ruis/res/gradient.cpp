/*
ruis - GUI framework

Copyright (C) 2012-2025  Ivan Gagis <igagis@gmail.com>

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program. If not, see <http://www.gnu.org/licenses/>.
*/

/* ================ LICENSE END ================ */

#include "gradient.hpp"

#include <r4/vector.hpp>

#include "../config.hpp"
#include "../context.hpp"
#include "../util/util.hpp"

using namespace ruis::res;

ruis::res::gradient::gradient(utki::shared_ref<ruis::render::renderer> renderer) :
	renderer(std::move(renderer)),
	vao(this->renderer.get().obj().empty_vertex_array)
{}

void gradient::set(
	std::vector<std::tuple<real, color>>& stops, //
	bool vertical
)
{
	std::vector<r4::vector2<float>> vertices;
	//	std::vector<uint32_t> colors;
	std::vector<r4::vector4<float>> colors;
	for (auto& s : stops) {
		{
			auto c = std::get<1>(s);
			// TODO: implement some utility function for converting color from uint32_t
			auto clr = c.to_vec4f();

			colors.push_back(clr);
			colors.push_back(clr);
		}

		//		TRACE(<< "put color = " << std::hex << colors.back() << std::endl)

		if (vertical) {
			vertices.emplace_back(1, std::get<0>(s));
			vertices.emplace_back(0, std::get<0>(s));
		} else {
			ASSERT(!vertical)
			vertices.emplace_back(std::get<0>(s), 0);
			vertices.emplace_back(std::get<0>(s), 1);
		}
		//		TRACE(<< "put pos = " << vertices.back() << std::endl)
	}
	ASSERT(vertices.size() == colors.size())

	std::vector<uint16_t> indices;
	for (size_t i = 0; i != vertices.size(); ++i) {
		indices.push_back(uint16_t(i));
	}

	auto& r = this->renderer.get();

	// clang-format off
	this->vao = r.render_context.get().make_vertex_array(
		{
			r.render_context.get().make_vertex_buffer(utki::make_span(vertices)),
		 	r.render_context.get().make_vertex_buffer(utki::make_span(colors))
		},
		r.render_context.get().make_index_buffer(utki::make_span(indices)),
		render::vertex_array::mode::triangle_strip
	);
	// clang-format on
}

utki::shared_ref<gradient> gradient::load(
	const ruis::resource_loader& loader, //
	const tml::forest& desc,
	const papki::file& fi
)
{
	bool vertical = false;

	std::vector<std::tuple<real, color>> stops;

	const char* stop_c = "stop";

	for (auto& p : desc) {
		if (p.value == "vertical") {
			vertical = get_property_value(p).to_bool();
		} else if (p.value == stop_c) {
			real pos = 0;
			ruis::color color{};
			for (auto& pp : p.children) {
				if (pp.value == "pos") {
					pos = get_property_value(pp).to_float();
				} else if (pp.value == "color") {
					color = color::make_from({get_property_value(pp)});
				}
			}
			stops.emplace_back(pos, color);
		}
	}

	auto ret = utki::make_shared<gradient>(loader.renderer);

	ret.get().set(
		stops, //
		vertical
	);

	return ret;
}

void gradient::render(const ruis::matrix4& m) const
{
	this->renderer.get().shaders().pos_clr->render(
		m, //
		this->vao.get()
	);
}
