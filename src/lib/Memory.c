/*
    Name : Logisim Default Libraries
    Description : implementation of Logisim dedicated libraries.
*/

#include "../include/signal.h"
#include "../include/node/node.h"
#include "../include/lib/Memory.h"
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
    if (SigChkTypeEdge(&node->data[0], SigGetLogic(node->srce[0]), node->attr[0])) {
        node->data[1] = (node->data[1] & ~node->srce[1].state) | node->srce[1].value;
        s.value = node->data[1];
        SendSignal(node->dest[0], s);
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
    if (SigChkTypeEdge(&node->data[0], SigGetLogic(node->srce[0]), node->attr[0])) {
        if (SigGetLogic(node->srce[2])) {
            if (SigGetLogic(node->srce[3]))
                node->data[1]--;
            else
                node->data[1]++;
        }
        else if (SigGetLogic(node->srce[3]))
            node->data[1] = SigGetLogic(node->srce[1]);
    }
}
