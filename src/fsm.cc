/**
 * Alberto Li
 * Joshua Henson
 * ECE 3020, Dr. Hughes
 * 04/23/17
 * Programming Assignment #3 -- Finite State Machine Simulation
 * This Simulation was inspired by the ECE 3056 LC-3b Simulator
 */

#include <algorithm>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <list>
#include <vector>
#include <map>
#include <cmath>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <alloca.h>

using namespace std;

/***************************************************************/
/* DEFINITION OF CONSTANTS PROVIDED IN PROBLEM STATEMENT       */
/***************************************************************/

#define TRUE  1
#define FALSE 0
#define MAX_NODES 25
#define MAX_INPUT_BITS 4
#define MAX_STATE_NAME 8
#define MAX_OUTPUT_NAME 5

/***************************************************************/
/* DEFINITION OF STRUCTURES USED TO REPRESENT STATES IN FSM    */
/***************************************************************/

typedef struct Arc_Struct {
    string start_node; //node that arc starts from
    string end_node; //node that arc ends at
    string input; //input that results in state change
    string output; //only used for MEALY state machine
} Arc_Entry;

typedef struct Node_Struct {
    string name; //name of node
    string output; //ouput displayed, used only for MOORE state machine
    map<string,bool> io; //input / output string
    vector<Arc_Entry> arcs; //vector of arcs from this state
} Node_Entry;

/***************************************************************/
/* GLOBAL VARIABLES                                            */
/***************************************************************/

string MACHINE; //type of machine selected by user
int INPUT_BITS; //number of input bits selected by user
Node_Entry NODE; //data structure for node
vector<Node_Entry> NV; //vector that holds all nodes
Arc_Entry ARC; //data structure for arc

/***************************************************************/
/* GLOBAL FUNCTIONS                                            */
/***************************************************************/

/**
 * Prints help menu at beginning of simuation and when requested by user
 */
void help() {
    printf("----------------------------------------------- FSM Help ------------------------------------------------ \n");
    printf("Please enter the machine type and number of input bits as prompted upon starting simulation.              \n");
    printf("The following commands can be used to define different states and the transitions between them.           \n");
    printf("--------------------------------------------------------------------------------------------------------- \n");
    printf("NODE [name] {Mealy} || NODE [name] [output] {Moore}                    -  add a node to the graph         \n");
    printf("ARC [start] [end] [in / out] {Mealy} || ARC [start] [end] [in] {Moore} -  add arc to a node in the graph  \n");
    printf("output                                                                 -  shows output graph              \n");
    printf("?                                                                      -  display this help menu          \n");
    printf("quit                                                                   -  exit the program                \n\n");
}

/*
* Wildcard Matching, used for 'x' in input conditions
* @param first
* @param second
* @return bool whether or not first and second are the same with wildcard
*       checking
* Source(Modified): geeksforgeeks.org/wildcard-character-matching/
*/
bool match(char *first, char *second) {

    // If we reach at the end of both strings, we are done
    if (*first == '\0' && *second == '\0') {
        return true;
    }

    // Make sure that the characters after 'x' are present
    // in second string. This function assumes that the first
    // string will not contain two consecutive 'x'
    if (*first == 'x' && *(first + 1) != '\0' && *second == '\0') {
        return false;
    }

    // If the first string contains '?', or current characters
    // of both strings match
    if (*first == '?' || *first == *second) {
        return match(first + 1, second + 1);
    }

    // If there is 'x', then there are two possibilities
    // a) We consider current character of second string
    // b) We ignore current character of second string.
    if (*first == 'x') {
        return match(first + 1, second) || match(first, second + 1);
    }

    return false;
}

/*
* Compares the name fields of two nodes, used for sorting alpahbetically
* @param a
* @param b
* @returns true if a comes before b alphabetically
*/
bool alphabetic(const Node_Entry &a, const Node_Entry &b) {
    return a.name < b.name;
}

/**
 * Changes the case of a given string to upper case
 * @param  str
 * @return transformed string
 */
string toUpper(const string& str) {
    string result;
    locale loc;
    for (unsigned int i = 0; i < str.length(); ++i) {
        result += toupper(str.at(i), loc);
    }
    return result;
}

