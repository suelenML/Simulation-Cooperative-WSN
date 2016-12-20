/****************************************************************************
 *  Copyright: National ICT Australia,  2007 - 2012                         *
 *  Developed at the ATP lab, Networked Systems research theme              *
 *  Author(s): Athanassios Boulis, Yuriy Tselishchev                        *
 *  This file is distributed under the terms in the attached LICENSE file.  *
 *  If you do not find this file, copies can be found by writing to:        *
 *                                                                          *
 *      NICTA, Locked Bag 9013, Alexandria, NSW 1435, Australia             *
 *      Attention:  License Inquiry.                                        *
 *                                                                          *
 ****************************************************************************/

#include "Basic802154.h"
//#include "ThroughputTest.h"



// This module is virtual and can not be used directly
Define_Module(Basic802154);

void Basic802154::startup() {
    printStateTransitions = par("printStateTransitions");
    if (printStateTransitions) {
        stateDescr[1000] = "MAC_STATE_SETUP";
        stateDescr[1001] = "MAC_STATE_SLEEP";
        stateDescr[1002] = "MAC_STATE_CAP";
        stateDescr[1003] = "MAC_STATE_GTS";
    }

    //Suelen
    //Informa se deve usar retransmissão ou não
    userelay = par("userelay");
    selecao = par("selecao");

    //Suelen
    beaconsPerdidos = 0;
    inicioGTSRetrans = -1;
    GTSstartRetrans = 0;
    GTSendRetrans = 0;
    GTSlengthRetrans = 0;
    primeiraRetrans = 0;
    irDormir = 0;
    primeiraSelecao = 0;
    msgEnviadas = 0;
    msgRecebidas = 0;
    msgRtrans = 0;
    beaconsRecebidos = 0;
    mensagensPerdidas = 0;
    limiteRSSI = -87;
    mensagensRecuperadas = 0;
    nodosAssociados.clear();

     //Suelen
    //Variaveis utilizadas para determinar o numero de cooperantes
    numdadosrecebidosnogtstransmissao = 0;
    SamLoss = 0.0;
    numhosts = getParentModule()->getParentModule()->getParentModule()->par("numNodes");;
    vectortaxaperda.setName("Taxa de Perda");
    alpha = par("alpha");
    betha = par("betha");
    ganho = par("ganho");
    limiaralpha = par("limiaralpha");
    limiarbetha = par("limiabetha");
    EstLoss = 0.0;
    DevLoss = 0.0;
    numeronodoscooperantes = 0; // no do Odilson iniciava em 20 ver o pq??

    //Vetor de taxa de Sucesso
    for(int i=0;i<numhosts;i++ ){
        TAXA_DE_SUCESSO txSucess;
        txSucess.id = i;
        txSucess.taxaDeSucesso = 0.001;
        historicoTaxaDeSucesso.push_back(txSucess);
    }
    //Parâmetro da taxa de sucesso
    alphaSucess = par("alphaSucess");


    isPANCoordinator = par("isPANCoordinator");
    isFFD = par("isFFD");

    // CAP-related parameters
    minCAPLength = par("minCAPLength");

    // Slot parameters
    unitBackoffPeriod = par("unitBackoffPeriod");
    baseSlotDuration = par("baseSlotDuration");
    numSuperframeSlots = par("numSuperframeSlots");
    baseSuperframeDuration = baseSlotDuration * numSuperframeSlots;

    // CSMA parameters
    enableSlottedCSMA = par("enableSlottedCSMA");
    macMinBE = par("macMinBE");
    macMaxBE = par("macMaxBE");
    macMaxCSMABackoffs = par("macMaxCSMABackoffs");
    macMaxFrameRetries = par("macMaxFrameRetries");
    batteryLifeExtention = par("batteryLifeExtention");

    // Physical layer parameters
    phyDataRate = par("phyDataRate");
    phyDelaySleep2Tx = (double) par("phyDelaySleep2Tx") / 1000.0;
    phyDelayRx2Tx = (double) par("phyDelayRx2Tx") / 1000.0;
    phyDelayForValidCS = (double) par("phyDelayForValidCS") / 1000.0;
    phyLayerOverhead = par("phyFrameOverhead");
    phyBitsPerSymbol = par("phyBitsPerSymbol");
    symbolLen = 1 / (phyDataRate * 1000 / phyBitsPerSymbol);
    ackWaitDuration = symbolLen * unitBackoffPeriod+
    phyDelayRx2Tx * 2 + TX_TIME(ACK_PKT_SIZE);

    // HUB connection parameters
    guardTime = (double) par("guardTime") / 1000.0;
    maxLostBeacons = par("maxLostBeacons");

    // General MAC initialisation
    currentPacket = NULL;
    macState = MAC_STATE_SETUP;
    associatedPAN = -1;
    currentFrameStart = 0;
    GTSstart = 0;
    GTSend = 0;
    CAPend = 0;
    seqNum = 0;

    // Statistics for reporting
    lostBeacons = 0;
    sentBeacons = 0;
    recvBeacons = 0;
    packetoverflow = 0;
    desyncTime = 0;
    desyncTimeStart = 0;
    packetBreak.clear();
    declareOutput("pkt TX state breakdown");

    // Coordinator initialisation
    if (isPANCoordinator) {
        if (!isFFD) {
            opp_error(
                    "Only full-function devices (isFFD=true) can be PAN coordinators");
        }
        if(userelay){
            beta1 = par("beta1");
            beta2 = par("beta2");
            beta3 = par("beta3");
            beta4 = par("beta4");
        }

        associatedPAN = SELF_MAC_ADDRESS;
        macBSN = genk_intrand(0, 255) + 1;

        //Numero de nodes a serem associados ao PAN
        maxChild = par("maxChild");
        nchildren = 0; //guarda o número de filhos


        //initialise frameOrder and beaconOrder
        frameOrder = par("frameOrder");
        beaconOrder = par("beaconOrder");
        if (frameOrder < 0 || beaconOrder < 0 || beaconOrder > 14
                || frameOrder > 14 || beaconOrder < frameOrder) {
            opp_error(
                    "Invalid combination of frameOrder and beaconOrder parameters. Must be 0 <= frameOrder <= beaconOrder <= 14");
        }

        beaconInterval = baseSuperframeDuration * (1 << beaconOrder);
        frameInterval = baseSuperframeDuration * (1 << frameOrder);
        CAPlength = numSuperframeSlots;

        if (beaconInterval <= 0 || frameInterval <= 0) {
            opp_error(
                    "Invalid parameter combination of baseSlotDuration and numSuperframeSlots");
        }


        setTimer(FRAME_START, 0);	//frame start is NOW
    }
}
// método Ríad
//verifica quais nodos só conseguiram transmitir por retransmissão.
//caso de dois nodos retransmitirem o mesmo nodo
/*void Basic802154::contabilizarMensagens() {
    int numNodosEscutados = nodosEscutados.size();
    int i = 0;

    cout<<"Sou Nodo: "<<SELF_MAC_ADDRESS<<"\n";
    cout<<"tamanho cooperacoes beacon: "<<cooperacoesDoBeacon.size()<<"\n";

    for(int j=0;j<numNodosEscutados;j++){
        cout<<"NodoEscutado["<<j<<"]: "<<nodosEscutados[j].idNodo<<"\n";
    }
    while (i < numNodosEscutados) {

        int nodoEscutado = nodosEscutados[i].idNodo;
        map<int, bool>::iterator nodoID = cooperacoesDoBeacon.find(
                nodoEscutado);// se ele não encontrar no find (nodoEscutado) retorna o ultimo elemento da lista

        if (nodoID != cooperacoesDoBeacon.end()) { // se não for o último elemento da lista é pq encontrou
            //cout<<"Lista de cooperacao: "<<cooperacoesDoBeacon.size()<< "\n";
            cooperacoesDoBeacon.erase(nodoID);
            retransmissoesNaoEfetivas++;
            //cout<< "Não efetivas: "<<retransmissoesNaoEfetivas <<"\n";
            //cout<<"Lista de cooperacao Depois de apagar: "<<cooperacoesDoBeacon.size()<< "\n";
        }
        i++;
    }

    retransmissoesEfetivas += cooperacoesDoBeacon.size();
    cout << "até o momento foram " << retransmissoesEfetivas
            << "retransmissões efetivas\n";
    cout << "até o momento foram " << retransmissoesNaoEfetivas
            << "retransmissões não efetivas\n";

}*/


//verifica quais nodos só conseguiram transmitir por retransmissão.
//caso de dois nodos retransmitirem o mesmo nodo
/*void Basic802154::contabilizarMensagens() {
    int repetido = 0,k = 0;
    std::map<int, vector<MENSAGENS_ESCUTADAS>*>::iterator iter;
    vector<MENSAGENS_ESCUTADAS> *vetor = new vector<MENSAGENS_ESCUTADAS>;
      for (iter = historicoDeCooperacao.begin(); iter != historicoDeCooperacao.end(); iter++) {
          vector<MENSAGENS_ESCUTADAS>* v;
          for(v = iter->second; v!=v->end(); v++){
              repetido=0;
              for(k = 0; k < nodosEscutados.size();k++){
                  if(v->idMens == nodosEscutados[k].idMens && v->idNodo == nodosEscutados[k].idNodo){
                      repetido = 1;
                  }

              }
              if(repetido = 0){
                 vetor->push_back(v);
              }
      }
          historicoDeSucesso[i] = vetor;
          i++;
      }

}*/


// método Ríad
//método usado pelo coordenador para adiciona ao beacon uma lista com os nodos cooperantes.
void Basic802154::enviarNodosCooperantes(Basic802154Packet *beaconPacket) {

    int tamanhoLista = nodosColaboradores.size();

    if (tamanhoLista > 0) {

        beaconPacket->setVizinhosOuNodosCooperantesArraySize(tamanhoLista);
        int i;
        for (i = 0; i < tamanhoLista; i++) {
            beaconPacket->setVizinhosOuNodosCooperantes(i,
                    nodosColaboradores[i]);
        }
    }
}

