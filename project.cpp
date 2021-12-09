/*
CMSC 411 Project
Authors: Deven Parmar and Kevin Lee
*/

#include <iostream>
#include <fstream>
#include <string>
using namespace std;

//The cycles that each component takes will never change, so they are listed here as constants
const int FP_ADDER_CYCLES = 2; //Number of cycles for Pipelined FP adder - used in FP adds and subtracts
const int FP_MULT_CYCLES = 10; //Number of cycles for Pipelined FP multiplier
const int FP_DIVIDER_CYCLES = 40; //Number of cycles for Pipelined FP divider
const int INT_UNIT_CYCLES = 1; //Number of cycles for Loads, Stores, Stalls, Integer Adds and Subtracts

//Initialize data memory
int dataMem[] = {45,12,0,92,10,135,254,127,18,4,55,8,2,98,13,5,233,158,167};
//Initialize Floating Point Registers - set to 0 in main function
float FP_Registers[32];
//Initialize integer registers - set to 0 in main function
int Int_Registers[32];

void ReadFile(string fileName);
void Write(string operation[]);
void DumpMemory();

/* ******************************************************************************
FUNCTION: int main()

Takes command line argument for the file name, calls the ReadFile function
*/
int main (int argc, char* argv[]){
    //Set all registers equal to 0
    for(int i = 0; i < 32; i++){
        FP_Registers[i] = 0;
        Int_Registers[i] = 0;
    }


    if (argc < 2){ //Argument 1 is the program name, Argument 2 is the text file
        cout << "You are missing a data file." << endl;
        cout << "Expected usage ./project code.txt" << endl;
    }
    else{
        //Start the program - pass the file name to the readfile function
        ReadFile(argv[1]);
    }
}

void ReadFile(string fileName){
    ifstream file (fileName);
    string lines[128][4];
    int numLines = 0;
    int loopStart = -1;
    int temp = 0;

    if(file.is_open()){
        while(getline(file, lines[numLines][0])){ 
            //reads line until whitespace character, stops at end of file
            loopStart = lines[numLines][0].find("Loop:");
            if(loopStart >= 0){
                lines[numLines][0].erase(loopStart, 6); //Removes "Loop:    " from the line, to make things easier
                loopStart = numLines; //Records what line the loop starts
            }
            
            //Now take apart the string...
            temp = lines[numLines][0].find("	");
            if(temp != -1){
                lines[numLines][0].erase(temp,1); //Removes any tabs from the start
            }
            
            //Now that all whitespace & trash are gone, we can read in the commands + arguments
            //Find position of the first space character (marks the end of a command)
            temp = lines[numLines][0].find(" ");
            //This part depends on the fact that there are no empty lines in the txt file
            lines[numLines][1] = lines[numLines][0].substr(temp+1); //Copies everything after the space into column 1
            lines[numLines][0].erase(temp, lines[numLines][0].length() - 1); //Leaves just the command in column 0

            numLines++;
        }
    }
}