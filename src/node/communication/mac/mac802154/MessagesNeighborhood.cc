/*
 * MessagesNeighborhood.cc
 *
 *  Created on: 15/7/2020
 *      Author: suelen
 */

#include <MessagesNeighborhood.h>


MessagesNeighborhood:: MessagesNeighborhood(){

}

MessagesNeighborhood::~MessagesNeighborhood(){
    delete frametrans;
    delete frameretrans;

}

void MessagesNeighborhood::setFrameTransmission(Basic802154Packet* framet){
    frametrans = framet;
}
void MessagesNeighborhood::setFrameRetransmission(Basic802154Packet* framer){
    frameretrans = framer;
}
