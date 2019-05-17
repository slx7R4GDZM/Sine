// Copyright slx7R4GDZM
// Distributed under the terms of the MIT License.
// Refer to the License.txt file for details.

#pragma once

#include "../Space-Object.h"

class Photon : public Space_Object
{
public:
    void spawn(u8 direction, s8 vel_x, s8 vel_y, Position base_pos);
    void update(u8 fast_timer, Vector_Generator& vector_generator, RenderWindow& window);
    static void fire_photon(Photon photon[], u8 max_photons, u8 direction, Space_Object space_object);
    static bool any(const Photon photon[], u8 max_photons);
private:
    static void offset_position(s8 base_offset, u8& pos_major, u8& pos_minor);
    void draw(Vector_Generator& vector_generator, RenderWindow& window) const;
};
