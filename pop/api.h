#pragma once

namespace NAVIGATOR {

struct API : public Process {
    uint8_t clearColor;
    uint8_t lblColor;
    int32_t itemStrideX, itemOffsetX,  itemStrideY, itemOffsetY;
    int8_t moveX, moveY;
};

extern API api;

}
