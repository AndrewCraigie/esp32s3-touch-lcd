#include <Arduino.h>
#include "LCD_Test.h"
#include "esp_heap_caps.h"  // Added to use DMA-safe allocation

UWORD Imagesize = LCD_1IN28_HEIGHT * LCD_1IN28_WIDTH * 2;
UWORD *BlackImage;
CST816S touch(6, 7, 13, 5); // sda, scl, rst, irq

void setup()
{

    delay(1000);  // Allow time for USB CDC to initialize
    printf("Serial initialized.\r\n");

    // Check memory at boot
    printf("Free heap at boot: %d bytes \r\n", ESP.getFreeHeap());
    printf("Free PSRAM at boot: %d bytes \r\n", ESP.getFreePsram());

    printf("\r\n--- Device Diagnostics ---\r\n");
    printf("Chip model: %s\r\n", ESP.getChipModel());
    printf("Chip revision: %d\r\n", ESP.getChipRevision());
    printf("Chip has %d core(s)\r\n", ESP.getChipCores());
    printf("Flash size: %lu bytes\r\n", ESP.getFlashChipSize());
    printf("Sketch size: %lu bytes\r\n", ESP.getSketchSize());
    printf("Free sketch space: %lu bytes\r\n", ESP.getFreeSketchSpace());
    printf("Heap: free=%u bytes, min=%u bytes, largest block=%u bytes\r\n",
       ESP.getFreeHeap(), ESP.getMinFreeHeap(), ESP.getMaxAllocHeap());
    printf("PSRAM: free=%u bytes, min=%u bytes, largest block=%u bytes\r\n",
       ESP.getFreePsram(), ESP.getMinFreePsram(), ESP.getMaxAllocPsram());
    printf("EFuse MAC address: 0x%012llX\r\n", ESP.getEfuseMac());
    printf("--------------------------\r\n");


    // Initialize PSRAM
    if (psramInit()) {
        printf("PSRAM is correctly initialized \r\n");
    } else {
        printf("PSRAM not available \r\n");
    }

    // DMA-safe framebuffer allocation
    BlackImage = (UWORD *)heap_caps_malloc(Imagesize, MALLOC_CAP_DMA);
    if (BlackImage == NULL) {
        printf("Failed to allocate BlackImage (DMA safe) \r\n");
        exit(0);
    } else {
        printf("BlackImage buffer allocated at %p (size: %d) \r\n", BlackImage, Imagesize);
    }

    touch.begin();

    // Initialize hardware
    if (DEV_Module_Init() != 0) {
        printf("GPIO Init Fail! \r\n");
    } else {
        printf("GPIO Init successful! \r\n");
    }

    LCD_1IN28_Init(HORIZONTAL);
    LCD_1IN28_Clear(WHITE);

    // Create image cache
    Paint_NewImage((UBYTE *)BlackImage, LCD_1IN28.WIDTH, LCD_1IN28.HEIGHT, 0, WHITE);
    Paint_SetScale(65);
    Paint_SetRotate(ROTATE_0);
    Paint_Clear(WHITE);

    printf("drawing...\r\n");

    // Basic color sequence
    delay(2000);
    LCD_1IN28_Clear(RED);
    delay(2000);
    LCD_1IN28_Clear(GREEN);
    delay(2000);
    LCD_1IN28_Clear(BLUE);

}

void loop()
{
    delay(2); // allow CPU to switch tasks
}
