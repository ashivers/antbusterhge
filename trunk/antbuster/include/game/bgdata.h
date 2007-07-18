#ifndef BGDATA_H
#define BGDATA_H

const int BGH_BLOCK_MAX     = 32;   //水平块数
const int BGV_BLOCK_MAX     = 25;   //垂直块数

const char bgData[BGH_BLOCK_MAX][BGV_BLOCK_MAX + 1] = 
{
    "0000000011111111111111111",
    "0000000111111111111111111",
    "0000001111111111111111111",
    "0000011111111111111111111",
    "0000111111111111111111111",
    "0001111111111111111111111",
    "0011111111111111111111111",
    "0111111111111111111111111",
    "1111111111111111111111111",
    "1111111111111111111111111",
    "1111111111111111111111111",
    "1111111111111111111111111",
    "1111111111111111111111111",
    "1111111111111111111111111",
    "1111111111111111111111111",
    "1111111111111111111111111",
    "1111111111111111111111111",
    "1111111111111111111111111",
    "1111111111111111111111111",
    "1111111111111111111111111",
    "1111111111111111111100011",
    "1111111111111111111100001",
    "1111111111111111111000000",
    "1111111111111111111000000",
    "1111111111111111110000000",
    "1111111111111111110000000",
    "1111111111111111100000000",
    "1111111111111111100000000",
    "1111111111111111000000000",
    "1111111111111111000000000",
    "1111111111111110000000000",
}

#endif // BGDATA_H