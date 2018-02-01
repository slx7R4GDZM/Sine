// Copyright slx7R4GDZM
// Distributed under the terms of the MIT License.
// Refer to the License.txt file for details.

#pragma once

#include "../Space-Object.h"

class Photon : public Space_Object
{
private:
    static void solve_position(const s8 base_offset, u8& pos_major, u8& pos_minor);
    void draw(Vector_Generator& vector_generator, RenderWindow& window) const;
public:
    void spawn(const u8 direction, const s8 vel_x, const s8 vel_y, const Position base_pos);
    void update(const u8 fast_timer, Vector_Generator& vector_generator, RenderWindow& window);
    static void fire_photon(Photon photon[], const u8 max_photons, const u8 direction, const Space_Object space_object);
    static bool any(const Photon photon[], const u8 max_photons);
};
