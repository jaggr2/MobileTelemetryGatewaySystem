/***************************************************************************//**
 * @brief   Register definitions for the LSM330.
 *
 * @file    lsm330_registers.h
 * @version 1.0
 * @date    08.01.2015
 * @author  Daniel Meer
 *
 * @remark  Last Modifications:
 *          08.01.2015 meerd1 created
 ******************************************************************************/

#ifndef TXW51_FRAMEWORK_HW_LSM330_REGISTERS_H_
#define TXW51_FRAMEWORK_HW_LSM330_REGISTERS_H_

/*----- Header-Files ---------------------------------------------------------*/

/*----- Macros ---------------------------------------------------------------*/

/* WHO_AM_I_A (Who am I linear acceleration sensor register (r)) */
#define TXW51_LSM330_REG_WHO_AM_I_A             ( 0x0FUL )
#define TXW51_LSM330_REG_WHO_AM_I_A_DEFAULT     ( 0x40UL )

/* CTRL_REG4_A (Linear acceleration sensor control register 4 (r/w)) */
#define TXW51_LSM330_REG_CTRL_REG4_A            ( 0x23UL )
#define TXW51_LSM330_REG_CTRL_REG4_A_STRT       ( 1UL << 0 )
#define TXW51_LSM330_REG_CTRL_REG4_A_VFILT      ( 1UL << 2 )
#define TXW51_LSM330_REG_CTRL_REG4_A_INT1_EN    ( 1UL << 3 )
#define TXW51_LSM330_REG_CTRL_REG4_A_INT2_EN    ( 1UL << 4 )
#define TXW51_LSM330_REG_CTRL_REG4_A_IEL        ( 1UL << 5 )
#define TXW51_LSM330_REG_CTRL_REG4_A_IEA        ( 1UL << 6 )
#define TXW51_LSM330_REG_CTRL_REG4_A_DR_EN      ( 1UL << 7 )

#define TXW51_LSM330_REG_CTRL_REG4_A_IEL_Latched        ( 0 )
#define TXW51_LSM330_REG_CTRL_REG4_A_IEL_Pulsed         ( TXW51_LSM330_REG_CTRL_REG4_A_IEL )

#define TXW51_LSM330_REG_CTRL_REG4_A_IEA_ActiveLow      ( 0 )
#define TXW51_LSM330_REG_CTRL_REG4_A_IEA_ActiveHigh     ( TXW51_LSM330_REG_CTRL_REG4_A_IEA )

/* CTRL_REG5_A (Linear acceleration sensor control register 5 (r/w)) */
#define TXW51_LSM330_REG_CTRL_REG5_A            ( 0x20UL )
#define TXW51_LSM330_REG_CTRL_REG5_A_XEN        ( 1UL << 0 )
#define TXW51_LSM330_REG_CTRL_REG5_A_YEN        ( 1UL << 1 )
#define TXW51_LSM330_REG_CTRL_REG5_A_ZEN        ( 1UL << 2 )
#define TXW51_LSM330_REG_CTRL_REG5_A_BDU        ( 1UL << 3 )
#define TXW51_LSM330_REG_CTRL_REG5_A_ODR_Pos    ( 4 )
#define TXW51_LSM330_REG_CTRL_REG5_A_ODR_Msk    ( 0x0FUL << TXW51_LSM330_REG_CTRL_REG5_A_ODR_Pos )

/* CTRL_REG6_A (Linear acceleration sensor control register 6 (r/w)) */
#define TXW51_LSM330_REG_CTRL_REG6_A                ( 0x24UL )
#define TXW51_LSM330_REG_CTRL_REG6_A_SIM            ( 1UL << 0 )
#define TXW51_LSM330_REG_CTRL_REG6_A_FSCALE_Pos     ( 3 )
#define TXW51_LSM330_REG_CTRL_REG6_A_FSCALE_Msk     ( 0x07UL << TXW51_LSM330_REG_CTRL_REG6_A_FSCALE_Pos )
#define TXW51_LSM330_REG_CTRL_REG6_A_BW_Pos         ( 6 )
#define TXW51_LSM330_REG_CTRL_REG6_A_BW_Mks         ( 0x03UL << TXW51_LSM330_REG_CTRL_REG6_A_BW_Pos )

