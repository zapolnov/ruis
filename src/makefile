include prorab.mk
include prorab-license.mk
include prorab-install-dbgsrc.mk
include prorab-clang-format.mk

$(eval $(call prorab-config, ../config))

this__src_dir := ruis

this_name := $(this__src_dir)$(this_dbg)

this_soname := $(shell cat $(d)soname.txt)

this_srcs += $(call prorab-src-dir, $(this__src_dir))

#    this_cxxflags += -DM_RUIS_RENDER_WIDGET_BORDERS

ifeq ($(os),windows)
    # we don't run clang-tidy on windows becasue trick with -isystem (see else branch)
    # is not working in msys2 for some reason, i.e. compiler cannot find freetype headers then
    this_lint_cmd :=
    this_cxxflags += $(shell pkg-config --cflags freetype2)
else
    # use -isystem instead of -I for freetype because otherwise clang-tidy goes inside
    # of freetype headers and finds a lot of findings there
    this_cxxflags += $(subst -I,-isystem,$(shell pkg-config --cflags freetype2))
endif

this_ldlibs += $(shell pkg-config --libs freetype2)

# TODO: add linking to dbg versions of all libs
this_ldlibs += -l papki
this_ldlibs += -l tml
this_ldlibs += -l svgdom
this_ldlibs += -l svgren
this_ldlibs += -l utki$(this_dbg)
this_ldlibs += -l rasterimage
this_ldlibs += -l veg

this_ldlibs += -l m

$(eval $(prorab-build-lib))

$(eval $(prorab-clang-format))

this_src_dir := $(this__src_dir)
this_license_file := ../LICENSE
$(eval $(prorab-license))

$(eval $(prorab-install-dbgsrc))
