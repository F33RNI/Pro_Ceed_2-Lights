/*
 * Licensed under the Unlicense License;
 *
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      https://unlicense.org
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */


/**************************************/
/*            Colors setup            */
/**************************************/
// {R, G} 0 to 255
// NOTE: If the channel is on a non-PWM pin, the color cannot be changed. Only 2 states - on(>0) / off(=0)
// Turns
const uint8_t COLOR_TURN[2] = {255, 255};
// Brake
const uint8_t COLOR_BRAKE[2] = {255, 0};
// Emergency signal
const uint8_t COLOR_EMRGN[2] = {255, 0};
// Autopilot mode
const uint8_t COLOR_APILOT[2] = {0, 255};


/************************************/
/*            Time setup            */
/************************************/
// After this time in ms, the counter will be increase
const uint16_t TIME_CONSTANT = 500;


/************************************/
/*            Pins setup            */
/************************************/
// 1st (left) LED strip pins (r,g channels)
const uint8_t STRIP_LEFT_RED_PIN = 3;
const uint8_t STRIP_LEFT_GREEN_PIN = 2;

// 2nd (right) LED strip pins (r,g channels)
const uint8_t STRIP_RIGHT_RED_PIN = 5;
const uint8_t STRIP_RIGHT_GREEN_PIN = 4;

// Buttons pins
// Left turn
const uint8_t BUTTON_L_TURN_PIN = 8;
// Right turn
const uint8_t BUTTON_R_TURN_PIN = 9;
// Brake
const uint8_t BUTTON_BRAKE_PIN = 10;
// Emergency signal
const uint8_t BUTTON_EMRGN_PIN = 11;
// Autopilot mode
const uint8_t BUTTON_APILOT_PIN = 12;


// System variables
// mode: 0 - nothing to do, 1 - left turn, 2 - right turn, 3 - brake, 4 - emergency, 5 - autopilot
uint8_t mode;
// 3 steps (for turn signal:  10 / 11 / 00) or 2 steps (blinking signal:  >0 / =0)
uint8_t position_counter;
// Stores millis() time for increment position_counter
uint64_t cycle_timer;


void setup()
{
    // Pins initialization
    pinMode(STRIP_LEFT_GREEN_PIN, OUTPUT);
    pinMode(STRIP_LEFT_RED_PIN, OUTPUT);
    pinMode(STRIP_RIGHT_GREEN_PIN, OUTPUT);
    pinMode(STRIP_RIGHT_RED_PIN, OUTPUT);
    pinMode(BUTTON_L_TURN_PIN, INPUT_PULLUP);
    pinMode(BUTTON_R_TURN_PIN, INPUT_PULLUP);
    pinMode(BUTTON_BRAKE_PIN, INPUT_PULLUP);
    pinMode(BUTTON_EMRGN_PIN, INPUT_PULLUP);
    pinMode(BUTTON_APILOT_PIN, INPUT_PULLUP);
}

void loop()
{
    // Scan for buttons
    buttons_scanner();

    // Show current effect
    effects_handler();

    // Increment position_counter
    if (millis() - cycle_timer >= TIME_CONSTANT) {
        // 2 steps for brake, emergency and autopilot
        // 3 steps for nothing to do, left turn, and right turn
        if ((mode >= 3 && position_counter < 1) || position_counter < 2)
            position_counter++;
        else
            position_counter = 0;

        // Reset timer
        cycle_timer = millis();
    }
}