/* CTRL_REG7_A (Linear acceleration sensor control register 7 (r/w)) */
#define TXW51_LSM330_REG_CTRL_REG7_A                ( 0x25UL )
#define TXW51_LSM330_REG_CTRL_REG7_A_P2_BOOT        ( 1UL << 0 )
#define TXW51_LSM330_REG_CTRL_REG7_A_P1_OVERRUN     ( 1UL << 1 )
#define TXW51_LSM330_REG_CTRL_REG7_A_P1_WTM         ( 1UL << 2 )
#define TXW51_LSM330_REG_CTRL_REG7_A_P1_EMPTY       ( 1UL << 3 )
#define TXW51_LSM330_REG_CTRL_REG7_A_ADD_INC        ( 1UL << 4 )
#define TXW51_LSM330_REG_CTRL_REG7_A_WTM_EN         ( 1UL << 5 )
#define TXW51_LSM330_REG_CTRL_REG7_A_FIFO_EN        ( 1UL << 6 )
#define TXW51_LSM330_REG_CTRL_REG7_A_BOOT           ( 1UL << 7 )

/* STATUS_REG_A (Linear acceleration sensor status register (r)) */
#define TXW51_LSM330_REG_STATUS_REG_A               ( 0x27UL )
#define TXW51_LSM330_REG_STATUS_REG_A_XDA           ( 1UL << 0 )
#define TXW51_LSM330_REG_STATUS_REG_A_YDA           ( 1UL << 1 )
#define TXW51_LSM330_REG_STATUS_REG_A_ZDA           ( 1UL << 2 )
#define TXW51_LSM330_REG_STATUS_REG_A_ZYXDA         ( 1UL << 3 )
#define TXW51_LSM330_REG_STATUS_REG_A_XOR           ( 1UL << 4 )
#define TXW51_LSM330_REG_STATUS_REG_A_YOR           ( 1UL << 5 )
#define TXW51_LSM330_REG_STATUS_REG_A_ZOR           ( 1UL << 6 )
#define TXW51_LSM330_REG_STATUS_REG_A_ZYXOR         ( 1UL << 7 )

/* OFF_xx (Offset correction xx-axis register, signed value (r/w)) */
#define TXW51_LSM330_REG_OFF_X      ( 0x10UL )
#define TXW51_LSM330_REG_OFF_Y      ( 0x11UL )
#define TXW51_LSM330_REG_OFF_Z      ( 0x12UL )

/* CS_xx (Constant shift signed value xx-axis register (r/w)) */
#define TXW51_LSM330_REG_CS_X       ( 0x13UL )
#define TXW51_LSM330_REG_CS_Y       ( 0x14UL )
#define TXW51_LSM330_REG_CS_Z       ( 0x15UL )

/* LC_x (16-bit long-counter register for interrupt state machine programs timing (r/w)) */
#define TXW51_LSM330_REG_LC_L       ( 0x16UL )
#define TXW51_LSM330_REG_LC_H       ( 0x17UL )

/* STAT (Interrupt synchronization register (r)) */
#define TXW51_LSM330_REG_STAT       ( 0x18UL )

/* VFC_x (Vector coefficient register 1 for DIff filter (r/w)) */
#define TXW51_LSM330_REG_VFC_1      ( 0x1BUL )
#define TXW51_LSM330_REG_VFC_2      ( 0x1CUL )
#define TXW51_LSM330_REG_VFC_3      ( 0x1DUL )
#define TXW51_LSM330_REG_VFC_4      ( 0x1EUL )

/* THRS3 (Threshold value register (r/w)) */
#define TXW51_LSM330_REG_THRS3      ( 0x1FUL )

/* OUT_xx_yy_A (xx-axis linear acceleration output data register (r)) */
#define TXW51_LSM330_REG_OUT_X_L_A      ( 0x28UL )
#define TXW51_LSM330_REG_OUT_X_H_A      ( 0x29UL )
#define TXW51_LSM330_REG_OUT_Y_L_A      ( 0x2AUL )
#define TXW51_LSM330_REG_OUT_Y_H_A      ( 0x2BUL )
#define TXW51_LSM330_REG_OUT_Z_L_A      ( 0x2CUL )
#define TXW51_LSM330_REG_OUT_Z_H_A      ( 0x2DUL )

/* FIFO_CTRL_REG_A (linear acceleration sensor FIFO control register (r/w)) */
#define TXW51_LSM330_REG_FIFO_CTRL_REG_A            ( 0x2EUL )
#define TXW51_LSM330_REG_FIFO_CTRL_REG_A_WTMP_Pos   ( 0 )
#define TXW51_LSM330_REG_FIFO_CTRL_REG_A_WTMP_Msk   ( 0x1FUL << TXW51_LSM330_REG_FIFO_CTRL_REG_A_WTMP_Pos )
#define TXW51_LSM330_REG_FIFO_CTRL_REG_A_FMODE_Pos  ( 5 )
#define TXW51_LSM330_REG_FIFO_CTRL_REG_A_FMODE_Msk  ( 0x07UL << TXW51_LSM330_REG_FIFO_CTRL_REG_A_FMODE_Pos )