/**
 * Modifies iterator to point to the NODE specifide by @name
 * @param  it
 * @param  name
 * @return true if the NODE is inside the graph
 */
bool locate(vector<Node_Entry>::iterator& it, string name) {
    for (it = NV.begin(); it < NV.end(); ++it) {
        if ((*it).name == name) {
            return true;
        }
    }
    return false;
}

/*
* Adds arc to an existing node in a Mealy Machine
* @param startNode
* @param endNode
* @param input
* @param output
*/
void addArc(string startNode, string endNode, string input, string output) {
    vector<Node_Entry>::iterator vit;
    locate(vit,startNode); // moves vector_iterator to startNode
    vector<Arc_Entry>::iterator ait = (*vit).arcs.begin();
    string temp1;

    // Update Arc States
    ARC.start_node = startNode;
    ARC.end_node = endNode;
    ARC.input = input;
    ARC.output = output;

    // Error Check input
    if ((input.size() != INPUT_BITS)) {
        cout << input.size() << " " << INPUT_BITS;
        cout << "invalid input, please specify an input using 0,1,x of " << INPUT_BITS << " bits!" << endl;
        return;
    }
    if (output.size() > MAX_OUTPUT_NAME) {
        cout << "invalid output, please specify an output of at most " << MAX_OUTPUT_NAME << " characters!" << endl;
        return;
    }

    // Update Possible Arcs
    map<string, bool>::iterator it;
    for (it = (*vit).io.begin(); it != (*vit).io.end(); ++it) {
        if (match(strdup(input.c_str()),
            strdup((it->first).c_str())) && it->second) { // Check for Adding same node
            cout << "Invalid action, this Arc has already been added!" << endl;
            return;
        }

        if (match(strdup(input.c_str()), strdup((it->first).c_str()))) {
            it->second = true;
        }
    }

    // Add Arc to designated node
    (*vit).arcs.push_back(ARC);
}

/*
* Adds arc to an existing node in a Moore Machine
* @param startNode
* @param endNode
* @param input
*/
void addArc(string startNode, string endNode, string input) {
    vector<Node_Entry>::iterator vit;
    locate(vit,startNode); // moves vector_iterator to startNode
    vector<Arc_Entry>::iterator ait = (*vit).arcs.begin();
    string temp1;

    // Update Arc States
    ARC.start_node = startNode;
    ARC.end_node = endNode;
    ARC.input = input;

    // Error Check input
    if ((input.size() != INPUT_BITS)) {
        cout << input.size() << " " << INPUT_BITS;
        cout << "invalid input, please specify an input using 0,1,x of " << INPUT_BITS << " bits!" << endl;
        return;
    }

    // Update Possible Arcs
    map<string, bool>::iterator it;
    for (it = (*vit).io.begin(); it != (*vit).io.end(); ++it) { //arcs
        if (match(strdup(input.c_str()),
            strdup((it->first).c_str())) && it->second) { // Check for Adding same node
            cout << "Invalid action, this Arc has already been added!" << endl;
            return;
        }

        if (match(strdup(input.c_str()), strdup((it->first).c_str()))) {
            it->second = true;
        }
    }

    // Add Arc to designated node
    (*vit).arcs.push_back(ARC);
}


