/**
 * Created February 2, 2024
 *
 * The MIT License (MIT)
 * Copyright (c) 2024 K. Suwatchai (Mobizt)
 *
 *
 * Permission is hereby granted, free of charge, to any person returning a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */
#ifndef CORE_LIST_H
#define CORE_LIST_H
#include <Arduino.h>
#include <vector>
using namespace std;

namespace firebase
{
    static vector<uint32_t> firebase_app_list;
    static vector<uint32_t> firebase_client_list;
    static vector<uint32_t> firebase_result_list;
    static int slot_add = 0;
    static int slot_remove = 0;

    class List
    {
    private:
    public:
        List(){};
        ~List(){};

        void addRemoveList(vector<uint32_t> &list, uint32_t addr, bool add)
        {
            for (size_t i = 0; i < list.size(); i++)
            {
                if (list[i] == addr)
                {
                    if (add)
                        return;
                    else
                        list.erase(list.begin() + i);
                }
            }
            if (add)
                list.push_back(addr);
        }

        bool existed(vector<uint32_t> &list, uint32_t addr)
        {
            for (size_t i = 0; i < list.size(); i++)
            {
                if (list[i] == addr)
                    return true;
            }
            return false;
        }
    };
};

#endif