void Basic802154::timerFiredCallback(int index) {
    switch (index) {

    // Start of a new superframe
    case FRAME_START: {
        if (isPANCoordinator) {	// as a PAN coordinator, create and broadcast beacon packet

            beaconPacket = new Basic802154Packet("PAN beacon packet",
                    MAC_LAYER_PACKET);
            beaconPacket->setDstID(BROADCAST_MAC_ADDRESS);
            beaconPacket->setPANid(SELF_MAC_ADDRESS);
            beaconPacket->setMac802154PacketType(MAC_802154_BEACON_PACKET);
            beaconPacket->setBeaconOrder(beaconOrder);
            beaconPacket->setFrameOrder(frameOrder);
            if (++macBSN > 255)
                macBSN = 0;
            beaconPacket->setBSN(macBSN);
            beaconPacket->setCAPlength(numSuperframeSlots);



            if(userelay){
            //modificação Ríad
            //no começo de um novo beacon as mensagens são contabilizadas, a lista de nodos escutada é limpada e novos nodos cooperadores podem ser selecionados
                //contabilizarMensagens();
                //verificaRetransmissoesRepetidas();
                nodosEscutados.clear();

                if (tempoDeBeacon == selecao) {
                    selecionaNodosSmart(beaconPacket); // Função Correta
                    //selecionaNodosSmartNumVizinhos(beaconPacket);
                    //selecaoCoopAleatoria(beaconPacket);// Seleção Aleatória
                    enviarNodosCooperantes(beaconPacket);
                    tempoDeBeacon = 0;
                    primeiraSelecao = 1;
                    if (selecao == 3)
                        selecao = 5;
                    /*else if (selecao == 40)
                        selecao = 40;*/
                    //beaconPacket->setTempoBeacon(tempoDeBeacon);
                } else {
                    //cout<<"Numero de cooperantes: "<<beaconPacket->getVizinhosOuNodosCooperantesArraySize() <<"\n";
                    if(primeiraSelecao ==1){
                        //Para manter os colaboradores até a próxima seleção
                        enviarNodosCooperantes(beaconPacket);
                    }
                    //beaconPacket->setTempoBeacon(tempoDeBeacon);
                    tempoDeBeacon++;
                    // ver para manter os cooperantes aqui
                }
                //Suelen
                numdadosrecebidosnogtstransmissao = 0;

            }
            // GTS fields and CAP length are set in the decision layer
            prepareBeacon_hub(beaconPacket);

            // Verifica os nodos que tinham GTS e não estao mais associados
              /*if(beaconPacket->getGTSlistArraySize() > 0){
                  for (int j = 0; j < (int) beaconPacket->getGTSlistArraySize(); j++) {
                      if (associatedDevices[beaconPacket->getGTSlist(j).owner] == false){
                          remover(beaconPacket,beaconPacket->getGTSlist(j).owner);
                      }
                  }
              }*/


            beaconPacket->setByteLength(
                    BASE_BEACON_PKT_SIZE
                            + beaconPacket->getGTSlistArraySize()
                                    * GTS_SPEC_FIELD_SIZE);
            CAPlength = beaconPacket->getCAPlength();
            CAPend = CAPlength * baseSlotDuration * (1 << frameOrder)
                    * symbolLen;
            sentBeacons++;

            trace() << "Transmitting [PAN beacon packet] now, BSN = " << macBSN;
            cout<< "Transmitting [PAN beacon packet] now, BSN = " << macBSN<< "\n";
            setMacState(MAC_STATE_CAP);
            toRadioLayer(beaconPacket);
            toRadioLayer(createRadioCommand(SET_STATE, TX));
            setTimer(ATTEMPT_TX, TX_TIME(beaconPacket->getByteLength()));
            beaconPacket = NULL;

            currentFrameStart = getClock() + phyDelayRx2Tx;
            setTimer(FRAME_START, beaconInterval * symbolLen);
        } else {	// if not a PAN coordinator, then wait for beacon
            //cout<<"Setar RX: "<< SELF_MAC_ADDRESS<<"\n";
            toRadioLayer(createRadioCommand(SET_STATE, RX));
            setTimer(BEACON_TIMEOUT, guardTime * 7);
        }
        break;
    }
        //Suelen
    case GTS_START: {
        if (macState == MAC_STATE_SLEEP) {
            toRadioLayer(createRadioCommand(SET_STATE, RX));
        }
        setMacState(MAC_STATE_GTS);

        // we delay transmission attempt by the time required by radio to wake up
        // note that GTS_START timer was scheduled exactly phyDelaySleep2Tx seconds
        // earlier than the actual start time of GTS slot
        setTimer(ATTEMPT_TX, phyDelaySleep2Tx);


        //SUELEN
       // Aqui fazer o controle de quando ir dormir se for retransmissor
        if(userelay){
            if(cooperador){
                //Se for cooperante só dorme depois da retransmissão
                // inform the decision layer that GTS has started
                startedGTS_node();
                break;
            }else{
                if(primeiraRetrans > 0){// Sinaliza o fim das transmissões
                     // Se não for cooperante e existir retranmissores, dorme ao iniciar a retransmissao
                    setTimer(SLEEP_START, primeiraRetrans - simTime());

                     // inform the decision layer that GTS has started
                    startedGTS_node();
                break;
                }else{// Se não existir retransmissões
                     // set a timer to go to sleep after this GTS slot ends
                     setTimer(SLEEP_START, irDormir - simTime());
                    // inform the decision layer that GTS has started
                    startedGTS_node();
                    break;
                }
            }
        }

         //limpa o buffer da aplicacao antes de dormir
         //setTimer(CLEAR_BUFFER_APP, GTSlength);
        // set a timer to go to sleep after this GTS slot ends
        setTimer(SLEEP_START, phyDelaySleep2Tx + GTSlength);

        // inform the decision layer that GTS has started
        startedGTS_node();
        break;
    }

        // beacon timeout fired - indicates that beacon was missed by this node
    case BEACON_TIMEOUT: {
        lostBeacons++;
        //Limpa o Buffer da App
        limparBufferAplicacao();
        //cMessage *msg = new cMessage("BEACON",BEACON);
        //handleRadioControlMessage(msg);

        if (lostBeacons >= maxLostBeacons) {
            trace() << "Lost synchronisation with PAN " << associatedPAN;
            setMacState(MAC_STATE_SETUP);
            associatedPAN = -1;
            desyncTimeStart = getClock();
            disconnectedFromPAN_node();
            /*std::vector<int>::iterator iter;
            int i=0;
            for (iter = nodosAssociados.begin();iter != nodosAssociados.end(); iter++) {
                if(nodosAssociados[i] == SELF_MAC_ADDRESS){
                    nodosAssociados.erase(iter);
                    break;
                }
                i++;
            }*/


            if (currentPacket)
                clearCurrentPacket("No PAN");
        } else if (associatedPAN != -1) {
            trace() << "Missed beacon from PAN " << associatedPAN<< ", will wake up to receive next beacon in "<< beaconInterval * symbolLen - guardTime * 7 << " seconds";
            beaconsPerdidos ++;
             cout<< "Beacon Perdido NODO: "<< SELF_MAC_ADDRESS<<"\n";
            setMacState(MAC_STATE_SLEEP);
            toRadioLayer(createRadioCommand(SET_STATE, SLEEP));
            setTimer(FRAME_START, beaconInterval * symbolLen - guardTime * 7);
        }
        break;
    }

        // packet was not received
    case ACK_TIMEOUT: {
        collectPacketHistory("NoAck");
        attemptTransmission("ACK timeout");
        break;
    }

        // previous transmission is reset, attempt a new transmission
    case ATTEMPT_TX: {
        if (getTimer(ACK_TIMEOUT) != -1)
            break;
        attemptTransmission("ATTEMPT_TX timer");
        break;
    }

        // timer to preform Clear Channel Assessment (CCA)
    case PERFORM_CCA: {
        if (macState == MAC_STATE_GTS || macState == MAC_STATE_SLEEP)
            break;
        CCA_result CCAcode = radioModule->isChannelClear();
        if (CCAcode == CLEAR) {
            //Channel clear
            if (--CW > 0) {
                setTimer(PERFORM_CCA, unitBackoffPeriod * symbolLen);
            } else {
                transmitCurrentPacket();
            }
        } else if (CCAcode == BUSY) {
            //Channel busy
            CW = enableSlottedCSMA ? 2 : 1;
            if (++BE > macMaxBE)
                BE = macMaxBE;
            if (++NB > macMaxCSMABackoffs) {
                collectPacketHistory("CSfail");
                currentPacketRetries--;
                attemptTransmission("Current NB exeeded maxCSMAbackoffs");
            } else {
                performCSMACA();
            }
        } else if (CCAcode == CS_NOT_VALID_YET) {
            //Clear Channel Assesment (CCA) pin is not valid yet
            setTimer(PERFORM_CCA, phyDelayForValidCS);
        } else {
            //Clear Channel Assesment (CCA) pin is not valid at all (radio is sleeping?)
            //trace() << "ERROR: isChannelClear() called when radio is not ready";
            toRadioLayer(createRadioCommand(SET_STATE, RX));
        }
        break;
    }

    case SLEEP_START: {
        // SLEEP_START timer can sometimes be scheduled in the end of a frame
        // i.e. when BEACON_ORDER = FRAME_ORDER, overlapping with the interval
        // when a node already tries to prepare for beacon reception. Thus
        // check if BEACON_TIMEOUT timer is set before going to sleep
        if (getTimer(BEACON_TIMEOUT) != -1)
            break;
        cancelTimer(PERFORM_CCA);
        setMacState(MAC_STATE_SLEEP);
        toRadioLayer(createRadioCommand(SET_STATE, SLEEP));
        break;
    }
    //SUELEN
    case GTS_RETRANS:{
           if (macState == MAC_STATE_SLEEP) {
               toRadioLayer(createRadioCommand(SET_STATE, RX));
           }
           setMacState(MAC_STATE_GTS);

           // we delay transmission attempt by the time required by radio to wake up
           // note that GTS_START timer was scheduled exactly phyDelaySleep2Tx seconds
           // earlier than the actual start time of GTS slot

           GTSend = GTSendRetrans;
           GTSstart = GTSstartRetrans;
           GTSlength = GTSlengthRetrans;


           Basic802154Packet *packetRetrans = new Basic802154Packet("Retransmissao",
                           MAC_LAYER_PACKET);
           packetRetrans->setPANid(SELF_MAC_ADDRESS);
           packetRetrans->setMac802154PacketType(MAC_802154_DATA_PACKET);
           packetRetrans->setSrcID(SELF_MAC_ADDRESS);
           packetRetrans->setSource(SELF_MAC_ADDRESS);
           packetRetrans->setSeqNum(seqNum++);
           packetRetrans->setDstID(0);
           packetRetrans->setByteLength(COMMAND_PKT_SIZE);
           packetRetrans->setRetransmissao(true);
           retransmitir(packetRetrans);

           //cout<<"Sou o Nodo: "<<SELF_MAC_ADDRESS << "Retransmitidos\n";
//           for(int i=0;i< packetRetrans->getDadosVizinhoArraySize();i++){
//               cout<<"Enviando dadosVizinhos["<<i<<"]: "<<packetRetrans->getDadosVizinho(i)<<"\n";
//               }

           transmitPacket(packetRetrans);

           //limpa o buffer da aplicacao antes de dormir
            //setTimer(CLEAR_BUFFER_APP, GTSlength);
           // set a timer to go to sleep after this GTS slot ends
           setTimer(SLEEP_START, phyDelaySleep2Tx + GTSlength);
           break;
       }

    case BACK_TO_SETUP: {
        // This timer is scheduled to the end of the CAP period
        // when beacon is received, but node is not (yet) connected.
        // So when this timer fires and node is not connected, it
        // has to go back to setup stage
        if (associatedPAN == -1)
            setMacState(MAC_STATE_SETUP);
    }

    }
}
// método Ríad
void Basic802154::preencherDados(Basic802154Packet *macPacket) {
    if (neigmap.size() > 0) {

        unsigned i = neigmap.size();
        std::map<int, Neighborhood*>::iterator iter;

        macPacket->setVizinhosOuNodosCooperantesArraySize(i);
        macPacket->setSomaSinais(somaDeSinais);
        //macPacket->setEnergy(resMgrModule->getRemainingEnergy()/ resMgrModule->getInitialEnergy()); // Considera a energia restante no nodo
        macPacket->setEnergy(resMgrModule->getSpentEnergy()); //Considera a energia gasta
        //cout<<"Energia Gasta: "<< resMgrModule->getSpentEnergy()<< "  Nodo: "<< SELF_MAC_ADDRESS<<"\n";
        //cout<<"Energia rstante: "<< resMgrModule->getRemainingEnergy()<< "  Nodo: "<< SELF_MAC_ADDRESS<<"\n";
        i = 0;
        for (iter = neigmap.begin(); iter != neigmap.end(); iter++) {
            Neighborhood *nodo = iter->second;

            macPacket->setVizinhosOuNodosCooperantes(i, nodo->nodeId);
            i++;
        }
    }

}

