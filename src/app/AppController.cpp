#include "AppController.h"

#include <iostream>

void AppController::OpenFileButton()
{
    SetStatusText("OpenFile Pressed");
}

void AppController::SaveButton()
{
    SetStatusText("Save Pressed");
}

void AppController::UndoButton()
{
    SetStatusText("Undo Pressed");
}

void AppController::RedoButton()
{
    SetStatusText("Redo Pressed");
}
