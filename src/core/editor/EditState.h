#pragma once

struct EditState
{
    // Exposure adjustment in EV/stops.
    //  0.0 = unchanged
    // +1.0 = twice the light
    // -1.0 = half the light
    float exposure = 0.0f;
};