/* FIFO_SRC_REG_A (Linear acceleration sensor FIFO source control register (r)) */
#define TXW51_LSM330_REG_FIFO_SRC_REG_A             ( 0x2FUL )
#define TXW51_LSM330_REG_FIFO_SRC_REG_A_FFS_Pos     ( 0 )
#define TXW51_LSM330_REG_FIFO_SRC_REG_A_FFS_Msk     ( 0x1FUL << TXW51_LSM330_REG_FIFO_SRC_REG_A_FFS_Pos )
#define TXW51_LSM330_REG_FIFO_SRC_REG_A_EMPTY       ( 1UL << 5 )
#define TXW51_LSM330_REG_FIFO_SRC_REG_A_OVRN_FIFO   ( 1UL << 6 )
#define TXW51_LSM330_REG_FIFO_SRC_REG_A_WTM         ( 1UL << 7 )

/* CTRL_REG2_A (State machine 1 control register (r/w)) */
#define TXW51_LSM330_REG_CTRL_REG2_A                ( 0x21UL )
#define TXW51_LSM330_REG_CTRL_REG2_A_SM1_EN         ( 1UL << 0 )
#define TXW51_LSM330_REG_CTRL_REG2_A_SM1_PIN        ( 1UL << 3 )
#define TXW51_LSM330_REG_CTRL_REG2_A_SM1_HYST_Pos   ( 5 )
#define TXW51_LSM330_REG_CTRL_REG2_A_SM1_HYST_Msk   ( 0x07UL << TXW51_LSM330_REG_CTRL_REG2_A_SM1_HYST_Pos )

/* STx_1 (State machine 1 code register (r/w)) */
#define TXW51_LSM330_REG_ST1_1              ( 0x40UL )
#define TXW51_LSM330_REG_ST2_1              ( 0x41UL )

#define TXW51_LSM330_SM_OPCODE_GNTH1        ( 0x05UL )
#define TXW51_LSM330_SM_OPCODE_CONT         ( 0x11UL )

/* TIM4_1 () */
/* TIM3_1 () */
/* TIM2_1 () */
/* TIM1_1 () */
/* THRS2_1 () */
/* THRS1_1 (Threshold signed value for state machine 1 operation register (r/w)) */
#define TXW51_LSM330_REG_THRS1_1            ( 0x57UL )

/* MASKB_1 (Axis and sign mask (swap) for state machine 1 motion-detection operation register (r/w)) */
#define TXW51_LSM330_REG_MASKB_1            ( 0x59UL )
#define TXW51_LSM330_REG_MASKB_1_N_Z        ( 1UL << 2 )
#define TXW51_LSM330_REG_MASKB_1_P_Z        ( 1UL << 3 )
#define TXW51_LSM330_REG_MASKB_1_N_Y        ( 1UL << 4 )
#define TXW51_LSM330_REG_MASKB_1_P_Y        ( 1UL << 5 )
#define TXW51_LSM330_REG_MASKB_1_N_X        ( 1UL << 6 )
#define TXW51_LSM330_REG_MASKB_1_P_X        ( 1UL << 7 )

/* MASKA_1 () */
#define TXW51_LSM330_REG_MASKA_1            ( 0x5AUL )
#define TXW51_LSM330_REG_MASKA_1_N_Z        ( 1UL << 2 )
#define TXW51_LSM330_REG_MASKA_1_P_Z        ( 1UL << 3 )
#define TXW51_LSM330_REG_MASKA_1_N_Y        ( 1UL << 4 )
#define TXW51_LSM330_REG_MASKA_1_P_Y        ( 1UL << 5 )
#define TXW51_LSM330_REG_MASKA_1_N_X        ( 1UL << 6 )
#define TXW51_LSM330_REG_MASKA_1_P_X        ( 1UL << 7 )

/* SETT1 (Setting of threshold, peak detection and flags for state machine 1 motion-detection
operation register (r/w)) */
#define TXW51_LSM330_REG_SETT1              ( 0x5BUL )
#define TXW51_LSM330_REG_SETT1_SITR         ( 1UL << 0 )

