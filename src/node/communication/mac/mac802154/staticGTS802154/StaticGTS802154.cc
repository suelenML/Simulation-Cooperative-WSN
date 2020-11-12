/****************************************************************************
 *  Copyright: National ICT Australia,  2007 - 2012                         *
 *  Developed at the ATP lab, Networked Systems research theme              *
 *  Author(s): Yuriy Tselishchev                                            *
 *  This file is distributed under the terms in the attached LICENSE file.  *
 *  If you do not find this file, copies can be found by writing to:        *
 *                                                                          *
 *      NICTA, Locked Bag 9013, Alexandria, NSW 1435, Australia             *
 *      Attention:  License Inquiry.                                        *
 *                                                                          *
 ****************************************************************************/

#include "StaticGTS802154.h"

Define_Module(StaticGTS802154);

/***
 * Initialising some parameters, specific to Static GTS module
 * by overriding the startup() method. Important to call startup()
 * of the parent module in the end, otherwise it will not initialize
 ***/
void StaticGTS802154::startup() {
	// initialise GTS-specific parameters
	GTSlist.clear(); totalGTS = 0; assignedGTS = 0; GTSlistCopy.clear();
	requestGTS = par("requestGTS");
	gtsOnly = par("gtsOnly");
	//userelay = par("userelay");

	// other parameters are from Basic802154, need to read them for GTS scheduling
	totalSlots = par("numSuperframeSlots"); 	
	baseSlot = par("baseSlotDuration");
	minCap = par("minCAPLength");
	frameOrder = par("frameOrder");
	useGACK = par("useGACK");
	/*
	cout<<"------------GTS------------------\n";
	cout<<"requestGTS: "<< requestGTS<<"\n";
	cout<<"gtsOnly: "<< gtsOnly <<"\n";
	cout<<"totalSlots: "<< totalSlots<<"\n";
	cout<<"minCap: "<<minCap <<"\n";
	cout<<"frameOrder: "<< frameOrder<<"\n";
	cout<<"------------------------------\n";
	*/



	return Basic802154::startup();
}

/***
 * GTS request received by hub, need to return the number of 
 * slots to be granted. Can return 0 to indicate request denial
 ***/
int StaticGTS802154::gtsRequest_hub(Basic802154Packet *gtsPkt) {
	//Length of CAP after lengths of all GTS slots are subtracted
	int CAPlength = totalSlots;
	
	//check if the node already exists in the GTS list
	vector<Basic802154GTSspec>::iterator i;
	int total = 0;
	for (i = GTSlist.begin(); i != GTSlist.end(); i++) {
		total++;
		//cout<< "GTS Length Na Lista: "<<i->length << " GTS Length Do Pacote: " <<gtsPkt->getGTSlength()<< "\n";
		if (i->owner == gtsPkt->getSrcID() && !i->retransmissor) {
			if (i->length == gtsPkt->getGTSlength()) {
				return i->length;
			} else {
				totalGTS -= i->length;
				GTSlist.erase(i);
				total--;
			}
		} else {
			CAPlength -= i->length;
		}
	}
	
	//node not found, or requested slots changed
//	if (total >= 7 || (CAPlength - gtsPkt->getGTSlength()) *
//	    baseSlot * (1 << frameOrder) < minCap) {
//	    cout<<"Não alocou o GTS\n";
//		trace() << "GTS request from " << gtsPkt->getSrcID() <<
//		    " cannot be acocmodated";
//
//		return 0;
//	}

	if ((CAPlength - gtsPkt->getGTSlength()) *
	        baseSlot * (1 << frameOrder) < minCap) {
	        cout<<"Não alocou o GTS\n"<< (CAPlength - gtsPkt->getGTSlength()) *
	                baseSlot * (1 << frameOrder)<< "\n";
	        trace() << "GTS request from " << gtsPkt->getSrcID() <<
	            " cannot be acocmodated";

	        return 0;
	    }
	//cout<<"Alocou o GTS: -- variavel de conferencia: "<< (CAPlength - gtsPkt->getGTSlength()) * baseSlot * (1 << frameOrder)<< "\n";
	Basic802154GTSspec newGTSspec;
	newGTSspec.length = gtsPkt->getGTSlength();
	totalGTS += newGTSspec.length;
	newGTSspec.owner = gtsPkt->getSrcID();
	//SUELEN
	newGTSspec.retransmissor = false;
	GTSlist.push_back(newGTSspec);
	GTSlistCopy.push_back(newGTSspec);
	//copiaGTSSemRetransmissao();
	cout<< "Alocou um GTS: "<< newGTSspec.owner <<"\n";
	trace()<< "Alocou um GTS ";
	return newGTSspec.length;
}

/***
 * Hub can alter the beacon before broadcasting it
 * In particular, assign GTS slots and set CAP length
 ***/
