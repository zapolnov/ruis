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

#pragma once

#include "../../res/nine_patch.hpp"
#include "../base/blending_widget.hpp"

#include "image.hpp"
#include "padding.hpp"

namespace ruis {

/**
 * @brief Nine patch widget.
 * Nine patch widget displays a nine-patch and can hold child widgets in its central area.
 */
class nine_patch :
	public virtual widget, //
	public blending_widget,
	public padding
{
	std::shared_ptr<const res::nine_patch> cur_nine_patch;
	std::shared_ptr<const render::texture_2d> image_texture;

public:
	struct parameters {
		std::shared_ptr<const res::nine_patch> nine_patch;
		std::shared_ptr<const res::nine_patch> disabled_nine_patch;
	};

private:
	parameters params;

public:
	struct all_parameters {
		layout_parameters layout_params;
		widget::parameters widget_params;
		container::parameters container_params;
		padding::parameters padding_params;
		blending_widget::parameters blending_params;
		nine_patch::parameters nine_patch_params;
	};

	nine_patch(
		utki::shared_ref<ruis::context> context, //
		all_parameters params,
		widget_list children
	);

	nine_patch(const nine_patch&) = delete;
	nine_patch& operator=(const nine_patch&) = delete;

	nine_patch(nine_patch&&) = delete;
	nine_patch& operator=(nine_patch&&) = delete;

	~nine_patch() override = default;

	void render(const mat4& matrix) const override;

	void set_nine_patch(std::shared_ptr<const res::nine_patch> np);

	void set_disabled_nine_patch(std::shared_ptr<const res::nine_patch> np);

	void on_enabled_change() override;

private:
	void update_cur_nine_patch();

	void render_nine_patch(const mat4& matrix) const;

protected:
	sides<real> get_min_borders() const noexcept override;
};

namespace make {
inline utki::shared_ref<ruis::nine_patch> nine_patch(
	utki::shared_ref<ruis::context> context, //
	nine_patch::all_parameters params,
	widget_list children = {}
)
{
	return utki::make_shared<ruis::nine_patch>(
		std::move(context), //
		std::move(params),
		std::move(children)
	);
}

} // namespace make

} // namespace ruis