/* A packet is received from upper layer (Network) */
void Basic802154::fromNetworkLayer(cPacket * pkt, int dstMacAddress) {
    Basic802154Packet *macPacket = new Basic802154Packet("802.15.4 MAC data packet", MAC_LAYER_PACKET);

    //modificação Ríad
    if(userelay){
        preencherDados(macPacket);
    }
    encapsulatePacket(macPacket, pkt);
    macPacket->setSrcID(SELF_MAC_ADDRESS); //if connected to PAN, would have a short MAC address assigned,
                                           //but we are not using short addresses in this model
    macPacket->setDstID(dstMacAddress);
    macPacket->setMac802154PacketType(MAC_802154_DATA_PACKET);
    macPacket->setSeqNum(seqNum++);

    if (seqNum > 255)
        seqNum = 0;
    if (!acceptNewPacket(macPacket))
        packetoverflow++;


}

void Basic802154::finishSpecific() {
    if (currentPacket)
        cancelAndDelete(currentPacket);
    if (desyncTimeStart >= 0)
        desyncTime += getClock() - desyncTimeStart;

    map<string, int>::const_iterator iter;
    declareOutput("Packet breakdown");
    if (packetoverflow > 0)
        collectOutput("Packet breakdown", "Failed, buffer overflow",
                packetoverflow);
    for (iter = packetBreak.begin(); iter != packetBreak.end(); ++iter) {
        if (iter->first.compare("Success") == 0) {
            collectOutput("Packet breakdown", "Success, first try",
                    iter->second);
        } else if (iter->first.compare("Broadcast") == 0) {
            collectOutput("Packet breakdown", "Broadcast", iter->second);
        } else if (iter->first.find("Success") != string::npos) {
            collectOutput("Packet breakdown", "Success, not first try",
                    iter->second);
        } else if (iter->first.find("NoAck") != string::npos) {
            collectOutput("Packet breakdown", "Failed, no ack", iter->second);
        } else if (iter->first.find("CSfail") != string::npos) {
            collectOutput("Packet breakdown", "Failed, busy channel",
                    iter->second);
        } else if (iter->first.find("NoPAN") != string::npos) {
            collectOutput("Packet breakdown", "Failed, no PAN", iter->second);
        } else {
            //trace() << "Unknown packet breakdonw category: " << iter->first<< " with " << iter->second << " packets";
        }
    }

    if (!isPANCoordinator) {
        if (desyncTime > 0) {
            declareOutput("Fraction of time without PAN connection");
            collectOutput("Fraction of time without PAN connection", "",
            SIMTIME_DBL(desyncTime) / SIMTIME_DBL(getClock()));
        }
        declareOutput("Beacons received");
        collectOutput("Beacons received", "", recvBeacons);
        // Suelen
        declareOutput("Total de Beacons Perdidos");
        collectOutput("Total de Beacons Perdidos", "", beaconsPerdidos);

        declareOutput("Mensagens Perdidas");
        collectOutput("Mensagens Perdidas", "", mensagensPerdidas);

        //declareOutput("Msg's enviadas");
        //collectOutput("Msg's enviadas", "", msgEnviadas);

    } else {
        declareOutput("Beacons sent");
        collectOutput("Beacons sent", "", sentBeacons);

        //Suelen
        declareOutput("Msg's Recebidas");
        collectOutput("Msg's Recebidas", "", msgRecebidas);

        declareOutput("Msg's Recebidas Retransmissao");
        collectOutput("Msg's Recebidas Retransmissao", "", msgRtrans);

        declareOutput("Retransmissoes Uteis");
        collectOutput("Retransmissoes Uteis","",retransmissoesEfetivas);

        declareOutput("Msg's Recuperadas");
        collectOutput("Msg's Recuperadas","",mensagensRecuperadas);





      }
}

/* Helper function to change internal MAC state and print a debug statement if neccesary 
 */
void Basic802154::setMacState(int newState) {
    if (macState == newState)
        return;
    if (printStateTransitions)
        trace() << "MAC state changed from " << stateDescr[macState] << " to "
                << stateDescr[newState];
    macState = newState;
}
// método Ríad
//esse método só é usado pelo coordenador, ele é chamado sempre que um nodo tem um vizinho que não conseguiu
//se comunicar com o coordenador.
void Basic802154::adicionarNodoSolto(int nodoConectado, int nodoSolto) {
    std::map<int, vector<int>*>::iterator inter = listaDeNodosSoltos.find(
            nodoSolto);
    if (inter == listaDeNodosSoltos.end()) {
        vector<int> *novo = new vector<int>;
        novo->push_back(nodoConectado);
        listaDeNodosSoltos[nodoSolto] = novo;
    } else {
        vector<int> *velho = inter->second;
        velho->push_back(nodoConectado);
    }
}

