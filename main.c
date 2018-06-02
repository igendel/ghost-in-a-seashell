/* Ghost in a (sea)Shell project
 * by Ido Gendel, 2018
 * Vibration-activated, slow fading in/out LED
 * Pin assignment:
 * GP2 (pin 5) Falling edge interrupt (for wake-up)
 * GP5 (pin 2) LED output
 */

// PIC12F675 Configuration Bit Settings
#pragma config FOSC = INTRCIO   // INTOSC oscillator, I/O function on GP4/5
#pragma config WDTE = OFF       // WDT disabled
#pragma config PWRTE = ON       // PWRT enabled
#pragma config MCLRE = ON       // GP3/MCLR pin function is MCLR
#pragma config BOREN = ON       // BOD enabled
#pragma config CP = OFF         // Program Memory code protection is disabled
#pragma config CPD = OFF        // Data memory code protection is disabled

#define _XTAL_FREQ 4000000UL

#include <xc.h>
#include <stdint.h>

#define PWM_CYCLE_REPEATS 16U

//=========================================================
// Set up the hardware modules
void setup(void) {
    
    // Disable Analog and Comparator
    ANSEL = 0;
    CMCON = 7;
    // Set GP5 to output (for LED), all the others remain inputs
    TRISIObits.TRISIO5 = 0;
    
    // Set up external interrupt, no Global Interrupt Enable (just wakeup)
    INTEDG = 0;
    INTE   = 1;
    
}

//=========================================================
// Run a single, blocking, software-controlled PWM cycle
void PWMCycle(uint8_t tOn) {

    uint8_t tOff = 255U - tOn;
    
    // ON, if required 
    while(tOn) {
        GPIObits.GP5 = 1;
        --tOn;
    }
    
    // OFF, if required
    while (tOff) {
        GPIObits.GP5 = 0;
        --tOff;
    }
    
}

//=========================================================
void main(void) {
    
    uint8_t d, n;
    
    setup();
    
    while (1) {
    
        __delay_ms(500);
        
        // Slow fade in
        d = 0U;
        do {
            
            for (n = 0; n < PWM_CYCLE_REPEATS; ++n) {
                PWMCycle(d);
            }    
            ++d;
            
        } while (d);
        
        __delay_ms(5000);
        
        // Slow fade out
        d = 255U;
        while (d) {
            --d;
            for (n = 0; n < PWM_CYCLE_REPEATS; ++n) {
                PWMCycle(d);
            }    
        }
        
        // Clear interrupt flag (otherwise MCU will wake up immediately)
        INTF = 0;
        SLEEP();    
        
    }
    
}
