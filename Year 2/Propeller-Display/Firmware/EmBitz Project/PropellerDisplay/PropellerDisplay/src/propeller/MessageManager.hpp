#pragma once

#include <array>
#include <cstdint>
#include "ColourT.hpp"

class MessageManager {
public:

    void init();

    void display(std::array<std::uint8_t, 23>& msg, int count);
private:
    ColourT bgColour;
    ColourT textColour;
    int numCharDisplayed;
    std::array<std::uint8_t, 23> displayedMsg;
};
