#include "TUI.hpp"
#include "NCursesTUI.hpp"
#include "UI_Object.hpp"
#include "Rect.hpp"
#include "Text.hpp"
#include "Animations.hpp"

#include <ncurses.h>
#include <thread>
#include <string>

int main()
{
    TUI& tui = NCursesTUI::get();

    auto rect = std::make_shared<Rect>(Vector2i(30, 24), Vector2i(5, 3));
    rect->draw_filled = true;
    rect->set_all({' ', TUI::BRIGHTWHITE, TUI::GREEN});
    NCursesTUI::make_pretty(*rect);

    auto text = std::make_shared<Text>();
    text->position = {1, 5};
    text->background = TUI::TRANSPARENT;
    text->foreground = TUI::BRIGHTWHITE;
    text->set_text("Alignment test");

    TUI::UI_Object scene;
    scene.children = {TUI::UIobj_ptr(rect), TUI::UIobj_ptr(text)};

    auto nested_rect1 = std::make_shared<Rect>(Vector2i(5, 5));
    NCursesTUI::make_pretty(*nested_rect1);
    nested_rect1->draw_filled = true;
    nested_rect1->vertical_anchor = TUI::UI_Object::MIDDLE;
    nested_rect1->set_color(TUI::WHITE, TUI::RED);

    auto nested_rect2 = std::make_shared<Rect>(Vector2i(5, 5));
    NCursesTUI::make_pretty(*nested_rect2);
    nested_rect2->draw_filled = true;
    nested_rect2->horizontal_anchor = TUI::UI_Object::MIDDLE;
    nested_rect2->set_color(TUI::WHITE, TUI::BLUE);

    auto nested_rect3 = std::make_shared<Rect>(Vector2i(5, 5));
    NCursesTUI::make_pretty(*nested_rect3);
    nested_rect3->draw_filled = true;
    nested_rect3->horizontal_anchor = TUI::UI_Object::END;
    nested_rect3->vertical_anchor = TUI::UI_Object::END;
    nested_rect3->set_color(TUI::WHITE, TUI::BRIGHTBLACK);

    rect->children = {nested_rect1, nested_rect2, nested_rect3};

    //TUI::Text limits_indicator;

    while(tui.get_input() != '~' && tui.get_input() != '`') {
        tui.clear();
        Vector2i screen_dimensions = tui.get_screen_size();
        rect->size = screen_dimensions - Vector2i(10, 6);
        scene.draw(tui, tui.get_input());
        tui.render();

        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
}