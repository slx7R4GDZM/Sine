// Copyright slx7R4GDZM
// Distributed under the terms of the MIT License.
// Refer to the License.TXT file for details.

Silent(ON);
#include "Game/Game.HC"
Silent(OFF);

U0 main()
{
    SettingsPush();
    DocCursor(OFF);
    Bool keep_auto_complete_on = AutoComplete(OFF);
    StrCpy(Fs->task_title, "Sine");

    CDoc* print_on_exit = DocNew();
    DocInsDoc(print_on_exit, DocPut());
    DocClear();

    init_game(print_on_exit);

    DCFill();
    FlushMsgs();
    ms.show = TRUE;
    SettingsPop();
    AutoComplete(keep_auto_complete_on);

    DocInsDoc(DocPut(), print_on_exit);
    DocDel(print_on_exit);
}

main();