void StaticGTS802154::prepareBeacon_hub(Basic802154Packet *beaconPacket) {
	int CAPlength = totalSlots;
	vector<Basic802154GTSspec>::iterator iter;

	// Retira os cooperantes anteriores
	GTSlist.clear();
    // Deixa a lista de GTS apenas com os nodos transmissores
    for (iter = GTSlistCopy.begin(); iter != GTSlistCopy.end(); iter++) {
       GTSlist.push_back(*iter);
    }
    // verifica se o nodo continua associado e possuí GTS
//    for (int j= 0; j < GTSlist.size(); j++) {
//        if(GTSlist[j].retransmissor){
//           //GTSlist.erase(GTSlist.begin()+j);
//        }
//    }

    cout<< "Numero de coop "<< beaconPacket->getVizinhosOuNodosCooperantesArraySize() <<"\n";

    if(beaconPacket->getVizinhosOuNodosCooperantesArraySize()>0){
            /*for (int k = 0; k < (int)GTSlist.size(); k++) {
                cout<< "Lista GTS Depois de copiar****Básico*****["<<k<<"]: "<<GTSlist[k].owner<<"\n";
            }*/

            /*Aqui tenho que dar um slot para o coordenador enviar o GACK*/
            if(useGACK){
                gtsRequest_hubRetransmissao(0,1);
            }

            // Atribui slot GTS para retransmissores
            for (int j = 0; j < (int) beaconPacket->getVizinhosOuNodosCooperantesArraySize(); j++){
                if(useRetransIndependent){
                    if(beaconPacket->getNumSlotPerCoopArraySize()>0){
                        for(int k = 0; k < beaconPacket->getNumSlotPerCoop(j); k++){
                            gtsRequest_hubRetransmissao(beaconPacket->getVizinhosOuNodosCooperantes(j),1);
                        }

                    }
                }else{
                    gtsRequest_hubRetransmissao(beaconPacket->getVizinhosOuNodosCooperantes(j),1);
                }
               //gtsRequest_hubRetransmissao(beaconPacket->getVizinhosOuNodosCooperantes(j),1); // Aqui foi teste para dar mais de um slot aos cooperantes
               //gtsRequest_hubRetransmissao(beaconPacket->getVizinhosOuNodosCooperantes(j),1);
            }
            /* Aqui estou atribuindo um slot para cada cooperante auxiliar*/
            if(useCoopAux){
                if(beaconPacket->getCoopAuxiliaresArraySize() >0){
                    for(int i = 0;i < (int) beaconPacket->getCoopAuxiliaresArraySize();i++){
                        gtsRequest_hubRetransmissao(beaconPacket->getCoopAuxiliares(i),1);
                    }
                }
            }
            /* Ao inserir os slots para os cooperantes eles acabam ficando antes das transmissoes,
             * por isso ordeno para deixar eles para o fim.
             */
           ordenaListGTS(beaconPacket);

    }


	beaconPacket->setGTSlistArraySize(GTSlist.size());
	for (int i = 0; i < (int)GTSlist.size(); i++) {
		if (CAPlength > GTSlist[i].length) {
			CAPlength -= GTSlist[i].length;
			//cout<<"GTSlist["<<i<<"].length;"<< GTSlist[i].length<<"\n";
			GTSlist[i].start = CAPlength + 1;
			//trace()<<"GTSlist["<<i<< "].start: "<< GTSlist[i].start;
			//cout<<"GTS["<<GTSlist[i].owner<<"] inicia no slot: "<< CAPlength + 1  << "\n";
			cout <<"GTS[ "<< i<< "]:"<<GTSlist[i].owner << "\n";

			beaconPacket->setGTSlist(i, GTSlist[i]);
		} else {
			trace() << "Internal ERROR: GTS list corrupted";
			trace()<< "CAPlength: "<<CAPlength<< "GTSlist[i].length: "<<GTSlist[i].length<<endl;
			GTSlist.clear(); totalGTS = 0;
			beaconPacket->setGTSlistArraySize(0);	
			CAPlength = totalSlots;
			break;
		}
	}
	cout<<"tam: "<< GTSlist.size()<<"\n";
	if(GTSlist.size()>0){
	 for (int i = (int)GTSlist.size()-1; i >= 0; i--) {
	     //cout<<"GTS["<< i<<"]: "<<GTSlist[i].owner << "\n";
	        if((i-1) >= 0 && GTSlist[i-1].retransmissor == 1){
	            beaconPacket->setSlotInicioRetrans(i-1); // o slotInicioRetrans ´e um short
	            //cout<<"É retrans: "<<GTSlist[i-1].retransmissor<<"\n";
                //cout<<"GTS: "<< i<< " Sou o nodo: "<< GTSlist[i].owner<<" O próximo é retransmissor"<< GTSlist[i-1].owner<< "\n";
                break;
	        }
	    }
	}


	beaconPacket->setCAPlength(CAPlength);

	/*for (int i = 0; i < (int)GTSlist.size(); i++) {
	    cout<< "Lista GTS["<<i<<"]: "<<GTSlist[i].owner<<"\n";
	}*/
}

/***
 * If disconnected from PAN, also need to reset GTS slots
 ***/
