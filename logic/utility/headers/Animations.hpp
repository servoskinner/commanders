#pragma once

#include "Vector2i.hpp"
#include "TUI.hpp"
#include "UI_Object.hpp"
#include "Rect.hpp"
#include "Text.hpp"
#include "Timer.hpp"

#include <string>
#include <vector>
#include <queue>
#include <functional>

/// @brief Base animated class. Changes its state on tick.
class Animated : public TUI::UI_Object
{
    public:
    Animated(float period) : timer() {
        timer.events.emplace_back([this](){ this->tick(); }, period);
    };
    
    /// @brief If paused, the object is still visible but the animation does not play.
    bool paused = false;

    inline float get_period() { return timer.events.back().t_seconds;}
    inline void set_period(float period) { timer.events.back().t_seconds = period;}

    /// @brief Specify what the animated object does when its frame changes
    virtual void tick() = 0;

    protected:
    Timer timer;
    virtual void draw_self(TUI& tui, unsigned input = 0, Vector2i origin = {0, 0}) override
    {
        if (visible && !paused) {
            timer.process();
        }
        UI_Object::draw_self(tui, input, origin);
    }
};

/// @brief Object that toggles its children' visibility once in a period.
class Blinker : public Animated
{
    public:
    Blinker(float period = 0.5) : Animated(period) {}
    
    protected:
    void tick() override;
};

/// @brief Box with text moving along its contour. CRAZY!
class Crazy_box : public Animated
{
    public:
    Crazy_box(float period = 0.1) : Animated(period) {}
    std::string text;
    TUI::Color text_color = TUI::BLACK;
    TUI::Rect rect;

    Vector2i get_size() const override {
        return rect.get_size();
    }

    protected:
    int offset = 0;

    void tick() override {
        offset = (offset + 1) % ((rect.size.x + rect.size.y - 2)*2);
    };
    Vector2i position_character(int position);
    void draw_self(TUI& tui, unsigned input = 0, Vector2i origin = {0, 0}) override;
};

/// @brief Text that appears dynamically as if it's being typed
class Rolling_text : public Animated
{
    public:
    Rolling_text(float period = 0.1) : Animated(period) {}
    std::string origin_text;
    TUI::Text text;

    Vector2i get_size() const override {
        return text.get_size();
    }

    protected:

    int n_drawn_chars = 0;
    void tick() override;
    void draw_self(TUI& tui, unsigned input = 0, Vector2i origin = {0, 0}) override;
};