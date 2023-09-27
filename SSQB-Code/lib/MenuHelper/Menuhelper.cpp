#include "MenuHelper.h"

// Constructor
// effects: array of effects
// potBindings: array of pot bindings
// display: pointer to display
MenuHelper::MenuHelper(PedalContext * ctx){
    this->ctx = ctx;
}

// Handle encoder input
// inputEvent: one of MENU_LEFT, MENU_RIGHT, MENU_PRESS, MENU_HOLD
void MenuHelper::HandleInput(inputEvent e){
    menuState newState = CurrentState->HandleInput(e, ctx);
    
    switch (newState)
    {
    case MENU_EFFECT_VIEW:
        CurrentState = &effectViewState;
        break;
    case MENU_PARAM_VIEW:
        CurrentState = &paramViewState;
    case MENU_DEBUG_SCREEN:
        CurrentState = &debugScreenState;
    // case MENU_PARAM_EDIT:
    //     CurrentState = &paramEditState;
    case MENU_STAY:
        return;
    default:
        break;
    }

    CurrentState->Enter(ctx);
}

void MenuHelper::UpdateDisplay(){
    CurrentState->Draw(ctx);
}

// Effect view state

void EffectViewState::Enter(PedalContext * ctx){
    Draw(ctx);
}

menuState EffectViewState::HandleInput(inputEvent e, PedalContext * ctx){
    switch (e)
    {
    case MENU_LEFT:
        if (ctx->CurrentEffectNum + 1 < ctx->numEffects)
            ctx->CurrentEffectNum++;
        break;
    case MENU_RIGHT:
        if (ctx->CurrentEffectNum > 0)
            ctx->CurrentEffectNum--;
        else
            return MENU_DEBUG_SCREEN;
        break;
    case MENU_PRESS:
        return MENU_PARAM_VIEW;
    case MENU_HOLD:
        // ctx->effects[CurrentEffectNum]->Mute();
        break;
    default:
        break;
    }

    return MENU_STAY;
}

void EffectViewState::Draw(PedalContext * ctx){
    Adafruit_SSD1306 * d = ctx->display;
    d->setCursor(0, 0);
    d->clearDisplay();
    d->println(ctx->effects[ctx->CurrentEffectNum]->getName());
    d->setFont(&Picopixel);
    d->drawFastHLine(0, 10, 128, 1);
    d->setCursor(0, 20);
    ctx->effects[ctx->CurrentEffectNum]->Draw(ctx->display);
    d->display();
}

// Debug View state
void DebugScreenState::Enter(PedalContext *ctx)
{
    Draw(ctx);
}

void DebugScreenState::Draw(PedalContext * ctx){
    // diagnostics display
    Adafruit_SSD1306 * d = ctx->display;

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

menuState DebugScreenState::HandleInput(inputEvent e, PedalContext * ctx){
    switch (e)
    {
    case MENU_LEFT:
        return MENU_EFFECT_VIEW;
    default:
        break;
    }
    return MENU_STAY;
}


// Param view state
void ParamViewState::Enter(PedalContext * ctx){
    ctx->CurrentParamNum = 0;
    Draw(ctx);
}

menuState ParamViewState::HandleInput(inputEvent e, PedalContext * ctx){
    switch (e)
    {
    case MENU_PRESS:
        return MENU_EFFECT_VIEW;
    default:
        break;
    }

    return MENU_STAY;
}

void ParamViewState::Draw(PedalContext * ctx){
    Adafruit_SSD1306 * d = ctx->display;
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