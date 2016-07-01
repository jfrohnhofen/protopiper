/*connect only the extrusion servo with factory settings and run this code
*the servo led should turn first pink then green after 2 seconds
*/

#include "Dynamixel.h"

#define servo_old_id 254 //broadcast
#define servo_new_id 2
#define servo_direction_pin 2

Dynamixel servo(servo_old_id, servo_direction_pin);

void setup() {
   //pink led on
    servo.write(Dynamixel::LED, Dynamixel::PINK);
    delay(500);
    //update ID
    servo.write(Dynamixel::ID, servo_new_id);
    delay(1000);
    //redefine servo
    Dynamixel servo(servo_new_id, servo_direction_pin);
    delay(100);
    //setup WHEEL or JOINT mode
    servo.write(Dynamixel::CONTROL_MODE, Dynamixel::JOINT);
    delay(1000);
    //green led on
    servo.write(Dynamixel::LED, Dynamixel::GREEN);
}

void loop() {
    
}



