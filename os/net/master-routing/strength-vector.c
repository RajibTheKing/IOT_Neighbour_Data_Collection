#include "strength-vector.h"
#include <stdio.h>



/*---------------------------------------------------------------------------*/
/**
 * \brief this fucntion is called when a beacon is resceived
 *        it calcultes network strength based on extended beacon sequence number    
 *  
 * */
void onReceivedNewBeacon(int own_node_id, Beacon receivedBeacon)
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
        strength_vector[i] = (receivedCounter[i] * 1.0) / (highestSequence * 1.0);
    }

    debug_purpose++;
    if(debug_purpose  % 30 == 0 && own_node_id == 1){
        //printf("strength_vector(%d): %d ", highestSequence, own_node_id);
        printf("strength_vector: %d ", own_node_id);

        for(i = 1; i<= highestNodeID; i++){
            printf("%d-", i);
            printFLoat(strength_vector[i]);
            printf(",");
        }
        printf("\n");
    }
}

void set_own_node_id_as_highest(int id){
    highestNodeID = id;
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


/*---------------------------------------------------------------------------*/
/**
 * \brief prepare network strenth as byte array for sending      
 *  
 * */
uint8_t prepareStrengthVectorToSend(uint8_t data[])
{
    int i;
    int index = 0;
    data[0] = (uint8_t) highestNodeID;
    index +=1;
    for(i=1; i<=highestNodeID; i++){
        uint8_t nodeId = i;
        uint16_t value = strength_vector[i] * 10000;
        memcpy(data + index, &nodeId, sizeof(uint8_t));
        memcpy(data + index + sizeof(uint8_t), &value, sizeof(uint16_t));
        index += 3;
    }

    return index;
}