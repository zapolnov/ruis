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

#include "drop_down_box.hpp"

#include <utki/debug.hpp>

#include "../../../default_style.hpp"
#include "../../group/overlay.hpp"
#include "../../label/gap.hpp"
#include "../../label/rectangle.hpp"
#include "../../proxy/mouse_proxy.hpp"

using namespace std::string_literals;
using namespace std::string_view_literals;

using namespace ruis::length_literals;

using namespace ruis;

namespace {
std::vector<utki::shared_ref<widget>> make_drop_down_box_widget_structure(utki::shared_ref<ruis::context> c)
{
	namespace m = make;

	// clang-format off
	return {
		m::pile(c,
			{
				.layout_params = {
					.dims = {ruis::dim::max, ruis::dim::max},
					.weight = 1
				},
				.widget_params = {
					.id = "ruis_dropdown_selection"s
				}
			}
		),
		m::gap(c,
			{
				.layout_params = {
					.dims = {3_pp, 0_px}
				}
			}
		),
		m::image(c,
			{
				.layout_params = {
					.dims = {ruis::dim::min, ruis::dim::fill}
				},
				.image_params = {
					.img = c.get().loader().load<ruis::res::image>("ruis_img_divider_vert"sv)
				}
			}
		),
		m::gap(c,
			{
				.layout_params = {
					.dims = {3_pp, 0_px}
				}
			}
		),
		m::image(c,
			{
				.image_params = {
					.img = c.get().loader().load<ruis::res::image>("ruis_img_dropdown_arrow"sv)
				}
			}
		)
	};
	// clang-format on
}
} // namespace

drop_down_box::drop_down_box(
	utki::shared_ref<ruis::context> context, //
	all_parameters params
) :
	widget(
		std::move(context), //
		std::move(params.layout_params),
		std::move(params.widget_params)
	),
	button(
		this->context, //
		button::parameters{}
	),
	nine_patch_push_button(
		this->context, //
		{
			.container_params = {.layout = ruis::layout::row},
			.nine_patch_button_params =
				[&c = this->context.get(), &params]() {
					if (!params.nine_patch_button_params.pressed_nine_patch) {
						params.nine_patch_button_params.pressed_nine_patch =
							c.loader().load<res::nine_patch>("ruis_npt_button_pressed"sv);
					}
					if (!params.nine_patch_button_params.unpressed_nine_patch) {
						params.nine_patch_button_params.unpressed_nine_patch =
							c.loader().load<res::nine_patch>("ruis_npt_button_normal"sv);
					}
					return std::move(params.nine_patch_button_params);
				}() //
		},
		make_drop_down_box_widget_structure(this->context)
	),
	selection_box(
		this->context, //
		this->get_widget_as<ruis::container>("ruis_dropdown_selection"),
		std::move(params.providable_params)
	)
{
	this->pressed_change_handler = [this](button& b) {
		if (!b.is_pressed()) {
			return;
		}

		this->show_drop_down_menu();
	};
}

bool drop_down_box::on_mouse_button(const mouse_button_event& e)
{
	if (e.is_down) {
		++this->num_mouse_buttons_pressed;
	} else {
		// TODO: twice I got this assert triggered, perhaps there are situations when
		//       button release event comes without prior button press, need to investigate
		ASSERT(this->num_mouse_buttons_pressed != 0)

		--this->num_mouse_buttons_pressed;
	}

	if (!e.is_down) {
		this->handle_mouse_button_up(true);
	}

	return this->nine_patch_push_button::on_mouse_button(e);
}

bool drop_down_box::on_mouse_move(const mouse_move_event& e)
{
	if (auto cm = this->current_drop_down_menu.lock()) {
		if (this->num_mouse_buttons_pressed != 0) {
			cm->on_mouse_move(
				mouse_move_event{
					e.pos + this->get_absolute_pos() - cm->get_absolute_pos(),
					e.pointer_id,
					e.ignore_mouse_capture
				}
			);
		}
	}

	return this->nine_patch_push_button::on_mouse_move(e);
}

