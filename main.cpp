#include <cstdio>
#include <math.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"

#include "esp_adc/adc_oneshot.h"
#include "esp_adc/adc_cali.h"
#include "esp_adc/adc_cali_scheme.h"

extern "C" void app_main(void)
{
    adc_oneshot_unit_handle_t adc_handle;

    adc_oneshot_unit_init_cfg_t init_cfg = {
        .unit_id = ADC_UNIT_1,
        .clk_src = ADC_RTC_CLK_SRC_DEFAULT,
        .ulp_mode = ADC_ULP_MODE_DISABLE
    };
    adc_oneshot_new_unit(&init_cfg, &adc_handle);

    adc_oneshot_chan_cfg_t chan_cfg = {
        .atten = ADC_ATTEN_DB_12,
        .bitwidth = ADC_BITWIDTH_12
    };
    adc_oneshot_config_channel(adc_handle, ADC_CHANNEL_6, &chan_cfg);


    adc_cali_handle_t cali_handle;

    adc_cali_curve_fitting_config_t cali_cfg = {
        .unit_id = ADC_UNIT_1,
        .chan = ADC_CHANNEL_6,
        .atten = ADC_ATTEN_DB_12,
        .bitwidth = ADC_BITWIDTH_12
    };

    adc_cali_create_scheme_curve_fitting(&cali_cfg, &cali_handle);

    printf("RAW\tU_manual(mV)\tU_cali(mV)\tError(%%)\n");
    printf("--------------------------------------------------\n");

    while (true) {
        int raw = 0;
        int voltage_cali = 0;

        adc_oneshot_read(adc_handle, ADC_CHANNEL_6, &raw);
        adc_cali_raw_to_voltage(cali_handle, raw, &voltage_cali);

        float voltage_manual = (raw / 4095.0f) * 3300.0f;
        float error = fabs(voltage_manual - voltage_cali) / voltage_cali * 100.0f;

        printf("%4d\t%8.1f\t\t%4d\t\t%.2f\n",
               raw, voltage_manual, voltage_cali, error);

        vTaskDelay(pdMS_TO_TICKS(100));
    }
}