#pragma once
#include "GenericEffect.h"
#include "PotStruct.h"
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
    MENU_DEBUG_SCREEN
};

typedef struct pedalContext 
{
    // External variables
    GenericEffect **effects;
    uint32_t numEffects;
    PotStruct *pots;
    Adafruit_SSD1306 *display;
} PedalContext;

// State machine base class
class ScreenState
{
public:
    virtual void Draw(MenuHelper *) = 0;
    virtual void Enter(MenuHelper *) = 0;
    void Left(MenuHelper *);
    void Right(MenuHelper *);
    void Press(MenuHelper *);
    void Hold(MenuHelper *);
};

// Effect view state
class EffectViewState : public ScreenState
{
private:
    uint32_t CurrentEffectNum;

public:
    void Left(MenuHelper *);
    void Right(MenuHelper *);
    void Press(MenuHelper *);
    void Hold(MenuHelper *);
    void Draw(MenuHelper *);
    void Enter(MenuHelper *);
};

class ParamViewState : public ScreenState
{
public:
    void Enter(MenuHelper *);
    void Draw(MenuHelper *);
    void Press(MenuHelper *);
};

class ParamEditState : public ScreenState
{
};

class DebugScreenState : public ScreenState
{
public:
    void Enter(MenuHelper *);
    void Draw(MenuHelper *);
    void Left(MenuHelper *);
};

// Menu system for the pedal
class MenuHelper
{
private:
    friend class ScreenState;
    friend class EffectViewState;
    friend class ParamViewState;
    friend class ParamEditState;
    friend class DebugScreenState;

    // State variables
    ScreenState *CurrentState;
    uint32_t CurrentEffectNum = 0;
    uint32_t CurrentParamNum = 0;

    EffectViewState effectViewState;
    ParamViewState paramViewState;
    // ParamEditState paramEditState;
    DebugScreenState debugScreenState;

    // External variables
    PedalContext *ctx;

public:
    // Constructor
    // effects: array of effects
    // numEffects: number of effects
    // potBindings: array of pot bindings
    // display: pointer to display
    MenuHelper(GenericEffect *[], uint32_t numEffects, PotStruct[], Adafruit_SSD1306 *);

    // Handle encoder input
    // inputEvent: one of MENU_LEFT, MENU_RIGHT, MENU_PRESS, MENU_HOLD
    void HandleInput(inputEvent);

    // Update the display
    void UpdateDisplay();
};
