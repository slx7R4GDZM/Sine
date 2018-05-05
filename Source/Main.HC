// Copyright slx7R4GDZM
// Distributed under the terms of the MIT License.
// Refer to the License.txt file for details.

#include "Game/Game.HC"

U0 main()
{
    SettingsPush();
    DocClear();

    // since settings aren't implemented just make the window cover the screen
    Fs->win_left   = 0x00;
    Fs->win_right  = 0x4F;
    Fs->win_top    = 0x00;
    Fs->win_bottom = 0x3B;

    init_game();
    SettingsPop();
    DCFill();
}

main();
