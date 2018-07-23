// Copyright slx7R4GDZM
// Distributed under the terms of the MIT License.
// Refer to the License.TXT file for details.

#ifndef SINE_SETTINGS_HANDLER_HC
#define SINE_SETTINGS_HANDLER_HC

#include "Other/Constants.HC"
#include "Other/Text.HC"

class Settings_Handler
{
    I64 button_scan_code[TOTAL_BUTTONS];
    U8 option_switch;

    // some of these can probably be made to be temporary
    // and done with after the window is created
    U8 starting_border;
    Vector2U16 start_resolution;
    Vector2I16 starting_win_pos;
    F64 crop_ratio;
    F64 gamma_correction;
    Bool frame_limiter_mode;
};

U0 set_position_and_resolution(Vector2U16 win_res, Vector2I16 win_pos, Bool border)
{
    Fs->win_left = win_pos.x / 8 + border;
    Fs->win_right = Fs->win_left + win_res.x / 8 - 1;

    Fs->win_top = win_pos.y / 8 + border;
    Fs->win_bottom = Fs->win_top + win_res.y / 8 - 1;

    TaskDerivedValsUpdate();
}

I16 get_start_position(I16 starting_position, Bool border, I16 lower_edge)
{
    if (starting_position != -1)
        return starting_position;
    else if (border)
        return (lower_edge - 1) * 8;
    else
        return lower_edge * 8;
}

U0 set_up_startup_window(Settings_Handler* settings)
{
    Bool border = !settings->starting_border;
    WinBorder(border);

    Vector2I16 pos;
    pos.x = get_start_position(settings->starting_win_pos.x, border, Fs->win_left);
    pos.y = get_start_position(settings->starting_win_pos.y, border, Fs->win_top);

    set_position_and_resolution(settings->start_resolution, pos, border);
}

F64 clamp_F64_string(CDoc* std_out, U8* setting, U8* value, F64 min_value, F64 max_value)
{
    F64 string_value = Clamp(Str2F64(value), min_value, max_value);
    if (string_value < min_value)
        DocPrint(std_out, "Value for setting %s is too low, limiting it to %n\n", setting, min_value);
    else if (string_value > max_value)
        DocPrint(std_out, "Value for setting %s is too high, limiting it to %n\n", setting, max_value);

    return string_value;
}

I64 clamp_I64_string(CDoc* std_out, U8* setting, U8* value, I64 min_value, I64 max_value)
{
    I64 string_value = ClampI64(Str2I64(value), min_value, max_value);
    if (string_value < min_value)
        DocPrint(std_out, "Value for setting %s is too low, limiting it to %d\n", setting, min_value);
    else if (string_value > max_value)
        DocPrint(std_out, "Value for setting %s is too high, limiting it to %d\n", setting, max_value);

    return string_value;
}

U0 replace_bits(U8* destination, U8 source, U8 mask, U8 offset)
{
    *destination = (*destination & ~(mask << offset)) | (source << offset);
}

I64 round_to_eights(I64 number)
{
    return Round(number / 8.0) * 8;
}

