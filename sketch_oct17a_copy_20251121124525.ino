/**
 *
 * @license MIT License
 *
 * Copyright (c) 2022 lewis he
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 * @file      QMI8658_GetDataExample.ino
 * @author    Lewis He (lewishe@outlook.com)
 * @date      2022-10-16
 *
 */
#include <Arduino.h>
#include <Wire.h>
#include <SPI.h>
#include "SensorQMI8658.hpp"
#ifdef ARDUINO_T_BEAM_S3_SUPREME
#include <XPowersAXP2101.tpp>   //PMU Library https://github.com/lewisxhe/XPowersLib.git
#endif


#ifndef IMU_CS
#define IMU_CS      34      // IMU CS PIN
#endif

/* SPI interface*/

#ifndef SPI_MOSI
#define SPI_MOSI   (35)
#endif

#ifndef SPI_SCK
#define SPI_SCK    (36)
#endif

#ifndef SPI_MISO
#define SPI_MISO   (37)
#endif

SensorQMI8658 qmi;

IMUdata acc;
IMUdata gyr;


void beginPower()
{
  // T_BEAM_S3_SUPREME The PMU voltage needs to be turned on to use the sensor
#if defined(ARDUINO_T_BEAM_S3_SUPREME)
    XPowersAXP2101 power;
    power.begin(Wire1, AXP2101_SLAVE_ADDRESS, 42, 41);
    power.disableALDO1();
    power.disableALDO2();
    delay(250);
    power.setALDO1Voltage(3300); power.enableALDO1();
    power.setALDO2Voltage(3300); power.enableALDO2();
#endif
}

void setup()
{
    Serial.begin(115200);
    while (!Serial);

    beginPower();

    bool ret = false;
#if defined(SPI_MOSI) && defined(SPI_SCK) && defined(SPI_MISO)
    ret = qmi.begin(SPI, IMU_CS, SPI_MOSI, SPI_MISO, SPI_SCK);
#else
    ret = qmi.begin(SPI, IMU_CS);
#endif

    if (!ret) {
        Serial.println("Failed to find QMI8658 - check your wiring!");
        while (1) {
            delay(1000);
        }
    }

    /* Get chip id*/
    Serial.print("Device ID:");
    Serial.println(qmi.getChipID(), HEX);


    if (qmi.selfTestAccel()) {
        Serial.println("Accelerometer self-test successful");
    } else {
        Serial.println("Accelerometer self-test failed!");
    }

    if (qmi.selfTestGyro()) {
        Serial.println("Gyroscope self-test successful");
    } else {
        Serial.println("Gyroscope self-test failed!");
    }


    qmi.configAccelerometer(
        SensorQMI8658::ACC_RANGE_4G,
        SensorQMI8658::ACC_ODR_1000Hz,
        SensorQMI8658::LPF_MODE_0);

    qmi.configGyroscope(
        SensorQMI8658::GYR_RANGE_64DPS,
        SensorQMI8658::GYR_ODR_896_8Hz,
        SensorQMI8658::LPF_MODE_3);

    qmi.enableGyroscope();
    qmi.enableAccelerometer();
    qmi.dumpCtrlRegister();



#if IMU_IRQ > 0
// If you want to enable interrupts, then turn on the interrupt enable
    qmi.enableINT(SensorQMI8658::INTERRUPT_PIN_1, true);
    qmi.enableINT(SensorQMI8658::INTERRUPT_PIN_2, false);
#endif

    Serial.println("Read data now...");

}


void loop()
{
    if (qmi.getDataReady()) {

        if (qmi.getAccelerometer(acc.x, acc.y, acc.z)) {

            // Print to serial plotter
            Serial.print("ACCEL.x:"); Serial.print(acc.x); Serial.print(",");
            Serial.print("ACCEL.y:"); Serial.print(acc.y); Serial.print(",");
            Serial.print("ACCEL.z:"); Serial.print(acc.z); Serial.println();

        }
        if (qmi.getGyroscope(gyr.x, gyr.y, gyr.z)) {

            // Print to serial plotter
            Serial.print("GYRO.x:"); Serial.print(gyr.x); Serial.print(",");
            Serial.print("GYRO.y:"); Serial.print(gyr.y); Serial.print(",");
            Serial.print("GYRO.z:"); Serial.print(gyr.z); Serial.println();
        }
    }
    delay(100);
}





// ***** RebelSat junkyard *****


// void setup()


// // #define USE_I2C              //Using the I2C interface

// #ifdef USE_I2C
// #ifndef SENSOR_SDA
// #define SENSOR_SDA  17
// #endif

// #ifndef SENSOR_SCL
// #define SENSOR_SCL  18
// #endif


// #endif  /* USE_I2C*/

// #ifndef IMU_CS
// #define IMU_CS      34      // IMU CS PIN
// #endif

// #ifndef IMU_IRQ
// #define IMU_IRQ     33      // IMU INT PIN
// #endif

// #ifndef OLED_SDA
// #define OLED_SDA    22      // Display Wire SDA Pin
// #endif

// #ifndef OLED_SCL
// #define OLED_SCL    21      // Display Wire SCL Pin
// #endif


    // /*
    // * If both the accelerometer and gyroscope sensors are turned on at the same time,
    // * the output frequency will be based on the gyroscope output frequency.
    // * The example configuration is 896.8HZ output frequency,
    // * so the acceleration output frequency is also limited to 896.8HZ
    // * */
    // qmi.enableGyroscope();
    // qmi.enableAccelerometer();

    // // Print register configuration information
    // qmi.dumpCtrlRegister();


// void loop()

/*
    // When the interrupt pin is passed in through setPin,
    // the GPIO will be read to see if the data is ready.
    if (qmi.getDataReady()) {
      

*/

        // Serial.print("Timestamp:");
        // Serial.print(qmi.getTimestamp());


/*
            m2/s to mg
            Serial.print(" ACCEL.x:"); Serial.print(acc.x * 1000); Serial.println(" mg");
            Serial.print(",ACCEL.y:"); Serial.print(acc.y * 1000); Serial.println(" mg");
            Serial.print(",ACCEL.z:"); Serial.print(acc.z * 1000); Serial.println(" mg");
            */

            // Serial.print(" GYRO.x:"); Serial.print(gyr.x); Serial.println(" degrees/sec");
            // Serial.print(",GYRO.y:"); Serial.print(gyr.y); Serial.println(" degrees/sec");
            // Serial.print(",GYRO.z:"); Serial.print(gyr.z); Serial.println(" degrees/sec");

        // Serial.print("Temperature:");
        // Serial.print(qmi.getTemperature_C());
        // Serial.println(" degrees C");