void effects_handler() {
    switch (mode)
    {
    case 1:
        // 1 - Left turn
        if (position_counter == 1) {
            // Step 1 - turn on right strip
            digitalWrite(STRIP_LEFT_RED_PIN, 0);
            digitalWrite(STRIP_LEFT_GREEN_PIN, 0);
            analogWrite(STRIP_RIGHT_RED_PIN, COLOR_TURN[0]);
            analogWrite(STRIP_RIGHT_GREEN_PIN, COLOR_TURN[1]);
        }
        else if (position_counter == 2) {
            // Step 2 - turn on left strip
            analogWrite(STRIP_LEFT_RED_PIN, COLOR_TURN[0]);
            analogWrite(STRIP_LEFT_GREEN_PIN, COLOR_TURN[1]);
            digitalWrite(STRIP_RIGHT_RED_PIN, 0);
            digitalWrite(STRIP_RIGHT_GREEN_PIN, 0);
        }
        else {
            // Step 3 - turn off both strips
            digitalWrite(STRIP_LEFT_RED_PIN, 0);
            digitalWrite(STRIP_LEFT_GREEN_PIN, 0);
            digitalWrite(STRIP_RIGHT_RED_PIN, 0);
            digitalWrite(STRIP_RIGHT_GREEN_PIN, 0);
        }
        break;

    case 2:
        // 2 - Right turn
        if (position_counter == 1) {
            // Step 1 - turn on left strip
            analogWrite(STRIP_LEFT_RED_PIN, COLOR_TURN[0]);
            analogWrite(STRIP_LEFT_GREEN_PIN, COLOR_TURN[1]);
            digitalWrite(STRIP_RIGHT_RED_PIN, 0);
            digitalWrite(STRIP_RIGHT_GREEN_PIN, 0);
        }
        else if (position_counter == 2) {
            // Step 2 - turn on right strip
            digitalWrite(STRIP_LEFT_RED_PIN, 0);
            digitalWrite(STRIP_LEFT_GREEN_PIN, 0);
            analogWrite(STRIP_RIGHT_RED_PIN, COLOR_TURN[0]);
            analogWrite(STRIP_RIGHT_GREEN_PIN, COLOR_TURN[1]);
        }
        else {
            // Step 3 - turn off both strips
            digitalWrite(STRIP_LEFT_RED_PIN, 0);
            digitalWrite(STRIP_LEFT_GREEN_PIN, 0);
            digitalWrite(STRIP_RIGHT_RED_PIN, 0);
            digitalWrite(STRIP_RIGHT_GREEN_PIN, 0);
        }
        break;

    case 3:
        // 3 - Brake
        analogWrite(STRIP_LEFT_RED_PIN, COLOR_BRAKE[0]);
        analogWrite(STRIP_LEFT_GREEN_PIN, COLOR_BRAKE[1]);
        analogWrite(STRIP_RIGHT_RED_PIN, COLOR_BRAKE[0]);
        analogWrite(STRIP_RIGHT_GREEN_PIN, COLOR_BRAKE[1]);
        break;

    case 4:
        // 4 - Emergency
        if (position_counter > 0) {
            // Step 1 - turn both strips on
            analogWrite(STRIP_LEFT_RED_PIN, COLOR_EMRGN[0]);
            analogWrite(STRIP_LEFT_GREEN_PIN, COLOR_EMRGN[1]);
            analogWrite(STRIP_RIGHT_RED_PIN, COLOR_EMRGN[0]);
            analogWrite(STRIP_RIGHT_GREEN_PIN, COLOR_EMRGN[1]);
        }
        else {
            // Step 2 - turn both strips off
            digitalWrite(STRIP_LEFT_RED_PIN, 0);
            digitalWrite(STRIP_LEFT_GREEN_PIN, 0);
            digitalWrite(STRIP_RIGHT_RED_PIN, 0);
            digitalWrite(STRIP_RIGHT_GREEN_PIN, 0);
        }
        break;

    case 5:
        // 5 - Autopilot
        if (position_counter > 0) {
            // Step 1 - turn both strips on
            analogWrite(STRIP_LEFT_RED_PIN, COLOR_APILOT[0]);
            analogWrite(STRIP_LEFT_GREEN_PIN, COLOR_APILOT[1]);
            analogWrite(STRIP_RIGHT_RED_PIN, COLOR_APILOT[0]);
            analogWrite(STRIP_RIGHT_GREEN_PIN, COLOR_APILOT[1]);
        }
        else {
            // Step 2 - turn both strips off
            digitalWrite(STRIP_LEFT_RED_PIN, 0);
            digitalWrite(STRIP_LEFT_GREEN_PIN, 0);
            digitalWrite(STRIP_RIGHT_RED_PIN, 0);
            digitalWrite(STRIP_RIGHT_GREEN_PIN, 0);
        }
        break;

    default:
        // 0 - Nothing to do
        digitalWrite(STRIP_LEFT_RED_PIN, 0);
        digitalWrite(STRIP_LEFT_GREEN_PIN, 0);
        digitalWrite(STRIP_RIGHT_RED_PIN, 0);
        digitalWrite(STRIP_RIGHT_GREEN_PIN, 0);
        break;
    }
}

void buttons_scanner() {
    if (button_emrgn_pressed())
        // Firstly, check the emergency switch, because it has the highest priority
        mode = 4;
    else if (button_brake_pressed())
        // Check the brake switch (second priority)
        mode = 3;
    else if (button_l_turn_pressed())
        // Check the left turn switch (third priority)
        mode = 1;
    else if (button_r_turn_pressed())
        // Check the right turn switch (third priority)
        mode = 2;
    else if (button_apilot_pressed())
        // Check the autopilot switch (lowest priority)
        mode = 5;
    else
        // If no switches are turned on
        mode = 0;
    
}

boolean button_l_turn_pressed() {
    // Return the inverse value, since pin is pulled up to VCC (HIGH - the button is not pressed, LOW - is pressed)
    return !digitalRead(BUTTON_L_TURN_PIN);
}

boolean button_r_turn_pressed() {
    // Return the inverse value, since pin is pulled up to VCC (HIGH - the button is not pressed, LOW - is pressed)
    return !digitalRead(BUTTON_R_TURN_PIN);
}

boolean button_brake_pressed() {
    // Return the inverse value, since pin is pulled up to VCC (HIGH - the button is not pressed, LOW - is pressed)
    return !digitalRead(BUTTON_BRAKE_PIN);
}

boolean button_emrgn_pressed() {
    // Return the inverse value, since pin is pulled up to VCC (HIGH - the button is not pressed, LOW - is pressed)
    return !digitalRead(BUTTON_EMRGN_PIN);
}

boolean button_apilot_pressed() {
    // Return the inverse value, since pin is pulled up to VCC (HIGH - the button is not pressed, LOW - is pressed)
    return !digitalRead(BUTTON_APILOT_PIN);
}