// método Ríad
//método que monta e resolve o probelma de otimização e escreve um arquivo .mod
void Basic802154::selecionaNodosSmartNumVizinhos(
        Basic802154Packet *beaconPacket) {

    std::string fileName("prob" + std::to_string(numeroDoProblema) + ".mod");
    char *cstr = new char[fileName.length() + 1];
    strcpy(cstr, fileName.c_str());

    std::ofstream out(cstr);

    std::map<int, Neighborhood*>::iterator iterNeighborhood;

    out << "min:";
    bool primeiro = true;
    if (neigmap.size() > 0) {
        for (iterNeighborhood = neigmap.begin();
                iterNeighborhood != neigmap.end(); iterNeighborhood++) {
            Neighborhood *nodo = iterNeighborhood->second;
            if (primeiro) {
                primeiro = false;
                out << beta3 * nodo->numeroDevizinhos << "*x" << nodo->nodeId;
            } else {
                out << "+" << beta3 * nodo->numeroDevizinhos << "* x"
                        << nodo->nodeId;
            }

        }
        out << ";\n";

        for (iterNeighborhood = neigmap.begin();
                iterNeighborhood != neigmap.end(); iterNeighborhood++) {
            Neighborhood *nodo = iterNeighborhood->second;

            if (nodo->numeroDevizinhos > 0) {
                primeiro = true;
                int i, nodosConectados = 0;
                for (i = 0; i < nodo->numeroDevizinhos; i++) {

                    if (neigmap.find(nodo->vizinhos[i]) == neigmap.end()) {
                        adicionarNodoSolto(nodo->nodeId, nodo->vizinhos[i]);
                    } else {
                        nodosConectados++;
                        if (primeiro) {
                            out << "C" << nodo->nodeId << ":x" << nodo->nodeId
                                    << "+ x" << nodo->vizinhos[i];
                            primeiro = false;
                        } else {
                            out << "+x" << nodo->vizinhos[i];
                        }
                    }
                }
                if (!primeiro) {
                    out << ">=1;\n";
                }
            }
        }
        out << "\n";
        std::map<int, vector<int>*>::iterator iter;

        for (iter = listaDeNodosSoltos.begin();
                iter != listaDeNodosSoltos.end(); iter++) {

            vector<int>::iterator v = iter->second->begin();

            out << "C" << iter->first << ":";
            primeiro = true;
            while (iter->second->end() != v) {
                if (primeiro) {
                    out << "x" << *v;
                    primeiro = false;
                } else {
                    out << "+x" << *v;
                }
                v++;
            }
            out << ">=1;\n";
        }
        out << "\n";

        primeiro = true;
        int i = 0;
        for (iterNeighborhood = neigmap.begin();
                iterNeighborhood != neigmap.end(); iterNeighborhood++) {
            Neighborhood *nodo = iterNeighborhood->second;
            out << "bin x" << nodo->nodeId << ";" << "\n";
            i++;

        }

        out.close();

        lprec *lp;

        char *lpName = "prob";

        lp = read_LP(cstr, 2, lpName);

        if (lp == NULL) {
            fprintf(stderr, "Unable to read model\n");
        } else {

            solve(lp);
            //print_solution(lp, 1);
            REAL resultado_lp[i];
            get_variables(lp, resultado_lp);
            int j = 0;
            primeiro = true;
            //limpando lista de colaboradores
            nodosColaboradores.clear();
            //cout<<"Selecao de Cooperantes\n";
            for (iterNeighborhood = neigmap.begin();
                    iterNeighborhood != neigmap.end(); iterNeighborhood++) {
                Neighborhood *nodo = iterNeighborhood->second;

                if (resultado_lp[j] == 1) {
                    nodosColaboradores.push_back(nodo->nodeId);
                    //cout<<"- : "<<nodo->nodeId <<"\n";
                }
                j++;

            }
        }
        numeroDoProblema++;
    }
}
// método Ríad
//método que monta e resolve o probelma de otimização e escreve um arquivo .mod
void Basic802154::selecionaNodosSmart(Basic802154Packet *beaconPacket) {

    std::string fileName("prob" + std::to_string(numeroDoProblema) + ".mod");
    char *cstr = new char[fileName.length() + 1];
    strcpy(cstr, fileName.c_str());
    std::ofstream out(cstr);
    std::map<int, Neighborhood*>::iterator iterNeighborhood;


    out << "min:";
    bool primeiro = true;
    if (neigmap.size() > 0) {
        for (iterNeighborhood = neigmap.begin();
                iterNeighborhood != neigmap.end(); iterNeighborhood++) {
            Neighborhood *nodo = iterNeighborhood->second;
            if (primeiro) {
                primeiro = false;

                cout<<"Energia: "<< nodo->energy<<" RSSI: "<<nodo->somaRssi<<" Vizinhos: "<<nodo->numeroDevizinhos << "Taxa de Sucesso: "<<nodo->txSucesso << " ID: "<< nodo->nodeId<<"\n";
                out << ((beta1 / nodo->energy) + (beta2 / nodo->somaRssi)
                                + (beta3 * nodo->numeroDevizinhos) + (beta4 / nodo->txSucesso)) << "*x"
                        << nodo->nodeId;

               /*POSSIVEIS_COOPERANTES possiveisCoop;
               possiveisCoop.id = nodo->nodeId;
               possiveisCoop.valor = ((beta1 * nodo->energy) + (beta2 * nodo->somaRssi)
                       + (beta3 * nodo->numeroDevizinhos)+ (beta4 * nodo->txSucesso));
               cooperantes.push_back(possiveisCoop);*/


            } else {
                cout<<"Energia: "<< nodo->energy<<" RSSI: "<<nodo->somaRssi<<" Vizinhos: "<<nodo->numeroDevizinhos << "Taxa de Sucesso: "<<nodo->txSucesso <<" ID: "<< nodo->nodeId<<"\n";
                out << "+"
                        << ((beta1 / nodo->energy) + (beta2 / nodo->somaRssi)
                                + (beta3 * nodo->numeroDevizinhos) + (beta4 / nodo->txSucesso)) << "* x"
                        << nodo->nodeId;

                /*POSSIVEIS_COOPERANTES possiveisCoop;
                possiveisCoop.id = nodo->nodeId;
                possiveisCoop.valor = ((beta1 * nodo->energy) + (beta2 * nodo->somaRssi)
                          + (beta3 * nodo->numeroDevizinhos)+ (beta4 * nodo->txSucesso));
                cooperantes.push_back(possiveisCoop);*/

            }

        }
        out << ";\n";

        for (iterNeighborhood = neigmap.begin();
                iterNeighborhood != neigmap.end(); iterNeighborhood++) {
            Neighborhood *nodo = iterNeighborhood->second;
            cout <<"\n--->>>>>eu sou o nodo"<<nodo->nodeId<<"e eu escutei:\n ";
            if (nodo->numeroDevizinhos > 0) {
                primeiro = true;
                int i, nodosConectados = 0;
                for (i = 0; i < nodo->numeroDevizinhos; i++) {
                    cout <<"--->>>>>"<<nodo->vizinhos[i]<<"\n";
                    //aqui é verificado se o nodo vizinho foi escutado pelo coordenador. Caso ele não tenha
                    //sido escutado pelo coordenador ele é considerado um nodo solto

                    if (neigmap.find(nodo->vizinhos[i]) == neigmap.end()) {
                        adicionarNodoSolto(nodo->nodeId, nodo->vizinhos[i]);
                    } else {
                        nodosConectados++;
                        if (primeiro) {
                            out << "C" << nodo->nodeId << ":x" << nodo->nodeId
                                    << "+ x" << nodo->vizinhos[i];
                            primeiro = false;
                        } else {
                            out << "+x" << nodo->vizinhos[i];
                        }
                    }
                }
                if (!primeiro) {
                    out << ">=1;\n";
                }
            }
        }
        out << "\n";
        std::map<int, vector<int>*>::iterator iter;

        for (iter = listaDeNodosSoltos.begin();
                iter != listaDeNodosSoltos.end(); iter++) {

            vector<int>::iterator v = iter->second->begin();

            out << "C" << iter->first << ":";
            primeiro = true;
            while (iter->second->end() != v) {
                if (primeiro) {
                    out << "x" << *v;
                    primeiro = false;
                } else {
                    out << "+x" << *v;
                }
                v++;
            }
            out << ">=1;\n";
        }
        if (listaDeNodosSoltos.size() > 0) {
            out << "C0:";
            primeiro = true;
            for (iter = listaDeNodosSoltos.begin();
                    iter != listaDeNodosSoltos.end(); iter++) {

                int id = iter->first;

                if (primeiro) {
                    out << "x" << id;
                    primeiro = false;
                } else {
                    out << "+x" << id;
                }
            }
            out << "=0;\n";
        }
        out << "\n";
        primeiro = true;
        int i = 0;
        for (iterNeighborhood = neigmap.begin();
                iterNeighborhood != neigmap.end(); iterNeighborhood++) {
            Neighborhood *nodo = iterNeighborhood->second;

            out << "bin x" << nodo->nodeId << ";" << "\n";
            i++;

        }

        out.close();

        lprec *lp;

        char *lpName = "prob";

        lp = read_LP(cstr, 2, lpName);

        if (lp == NULL) {
            fprintf(stderr, "Unable to read model\n");
        } else {

            solve(lp);
            //print_solution(lp, 1);
            print_solution(lp, 1);
            REAL resultado_lp[i];
            get_variables(lp, resultado_lp);
            /*
            for(int k=0; k<i;k++){
                cout<<"elementos otimizacao: "<< resultado_lp[k] <<"\n";
            }

            //For para multiplicar o alor da otimização pela funcao beneficiio
            for(int i = 0; i < (int)cooperantes.size();i++){
                cout<<"Id: "<< cooperantes[i].id<<"\n";
                cout<<"Valor: "<< cooperantes[i].valor<<"\n";
                cooperantes[i].valor = resultado_lp[i] * cooperantes[i].valor;
                cout<<"Valor Depois: "<< cooperantes[i].valor<<"\n";
            }

            //Ordena o valor obtido da funcao benefio
            POSSIVEIS_COOPERANTES temp;
            for (int i=0;i<(int)cooperantes.size(); i++){
                for(int j=i+1;j<(int)cooperantes.size();j++)
                {
                    if (cooperantes[i].valor > cooperantes[j].valor)
                    {
                        temp.valor=cooperantes[i].valor;
                        temp.id=cooperantes[i].id;
                        cooperantes[i].valor=cooperantes[j].valor;
                        cooperantes[i].id=cooperantes[j].id;
                        cooperantes[j].valor=temp.valor;
                        cooperantes[j].id = temp.id;
                    }
                }
            }

            for (int j=0; j< (int)cooperantes.size(); j++) {
                  cout<<"\nCooperante: "<< cooperantes[j].id<<"\n";
                  cout<<"Valor: "<< cooperantes[j].valor<"\n";
            }
            */
            int j = 0;
            primeiro = true;
            //limpando lista de colaboradores
            nodosColaboradores.clear();
            for (iterNeighborhood = neigmap.begin();
                    iterNeighborhood != neigmap.end(); iterNeighborhood++) {
                Neighborhood *nodo = iterNeighborhood->second;
                /*if(j==numCoop)
                    break;

                for (int i=0;i<(int)cooperantes.size(); i++){
                    if(nodo->nodeId == cooperantes[i].id && cooperantes[i].valor != 0){
                        cout<<"coop: "<<cooperantes[i].id <<"\n";
                        nodosColaboradores.push_back(nodo->nodeId);
                        j++;
                        break;
                    }
                }*/
                //cout<<resultado_lp[j]<<"\n";
                if (resultado_lp[j] == 1) {
                    nodosColaboradores.push_back(nodo->nodeId);
                }
                j++;

            }
        }
        numeroDoProblema++;
    }
    //cooperantes.clear();
}
// método Ríad
//esse método é executado por qualquer nodo quando ele recebe uma mensagem.
//A função é gravar criar uma lista com os nodos vizinhos
void Basic802154::AtualizarVizinhaca(Basic802154Packet * pkt, double rssi) {
    /*caso um nodo ainda não tenha sido marcado como vizinho ele entre no if e as informaçõe necessárias são
     * gravadas a estrutura Neighborhood.
     */


///pega o ID do nodo que recebeu o pacote e verifica se ele já está em neigmap
    std::map<int, Neighborhood*>::iterator iterNeighborhood = neigmap.find(
            pkt->getSrcID());
    Neighborhood *nodo;
    cout<<"Eu sou o: "<< SELF_MAC_ADDRESS<< "\n";
    unsigned vizinhos = pkt->getVizinhosOuNodosCooperantesArraySize();
    if(rssi>limiteRSSI){
        if (iterNeighborhood == neigmap.end()) {
            nodo = new Neighborhood();
            nodo->nodeId = pkt->getSrcID();
            nodo->rssi = (rssi / MAX_RSSI);
            somaDeSinais = somaDeSinais + (rssi / MAX_RSSI);
            nodo->energy = pkt->getEnergy();
            nodo->numeroDevizinhos = vizinhos;
            nodo->somaRssi = pkt->getSomaSinais(); //atulizar soma de rssi
            nodo->txSucesso = taxaDeSucesso(pkt->getSrcID(), 1);

            if (isPANCoordinator && vizinhos > 0) {
                //atualizar lista de vinhos
                int i;
                for (i = 0; i < vizinhos; i++) {
                    nodo->vizinhos.push_back(pkt->getVizinhosOuNodosCooperantes(i));
                }
            }
            neigmap[pkt->getSrcID()] = nodo;
        } else {            //se o nodo já está na lista atualiza os dados
            nodo = iterNeighborhood->second;
            somaDeSinais = somaDeSinais - nodo->rssi;
            nodo->rssi = (rssi / MAX_RSSI);
            somaDeSinais = somaDeSinais + (rssi / MAX_RSSI);
            nodo->energy = pkt->getEnergy();
            nodo->numeroDevizinhos = vizinhos;
            nodo->somaRssi = pkt->getSomaSinais();
            nodo->txSucesso = taxaDeSucesso(pkt->getSrcID(), 1);
            if (isPANCoordinator && vizinhos > 0) {
                //atualizar lista de vinhos
                nodo->vizinhos.clear();
                int i;
                for (i = 0; i < vizinhos; i++) {
                    nodo->vizinhos.push_back(pkt->getVizinhosOuNodosCooperantes(i));
                }
            }

        }
    }
}
double Basic802154:: taxaDeSucesso(int id, int recebidas){
    double SR, SampleSR;
    /*SampleSR = (qntMsgEnviada/qntMsgRecebida)*/
    SampleSR = 1/recebidas;
    for(int i = 1; i <= numhosts-1; i++){
        if(i==id){
            historicoTaxaDeSucesso[i].id = id;
            historicoTaxaDeSucesso[i].taxaDeSucesso = (1- alphaSucess)* historicoTaxaDeSucesso[i].taxaDeSucesso  + (alphaSucess*SampleSR);
            SR = historicoTaxaDeSucesso[i].taxaDeSucesso;
            break;
        }
    }
    return SR;

}


