/*
<:copyright-BRCM:2016:DUAL/GPL:standard

   Copyright (c) 2016 Broadcom 
   All Rights Reserved

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License, version 2, as published by
the Free Software Foundation (the "GPL").

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.


A copy of the GPL is available at http://www.broadcom.com/licenses/GPLv2.php, or by
writing to the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
Boston, MA 02111-1307, USA.

:>
*/


#ifndef WAN_TYPES_H_INCLUDED
#define WAN_TYPES_H_INCLUDED

#include <linux/types.h>


typedef enum
{
    SUPPORTED_WAN_TYPES_AUTO_SENSE_UNAVAILABLE = (    0),
    SUPPORTED_WAN_TYPES_BIT_GPON               = (1<< 0),
    SUPPORTED_WAN_TYPES_BIT_EPON_1_1           = (1<< 1),
    SUPPORTED_WAN_TYPES_BIT_TURBO_EPON_2_1     = (1<< 2),
    SUPPORTED_WAN_TYPES_BIT_EPON_10_1          = (1<< 3),
    SUPPORTED_WAN_TYPES_BIT_EPON_10_10         = (1<< 4),
    SUPPORTED_WAN_TYPES_BIT_AE_1_1             = (1<< 5),
    SUPPORTED_WAN_TYPES_BIT_AE_10_10           = (1<< 6),
    SUPPORTED_WAN_TYPES_BIT_XGPON              = (1<< 7),
    SUPPORTED_WAN_TYPES_BIT_NGPON2_10_25       = (1<< 8),
    SUPPORTED_WAN_TYPES_BIT_NGPON2_10_10       = (1<< 9),
    SUPPORTED_WAN_TYPES_BIT_XGSPON             = (1<<10),
} SUPPORTED_WAN_TYPES;

typedef uint32_t SUPPORTED_WAN_TYPES_BITMAP;

#endif /* WAN_TYPES_H_INCLUDED */
