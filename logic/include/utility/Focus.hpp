#pragma once

#include <optional>
#include <mutex>
#include <memory>

class Focus
{
    public:
    Focus();
    Focus& operator= (Focus& other);
    bool has_control();
    void claim_control();
    std::optional<std::reference_wrapper<Focus>> get_controller();

    private:

    bool control;
    Focus(const Focus& other) = delete;

    static std::mutex focus_mutex;
    static std::optional<std::reference_wrapper<Focus>> current_controller;
};