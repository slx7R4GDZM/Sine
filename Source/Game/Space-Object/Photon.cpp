// Copyright slx7R4GDZM
// Distributed under the terms of the MIT License.
// Refer to the License.txt file for details.

#include "Photon.h"

#include "../../Graphics/Vector-Generator.h"
#include "../../Other/Constants.h"
#include "../../Other/Vectors.h"

void Photon::spawn(u8 direction, s8 vel_x, s8 vel_y, Position base_pos)
{
    status = 18;
    vel_x_major = clamp_s8((lookup_cosine(direction) >> 1) + vel_x, -111, 111);
    vel_y_major = clamp_s8((  lookup_sine(direction) >> 1) + vel_y, -111, 111);

    pos = base_pos;
    solve_position(lookup_cosine(direction), pos.x_major, pos.x_minor);
    solve_position(  lookup_sine(direction), pos.y_major, pos.y_minor);
}

void Photon::solve_position(s8 base_offset, u8& pos_major, u8& pos_minor)
{
    const s8 pos_offset = ((base_offset >> 1) * 3) >> 1;
    update_position_axis(pos_major, pos_minor, pos_offset);
}

void Photon::update(u8 fast_timer, Vector_Generator& vector_generator, RenderWindow& window)
{
    if (status && status < TRUE_EXPLOSION_START)
    {
        if ((fast_timer & 3) == 0)
            status--;

        update_position();
        draw(vector_generator, window);
    }
    else if (status >= TRUE_EXPLOSION_START)
    {
        draw_explosion(vector_generator, window);
        status += (twos_complement(status) >> 4) + 1;
    }
}

void Photon::draw(Vector_Generator& vector_generator, RenderWindow& window) const
{
    set_position_and_size(MUL_1, vector_generator, window);
    vector_generator.process(PHOTON, window);
}

void Photon::fire_photon(Photon photon[], u8 max_photons, u8 direction, Space_Object space_object)
{
    for (u8 i = 0; i < max_photons; i++)
    {
        if (photon[i].status == INDISCERNIBLE)
        {
            photon[i].spawn(direction,
                            space_object.vel_x_major,
                            space_object.vel_y_major,
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
