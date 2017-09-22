// Copyright slx7R4GDZM
// Distributed under the terms of the MIT License.
// Refer to the License.txt file for details.

#include "Photon.h"

#include "../../Graphics/Vector-Generator.h"
#include "../../Other/Constants.h"
#include "../../Other/Vectors.h"

void Photon::spawn(const u8 direction, const s8 vel_x, const s8 vel_y, const Position base_pos)
{
    photon_timer = 0;
    vel_x_major = clamp_s8((lookup_cosine(direction) >> 1) + vel_x, -111, 111);
    vel_y_major = clamp_s8((lookup_sine(direction) >> 1) + vel_y, -111, 111);

    pos = base_pos;
    solve_position(lookup_cosine(direction), pos.x_major, pos.x_minor);
    solve_position(lookup_sine(direction), pos.y_major, pos.y_minor);

    status = 17;
}

void Photon::update(Vector_Generator& vector_generator, RenderWindow& window)
{
    if (status && status < TRUE_EXPLOSION_START)
    {
        photon_timer++;
        if (photon_timer == 5) // or == 4 and = 0?
        {
            photon_timer = 1;
            status--;
        }
        update_position();
        draw(vector_generator, window);
    }
    else if (status >= TRUE_EXPLOSION_START)
    {
        draw_explosion(vector_generator, window);
        status += 16 - (status - 1) / 16;
    }
}

// there's probably a much better way to do this
void Photon::solve_position(const s8 base_offset, u8& pos_major, u8& pos_minor)
{
    float temp_offset = base_offset / 2;
    if (base_offset < 0 && base_offset % 2)
        temp_offset--;

    temp_offset *= 1.5f;

    s8 pos_offset = temp_offset;
    if (pos_offset < 0 && pos_offset > temp_offset)
        pos_offset--;

    pos_minor += pos_offset;
    if (pos_offset < 0 && underflowed_u8(pos_minor, pos_minor - pos_offset))
        pos_major--;
    else if (pos_offset > 0 && overflowed_u8(pos_minor, pos_minor - pos_offset))
        pos_major++;
}

void Photon::draw(Vector_Generator& vector_generator, RenderWindow& window) const
{
    set_position_and_size(MUL_1, vector_generator, window);
    vector_generator.process(PHOTON, window);
}

void Photon::fire_photon(Photon photon[], const u8 max_photons, const u8 direction, const Space_Object space_object)
{
    bool photon_added = false;
    for (u8 i = 0; i < max_photons && !photon_added; i++)
    {
        if (photon[i].status == INDISCERNIBLE)
        {
            photon[i].spawn(direction,
                            space_object.get_vel_x(),
                            space_object.get_vel_y(),
                            space_object.get_position());
            photon_added = true;
        }
    }
}

bool Photon::any(const Photon photon[], const u8 max_photons)
{
    for (u8 i = 0; i < max_photons; i++)
    {
        if (photon[i].status != INDISCERNIBLE)
            return true;
    }
    return false;
}
