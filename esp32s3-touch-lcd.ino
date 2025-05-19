#include <Arduino.h>
#include "LCD_Test.h"
#include "esp_heap_caps.h"  // Added to use DMA-safe allocation

UWORD Imagesize = LCD_1IN28_HEIGHT * LCD_1IN28_WIDTH * 2;
UWORD *BlackImage;
CST816S touch(6, 7, 13, 5); // sda, scl, rst, irq

void setup()
{
    Serial.begin(115200);

    // Check memory at boot
    Serial.printf("Free heap at boot: %d bytes\n", ESP.getFreeHeap());
    Serial.printf("Free PSRAM at boot: %d bytes\n", ESP.getFreePsram());

    // Initialize PSRAM
    if (psramInit()) {
        Serial.println("PSRAM is correctly initialized");
    } else {
        Serial.println("PSRAM not available");
    }

    // DMA-safe framebuffer allocation
    BlackImage = (UWORD *)heap_caps_malloc(Imagesize, MALLOC_CAP_DMA);
    if (BlackImage == NULL) {
        Serial.println("Failed to allocate BlackImage (DMA safe)");
        exit(0);
    } else {
        Serial.printf("BlackImage buffer allocated at %p (size: %d)\n", BlackImage, Imagesize);
    }

    touch.begin();

    // Initialize hardware
    if (DEV_Module_Init() != 0) {
        Serial.println("GPIO Init Fail!");
    } else {
        Serial.println("GPIO Init successful!");
    }

    LCD_1IN28_Init(HORIZONTAL);
    LCD_1IN28_Clear(WHITE);

    // Create image cache
    Paint_NewImage((UBYTE *)BlackImage, LCD_1IN28.WIDTH, LCD_1IN28.HEIGHT, 0, WHITE);
    Paint_SetScale(65);
    Paint_SetRotate(ROTATE_0);
    Paint_Clear(WHITE);

    Serial.println("drawing...\r\n");

    // Basic color sequence
    delay(2000);
    LCD_1IN28_Clear(RED);
    delay(2000);
    LCD_1IN28_Clear(GREEN);
    delay(2000);
    LCD_1IN28_Clear(BLUE);
    delay(2000);

    // Drawing demo
#if 1
    Paint_Clear(WHITE);
    Paint_DrawPoint(50, 41, BLACK, DOT_PIXEL_1X1, DOT_FILL_RIGHTUP);
    Paint_DrawPoint(50, 46, BLACK, DOT_PIXEL_2X2, DOT_FILL_RIGHTUP);
    Paint_DrawPoint(50, 51, BLACK, DOT_PIXEL_3X3, DOT_FILL_RIGHTUP);
    Paint_DrawPoint(50, 56, BLACK, DOT_PIXEL_4X4, DOT_FILL_RIGHTUP);
    Paint_DrawPoint(50, 61, BLACK, DOT_PIXEL_5X5, DOT_FILL_RIGHTUP);

    Paint_DrawLine(60, 40, 90, 70, MAGENTA, DOT_PIXEL_2X2, LINE_STYLE_SOLID);
    Paint_DrawLine(60, 70, 90, 40, MAGENTA, DOT_PIXEL_2X2, LINE_STYLE_SOLID);

    Paint_DrawRectangle(60, 40, 90, 70, RED, DOT_PIXEL_2X2, DRAW_FILL_EMPTY);
    Paint_DrawRectangle(100, 40, 130, 70, BLUE, DOT_PIXEL_2X2, DRAW_FILL_FULL);

    Paint_DrawLine(135, 55, 165, 55, CYAN, DOT_PIXEL_1X1, LINE_STYLE_DOTTED);
    Paint_DrawLine(150, 40, 150, 70, CYAN, DOT_PIXEL_1X1, LINE_STYLE_DOTTED);

    Paint_DrawCircle(150, 55, 15, GREEN, DOT_PIXEL_1X1, DRAW_FILL_EMPTY);
    Paint_DrawCircle(185, 55, 15, GREEN, DOT_PIXEL_1X1, DRAW_FILL_FULL);

    Paint_DrawNum(50, 80, 9.87654321, &Font20, 3, WHITE, BLACK);
    Paint_DrawString_EN(50, 100, "ABC", &Font20, 0x000f, 0xfff0);
    Paint_DrawString_EN(50, 161, "Waveshare", &Font16, RED, WHITE);

    LCD_1IN28_Display(BlackImage);
    DEV_Delay_ms(1000);
#endif

    // Sensor + battery test
#if 1
    float acc[3], gyro[3];
    unsigned int tim_count = 0;
    uint16_t result;

    QMI8658_init();
    Serial.println("QMI8658_init done");

    const float conversion_factor = 3.3f / (1 << 12) * 3;
    Paint_Clear(WHITE);
    Paint_DrawRectangle(0, 0, 240, 47, 0xF410, DOT_PIXEL_2X2, DRAW_FILL_FULL);
    Paint_DrawRectangle(0, 47, 240, 120, 0x4F30, DOT_PIXEL_2X2, DRAW_FILL_FULL);
    Paint_DrawRectangle(0, 120, 240, 195, 0xAD55, DOT_PIXEL_2X2, DRAW_FILL_FULL);
    Paint_DrawRectangle(0, 195, 240, 240, 0x2595, DOT_PIXEL_2X2, DRAW_FILL_FULL);

    Paint_DrawString_EN(45, 30, "LongPress Quit", &Font16, WHITE, BLACK);
    Paint_DrawString_EN(45, 50, "ACC_X = ", &Font16, WHITE, BLACK);
    Paint_DrawString_EN(45, 75, "ACC_Y = ", &Font16, WHITE, BLACK);
    Paint_DrawString_EN(45, 100, "ACC_Z = ", &Font16, WHITE, BLACK);
    Paint_DrawString_EN(45, 125, "GYR_X = ", &Font16, WHITE, BLACK);
    Paint_DrawString_EN(45, 150, "GYR_Y = ", &Font16, WHITE, BLACK);
    Paint_DrawString_EN(45, 175, "GYR_Z = ", &Font16, WHITE, BLACK);
    Paint_DrawString_EN(45, 200, "BAT(V)=", &Font16, WHITE, BLACK);
    LCD_1IN28_Display(BlackImage);

    while (true)
    {
        result = DEC_ADC_Read();
        QMI8658_read_xyz(acc, gyro, &tim_count);

        Paint_Clear(WHITE);
        Paint_DrawRectangle(120, 47, 220, 120, 0x4F30, DOT_PIXEL_2X2, DRAW_FILL_FULL);
        Paint_DrawRectangle(120, 120, 220, 195, 0xAD55, DOT_PIXEL_2X2, DRAW_FILL_FULL);
        Paint_DrawRectangle(120, 195, 220, 240, 0x2595, DOT_PIXEL_2X2, DRAW_FILL_FULL);

        Paint_DrawNum(120, 50, acc[0], &Font16, 2, BLACK, WHITE);
        Paint_DrawNum(120, 75, acc[1], &Font16, 2, BLACK, WHITE);
        Paint_DrawNum(120, 100, acc[2], &Font16, 2, BLACK, WHITE);
        Paint_DrawNum(120, 125, gyro[0], &Font16, 2, BLACK, WHITE);
        Paint_DrawNum(120, 150, gyro[1], &Font16, 2, BLACK, WHITE);
        Paint_DrawNum(120, 175, gyro[2], &Font16, 2, BLACK, WHITE);
        Paint_DrawNum(130, 200, result * conversion_factor, &Font16, 2, BLACK, WHITE);

        LCD_1IN28_DisplayWindows(120, 50, 210, 200, BlackImage);
        LCD_1IN28_DisplayWindows(130, 200, 220, 220, BlackImage);

        if (touch.available()) {
            if (touch.data.y < 45) {
                Serial.println("Touch detected near top edge. Exiting sensor loop.");
                break;
            }
        }
    }
#endif

    delay(2000);

    // Touch test
#if 1
    Paint_Clear(WHITE);
    Paint_DrawRectangle(0, 0, 240, 47, 0x2595, DOT_PIXEL_2X2, DRAW_FILL_FULL);
    Paint_DrawString_EN(60, 30, "Touch test", &Font16, WHITE, BLACK);
    LCD_1IN28_Display(BlackImage);

    while (true)
    {
        if (touch.available()) {
            Paint_DrawPoint(touch.data.x, touch.data.y, BLACK, DOT_PIXEL_3X3, DOT_FILL_RIGHTUP);
            LCD_1IN28_DisplayWindows(touch.data.x, touch.data.y, touch.data.x + 3, touch.data.y + 3, BlackImage);
        }
    }
#endif
}

void loop()
{
    delay(2); // allow CPU to switch tasks
}
