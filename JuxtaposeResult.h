#pragma once
#include "ZType.h"

struct JuxtaposeResult {
    JuxtaposeResult(ZType* type = nullptr, bool advances = false, bool completed = false) {
        this->type = type;
        this->advances = advances;
        this->completed = completed;
    }

    ZType* type;
    bool advances;
    bool completed;
};
