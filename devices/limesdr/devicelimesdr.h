///////////////////////////////////////////////////////////////////////////////////
// Copyright (C) 2017 Edouard Griffiths, F4EXB                                   //
//                                                                               //
// This program is free software; you can redistribute it and/or modify          //
// it under the terms of the GNU General Public License as published by          //
// the Free Software Foundation as version 3 of the License, or                  //
//                                                                               //
// This program is distributed in the hope that it will be useful,               //
// but WITHOUT ANY WARRANTY; without even the implied warranty of                //
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the                  //
// GNU General Public License V3 for more details.                               //
//                                                                               //
// You should have received a copy of the GNU General Public License             //
// along with this program. If not, see <http://www.gnu.org/licenses/>.          //
///////////////////////////////////////////////////////////////////////////////////

#ifndef DEVICES_LIMESDR_DEVICELIMESDR_H_
#define DEVICES_LIMESDR_DEVICELIMESDR_H_

#include "lime/LimeSuite.h"

class DeviceLimeSDR
{
public:
    enum PathRFE
    {
        PATH_RFE_NONE = 0,
        PATH_RFE_LNAH,
        PATH_RFE_LNAL,
        PATH_RFE_LNAW,
        PATH_RFE_LB1,
        PATH_RFE_LB2
    };

    /** set NCO frequency with positive or negative frequency (deals with up/down convert). Enables or disables NCO */
    static bool setNCOFrequency(lms_device_t *device, bool dir_tx, std::size_t chan, bool enable, float frequency);
    /** set LNA gain Range: [1-30] (dB) **/
    static bool SetRFELNA_dB(lms_device_t *device, std::size_t chan, int value);
    /** set TIA gain Range: [1-3] **/
    static bool SetRFETIA_dB(lms_device_t *device, std::size_t chan, int value);
    /** set PGA gain Range: [0-32] (dB) **/
    static bool SetRBBPGA_dB(lms_device_t *device, std::size_t chan, float value);
    /** Set antenna path **/
    static bool setAntennaPath(lms_device_t *device, std::size_t chan, int path);
};

#endif /* DEVICES_LIMESDR_DEVICELIMESDR_H_ */
