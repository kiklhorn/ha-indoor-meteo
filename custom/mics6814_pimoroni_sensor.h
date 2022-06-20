/* ------------  
Martin Kiklhorn notes:
Pimoroni uses nuvoton MS51XB9AE 
as an digital interface between I2C (0x19) and 
- analog MICS6814 gas NO2/CO/NH3 multi sensor
- RGB indication Led

https://forums.pimoroni.com/t/mics6814-typical-resistances/17230/2
https://github.com/ZodiusInfuser/IOExpander_Library
https://esphome.io/components/sensor/custom.html

---------------------------------------*/

#include "esphome.h"
#include <IOExpander.h>
#include <ColorConverterLib.h>
#include <Adafruit_NeoPixel.h>
//#include git:"https://github.com/ZodiusInfuser/IOExpander/IOExpander.h"

/***** Global Constants *****/

static const byte MICS6814_LED_R = 3;
static const byte MICS6814_LED_G = 7;
static const byte MICS6814_LED_B = 2;


static const byte MICS6814_VREF = 14;
static const byte MICS6814_RED = 12;
static const byte MICS6814_NH3 = 11;
static const byte MICS6814_OX = 13;

static const byte MICS6814_HEATER_EN = 1;

static constexpr float BRIGHTNESS = 0.5f; 
static constexpr unsigned int PERIOD = (unsigned int)(255.0f / BRIGHTNESS);  

static const bool INVERT_OUTPUT = true; //true for common cathode, false for common anode

/***** Global Variables *****/
IOExpander ioe(Wire, 0x19);

class MICS6814CustomSensor : public PollingComponent {
    public:
    Sensor *red_sensor = new Sensor();
    Sensor *nh3_sensor = new Sensor();
    Sensor *oxd_sensor = new Sensor();

    MICS6814CustomSensor() : PollingComponent(60000) { }

    void setup() override {

    Serial.begin(9600);
    delay(1000);
    
    //Wire.begin(D2, D1);
    
    if(!ioe.initialise())
    
    {
    while(true)
        delay(1000);
        Serial.println("Not Initialised");
    }

    ioe.setMode(MICS6814_VREF, IOExpander::PIN_ADC);
    ioe.setMode(MICS6814_RED, IOExpander::PIN_ADC);
    ioe.setMode(MICS6814_NH3, IOExpander::PIN_ADC);
    ioe.setMode(MICS6814_OX, IOExpander::PIN_ADC);

    ioe.setMode(MICS6814_HEATER_EN, IOExpander::PIN_OUT);
    ioe.output(MICS6814_HEATER_EN, LOW);

    ioe.setPwmPeriod(PERIOD);
    ioe.setPwmControl(2);  //PWM as fast as we can to avoid LED flicker

    ioe.setMode(MICS6814_LED_R, IOExpander::PIN_PWM, false, INVERT_OUTPUT);
    ioe.setMode(MICS6814_LED_G, IOExpander::PIN_PWM, false, INVERT_OUTPUT);
    ioe.setMode(MICS6814_LED_B, IOExpander::PIN_PWM, false, INVERT_OUTPUT);
    
    }

    void update() override {

    float h = fmodf(((float)millis() * 360.0f) / 10000.0f, 360.0f);
    byte r, g, b;
    ColorConverter::HsvToRgb(h, 100.0f, 100.0f, r, g, b);
    
    ioe.output(MICS6814_LED_R, r);
    ioe.output(MICS6814_LED_G, g);
    ioe.output(MICS6814_LED_B, b);

    float ref = ioe.inputAsVoltage(MICS6814_VREF);
    float red = ioe.inputAsVoltage(MICS6814_RED);
    float nh3 = ioe.inputAsVoltage(MICS6814_NH3);
    float oxd = ioe.inputAsVoltage(MICS6814_OX);

    red = (red * 56000) / (ioe.getAdcVref() - red);
    nh3 = (nh3 * 56000) / (ioe.getAdcVref() - nh3);
    oxd = (oxd * 56000) / (ioe.getAdcVref() - oxd);


    red_sensor->publish_state(red);
    nh3_sensor->publish_state(nh3);
    oxd_sensor->publish_state(oxd);
    ESP_LOGD("MICS6814", "The value of sensor red is: %.0f", red);
    ESP_LOGD("MICS6814", "The value of sensor nh3 is: %.0f", nh3);
    ESP_LOGD("MICS6814", "The value of sensor oxd is: %.0f", oxd);
    }
};
