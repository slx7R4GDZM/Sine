// Copyright slx7R4GDZM
// Distributed under the terms of the MIT License.
// Refer to the License.TXT file for details.

#ifndef SINE_VECTOR_GENERATOR_HC
#define SINE_VECTOR_GENERATOR_HC

#include "Other/Common-Things.HC"
#include "Settings/Settings-Handler.HC"

class Vector_Generator
{
    // vector generator variables
    I16 current_x;
    I16 current_y;
    U8 global_scale;

    // modern graphics variables
    F64 res_scale;
    F64 x_offset;
    F64 y_offset;

    Vector2U16 resolution;
    F64 crop_ratio;
};

#define INTERNAL_RES 1024

U0 crop_with_extra_space(Vector_Generator* vector_generator, U32 axis_to_crop, F64* crop_offset, U32 scale_axis, F64* scale_offset)
{
    vector_generator->res_scale = ToF64(INTERNAL_RES) / scale_axis;
    *crop_offset = ToI64(axis_to_crop - scale_axis) * vector_generator->res_scale / 2;
    *scale_offset = 0;
}

U0 set_resolution_scale(Vector_Generator* vg)
{
    vg->resolution.x = (Fs->win_right - Fs->win_left + 1) * FONT_WIDTH;
    vg->resolution.y = (Fs->win_bottom - Fs->win_top + 1) * FONT_HEIGHT;
    if (vg->crop_ratio < 1.0)
    {
        if (vg->resolution.x > vg->resolution.y * vg->crop_ratio)
            crop_with_extra_space(vg, vg->resolution.x, &vg->x_offset, vg->resolution.y, &vg->y_offset);
        else
        {
            vg->res_scale = INTERNAL_RES / (vg->resolution.x / vg->crop_ratio);
            vg->x_offset = (vg->resolution.x - vg->resolution.x / vg->crop_ratio) * vg->res_scale / 2;
            vg->y_offset = (vg->resolution.y - vg->resolution.x / vg->crop_ratio) * vg->res_scale / 2;
        }
    }
    else
    {
        if (vg->resolution.x < vg->resolution.y * vg->crop_ratio)
            crop_with_extra_space(vg, vg->resolution.y, &vg->y_offset, vg->resolution.x, &vg->x_offset);
        else
        {
            vg->res_scale = INTERNAL_RES / (vg->resolution.y * vg->crop_ratio);
            vg->x_offset = (vg->resolution.x - vg->resolution.y * vg->crop_ratio) * vg->res_scale / 2;
            vg->y_offset = (vg->resolution.y - vg->resolution.y * vg->crop_ratio) * vg->res_scale / 2;
        }
    }
}

U0 init_vector_generator(Vector_Generator* vector_generator, Settings_Handler* settings)
{
    vector_generator->crop_ratio = settings->crop_ratio;
    set_resolution_scale(vector_generator);

    U64 i;
    for (i = 0; i < 16; i++)
    {
        U16 color = Round(Pow((15 - i) * 17 / 255.0, 1 / settings->gamma_correction) * 65535);
        CBGR48 palette_color;
        palette_color.r = color;
        palette_color.g = color;
        palette_color.b = color;
        GrPaletteColorSet(i, palette_color);
    }
}

// drawing stuff

U0 draw_line_segment(Vector_Generator* vector_generator, F64 start_x, F64 start_y, I16 delta_x, I16 delta_y, CDC* dc)
{
    if (delta_x || delta_y)
    {
        F64 end_x = start_x + delta_x / vector_generator->res_scale;
        F64 end_y = start_y - delta_y / vector_generator->res_scale;
        GrLine(dc, start_x, start_y, end_x, end_y);
    }
    else
        GrPlot(dc, start_x, start_y);
}