void drop_down_box::show_drop_down_menu()
{
	if (!this->get_provider()) {
		return;
	}

	auto olay = this->try_get_ancestor<overlay>();
	if (!olay) {
		throw std::logic_error("drop_down_box: no overlay parent found");
	}

	// clang-format off
	auto np = ruis::make::pile(this->context,
		{},
		{
			ruis::make::gap(this->context,
				{
					.widget_params{
						.id = "ruis_min_size_forcer"s
					}
				}
			),
			ruis::make::mouse_proxy(this->context,
				{
					.layout_params{
						.dims{ruis::dim::fill, ruis::dim::fill}
					},
					.widget_params{
						.id = "ruis_drop_down_menu_mouse_proxy"s
					}
				}
			),
			ruis::make::nine_patch(this->context,
				{
					.layout_params{
						.dims{ruis::dim::max, ruis::dim::min}
					},
					.widget_params{
						.id = "ruis_contextmenu_content"s
					},
					.container_params{
						.layout = ruis::layout::column
					},
					.nine_patch_params{
						.nine_patch = this->context.get().loader().load<ruis::res::nine_patch>("ruis_npt_contextmenu_bg"sv)
					}
				}
			)
		}
	);
	// clang-format on

	// force minimum horizontal size of the drop down menu to be the same as the drop down box horizontal size
	{
		auto& min_size_forcer = np.get().get_widget("ruis_min_size_forcer"sv);

		auto& lp = min_size_forcer.get_layout_params();
		lp.dims.x() = length::make_px(this->rect().d.x());
	}

	auto& va = np.get().get_widget_as<ruis::nine_patch>("ruis_contextmenu_content").content();

	for (size_t i = 0; i != this->get_provider()->count(); ++i) {
		va.push_back(this->wrap_item(this->get_provider()->get_widget(i), i));
	}

	this->hovered_index = -1;

	np.get().get_widget_as<mouse_proxy>("ruis_drop_down_menu_mouse_proxy").mouse_button_handler =
		[this](mouse_proxy&, const mouse_button_event& e) -> bool {
		// LOG("button down = " << e.is_down << std::endl)
		if (!e.is_down) {
			this->handle_mouse_button_up(false);
		}

		return true;
	};

	this->current_drop_down_menu =
		olay->show_popup(np, this->pos_in_ancestor(vector2(0), olay) + vector2(0, this->rect().d.y())).to_shared_ptr();
}

void drop_down_box::handle_mouse_button_up(bool is_first_button_up_event)
{
	auto ddm = this->current_drop_down_menu.lock();
	if (!ddm) {
		return;
	}

	if (this->hovered_index < 0) {
		if (!is_first_button_up_event) {
			this->context.get().post_to_ui_thread([ddm]() {
				ddm->remove_from_parent(); // close drop down menu
			});
		}
		return;
	}
	this->set_selection(this->hovered_index);

	auto ddb = utki::make_shared_from(*this);

	this->context.get().post_to_ui_thread([ddb, ddm]() {
		ddm->remove_from_parent(); // close drop down menu
		if (ddb.get().selection_handler) {
			ddb.get().selection_handler(ddb.get());
		}
	});
}

utki::shared_ref<widget> drop_down_box::wrap_item(
	const utki::shared_ref<widget>& w, //
	size_t index
)
{
	// clang-format off
	auto wd = ruis::make::pile(this->context,
		{
			.layout_params{
				.dims{ruis::dim::max, ruis::dim::min}
			}
		},
		{
			ruis::make::mouse_proxy(this->context,
				{
					.layout_params{
						.dims{ruis::dim::fill, ruis::dim::fill}
					},
					.widget_params{
						.id = "ruis_dropdown_mouseproxy"s
					}
				}
			),
			ruis::make::rectangle(this->context,
				{
					.layout_params{
						.dims{ruis::dim::fill, ruis::dim::fill}
					},
					.widget_params{
						.id = "ruis_dropdown_color"s,
						.visible = false
					},
					.color_params{
						.color = this->context.get().style().get_color_highlight()
					}
				}
			)
		}
	);
	// clang-format on

	auto mp = wd.get().try_get_widget_as<mouse_proxy>("ruis_dropdown_mouseproxy");
	ASSERT(mp)

	auto cl = wd.get().try_get_widget_as<rectangle>("ruis_dropdown_color");
	ASSERT(cl)
	auto cl_weak = utki::make_weak(cl);

	wd.get().push_back(w);

	// TODO: which pointer id?
	mp->hovered_change_handler = [this, cl_weak, index](mouse_proxy& w, unsigned id) {
		// LOG("hover index = " << index << std::endl)
		if (auto c = cl_weak.lock()) {
			c->set_visible(w.is_hovered(id));
		}
		if (w.is_hovered(id)) {
			this->hovered_index = int(index);
			// LOG("this->hovered_index = " << this->hovered_index << std::endl)
		} else {
			if (this->hovered_index >= 0 && decltype(index)(this->hovered_index) == index) {
				// LOG("this->hovered_index = -1;" << std::endl)
				this->hovered_index = -1;
			}
		}
	};

	return wd;
}

void drop_down_box::on_reload()
{
	this->nine_patch_push_button::on_reload();
	this->selection_box::on_reload();
}
