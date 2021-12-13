/*
CMSC 411 Project
Authors: Deven Parmar and Kevin Lee
*/

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
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
int write_offset = 0;

void ReadFile(string fileName);
void Write(string operation[]);
void DumpMemory();
int* GetDataMem(string argument);

//passes an initialized empty 2D vector and fills it
void VectorFill(string lines[][4], int numLines, vector<vector<string>> &container);

//function that writes to csv from 2D vector
void WriteMod(vector<vector<string>> &container);




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




//fills vector, can implement logic for printing here 
void vector_fill(string lines[][4], int numLines, vector<vector<string>> &container){

//example of filling vector (no logic just prints)

int cycle = 1; //current cycle
int nextAvailable = 1; //next available cycle
string full_cycle[5] = {"IF", "ID","EX", "MEM", "WB"}; 
string spacer = "  "; //keeps equal whitespace on the left side

for(int i = 0; i < numLines; i++){
    
    int counter = 0;
    //fills whitespace on left side
    while(counter != nextAvailable){
        container[i].push_back(spacer);
        counter++;
    }
    for(int j = 0; j < 5; j++){
        container[i].push_back(full_cycle[j]);
        cycle++;
    }
    nextAvailable++;
}
}


//modified write to output csv from 2D vector
void Write_mod(vector<vector<string>> &container){
  ofstream file("output.csv");

  cout << "outputting csv file" << endl;

  for(int i = 0; i < static_cast<int>(container.size()); i++){
    for(int j = 0; j < static_cast<int>(container[i].size()); j++){
      file << container[i][j] << ",";
    }
    file << endl;
  }
}




/* ******************************************************************************
FUNCTION: void ReadFile(string fileName)

Reads File, separates commands and arguments so the "computer" can recognize them
*/
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
            lines[numLines][1] = lines[numLines][0].substr(temp + 1); //Copies everything after the space into column 1
            lines[numLines][0].erase(temp, lines[numLines][0].length() - 1); //Leaves just the command in column 0
            
            for(int i = 1; i < 3; i++){ //Loops to separate up to 3 arguments
                temp = lines[numLines][i].find(","); //Finds the first comma in Column 1
                if(temp != -1){
                    lines[numLines][i + 1] = lines[numLines][i].substr(temp + 1); //Copies everything after the comma into the next column
                    lines[numLines][i].erase(temp, lines[numLines][i].length() - 1); //Leaves a single argument in each column
                }
            }

            numLines++;
        }
    }
    file.close();

    //Critical data to take away:
    //  - lines[128][4] - Maybe make this a global variable to avoid pointers?
    //  - loopStart 


    //initialize a vector and fill it
    vector<vector<string>> container;
    for(int i = 0; i < numLines; i++){
        vector<string> vect;
        container.push_back(vect);
    }

    vector_fill(lines,numLines,container);
    Write_mod(container);

}


void Write(string operation[]){
    ofstream file ("output.csv");
    
    for(int i = 0; i < sizeof(operation); i++){
        file << operation[i];
        if(i != sizeof(operation) - 1){ //No commas at the end of the line
            file << ",";
        }
    }
    file << "\n";
    
    file.close();
}

int* GetDataMem(string argument){
    int *dataMemPtr;
    int address;
    size_t temp = argument.find("("); //Finds the first open parenthesis, denoting the end of the offset
    //Since the positions of a string start at 0, this number is also the size, in characters, of the offset
    int offset = stoi(argument, &temp);
    argument.erase(0, temp); //Erases from position 0 to the "$" of the data memory address
    size_t temp = argument.find("$");
    
    if(temp != -1){ //The address is in a register
        argument.erase(temp, 1); //Erases just the "$"
        address = stoi(argument, nullptr); 
        //There are no more numbers left in the string except for address, so no need to worry about bounds
        address = Int_Registers[address];
    }else{
        address = stoi(argument, nullptr); 
        //There are no more numbers left in the string except for address, so no need to worry about bounds
    }

    if((offset + address) >= sizeof(dataMem)){
        offset = offset % sizeof(dataMem);
        //Passes the offset through a modulus, in case the offset is larger than the dataMem size
        //This is because the data memory address rolls over when it goes over the last address
        address = address + offset;
        if(address > (sizeof(dataMem) - 1)){ //Handles the address rollover
            address = address - sizeof(dataMem);
        }
    }

    dataMemPtr = &dataMem[address];
    return dataMemPtr;
}