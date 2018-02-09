// Copyright slx7R4GDZM
// Distributed under the terms of the MIT License.
// Refer to the License.txt file for details.

#ifndef SINE_SPACE_OBJECT_HC
#define SINE_SPACE_OBJECT_HC

#include "Graphics/Vector-Generator.HC"
#include "Other/Constants.HC"
#include "Other/Vectors.HC"

class Position
{
    U8 x_major;
    U8 y_major;
    U8 x_minor;
    U8 y_minor;
};

class Space_Object
{
    U8 status;
    I8 vel_x_major;
    I8 vel_y_major;
    Position pos;
};

class Coordinate
{
    U16 position_x;
    U16 position_y;
};

U0 get_total_pos(Coordinate* total_pos, Position* pos)
{
    total_pos->position_x = pos->x_major * 256 + pos->x_minor;
    total_pos->position_y = pos->y_major * 256 + pos->y_minor;
}

I8 lookup_sine(U8 direction)
{
    if (direction <= 64)
        return SINE_TABLE[direction];
    if (direction < 128)
        return SINE_TABLE[128 - direction];
    if (direction <= 192)
        return -SINE_TABLE[direction - 128];
    else
        return -SINE_TABLE[256 - direction];
}

I8 lookup_cosine(U8 direction)
{
    return lookup_sine(direction + 64);
}

U0 update_position_axis(U8* pos_major, U8* pos_minor, I8 vel_major)
{
    U8 old_pos_minor = *pos_minor;
    *pos_minor += vel_major;

    if (vel_major < 0) // down or left
    {
        if (underflowed_U8(*pos_minor, old_pos_minor))
            (*pos_major)--;
    }
    else if (vel_major > 0) // up or right
    {
        if (overflowed_U8(*pos_minor, old_pos_minor))
            (*pos_major)++;
    }
}

U0 wrap_position(U8* pos_major, U8 max_pos_major)
{
    if (*pos_major == 255) // underflowed
        *pos_major = max_pos_major;
    else if (*pos_major > max_pos_major)
        *pos_major = 0;
}

U0 update_position(Space_Object* so)
{
    update_position_axis(&so->pos.x_major, &so->pos.x_minor, so->vel_x_major);
    update_position_axis(&so->pos.y_major, &so->pos.y_minor, so->vel_y_major);
    wrap_position(&so->pos.x_major, MAX_X_POS_MAJOR);
    wrap_position(&so->pos.y_major, MAX_Y_POS_MAJOR);
}

U8 limit_position(U8 pos_major, U8 max_pos_major)
{
    if (pos_major < 3)
        return 3;
    else if (pos_major > max_pos_major)
        return max_pos_major;
    else
        return pos_major;
}

U0 hit(U16 pos_1, U16 pos_2, U8 minimum_space, Bool* was_hit)
{
    if (pos_1 < pos_2)
    {
        if (pos_2 - pos_1 < minimum_space * 2)
            *was_hit = TRUE;
    }
    else
    {
        if (pos_1 - pos_2 <= minimum_space * 2)
            *was_hit = TRUE;
    }
}

Bool collide(Space_Object* o1, Space_Object* o2, U8 minimum_space)
{
    if (o1->status && o1->status < TRUE_EXPLOSION_START
     && o2->status && o2->status < TRUE_EXPLOSION_START)
    {
        Coordinate pos_1;
        Coordinate pos_2;
        get_total_pos(&pos_1, &o1->pos);
        get_total_pos(&pos_2, &o2->pos);
        Bool hit_x = FALSE;
        hit(pos_1.position_x, pos_2.position_x, minimum_space, &hit_x);
        if (hit_x)
        {
            Bool hit_y = FALSE;
            hit(pos_1.position_y, pos_2.position_y, minimum_space, &hit_y);
            if (hit_y)
                return TRUE;
        }
    }
    return FALSE;
}

U0 set_position_and_size(Space_Object* space_object, U8 scale, Vector_Generator* vector_generator, CDC* dc)
{
    // add 128 to y making the 4:3 Space_Object space centered inside the 1:1 DVG space
    U16 vector_object[3];
    vector_object[0] = ((LABS << 12) | (space_object->pos.y_major << 5) | (space_object->pos.y_minor >> 3)) + 128;
    vector_object[1] = (scale << 12) | (space_object->pos.x_major << 5) | (space_object->pos.x_minor >> 3);
    vector_object[2] =  (RTSL << 12);
    process(vector_generator, vector_object, dc);
}

U0 draw_explosion(Space_Object* space_object, Vector_Generator* vector_generator, CDC* dc)
{
    U8 scale = (space_object->status + 33) / 17;
    set_position_and_size(space_object, scale, vector_generator, dc);

    switch (space_object->status / 4 % 4)
    {
    case 0:
        process(vector_generator, EXPLOSION_4, dc);
        break;
    case 1:
        process(vector_generator, EXPLOSION_3, dc);
        break;
    case 2:
        process(vector_generator, EXPLOSION_2, dc);
        break;
    case 3:
        process(vector_generator, EXPLOSION_1, dc);
        break;
    }
}

#endif
