#ifndef _BOARD_H_
#define _BOARD_H_
u32 chip_mode(void);
int cpu_clk_config(void);
void wtd_enable(u8 enable);
void npu_ocm_enable();
void iram_access_enable();
void pin_power_detect();

#endif
