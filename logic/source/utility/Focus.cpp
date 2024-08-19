#include "Focus.hpp"

std::mutex Focus::focus_mutex = {};
std::optional<std::reference_wrapper<Focus>> Focus::current_controller = {};

Focus::Focus()
{
    std::lock_guard<std::mutex> lock(Focus::focus_mutex);

    if (Focus::current_controller.has_value()) {
        control = false;
    }
    else {
        control = true;
        Focus::current_controller.emplace(std::ref(*this));
    }
}

bool Focus::has_control()
{
    std::lock_guard<std::mutex> lock(Focus::focus_mutex);
    return control;
}

std::optional<std::reference_wrapper<Focus>> Focus::get_controller()
{
    std::lock_guard<std::mutex> lock(Focus::focus_mutex);
    return Focus::current_controller;
}

void Focus::claim_control()
{
    std::lock_guard<std::mutex> lock(Focus::focus_mutex);

    if (!control) {
        if (Focus::current_controller.has_value()) {
            Focus::current_controller->get().control = false;
        }
        Focus::current_controller.emplace(std::ref(*this));
        control = true;
    }
}

Focus& Focus::operator=(Focus& other)
{
    std::lock_guard<std::mutex> lock(Focus::focus_mutex);

    if (other.control) {
        other.control = false;
        control = true;

        Focus::current_controller.emplace(std::ref(*this));
    }
    
    return *this;
}