// método Ríad
//
void Basic802154::souNodoCooperante(Basic802154Packet * pkt) {
    unsigned int i = 0;
    cooperador = false;

    while (pkt->getVizinhosOuNodosCooperantesArraySize() > i) {

        if (pkt->getVizinhosOuNodosCooperantes(i) == self) {
            cooperador = true;
            break;
        }
        i++;
    }
}
// método Ríad
void Basic802154::listarNodosEscutados(Basic802154Packet *rcvPacket, double rssi) {
    int repetido = 0;
    if(isPANCoordinator){
        cout <<"Sou o nodo:"<< SELF_MAC_ADDRESS<<" número de sequencia: "<< rcvPacket->getSeqNum()<<" Nodo que enviou: "<< rcvPacket->getSrcID()<< "\n";
    }
    if (rcvPacket->getDadosVizinhoArraySize() == 0 && rcvPacket->getSrcID() != 0) { // evita que retransmissoes sejam retransmitidas novamente
        for(int i=0; i< (int)nodosEscutados.size();i++){
            if(nodosEscutados[i].idMens == rcvPacket->getSeqNum() && nodosEscutados[i].idNodo == rcvPacket->getSrcID()){
                repetido = 1;
                break;
            }
        }
        /*if(!isPANCoordinator){*/
            if(repetido == 0 && rssi >= limiteRSSI){// nodos comuns só escutam seus vizinhos se o rssi for maior que -87Dbm
                 MENSAGENS_ESCUTADAS escutados;
                 escutados.idMens = rcvPacket->getSeqNum();
                 escutados.idNodo = rcvPacket->getSrcID();
                nodosEscutados.push_back(escutados); // insere nos nodos escutados
                //cout<<"Inserindo Escutado: "<< nodosEscutados.front() <<"\n";
             }
            /*}else{ // A parte comentada é caso o coordenador não use as mesmas regras de restrição de rssi
             if(repetido == 0){
                  MENSAGENS_ESCUTADAS escutados;
                  escutados.idMens = rcvPacket->getSeqNum();
                  escutados.idNodo = rcvPacket->getSrcID();
                 nodosEscutados.push_back(escutados); // insere nos nodos escutados
                 //cout<<"Inserindo Escutado: "<< nodosEscutados.front() <<"\n";
              }
         }*/
    }
}
//Suelen Este método armazena as retransmissoes por beacon interval
void Basic802154::armazenaRetransmissoes(Basic802154Packet *rcvPacket){

    if (historicoDeCooperacao.find(rcvPacket->getSrcID())
                                    != historicoDeCooperacao.end()) {

        map<int, vector<MENSAGENS_ESCUTADAS>*>::iterator iter;
        iter = historicoDeCooperacao.find(rcvPacket->getSrcID());
        vector<MENSAGENS_ESCUTADAS>* v = iter->second;
        for(int i = 0;i < (int)rcvPacket->getDadosVizinhoArraySize();i++){
            MENSAGENS_ESCUTADAS escutados;
            escutados.idMens = rcvPacket->getDadosVizinho(i).idMens;
            escutados.idNodo = rcvPacket->getDadosVizinho(i).idNodo;
            v->push_back(escutados);
        }

    }else{
        vector<MENSAGENS_ESCUTADAS> *vetor = new vector<MENSAGENS_ESCUTADAS>;
        for(int i = 0;i < (int)rcvPacket->getDadosVizinhoArraySize();i++){
            MENSAGENS_ESCUTADAS escutados;
            escutados.idMens = rcvPacket->getDadosVizinho(i).idMens;
            escutados.idNodo = rcvPacket->getDadosVizinho(i).idNodo;
            vetor->push_back(escutados);
            historicoDeCooperacao[rcvPacket->getSrcID()] = vetor;
        }

    }

}

void Basic802154::verificarRetransmissao(Basic802154Packet *rcvPacket) {
    int i = 0,j = 0,repetido = 0;
    int utilidadeRetransmissao = 0;

    vector<MENSAGENS_ESCUTADAS> *vetor = new vector<MENSAGENS_ESCUTADAS>;



    if (rcvPacket->getDadosVizinhoArraySize() > 0) {
        cout<<"Numer de escutados: "<<rcvPacket->getDadosVizinhoArraySize()<<"\n";
        /** Salvando todas as cooperações para excluir cooperantes que estão repetindo as mensagens**/
        armazenaRetransmissoes(rcvPacket);

        if (historicoDeSucesso.find(rcvPacket->getSrcID())
                        != historicoDeSucesso.end()) {

            for(int c = 0;c < (int)nodosEscutados.size();c++){
                cout<<"nodosEscutados["<<c<<"].idNodo: "<<nodosEscutados[c].idNodo<< "\n";
                cout<<"nodosEscutados["<<c<<"].idMens: "<<nodosEscutados[c].idMens<<"\n";
            }
            map<int, vector<MENSAGENS_ESCUTADAS>*>::iterator iter;
            iter = historicoDeSucesso.find(rcvPacket->getSrcID());
            vector<MENSAGENS_ESCUTADAS>* v = iter->second;

            for(i = 0;i < (int)rcvPacket->getDadosVizinhoArraySize();i++){
                repetido = 0;
                for(j = 0;j < (int)nodosEscutados.size();j++){
                    //if(nodosEscutados[j].idMens == rcvPacket->getDadosVizinho(i)){
                    if(nodosEscutados[j].idMens == rcvPacket->getDadosVizinho(i).idMens && nodosEscutados[j].idNodo == rcvPacket->getDadosVizinho(i).idNodo){
                        repetido = 1;
                        //MENSAGENS_ESCUTADAS_REPETIDAS repetidos;
                        //repetidos.idMens = rcvPacket->getDadosVizinho(i).idMens;;
                        //repetidos.idNodo = rcvPacket->getDadosVizinho(i).idNodo;
                        //repetidos.idRetransmissor =  rcvPacket->getSrcID();
                        //retransmissoesRepetidas.push_back(repetidos);
                        break;
                    }
                }
                if(repetido==0){
                   //cooperacoesDoBeacon[rcvPacket->getDadosVizinho(i).idNodo] = true;
                   MENSAGENS_ESCUTADAS escutados;
                   escutados.idMens = rcvPacket->getDadosVizinho(i).idMens;
                   escutados.idNodo = rcvPacket->getDadosVizinho(i).idNodo;
                   v->push_back(escutados);
                   nodosEscutados.push_back(escutados);// Se a msg que veio da cooperação não havia sido escutada agoa foi, por isso add aqui
                   utilidadeCoop++;
                   utilidadeRetransmissao++;
                }
            }
        }else{
            for(int c = 0;c < (int)nodosEscutados.size();c++){
                cout<<"nodosEscutados["<<c<<"].idNodo: "<<nodosEscutados[c].idNodo<< "\n";
                cout<<"nodosEscutados["<<c<<"].idMens: "<<nodosEscutados[c].idMens<<"\n";
            }
           for(i = 0;i < (int)rcvPacket->getDadosVizinhoArraySize();i++){
               repetido = 0;
                for(j = 0;j < (int)nodosEscutados.size();j++){
                    if(nodosEscutados[j].idMens == rcvPacket->getDadosVizinho(i).idMens && nodosEscutados[j].idNodo == rcvPacket->getDadosVizinho(i).idNodo){
                        repetido = 1;
                        //MENSAGENS_ESCUTADAS_REPETIDAS repetidos;
                        //repetidos.idMens = rcvPacket->getDadosVizinho(i).idMens;
                        //repetidos.idNodo = rcvPacket->getDadosVizinho(i).idNodo;
                        //repetidos.idRetransmissor =  rcvPacket->getSrcID();
                        //retransmissoesRepetidas.push_back(repetidos);
                        break;
                    }
                }
                if(repetido==0){
                   //cooperacoesDoBeacon[rcvPacket->getDadosVizinho(i).idNodo] = true;
                   MENSAGENS_ESCUTADAS escutados;
                   escutados.idMens = rcvPacket->getDadosVizinho(i).idMens;
                   escutados.idNodo = rcvPacket->getDadosVizinho(i).idNodo;
                   vetor->push_back(escutados);
                   nodosEscutados.push_back(escutados);// Se a msg que veio da cooperação não havia sido escutada agoa foi, por isso add aqui
                   utilidadeCoop++;
                   utilidadeRetransmissao++;
                   historicoDeSucesso[rcvPacket->getSrcID()] = vetor;
                }
            }
           //historicoDeSucesso[rcvPacket->getSrcID()] = vetor;
        }

        if(utilidadeRetransmissao != 0){
                mensagensRecuperadas = mensagensRecuperadas + utilidadeRetransmissao;
                retransmissoesEfetivas++;
                //cout<<"Retransmissões Uteis: "<< retransmissoesEfetivas<< "\n";
            }else{
                retransmissoesNaoEfetivas++;
                //cout<<"Retransmissões que não foram Uteis: "<< retransmissoesNaoEfetivas<< "\n";
            }
            cout<<"O nodo retransmitiu "<< (int)rcvPacket->getDadosVizinhoArraySize()<<" Mensagens e "<<utilidadeRetransmissao<< " Foram uteis.\n";
            //cout<<"Até esta retransmissão este cooperante retransmitu "<<utilidadeCoop <<" Mensagens uteis.\n";
            cout<<"Retransmissões Uteis: "<< retransmissoesEfetivas<< "\n";
            cout<<"Retransmissões que não foram Uteis: "<< retransmissoesNaoEfetivas<< "\n";

    }

}
//Suelen Verifica os nodos que retransmitiram todas as mensagens repetidas
void Basic802154::verificaRetransmissoesRepetidas(){
    std::map<int, vector<MENSAGENS_ESCUTADAS>*>::iterator iter;
    std::map<int, vector<MENSAGENS_ESCUTADAS>*>::iterator iterProx;
    vector<MENSAGENS_ESCUTADAS>::iterator i;
    vector<MENSAGENS_ESCUTADAS>::iterator j;

    for (iter = historicoDeCooperacao.begin(); iter != historicoDeCooperacao.end(); iter++) {
        vector<MENSAGENS_ESCUTADAS> *nodo = iter->second;
        cout<<"Cooperante: "<< iter->first<<"\n";
        for(i =(iter->second)->begin();i!=(iter->second)->end();i++){
            cout<<"valor 1: "<<i->idNodo<<"\n";
            cout<<"Mens: "<<i->idMens<<"\n";

             for (iterProx = iter++; iterProx != historicoDeCooperacao.end(); iterProx++) {
                vector<MENSAGENS_ESCUTADAS> *nodoProx = iterProx->second;
                for(j =(iterProx->second)->begin();j!=(iterProx->second)->end();j++){
                   cout<<"valor 2: "<<j->idNodo<<"\n";
                        if(i->idNodo == j->idNodo && i->idMens == j->idMens){
                                // fazer algum processamento pra guardar os nodos que enviam as mesmas mensagens
                        }

                }
            }
        }
    }

}





