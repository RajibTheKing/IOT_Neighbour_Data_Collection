#include "strength-vector.h"
void onReceivedNewBeacon(Beacon receivedBeacon)
{
    int i;
    int currentSequence = receivedBeacon.seq;
    int nodeID = receivedBeacon.nodeID;

    if(highestSequence < currentSequence){
        highestSequence = currentSequence;
    }

    if(highestNodeID < nodeID){
        highestNodeID = nodeID;
    }

    receivedCounter[nodeID]++;

    

    for(i = 1; i<=highestNodeID; i++)
    {
        strength_vector[i] = 100.0 * (receivedCounter[i] * 1.0) / (highestSequence * 1.0);
    }

    debug_purpose++;
    if(debug_purpose  % 30 == 0){
        printf("TheKing--> strength_vector(%d): ", highestSequence);
        
        for(i = 1; i<= highestNodeID; i++){
            printf("(%d - ", i);
            printFLoat(strength_vector[i]);
            printf("), ");
        }
        printf("\n");
    }
}

void printFLoat(float value){
    
    char convertedRes[50]; //size of the number
    ftoa(value, convertedRes, 3);
    printf("%s", convertedRes);
}

// Converts a floating-point/double number to a string. 
void ftoa(float n, char* res, int afterpoint) 
{ 
    // Extract integer part 
    int ipart = (int)n; 
  
    // Extract floating part 
    float fpart = n - (float)ipart; 
  
    // convert integer part to string 
    int i = intToStr(ipart, res, 0); 
  
    // check for display option after point 
    if (afterpoint != 0) { 
        res[i] = '.'; // add dot 
  
        // Get the value of fraction part upto given no. 
        // of points after dot. The third parameter  
        // is needed to handle cases like 233.007 
        fpart = fpart * myPow(10, afterpoint); 
  
        intToStr((int)fpart, res + i + 1, afterpoint); 
    } 
} 

int myPow(int x, int p)
{
    int y = 1;
    while(p--){
        y = y * x;
    }
    return y;
}


// Reverses a string 'str' of length 'len' 
void reverse(char* str, int len) 
{ 
    int i = 0, j = len - 1, temp; 
    while (i < j) { 
        temp = str[i]; 
        str[i] = str[j]; 
        str[j] = temp; 
        i++; 
        j--; 
    } 
} 
  
// Converts a given integer x to string str[].  
// d is the number of digits required in the output.  
// If d is more than the number of digits in x,  
// then 0s are added at the beginning. 
int intToStr(int x, char str[], int d) 
{ 
    int i = 0; 
    while (x) { 
        str[i++] = (x % 10) + '0'; 
        x = x / 10; 
    } 
  
    // If number of digits required is more, then 
    // add 0s at the beginning 
    while (i < d) 
        str[i++] = '0'; 
  
    reverse(str, i); 
    str[i] = '\0'; 
    return i; 
} 

float getStrengthByNodeID(int nodeID)
{
    return strength_vector[nodeID];
}