/* PR1 () */
/* TC1 () */
/* OUTS1 () */
/* PEAK1 () */
/* CTRL_REG3_A () */
/* STx_2 () */
/* TIM4_2 () */
/* TIM3_2 () */
/* TIM2_2 () */
/* TIM1_2 () */
/* THRS2_2 () */
/* THRS1_2 () */
/* MASKB_2 () */
/* MASKA_2 () */
/* SETT2 () */
/* PR2 () */
/* TC2 () */
/* OUTS2 () */
/* PEAK2 () */
/* DES2 () */




/* WHO_AM_I_G (Who am I angular rate sensor register (r)) */
#define TXW51_LSM330_REG_WHO_AM_I_G             ( 0x0FUL )
#define TXW51_LSM330_REG_WHO_AM_I_G_DEFAULT     ( 0xD4UL )

/* CTRL_REG1_G (Angular rate sensor control register 1 (r/w)) */
#define TXW51_LSM330_REG_CTRL_REG1_G            ( 0x20UL )
#define TXW51_LSM330_REG_CTRL_REG1_G_XEN        ( 1UL << 0 )
#define TXW51_LSM330_REG_CTRL_REG1_G_YEN        ( 1UL << 1 )
#define TXW51_LSM330_REG_CTRL_REG1_G_ZEN        ( 1UL << 2 )
#define TXW51_LSM330_REG_CTRL_REG1_G_PD         ( 1UL << 3 )
#define TXW51_LSM330_REG_CTRL_REG1_G_BW_Pos     ( 4 )
#define TXW51_LSM330_REG_CTRL_REG1_G_BW_Msk     ( 0x03UL << TXW51_LSM330_REG_CTRL_REG1_G_BW_Pos )
#define TXW51_LSM330_REG_CTRL_REG1_G_DR_Pos     ( 6 )
#define TXW51_LSM330_REG_CTRL_REG1_G_DR_Msk     ( 0x03UL << TXW51_LSM330_REG_CTRL_REG1_G_DR_Pos )

/* CTRL_REG2_G (Angular rate sensor control register 2 (r/w)) */
#define TXW51_LSM330_REG_CTRL_REG2_G            ( 0x21UL )
#define TXW51_LSM330_REG_CTRL_REG2_G_HPCF_Pos   ( 0 )
#define TXW51_LSM330_REG_CTRL_REG2_G_HPCF_Msk   ( 0x0FUL << TXW51_LSM330_REG_CTRL_REG2_G_HPCF_Pos )
#define TXW51_LSM330_REG_CTRL_REG2_G_HPM_Pos    ( 4 )
#define TXW51_LSM330_REG_CTRL_REG2_G_HPM_Msk    ( 0x03UL << TXW51_LSM330_REG_CTRL_REG2_G_HPM_Pos )
#define TXW51_LSM330_REG_CTRL_REG2_G_LVLEN      ( 1UL << 6 )
#define TXW51_LSM330_REG_CTRL_REG2_G_EXTREN     ( 1UL << 7 )

/* CTRL_REG3_G (Angular rate sensor control register 3 (r/w)) */
#define TXW51_LSM330_REG_CTRL_REG3_G            ( 0x22UL )
#define TXW51_LSM330_REG_CTRL_REG3_G_I2_EMPTY   ( 1UL << 0 )
#define TXW51_LSM330_REG_CTRL_REG3_G_I2_ORUN    ( 1UL << 1 )
#define TXW51_LSM330_REG_CTRL_REG3_G_I2_WTM     ( 1UL << 2 )
#define TXW51_LSM330_REG_CTRL_REG3_G_I2_DRDY    ( 1UL << 3 )
#define TXW51_LSM330_REG_CTRL_REG3_G_PP_OD      ( 1UL << 4 )
#define TXW51_LSM330_REG_CTRL_REG3_G_H_LACTIVE  ( 1UL << 5 )
#define TXW51_LSM330_REG_CTRL_REG3_G_I1_BOOT    ( 1UL << 6 )
#define TXW51_LSM330_REG_CTRL_REG3_G_I1_INT1    ( 1UL << 7 )

/* CTRL_REG4_G (Angular rate sensor control register 4 (r/w)) */
#define TXW51_LSM330_REG_CTRL_REG4_G            ( 0x22UL )
#define TXW51_LSM330_REG_CTRL_REG4_G_SIM        ( 1UL << 0 )
#define TXW51_LSM330_REG_CTRL_REG4_G_FS_Pos     ( 4 )
#define TXW51_LSM330_REG_CTRL_REG4_G_FS_Msk     ( 0x03UL << TXW51_LSM330_REG_CTRL_REG4_G_FS_Pos )
#define TXW51_LSM330_REG_CTRL_REG4_G_BLE        ( 1UL << 6 )
#define TXW51_LSM330_REG_CTRL_REG4_G_BDU        ( 1UL << 7 )