/*
// método Ríad
void Basic802154::verificarRetransmissao(Basic802154Packet *rcvPacket) {
    unsigned int i;
    cout<<"Tamanho getDadosVizinhos: "<< rcvPacket->getDadosVizinhoArraySize() <<"\n";
    cout<<"Escutados do Nodo: "<< rcvPacket->getSrcID()<<"\n";
    for(int k= 0;k<cooperacoesDoBeacon.size();k++){
        cout<<"coop beacon: "<<k<<"\n";
    }
    if (rcvPacket->getDadosVizinhoArraySize() > 0) {
        if (historicoDeCooperacao.find(rcvPacket->getSrcID())
                != historicoDeCooperacao.end()) {

            map<int, vector<int>*>::iterator iter;

            iter = historicoDeCooperacao.find(rcvPacket->getSrcID());

            vector<int>* v = iter->second;

            for (i = 0; i < rcvPacket->getDadosVizinhoArraySize(); i++) {
                int nodo = rcvPacket->getDadosVizinho(i);
                v->push_back(nodo);
                cout<<"nodo: "<< nodo<<"\n";
                cooperacoesDoBeacon[nodo] = true;
            }

        } else {
            vector<int> *vetor = new vector<int>;

            for (i = 0; i < rcvPacket->getDadosVizinhoArraySize(); i++) {
                int nodo = rcvPacket->getDadosVizinho(i);
                vetor->push_back(nodo);
                cout<<"nodo: "<< nodo<<"\n";
                cooperacoesDoBeacon[nodo] = true;
            }
            historicoDeCooperacao[rcvPacket->getSrcID()] = vetor;
        }

        for(int k= 0;k<cooperacoesDoBeacon.size();k++){
                cout<<"coop beacon depois: "<<k<<"\n";
            }


    }
}*/

Basic802154Packet *Basic802154::newConnectionRequest(int PANid) {
    Basic802154Packet *result = new Basic802154Packet("PAN associate request",
            MAC_LAYER_PACKET);
    result->setDstID(PANid);
    result->setPANid(PANid);
    result->setMac802154PacketType(MAC_802154_ASSOCIATE_PACKET);
    result->setSrcID(SELF_MAC_ADDRESS);
    result->setByteLength(COMMAND_PKT_SIZE);
    return result;
}

Basic802154Packet *Basic802154::newGtsRequest(int PANid, int slots) {
    Basic802154Packet *result = new Basic802154Packet("GTS request",
            MAC_LAYER_PACKET);
    result->setPANid(PANid);
    result->setDstID(PANid);
    result->setMac802154PacketType(MAC_802154_GTS_REQUEST_PACKET);
    result->setSrcID(SELF_MAC_ADDRESS);
    result->setGTSlength(slots);
    result->setByteLength(COMMAND_PKT_SIZE);
    return result;
}

/* This function will handle a MAC frame received from the lower layer (physical or radio)
 */
void Basic802154::fromRadioLayer(cPacket * pkt, double rssi, double lqi) {
    Basic802154Packet *rcvPacket = dynamic_cast<Basic802154Packet*>(pkt);
    if (!rcvPacket) {
        return;
    }
//cout << rssi<<"\n";
//Modificação Ríad
    //cout<<"Sou o  NODO " << SELF_MAC_ADDRESS<< "Estou Na escuta\n";
    //cout<<"Pacote Recbido: "<< rcvPacket->getSrcID()<<" Energgia" <<  rcvPacket->getEnergy()<<" RSSI: "<< rcvPacket->getSomaSinais()<<"\n";
    if(userelay){
        if(rcvPacket->getMac802154PacketType() == MAC_802154_DATA_PACKET){// Coloquei esse if pq quero guardar as inf apenas dos pacotes de dados
            AtualizarVizinhaca(rcvPacket, rssi); // insere o nodo que enviou o pacote como vizinho

            if (isPANCoordinator) {
                listarNodosEscutados(rcvPacket, rssi); // insere o nodo que enviou o pacote como escutado
                verificarRetransmissao(rcvPacket);
            }

            if (cooperador) { // essa variavel é setada ao ouvir o beacon
               listarNodosEscutados(rcvPacket,rssi);// insere o nodo que enviou o pacote como escutado
            }
        }
    }
    if (rcvPacket->getDstID() != SELF_MAC_ADDRESS
            && rcvPacket->getDstID() != BROADCAST_MAC_ADDRESS) {
        return;
    }

    switch (rcvPacket->getMac802154PacketType()) {

    /* received a BEACON frame */
    case MAC_802154_BEACON_PACKET: {
        //Modificação Ríad
        if(userelay){
            //tempoDeBeacon = rcvPacket->getTempoBeacon();
            souNodoCooperante(rcvPacket);
        }
        recvBeacons++;

        if (isPANCoordinator)
            break;			//PAN coordinators ignore beacons from other PANs
        if (associatedPAN != -1 && associatedPAN != rcvPacket->getPANid())
            break;			//Ignore, if associated to another PAN

        //cancel beacon timeout message (if present)
        cancelTimer(BEACON_TIMEOUT);

        //LimparBuffer
        cout<<"Sou o nodo: "<< SELF_MAC_ADDRESS<<"\n";
        //cout<<"tamanho Buffer: "<<TXBuffer.size()<<"\n";
        limparBufferAplicacao();
//        // Gerar Mensagem da aplicação
//        RadioControlMessage *msg1 = new RadioControlMessage("BEACON_CHEGADA",BEACON_CHEGADA1);
//        msg1->setRadioControlMessageKind(BEACON_CHEGADA1);
//        handleRadioControlMessage(msg1);

        //this node is connected to this PAN (or will try to connect), update frame parameters
        double offset = TX_TIME(rcvPacket->getByteLength());
        currentFrameStart = getClock() - offset;	//frame start is in the past
        lostBeacons = 0;
        frameOrder = rcvPacket->getFrameOrder();
        beaconOrder = rcvPacket->getBeaconOrder();
        beaconInterval = baseSuperframeDuration * (1 << beaconOrder);
        macBSN = rcvPacket->getBSN();
        CAPlength = rcvPacket->getCAPlength();
        CAPend = CAPlength * baseSlotDuration * (1 << frameOrder) * symbolLen;
        GTSstart = 0;
        GTSend = 0;
        GTSlength = 0;
        //SUELEN
        GTSstartRetrans = 0;
        GTSendRetrans = 0;
        GTSlengthRetrans = 0;

        //Suelen
        if(userelay){
            primeiraRetrans = 0;
            irDormir = 0;
            inicioGTSRetrans = rcvPacket->getSlotInicioRetrans();

            //SUELEN
            cout<<"Beacon Recebido no Nodo:"<<SELF_MAC_ADDRESS << "\n";
            int teste = rcvPacket->getVizinhosOuNodosCooperantesArraySize();
            cout<< "Este Beacon informa que tem "<< teste<< " Nodos Cooperantes\n";
                if((int) rcvPacket->getGTSlistArraySize()>0 && rcvPacket->getVizinhosOuNodosCooperantesArraySize()>0){
                    // seta o tempo de inicio das retransmissões
                    primeiraRetrans = ((rcvPacket->getGTSlist(inicioGTSRetrans).start - 1)* baseSlotDuration * (1 << frameOrder) * symbolLen) + getClock();
                }
        }

        //cout<<"Devia ser Associado A: "<< rcvPacket->getPANid()<<"\n";
        //cout<<"Associado A: "<< associatedPAN<<"\n";
        for (int i = 0; i < (int) rcvPacket->getGTSlistArraySize(); i++) {
            if (rcvPacket->getGTSlist(i).owner == SELF_MAC_ADDRESS && associatedPAN == rcvPacket->getPANid()) {

                //Suelen obs:antes era só o conteudo do else
               if(rcvPacket->getGTSlist(i).retransmissor){
                   cout<< "Tamanho Lista: "<< (int) rcvPacket->getGTSlistArraySize()<< "Lista GTS["<<i<<"]: "<<rcvPacket->getGTSlist(i).owner<<" Cooperante\n";
                   GTSstartRetrans = (rcvPacket->getGTSlist(i).start - 1)
                                           * baseSlotDuration * (1 << frameOrder) * symbolLen;
                   GTSendRetrans = GTSstartRetrans
                           + rcvPacket->getGTSlist(i).length * baseSlotDuration
                                   * (1 << frameOrder) * symbolLen;
                   GTSlengthRetrans = GTSendRetrans - GTSstartRetrans;
                   trace() << "GTS slot from " << getClock() + GTSstartRetrans << " to "
                           << getClock() + GTSendRetrans << " length " << GTSlengthRetrans;
                   cout << "Nodo "<< rcvPacket->getGTSlist(i).owner  <<" Usa o GTS slot from " << getClock() + GTSstartRetrans << " to "
                           << getClock() + GTSendRetrans << " length " << GTSlengthRetrans<< "\n";

                   // set GTS timer phyDelaySleep2Tx seconds earlier as radio can be sleeping
                   setTimer(GTS_RETRANS, GTSstartRetrans - phyDelaySleep2Tx - offset);

               }else{
                cout<< "Tamanho Lista: "<< (int) rcvPacket->getGTSlistArraySize()<< "Lista GTS["<<i<<"]: "<<rcvPacket->getGTSlist(i).owner<<"\n";
                GTSstart = (rcvPacket->getGTSlist(i).start - 1)
                        * baseSlotDuration * (1 << frameOrder) * symbolLen;
                GTSend = GTSstart
                        + rcvPacket->getGTSlist(i).length * baseSlotDuration
                                * (1 << frameOrder) * symbolLen;
                GTSlength = GTSend - GTSstart;

                limparBufferAplicacao();
                // Gerar Mensagem da aplicação
                RadioControlMessage *msg1 = new RadioControlMessage("BEACON_CHEGADA",BEACON_CHEGADA1);
                msg1->setRadioControlMessageKind(BEACON_CHEGADA1);
                handleRadioControlMessage(msg1);

                trace() << "GTS slot from " << getClock() + GTSstart << " to "
                        << getClock() + GTSend << " length " << GTSlength;
                cout << "Nodo "<< rcvPacket->getGTSlist(i).owner  <<" Usa o GTS slot from " << getClock() + GTSstart << " to "<< getClock() + GTSend << " length " << GTSlength<< "\n";

                  }

              }
        }
        //Suelen
        if(userelay){
            if(associatedPAN == rcvPacket->getPANid()){
                if((int) rcvPacket->getGTSlistArraySize()>0){
                    irDormir = (((rcvPacket->getGTSlist(0).start - 1) * baseSlotDuration * (1 << frameOrder) * symbolLen)
                            + (rcvPacket->getGTSlist(0).length * baseSlotDuration * (1 << frameOrder) * symbolLen) +getClock());
                }
            }

        }

        if (associatedPAN != rcvPacket->getPANid()) {
            setTimer(BACK_TO_SETUP, CAPend - offset);
        }
        setMacState(MAC_STATE_CAP);
        if (associatedPAN == rcvPacket->getPANid()) {
            if (GTSstart != CAPend){
                //SUELEN
                //COMENTEI PQ NESTE COMANDO SÓ ACORDA EM SEU MOMENTO D TRANSMISSAO
                // set timer to sleep after CAP, unless GTS slots starts right after
                //setTimer(SLEEP_START, CAPend - offset);
            }if (GTSstart > 0) {
                // set GTS timer phyDelaySleep2Tx seconds earlier as radio can be sleeping
                //cout<<"tempo atual: "<< getClock()<<"\n";
                //cout<<"No Tempo: "<< GTSstart - phyDelaySleep2Tx - offset <<"\n";
                //setTimer(GTS_START, GTSstart - phyDelaySleep2Tx - offset);
                setTimer(GTS_START, GTSstart - phyDelaySleep2Tx - offset);
            }
        } else {
            setTimer(BACK_TO_SETUP, CAPend - offset);
        }

        receiveBeacon_node(rcvPacket);
        attemptTransmission("CAP started");
        setTimer(FRAME_START,
                baseSuperframeDuration * (1 << beaconOrder) * symbolLen
                        - guardTime - offset);
        break;
    }

        // received request to associate
    case MAC_802154_ASSOCIATE_PACKET: {

        // only PAN coordinators can accept association requests
        // if multihop communication is to be allowed - then this has to be changed
        // in particular, any FFD can become a coordinator and accept requests
        if (!isPANCoordinator)
            break;

        // if PAN id does not match - do nothing
        if (rcvPacket->getPANid() != SELF_MAC_ADDRESS)
            break;

        if (associationRequest_hub(rcvPacket)) {
            trace() << "Accepting association request from "
                    << rcvPacket->getSrcID();
            cout<< "Nodo "<< rcvPacket->getSrcID()<<" Associado \n";
            // update associatedDevices and reply with an ACK
            associatedDevices[rcvPacket->getSrcID()] = true;
            Basic802154Packet *ackPacket = new Basic802154Packet(
                    "PAN associate response", MAC_LAYER_PACKET);
            ackPacket->setPANid(SELF_MAC_ADDRESS);
            ackPacket->setMac802154PacketType(MAC_802154_ACK_PACKET);
            ackPacket->setDstID(rcvPacket->getSrcID());
            ackPacket->setByteLength(ACK_PKT_SIZE);

            //Insere na lista de associados
            nodosAssociados.push_back(rcvPacket->getSrcID());

            //Conta como filho!!!
            nchildren++;

            toRadioLayer(ackPacket);
            toRadioLayer(createRadioCommand(SET_STATE, TX));
            setTimer(ATTEMPT_TX, TX_TIME(ACK_PKT_SIZE));
        } else {
            //trace() << "Denied association request from "<< rcvPacket->getSrcID()<< ". Maximum number of children was reached = " << nchildren;
            // Need to send a packet to deny the reuqest.
            // But current implementation always accepts them
        }
        break;
    }

        // received GTS request
    case MAC_802154_GTS_REQUEST_PACKET: {

        // only PAN coordinators can accept GTS requests
        if (!isPANCoordinator)
            break;

        // if PAN id does not match - do nothing
        if (rcvPacket->getPANid() != SELF_MAC_ADDRESS)
            break;

        trace() << "Received GTS request from " << rcvPacket->getSrcID();

        // reply with an ACK
        Basic802154Packet *ackPacket = new Basic802154Packet("PAN GTS response",
                MAC_LAYER_PACKET);
        ackPacket->setPANid(SELF_MAC_ADDRESS);
        ackPacket->setMac802154PacketType(MAC_802154_ACK_PACKET);
        ackPacket->setDstID(rcvPacket->getSrcID());
        ackPacket->setByteLength(ACK_PKT_SIZE);
        // gtsRequest_hub function handles the decision on the amount of GTS slots to allocate
        ackPacket->setGTSlength(gtsRequest_hub(rcvPacket));

        toRadioLayer(ackPacket);
        toRadioLayer(createRadioCommand(SET_STATE, TX));
        setTimer(ATTEMPT_TX, TX_TIME(ACK_PKT_SIZE));

        break;
    }

        // ack frames are handled by a separate function
    case MAC_802154_ACK_PACKET: {
        if (rcvPacket->getDstID() != SELF_MAC_ADDRESS)
            break;
        handleAckPacket(rcvPacket);
        break;
    }

        // data frame
    case MAC_802154_DATA_PACKET: {
        cout<<"Eu sou o: "<< SELF_MAC_ADDRESS<< "\n";
        if(rssi > limiteRSSI){// Só irá receber a mensagem se o RSSI for bom

           if (isNotDuplicatePacket(rcvPacket)) {
                dataReceived_hub(rcvPacket);
                toNetworkLayer(decapsulatePacket(rcvPacket));
            } else {
                //trace() << "Packet [" << rcvPacket->getName() << "] from node "<< rcvPacket->getSrcID() << " is a duplicate";
                //cout<<"Duplicado\n";
            }
            // Suelen -- Variavel utilizada para determinar o numero de coop
            if(userelay){
                if (isPANCoordinator){

                   if(rcvPacket->getRetransmissao() == true){
                      msgRtrans++;
                   }else{
                       msgRecebidas++;
                       numdadosrecebidosnogtstransmissao++;
                      // if(listasgEnviadas[rcvPacket->getSrcID()] < rcvPacket->getNumMsgEnviadas()){
                           //listasgEnviadas[rcvPacket->getSrcID()] = rcvPacket->getNumMsgEnviadas();
                           //cout<<"Msgm Enviadas: "<<listasgEnviadas[rcvPacket->getSrcID()]<<"\n";
                      // }

                   }
                }
            }

            // If the frame was sent to broadcast address, nothing else needs to be done
            if (rcvPacket->getDstID() == BROADCAST_MAC_ADDRESS)
                break;

    //        if(rcvPacket->getDadosVizinhoArraySize()>0){
    //            cout<<"recebi por cooperacao:\n";
    //            for(int i=0;i<rcvPacket->getDadosVizinhoArraySize();i++){
    //                cout<<"dadosvizinhos["<<i<<"]: "<< rcvPacket->getDadosVizinho(i)<<"\n";
    //            }
    //        }

            // otherwise, generate and send an ACK
            Basic802154Packet *ackPacket = new Basic802154Packet("Ack packet",
                    MAC_LAYER_PACKET);
            ackPacket->setPANid(SELF_MAC_ADDRESS);
            ackPacket->setMac802154PacketType(MAC_802154_ACK_PACKET);
            ackPacket->setDstID(rcvPacket->getSrcID());
            ackPacket->setSeqNum(rcvPacket->getSeqNum());
            ackPacket->setByteLength(ACK_PKT_SIZE);

            toRadioLayer(ackPacket);
            toRadioLayer(createRadioCommand(SET_STATE, TX));
            setTimer(ATTEMPT_TX, TX_TIME(ACK_PKT_SIZE));

            break;
        }
    }

    default: {
        //trace() << "WARNING: unknown packet type received ["<< rcvPacket->getName() << "]";
    }
    }
}

