#include <iostream>
#include <algorithm>

#include "mbed.h"

using namespace std;

Serial pc( USBTX, USBRX );
AnalogOut Aout(DAC0_OUT);
AnalogIn Ain(A0);

DigitalIn  Switch(SW3);
DigitalOut redLED(LED1);
DigitalOut greenLED(LED2);

BusOut display(D6, D7, D9, D10, D11, D5, D4, D8);
char table[11] = {0x3F, 0x06, 0x5B, 0x4F, 0x66, 0x6D, 0x7D, 0x07, 0x7F, 0x6F, 0x00};

float sine_freq = 100;         // frequency of sine wave
float sample_period = 0.0001; // period of sampling

double data[10000] = {0}; // store input data

int k = 0;
int sample = 10000;
float ADCdata[10000];


int main()
{
    
    while(1){
        
        
        float step = 2 * sample_period * sine_freq; // step of sine wave

        int i = 0;      // index of loop
        int count0 = 0; // counter of 0
        int count1 = 0; // counter of 1

        int freq = 0;
        int digit[3] = {0}; // store 3 digit freq

        for (i = 0; i < 10000; i++) {                    
            float j = i*step;

            Aout = 0.5 + 0.5 * sin(j*3.14159);
            wait(sample_period);

            data[i] = Ain;
        }

        double max = *max_element(data, data+10000);
        double min = *min_element(data, data+10000);

        // find the first "1" input
        for (i = 1; (data[i] == max) ; i++)
        {
        }
        // then count for 1 and 0
        for (i++, count1++; (data[i] == max) ; i++)
        {
            if (data[i] == max)
                count1++;
            if (data[i] == min)
                count0++;   
        }

        // calculate sine wave frequency and seperate each digit
        freq = 1 / (count0 + count1) * 100;
        // pc.printf("%d\r\n", freq);
        digit[1] = (freq - freq % 100) / 100;
        digit[2] = (freq - digit[1] * 100 - freq % 10 ) / 10 ;
        digit[3] = (freq - digit[1] * 100 - digit[2] * 10);
        
        if (Switch == 0){
        
            redLED = 0;
            greenLED = 1;

            // show on seven-segment display
            display = table[digit[1]]; 
            wait(1);
            display = table[digit[2]];
            wait(1);
            display = 0x80 | table[digit[3]];// number with decimal point
            wait(1);
        }
        else{

            redLED = 1;
            greenLED = 0;
            display = table[10];

        }

        for (k = 0; k < sample; k++){

            Aout = Ain;

            ADCdata[k] = Ain;

            wait(1./sample);

        }

        for (k = 0; k < sample; k++){

            pc.printf("%1.3f\r\n", ADCdata[k]);

            wait(0.1);

        }

        int m = 0;
        float freq_cal = 2 * sample_period * freq;

        for (m = 0; m < 10000; m++) {                    
            float n = m*freq_cal;

            Aout = 0.5 + 0.5 * sin(n*3.14159);
            wait(sample_period);
        }

    }
        
}
