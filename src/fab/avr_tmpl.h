 /*
 ****************************************************************************
 *
 * simulavr - A simulator for the Atmel AVR family of microcontrollers.
 * Copyright (C) 2001, 2002, 2003   Klaus Rudolph
 * Copyright (C) 2008 Onno Kortmann
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 ****************************************************************************
 *
 * THIS FILE HAS BEEN AUTOMATICALLY GENERATED FROM avr_tmpl.h.
 *                     --- DO NOT EDIT MANUALLY! ---
 */
\#ifndef AVR_PART_$(part)
\#define AVR_PART_$(part)
\#include "hwdecls.h"
\#include "avrdevice.h"

class AVR_$(part) : public AvrDevice {
 public:
    AVR_$(part)();
    ~AVR_$(part)();
 private:
#for $Letter in $io_ports
    HWPort *port$Letter.lower;
#endfor

    HWMcucr *mcucr;
#if $has_prescaler    
    HWPrescaler *prescaler;
#endif

#if $has_timer1
    HWTimer01Irq *timer01irq;
    HWTimer1 *timer1;
#endif    
};
\#endif