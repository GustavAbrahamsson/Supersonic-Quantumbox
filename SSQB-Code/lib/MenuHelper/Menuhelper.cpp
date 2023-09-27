#include "MenuHelper.h"

// Constructor
// effects: array of effects
// potBindings: array of pot bindings
// display: pointer to display
MenuHelper::MenuHelper(GenericEffect * effects[], uint32_t numEffects, PotStruct pots[], Adafruit_SSD1306 * display){
    this->effects = effects;
    this->numEffects = numEffects;
    this->pots = pots;
    this->display = display;

    // Create states
    /*
    EffectViewState = &new EffectViewState();
    ParamViewState = &new ParamViewState();
    ParamEditState = &new ParamEditState();
    DebugScreenState = &(new DebugScreenState();
    */


}

// Handle encoder input
// inputEvent: one of MENU_LEFT, MENU_RIGHT, MENU_PRESS, MENU_HOLD
void MenuHelper::HandleInput(inputEvent e){
    switch (e)
    {
    case MENU_LEFT:
        CurrentState->Left(this);
        break;
    case MENU_RIGHT:
        CurrentState->Right(this);
        break;
    case MENU_PRESS:
        CurrentState->Press(this);
        break;
    case MENU_HOLD:
        CurrentState->Hold(this);
        break;
    default:
        break;
    }
}

// Effect view state

void EffectViewState::Enter(MenuHelper * menuHelper){
    menuHelper->CurrentState = &menuHelper->effectViewState;
    Draw(menuHelper);
}

void EffectViewState::Left(MenuHelper * menuHelper){
    if (CurrentEffectNum + 1 < menuHelper->numEffects)
        CurrentEffectNum++;
}

void EffectViewState::Right(MenuHelper * menuHelper){
    if (CurrentEffectNum > 0)
        CurrentEffectNum--;
    else
        menuHelper->debugScreenState.Enter(menuHelper);
}

void EffectViewState::Press(MenuHelper * menuHelper){
    menuHelper->paramViewState.Enter(menuHelper);
}

void EffectViewState::Hold(MenuHelper * menuHelper){
    //menuHelper->effects[CurrentEffectNum]->Mute();
}

void EffectViewState::Draw(MenuHelper * menuHelper){
    Adafruit_SSD1306 * d = menuHelper->display;
    d->setCursor(0, 0);
    d->clearDisplay();
    d->println(menuHelper->effects[CurrentEffectNum]->getName());
    d->setFont(&Picopixel);
    d->drawFastHLine(0, 10, 128, 1);
    d->setCursor(0, 20);
    menuHelper->effects[CurrentEffectNum]->Draw(menuHelper->display);
    d->display();
}

// Debug View state
void DebugScreenState::Enter(MenuHelper *menuHelper)
{
    menuHelper->CurrentState = &menuHelper->debugScreenState;
    Draw(menuHelper);
}

void DebugScreenState::Draw(MenuHelper * MenuHelper){
    // diagnostics display
    Adafruit_SSD1306 * d = MenuHelper->display;

    d->setCursor(0, 0);
    d->println("Diagnostics");
    d->drawFastHLine(0, 10, 128, 1);
    d->setFont(&Picopixel);
    d->setCursor(0, 20);
    //d->println("Encoder: " + String(encoderCount));
    //d->println("Button: " + String(encoderButton));
    //d->println("Pots: " + String(pots[0]) + ", " + String(pots[1]) + ", " + String(pots[2]));
    //d->println("DSP %: " + String(avgDspTime * 100));
    //d->println("PSRAM used: " + String(maxRam - ESP.getFreePsram()) + " / " + String(maxRam));
}

void DebugScreenState::Left(MenuHelper * menuHelper){
    menuHelper->effectViewState.Enter(menuHelper);
}


// Param view state
void ParamViewState::Enter(MenuHelper * menuHelper){
    menuHelper->CurrentState = &menuHelper->paramViewState;
    menuHelper->CurrentParamNum = 0;
}

void ParamViewState::Press(MenuHelper * menuHelper){
    menuHelper->effectViewState.Enter(menuHelper);
}




/**
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

**/