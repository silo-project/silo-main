/*
    Name : Logisim Default Libraries
    Description : implementation of Logisim dedicated libraries.
*/

#include "../include/signal.h"
#include "../include/node/node.h"
#include "../include/gate/gate.h"
#include "../include/gate/Memory.h"
#include "../include/simulator/simulator.h"

void Memory_Register(NODE * node) {
/*
attr[0] : type of edge
0 == rising edge
1 == falling edge
strg[0] : PREV_CLK
strg[1] : value
inpt[0] : CURR_CLK
inpt[1] : input
oupt[0] : output
*/
    SIGNAL s;
    s.state = -1;
    if (SigChkTypeEdge(&node->storage[0], SigGetLogic(node->input[0]), node->attribute[0])) {
        node->storage[1] = (node->storage[1] & ~node->input[1].state) | node->input[1].value;
        s.value = node->storage[1];
        SendSignal(node->output[0], s);
    }
}

void Memory_Counter(NODE * node) {
/*
attr[0] : type of edge
0 == rising edge
1 == falling edge
strg[0] : PREV_CLK
strg[1] : value
inpt[0] : CURR_CLK
inpt[1] : input
inpt[2] : enable
inpt[3] : alternative
oupt[0] : output

*/
    SIGNAL s;
    s.state = -1;
    if (SigChkTypeEdge(&node->storage[0], SigGetLogic(node->input[0]), node->attribute[0])) {
        if (SigGetLogic(node->input[2])) {
            if (SigGetLogic(node->input[3]))
                node->storage[1]--;
            else
                node->storage[1]++;
        }
        else if (SigGetLogic(node->input[3]))
            node->storage[1] = SigGetLogic(node->input[1]);
    }
}