/*
* Adds node graph in a Mealy Machine
* @param name
* @return true if node can be added to graph
*/
bool addNode(const string name) {

    // Make sure Nodes in graph does not exceed MAX_NODES def
    if (NV.size() >= 25) {
        cout << "Cannot add to graph. Your graph is already at a max capacity of "
        << MAX_NODES << " nodes!" << endl;
        return false;
    }

    // Iterate through vector to check if is node is already in graph
    bool inside = FALSE;
    for (int i = 0; i < NV.size(); ++i) {
        if(NV[i].name == name) {
            inside = TRUE;
        }
    }

    // Add node to graph
    if (inside) { // Already in graph, do not add
        cout << name << " is already in the graph! It cannot be added again"
        << endl;
        return false;
    } else { // Not in graph, add the node!
        NODE.name = name;

        switch (INPUT_BITS) { // Initialize possible input combos to false
            case 1:
                NODE.io.insert(make_pair("0",false));
                NODE.io.insert(make_pair("1",false));
                break;
            case 2:
                NODE.io.insert(make_pair("00",false));
                NODE.io.insert(make_pair("01",false));
                NODE.io.insert(make_pair("10",false));
                NODE.io.insert(make_pair("11",false));
                break;
            case 3:
                NODE.io.insert(make_pair("000",false));
                NODE.io.insert(make_pair("001",false));
                NODE.io.insert(make_pair("010",false));
                NODE.io.insert(make_pair("011",false));
                NODE.io.insert(make_pair("100",false));
                NODE.io.insert(make_pair("101",false));
                NODE.io.insert(make_pair("110",false));
                NODE.io.insert(make_pair("111",false));
                break;
            case 4:
                NODE.io.insert(make_pair("0000",false));
                NODE.io.insert(make_pair("0001",false));
                NODE.io.insert(make_pair("0010",false));
                NODE.io.insert(make_pair("0011",false));
                NODE.io.insert(make_pair("0100",false));
                NODE.io.insert(make_pair("0101",false));
                NODE.io.insert(make_pair("0110",false));
                NODE.io.insert(make_pair("0111",false));
                NODE.io.insert(make_pair("1000",false));
                NODE.io.insert(make_pair("1001",false));
                NODE.io.insert(make_pair("1010",false));
                NODE.io.insert(make_pair("1011",false));
                NODE.io.insert(make_pair("1100",false));
                NODE.io.insert(make_pair("1101",false));
                NODE.io.insert(make_pair("1110",false));
                NODE.io.insert(make_pair("1111",false));
                break;
        }
        NV.push_back(NODE);
        sort(NV.begin(), NV.end(), alphabetic);
        return true;
    }
}

/*
* Adds node to graph in a Moore Machine
* @param name
* @param output
* @return true if node can be added to graph
*/
bool addNode(const string name, const string output) {
    // Make sure Nodes in graph does not exceed MAX_NODES def
    if (NV.size() >= 25) {
        cout << "Cannot add to graph. Your graph is already at a max capacity of "
                << MAX_NODES << " nodes!" << endl;
        return false;
    }

    // Iterate through vector to check if is node is already in graph
    bool inside = FALSE;
    for (int i = 0; i < NV.size(); ++i) {
        if(NV[i].name == name) {
            inside = TRUE;
        }
    }

    // Add node to graph
    if (inside) { // Already in graph, do not add
        cout << name << " is already in the graph! It cannot be added again." << endl;
        return false;
    } else { // Not in graph, add the node!
        NODE.name = name;
        NODE.output = output;
        switch (INPUT_BITS) { // Initialize possible input combos to false
            case 1:
                NODE.io.insert(make_pair("0",false));
                NODE.io.insert(make_pair("1",false));
                break;
            case 2:
                NODE.io.insert(make_pair("00",false));
                NODE.io.insert(make_pair("01",false));
                NODE.io.insert(make_pair("10",false));
                NODE.io.insert(make_pair("11",false));
                break;
            case 3:
                NODE.io.insert(make_pair("000",false));
                NODE.io.insert(make_pair("001",false));
                NODE.io.insert(make_pair("010",false));
                NODE.io.insert(make_pair("011",false));
                NODE.io.insert(make_pair("100",false));
                NODE.io.insert(make_pair("101",false));
                NODE.io.insert(make_pair("110",false));
                NODE.io.insert(make_pair("111",false));
                break;
            case 4:
                NODE.io.insert(make_pair("0000",false));
                NODE.io.insert(make_pair("0001",false));
                NODE.io.insert(make_pair("0010",false));
                NODE.io.insert(make_pair("0011",false));
                NODE.io.insert(make_pair("0100",false));
                NODE.io.insert(make_pair("0101",false));
                NODE.io.insert(make_pair("0110",false));
                NODE.io.insert(make_pair("0111",false));
                NODE.io.insert(make_pair("1000",false));
                NODE.io.insert(make_pair("1001",false));
                NODE.io.insert(make_pair("1010",false));
                NODE.io.insert(make_pair("1011",false));
                NODE.io.insert(make_pair("1100",false));
                NODE.io.insert(make_pair("1101",false));
                NODE.io.insert(make_pair("1110",false));
                NODE.io.insert(make_pair("1111",false));
                break;
        }
        NV.push_back(NODE);
        sort(NV.begin(), NV.end(), alphabetic);
        return true;
    }
}

