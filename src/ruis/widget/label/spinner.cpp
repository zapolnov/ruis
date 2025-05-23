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

#include "spinner.hpp"

#include <ratio>

#include <utki/math.hpp>

using namespace std::string_view_literals;

using namespace ruis;

spinner::spinner( //
	utki::shared_ref<ruis::context> context,
	all_parameters params
) :
	widget( //
		std::move(context),
		std::move(params.layout_params),
		std::move(params.widget_params)
	),
	image(
		this->context,
		{.blending_params = std::move(params.blending_params), //
		 .image_params = std::move(params.image_params)}
	)
{}

void spinner::render(const matrix4& matrix) const
{
	matrix4 matr(matrix);

	matr.translate(this->rect().d / 2);
	matr.rotate(ruis::quat(this->angle));
	matr.translate(-this->rect().d / 2);

	this->image::render(matr);
}

void spinner::set_active(bool active)
{
	if (active) {
		this->context.get().updater.get().start(utki::make_shared_from(*this));
	} else {
		this->context.get().updater.get().stop(*this);
	}
}

void spinner::update(uint32_t dt_ms)
{
	angle += real(utki::pi) / real(std::milli::den) * real(dt_ms);
}

utki::shared_ref<ruis::spinner> ruis::make::refresh(
	utki::shared_ref<ruis::context> context, //
	spinner::all_parameters params
)
{
	if (!params.image_params.img) {
		params.image_params.img = context.get().loader().load<res::image>("ruis_img_refresh"sv);
	}
	if (!params.image_params.disabled_img) {
		params.image_params.disabled_img = context.get().loader().load<res::image>("ruis_img_refresh_disabled"sv);
	}

	return utki::make_shared<ruis::spinner>(
		std::move(context), //
		std::move(params)
	);
}