U0 parse_settings(Settings_Handler* settings, CDoc* std_out, U8* setting, U8* value)
{
    if (StrCmp(setting, "Language") == 0)
    {
        U8 language = clamp_I64_string(std_out, setting, value, ENGLISH, SPANISH);
        replace_bits(&settings->option_switch, language, SPANISH, 6);
    }
    else if (StrCmp(setting, "Starting-Lives") == 0)
    {
        U8 starting_lives = clamp_I64_string(std_out, setting, value, 0, 1);
        replace_bits(&settings->option_switch, starting_lives, 1, 5);
    }
    else if (StrCmp(setting, "Center-Coin-Multiplier") == 0)
    {
        U8 center_coin_multiplier = clamp_I64_string(std_out, setting, value, 0, 1);
        replace_bits(&settings->option_switch, center_coin_multiplier, 1, 4);
    }
    else if (StrCmp(setting, "Right-Coin-Multiplier") == 0)
    {
        U8 right_coin_multiplier = clamp_I64_string(std_out, setting, value, 0, 3);
        replace_bits(&settings->option_switch, right_coin_multiplier, 3, 2);
    }
    else if (StrCmp(setting, "Coinage") == 0)
    {
        U8 coinage = clamp_I64_string(std_out, setting, value, 0, 3);
        replace_bits(&settings->option_switch, coinage, 3, 0);
    }
    else if (StrCmp(setting, "Starting-Border") == 0)
        settings->starting_border = clamp_I64_string(std_out, setting, value, WIN_NORMAL, WIN_BORDERLESS);
    else if (StrCmp(setting, "Starting-X-Resolution") == 0)
        settings->start_resolution.x = round_to_eights(clamp_I64_string(std_out, setting, value, 8, 640));
    else if (StrCmp(setting, "Starting-Y-Resolution") == 0)
        settings->start_resolution.y = round_to_eights(clamp_I64_string(std_out, setting, value, 8, 480));
    else if (StrCmp(setting, "Start-Window-Position-X") == 0)
    {
        settings->starting_win_pos.x = clamp_I64_string(std_out, setting, value, -1, 632);
        if (settings->starting_win_pos.x != -1)
            settings->starting_win_pos.x = round_to_eights(settings->starting_win_pos.x);
    }
    else if (StrCmp(setting, "Start-Window-Position-Y") == 0)
    {
        settings->starting_win_pos.y = clamp_I64_string(std_out, setting, value, -1, 472);
        if (settings->starting_win_pos.y != -1)
            settings->starting_win_pos.y = round_to_eights(settings->starting_win_pos.y);
    }
    else if (StrCmp(setting, "Crop-Ratio") == 0)
        settings->crop_ratio = clamp_F64_string(std_out, setting, value, FLT_MIN, F64_MAX);
    else if (StrCmp(setting, "Gamma-Correction") == 0)
        settings->gamma_correction = clamp_F64_string(std_out, setting, value, FLT_MIN, F64_MAX);
    else if (StrCmp(setting, "Frame-Limit-Mode") == 0)
        settings->frame_limiter_mode = clamp_I64_string(std_out, setting, value, SLEEPING, BUSY_WAITING);
    else
        DocPrint(std_out, "Invalid setting \"%s\"\n", setting);
}

U0 parse_buttons(Settings_Handler* settings, CDoc* std_out, U8* setting, U8* value)
{
    // determine if the button choice exists
    U8 current_button = INVALID_BUTTON;
    U64 i;
    for (i = 0; i < TOTAL_BUTTONS && current_button == INVALID_BUTTON; i++)
    {
        if (StrCmp(setting, BUTTON_TABLE[i]) == 0)
            current_button = i;
    }

    if (current_button == INVALID_BUTTON)
        DocPrint(std_out, "Invalid button \"%s\"\n", setting);
    else
        settings->button_scan_code[current_button] = Str2I64(value);
}

U8* sub_str(U8* string, U64 position, U64 length)
{
    U8* output = MAlloc(length + 1);
    MemCpy(output, &string[position], length);
    output[length] = '\0';
    return output;
}

Bool is_graph(U8 character)
{
    return character > 32 && character != 128;
}

U8* get_next(U8* string, U64* position, U64 size)
{
    // move past the whitespace
    while (*position < size && !is_graph(string[*position]))
        (*position)++;

    // grab the usable text
    U8* output = NULL;
    U64 i;
    for (i = 0; *position + i < size; i++)
    {
        if (!is_graph(string[*position + i]))
        {
            output = sub_str(string, *position, i);
            break;
        }
    }
    *position += i + 1;
    return output;
}

U8* remove_cursor(U8* string, U64 position, U64 length)
{
    U64 i;
    for (i = 0; i <= length; i++)
    {
        if (string[position + i] == CH_CURSOR)
        {
            U8* fixed_string = MAlloc(length + 1);
            MemCpy(fixed_string, &string[position], i);
            MemCpy(&fixed_string[i], &string[position + i + 1], length - i);
            fixed_string[length] = '\0';
            return fixed_string;
        }
    }
    return sub_str(string, position, length);
}

