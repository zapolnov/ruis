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

#include "resource_loader.hpp"

#include <papki/root_dir.hpp>
#include <papki/util.hpp>

#include "util/util.hpp"

using namespace ruis;

namespace {
constexpr const char* wording_include = "include";
constexpr const char* wording_include_subdirs = "include_subdirs";
} // namespace

decltype(resource_loader::res_packs)::const_iterator resource_loader::mount_res_pack(const papki::file& fi)
{
	ASSERT(!fi.is_open())

	std::string dir = fi.dir();

	if (fi.not_dir().size() == 0) {
		fi.set_path(dir + "main.res");
	}

	auto script = tml::read(fi);
	ASSERT(!fi.is_open())

	// handle includes
	for (auto& p : script) {
		if (p.value == wording_include) {
			fi.set_path(dir + get_property_value(p).string);
			this->mount_res_pack(fi);
			// TODO: remove "include" tree from the forest?
		} else if (p.value == wording_include_subdirs) {
			fi.set_path(fi.dir());
			for (auto& f : fi.list_dir()) {
				if (papki::is_dir(f)) {
					fi.set_path(dir + f);
					this->mount_res_pack(fi);
				}
			}
			// TODO: remove "include_subdirs" tree from the forest?
		}
	}

	this->res_packs.emplace_back(
		papki::root_dir::make(
			fi.spawn(), //
			dir
		),
		std::move(script)
	);

	ASSERT(this->res_packs.back().fi)
	ASSERT(!this->res_packs.back().script.empty())

	return std::prev(this->res_packs.end());
}

void resource_loader::unmount_res_pack(decltype(res_packs)::const_iterator id)
{
	this->res_packs.erase(id);
}

void resource_loader::res_pack_entry::add_to_cache(
	const utki::shared_ref<resource>& res, //
	std::string_view id
) const
{
	ASSERT(!utki::contains(this->cache, id))

	this->cache.insert(
		std::make_pair(
			id, //
			utki::make_weak(res)
		)
	);
}

std::shared_ptr<resource> resource_loader::res_pack_entry::find_in_cache(std::string_view id) const
{
	auto i = this->cache.find(id);

	if (i != this->cache.end()) {
		if (auto r = i->second.lock()) {
			return r;
		}
		this->cache.erase(i);
	}
	return nullptr; // no resource found with given id, return invalid reference
}

const tml::forest* resource_loader::res_pack_entry::find_resource_in_script(std::string_view id) const
{
	auto j = std::find(
		this->script.begin(), //
		this->script.end(),
		id
	);
	if (j != this->script.end()) {
		ASSERT(j->value.string == id)
		return &j->children;
	}

	return nullptr;
}
