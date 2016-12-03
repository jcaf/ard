/****************************************************************************
* Project: Battery System
*
* 2016-05-11-17.01
* jcaf @ jcafOpenSuse
*
* Copyright 2016 Juan Carlos Agüero Flores
*
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
*     http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
****************************************************************************/
#include <Arduino.h>
#include "system.h"
#include "utils.h"
#include "fuelsensor.h"
#include "fuelsensor_jt606x.h"
#include "spline.h"
#include <EEPROM.h>

struct _fuelsensor fuelsensor;

#define fuelsensor_loaddata() do{EEPROM.get(EEPROM_ADDR_FSEE, fuelsensor.fsEE);}while(0)

void fuelsensor_init(void)
{
    fuelsensor_loaddata();
}

void fuelsensor_check_consistency(void)
{
    if (fuelsensor.fsEE.tank.type == FS_TANK_TYPE_RECTANGULAR)
    {

        fuelsensor.fsEE.bf.enable_get_volume = fuelsensor.fsEE.bf.rectangular_area_consistent &&
                                               fuelsensor.fsEE.bf.lengthsTankdepth_consistent &&
                                               fuelsensor.fsEE.bf.full_calib && fuelsensor.fsEE.bf.zero_calib;

    }
    else if (fuelsensor.fsEE.tank.type == FS_TANK_TYPE_IRREGULAR)
    {
        fuelsensor.fsEE.bf.enable_get_volume = fuelsensor.fsEE.bf.irregular_spline_nodetable_consistent &&
                                               fuelsensor.fsEE.bf.lengthsTankdepth_consistent &&
                                               fuelsensor.fsEE.bf.full_calib && fuelsensor.fsEE.bf.zero_calib;

    }
    //-------------
    if (!fuelsensor.fsEE.bf.enable_get_volume)
    {
        fuelsensor.fsEE.outputType.type = (FS_OUTPUTTYPE)LENGTH;

        if (!fuelsensor.fsEE.bf.lengthsTankdepth_consistent)
        {
            fuelsensor.fsEE.outputType.length.units = FS_UNITMEA_LENGTH_PERCENTAGE;
        }
    }
    //fuelsensor_savedata();
}

void fuelsensor_job(void)
{
    static unsigned long last_millis=0;

    float fuelsensor_value=0.0f;//format value

    float h;
    float Hx;

    if ( (millis()-  last_millis) > 1000)
    {
        last_millis  = millis();

        if ( (fuelsensor.fsEE.bf.full_calib) && (fuelsensor.fsEE.bf.zero_calib) )
        {
            if (jt606_send_readFuelLevel())
            {
                //
                //x defect. length of units = Centimeters
                h = fuelsensor.fsEE.calib.tank.zero2full * (jt606.internalSensor.percentage/100);
                Hx = fuelsensor.fsEE.calib.tank.innertank - ( (fuelsensor.fsEE.calib.tank.zero2full + fuelsensor.fsEE.calib.tank.full2upper) - h);
                //

                if (fuelsensor.fsEE.outputType.type == (FS_OUTPUTTYPE)LENGTH)
                {
                    if (fuelsensor.fsEE.outputType.length.units == FS_UNITMEA_LENGTH_PERCENTAGE)
                    {
                        fuelsensor_value = jt606.internalSensor.percentage;
                    }
                    else
                    {
                        if (fuelsensor.fsEE.outputType.length.units == FS_UNITMEA_LENGTH_CENTIMETERS)
                        {
                            fuelsensor_value = Hx;
                        }
                        else if (fuelsensor.fsEE.outputType.length.units == FS_UNITMEA_LENGTH_METERS)
                        {
                            Hx /= 100.0;
                            fuelsensor_value = Hx;
                        }
                    }
                }
                else if (fuelsensor.fsEE.outputType.type == (FS_OUTPUTTYPE)VOLUME)
                {
                    if (fuelsensor.fsEE.tank.type == FS_TANK_TYPE_RECTANGULAR)
                    {
                        fuelsensor_value = Hx *fuelsensor.fsEE.tank.rectangular.area.length * fuelsensor.fsEE.tank.rectangular.area.width * 1E-6;//m3

                        if (fuelsensor.fsEE.outputType.volume.units == FS_UNITMEA_VOLUME_GALLONS)
                        {
                            fuelsensor_value *= 264.17;  //1 m3 = 264.17 gal
                        }
                        else //liters
                        {
                            fuelsensor_value *= 1000;   //1m3 = 1000l
                        }
                    }
                    else//IRREGULAR
                    {
                        //spline
                        fuelsensor_value = spline_poly_eval(Hx);

                        if (fuelsensor_value >= 0)
                        {
                            if (fuelsensor.fsEE.outputType.volume.units == FS_UNITMEA_VOLUME_GALLONS)
                            {
                                if (fuelsensor.fsEE.tank.irregular.spline.node_units.volume.units == FS_UNITMEA_VOLUME_LITERS)
                                {
                                    fuelsensor_value *= 0.26417;//liters to gallons
                                }
                            }
                            else //liters
                            {
                                if (fuelsensor.fsEE.tank.irregular.spline.node_units.volume.units == FS_UNITMEA_VOLUME_GALLONS)
                                {
                                    fuelsensor_value *= 3.79;//gal to liters
                                }
                            }
                        }
                    }
                }
                //

            }
        }
        else
        {
            fuelsensor_value = -2;
        }
        fuelsensor.value = fuelsensor_value;
    }
}


String fuelsensor_reading(void)
{
    //-------
    String unit;
    if (fuelsensor.fsEE.outputType.type == (FS_OUTPUTTYPE)LENGTH)
    {
        if (fuelsensor.fsEE.outputType.length.units == FS_UNITMEA_LENGTH_PERCENTAGE)
            unit = " %";
        else if (fuelsensor.fsEE.outputType.length.units == FS_UNITMEA_LENGTH_CENTIMETERS)
                unit = " cm";
        else if (fuelsensor.fsEE.outputType.length.units == FS_UNITMEA_LENGTH_METERS)
                unit = " m";
    }
    else if (fuelsensor.fsEE.outputType.type == (FS_OUTPUTTYPE)VOLUME)
    {
        if (fuelsensor.fsEE.outputType.volume.units == FS_UNITMEA_VOLUME_GALLONS)
            unit = " gal";
        else //liters
            unit = " L";
    }
    String msg_fuelsensor_reading;
    if (fuelsensor.value >= 0)
    {
        msg_fuelsensor_reading = "Reading: "+ String(fuelsensor.value) + unit;
    }
    else
    {
        if (fuelsensor.value == -1)
        {
            msg_fuelsensor_reading = "Reading: "+ String("OUT LIMITS");//OUT OF LIMITS
        }
        else if (fuelsensor.value == -2)
        {
            msg_fuelsensor_reading = "Reading: "+ String("-NO CALIB-");
        }
        else
        {
           msg_fuelsensor_reading = "Reading: "+ String("-NEGATIVE-");
        }
    }
    return msg_fuelsensor_reading;
}
