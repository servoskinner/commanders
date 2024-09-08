#include "TUI.hpp"
#include "Event_timer.hpp"

#include <string>
#include <vector>
#include <queue>
#include <functional>

class Animated : public TUI::UI_Object
{
    public:
    Animated(float period, std::function<void(void)> on_tick);

    inline float get_period() { return timer.events.back().t_seconds;}
    inline void set_period(float period) { timer.events.back().t_seconds = period;}

    protected:
    Event_timer timer;
    virtual void draw_self(unsigned input = 0, int orig_y = 0, int orig_x = 0) override
    {
        timer.process();
        UI_Object::draw_self();
    }
};

class Blinker : public Animated
{
    public:
    Blinker(float period = 0.5) : Animated(period, [this](){this->tick();}) {}
    
    protected:
    void tick();
 
};

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