#pragma once
#include "GenericEffect.h"
#include <Adafruit_SSD1306.h>
#include <Adafruit_GFX.h>
#include <Fonts/Picopixel.h>


enum inputEvent
{
    MENU_LEFT,
    MENU_RIGHT,
    MENU_PRESS,
    MENU_HOLD
};

enum menuState
{
    MENU_EFFECT_VIEW,
    MENU_PARAM_VIEW,
    MENU_PARAM_EDIT,
    MENU_DEBUG_SCREEN,
    MENU_STAY
};

typedef struct pedalContext 
{
    // External variables
    GenericEffect **effects;
    uint32_t numEffects;
    Adafruit_SSD1306 *display;
    uint32_t CurrentEffectNum;
    uint32_t CurrentParamNum;
} PedalContext;

// State machine base class
class ScreenState
{
public:
    virtual void Draw(PedalContext *) = 0;
    virtual void Enter(PedalContext *) = 0;
    virtual menuState HandleInput(inputEvent, PedalContext *) = 0;
};

// Effect view state
class EffectViewState : public ScreenState
{
public:
    menuState HandleInput(inputEvent, PedalContext *);
    void Draw(PedalContext *);
    void Enter(PedalContext *);
};

class ParamViewState : public ScreenState
{
public:
    void Enter(PedalContext *);
    void Draw(PedalContext *);
    menuState HandleInput(inputEvent, PedalContext *);
};

class DebugScreenState : public ScreenState
{
public:
    void Enter(PedalContext *);
    void Draw(PedalContext *);
    menuState HandleInput(inputEvent, PedalContext *);
};

// Menu system for the pedal
class MenuHelper
{
private:
    // State variables
    ScreenState *CurrentState;

    EffectViewState effectViewState;
    ParamViewState paramViewState;
    DebugScreenState debugScreenState;

    // External variables
    PedalContext *ctx;

public:
    // Constructor
    // effects: array of effects
    // numEffects: number of effects
    // potBindings: array of pot bindings
    // display: pointer to display
    MenuHelper(PedalContext *);

    // Handle encoder input
    // inputEvent: one of MENU_LEFT, MENU_RIGHT, MENU_PRESS, MENU_HOLD
    void HandleInput(inputEvent);

    // Update the display
    void UpdateDisplay();

    // Transition state
    void Transition(menuState);
};
