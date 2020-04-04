#pragma once
#define EVENT(x) constexpr int (x) = __COUNTER__;   // NOLINT(cppcoreguidelines-macro-usage)

EVENT(E_GAME_STARTED)