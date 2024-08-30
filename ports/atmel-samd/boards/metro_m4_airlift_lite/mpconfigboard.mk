USB_VID = 0x239A
USB_PID = 0x8038
USB_PRODUCT = "Metro M4 Airlift Lite"
USB_MANUFACTURER = "Adafruit Industries LLC"

CHIP_VARIANT = SAMD51J19A
CHIP_FAMILY = samd51

QSPI_FLASH_FILESYSTEM = 1
EXTERNAL_FLASH_DEVICES = "S25FL116K, S25FL216K, GD25Q16C, W25Q16JVxQ"
LONGINT_IMPL = MPZ

CIRCUITPY__EVE = 1
CIRCUITPY_SYNTHIO = 0

#FROZEN_MPY_DIRS += $(TOP)/frozen/Adafruit_CircuitPython_GPS
#FROZEN_MPY_DIRS += $(TOP)/frozen/Adafruit_CircuitPython_MMC56x3
#FROZEN_MPY_DIRS += $(TOP)/frozen/Adafruit_CircuitPython_NeoPixel
#FROZEN_MPY_DIRS += $(TOP)/frozen/Adafruit_CircuitPython_Register

FROZEN_MPY_DIRS += $(TOP)/frozen/adcs
#FROZEN_MPY_DIRS += $(TOP)/frozen/SierraLobo_CircuitPython_DS134x
#FROZEN_MPY_DIRS += $(TOP)/frozen/SierraLobo_CircuitPython_IAM20380
#FROZEN_MPY_DIRS += $(TOP)/frozen/SierraLobo_CircuitPython_INA230
#FROZEN_MPY_DIRS += $(TOP)/frozen/SierraLobo_CircuitPython_MC3419

FROZEN_MPY_DIRS += $(TOP)/frozen/SierraLobo_MTDB
FROZEN_MPY_DIRS += $(TOP)/frozen/pycubed_rfm9x
FROZEN_MPY_DIRS += $(TOP)/frozen/tasko
FROZEN_MPY_DIRS += $(TOP)/frozen/sierralobo_rockblock

#USER_C_MODULES=../../examples/usercmodule
