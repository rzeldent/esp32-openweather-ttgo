#pragma once

// https://github.com/pavius/the-dot-factory
// https://github.com/amitjakhu/dripicons-weather

typedef struct
{
    const uint16_t *data;
    uint16_t width;
    uint16_t height;
    uint8_t dataSize;
} tImage;

#include "image_OpenWeather.h"

#define IMAGE_TRANSPARTENT_COLOR	0xFFFF

#include "image_01d.h"
#include "image_01n.h"
#include "image_02d.h"
#include "image_02n.h"
#include "image_03d.h"
#include "image_03n.h"
#include "image_04.h"
#include "image_09.h"
#include "image_10d.h"
#include "image_10n.h"
#include "image_11.h"
#include "image_13.h"
#include "image_50.h"
