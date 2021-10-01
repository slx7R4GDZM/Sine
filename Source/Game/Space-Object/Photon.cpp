// Copyright slx7R4GDZM
// Distributed under the terms of the MIT License.
// Refer to the License.txt file for details.

#include "Photon.h"

#include "../../Graphics/Vector-Generator.h"
#include "../../Other/Constants.h"
#include "../../Other/Vectors.h"

void Photon::spawn(float delta_time, float direction, float vel_x, float vel_y, Position base_pos)
{
    status = 18;
    photon_life = status * 4 - delta_time - 1;
    this->vel_x = clamp((lookup_cosine(direction) * 64) + vel_x, -111, 111);
    this->vel_y = clamp((  lookup_sine(direction) * 64) + vel_y, -111, 111);

    pos = base_pos;
    offset_position(delta_time, lookup_cosine(direction) * 128, vel_x, pos.x);
    offset_position(delta_time,   lookup_sine(direction) * 128, vel_y, pos.y);
}

void Photon::offset_position(float delta_time, float base_offset, float vel, float& pos)
{
    const float pos_offset = base_offset * 0.75f;
    update_position_axis(1, pos, pos_offset);
    update_position_axis(delta_time, pos, vel);
}

void Photon::update(float delta_time, Vector_Generator& vector_generator, RenderWindow& window)
{
    if (!status)
        return;

    if (status < TRUE_EXPLOSION_START)
    {
        photon_life -= delta_time;
        if (photon_life <= 0)
            status = INDISCERNIBLE;

        update_position(delta_time);
        draw(vector_generator, window);
    }
    else
    {
        draw_explosion(vector_generator, window);
        advance_explosion(((twos_complement(status) >> 4) + 1) * delta_time);
    }
}

void Photon::draw(Vector_Generator& vector_generator, RenderWindow& window) const
{
    set_position_and_size(MUL_1, vector_generator, window);
    vector_generator.process(PHOTON, window);
}

void Photon::fire_photon(float delta_time, Photon photon[], u8 max_photons, float direction, Space_Object space_object)
{
    for (u8 i = 0; i < max_photons; i++)
    {
        if (photon[i].status == INDISCERNIBLE)
        {
            photon[i].spawn(delta_time,
                            direction,
                            space_object.vel_x,
                            space_object.vel_y,
                            space_object.pos);
            return;
        }
    }
}

bool Photon::any(const Photon photon[], u8 max_photons)
{
    for (u8 i = 0; i < max_photons; i++)
    {
        if (photon[i].status != INDISCERNIBLE)
            return true;
    }
    return false;
}
