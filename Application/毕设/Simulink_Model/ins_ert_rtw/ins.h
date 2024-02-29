/*
 * File: ins.h
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

#ifndef RTW_HEADER_ins_h_
#define RTW_HEADER_ins_h_
#ifndef ins_COMMON_INCLUDES_
#define ins_COMMON_INCLUDES_
#include "rtwtypes.h"
#endif                                 /* ins_COMMON_INCLUDES_ */

#include <stddef.h>
#include "ins_types.h"

/* Macros for accessing real-time model data structure */
#ifndef rtmGetErrorStatus
#define rtmGetErrorStatus(rtm)         ((rtm)->errorStatus)
#endif

#ifndef rtmSetErrorStatus
#define rtmSetErrorStatus(rtm, val)    ((rtm)->errorStatus = (val))
#endif

/* External inputs (root inport signals with default storage) */
typedef struct {
  real_T Input;                        /* '<Root>/Input' */
} ExtU_ins_T;

/* External outputs (root outports fed by signals with default storage) */
typedef struct {
  real_T Output;                       /* '<Root>/Output' */
} ExtY_ins_T;

/* Real-time Model Data Structure */
struct tag_RTM_ins_T {
  const char_T * volatile errorStatus;
};

/* External inputs (root inport signals with default storage) */
extern ExtU_ins_T ins_U;

/* External outputs (root outports fed by signals with default storage) */
extern ExtY_ins_T ins_Y;

/* Model entry point functions */
extern void ins_initialize(void);
extern void ins_step(void);
extern void ins_terminate(void);

/* Real-time Model object */
extern RT_MODEL_ins_T *const ins_M;

/*-
 * The generated code includes comments that allow you to trace directly
 * back to the appropriate location in the model.  The basic format
 * is <system>/block_name, where system is the system number (uniquely
 * assigned by Simulink) and block_name is the name of the block.
 *
 * Use the MATLAB hilite_system command to trace the generated code back
 * to the model.  For example,
 *
 * hilite_system('<S3>')    - opens system 3
 * hilite_system('<S3>/Kp') - opens and selects block Kp which resides in S3
 *
 * Here is the system hierarchy for this model
 *
 * '<Root>' : 'ins'
 * '<S1>'   : 'ins/Subsystem'
 */
#endif                                 /* RTW_HEADER_ins_h_ */

/*
 * File trailer for generated code.
 *
 * [EOF]
 */
