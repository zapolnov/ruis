#include <tst/set.hpp>
#include <tst/check.hpp>

#include <ruis/context.hpp>
#include <ruis/widget/container.hpp>
#include <ruis/updater.hpp>
#include <ruis/util/mouse_cursor.hpp>
#include <ruis/gui.hpp>

#include "../../harness/util/dummy_context.hpp"

namespace{
class container_which_invalidates_its_layout_during_layouting : public ruis::container{
public:
    container_which_invalidates_its_layout_during_layouting(
                utki::shared_ref<ruis::context> c
        ) :
            ruis::widget(std::move(c), {}, {}),
            ruis::container(this->context, {}, {})
    {}

    void on_lay_out()override{
        this->invalidate_layout();
    }
};
}

namespace{
const tst::set set("layouting", [](tst::suite& suite){
    suite.add("invalidate_layout_during_layouting_should_result_in_dirty_layout__lay_out_method", []{
        auto context = make_dummy_context();

        auto c = ruis::make::container(context, {});
        auto tc = utki::make_shared<container_which_invalidates_its_layout_during_layouting>(context);

        tst::check(tc.get().is_layout_dirty(), SL);
        c.get().push_back(tc);
        tst::check(tc.get().is_layout_dirty(), SL);

        // after performing layouting on parent container the child container's layout should be dirty
        // because it invalidates its layout during layouting in its on_lay_out() overridden method
        c.get().lay_out();
        tst::check(tc.get().is_layout_dirty(), SL);
    });

    suite.add("invalidate_layout_during_layouting_should_result_in_dirty_layout__resize_to_same_size", []{
        auto context = make_dummy_context();

        auto c = ruis::make::container(context, {});
        auto tc = utki::make_shared<container_which_invalidates_its_layout_during_layouting>(context);

        tst::check(tc.get().is_layout_dirty(), SL);
        c.get().push_back(tc);
        tst::check(tc.get().is_layout_dirty(), SL);

        // when resizing widget to the same size it should not do anything except perform re-layouting (TODO: why should it?) in case
        // layout was dirty
        tc.get().resize(tc.get().rect().d);
        tst::check(tc.get().is_layout_dirty(), SL);
    });

    suite.add("invalidate_layout_during_layouting_should_result_in_dirty_layout__resize_to_different_size", []{
        auto context = make_dummy_context();

        auto c = ruis::make::container(context, {});
        auto tc = utki::make_shared<container_which_invalidates_its_layout_during_layouting>(context);

        tst::check(tc.get().is_layout_dirty(), SL);
        c.get().push_back(tc);
        tst::check(tc.get().is_layout_dirty(), SL);

        // when resizing widget to different size it should change it's size and call on_resize() virtual method
        // which by default does re-layouting
        tc.get().resize(c.get().rect().d + ruis::vector2{1, 1});
        tst::check(tc.get().is_layout_dirty(), SL);
    });

    suite.add("invalidate_layout_during_layouting_should_result_in_dirty_layout__gui_render", []{
        auto context = make_dummy_context();

        auto tc = utki::make_shared<container_which_invalidates_its_layout_during_layouting>(context);

        tst::check(tc.get().is_layout_dirty(), SL);
        
        // when rendering GUI it should perform layoputing in case it was dirty
        ruis::gui gui(context);
        gui.set_root(tc);
        
        // after setting widget as gui root it will be resized to window size and hence it will be layed out
        tst::check(tc.get().is_layout_dirty(), SL);

        tc.get().invalidate_layout();

        // gui render method will check if layout is dirty and perform re-layouting
        gui.render(ruis::matrix4().set_identity());
        tst::check(tc.get().is_layout_dirty(), SL);
    });
});
}
