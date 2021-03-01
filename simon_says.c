#include <wiringPi.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <softTone.h>

/*function declarations*/
 
int playRound(int,int[]);
int gameOver(int, int[], int, int);
int winner(void);
int buttonPress(void);


/*define GPIO pins for more readability*/

#define blue_led 0
#define green_led 1
#define yellow_led 2
#define red_led 3

#define blue_in 4
#define green_in 5
#define yellow_in 6
#define red_in 23 

#define buzzer 22





int main(void){

	/*stop the program if there's anything awry with GPIO setup*/
	if (wiringPiSetup() == -1){
		printf("GPIO setup failed\n");
		return(-1);
	}
	if(softToneCreate(buzzer)<0){
		printf("buzzer setup failed!");
		return(-1);	
	}


	/*final setup touches*/
	for(int i=0;i<4;i++){
		pinMode(i,OUTPUT);	//LED pins are outputs
		digitalWrite(i, HIGH);	//make sure LEDs are off
	}

	for(int i=4;i<7;i++){
		pinMode(i,INPUT);	//buttons are inputs
	}
	pinMode(23, INPUT);		//last button is also an input
	
	
	srand(time(NULL));		//seed the random number generation
	
	int gameValues [10];		//generate the game sequence
        for (int i = 0; i<10;i++){
		gameValues[i] = (rand()%4);	//which LED is illuminated is determined by the integer stored in gameValues
						   //0 -> blue
						   //1 -> green
						   //2 -> yellow
						   //3 -> red

	}

	playRound(1, gameValues);	//play the first round!
}
/*
* playRound is a recursively defined function. it calls the other functions that make the game work.
* roundNum is the number of the round. starts at 1 and goes up to 10
* gameValues is the game sequence. 
*/
int playRound(int roundNum, int gameValues[]){

	if(roundNum <= 10){						//roundNum>10 would mean 10 rounds have been played, and the player has won
		delay(1500);						//allows some breathing room between rounds

		for (int i=0;i<roundNum;i++){				//This loop flashes the LEDs and buzzes the buzzer in sequence up to the round number.
			digitalWrite(gameValues[i], LOW);		   // >100*gameValues[i]+1 is some math done so that each LED has its own buzzer tone
			softToneWrite(buzzer, 100*(gameValues[i]+1));      // >light and buzzer are on for half a second, then off for half a second before moving to the
			delay(500);					   //  LED in the sequence
			digitalWrite(gameValues[i], HIGH);
			softToneWrite(buzzer,0);
			delay(500);
		}

		int input = 0;						//declare input variable which is used for storing the user input

		for(int i = 0;i < roundNum;i++){			//This loop is for reading the user's input
			input = buttonPress();				//buttonPress() reads the button press and converts it to an integer corresponding with an LED
			
			softToneWrite(buzzer, 100*(input+1));		//buzz the tone corresponding to the LED that the user picked
			digitalWrite(input, LOW);			//flash the LED that the user picked
			delay(250);					   //these 2 actions show the user that their choice has been registered
			digitalWrite(input, HIGH);
			softToneWrite(buzzer,0);
			
			
			if(input != gameValues[i]){			//check that the input value matches the expected value. If it doesn't then call gameOver()
				printf("Incorrect\n");
				return(gameOver(roundNum, gameValues, input, i));
			}
		}
		printf("Correct\n");
		return(playRound(++roundNum, gameValues));		//plays the next round
	}
	else{return(winner());}						//if the player reached round 11, then they won. call winner().
}

/*
* buttonPress() is a function that requires no input. It is used to read the buttons pressed by the user
*/

int buttonPress(void){

	while(1){					//This loop makes sure that the user is not still holding any buttons before looking for the next input
		if(digitalRead(blue_in) != LOW &&	   //if they are, then the loop will keep looping until they are not anymore
		   digitalRead(green_in) != LOW &&
		   digitalRead(yellow_in) != LOW &&
		   digitalRead(red_in) != LOW){
			break;
		}
	}
	while(1){							//this loop is used to poll for the user's choice. If the user input is not registered, then the loop will keep looping
		if(digitalRead(blue_in) == LOW){return (0);}		   //once the user's input has been found, the buttonPress() function returns the integer corresponding with that button press
		else if(digitalRead(green_in) == LOW){return (1);}
		else if(digitalRead(yellow_in) == LOW){return(2);}
		else if(digitalRead(red_in) == LOW){return(3);}
	}


}

/*
* gameOver is called when the player inputs the wrong sequence. It prints the sequence given to the player and the player input sequence up to the point where the user lost.
* int roundNum:     the number of the round during which the player lost 
* int gameValues[]: the array of expected values
* int finalButton:  the last button that the player pressed
* finalStep:        the number of the last input the player made
*/
int gameOver(int roundNum, int gameValues[], int finalButton, int finalStep){
	printf("\tsequence required\tsequence input\n");


	char *colors[4];									//this string[] is for printing the sequences in a human-readable form
	colors[0] = "blue  ";colors[1] = "green ";colors[2]="yellow";colors[3]="red   ";	
	
	for(int i=0;i<finalStep;i++){
		int currentColor= gameValues[i];
		printf("\t%s\t\t\t%s\n",colors[currentColor], colors[currentColor]);
	}
	printf("\t%s\t\t\t%s\n", colors[gameValues[finalStep]], colors[finalButton]);
	return(0);
}

/*
* winner() is called when the player wins. it just prints congrats.
*/
int winner(void){
	printf("congratulations! you won!\n");
	return(0);
	
}


