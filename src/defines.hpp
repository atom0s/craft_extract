/**
 * craft_extract - Copyright (c) 2022 atom0s [atom0s@live.com]
 *
 * This file is part of craft_extract.
 *
 * craft_extract is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * craft_extract is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with craft_extract.  If not, see <https://www.gnu.org/licenses/>.
 */

#ifndef CRAFT_EXTRACT_DEFINES_HPP
#define CRAFT_EXTRACT_DEFINES_HPP

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

#include <Windows.h>
#include <algorithm>
#include <chrono>
#include <format>
#include <fstream>
#include <functional>
#include <iostream>
#include <map>
#include <ranges>
#include <sstream>
#include <string>
#include <thread>
#include <vector>

namespace craft_extract
{
    /**
     * Output File Format Mode Enumeration
     */
    enum class output_mode : int32_t
    {
        none   = 0,
        csv    = 1,
        json   = 2,
        sqlite = 3,
        text   = 4,
    };

    /**
     * Parser Function Forwards
     */
    using parse_f = std::function<bool(FILE*, const std::size_t)>;
    using save_f  = std::function<bool(const std::string& output, craft_extract::output_mode)>;

} // namespace craft_extract

#endif // CRAFT_EXTRACT_DEFINES_HPP
