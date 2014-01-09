// StepOverCan - sending step/dir pulses over the CAN bus
//
// Loosely based on nxp_lpcxpresso_11c24_periph_ccan_rom from LPCOpen.
// -jcw, 2013-12-24

#include "board.h"

#define TEST_CCAN_BAUD_RATE 500000

#define CAN_ADDR_A 0x410
#define CAN_ADDR_B 0x411

#define PIN_SLEEP     LPC_GPIO,2,8
#define PIN_ENABLE    LPC_GPIO,2,7
#define PIN_RESET     LPC_GPIO,2,6
#define PIN_MS1       LPC_GPIO,2,2
#define PIN_MS2       LPC_GPIO,2,1
#define PIN_MS3       LPC_GPIO,2,0

#define PIN_DIRA      LPC_GPIO,0,1
#define PIN_STEPA     LPC_GPIO,0,11
#define PIN_DIRB      LPC_GPIO,1,1
#define PIN_STEPB     LPC_GPIO,1,2

#define PIN_DIRA_IN   LPC_GPIO,3,0
#define PIN_STEPA_IN  LPC_GPIO,3,1
#define PIN_DIRB_IN   LPC_GPIO,3,2
#define PIN_STEPB_IN  LPC_GPIO,2,10

#define PIN_LED1_O    LPC_GPIO,1,11
#define PIN_LED2_O    LPC_GPIO,1,10

CCAN_MSG_OBJ_T msg_obj;

union {
  uint8_t byte;
  struct {
    uint8_t sleep :1;
    uint8_t reset :1;
    uint8_t enable :1;
    uint8_t ms1 :1;
    uint8_t ms2 :1;
    uint8_t ms3 :1;
    uint8_t dir :1;
    uint8_t step :1;
  } bits;
} payload;

static void copyToPins (uint16_t addr) {
  Chip_GPIO_SetPinState(PIN_SLEEP, payload.bits.sleep);   // default off
  Chip_GPIO_SetPinState(PIN_ENABLE, payload.bits.enable); // default on
  Chip_GPIO_SetPinState(PIN_RESET, payload.bits.reset);   // default off
  Chip_GPIO_SetPinState(PIN_MS1, payload.bits.ms1);
  Chip_GPIO_SetPinState(PIN_MS2, payload.bits.ms2);
  Chip_GPIO_SetPinState(PIN_MS3, payload.bits.ms3);

#if 1
  Chip_GPIO_SetPinState(PIN_LED1_O, !payload.bits.dir);
  Chip_GPIO_SetPinState(PIN_LED2_O, !payload.bits.step);
#endif
  
  switch (addr) {
    case CAN_ADDR_A:
      Chip_GPIO_SetPinState(PIN_DIRA, payload.bits.dir);
      Chip_GPIO_SetPinState(PIN_STEPA, payload.bits.step);
      break;
    case CAN_ADDR_B:
      Chip_GPIO_SetPinState(PIN_DIRB, payload.bits.dir);
      Chip_GPIO_SetPinState(PIN_STEPB, payload.bits.step);
      break;
  }
}

static void setupPins (void) {
  Chip_GPIO_SetPinDIROutput(PIN_SLEEP);
  Chip_GPIO_SetPinDIROutput(PIN_ENABLE);
  Chip_GPIO_SetPinDIROutput(PIN_RESET);
  Chip_GPIO_SetPinDIROutput(PIN_MS1);
  Chip_GPIO_SetPinDIROutput(PIN_MS2);
  Chip_GPIO_SetPinDIROutput(PIN_MS3);
  Chip_GPIO_SetPinDIROutput(PIN_DIRA);
  Chip_GPIO_SetPinDIROutput(PIN_STEPA);
  Chip_GPIO_SetPinDIROutput(PIN_DIRB);
  Chip_GPIO_SetPinDIROutput(PIN_STEPB);
#if 1
  Chip_GPIO_SetPinDIROutput(PIN_LED1_O);
  Chip_GPIO_SetPinDIROutput(PIN_LED2_O);
#endif
  
  // inverted logic, only pull enable low, leave sleep/reset inactive
  payload.bits.sleep = payload.bits.reset = 1;
  // payload.bits.ms1 = payload.bits.ms2 = payload.bits.ms3 = 1;
  copyToPins(CAN_ADDR_A);
  copyToPins(CAN_ADDR_B);
}

