/* --COPYRIGHT--,BSD
 * Copyright (c) 2014, Texas Instruments Incorporated
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * *  Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * *  Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * *  Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 * --/COPYRIGHT--*/
//**************************************************************
// structure.c
//
//! \brief Contians definitions for element and sensor structures
//!        used in associated application.
//!
//!        Author: a0224638, 9/10/2015
//!
//**************************************************************

#include "structure.h"


//! \brief Structure definition for button
//!        element (connected to P5.0)
//!
const struct Element my_element = {

        // Select P5.0
		.inputBits = CAPTIOPOSEL_5 + CAPTIOPISEL_0,

		// Set .maxResponse and .threshold according
		// to selected measurement configuration
#ifdef RO_CTIO_TA2_WDTA
        //.maxResponse = 300+655,
        .threshold = 300
#endif
#ifdef RO_CTIO_TA2_TA3
		//.maxResponse = 250+300,
		.threshold = 250
#endif
#ifdef fRO_CTIO_TA2_TA3
		//.maxResponse = 250+250,
		.threshold = 250
#endif
};



//! \brief  This defines the grouping of elements in, the method to measure change in
//!         capacitance for, and the function of the My Button sensor group
//!
const struct Sensor my_button =	{

		//Select appropriate .halDefinition
#ifdef RO_CTIO_TA2_WDTA
		.halDefinition = RO_CTIO_TA2_WDTA,
#endif
#ifdef RO_CTIO_TA2_TA3
		.halDefinition = RO_CTIO_TA2_TA3,
#endif
#ifdef fRO_CTIO_TA2_TA3
		.halDefinition = fRO_CTIO_TA2_TA3,
#endif
		//Set CAPTIO0CTL register for input CAPTIO config
		// based on chosen timers
		.inputCaptioctlRegister = (uint16_t *)&CAPTIO0CTL,
        .numElements = 1,
        .baseOffset = 4,

        // Pointer to elements
        .arrayPtr[0] = &my_element,  // point to first element

        // Timer Information
#ifdef RO_CTIO_TA2_WDTA
      	.measGateSource = GATE_WDTA_VLO,     //  Gate Clock Source
		.accumulationCycles = WDTA_GATE_64
#endif
#ifdef RO_CTIO_TA2_TA3
		.measGateSource = TIMER_ACLK,     // Clock Source for the Gate Timer A3
		.sourceScale = TIMER_SOURCE_DIV_3,
		.accumulationCycles = 15        //CCRx for Timer A3*/
#endif
#ifdef fRO_CTIO_TA2_TA3
		.measGateSource = TIMER_SMCLK,     // Clock source for Timer A3
		.sourceScale = TIMER_SOURCE_DIV_0,
		.accumulationCycles = 10000
#endif
};
