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

#include "nine_patch_button.hpp"

#include "../../../context.hpp"
#include "../../../util/util.hpp"

using namespace ruis;

nine_patch_button::nine_patch_button( //
	utki::shared_ref<ruis::context> context,
	container::parameters container_params,
	button::parameters button_params,
	blending_widget::parameters blending_params,
	nine_patch::parameters nine_patch_params,
	parameters params,
	widget_list children
) :
	widget(std::move(context), {}, {}),
	button(this->context, std::move(button_params)),
	// clang-format off
	nine_patch(
		this->context,
		{
			.container_params = std::move(container_params),
			.blending_params = std::move(blending_params),
			.nine_patch_params = std::move(nine_patch_params)
		},
		std::move(children)
	),
	// clang-format on
	params(std::move(params))
{
	this->nine_patch_button::on_pressed_change();
}

void nine_patch_button::on_pressed_change()
{
	this->set_nine_patch(this->is_pressed() ? this->params.pressed_nine_patch : this->params.unpressed_nine_patch);
	this->button::on_pressed_change();
}

void nine_patch_button::set_pressed_nine_patch(std::shared_ptr<const res::nine_patch> np)
{
	this->params.pressed_nine_patch = std::move(np);
}

void nine_patch_button::set_unpressed_nine_patch(std::shared_ptr<const res::nine_patch> np)
{
	this->params.unpressed_nine_patch = std::move(np);
}
