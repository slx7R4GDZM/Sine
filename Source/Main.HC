// Copyright slx7R4GDZM
// Distributed under the terms of the MIT License.
// Refer to the License.TXT file for details.

#include "Game/Game.HC"

U0 main()
{
    SettingsPush();
    DocClear();
    DocCursor(OFF);
    Bool keep_auto_complete_on = AutoComplete(OFF);
    StrCpy(Fs->task_title, "Sine");
    CDoc* print_later = DocNew();

    init_game(print_later);

    DCFill();
    FlushMsgs();
    SettingsPop();
    AutoComplete(keep_auto_complete_on);
    DocInsDoc(DocPut(), print_later);
    DocDel(print_later);
}

main();
