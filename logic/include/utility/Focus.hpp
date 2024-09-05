#pragma once

#include <optional>
#include <mutex>
#include <memory>

class Focus
{
    public:
    class Focus_space
    {
        public:
        std::mutex focus_mutex;
        std::optional<std::reference_wrapper<Focus>> current_controller;
        std::optional<std::reference_wrapper<Focus>> get_controller();
    };

    Focus(Focus_space& focus_space);
    Focus& operator= (Focus& other);
    bool has_control();
    void claim_control();
    
    private:
    bool control;
    Focus(const Focus& other) = delete;
    Focus_space& focus_space;    
};