#ifndef __CONSOLE_EXTEND_HPP__
#define __CONSOLE_EXTEND_HPP__

#include <cstdint>
#include "console.hpp"

namespace Console
{
    void drawBuffer(uint32_t* buffer);
    uint8_t checkKeys();
} // namespace Console

#endif // __CONSOLE_EXTEND_HPP__