void Basic802154::handleAckPacket(Basic802154Packet * rcvPacket) {
    if (currentPacket == NULL) {
        //trace() << "WARNING received ACK packet while currentPacket == NULL";
        return;
    }

    cancelTimer(ACK_TIMEOUT);

    switch (currentPacket->getMac802154PacketType()) {

//received an ack while waiting for a response to association request
    case MAC_802154_ASSOCIATE_PACKET: {
        associatedPAN = rcvPacket->getPANid();
        if (desyncTimeStart >= 0) {
            desyncTime += getClock() - desyncTimeStart;
            desyncTimeStart = -1;
        }
        trace() << "Associated with PAN:" << associatedPAN;
        setMacState(MAC_STATE_CAP);
        clearCurrentPacket("Success", true);
        connectedToPAN_node();
        break;
    }

        //received an ack while waiting for a response to data packet
    case MAC_802154_DATA_PACKET: {
        if (currentPacket->getSeqNum() == rcvPacket->getSeqNum()) {
            trace() << "Data packet successfully transmitted to "
                    << rcvPacket->getSrcID() << ", local clock " << getClock();
            cout << "Data packet successfully transmitted to "
                                << rcvPacket->getSrcID() << ", local clock " << getClock()<< "\n";
            clearCurrentPacket("Success", true);
        } else {
            collectPacketHistory("NoAck");
            attemptTransmission("Wrong SeqNum in Ack");
        }
        break;
    }

    case MAC_802154_GTS_REQUEST_PACKET: {
        assignedGTS_node(rcvPacket->getGTSlength());
        clearCurrentPacket("Success", true);
        break;
    }

    default: {
        //trace() << "WARNING: received unexpected ACK to packet ["<< currentPacket->getName() << "]";
        break;
    }
    }
}

/* Finishes the transmission attempt(s) for current packet
 * Records packet history and performs transmission outcome callback
 */
void Basic802154::clearCurrentPacket(const char * s, bool success) {
    if (currentPacket == NULL)
        return;
    if (s)
        collectPacketHistory(s);
    if (success)
        currentPacketSuccess = true;
    if (currentPacket->getMac802154PacketType() == MAC_802154_DATA_PACKET) {
        if (currentPacket->getDstID() != BROADCAST_MAC_ADDRESS)
            packetBreak[currentPacketHistory]++;
        else
            packetBreak["Broadcast"]++;
    }
    //trace() << "Transmission outcome for [" << currentPacket->getName() << "]: "<< currentPacketHistory;

// transmissionOutcome callback below might request another transmission by
// calling transmitPacket(). Therefore, we save and clear the currentPacket
// variable and delete it only _after_ the callback.
    Basic802154Packet *tmpPacket = currentPacket;
    currentPacket = NULL;
    transmissionOutcome(tmpPacket, currentPacketSuccess, currentPacketHistory);
    cancelAndDelete(tmpPacket);
}

// This function provides a new packet to be transmitted by the MAC
// It may not be transmitted immediately, but MAC will keep it untill 
// one of the following happens:
// 1) All transmission attempts were exausted (if specified)
// 2) Delay limit was exceeded (if specified)
// 3) Packet can not be delivered, e.g. if PAN connection is lost
// 4) transmitPacket called again, replacing the old packet
void Basic802154::transmitPacket(Basic802154Packet *pkt, int retries,
        bool state, double limit) {
    clearCurrentPacket();
    //trace() << "transmitPacket([" << pkt->getName() << "]," << retries << ","<< state << "," << limit << ")";
   // cout<< "transmitPacket([" << pkt->getName() << "]," << retries << ","<< state << "," << limit << ")\n";

    currentPacket = pkt;
    currentPacketGtsOnly = state;
    currentPacketHistory = "";
    currentPacketLimit = limit;
    currentPacketSuccess = false;
    if (currentPacket->getDstID() == BROADCAST_MAC_ADDRESS) {
        currentPacketRetries = 1;
        currentPacketResponse = 0;
    } else {
        currentPacketRetries = retries == 0 ? macMaxFrameRetries : retries;
        currentPacketResponse = ackWaitDuration;
    }
    if (getTimer(ATTEMPT_TX) < 0 && getTimer(ACK_TIMEOUT) < 0)
        attemptTransmission("transmitPacket() called");
}

