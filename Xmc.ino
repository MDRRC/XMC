/***********************************************************************************************************************
   @file     Xmc.ino
   @ingroup  Application
   @brief    Main application of the XMC (XPressNet Manual Control)
 **********************************************************************************************************************/

/***********************************************************************************************************************
   I N C L U D E S
 **********************************************************************************************************************/
#include "Bounce2.h"
#include "fsmlist.hpp"
#include "xmc_app.h"
#include "xmc_event.h"
#include <Arduino.h>

/***********************************************************************************************************************
   E X P O R T E D   S Y M B O L   D E F I N I T I O N S (defines, typedefs)
 **********************************************************************************************************************/
#define XMC_3SEC_TIMEOUT 3000
#define XMC_ENCODER_PIN_A PA2
#define XMC_ENCODER_PIN_B PA3
#define XMC_ENCODER_PIN_PB PA1

#define XMC_SWITCH_POWER PC13
#define XMC_SWITCH_F0 PC14
#define XMC_SWITCH_F1 PC15
#define XMC_SWITCH_F2 PA0
#define XMC_SWITCH_F3 PA5
#define XMC_SWITCH_F4 PA6
#define XMC_SWITCH_F5 PA7

/***********************************************************************************************************************
   D A T A   D E C L A R A T I O N S (exported, local)
 **********************************************************************************************************************/

/**
 * Variables.
 */

Bounce xmcPulseSwitchPushButton = Bounce();
Bounce xmcPulseSwitchPower      = Bounce();
Bounce xmcPulseSwitch0          = Bounce();
Bounce xmcPulseSwitch1          = Bounce();
Bounce xmcPulseSwitch2          = Bounce();
Bounce xmcPulseSwitch3          = Bounce();
Bounce xmcPulseSwitch4          = Bounce();
Bounce xmcPulseSwitch5          = Bounce();

updateEvent3sec xmcUpdateEvent3Sec;
xpNetEventUpdate xmcXpNetUpdate;
pushButtonsEvent xmcPushButtonEvent;
pulseSwitchEvent wmcPulseSwitchEvent;
updateEvent100msec xmcUpdateEvent100msec;
updateEvent500msec xmcUpdateEvent500msec;

unsigned long xmcTimer100msec;
unsigned long xmcTimer500msec;
unsigned long xmcTimerMsec3Sec;
unsigned long xmcTimerPulseSwitch;
unsigned long xmcTimerPulseSwitchPushButton;

bool turnedWhilePressed           = false;
unsigned int encoder0PosActual    = 0;
volatile unsigned int encoder0Pos = 0;
uint8_t Runled                    = 0;

/***********************************************************************************************************************
   L O C A L   F U N C T I O N S
 **********************************************************************************************************************/

/***********************************************************************************************************************
 */
static void XmcUpdate3Sec(void)
{
    if (millis() - xmcTimerMsec3Sec > XMC_3SEC_TIMEOUT)
    {
        xmcTimerMsec3Sec = millis();
        send_event(xmcUpdateEvent3Sec);
    }
}

/***********************************************************************************************************************
 */
static void xmcUpdate100msec(void)
{
    if (millis() - xmcTimer100msec >= 100)
    {
        xmcTimer100msec = millis();
        send_event(xmcUpdateEvent100msec);
    }
}

/***********************************************************************************************************************
 */
static void xmcUpdate500msec(void)
{
    if (millis() - xmcTimer500msec >= 500)
    {
        xmcTimer500msec = millis();
        send_event(xmcUpdateEvent500msec);

        if (Runled != 0)
        {
            digitalWrite(PB1, LOW);
            Runled = 0;
        }
        else
        {
            digitalWrite(PB1, HIGH);
            Runled = 1;
        }
    }
}

/***********************************************************************************************************************
 */
void doEncoderA()
{
    // look for a low-to-high on channel A
    if (digitalRead(XMC_ENCODER_PIN_A) == HIGH)
    {
        // check channel B to see which way encoder is turning
        if (digitalRead(XMC_ENCODER_PIN_B) == LOW)
        {
            // CW
            encoder0Pos = encoder0Pos + 1;
        }
        else
        {
            // CCW
            encoder0Pos = encoder0Pos - 1;
        }
    }
    else // must be a high-to-low edge on channel A
    {
        // check channel B to see which way encoder is turning
        if (digitalRead(XMC_ENCODER_PIN_B) == HIGH)
        {
            // CW
            encoder0Pos = encoder0Pos + 1;
        }
        else
        {
            // CCW
            encoder0Pos = encoder0Pos - 1;
        }
    }
}

/***********************************************************************************************************************
 */
int8_t DecoderUpdate(void)
{
    int8_t Delta = 0;
    if (encoder0Pos != encoder0PosActual)
    {
        Delta             = encoder0Pos - encoder0PosActual;
        encoder0PosActual = encoder0Pos;
    }

    return (Delta);
}

/***********************************************************************************************************************
   E X P O R T E D   F U N C T I O N S
 **********************************************************************************************************************/

