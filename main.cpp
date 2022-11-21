#include "mbed.h"
#include "rtos.h"
#include "uLCD_4DGL.h"
#include "SDFileSystem.h"
#include "wave_player.h"

SDFileSystem sd(p5, p6, p7, p8, "sd"); //SD card

Serial bluemod(p28,p27);
 
AnalogOut DACout(p18);
 
wave_player waver(&DACout);

PwmOut red(p23);
PwmOut green(p24);
PwmOut blue(p25);
uLCD_4DGL LCD(p9,p10,p30); // serial tx, serial rx, reset pin;

// mutex to make the lcd lib thread safe
Mutex lcd_mutex;

// Thread 1
// print counter into first line and wait for 1 s
void thread1(void const *args)
{
    int i= 0;
    while(true) {       // thread loop
        lcd_mutex.lock();
        LCD.locate(0,0);
        LCD.printf("Th1 count: %d",i);
        lcd_mutex.unlock();
        i++;
        Thread::wait(1000);
    }
}

// Thread 2
// print counter into third line and wait for 0,5s
void thread2(void const *args)
{
    int k= 0;
    while(true) {       // thread loop
        lcd_mutex.lock();
        LCD.locate(0,1);
        LCD.printf("Th2 count : %d",k);
        lcd_mutex.unlock();
        k++;
        Thread::wait(500); // wait 0.5s
    }
}
// Thread 3
// RGB LED
void thread3(void const *args)
{
    char bred=0;
    char bgreen=0;
    char bblue=0;
    while(true)
    {
    if (bluemod.getc()=='!') {
            if (bluemod.getc()=='C') { //color data packet
                bred = bluemod.getc(); // RGB color values
                bgreen = bluemod.getc();
                bblue = bluemod.getc();
                if (bluemod.getc()==char(~('!' + 'C' + bred + bgreen + bblue))) { //checksum OK?
                    red = bred/255.0; //send new color to RGB LED PWM outputs
                    green = bgreen/255.0;
                    blue = bblue/255.0;
                }
            }
        }
        Thread::wait(1);
        }
}
// Thread 4
// Speaker
void thread4(void const *args)
{
        
        FILE *wave_file;
        wave_file=fopen("/sd/mydir/Hello.wav","r");
        waver.play(wave_file);
        fclose(wave_file);
}

int main()
{
    LCD.cls();
    
    red = green = blue = 0;
    
    Thread t1(thread1); //start thread1
    Thread t2(thread2); //start thread2
    Thread t4(thread4);
    char bred=0;
    char bgreen=0;
    char bblue=0;
    while(true)
    {
        if (bluemod.readable())
        {
            if (bluemod.getc()=='!') {
                    if (bluemod.getc()=='C') { //color data packet
                        bred = bluemod.getc(); // RGB color values
                        bgreen = bluemod.getc();
                        bblue = bluemod.getc();
                        if (bluemod.getc()==char(~('!' + 'C' + bred + bgreen + bblue))) { //checksum OK?
                            red = bred/255.0; //send new color to RGB LED PWM outputs
                            green = bgreen/255.0;
                            blue = bblue/255.0;
                        }
                    }
            }
        }
        Thread::wait(100);
    }
}