/****************************************************************************
 *  Copyright: National ICT Australia,  2007 - 2011                         *
 *  Developed at the ATP lab, Networked Systems research theme              *
 *  Author(s): Athanassios Boulis, Yuriy Tselishchev                        *
 *  This file is distributed under the terms in the attached LICENSE file.  *
 *  If you do not find this file, copies can be found by writing to:        *
 *                                                                          *
 *      NICTA, Locked Bag 9013, Alexandria, NSW 1435, Australia             *
 *      Attention:  License Inquiry.                                        *
 *                                                                          *
 ****************************************************************************/

#include "ThroughputTest.h"

Define_Module(ThroughputTest);

void ThroughputTest::startup()
{
	packet_rate = par("packet_rate");
	recipientAddress = par("nextRecipient").stringValue();
	recipientId = atoi(recipientAddress.c_str());
	startupDelay = par("startupDelay");
	delayLimit = par("delayLimit");
	packet_spacing = packet_rate > 0 ? 1 / float (packet_rate) : -1;
	//packet_spacing = 60 * 16 * (1 << frameorder) * symbolLen; // Suelen
	dataSN = 0;
	
	numNodes = getParentModule()->getParentModule()->par("numNodes");
	packetsSent.clear();
	packetsReceived.clear();
	bytesReceived.clear();

	// Suelen
	numpacketsReceived=0;


//	if (packet_spacing > 0 && recipientAddress.compare(SELF_NETWORK_ADDRESS) != 0)
//		setTimer(SEND_PACKET, packet_spacing + startupDelay);
//	else
//		trace() << "Not sending packets";

	declareOutput("Packets received per node");
	declareOutput("Total Packets received -- Coordenador");
	declareOutput("Packets received per node - Completo");



	/*
     Teste para verificar se mesmo não recebendo
     pacotes do nodo ele considera o nodo na tabela
     e coloca zero para o número de pacotes recebidos
     */
    for (int i = 1; i < numNodes; i++) {
        packetsReceived[i]= 0;
        packetsSent[i] = 0;
        //collectOutput("Packets received per node", i);

    }
}

void ThroughputTest::fromNetworkLayer(ApplicationPacket * rcvPacket,
		const char *source, double rssi, double lqi)
{
	int sequenceNumber = rcvPacket->getSequenceNumber();
	int sourceId = atoi(source);

	// This node is the final recipient for the packet
	if (recipientAddress.compare(SELF_NETWORK_ADDRESS) == 0) {
		//if (delayLimit == 0 || (simTime() - rcvPacket->getCreationTime()) <= delayLimit) {
			trace() << "Received packet #" << sequenceNumber << " from node " << source;
			collectOutput("Packets received per node", sourceId);
			packetsReceived[sourceId]++;
			bytesReceived[sourceId] += rcvPacket->getByteLength();


		//} else {
			//trace() << "Packet #" << sequenceNumber << " from node " << source <<
				//" exceeded delay limit of " << delayLimit << "s";
		//}
	// Packet has to be forwarded to the next hop recipient
	} else {
		ApplicationPacket* fwdPacket = rcvPacket->dup();
		// Reset the size of the packet, otherwise the app overhead will keep adding on
		fwdPacket->setByteLength(0);
		toNetworkLayer(fwdPacket, recipientAddress.c_str());
	}
	//Pacotes Recebidos---Suelen
	numpacketsReceived=0;
	for (int i = 0; i < numNodes; i++) {
	    numpacketsReceived = numpacketsReceived + packetsReceived[i];
	}
	collectOutputInt("Total Packets received -- Coordenador",0,"Total", getNumPacketsReceived());

}

void ThroughputTest::timerFiredCallback(int index)
{
	switch (index) {
		case SEND_PACKET:{
			trace() << "Sending packet #" << dataSN;
			toNetworkLayer(createGenericDataPacket(0, dataSN), recipientAddress.c_str());
			packetsSent[recipientId]++;
			dataSN++;
			setTimer(SEND_PACKET, packet_spacing);
			break;



		}
	}
}

// This method processes a received carrier sense interupt. Used only for demo purposes
// in some simulations. Feel free to comment out the trace command.
void ThroughputTest::handleRadioControlMessage(RadioControlMessage *radioMsg)
{
	switch (radioMsg->getRadioControlMessageKind()) {
		case CARRIER_SENSE_INTERRUPT:
			trace() << "CS Interrupt received! current RSSI value is: " << radioModule->readRSSI();
                        break;
       case BEACON_CHEGADA:
            trace() << "App Sending packet #" << dataSN;
            toNetworkLayer(createGenericDataPacket(0, dataSN), recipientAddress.c_str());
            packetsSent[recipientId]++;
            dataSN++;
            //setTimer(SEND_PACKET, packet_spacing);
            break;
	}
}

void ThroughputTest::finishSpecific() {
	declareOutput("Packets reception rate");
	declareOutput("Packets loss rate");
	declareOutput("Packets send");

	/*Suelen*/
	if(self == 0){ // Aqui apresenta os pacotes que o coordenador recebeu e insere zero para os que ele não recebeu
	    int cont = 1;
        for(int i = 1; i < numNodes; i++){
            collectOutput("Packets received per node - Completo", cont, "Recebidos", packetsReceived[i]);
            trace()<<"nodo Coord rebebeu do nodo: "<<i<<"  "<< packetsReceived[i];
            cont++;

        }
	}


	cTopology *topo;	// temp variable to access packets received by other nodes
	topo = new cTopology("topo");
	topo->extractByNedTypeName(cStringTokenizer("node.Node").asVector());

	long bytesDelivered = 0;
	for (int i = 0; i < numNodes; i++) {
		ThroughputTest *appModule = dynamic_cast<ThroughputTest*>
			(topo->getNode(i)->getModule()->getSubmodule("Application"));
		if (appModule) {
			int packetsSent = appModule->getPacketsSent(self);
			if (packetsSent > 0) { // this node sent us some packets
				float rate = (float)packetsReceived[i]/packetsSent;
				collectOutput("Packets reception rate", i, "total", rate);
				collectOutput("Packets loss rate", i, "total", 1-rate);
				collectOutput("Packets send",i, "total",packetsSent);

			}

			bytesDelivered += appModule->getBytesReceived(self);
		}

	}


	delete(topo);

	if (packet_rate > 0 && bytesDelivered > 0) {
		double energy = (resMgrModule->getSpentEnergy() * 1000000000)/(bytesDelivered * 8);	//in nanojoules/bit
		declareOutput("Energy nJ/bit");
		collectOutput("Energy nJ/bit","",energy);
	}


}


