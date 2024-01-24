#ifndef CORE_LIST_H
#define CORE_LIST_H
#include <Arduino.h>
#include <vector>
using namespace std;

namespace firebase
{

    static vector<uint32_t> firebase_app_list;
    static vector<uint32_t> firebase_client_list;

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
