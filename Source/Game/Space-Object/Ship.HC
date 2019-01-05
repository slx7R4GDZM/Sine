// Copyright slx7R4GDZM
// Distributed under the terms of the MIT License.
// Refer to the License.TXT file for details.

#ifndef SINE_SHIP_HC
#define SINE_SHIP_HC

#include "Game/Space-Object.HC"
#include "Graphics/Vector-Generator.HC"
#include "Other/Constants.HC"
#include "Other/Vectors.HC"

U0 spawn_ship(Space_Object* ship)
{
    ship->vel_x_major = 0;
    ship->vel_y_major = 0;

    ship->pos.x_major = 16;
    ship->pos.y_major = 12;
    ship->pos.x_minor = 96;
    ship->pos.y_minor = 96;
}

U0 crash_ship(Space_Object* ship, U8* player_lives, U8* ship_spawn_timer)
{
    (*player_lives)--;
    ship->status = EXPLOSION_START;
    *ship_spawn_timer = 129;
}

U0 update_explosion_offset(Space_Object* ship, Offset* offset, I8 velocity, U16* delta)
{
    if (ship->status < 162)
        offset->major = velocity >> 4;

    update_position_axis(&offset->major, &offset->minor, velocity);

    I16 long_vector_delta = offset->major;
    if (velocity < 0)
        long_vector_delta -= 256;

    // return VCTR delta equivalent to explosion offset
    *delta = (velocity < 0) << 10 | AbsI64(long_vector_delta);
}

U0 handle_explosion(Space_Object* ship, Offset* explosion_x, Offset* explosion_y, Vector_Generator* vector_generator, CDC* dc)
{
    U16 vector_object[6 * 6 + 1];
    U8 final_index;
    Bool done = FALSE;
    U64 i;
    for (i = 0; i < 6 && !done; i++)
    {
        U16 delta_x;
        U16 delta_y;
        update_explosion_offset(ship, &explosion_x[i], EXPLOSION_VELOCITY[i].x, &delta_x);
        update_explosion_offset(ship, &explosion_y[i], EXPLOSION_VELOCITY[i].y, &delta_y);

        U16 word_0 = VCTR_9 << 12 | delta_y;
        U16 word_1 =                delta_x;

        U16 explosion_part[6];

        // move to the explosion
        explosion_part[0] = word_0;
        explosion_part[1] = word_1;

        // explosion
        explosion_part[2] = SHIP_EXPLOSION[i];

        // back to start of explosion
        explosion_part[3] = (SHIP_EXPLOSION[i] & 0xFF0F) ^ 0x0404;

        // back to ship position
        explosion_part[4] = word_0 ^ 0x0400;
        explosion_part[5] = word_1 ^ 0x0400;

        MemCpy(&vector_object[i * 6], explosion_part, sizeof(explosion_part));

        // make pieces disappear as the ship status gets higher
        final_index = (i + 1) * 6;
        if (ship->status > 255 - (i + 1) * 16)
            done = TRUE;
    }
    vector_object[final_index] = RTSL << 12;
    set_position_and_size(ship, MUL_1, vector_generator, dc);
    process(vector_generator, vector_object, dc);
}

U0 draw_ship(Space_Object* ship, U8 direction, Bool draw_thrust, Vector_Generator* vector_generator, CDC* dc)
{
    Bool flip_x = FALSE;
    Bool flip_y = FALSE;

    U8 vector_offset;
    if (direction < 64)
        vector_offset = direction / 4;
    else if (direction < 128)
    {
        vector_offset = 31 - (direction - 1) / 4;
        flip_x = TRUE;
    }
    else if (direction < 192)
    {
        vector_offset = (direction - 128) / 4;
        flip_x = TRUE;
        flip_y = TRUE;
    }
    else
    {
        vector_offset = 63 - (direction - 1) / 4;
        flip_y = TRUE;
    }
    set_position_and_size(ship, DIV_4, vector_generator, dc);
    process(vector_generator, SHIP_TABLE, dc, SHIP_OFFSET_TABLE[vector_offset], flip_x, flip_y);

    if (draw_thrust)
        process(vector_generator, SHIP_THRUST_TABLE, dc, SHIP_THRUST_OFFSET_TABLE[vector_offset], flip_x, flip_y);
}

U0 update_ship(Space_Object* ship, U8 fast_timer, U8 direction, Offset* explosion_x, Offset* explosion_y, Bool thrusting, Vector_Generator* vector_generator, CDC* dc)
{
    if (!ship->status)
        return;

    if (ship->status < TRUE_EXPLOSION_START)
    {
        update_position(ship);
        Bool draw_thrust = thrusting && fast_timer & 4;
        draw_ship(ship, direction, draw_thrust, vector_generator, dc);
    }
    else
    {
        handle_explosion(ship, explosion_x, explosion_y, vector_generator, dc);
        if (fast_timer & 1)
        {
            ship->status++;
            if (ship->status == INDISCERNIBLE)
                spawn_ship(ship);
        }
    }
}

U0 negative_vel_change(I8* vel_major, U8* vel_minor, U8 old_vel_minor)
{
    if (underflowed_U8(*vel_minor, old_vel_minor))
    {
        if (*vel_major == MIN_VEL)
            *vel_minor = 1;
        else
            (*vel_major)--;
    }
}

U0 positive_vel_change(I8* vel_major, U8* vel_minor, U8 old_vel_minor)
{
    if (overflowed_U8(*vel_minor, old_vel_minor))
    {
        if (*vel_major == MAX_VEL)
            *vel_minor = 255;
        else
            (*vel_major)++;
    }
}

U0 add_thrust(Space_Object* ship, U8 direction, U8* vel_x_minor, U8* vel_y_minor)
{
    // x
    U8 old_vel_x_minor = *vel_x_minor;
    *vel_x_minor += lookup_cosine(direction) * 2;

    if (direction > 64 && direction < 192) // left
        negative_vel_change(&ship->vel_x_major, vel_x_minor, old_vel_x_minor);
    else // right
        positive_vel_change(&ship->vel_x_major, vel_x_minor, old_vel_x_minor);

    // y
    U8 old_vel_y_minor = *vel_y_minor;
    *vel_y_minor += lookup_sine(direction) * 2;

    if (direction > 128) // down
        negative_vel_change(&ship->vel_y_major, vel_y_minor, old_vel_y_minor);
    else // up
        positive_vel_change(&ship->vel_y_major, vel_y_minor, old_vel_y_minor);
}

U0 dampen_velocity_axis(I8* vel_major, U8* vel_minor)
{
    U8 old_vel_minor;
    if (*vel_major < 0) // reduction of 2 to 128
    {
        old_vel_minor = *vel_minor;
        *vel_minor -= *vel_major * 2; // subtract the negative vel_major to increase value
        if (overflowed_U8(*vel_minor, old_vel_minor))
            (*vel_major)++;
    }
    else if (*vel_minor || *vel_major) // reduction of 1 to 127
    {
        old_vel_minor = *vel_minor;
        *vel_minor -= *vel_major * 2 + 1;
        if (underflowed_U8(*vel_minor, old_vel_minor))
            (*vel_major)--;
    }
}

U0 dampen_velocity(Space_Object* ship, U8* vel_x_minor, U8* vel_y_minor)
{
    dampen_velocity_axis(&ship->vel_x_major, vel_x_minor);
    dampen_velocity_axis(&ship->vel_y_major, vel_y_minor);
}

#endif