/***********************************************************************************************************************
 @brief
 @param
 @return     None
**********************************************************************************************************************/
void setup()
{
    Serial.begin(115200);

    /* Init the pulse / rotary encoder. */
    pinMode(XMC_ENCODER_PIN_A, INPUT_PULLUP);
    pinMode(XMC_ENCODER_PIN_B, INPUT_PULLUP);
    attachInterrupt(digitalPinToInterrupt(XMC_ENCODER_PIN_A), doEncoderA, FALLING);

    /* Init debounces for switches. */
    pinMode(XMC_ENCODER_PIN_PB, INPUT_PULLUP);
    xmcPulseSwitchPushButton.interval(100);
    xmcPulseSwitchPushButton.attach(XMC_ENCODER_PIN_PB);

    pinMode(XMC_SWITCH_POWER, INPUT_PULLUP);
    xmcPulseSwitchPower.interval(100);
    xmcPulseSwitchPower.attach(XMC_SWITCH_POWER);

    pinMode(XMC_SWITCH_F0, INPUT_PULLUP);
    xmcPulseSwitch0.interval(100);
    xmcPulseSwitch0.attach(XMC_SWITCH_F0);

    pinMode(XMC_SWITCH_F1, INPUT_PULLUP);
    xmcPulseSwitch1.interval(100);
    xmcPulseSwitch1.attach(XMC_SWITCH_F1);

    pinMode(XMC_SWITCH_F2, INPUT_PULLUP);
    xmcPulseSwitch2.interval(100);
    xmcPulseSwitch2.attach(XMC_SWITCH_F2);

    pinMode(XMC_SWITCH_F3, INPUT_PULLUP);
    xmcPulseSwitch3.interval(100);
    xmcPulseSwitch3.attach(XMC_SWITCH_F3);

    pinMode(XMC_SWITCH_F4, INPUT_PULLUP);
    xmcPulseSwitch4.interval(100);
    xmcPulseSwitch4.attach(XMC_SWITCH_F4);

    pinMode(XMC_SWITCH_F5, INPUT_PULLUP);
    xmcPulseSwitch5.interval(100);
    xmcPulseSwitch5.attach(XMC_SWITCH_F5);

    /* Run led. */
    pinMode(PB1, OUTPUT);

    /* Init timers. */
    xmcTimer500msec               = millis();
    xmcTimerMsec3Sec              = millis();
    xmcTimerPulseSwitch           = millis();
    xmcTimerPulseSwitchPushButton = millis();

    /* Kick the state machine. */
    fsm_list::start();
}

/***********************************************************************************************************************
 @brief
 @param
 @return     None
**********************************************************************************************************************/
void loop()
{
    int8_t Delta = 0;

    /* Update bounce for switches. */
    xmcPulseSwitchPushButton.update();
    xmcPulseSwitchPower.update();
    xmcPulseSwitch0.update();
    xmcPulseSwitch1.update();
    xmcPulseSwitch2.update();
    xmcPulseSwitch3.update();
    xmcPulseSwitch4.update();
    xmcPulseSwitch5.update();

    /* Check for status change of switches and generate event is required. */
    if (xmcPulseSwitchPower.rose() == true)
    {
        xmcPushButtonEvent.Button = button_power;
        send_event(xmcPushButtonEvent);
    }

    if (xmcPulseSwitch0.rose() == true)
    {
        xmcPushButtonEvent.Button = button_0;
        send_event(xmcPushButtonEvent);
    }

    if (xmcPulseSwitch1.rose() == true)
    {
        xmcPushButtonEvent.Button = button_1;
        send_event(xmcPushButtonEvent);
    }

    if (xmcPulseSwitch2.rose() == true)
    {
        xmcPushButtonEvent.Button = button_2;
        send_event(xmcPushButtonEvent);
    }

    if (xmcPulseSwitch3.rose() == true)
    {
        xmcPushButtonEvent.Button = button_3;
        send_event(xmcPushButtonEvent);
    }

    if (xmcPulseSwitch4.rose() == true)
    {
        xmcPushButtonEvent.Button = button_4;
        send_event(xmcPushButtonEvent);
    }

    if (xmcPulseSwitch5.rose() == true)
    {
        xmcPushButtonEvent.Button = button_5;
        send_event(xmcPushButtonEvent);
    }

    /* Timer update events. */
    XmcUpdate3Sec();
    xmcUpdate100msec();
    xmcUpdate500msec();

    /* Pulse switch handling. */
    if (xmcPulseSwitchPushButton.fell() == true)
    {
        xmcTimerPulseSwitchPushButton = millis();
    }
    else if (xmcPulseSwitchPushButton.rose() == true)
    {
        if (turnedWhilePressed == false)
        {
            if (millis() - xmcTimerPulseSwitchPushButton > 3000)
            {
                wmcPulseSwitchEvent.Status = pushedlong;
                send_event(wmcPulseSwitchEvent);
            }
            else if (millis() - xmcTimerPulseSwitchPushButton < 300)
            {
                wmcPulseSwitchEvent.Status = pushedShort;
                send_event(wmcPulseSwitchEvent);
            }
            else if (millis() - xmcTimerPulseSwitchPushButton < 1100)
            {
                wmcPulseSwitchEvent.Status = pushedNormal;
                send_event(wmcPulseSwitchEvent);
            }
        }
        else
        {
            turnedWhilePressed = false;
        }
    }
    else if ((millis() - xmcTimerPulseSwitch) > 75)
    {
        /* Update pulse switch turn events if turned.*/
        xmcTimerPulseSwitch = millis();
        Delta               = DecoderUpdate();
        if (Delta != 0)
        {
            if (xmcPulseSwitchPushButton.read() == LOW)
            {
                turnedWhilePressed         = true;
                wmcPulseSwitchEvent.Status = pushturn;
            }
            else
            {
                wmcPulseSwitchEvent.Status = turn;
            }

            wmcPulseSwitchEvent.Delta = Delta;

            send_event(wmcPulseSwitchEvent);
        }
    }

    /* Kick XpNet module. */
    send_event(xmcXpNetUpdate);
}