/* CTRL_REG5_G (Angular rate sensor control register 5 (r/w)) */
#define TXW51_LSM330_REG_CTRL_REG5_G                ( 0x24UL )
#define TXW51_LSM330_REG_CTRL_REG5_G_OUT_SEL_Pos    ( 0 )
#define TXW51_LSM330_REG_CTRL_REG5_G_OUT_SEL_Msk    ( 0x03UL << TXW51_LSM330_REG_CTRL_REG5_G_OUT_SEL_Pos )
#define TXW51_LSM330_REG_CTRL_REG5_G_INT1_SEL_Pos   ( 2 )
#define TXW51_LSM330_REG_CTRL_REG5_G_INT1_Sel_Msk   ( 0x03UL << TXW51_LSM330_REG_CTRL_REG5_G_INT1_SEL_Pos )
#define TXW51_LSM330_REG_CTRL_REG5_G_HPEN           ( 1UL << 4 )
#define TXW51_LSM330_REG_CTRL_REG5_G_FIFO_EN        ( 1UL << 6 )
#define TXW51_LSM330_REG_CTRL_REG5_G_BOOT           ( 1UL << 7 )

/* REFERENCE_G (Interrupt reference value register (r/w)) */
#define TXW51_LSM330_REG_REFERENCE_G                ( 0x25UL )

/* OUT_TEMP_G (Temperature data output register (r)) */
#define TXW51_LSM330_REG_OUT_TEMP_G     ( 0x26UL )

/* STATUS_REG_G (Angular rate sensor register (r)) */
/* OUT_xx_yy_G (xx-axis angular rate output data register (r)) */
#define TXW51_LSM330_REG_OUT_X_L_G      ( 0x28UL )
#define TXW51_LSM330_REG_OUT_X_H_G      ( 0x29UL )
#define TXW51_LSM330_REG_OUT_Y_L_G      ( 0x2AUL )
#define TXW51_LSM330_REG_OUT_Y_H_G      ( 0x2BUL )
#define TXW51_LSM330_REG_OUT_Z_L_G      ( 0x2CUL )
#define TXW51_LSM330_REG_OUT_Z_H_G      ( 0x2DUL )

/* FIFO_CTRL_REG_G (Angular rate sensor FIFO control register (r/w)) */
#define TXW51_LSM330_REG_FIFO_CTRL_REG_G            ( 0x2EUL )
#define TXW51_LSM330_REG_FIFO_CTRL_REG_G_WTM_Pos    ( 0 )
#define TXW51_LSM330_REG_FIFO_CTRL_REG_G_WTM_Msk    ( 0x1FUL << TXW51_LSM330_REG_FIFO_CTRL_REG_G_WTM_Pos )
#define TXW51_LSM330_REG_FIFO_CTRL_REG_G_FM_Pos     ( 5 )
#define TXW51_LSM330_REG_FIFO_CTRL_REG_G_FM_Msk     ( 0x07 << TXW51_LSM330_REG_FIFO_CTRL_REG_G_FM_Pos )

/* FIFO_SRC_REG_G (Angular rate sensor FIFO source control register (r)) */
#define TXW51_LSM330_REG_FIFO_SRC_REG_G         ( 0x2FUL )

/* INT1_CFG_G (Angular rate sensor FIFO source control register (r/w)) */
/* INT1_SRC_G (Angular rate sensor interrupt source register (r)) */
/* INT1_THS_XH_G (Angular rate sensor interrupt threshold x-axis high register (r/w)) */
/* INT1_THS_XL_G (Angular rate sensor interrupt threshold x-axis low register (r/w)) */
/* INT1_THS_YH _G (Angular rate sensor interrupt threshold y-axis high register (r/w)) */
/* INT1_THS_YL_G (Angular rate sensor interrupt threshold y-axis low register (r/w)) */
/* INT1_THS_ZH_G (Angular rate sensor interrupt threshold z-axis high register (r/w)) */
/* INT1_THS_ZL_G (Angular rate sensor interrupt threshold z-axis low register (r/w)) */
/* INT1_DURATION_G (Angular rate sensor interrupt duration register (r/w)) */


/*----- Data types -----------------------------------------------------------*/

/*----- Function prototypes --------------------------------------------------*/

/*----- Data -----------------------------------------------------------------*/

#endif /* TXW51_FRAMEWORK_HW_LSM330_REGISTERS_H_ */