/*
* Prints all nodes with their associated arcs
*/
void printOutput() {
    map<string, bool>::iterator it;

    if (MACHINE == "MEALY") {
        cout << "Output GRAPH:" << endl;
        for (int i = 0; i < NV.size(); ++i) { //iterate through nodes
            cout << NV[i].name << endl;
            for (int j = 0; j < NV[i].arcs.size(); ++j) { //iterate through arcs
                cout << "\t";
                cout << NV[i].arcs[j].end_node << " " <<NV[i].arcs[j].input << " / "
                << NV[i].arcs[j].output << endl;
            }

            // Check unspecified inputs
            for (it = NV[i].io.begin(); it != NV[i].io.end(); ++it) {
                if (!(it->second)) {
                    cout << "\t%% warning: input " << it->first <<
                    " not specified %%" << endl;
                }
            }
        }

        cout << endl;
    } else if (MACHINE == "MOORE") {
        cout << "Output GRAPH:" << endl;
        for (int i = 0; i < NV.size(); ++i) { //iterate through nodes
            cout << NV[i].name << " / " << NV[i].output << endl;
            for (int j = 0; j < NV[i].arcs.size(); ++j) { //iterate through arcs
                cout << "\t";
                cout << NV[i].arcs[j].end_node << " " <<NV[i].arcs[j].input
                << endl;
            }

            // Check unspecified inputs
            for (it = NV[i].io.begin(); it != NV[i].io.end(); ++it) {
                if (!(it->second)) {
                    cout << "\t%% warning: input " << it->first <<
                    " not specified %%" << endl;
                }
            }
        }
        cout << endl;
    }
}

/*
* Prints all nodes and their associated arcs in a state transition table
*/
void printGraph() {
    if (MACHINE == "MEALY") { // for mealy machines
        if (NV.empty()) {
            cout << "Please add nodes to the graph before using (o)utput" << endl;
            return;
        }
        vector<Node_Entry>::iterator vit;
        vector<Arc_Entry>::iterator ait;

        // Inputs Line
        map<string, bool>::iterator it;
        cout << left;
        cout << "Current\t|\tNext State / Output" << endl;
        cout << "State\t| ";
        for (it = NV[0].io.begin(); it != NV[0].io.end(); ++it) {
            cout << setw(12) << ("X = " + it->first);
        }

        cout << "\n--------";
        for (int i = 0; i < pow(2, INPUT_BITS); i++) {
            cout << "------------";
        }
        cout << endl;

        bool amatch = false;

        // EndNode / Output
        for (vit = NV.begin(); vit < NV.end(); ++vit) { //nodes
                cout << (*vit).name << "\t| ";
            for (it = NV[0].io.begin(); it != NV[0].io.end(); ++it) { // states
                for (ait = (*vit).arcs.begin(); ait != (*vit).arcs.end(); ++ait) { //arcs
                    if (match(strdup(((*ait).input).c_str()), strdup((it->first).c_str()))) { //input vs. arc
                        cout << setw(12) << ((*ait).end_node + "/" + (*ait).output);
                        amatch = true;
                        break;
                    } else {
                        amatch = false;
                    }
                }
                if (!amatch) { //input vs. arc
                    cout << setw(12) << "x/x";
                }
                amatch = false;
            }
                cout << endl;
        }
    } else if (MACHINE == "MOORE") { // for moore machines
        if (NV.empty()) {
            cout << "Please add nodes to the graph before using (o)utput" << endl;
            return;
        }
        vector<Node_Entry>::iterator vit;
        vector<Arc_Entry>::iterator ait;

        // Inputs Line
        map<string, bool>::iterator it;
        cout << left;
        cout << "Current\t|\tNext State / Output" << endl;
        cout << "State\t| ";
        for (it = NV[0].io.begin(); it != NV[0].io.end(); ++it) {
            cout << setw(12) << ("X = " + it->first);
        }

        cout << "\n--------";
        for (int i = 0; i < pow(2, INPUT_BITS); i++) {
            cout << "------------";
        }
        cout << endl;

        bool amatch = false;

        // EndNode / Output
        for (vit = NV.begin(); vit < NV.end(); ++vit) { //nodes
                cout << (*vit).name << "\t| ";
            for (it = NV[0].io.begin(); it != NV[0].io.end(); ++it) { // states
                for (ait = (*vit).arcs.begin(); ait != (*vit).arcs.end(); ++ait) { //arcs
                    if (match(strdup(((*ait).input).c_str()), strdup((it->first).c_str()))) { //input vs. arc
                        cout << setw(12) << ((*ait).end_node + "/" + (*vit).output);
                        amatch = true;
                        break;
                    } else {
                        amatch = false;
                    }
                }
                if (!amatch) { //input vs. arc
                    cout << setw(12) << ("x/" + (*vit).output);
                }
                amatch = false;
            }
            cout << endl;
        }
    }
}

