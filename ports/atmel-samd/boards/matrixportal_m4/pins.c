#include "py/objtuple.h"
#include "shared-bindings/board/__init__.h"

STATIC const mp_rom_obj_tuple_t matrix_addr_tuple = {
    {&mp_type_tuple},
    5,
    {
        MP_ROM_PTR(&pin_PB07),
        MP_ROM_PTR(&pin_PB08),
        MP_ROM_PTR(&pin_PB09),
        MP_ROM_PTR(&pin_PB15),
        MP_ROM_PTR(&pin_PB13),
    }
};

STATIC const mp_rom_obj_tuple_t matrix_data_tuple = {
    {&mp_type_tuple},
    6,
    {
        MP_ROM_PTR(&pin_PB00),
        MP_ROM_PTR(&pin_PB01),
        MP_ROM_PTR(&pin_PB02),

        MP_ROM_PTR(&pin_PB03),
        MP_ROM_PTR(&pin_PB04),
        MP_ROM_PTR(&pin_PB05),
    }
};

STATIC const mp_rom_map_elem_t matrix_common_table[] = {
    { MP_OBJ_NEW_QSTR(MP_QSTR_rgb_pins),MP_ROM_PTR(&matrix_data_tuple) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_clock_pin),MP_ROM_PTR(&pin_PB06) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_latch_pin),MP_ROM_PTR(&pin_PB14) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_output_enable_pin),MP_ROM_PTR(&pin_PB12) },
};
MP_DEFINE_CONST_DICT(matrix_common_dict, matrix_common_table);


STATIC const mp_rom_map_elem_t board_module_globals_table[] = {
    CIRCUITPYTHON_BOARD_DICT_STANDARD_ITEMS

    { MP_OBJ_NEW_QSTR(MP_QSTR_A0), MP_ROM_PTR(&pin_PA02) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_A1), MP_ROM_PTR(&pin_PA05) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_A2), MP_ROM_PTR(&pin_PA04) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_A3), MP_ROM_PTR(&pin_PA06) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_A4), MP_ROM_PTR(&pin_PA07) },

    { MP_OBJ_NEW_QSTR(MP_QSTR_TX),MP_ROM_PTR(&pin_PA00) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_RX),MP_ROM_PTR(&pin_PA01) },

    // ESP control
    { MP_OBJ_NEW_QSTR(MP_QSTR_ESP_CS), MP_ROM_PTR(&pin_PB17) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_ESP_GPIO0), MP_ROM_PTR(&pin_PA20) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_ESP_BUSY), MP_ROM_PTR(&pin_PA22) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_ESP_RESET), MP_ROM_PTR(&pin_PA21) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_ESP_RTS), MP_ROM_PTR(&pin_PA18) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_ESP_TX), MP_ROM_PTR(&pin_PA13) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_ESP_RX), MP_ROM_PTR(&pin_PA12) },

    { MP_OBJ_NEW_QSTR(MP_QSTR_SCL),MP_ROM_PTR(&pin_PB30) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_SDA),MP_ROM_PTR(&pin_PB31) },

    { MP_OBJ_NEW_QSTR(MP_QSTR_NEOPIXEL),MP_ROM_PTR(&pin_PA23) },

    { MP_OBJ_NEW_QSTR(MP_QSTR_SCK),MP_ROM_PTR(&pin_PA16) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_MOSI),MP_ROM_PTR(&pin_PA19) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_MISO),MP_ROM_PTR(&pin_PA17) },

    { MP_OBJ_NEW_QSTR(MP_QSTR_MTX_ADDRESS),MP_ROM_PTR(&matrix_addr_tuple) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_MTX_COMMON),MP_ROM_PTR(&matrix_common_dict) },

    { MP_OBJ_NEW_QSTR(MP_QSTR_MTX_R1),MP_ROM_PTR(&pin_PB00) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_MTX_G1),MP_ROM_PTR(&pin_PB01) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_MTX_B1),MP_ROM_PTR(&pin_PB02) },

    { MP_OBJ_NEW_QSTR(MP_QSTR_MTX_R2),MP_ROM_PTR(&pin_PB03) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_MTX_G2),MP_ROM_PTR(&pin_PB04) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_MTX_B2),MP_ROM_PTR(&pin_PB05) },

    { MP_OBJ_NEW_QSTR(MP_QSTR_MTX_ADDRA),MP_ROM_PTR(&pin_PB07) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_MTX_ADDRB),MP_ROM_PTR(&pin_PB08) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_MTX_ADDRC),MP_ROM_PTR(&pin_PB09) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_MTX_ADDRD),MP_ROM_PTR(&pin_PB15) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_MTX_ADDRE),MP_ROM_PTR(&pin_PB13) },

    { MP_OBJ_NEW_QSTR(MP_QSTR_MTX_CLK),MP_ROM_PTR(&pin_PB06) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_MTX_LAT),MP_ROM_PTR(&pin_PB14) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_MTX_OE),MP_ROM_PTR(&pin_PB12) },

    { MP_ROM_QSTR(MP_QSTR_ACCELEROMETER_INTERRUPT), MP_ROM_PTR(&pin_PA27) },

    // Grounded when closed.
    { MP_OBJ_NEW_QSTR(MP_QSTR_BUTTON_UP),MP_ROM_PTR(&pin_PB22) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_BUTTON_DOWN),MP_ROM_PTR(&pin_PB23) },

    { MP_OBJ_NEW_QSTR(MP_QSTR_LED),MP_ROM_PTR(&pin_PA14) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_L),MP_ROM_PTR(&pin_PA14) },

    { MP_ROM_QSTR(MP_QSTR_I2C), MP_ROM_PTR(&board_i2c_obj) },
    { MP_ROM_QSTR(MP_QSTR_STEMMA_I2C), MP_ROM_PTR(&board_i2c_obj) },
    { MP_ROM_QSTR(MP_QSTR_SPI), MP_ROM_PTR(&board_spi_obj) },
    { MP_ROM_QSTR(MP_QSTR_UART), MP_ROM_PTR(&board_uart_obj) },
};
MP_DEFINE_CONST_DICT(board_module_globals, board_module_globals_table);
