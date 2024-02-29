/*
 * File: ins.c
 *
 * Code generated for Simulink model 'ins'.
 *
 * Model version                  : 1.5
 * Simulink Coder version         : 9.7 (R2022a) 13-Nov-2021
 * C/C++ source code generated on : Mon Jan 23 18:38:14 2023
 *
 * Target selection: ert.tlc
 * Embedded hardware selection: ARM Compatible->ARM Cortex-M
 * Code generation objectives: Unspecified
 * Validation result: Not run
 */

#include "ins.h"

/* External inputs (root inport signals with default storage) */
ExtU_ins_T ins_U;

/* External outputs (root outports fed by signals with default storage) */
ExtY_ins_T ins_Y;

/* Real-time model */
static RT_MODEL_ins_T ins_M_;
RT_MODEL_ins_T *const ins_M = &ins_M_;

/* Model step function */
void ins_step(void)
{
  /* Outport: '<Root>/Output' incorporates:
   *  Gain: '<S1>/Gain'
   *  Inport: '<Root>/Input'
   */
  ins_Y.Output = 2.0 * ins_U.Input;
}

/* Model initialize function */
void ins_initialize(void)
{
  /* (no initialization code required) */
}

/* Model terminate function */
void ins_terminate(void)
{
  /* (no terminate code required) */
}

/*
 * File trailer for generated code.
 *
 * [EOF]
 */