// This function will initiate a transmission (or retransmission) attempt
void Basic802154::attemptTransmission(const char * descr) {
    cancelTimer(ATTEMPT_TX);
    if (macState == MAC_STATE_SLEEP || macState == MAC_STATE_SETUP)
        return;
    //Suelen
    if(currentPacket){
       // Contabiliza mensagens perdidas por timeout de ACK
        if(descr == "ACK timeout" && currentPacket->getMac802154PacketType() ==1003){
            mensagensPerdidas++;
            //limparBufferAplicacao();
            trace()<<"Mensagens Perdidas: "<<mensagensPerdidas;
            cout<<"Mensagens Perdidas: "<<mensagensPerdidas<<"\n";
        }
    }

// if a packet already queued for transmission - check avaliable retries and delay
    if (currentPacket
            && (currentPacketRetries == 0
                    || (currentPacketLimit > 0
                            && (simTime() - currentPacket->getCreationTime())
                                    > currentPacketLimit))) {
        clearCurrentPacket();
        return;
    }

    if (currentPacket) {
        if (macState == MAC_STATE_GTS) {
            //trace() << "Transmitting [" << currentPacket->getName()<< "] in GTS";
            trace() << "Transmitting Nodo [" << currentPacket->getSrcID()<< "] in GTS Transmitting [" << currentPacket->getName()<< "]";
            cout << "Nodo [" << currentPacket->getSrcID()<< "] Transmitindo no GTS\n";


            transmitCurrentPacket();
        } else if (macState == MAC_STATE_CAP && currentPacketGtsOnly == false) {
            //trace() << "Transmitting [" << currentPacket->getName()<< "] in CAP, starting CSMA_CA";
            NB = 0;
            CW = enableSlottedCSMA ? 2 : 1;
            BE = batteryLifeExtention ?
                    (macMinBE < 2 ? macMinBE : 2) : macMinBE;
            performCSMACA();
        } else {
            //trace() << "Skipping transmission attempt in CAP due to GTSonly flag";
        }
    } else {
        //trace() << "Nothing to transmit";
        //cout<<"Nothing to transmit\n";
    }
}
// método Ríad
void Basic802154::retransmitir(Basic802154Packet *nextPacket) {
    if (nodosEscutados.size() > 0) {

        //vector<int>::iterator v = nodosEscutados.begin();
        //vector<MENSAGENS_ESCUTADAS>::iterator v = nodosEscutados.begin();

        //int i = 0;

        nextPacket->setDadosVizinhoArraySize(nodosEscutados.size());

        //while (i < nodosEscutados.size()) {
        for(int i = 0; i < (int)nodosEscutados.size();i++){

            nextPacket->setDadosVizinho(i,nodosEscutados[i]);

            cout<<"Escutado id: "<< nodosEscutados[i].idNodo<<"\n";
            cout<<"Escutado mens: "<< nodosEscutados[i].idMens<<"\n";
            //i++;
        }
    }
//após enviar o nome dos nodos que foram escutados a lista local é apagada
    nodosEscutados.clear();
}

// continue CSMA-CA algorithm
void Basic802154::performCSMACA() {
//generate a random delay, multiply it by backoff period length
    int rnd = genk_intrand(1, (1 << BE) - 1) + 1;
    simtime_t CCAtime = rnd * (unitBackoffPeriod * symbolLen);

//if using slotted CSMA - need to locate backoff period boundary
    if (enableSlottedCSMA) {
        simtime_t backoffBoundary = (ceil(
                (getClock() - currentFrameStart)
                        / (unitBackoffPeriod * symbolLen))
                - (getClock() - currentFrameStart)
                        / (unitBackoffPeriod * symbolLen))
                * (unitBackoffPeriod * symbolLen);
        CCAtime += backoffBoundary;
    }

    //trace() << "CSMA/CA random backoff value: " << rnd << ", in " << CCAtime << " seconds";

//set a timer to perform carrier sense after calculated time
    setTimer(PERFORM_CCA, CCAtime);
}

/* Transmit a packet by sending it to the radio */
void Basic802154::transmitCurrentPacket() {
    if (currentPacket == NULL) {
        //trace()<< "WARNING: transmitCurrentPacket() called while currentPacket == NULL";
        return;
    }

//check if transmission is allowed given current time and tx time
    simtime_t txTime = TX_TIME(currentPacket->getByteLength())
            + currentPacketResponse;
    simtime_t txEndTime = getClock() + txTime;
    int allowTx = 1;

    if (macState == MAC_STATE_CAP) {	//currently in CAP
        if (currentFrameStart + CAPend < txEndTime && CAPend != GTSstart)
            //transmission will not fit in CAP
            allowTx = 0;
    } else if (macState == MAC_STATE_GTS) {	//currently in GTS
        if (currentFrameStart + GTSend < txEndTime)
            //transmission will not fit in GTS
            allowTx = 0;
    }

    if (allowTx) {
        if (currentPacket->getMac802154PacketType() == MAC_802154_DATA_PACKET) {
            if (macState == MAC_STATE_CAP){
                collectOutput("pkt TX state breakdown", "Contention");
            }else{
                collectOutput("pkt TX state breakdown", "Contention-free");

            }
        }
        //decrement retry counter, set transmission end timer and modify mac and radio states.
        currentPacketRetries--;
        trace() << "Nodo "<<currentPacket->getSrcID() <<" |Transmitting [" << currentPacket->getName()<< "] now, remaining attempts " << currentPacketRetries;
        //cout << "Nodo [" << currentPacket->getSrcID()<< "] Transmitindo no GTS\n";
        cout << "Transmitting [" << currentPacket->getName()<< "] now, remaining attempts " << currentPacketRetries<< "\n";
        /*if(userelay){//Suelen
        //Modificação Ríad
            retransmitir(currentPacket);
            //cout<< "transmitPacket([" << currentPacket->getName() << "]): Nodo: "<<currentPacket->getSrcID() << "\n";

        }*/

        setTimer(currentPacketResponse > 0 ? ACK_TIMEOUT : ATTEMPT_TX, txTime);
        toRadioLayer(currentPacket->dup());
        toRadioLayer(createRadioCommand(SET_STATE, TX));// realiza a transmissão
    } else {
        //transmission not allowed
        trace() << "txTime " << txTime << " CAP:"<< (currentFrameStart + CAPend - getClock()) << " GTS:"<< (currentFrameStart + GTSend - getClock());
        trace() << "Transmission of [" << currentPacket->getName()<< "] stopped, not enough time";
    }
}

// String s represents an outcome of most recent transmission attempt for 
// current packet, it is saved (appended) to the final packet history
void Basic802154::collectPacketHistory(const char *s) {
    if (!currentPacket) {
        //trace()                << "WARNING: collectPacketState called while currentPacket==NULL, string:"<< s;
        return;
    }
    if (currentPacketHistory.size()) {
        currentPacketHistory.append(",");
        currentPacketHistory.append(s);
    } else {
        currentPacketHistory = s;
    }
}

//--- DECISION LAYER FUNCTIONS ---//

// A function to accept new packet from the Network layer
// ACTION: 	check if packet can be transmitted immediately
//			otherwise accept only if there is room in the buffer
bool Basic802154::acceptNewPacket(Basic802154Packet *newPacket) {
    if (getAssociatedPAN() != -1 && getCurrentPacket() == NULL) {
        transmitPacket(newPacket);
        return true;

    }
    return bufferPacket(newPacket);
}

// A function to react to beacon reception
// ACTION:  if not associated to a PAN, create and transmit connection request
void Basic802154::receiveBeacon_node(Basic802154Packet *beacon) {
    if (getAssociatedPAN() == -1)
        transmitPacket(newConnectionRequest(beacon->getPANid()));
}

// A function to react to packet transmission callback
// ACTION: Simply transmit next packet from the buffer if associated to PAN
void Basic802154::transmissionOutcome(Basic802154Packet *pkt, bool success,
        string history) {
    if (getAssociatedPAN() != -1 && TXBuffer.size()) {
        Basic802154Packet *packet = check_and_cast<Basic802154Packet*>(
                TXBuffer.front());
        TXBuffer.pop();
        transmitPacket(packet);
    }
}

int Basic802154::associationRequest_hub(Basic802154Packet *) {
    if (nchildren < maxChild)
        return 1;
    else
        return 0;
}


//Suelen
void Basic802154::calculaNumNodosCooperantes(){
    //taxa de perda
    SamLoss = numhosts - numdadosrecebidosnogtstransmissao -1;

    std::map<int, int>::iterator iter;
    int msgEnviadasTotal = 0;

    vectortaxaperda.recordWithTimestamp(simTime(),SamLoss);

    EV << "#####numdadosrecebidosnogtstransmissao:" << numdadosrecebidosnogtstransmissao << "#####SamLoss:" << SamLoss << endl;
    cout << "#####numdadosrecebidosnogtstransmissao:" << numdadosrecebidosnogtstransmissao << "#####SamLoss:" << SamLoss << "\n";


    //reinciando a variável que contabiliza a quantidade de msg que chegaram no período de transmissão.
    numdadosrecebidosnogtstransmissao = 0;

    if (alpha > limiaralpha)
        alpha = alpha - 0.1;
    if (betha > limiarbetha)
        betha = betha - 0.1;

    EstLoss = (1 - alpha) * EstLoss + alpha * SamLoss;

    DevLoss = (1 - betha) * DevLoss + betha * fabs(SamLoss);
            //abs(SamLoss - EstLoss);


    EV << " alpha " << alpha << " betha " << betha <<  "DevLoss:" << DevLoss << "EstLoss:" << EstLoss << endl;

    numeronodoscooperantes = (int)(ceil(ganho * EstLoss + DevLoss));

    EV << "[Conferencia] numeronodoscooperantes:" << numeronodoscooperantes << endl;
    cout << "[Conferencia] numeronodoscooperantes:" << numeronodoscooperantes << "\n";

}
void Basic802154::limparBufferAplicacao(){
    while (!TXBuffer.empty()) {
        cout<<"Limpando Buffer: "<<TXBuffer.front();
        cancelAndDelete(TXBuffer.front()); // cancela todos os eventos do omnet
        TXBuffer.pop();
        mensagensPerdidas++;
        cout<<"Mens Perdidas Por limpeza de buffer: "<<mensagensPerdidas<<" Nodo: "<<SELF_MAC_ADDRESS <<"\n";
        trace()<<"Mens Perdidas Por limpeza de buffer: "<<mensagensPerdidas<<" Nodo: "<<SELF_MAC_ADDRESS <<"\n";
     }

}

void Basic802154::selecaoCoopAleatoria(Basic802154Packet *beaconPacket){
    int numCoop = 0, qntNodosCoordEscuta = 0, coop = 0;

    qntNodosCoordEscuta = neigmap.size();

    cout<<"qntNodosAssociados: "<<qntNodosCoordEscuta<<"\n";
    numCoop = rand()%(qntNodosCoordEscuta+1);
    cout<<"numCoop: "<<numCoop<<"\n";

    int j=0;
    std::map<int, Neighborhood*>::iterator iterNeighborhood;
        while(j!=numCoop){
        coop = rand()%(numhosts+1);
              for (iterNeighborhood = neigmap.begin();
                      iterNeighborhood != neigmap.end(); iterNeighborhood++) {
                  Neighborhood *nodo = iterNeighborhood->second;

                  if (nodo->nodeId == coop) {
                      nodosColaboradores.push_back(nodo->nodeId);
                      j++;
                  }

              }

       }
}






