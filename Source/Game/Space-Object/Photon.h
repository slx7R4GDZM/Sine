// Copyright slx7R4GDZM
// Distributed under the terms of the MIT License.
// Refer to the License.txt file for details.

#pragma once

#include "../Space-Object.h"

class Photon : public Space_Object
{
public:
    void spawn(float delta_time, float direction, float vel_x, float vel_y, Position base_pos);
    void update(float delta_time, Vector_Generator& vector_generator, RenderWindow& window);
    static void fire_photon(float delta_time, Photon photon[], u8 max_photons, float direction, Space_Object space_object);
    static bool any(const Photon photon[], u8 max_photons);

    float photon_life;
private:
    static void offset_position(float delta_time, float base_offset, float& pos);
    void draw(Vector_Generator& vector_generator, RenderWindow& window) const;
};
