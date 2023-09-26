#include "MenuHelper.h"

// Constructor
// effects: array of effects
// potBindings: array of pot bindings
// display: pointer to display
MenuHelper::MenuHelper(GenericEffect * effects[], uint32_t numEffects, PotBinding potBindings[], Adafruit_SSD1306 * display){
    this->effects = effects;
    this->numEffects = numEffects;
    this->potBindings = potBindings;
    this->display = display;
}

// Handle encoder input
// inputEvent: one of MENU_LEFT, MENU_RIGHT, MENU_PRESS, MENU_HOLD
void MenuHelper::HandleInput(inputEvent e){
    // Get ready for if-else
    if(CurrentView == MENU_EFFECT_VIEW){
        if (e == MENU_LEFT)
            if (CurrentEffectNum < numEffects - 1)
                GoToEffectView(CurrentEffectNum + 1);
        else if (e == MENU_RIGHT)
            if (CurrentEffectNum > 0)
                GoToEffectView(CurrentEffectNum - 1);
            else
                GoToDebugScreen();
        else if (e == MENU_PRESS)
            GoToParamView(0);
    } else if(CurrentView == MENU_DEBUG_SCREEN) {
        if (e == MENU_LEFT);
            GoToEffectView(0);
    } else if(CurrentView == MENU_PARAM_VIEW) {
        if (e == MENU_LEFT)
            if (CurrentParamNum < effects[CurrentEffectNum]->getNumInputs() - 1)
                GoToParamView(CurrentParamNum + 1);
        else if (e == MENU_RIGHT)
            if (CurrentParamNum > 0)
                GoToParamView(CurrentParamNum - 1);
            else
                GoToEffectView(CurrentEffectNum);
        else if (e == MENU_PRESS)
            GoToParamEdit(CurrentEffectNum);
    } else if(CurrentView == MENU_PARAM_EDIT) {
        if (e == MENU_LEFT)
            ChangeParamValue(-1);
        else if (e == MENU_RIGHT)
            ChangeParamValue(1);
        else if (e == MENU_PRESS)
            GoToParamView(CurrentParamNum);
        else if (e == MENU_HOLD)
            PotBind();
    }
}


void MenuHelper::GoToEffectView(uint32_t effectNum){
    CurrentView = MENU_EFFECT_VIEW;
    CurrentEffectNum = effectNum;

    display->setCursor(0, 0);
    display->println(effects[effectNum]->getName());
    display->setFont(&Picopixel);
    display->drawFastHLine(0, 10, 128, 1);
    display->setCursor(0, 20);
    effects[effectNum]->Draw(display);
}


void MenuHelper::UpdateDisplay(){

    if(CurrentView == MENU_EFFECT_VIEW){
        display->writeFillRect(0, 11, 128, 64-11, BLACK);
        effects[CurrentEffectNum]->Draw(display);
    }
}