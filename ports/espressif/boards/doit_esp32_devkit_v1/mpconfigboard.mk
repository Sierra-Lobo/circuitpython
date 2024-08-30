CIRCUITPY_CREATOR_ID =  0xB0D00000
CIRCUITPY_CREATION_ID = 0x00320002

IDF_TARGET = esp32

CIRCUITPY_ESP_FLASH_MODE = dio
CIRCUITPY_ESP_FLASH_FREQ = 40m
CIRCUITPY_ESP_FLASH_SIZE = 4MB
CIRCUITPY_ESPCAMERA = 0


FROZEN_MPY_DIRS += $(TOP)/frozen/Adafruit_CircuitPython_GPS
FROZEN_MPY_DIRS += $(TOP)/frozen/Adafruit_CircuitPython_MMC56x3
FROZEN_MPY_DIRS += $(TOP)/frozen/Adafruit_CircuitPython_NeoPixel
FROZEN_MPY_DIRS += $(TOP)/frozen/Adafruit_CircuitPython_Register

FROZEN_MPY_DIRS += $(TOP)/frozen/adcs
FROZEN_MPY_DIRS += $(TOP)/frozen/SierraLobo_CircuitPython_DS134x
FROZEN_MPY_DIRS += $(TOP)/frozen/SierraLobo_CircuitPython_IAM20380
FROZEN_MPY_DIRS += $(TOP)/frozen/SierraLobo_CircuitPython_INA230
FROZEN_MPY_DIRS += $(TOP)/frozen/SierraLobo_CircuitPython_MC3419

#FROZEN_MPY_DIRS += $(TOP)/frozen/SierraLobo_MTDB/circuitpython_driver
FROZEN_MPY_DIRS += $(TOP)/frozen/SierraLobo_MTDB
FROZEN_MPY_DIRS += $(TOP)/frozen/pycubed_rfm9x
FROZEN_MPY_DIRS += $(TOP)/frozen/tasko
FROZEN_MPY_DIRS += $(TOP)/frozen/sierralobo_rockblock