/*
* Prompts the user for the type of machine and the number of input bits they
*       would like to use
*/
void initialize() {

    do {
        cout << "FSM-SIM> Please specify simulation type. Enter MEALY or MOORE: ";
        cin >> MACHINE;
        MACHINE = toUpper(MACHINE);
    } while(!(toUpper(MACHINE) == "MEALY" || toUpper(MACHINE) == "MOORE"));

    char buffer[10];

    do {
        cout << "FSM-SIM> Please specify the number of input bits (1-4): ";
        cin >> buffer;
        INPUT_BITS = (int) buffer[0] - '0';
    } while(!(INPUT_BITS >= 1 && INPUT_BITS <= 4));
}

/*
* Read command from user input
*/
void get_command() {
    // User Inputs
    char buffer[20];
    string name;
    string startNode;
    string endNode;
    string input;
    string forwardslash;
    string output;

    vector<Node_Entry>::iterator it;
    printf("FSM-SIM> ");
    cin >> buffer;

    switch(buffer[0]) {

        // Add NODE
        case 'N':
        case 'n':
        if (MACHINE == "MEALY") {
            cin >> name;
            if (name.size() > MAX_STATE_NAME) {
                cout << "Please specify a state name of at most " << MAX_STATE_NAME
                << " alphanumeric characters." << endl;
            } else {
                addNode(name);
            }
        } else if (MACHINE == "MOORE") {
            cin >> name;
            cin >> output;
            if (name.size() > MAX_STATE_NAME) {
                cout << "Please specify a state name of at most " << MAX_STATE_NAME
                << " alphanumeric characters." << endl;
            } else {
                addNode(name,output);
            }
        }
        break;

        // Add ARC
        case 'A':
        case 'a':
            if (MACHINE == "MEALY") {
                cin >> startNode >> endNode >> input >> forwardslash >> output;
                if (!locate(it,startNode)) { //Invalid StartNode
                    cout << "%% error: state \"" << startNode << "\" not defined %%" << endl;
                } else if (!locate(it,endNode)) { //Invalid EndNode
                    cout << "%% error: state \"" << endNode << "\" not defined %%" << endl;
                } else { //All Good! Add ARC!
                    replace( input.begin(), input.end(), 'X', 'x');
                    addArc(startNode,endNode,input,output);
                }
            } else if (MACHINE == "MOORE") {
                cin >> startNode >> endNode >> input;
                if (!locate(it,startNode)) { //Invalid StartNode
                    cout << "%% error: state \"" << startNode << "\" not defined %%" << endl;
                } else if (!locate(it,endNode)) { //Invalid EndNode
                    cout << "%% error: state \"" << endNode << "\" not defined %%" << endl;
                } else { //All Good! Add ARC!
                    replace( input.begin(), input.end(), 'X', 'x');
                    addArc(startNode,endNode,input,output);
                }
            }

            break;

        // Help
        case '?':
        case 'H':
        case 'h':
            help();
            break;

        // Quit
        case 'Q':
        case 'q':
            printf("Exiting FSM Simulator...\n");
            exit(0);

        // Output
        case 'O':
        case 'o':
            printOutput();
            printGraph();
            break;

        // Invalid Command
        default:
            printf("Invalid Command\n");
            break;
    }
}

/***************************************************************/
/* MAIN FUNCTION                                               */
/***************************************************************/

int main(int argc, char *argv[]) {

    printf("FSM Simulator\n\n");
    help();

    initialize();

    while (1) {
      get_command();
    }
}
