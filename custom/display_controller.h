#include <SPI.h>
#include <TFT_eSPI.h>
#include "esphome.h"

class DisplayController : public Component {
    public:
    void setup() override {
//        spi::initBus();
//        spi::begin_tft_write();

        tft.init();
//        PrintOnScreen();
    }
    void loop() override {
    //    update();
    }
    // void update() {
//        PrintOnScreen();
//    }
//    void PrintOnScreen() {
        // tft.fillRect(0, 0, 240, 40, TFT_BLACK);
        // tft.setTextColor(TFT_BLACK, TFT_WHITE);
        // tft.setCursor(8, 4);
        // tft.println("Hello World...");
//    }
    private:
    TFT_eSPI tft = TFT_eSPI();
};
// The globla diplay controller variable
DisplayController *displayController;