void StaticGTS802154::disconnectedFromPAN_node() {
	assignedGTS = 0;
}

/***
 * GTS request was successful
 ***/
void StaticGTS802154::assignedGTS_node(int slots) {
	assignedGTS = slots;
}

/***
 * Transmission of data packet requested earlier is complete
 * status string holds comma separated list of outcomes
 * for each transmission attempt
 ***/
void StaticGTS802154::transmissionOutcome(Basic802154Packet *pkt, bool success, string status) {
	if (getAssociatedPAN() != -1) {
		if (assignedGTS == 0 && requestGTS > 0) {
			transmitPacket(newGtsRequest(getAssociatedPAN(), requestGTS));
		} else if (TXBuffer.size()) {
			Basic802154Packet *packet = check_and_cast<Basic802154Packet*>(TXBuffer.front());
			TXBuffer.pop();
			transmitPacket(packet,0,gtsOnly);
		}
	}
}

bool StaticGTS802154::acceptNewPacket(Basic802154Packet *newPacket) 
{
	if (getAssociatedPAN() != -1 && getCurrentPacket() == NULL) {
		transmitPacket(newPacket,0,gtsOnly);
		return true;
	}
	return bufferPacket(newPacket);
}

/***
 * Timers can be accessed by overwriting timerFiredCallback
 **/
/*
void StaticGTS802154::timerFiredCallback(int index) {
	switch(index) {
		case NEW_TIMER: {
			//do something
			break;
		}
		
		default: {
			//important to call the function of the parent module
			Basic802154::timerFiredCallback(index);
		}
	}
}
*/

int StaticGTS802154::gtsRequest_hubRetransmissao(int id, int length) {
    //Length of CAP after lengths of all GTS slots are subtracted
    int CAPlength = totalSlots;

    //GTSlist.clear();
    if ((CAPlength - length) *
            baseSlot * (1 << frameOrder) < minCap) {
            cout<<"Não alocou o GTS\n"<< (CAPlength - length) *
                    baseSlot * (1 << frameOrder)<< "\n";
            trace() << "GTS request from " << id <<
                " cannot be acocmodated";

            return 0;
        }
    //cout<<"Alocou o GTS: Retransmissão: -- variavel de conferencia: "<< (CAPlength - length) * baseSlot * (1 << frameOrder)<< "\n";
    Basic802154GTSspec newGTSspec;
    newGTSspec.length = length;
    totalGTS += newGTSspec.length;
    newGTSspec.owner = id;
    newGTSspec.retransmissor = true;
    GTSlist.push_back(newGTSspec);

    return newGTSspec.length;
}
//inverte a ordem da lista do GTS (o primeiro serao ultimo e o ultimo se torna o primeiro)
void StaticGTS802154::ordenaListGTS(Basic802154Packet *beaconPacket) {
    beaconPacket->setGTSlistArraySize(GTSlist.size());
    int j = 0;
    Basic802154GTSspec aux;
        for (int i = 0; i < ((int)GTSlist.size())/2; i++) {
            j = GTSlist.size() - i -1;
            aux = GTSlist[i];
            GTSlist[i] = GTSlist[j];
            GTSlist[j]=aux;
        }
//        for (int i = 0; i < (int)GTSlist.size(); i++) {
//             cout<< "Lista GTS Depois de ordenar["<<i<<"]: "<<GTSlist[i].owner<<"\n";
//         }
}
//Acho que posso apagar
void StaticGTS802154::TempoNodosNaoCoopDormir(Basic802154Packet *beaconPacket){
    for (int i = (int)beaconPacket->getGTSlistArraySize(); i >= 0; i--) {
        if(beaconPacket->getGTSlist(i+1).retransmissor){
            cout<<"GTS: "<< i<< " Sou o nodo: "<< GTSlist[i].owner<<" O próximo é retransmissor"<< GTSlist[i+1].owner<< "\n";
        }
    }

}
void StaticGTS802154::remover(Basic802154Packet *beaconPacket, int id){
    if(GTSlist.size() > 0){
        cout<<"Antes de apagar do GTS\n";
        for (int j= 0; j < GTSlist.size(); j++) {
            cout<<"GTSlist[j].owner: "<< GTSlist[j].owner <<"\n";
                if(GTSlist[j].owner == id){
                   GTSlist.erase(GTSlist.begin()+j);
                }
            }

        cout<<"Depois de apagar do GTS\n";
            for (int j= 0; j < GTSlist.size(); j++) {
                cout<<"GTSlist[j].owner: "<< GTSlist[j].owner <<"\n";
             }
    }
}

/*
void StaticGTS802154::copiaGTSSemRetransmissao(){

    vector<Basic802154GTSspec>::iterator i;
    for (i = GTSlist.begin(); i != GTSlist.end(); i++) {
        GTSlistCopy.push_back(*i);
    }

    //for (int i = 0; i < (int)GTSlist.size(); i++) {
            //cout<< "Lista GTS Basico["<<i<<"]: "<<GTSlistCopy[i].owner<<"\n";
       // }
}*/
