/*
 * SPDX-FileCopyrightText: 2026 Suwatchai K. <suwatchai@outlook.com>
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef CORE_UTILS_LIST_H
#define CORE_UTILS_LIST_H

#include <Arduino.h>
#include <vector>

namespace firebase_ns
{
    class List
    {
    public:
        List() {}

        ~List() {}

        void addRemoveList(std::vector<uint32_t> &vec, uint32_t addr, bool add)
        {
            for (size_t i = 0; i < vec.size(); i++)
            {
                if (vec[i] == addr)
                {
                    if (add)
                        return;
                    else
                        vec.erase(vec.begin() + i);
                }
            }
            if (add)
                vec.push_back(addr);
        }

        bool existed(const std::vector<uint32_t> &vec, uint32_t addr)
        {
            for (size_t i = 0; i < vec.size(); i++)
            {
                if (vec[i] == addr)
                    return true;
            }
            return false;
        }
    };
}
#endif
