#pragma once

#include "TUI.hpp"
#include "Timer.hpp"

#include <string>
#include <vector>
#include <queue>
#include <functional>

/// @brief Base animated class. 
class Animated : public TUI::UI_Object
{
    public:
    Animated(float period, std::function<void(void)> on_tick);

    inline float get_period() { return timer.events.back().t_seconds;}
    inline void set_period(float period) { timer.events.back().t_seconds = period;}

    protected:
    Timer timer;
    virtual void draw_self(unsigned input = 0, int orig_y = 0, int orig_x = 0) override
    {
        if (visible) {
            timer.process();
        }
        UI_Object::draw_self();
    }
};

/// @brief Object that toggles its children' visibility once in a period.
class Blinker : public Animated
{
    public:
    Blinker(float period = 0.5) : Animated(period, [this](){this->tick();}) {}
    
    protected:
    void tick();
};

/// @brief Box with text moving around its contour.
class Crazy_box : public Animated
{
    public:
    Crazy_box(float delay = 0.1) : Animated(delay, [this](){this->tick();}) {}
    std::string text;
    unsigned text_color = 0;
    TUI::Rect rect;

    protected:
    int offset = 0;
    void tick(); // here
    std::pair<int, int> position_character(int position);
    void draw_self(unsigned input = 0, int orig_y = 0, int orig_x = 0) override;
};

/// @brief Text that appears dynamically as if it's being typed
class Rolling_text : public Animated
{
    public:
    Rolling_text(float delay = 0.1) : Animated(delay, [this](){this->tick();}) {}
    std::string origin_text;
    TUI::Text text;

    protected:
    int n_drawn_chars = 0;
    void tick();
    void draw_self(unsigned input = 0, int orig_y = 0, int orig_x = 0) override;
};