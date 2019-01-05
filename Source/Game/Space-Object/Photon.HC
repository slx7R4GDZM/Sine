// Copyright slx7R4GDZM
// Distributed under the terms of the MIT License.
// Refer to the License.TXT file for details.

#ifndef SINE_PHOTON_HC
#define SINE_PHOTON_HC

#include "Game/Space-Object.HC"
#include "Graphics/Vector-Generator.HC"
#include "Other/Constants.HC"
#include "Other/Vectors.HC"

U0 solve_position(I8 base_offset, U8* pos_major, U8* pos_minor)
{
    I8 pos_offset = ((base_offset >> 1) * 3) >> 1;
    update_position_axis(pos_major, pos_minor, pos_offset);
}

U0 spawn_photon(Space_Object* photon, U8 direction, I8 vel_x, I8 vel_y, Position base_pos)
{
    photon->status = 18;
    photon->vel_x_major = clamp_I8((lookup_cosine(direction) >> 1) + vel_x, -111, 111);
    photon->vel_y_major = clamp_I8((  lookup_sine(direction) >> 1) + vel_y, -111, 111);

    MemCpy(&photon->pos, &base_pos, sizeof(Position));
    solve_position(lookup_cosine(direction), &photon->pos.x_major, &photon->pos.x_minor);
    solve_position(  lookup_sine(direction), &photon->pos.y_major, &photon->pos.y_minor);
}

U0 draw_photon(Space_Object* photon, Vector_Generator* vector_generator, CDC* dc)
{
    set_position_and_size(photon, MUL_1, vector_generator, dc);
    process(vector_generator, PHOTON, dc);
}

U0 update_photon(Space_Object* photon, U8 fast_timer, Vector_Generator* vector_generator, CDC* dc)
{
    if (!photon->status)
        return;

    if (photon->status < TRUE_EXPLOSION_START)
    {
        if ((fast_timer & 3) == 0)
            photon->status--;

        update_position(photon);
        draw_photon(photon, vector_generator, dc);
    }
    else
    {
        draw_explosion(photon, vector_generator, dc);
        photon->status += (twos_complement(photon->status) >> 4) + 1;
    }
}

U0 fire_photon(Space_Object* photon, U8 max_photons, U8 direction, Space_Object space_object)
{
    U64 i;
    for (i = 0; i < max_photons; i++)
    {
        if (photon[i].status == INDISCERNIBLE)
        {
            spawn_photon(&photon[i],
                         direction,
                         space_object.vel_x_major,
                         space_object.vel_y_major,
                         space_object.pos);
            return;
        }
    }
}

Bool any(Space_Object* photon, U8 max_photons)
{
    U64 i;
    for (i = 0; i < max_photons; i++)
    {
        if (photon[i].status != INDISCERNIBLE)
            return TRUE;
    }
    return FALSE;
}

#endif