U8* get_line(U8* string, U64 position, U64 length, U64* line_size)
{
    U8* line = NULL;
    U64 i;
    for (i = 0; position + i <= length && !line; i++)
    {
        if (string[position + i] == '\n' || position + i == length)
        {
            line = remove_cursor(string, position, i);
            *line_size = i + 1;
        }
    }
    return line;
}

U0 parse_file_settings(Settings_Handler* settings, CDoc* std_out, U8* in_file, U64 length)
{
    U64 position = 0;
    while (position < length)
    {
        U64 line_size = 0;
        U8* line = get_line(in_file, position, length, &line_size);
        position += line_size;

        U64 line_pos = 0;
        U8* setting = get_next(line, &line_pos, line_size);
        if (setting && setting[0] != 35) // line doesn't start with number sign
        {
            U8* value = get_next(line, &line_pos, line_size);
            if (value)
            {
                if (setting[0] == 'B')
                    parse_buttons(settings, std_out, setting, value);
                else
                    parse_settings(settings, std_out, setting, value);
            }
            Free(value);
        }
        Free(line);
        Free(setting);
    }
}

U0 init_settings_handler(Settings_Handler* settings, CDoc* std_out)
{
    // parse default settings before user settings in case any settings
    // in the users' file are missing or for when the file couldn't load
    parse_file_settings(settings, std_out, DEFAULT_SETTINGS, StrLen(DEFAULT_SETTINGS));

    CFile* in_file = FOpen(SETTINGS_FILENAME, "r");
    if (in_file)
    {
        U8* settings_file = FileRead(SETTINGS_FILENAME);
        parse_file_settings(settings, std_out, settings_file, in_file->de.size);
        Free(settings_file);
        FClose(in_file);
    }
    else if (!DrvIsWritable())
        DocPrint(std_out, "Current drive isn't writable, can't create %s\n", SETTINGS_FILENAME);
    else
    {
        DocPrint(std_out, "Error reading file %s\n", SETTINGS_FILENAME);
        CDoc* out_file = DocNew(SETTINGS_FILENAME);
        DocPrint(out_file, DEFAULT_SETTINGS);
        DocWrite(out_file);
        DocDel(out_file);
        DocPrint(std_out, "Created file %s\n", SETTINGS_FILENAME);
    }
}

U0 output_settings(Settings_Handler* settings, CDoc* std_out)
{
    DocPrint(std_out, "----------------------------------------");
    U64 i;
    for (i = 0; i < TOTAL_BUTTONS; i++)
        DocPrint(std_out, "\n%18s = %s", BUTTON_TABLE[i], ScanCode2KeyName(settings->button_scan_code[i]));

    DocPrint(std_out, "\n----------------------------------------");
    DocPrint(std_out, "\nLanguage = %u", settings->option_switch >> 6);
    DocPrint(std_out, "\nStarting-Lives = %u", (settings->option_switch >> 5) & 0x01);
    DocPrint(std_out, "\nCenter-Coin-Multiplier = %u", (settings->option_switch >> 4) & 0x01);
    DocPrint(std_out, "\nRight-Coin-Multiplier = %u", (settings->option_switch >> 2) & 0x03);
    DocPrint(std_out, "\nCoinage = %u", settings->option_switch & 0x03);

    DocPrint(std_out, "\n----------------------------------------");
    DocPrint(std_out, "\nStarting-Border = %u", settings->starting_border);
    DocPrint(std_out, "\nStarting-X-Resolution = %u", settings->start_resolution.x);
    DocPrint(std_out, "\nStarting-Y-Resolution = %u", settings->start_resolution.y);
    DocPrint(std_out, "\nStart-Window-Position-X = %d", settings->starting_win_pos.x);
    DocPrint(std_out, "\nStart-Window-Position-Y = %d", settings->starting_win_pos.y);

    DocPrint(std_out, "\n----------------------------------------");
    DocPrint(std_out, "\nCrop-Ratio = %n", settings->crop_ratio);
    DocPrint(std_out, "\nGamma-Correction = %n", settings->gamma_correction);
    DocPrint(std_out, "\nFrame-Limit-Mode = %u", settings->frame_limiter_mode);
    DocPrint(std_out, "\n----------------------------------------\n");
}

#endif
