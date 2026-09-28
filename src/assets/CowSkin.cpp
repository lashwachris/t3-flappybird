#include "assets/CowSkin.h"

#include "render/Colors.h"

namespace {

constexpr PaletteEntry kPalette[] = {
    {'K', colors::rgb565(40, 30, 30)},     // Outline
    {'W', colors::rgb565(248, 248, 248)},  // Body and wing
    {'G', colors::rgb565(196, 200, 214)},  // Shading and feathers
    {'B', colors::rgb565(34, 32, 38)},     // Spots and hooves
    {'P', colors::rgb565(246, 170, 182)},  // Muzzle and ears
    {'R', colors::rgb565(196, 96, 120)},   // Nostrils and mouth
    {'H', colors::rgb565(226, 196, 120)},  // Horns
    {'E', colors::rgb565(20, 20, 20)},     // Eyes
};
constexpr uint8_t kPaletteSize = sizeof(kPalette) / sizeof(kPalette[0]);

// 28x20, facing right. Only the wing changes between frames.
constexpr const char* kWingUp[] = {
    "...KK.......................",
    "..KWWKKKK.........K.....K...",
    ".KWWKKWWWKK......KHK...KHK..",
    "..KKWWWWWWWK.....KHHKKKHHK..",
    "...KGWWGWWWK...KK.KWWWWWK.K.",
    "..KWWGWWGWWWK.KPPKBBWWWWWKPK",
    "...KWWGWWGWK.KPPPBBBBWWWWWPP",
    "...KWWWWWWWKKKKPPWBBWWWWWWPK",
    "....KKWWWKKWWWWKWWWEWWWEWWWK",
    "....KWKKKWWWWBBBWWWEWWWEWWWK",
    ".K.KWBBBWWWWWBBBWWWWWPPWWWWK",
    "KWKWBBBBBWWWWWWWWWWPPPPPPWK.",
    "WKKWWBBBWWWWWWWWWWPPRPPRPPK.",
    "WKKWWWWWWWWBBBWWWWPPPPPPPPK.",
    "BKKWWWWWWWWBBBWWWWWPPRRPPK..",
    "K..KWWWWWWWBBBWWWWKKKPPKK...",
    "...KWWGGGGGGGGGGWWWK.KK.....",
    "...KWWKWWWWWWWWWKWWK........",
    "...KBBKBBKKKKKBBKBBK........",
    "....KK.KK.....KK.KK.........",
};

constexpr const char* kWingMid[] = {
    "............................",
    "..................K.....K...",
    ".................KHK...KHK..",
    ".................KHHKKKHHK..",
    "...............KK.KWWWWWK.K.",
    "....KKKKK.....KPPKBBWWWWWKPK",
    ".KKKWWWWWKK..KPPPBBBBWWWWWPP",
    "KWWWWWWWWWWKKKKPPWBBWWWWWWPK",
    "WWWGWGWGWGWWKWWKWWWEWWWEWWWK",
    "KKWWWWWWWWWKWBBBWWWEWWWEWWWK",
    ".KKKWWWWWKKWWBBBWWWWWPPWWWWK",
    "KWKWKKKKKWWWWWWWWWWPPPPPPWK.",
    "WKKWWBBBWWWWWWWWWWPPRPPRPPK.",
    "WKKWWWWWWWWBBBWWWWPPPPPPPPK.",
    "BKKWWWWWWWWBBBWWWWWPPRRPPK..",
    "K..KWWWWWWWBBBWWWWKKKPPKK...",
    "...KWWGGGGGGGGGGWWWK.KK.....",
    "...KWWKWWWWWWWWWKWWK........",
    "...KBBKBBKKKKKBBKBBK........",
    "....KK.KK.....KK.KK.........",
};

constexpr const char* kWingDown[] = {
    "............................",
    "..................K.....K...",
    ".................KHK...KHK..",
    ".................KHHKKKHHK..",
    "...............KK.KWWWWWK.K.",
    "..............KPPKBBWWWWWKPK",
    ".............KPPPBBBBWWWWWPP",
    "......KKKKKKKKKPPWBBWWWWWWPK",
    "...KKKWKKKWWWWWKWWWEWWWEWWWK",
    "..KWWWWWWWKWWBBBWWWEWWWEWWWK",
    ".KWWWWWWWWWKWBBBWWWWWPPWWWWK",
    "KKWWGWWGWWWKWWWWWWWPPPPPPWK.",
    "WKWWWGWWGWWKWWWWWWPPRPPRPPK.",
    "WKWWWWWWWWKBBBWWWWPPPPPPPPK.",
    "KWWKKKWKKKWBBBWWWWWPPRRPPK..",
    "KKKKWWKWWWWBBBWWWWKKKPPKK...",
    "...KWWGGGGGGGGGGWWWK.KK.....",
    "...KWWKWWWWWWWWWKWWK........",
    "...KBBKBBKKKKKBBKBBK........",
    "....KK.KK.....KK.KK.........",
};

// Flap cycle: up, mid, down, mid.
constexpr PixelArt kFrames[] = {
    {28, 20, kWingUp, kPalette, kPaletteSize},
    {28, 20, kWingMid, kPalette, kPaletteSize},
    {28, 20, kWingDown, kPalette, kPaletteSize},
    {28, 20, kWingMid, kPalette, kPaletteSize},
};

}  // namespace

const CharacterSkin kCowSkin = {
    kFrames,
    sizeof(kFrames) / sizeof(kFrames[0]),
    0.09f,
    // Body and head only: the wing, horns, ears, tail and hooves don't collide.
    {4.0f, 6.0f, 20.0f, 11.0f},
};