U0 draw_vector(Vector_Generator* vector_generator, I16 raw_delta_x, I16 raw_delta_y, U8 brightness, Bool flip_x, Bool flip_y, CDC* dc)
{
    I16 delta_x = raw_delta_x;
    if (flip_x)
        delta_x = -raw_delta_x;

    I16 delta_y = raw_delta_y;
    if (flip_y)
        delta_y = -raw_delta_y;

    if (brightness)
    {
        F64 adjusted_x = vector_generator->current_x + vector_generator->x_offset;
        F64 adjusted_y = vector_generator->current_y + vector_generator->y_offset;

        F64 scaled_x_start =                                  adjusted_x / vector_generator->res_scale;
        F64 scaled_y_start = vector_generator->resolution.y - adjusted_y / vector_generator->res_scale;
        dc->color = 15 - brightness;
        draw_line_segment(vector_generator, scaled_x_start, scaled_y_start, delta_x, delta_y, dc);
    }
    vector_generator->current_x += delta_x;
    vector_generator->current_y += delta_y;
}

I16 apply_global_scale(Vector_Generator* vector_generator, I16 delta)
{
    if (vector_generator->global_scale <= MUL_128)
        return delta << vector_generator->global_scale;

    return delta >> (16 - vector_generator->global_scale);
}

// opcodes

U0 draw_long_vector(Vector_Generator* vector_generator, U8 opcode, U16* vector_object, U16* iteration, Bool flip_x, Bool flip_y, CDC* dc)
{
    I16 delta_y = vector_object[*iteration]     & 0x03FF;
    I16 delta_x = vector_object[*iteration + 1] & 0x03FF;

    delta_y = apply_global_scale(vector_generator, delta_y >> (9 - opcode));
    delta_x = apply_global_scale(vector_generator, delta_x >> (9 - opcode));

    if (vector_object[(*iteration)++] & 0x0400)
        delta_y = -delta_y;
    if (vector_object[*iteration]     & 0x0400)
        delta_x = -delta_x;

    U8 brightness = vector_object[*iteration] >> 12;

    draw_vector(vector_generator, delta_x, delta_y, brightness, flip_x, flip_y, dc);
}

U0 load_absolute(Vector_Generator* vector_generator, U16* vector_object, U16* iteration)
{
    vector_generator->current_y = vector_object[*iteration] & 0x03FF;
    if (vector_object[(*iteration)++] & 0x0400)
        vector_generator->current_y = -vector_generator->current_y;

    vector_generator->current_x = vector_object[*iteration] & 0x03FF;
    if (vector_object[*iteration]     & 0x0400)
        vector_generator->current_x = -vector_generator->current_x;

    vector_generator->global_scale = vector_object[*iteration] >> 12;
}

U0 draw_short_vector(Vector_Generator* vector_generator, U16* vector_object, U16* iteration, Bool flip_x, Bool flip_y, Bool brighten, CDC* dc)
{
    I16 delta_x = (vector_object[*iteration] & 0x0003) << 8;
    I16 delta_y = vector_object[*iteration] & 0x0300;

    U8 local_scale = (vector_object[*iteration] & 0x0008) >>  2
                   | (vector_object[*iteration] & 0x0800) >> 11;

    delta_x = apply_global_scale(vector_generator, delta_x >> (7 - local_scale));
    delta_y = apply_global_scale(vector_generator, delta_y >> (7 - local_scale));

    if (vector_object[*iteration] & 0x0004)
        delta_x = -delta_x;
    if (vector_object[*iteration] & 0x0400)
        delta_y = -delta_y;

    U8 brightness = (vector_object[*iteration] & 0x00F0) >> 4;
    if (brighten)
        brightness += 2;

    draw_vector(vector_generator, delta_x, delta_y, brightness, flip_x, flip_y, dc);
}

U0 process(Vector_Generator* vector_generator, U16* vector_object, CDC* dc, U16 iteration = 0, Bool flip_x = FALSE, Bool flip_y = FALSE, Bool brighten = FALSE)
{
    Bool done = FALSE;
    while (!done)
    {
        U8 opcode = vector_object[iteration] >> 12;
        switch (opcode)
        {
        case VCTR_0 ... VCTR_9:
            draw_long_vector(vector_generator, opcode, vector_object, &iteration, flip_x, flip_y, dc);
            break;
        case LABS:
            load_absolute(vector_generator, vector_object, &iteration);
            break;
        case RTSL:
            done = TRUE;
            break;
        case SVEC:
            draw_short_vector(vector_generator, vector_object, &iteration, flip_x, flip_y, brighten, dc);
            break;
        default:
            Print("Process: invalid opcode \"%u\"\n", opcode);
            done = TRUE;
            break;
        }
        iteration++;
    }
}

#endif
