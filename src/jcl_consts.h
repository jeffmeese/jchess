#ifndef JCL_CONSTS_H
#define JCL_CONSTS_H

#include <cstdint>

namespace jcl
{

constexpr uint8_t CASTLE_NONE        = 0x00;
constexpr uint8_t CASTLE_WHITE_KING  = 0x01;
constexpr uint8_t CASTLE_WHITE_QUEEN = 0x02;
constexpr uint8_t CASTLE_BLACK_KING  = 0x04;
constexpr uint8_t CASTLE_BLACK_QUEEN = 0x08;
constexpr uint8_t INVALID_ENPASSANT_COLUMN = 8;

}

#endif // #ifndef JCL_CONSTS_H
