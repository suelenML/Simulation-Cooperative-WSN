/*
 * MessagesNeighborhood.h
 *
 *  Created on: 15/7/2020
 *      Author: suelen
 */
#include <omnetpp.h>
#include "Basic802154Packet_m.h"

#ifndef MESSAGESNEIGHBORHOOD_H_
#define MESSAGESNEIGHBORHOOD_H_

class MessagesNeighborhood {
public:
    MessagesNeighborhood();
    virtual ~MessagesNeighborhood();

    //Inseri para a codificacao
    Basic802154Packet *frametrans;
    Basic802154Packet *frameretrans;

    virtual void setFrameTransmission(Basic802154Packet*);
    virtual void setFrameRetransmission(Basic802154Packet*);
};

#endif /* MESSAGESNEIGHBORHOOD_H_ */
