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

#include "key_proxy.hpp"

using namespace ruis;

key_proxy::key_proxy( //
	utki::shared_ref<ruis::context> context,
	all_parameters params,
	widget_list children
) :
	widget( //
		std::move(context),
		std::move(params.layout_params),
		std::move(params.widget_params)
	),
	// clang-format off
	container( //
		this->context,
		{
			.container_params = std::move(params.container_params)
		},
		std::move(children)
	)
// clang-format on
{}

bool key_proxy::on_key(const ruis::key_event& e)
{
	if (this->key_handler) {
		return this->key_handler(*this, e);
	}
	return false;
}

utki::shared_ref<ruis::key_proxy> ruis::make::key_proxy(
	utki::shared_ref<ruis::context> context, //
	key_proxy::all_parameters params,
	widget_list children
)
{
	if (!params.container_params.layout) {
		params.container_params.layout = ruis::layout::pile;
	}

	return utki::make_shared<ruis::key_proxy>(
		std::move(context), //
		std::move(params),
		std::move(children)
	);
}
