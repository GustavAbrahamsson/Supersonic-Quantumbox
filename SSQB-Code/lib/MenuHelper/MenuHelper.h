#pragma once
#include "GenericEffect.h"
#include <Adafruit_SSD1306.h>
#include <Adafruit_GFX.h>
#include <Fonts/Picopixel.h>

// Pot bindings (effect, param)
typedef struct PotBinding
{
    GenericEffect *effect;
    uint32_t param;
} PotBinding;

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

// Menu system for the pedal
class MenuHelper
{
private:
    // State variables
    menuState CurrentView = MENU_EFFECT_VIEW;
    uint32_t CurrentEffectNum = 0;
    uint32_t CurrentParamNum = 0;

    // External variables
    GenericEffect * * effects;
    uint32_t numEffects;
    PotBinding * potBindings;
    Adafruit_SSD1306 * display;

    void GoToEffectView(uint32_t);

    void GoToParamView(uint32_t);

    void GoToParamEdit(uint32_t);

    void GoToDebugScreen();

    void ChangeParamValue(int32_t);

    void PotBind();

public:
    // Constructor
    // effects: array of effects
    // numEffects: number of effects
    // potBindings: array of pot bindings
    // display: pointer to display
    MenuHelper(GenericEffect * [], uint32_t numEffects, PotBinding[], Adafruit_SSD1306 *);

    // Handle encoder input
    // inputEvent: one of MENU_LEFT, MENU_RIGHT, MENU_PRESS, MENU_HOLD
    void HandleInput(inputEvent);

    // Update the display
    void UpdateDisplay();
};

/**
class ScreenState{
    public:
        virtual void Left() = 0;
        virtual void Right() = 0;
        virtual void Press() = 0;
        virtual void Hold() = 0;
        virtual void Draw(Adafruit_SSD1306 *) = 0;
        virtual void Enter() = 0;
};
**/

