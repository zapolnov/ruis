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

#include "tab.hpp"

#include "../../util/util.hpp"

using namespace ruis;

bool tab::mask_overlaps(vector2 pos)
{
	auto borders = this->get_actual_borders();

	if (pos.x() < borders.left()) {
		if (pos.y() != 0 && this->rect().d.y() != 0) {
			if (pos.x() / (this->rect().d.y() - pos.y()) < borders.left() / this->rect().d.y()) {
				return false;
			}
		}

		if (this->parent() && &this->parent()->children().front().get() != this) {
			// this is not the first widget in the parent

			auto prev_iter = this->parent()->find(*this);
			ASSERT(prev_iter != this->parent()->children().end())
			--prev_iter;

			if (auto prev_tab = dynamic_cast<tab*>(&prev_iter->get())) {
				if (prev_tab->is_pressed()) {
					if (prev_tab->mask_overlaps(pos + this->rect().p - prev_tab->rect().p)) {
						return false;
					}
				}
			}
		}
	} else {
		auto rb = this->rect().d.x() - borders.right();
		auto dx = pos.x() - rb;
		if (dx > 0) {
			if (pos.y() != 0 && this->rect().d.y() != 0) {
				if (dx / pos.y() > borders.right() / this->rect().d.y()) {
					return false;
				}
			}
		}
	}
	return true;
}

bool tab::on_mouse_button(const mouse_button_event& e)
{
	if (e.is_down) {
		if (!mask_overlaps(e.pos)) {
			return false;
		}
	}
	// we want exactly nine_patch_button::on_mouse_button()
	// NOLINTNEXTLINE(bugprone-parent-virtual-call)
	if (this->nine_patch_button::on_mouse_button(e)) {
		return true;
	}
	return this->choice_button::on_mouse_button(e);
}

void tab::on_pressed_change()
{
	this->choice_button::on_pressed_change();
	this->nine_patch_toggle::on_pressed_change();
}

tab::tab(
	utki::shared_ref<ruis::context> context, //
	all_parameters params,
	widget_list children
) :
	widget(
		std::move(context), //
		std::move(params.layout_params),
		std::move(params.widget_params)
	),
	button(this->context, ruis::button::parameters{}),
	toggle_button(this->context),
	choice_button(this->context),
	nine_patch_toggle(
		this->context,
		std::move(params.container_params),
		std::move(params.button_params),
		std::move(params.blending_params),
		std::move(params.nine_patch_params),
		std::move(params.nine_patch_button_params),
		std::move(children)
	)
{
	if (!this->get_pressed_nine_patch()) {
		this->set_pressed_nine_patch(
			this->context.get().loader().load<res::nine_patch>("ruis_npt_tab_active").to_shared_ptr()
		);
	}
	if (!this->get_unpressed_nine_patch()) {
		this->set_unpressed_nine_patch(
			this->context.get().loader().load<res::nine_patch>("ruis_npt_tab_inactive").to_shared_ptr()
		);
	}

	// initialize nine-patch
	this->tab::on_pressed_change();
}