static void transmitPins (uint8_t step, uint8_t dir, uint16_t addr) {
  payload.bits.step = step;
  payload.bits.dir = dir;
  
  msg_obj.msgobj  = 0;
  msg_obj.mode_id = addr;
  msg_obj.mask    = 0x0;
  msg_obj.dlc     = 1;
  msg_obj.data[0] = payload.byte;
  LPC_CCAN_API->can_transmit(&msg_obj);
}

static void baudrateCalculate(uint32_t baud_rate, uint32_t *canTimingCfg) {
  Chip_Clock_EnablePeriphClock(SYSCTL_CLOCK_CAN);
  uint32_t pClk = Chip_Clock_GetMainClockRate();
  uint32_t clk_per_bit = pClk / baud_rate;
  uint32_t div, quanta, segs;
  for (div = 0; div <= 15; div++) {
    for (quanta = 1; quanta <= 32; quanta++) {
      for (segs = 3; segs <= 17; segs++) {
        if (clk_per_bit == (segs * quanta * (div + 1))) {
          segs -= 3;
          uint32_t seg1 = segs / 2;
          uint32_t seg2 = segs - seg1;
          uint32_t can_sjw = seg1 > 3 ? 3 : seg1;
          canTimingCfg[0] = div;
          canTimingCfg[1] = ((quanta - 1) & 0x3F) | (can_sjw & 0x03) << 6 |
                              (seg1 & 0x0F) << 8 | (seg2 & 0x07) << 12;
          return;
        }
      }
    }
  }
}

void CAN_rxCallback (uint8_t msg_obj_num) {
  msg_obj.msgobj = msg_obj_num;
  LPC_CCAN_API->can_receive(&msg_obj);
  // quick sanity check, ignore packets with more than 1 byte of data
  if (msg_obj.dlc == 1) {
    payload.byte = msg_obj.data[0];
    copyToPins(msg_obj.mode_id);
  }
}

void CAN_txCallback (uint8_t msg_obj_num) {
}

void CAN_errorCallback (uint32_t error_info) {
}

void CAN_IRQHandler (void) {
  LPC_CCAN_API->isr();
}

static CCAN_CALLBACKS_T callbacks = {
  CAN_rxCallback,
  CAN_txCallback,
  CAN_errorCallback,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
};

void delayMillis (int ms) {
  int i;
  while (--ms >= 0)
    for (i = 0; i < 1500; ++i)
      __asm("nop\n");
}

int main (void) {
  SystemCoreClockUpdate();
  Board_Init();
  setupPins();

  Chip_GPIO_SetPinState(PIN_LED1_O, 1);
  Chip_GPIO_SetPinState(PIN_LED2_O, 1);
  Chip_GPIO_SetPinState(PIN_STEPA, 1);
  Chip_GPIO_SetPinState(PIN_DIRA, 1);
  Chip_GPIO_SetPinState(PIN_STEPB, 1);
  Chip_GPIO_SetPinState(PIN_DIRB, 1);
  delayMillis(3000);
  Chip_GPIO_SetPinState(PIN_LED1_O, 0);
  Chip_GPIO_SetPinState(PIN_LED2_O, 0);
  Chip_GPIO_SetPinState(PIN_STEPA, 0);
  Chip_GPIO_SetPinState(PIN_DIRA, 0);
  Chip_GPIO_SetPinState(PIN_STEPB, 0);
  Chip_GPIO_SetPinState(PIN_DIRB, 0);

  uint32_t clkInitTable[2];
  baudrateCalculate(TEST_CCAN_BAUD_RATE, clkInitTable);
  LPC_CCAN_API->init_can(clkInitTable, true);
  LPC_CCAN_API->config_calb(&callbacks);
  NVIC_EnableIRQ(CAN_IRQn);

  /* Configure message object 1 to receive all 11-bit messages 0x410-0x413 */
  msg_obj.msgobj = 1;
  msg_obj.mode_id = 0x410;
  msg_obj.mask = 0x7FC;
  LPC_CCAN_API->config_rxmsgobj(&msg_obj);

  int prevStepA = 0, prevStepB = 0;
  while (true)
    if (Chip_GPIO_GetPinState(PIN_STEPA_IN) != prevStepA) {
      int dir = Chip_GPIO_GetPinState(PIN_DIRA);
      prevStepA = !prevStepA;
      transmitPins(prevStepA, dir, CAN_ADDR_A);
    } else if (Chip_GPIO_GetPinState(PIN_STEPB_IN) != prevStepB) {
      int dir = Chip_GPIO_GetPinState(PIN_DIRB);
      prevStepB = !prevStepB;
      transmitPins(prevStepB, dir, CAN_ADDR_B);
    }
}
