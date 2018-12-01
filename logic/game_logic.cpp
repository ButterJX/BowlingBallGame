#include <iostream>

//program state flags
bool AIMING;
bool ROLLING;
bool STRIKING;

//pin state values
bool PINS[10];
int PINS_LEFT;
int CURRENT_PINS_HIT;

//game state values
int SCORE;
int FRAME_COUNT;
int ROLL_COUNT;
int DOUBLE_POINTS;

using namespace std;


void throw_ball(){
    cout << "Throwing ball...\n";
    AIMING = false;
    ROLLING = true;
}

void strike_pins(){
    cout << "The ball is hitting the pins...\n";
    ROLLING = false;
    STRIKING = true;
    PINS_LEFT = 0;
    //randomly hits or misses pins. in reality, we will want to iterate through each pin to check whether it has changed position or not
    for (int i = 0; i < 10; i++){
        if (PINS[i]){
            PINS[i] = rand()%2;
            if (!PINS[i]){
                CURRENT_PINS_HIT++;
            }
        }
        if (PINS[i]){
            PINS_LEFT++;
        }
    }
}

void next_turn(){
    STRIKING = false;
    AIMING = true;
    
    //for determining the next frame and roll
    if (FRAME_COUNT < 10){
        if (ROLL_COUNT == 0){
            cout << "You hit " +  to_string(CURRENT_PINS_HIT) +  " pins! " + to_string(PINS_LEFT) + " remain this frame.\n";
            ROLL_COUNT++;
        }
        else{
            cout << "You hit " +  to_string(CURRENT_PINS_HIT) +  " pins and " + to_string(10 - PINS_LEFT) + " in total this frame.\n";
            ROLL_COUNT = 0;
            for (int i = 0; i < 10; i++){ PINS[i] = true; }
            FRAME_COUNT++;
        }
    }
    else {
        if (ROLL_COUNT < 2){
            cout << "You hit " +  to_string(CURRENT_PINS_HIT) +  " pins! " + to_string(PINS_LEFT) + " remain this frame.\n";
            ROLL_COUNT++;
        }
        else{
            cout << "You hit " +  to_string(CURRENT_PINS_HIT) +  " pins and " + to_string(10 - PINS_LEFT) + " in total this frame.\n";
            ROLL_COUNT = 0;
            for (int i = 0; i < 10; i++){ PINS[i] = true; }
            FRAME_COUNT++;
        }
    }
    
    //for determining the score
    //when a strike is bowled, the next two rolls' scores are added to that frame
    //when a spare is bowled, the next roll's scores are added to that frame
    //effectively, after a strike is bowled, the next two rolls are worth double,
    //and after a spare, the next roll is worth double
    
    if (DOUBLE_POINTS){
        SCORE += 2*CURRENT_PINS_HIT;
        DOUBLE_POINTS--;
    }
    else {
        SCORE += CURRENT_PINS_HIT;
    }
    CURRENT_PINS_HIT = 0;
    
    if (PINS_LEFT = 0){
        if (ROLL_COUNT == 1){ 
            DOUBLE_POINTS = 2;
            ROLL_COUNT = 0;
            FRAME_COUNT++;
        }
        else {
            DOUBLE_POINTS = 1;
        }
    }

    cout << "\n";
}

void game(){
    while (FRAME_COUNT < 11){ 
    
    if (AIMING){
        cout << "Your score is " + to_string(SCORE) + ". Frame: " + to_string(FRAME_COUNT) + ". Roll: " + to_string(ROLL_COUNT) + "\n";
        cout << "Aiming ball, camera is facing down lane.\n";
        system("pause");
        throw_ball();
    }

    if (ROLLING){
        cout << "Ball is rolling towards pins. Camera is following ball from the upper left, slightly behind.\n";
        //if (Ball.Position < 1 meter from pins){
            strike_pins();
        //}
    }

    if (STRIKING){
        //camera overhead of pins
        //start timer for 5-10s
        cout << "Camera is overhead of the pins to witness the carnage.\n";
        next_turn();
    }
    }
    cout << "Game over. Your final score was " + to_string(SCORE) + "!\n";
}

int main(){
    //initialize values for start of game
    
    AIMING = true;
    ROLLING = false;
    STRIKING = false;
    FRAME_COUNT = 1;
    ROLL_COUNT = 0;
    for (int i = 0; i < 10; i++){ PINS[i] = true; }
    PINS_LEFT = 10;
    CURRENT_PINS_HIT = 0;
    
    cout << "Welcome to Joel's shitty command line bowling simulator!\n";
    system("pause");
    cout << "\n";

    //start loop of game
    game();
} 