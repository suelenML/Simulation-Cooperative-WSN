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
#define MSG_SIZE 127

// This module is virtual and can not be used directly
Define_Module(Basic802154);

void Basic802154::startup() {
    inicioSim = 0;
    inicioSim = simTime();
    printStateTransitions = par("printStateTransitions");
    if (printStateTransitions) {
        stateDescr[1000] = "MAC_STATE_SETUP";
        stateDescr[1001] = "MAC_STATE_SLEEP";
        stateDescr[1002] = "MAC_STATE_CAP";
        stateDescr[1003] = "MAC_STATE_GTS";
    }


    //Informa se deve usar retransmissão ou não
    userelay = par("userelay");
    selecao = par("selecao");
    //Informa qual das seleçoes usará
    smart = par("smart");
    smartPeriodic = par("smartPeriodic");
    aleatoria = par("aleatoria");
    oportunista = par("oportunista");
    completamenteAleatoria = par("completamenteAleatoria");
    redeDinamica = par("redeDinamica");
    numCoopMax = 40; // definido 40 como numero maximo de cooperantes na rede
    pausar = getParentModule()->getParentModule()->par("pausar");
    pausado = false;
    eventoPauseATivado = false;
    //tempAtualizVizinhanca = par("tempAtualizVizinhanca");
    tempConfig = par("tempConfig");
    limitBISelecao = 10; // limito o intervalo de seleção em 10 para não ficar muito tempo sem seleção.

    //Suelen
    beaconsPerdidos = 0;
    inicioGTSRetrans = -1;
    GTSstartRetrans = 0;
    GTSendRetrans = 0;
    GTSlengthRetrans = 0;
    second_GTSstartRetrans = 0;
    second_GTSendRetrans = 0;
    second_GTSlengthRetrans = 0;
    third_GTSstartRetrans = 0;
    third_GTSendRetrans = 0;
    third_GTSlengthRetrans = 0;

    primeiraRetrans = 0;
    irDormir = 0;
    primeiraSelecao = 0;
    start_gack = 0;
    msgEnviadas = 0;
    msgRecebidas = 0;
    msgRtrans = 0;
    beaconsRecebidos = 0;
    mensagensPerdidas = 0;
    limiteRSSI = -87;
    mensagensRecuperadas = 0;
    qntidadeVezesCooperou = 0;
    idBeacon = 0;
    //atualizarVizinhanca = 7;//5; //periodicidade para realizar a escuta dos vizinhos; // usar 5 para o adaptativo
    atualizarVizinhanca = true;
    pausaEnviada = false;

    //Informa se utilizará codificação
    useNetworkCoding = par("useNetworkCoding");
    useCoopAux = par("useCoopAux");
    useGACK = par("useGACK");


    //Vetores
    nodosEscutados.clear();
    cooperacoesDoBeacon.clear();
    historicoDeCooperacao.clear();
    listaDeNodosSoltos.clear();
    neigmap.clear();
    matrizVizinhos.clear();




    //Variaveis utilizadas para determinar o número de cooperantes
    numdadosrecebidosnogtstransmissao = 0;
    SamLoss = 0.0;
    numhosts = getParentModule()->getParentModule()->getParentModule()->par(
            "numNodes");
    vectortaxaperda.setName("Taxa de Perda");
    alpha = par("alpha");
    betha = par("betha");
    ganho = par("ganho");
    limiaralpha = par("limiaralpha");
    limiarbetha = par("limiabetha");
    EstLoss = 0.0;
    DevLoss = 0.0;
    numeronodoscooperantes = 0;
    vizinhosAntigos.clear();

    //Vetor de taxa de Sucesso
    for (int i = 0; i < numhosts; i++) {
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

    if(pausar){// simula desassociação, nodos que pausam (desassociam) por um período de tempo
        pausarNoTempo = getParentModule()->getParentModule()->par("pausarNoTempo");
        retornarNoTempo = getParentModule()->getParentModule()->par("retornarNoTempo");

        setTimer(PAUSE_NODE, getClock() + pausarNoTempo);
        cout<<"getClock() + pausarNoTempo: "<<getClock() + pausarNoTempo<<"\n";
        setTimer(RESTART_NODE, getClock() + retornarNoTempo);
        cout<<"getClock() + retornarNoTempo: "<<getClock() + retornarNoTempo<<"\n";
    }
    if(useNetworkCoding){
        codificador = new Codificador();
        sucessoMsgCodRecebida = 0;
        sucessoMsgDirRecebida = 0;
        num_msg_decod_sucess = 0;
        retransCoded = 0;
        inicializeMetric();
        inicializaMatriz();
        neigmapNodosEscutados.clear();
        neigmapNodosEscutadosRefinamento.clear();
        neigmapNodosEnviados.clear();
        nodesCanRecover.clear();
        numeroDeCoop = 0;
        secondRetrans = false;
        var_msg_perNode = 0;
        var_msg_otherNodes = 0;
        var_msg_notRecover = 0;
        if(useCoopAux){
            nodosColaboradoresAuxiliares.clear();
            cooperanteAuxiliar = false;
            cooperanteAuxiliarAuxiliar = false;
        }
        for(int i = 0; i< numhosts; i++){
            vizinhosCoop.push_back(0);
            GACK.push_back(0);
            nodesNotReceived.push_back(0);
        }
    }


    // Coordinator initialisation
    if (isPANCoordinator) {
        if (!isFFD) {
            opp_error(
                    "Only full-function devices (isFFD=true) can be PAN coordinators");
        }
        if (userelay) {
            beta1 = par("beta1");
            beta2 = par("beta2");
            beta3 = par("beta3");
            beta4 = par("beta4");

            //Limpa a lista de msg de coop repetidas
            retransmissoesRepetidas.clear();

            //Limpa a lista de mensagens retransmitidas
            historicoDeSucesso.clear();

            //Limpa a lista de mensagens retransmitidas por beacon
            historicoDeSucessoBeacon.clear();

            //Limpa a lsat de associados
            nodosAssociados.clear();

            //limpa a lista de possiveis coop
            possiveisCooperantes.clear();

            // inicializa a quantidade de mensagens que o coordenador recebeu diretamente
            qntdMsgEscutCood = 0;

            //  inicializa a quantidade de mensagens que o coordenador recebeu por cooperações
            qntdMsgRecebCoop = 0;

            // inicializa a txSucesso atual da Smart
            txSucessSmart = 0.0;

            // inicializa a txSucesso anterior da Smart
            txSucessSmartAnterior = 0.0;

            // faz um controle para saber se já é o Beacon de realizar selecao de cooperantes
            tempoDeBeacon = 0;

            // ao iniciar o coord sinaliza que os nodos devem escutar os vizinhos no periodo de config
            tempAtualizVizinhanca = true;

            // sinaliza se está em processo de selecao
            processoSelecao = false;

            // sinaliza que um cooperante saiu da area do coordenador
            coopPause = false;

            // numero de vezes que o coordenador realizou seleção de cooperantes
            numSelRealizadas = 0;

            //tempo que inicia a selecao de cooperantes
            tempExecInicio = 0;

            // tempo que termina a selecao de cooperantes
            tempExecFim = 0;

            // tempo de duracao da selecao
            tempSelecao = 0;

            // tempo que termina a selecao de cooperantes com a variavel do tipo simTime
            inicioExec = 0;

            // tempo que termina a selecao de cooperantes com a variavel do tipo simTime
            fimExec = 0;

            tempTotalSelec = 0;

            //vetor que armazena o tempo de duração de cada selecao
            times.clear();

            numCoopSel = 0;
            numMSGSolicitadas = 0;

            // Inicializa o vetor de mensagens recuperadas por retransmissao de cada nodo
            // e também o de pacotes recebidos por transmissao
            for (int i = 0; i < numhosts; i++) {
                  msgRecuperadas.push_back(0);
                  msgRecuperadasBeacon.push_back(0);
                  pacotesEscutadosT.push_back(0);

            }

            /*Variáveis para a heuristica*/
            solutionSet.clear();
            finiteSet.clear();
            auxiliarSet.clear();
            matrizAdj = new int*[numhosts];
            for (int x = 0 ; x < numhosts ; x++) {
                matrizAdj[x] = new int[numhosts];
            }



        }



        associatedPAN = SELF_MAC_ADDRESS;
        macBSN = genk_intrand(0, 255) + 1;

        //Número de nodos a serem associados ao PAN
        maxChild = par("maxChild");
        nchildren = 0; //guarda o número de filhos
        nchildrenActual = 0; // guarda o número de nodos que atualmente associado ao coordenador
        nchildrenAntigos = 0; // guardo o número de nodos que estavam associados até o envio do beacon;

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

        cout<< "beaconInterval: "<< beaconInterval <<"\n";
        cout<< "frameInterval: "<< frameInterval <<"\n";
        cout<< "CAPlength: "<< CAPlength <<"\n";

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

void Basic802154::enviarCooperantesAuxiliares(Basic802154Packet *beaconPacket) {

    int tamanhoLista = nodosColaboradoresAuxiliares.size();

    if (tamanhoLista > 0) {

        beaconPacket->setCoopAuxiliaresArraySize(tamanhoLista);
        int i;
        for (i = 0; i < tamanhoLista; i++) {
            beaconPacket->setCoopAuxiliares(i,nodosColaboradoresAuxiliares[i]);

        }
    }
}

void Basic802154::preencherListaGack(Basic802154Packet *beaconPacket) {

    int tamanhoLista = GACK.size();
    numMSGSolicitadas =0;

    if (tamanhoLista > 0) {

        beaconPacket->setGackArraySize(tamanhoLista);
        int i;
        for (i = 0; i < tamanhoLista; i++) {
            beaconPacket->setGack(i, GACK[i]);
            if(GACK[i]==0){
                numMSGSolicitadas++;
            }
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

            if (userelay) {
                if(smart){// verifica se a rede é dinamica para atualizacao da selecao de forma mais constantes
                    if(nodosEscutados.size() > 0){ //&& historicoDeSucessoBeacon.size() >0){
                        qntdMsgEscutCood = nodosEscutados.size();
                        //contabilizarRetransmissoes();
                        txSucessSmart = ((qntdMsgEscutCood*100)/nchildrenAntigos);
                        trace()<< "txSuceso Smart: "<< txSucessSmart;
                        cout << "NchildrenActual: "<< nchildrenActual << "\n";
                        cout << "NchildrenAntigos: "<< nchildrenAntigos << "\n";
                        cout << "txSuceso Smart: "<< txSucessSmart << "\n";
                    }
                    if(coopPause){
                        selecao = 2;
                        tempAtualizVizinhanca = true;
                        tempoDeBeacon = 0;
                        processoSelecao = true;
                        coopPause = false;
                    }else{
                            if(idBeacon >= tempConfig){
                                if(txSucessSmart > txSucessSmartAnterior){ //Se a taxa de sucesso atual é melhor então pode aumentar o intervalo de seleção
                                   if(!processoSelecao){
                                    if(selecao < limitBISelecao){ // para que o intervalo de seleção não ultrapasse o limitBISelecao BI
                                        selecao++;
                                        tempAtualizVizinhanca = false;
                                        //tempAtualizVizinhanca = selecao - 1;
                                        //trocar tempAtualizVizinhanca para bool, verificar se selecaao é igual a 2
                                        // se for tempAtualizVizinhanca = true, se nao false.

                                    }else{ // Se ultrapassar limitBISelecao BI então reduz
                                            selecao--;
                                            tempAtualizVizinhanca = false;
                                            //tempAtualizVizinhanca = selecao - 1;
                                    }
                                  }else{// caso contrário enviaria para os nodos escutarem os vizinhos novamente
                                      tempAtualizVizinhanca = false;
                                  }

                                }else{// Se a Tx não for melhor, entao o tempo de seleção deve ser reduzido
                                    if(txSucessSmart < txSucessSmartAnterior){
                                        if(!processoSelecao){// precisa respeitar 2 beacons até uma noca seleçao, por isso se for par pode faze caso contrário não
                                            if(selecao >= 4){// 4 é o valor inicial, e para poder reduzir a metade, caso contrario seria selecionado sempre
                                                  selecao = (int)(selecao/2);
                                                  if(selecao == 2){
                                                      tempAtualizVizinhanca = true;
                                                      tempoDeBeacon = 0;
                                                      processoSelecao = true;
                                                  }else{
                                                      tempAtualizVizinhanca = false;
                                                  }
                                                  //tempAtualizVizinhanca = selecao - 1;
                                            }else{ // se não é maior que 4, mas é maior que 2 decrementa
                                                if(selecao > 2){
                                                    selecao--;
                                                }
                                                if(selecao == 2){
                                                    tempAtualizVizinhanca = true;
                                                    tempoDeBeacon = 0;
                                                    processoSelecao = true;
                                                }else{
                                                    if(selecao != 2){
                                                    tempAtualizVizinhanca = false;
                                                    }
                                                }
                                            }

                                        }
                                        else{// caso contrário enviaria para os nodos escutarem os nizinhos novamente
                                              tempAtualizVizinhanca = false;
                                        }

                                      }else{// Se a Tx for igual a anterior reduz o tempo de seleção para tentar melhorar
                                          if(txSucessSmart < 100){// se for menor que 100% de sucesso reduz o intervalo
                                            if(!processoSelecao){
                                              if(selecao >= 4){// 4 é o valor inicial, e para poder reduzir a metade, caso contrario seria selecionado sempre
                                                      selecao = (int)(selecao/2);
                                                      if(selecao == 2){// 2 pq é o intervalo mínimo para realizar a selecao
                                                          tempAtualizVizinhanca = true;
                                                          tempoDeBeacon = 0;
                                                          processoSelecao = true;
                                                      }else{
                                                          tempAtualizVizinhanca = false;
                                                      }
                                                      //tempAtualizVizinhanca = selecao - 1;
                                                }else{ // se não é maior que 4, mas é maior que 2 decrementa
                                                    if(selecao > 2){
                                                        selecao--;
                                                    }
                                                    if(selecao == 2){
                                                        tempAtualizVizinhanca = true;
                                                        tempoDeBeacon = 0;
                                                        processoSelecao = true;
                                                    }else{
                                                        if(selecao != 2){
                                                        tempAtualizVizinhanca = false;
                                                        }
                                                    }
                                                }
                                             }else{// caso contrário enviaria para os nodos escutarem os nizinhos novamente
                                                tempAtualizVizinhanca = false;
                                            }

                                        }else{
                                                if(txSucessSmart == 100){
                                                    if(!processoSelecao){
                                                        if(selecao < limitBISelecao){ // para que o intervalo de seleção não ultrapasse o limitBISelecao BI
                                                            selecao++;
                                                            tempAtualizVizinhanca = false;
                                                            //tempAtualizVizinhanca = selecao - 1;
                                                            //trocar tempAtualizVizinhanca para bool, verificar se selecaao é igual a 2
                                                            // se for tempAtualizVizinhanca = true, se nao false.

                                                        }else{ // Se ultrapassar limitBISelecao BI então reduz
                                                                selecao--;
                                                                tempAtualizVizinhanca = false;
                                                                //tempAtualizVizinhanca = selecao - 1;
                                                        }
                                                       //tempAtualizVizinhanca = false; não faz sentido, pq se for false ele entrou no if e continuara false
                                                    }else{// caso contrário enviaria para os nodos escutarem os nizinhos novamente
                                                        tempAtualizVizinhanca = false;
                                                    }
                                                }
                                            }
                                        }

                                    }
                                }
                        }
                        txSucessSmartAnterior = txSucessSmart;
                    }

                if(smartPeriodic){
                    if(idBeacon >= tempConfig){
                        //selecao = 7;// aumenta o intervalo de selecoes

                        if(tempoDeBeacon == selecao - 2){
                            tempAtualizVizinhanca = true;
                        }else{
                            tempAtualizVizinhanca = false;
                        }
                    }

                }

                //modificação Ríad
                //no começo de um novo beacon as mensagens são contabilizadas, a lista de nodos escutada é limpada e novos nodos cooperadores podem ser selecionados
                //contabilizarMensagens();
                //verificaRetransmissoesRepetidas();
                nodosEscutados.clear();
                historicoDeSucessoBeacon.clear();
                if(useNetworkCoding){
                    neigmapNodosEscutados.clear();
                    inicializaMatriz();
                    codificador->n_equations = 0;
                    sucessoMsgCodRecebida = 0;
                    sucessoMsgDirRecebida = 0;

                }


                if (tempoDeBeacon == selecao) {
                      if(smart || smartPeriodic){
                        tempExecInicio = clock();
                        //recordScalar("SimTime Start", simTime());
                        //inicioExec =  simTime(); //getClock();
                        //cout<< "tempo de inicio "<< tempExecInicio << endl;
                        //cout<< "tempo de inicioOmnet "<< inicioExec << endl;
                        selecionaNodosSmart(beaconPacket); // Função Correta
                        if(useCoopAux){
                            selectCoopAux();

                        }
                        //vizinhanca();// Heuristica Gulosa
                        //algGenetic(); // Algoritmo Genetico

                        tempExecFim = clock();
                        //recordScalar("SimTime End", simTime());
                        //fimExec = simTime(); //getClock();
                        //cout<< "tempo de finalizacao "<< tempExecInicio << endl;
                        //cout<< "tempo de finalizacaoOmnet "<< fimExec << endl;
                        tempSelecao = (tempExecFim - tempExecInicio);
                        //cout<< "variavel tempSelecao "<< tempSelecao << "segundos" << endl;
                        tempSelecao = tempSelecao/ (double) CLOCKS_PER_SEC;
                        cout<< "tempo de duracao foi de "<< tempSelecao << "segundos" << endl;

                        //Tempo Omnet
                        //cout<< "tempo Fim  "<< SIMTIME_DBL(fimExec) << "segundos" << endl;
                        //cout<< "tempo Inicio  "<< SIMTIME_DBL(tempExecFim) << "segundos" << endl;
                        //tempSelecao = (SIMTIME_DBL(fimExec - inicioExec));
                        //tempSelecao = (SIMTIME_DBL(tempExecFim - inicioExec) / SIMTIME_DBL(simTime()));
                        //cout<< "tempo atual  "<< SIMTIME_DBL(simTime()) << "segundos" << endl;
                        //cout<< "tempo de duracao pelo OMNeT foi de "<< tempSelecao << "segundos" << endl;




                      times.push_back(tempSelecao);
                      tempSelecao = 0;
                      numSelRealizadas++;
                      numCoopSel = numCoopSel + nodosColaboradores.size();
                      cout<< "Seleções realizadas até o momento: " << numSelRealizadas << "\n";

                    }
                    if(aleatoria){
                       selecaoCoopAleatoria(beaconPacket);// Seleção Aleatória dentre os que escutam o coordenador
                       numSelRealizadas++;
                    }
                    if(oportunista){
                        selecaoOportunista(beaconPacket); // Seleção Odilson
                        numSelRealizadas++;
                    }
                    if(completamenteAleatoria){
                        selecaoCompletamenteAleatoria(beaconPacket); // Seleção Aleatoria
                        numSelRealizadas++;
                    }
                    enviarNodosCooperantes(beaconPacket);
                    if(useCoopAux){
                        enviarCooperantesAuxiliares(beaconPacket);
                    }
                    tempoDeBeacon = 0;
                    idBeacon ++;
                    beaconPacket->setTempoBeacon(tempoDeBeacon);
                    beaconPacket->setIdBeacon(idBeacon);
                    primeiraSelecao = 1;
                    tempAtualizVizinhanca = false;
                    beaconPacket->setTempAtualizVizinhanca(tempAtualizVizinhanca);
                    processoSelecao = false;

                    /*if (!redeDinamica && selecao == 4){
                        selecao = 7;
                        //tempAtualizVizinhanca = selecao - 1;
                    }*/

                  neigmap.clear();//limpa a lista de vizinhos do coord ao enviar o beacon
                } else {
                    if (primeiraSelecao == 1) {
                        //Para manter os colaboradores até a próxima seleção
                        enviarNodosCooperantes(beaconPacket);
                        if(useCoopAux){
                            enviarCooperantesAuxiliares(beaconPacket);
                        }
                    }
                    idBeacon ++;
                    tempoDeBeacon++; // Para sinalizar quando terá uma seleção
                    beaconPacket->setTempAtualizVizinhanca(tempAtualizVizinhanca);
                    beaconPacket->setIdBeacon(idBeacon);
                    beaconPacket->setTempoBeacon(tempoDeBeacon);
                    beaconPacket->setPrimeiraSelecao(primeiraSelecao);



                }
                //Suelen
                numdadosrecebidosnogtstransmissao = 0;

                if(useGACK){
                    GACK.clear();
                    for(int i = 0; i< numhosts; i++){
                       GACK.push_back(0);
                    }

                }

            }
            nchildrenAntigos = nchildrenActual;
            cout<< "nchildrenAntigos Depois " << nchildrenAntigos << "\n";
            // GTS fields and CAP length are set in the decision layer
            prepareBeacon_hub(beaconPacket);


            beaconPacket->setByteLength(
                    BASE_BEACON_PKT_SIZE
                            + beaconPacket->getGTSlistArraySize()
                                    * GTS_SPEC_FIELD_SIZE);
            CAPlength = beaconPacket->getCAPlength();
            CAPend = CAPlength * baseSlotDuration * (1 << frameOrder)
                    * symbolLen;

               /* Aqui disparo o timer para o GACK*/
            if(useGACK){
                if(beaconPacket->getGTSlistArraySize()> 0){
                    for(int i = 0; i < (int) beaconPacket->getGTSlistArraySize(); i++) {
                         if(beaconPacket->getGTSlist(i).owner == SELF_MAC_ADDRESS) {
                           start_gack = (beaconPacket->getGTSlist(i).start - 1)
                                   * baseSlotDuration * (1 << frameOrder) * symbolLen;
                           setTimer(START_GACK, start_gack);
                           break;
                       }
                     }

                }
            }

            sentBeacons++;

            cout << "Coord enviará o GACK: " << start_gack << endl;
            cout << "CAPlength = " << CAPlength<< "\n";
            cout << "CAPend = " << CAPend<< "\n";
            trace() << "Transmitting [PAN beacon packet] now, BSN = " << macBSN;
            cout << "Transmitting [PAN beacon packet] now, BSN = " << macBSN
                    << "\n";
            //neigmap.clear();//limpa a lista de vizinhos do coord ao enviar o beacon
            //trace()<<"beaconPacket->getByteLength(): "<<beaconPacket->getByteLength();
            cout<<"beaconPacket->getByteLength(): "<<beaconPacket->getByteLength()<<"\n";
            cout<<"calculo TX_TIME"<<(phyLayerOverhead + beaconPacket->getByteLength())*1/(1000*phyDataRate/8.0)<<"\n";
            setMacState(MAC_STATE_CAP);
            toRadioLayer(beaconPacket);
            toRadioLayer(createRadioCommand(SET_STATE, TX));
            setTimer(ATTEMPT_TX, TX_TIME(beaconPacket->getByteLength()));
            beaconPacket = NULL;

            currentFrameStart = getClock() + phyDelayRx2Tx;
            cout << "beaconInterval * symbolLen: "<< beaconInterval * symbolLen << "\n";
            cout << "simtime "<< simTime() << "\n";
            cout << "simtime + (beaconInterval * symbolLen: )"<< simTime() + (beaconInterval * symbolLen) << "\n";
            setTimer(FRAME_START, beaconInterval * symbolLen);
          } else {	// if not a PAN coordinator, then wait for beacon
            if(!pausado){
            //cout<<"Setar RX: "<< SELF_MAC_ADDRESS<<"\n";
            toRadioLayer(createRadioCommand(SET_STATE, RX));
            // tamanho do maior beacon*8/ 250000(taxa de transmissao 250kbps)= timeout beacon
            //410*8/250000=0,014 ---> vou deixar o guardTime*15 pois eh quase o mesmo
            setTimer(BEACON_TIMEOUT, guardTime * 15);//era 3 o default
            //trace()<<"TimeOut de Beacon em: "<< guardTime * 15;
            }
        }
        break;
    }
    case WAKE_UP_START:{
        if (macState == MAC_STATE_SLEEP && !pausado) {
            toRadioLayer(createRadioCommand(SET_STATE, RX));
        }
        break;

    }
    case PAUSE_NODE:{
//        //cancel evento para receber beacon se existir
//        cancelTimer(FRAME_START);
//        cout<<"Sou o Nodo: "<< SELF_MAC_ADDRESS <<"\n";
//        // pensar em mandar uma msg para o coord avisando da pausa
//        setMacState(MAC_STATE_CAP);
//        transmitPacket(pausarNodo(0));
//        trace() << "Node: "<< SELF_MAC_ADDRESS <<"Transmitting [PAN pause node]";
//        cout << "Node: "<< SELF_MAC_ADDRESS <<"Transmitting [PAN pause node] \n";
//
//        toRadioLayer(createRadioCommand(SET_STATE, TX));// coloca o radio em modo TX
//        pausaEnviada = true;
          eventoPauseATivado = true;

        break;

    }
    case RESTART_NODE:{
        cout<<"Sou o Nodo: "<< SELF_MAC_ADDRESS <<" Estou reiniciando\n";
        // pensar em mandar uma msg para o coord avisando que retornou da pausa
        //setMacState(MAC_STATE_CAP);
        //transmitPacket(restartNodo(0)); // passo o id do nodo coordenador
        //toRadioLayer(restartNodo(0));// passo o id do nodo coordenador
        //trace() << "Node: "<< SELF_MAC_ADDRESS <<"Transmitting [PAN restart node]";
        //cout << "Node: "<< SELF_MAC_ADDRESS <<"Transmitting [PAN restart node] \n";

        //toRadioLayer(createRadioCommand(SET_STATE, TX));// coloca o radio em modo TX

        // Aqui colocar qndo ele receber o ack
        trace()<<"Sou o nodo["<<SELF_MAC_ADDRESS <<"] Estou Reiniciando";
        toRadioLayer(createRadioCommand(SET_STATE, RX));
        pausado = false;
        eventoPauseATivado = false;
        break;

        }

    case GTS_START: {
        if(!pausado){//Suelen
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
            if (userelay){
                if ((cooperador) || (cooperanteAuxiliar) || (cooperanteAuxiliarAuxiliar)) {
                    //Se for cooperante só dorme depois da retransmissão
                    // inform the decision layer that GTS has started
                    startedGTS_node();
                    break;
                } else{
                    if(smart || smartPeriodic){
                        trace()<<"idBeacon GTS_START: "<<idBeacon;
                        if(idBeacon < tempConfig){ //período de configuração da rede, escuta os vizinhos nos primeiros 5 beacons
                            if (primeiraRetrans > 0) {
                                setTimer(SLEEP_START, primeiraRetrans - simTime() -phyDelaySleep2Tx);
                               // inform the decision layer that GTS has started
                               startedGTS_node();
                               break;
                            }else{
                                setTimer(SLEEP_START, irDormir - simTime());
                               // inform the decision layer that GTS has started
                               startedGTS_node();
                               break;
                            }
                        }else{//Nao é periodo de configuraçao da rede
                               // se não for cooperante verificará se é o momento de atualizar a vizinhança ou não
                               //if(idBeacon == atualizarVizinhanca){ // verifica se ja está no periodo de escutar os vizinhos
                               if(atualizarVizinhanca){
                                   //atualizarVizinhanca = idBeacon + tempAtualizVizinhanca;
                                   trace()<<"idBeacon GTS_START: "<<idBeacon;
                                   trace()<<"atualizarVizinhanca: "<<atualizarVizinhanca;
                                       if (primeiraRetrans > 0) {    // Sinaliza o fim das transmissões
                                           // Se não for cooperante e existir retransmissores, dorme ao iniciar a retransmissao
                                           setTimer(SLEEP_START, primeiraRetrans - simTime() -phyDelaySleep2Tx);

                                           // inform the decision layer that GTS has started
                                           startedGTS_node();
                                           break;
                                       } else {     // Se não existir retransmissões
                                                    // set a timer to go to sleep after GTS slot ends
                                           setTimer(SLEEP_START, irDormir - simTime());
                                           // inform the decision layer that GTS has started
                                           startedGTS_node();
                                           break;
                                       }
                                  }else{// dorme apos transmitir (não fica escutando os vizinhos)
                                      setTimer(SLEEP_START, phyDelaySleep2Tx + GTSlength);
                                      // inform the decision layer that GTS has started
                                      startedGTS_node();
                                      break;
                                   }
                                   //Esse bloco comentado foi antes de mudar para os nodos do smart dormirem as vezes
                                  //if(/*tempoDeBeacon >= selecao -2 &&*/ smart){ // -2 pq ele preenche a lista de vizinhos ao receber o beacon, assim no 2º beacon fica escutando no 3 envia qm escutou e no 4 o coor seleciona os coop
                                    //if (primeiraRetrans > 0) {    // Sinaliza o fim das transmissões
                                        // Se não for cooperante e existir retranmissores, dorme ao iniciar a retransmissao
                                       // setTimer(SLEEP_START, primeiraRetrans - simTime() -phyDelaySleep2Tx);

                                        // inform the decision layer that GTS has started
                                        //startedGTS_node();
                                        //break;
                                    //} else {                     // Se não existir retransmissões
                                                                 // set a timer to go to sleep after this GTS slot ends
                                        //setTimer(SLEEP_START, irDormir - simTime());
                                        // inform the decision layer that GTS has started
                                        //startedGTS_node();
                                        //break;
                                    //}
                                  }
                        }else{
                            // set a timer to go to sleep after this GTS slot ends
                            setTimer(SLEEP_START, phyDelaySleep2Tx + GTSlength);
                            // inform the decision layer that GTS has started
                            startedGTS_node();
                            break;
                            }
                    }
                    /*// Era isso antes
                    if (primeiraRetrans > 0) {    // Sinaliza o fim das transmissões
                        // Se não for cooperante e existir retranmissores, dorme ao iniciar a retransmissao
                        setTimer(SLEEP_START, primeiraRetrans - simTime() -phyDelaySleep2Tx);

                        // inform the decision layer that GTS has started
                        startedGTS_node();
                        break;
                    } else {                     // Se não existir retransmissões
                                                 // set a timer to go to sleep after this GTS slot ends
                        setTimer(SLEEP_START, irDormir - simTime());
                        // inform the decision layer that GTS has started
                        startedGTS_node();
                        break;
                    }
                }*/
            }

            //limpa o buffer da aplicacao antes de dormir
            //setTimer(CLEAR_BUFFER_APP, GTSlength);
            // set a timer to go to sleep after this GTS slot ends
            setTimer(SLEEP_START, phyDelaySleep2Tx + GTSlength);

            // inform the decision layer that GTS has started
            startedGTS_node();
        }
        break;
    }

        // beacon timeout fired - indicates that beacon was missed by this node
    case BEACON_TIMEOUT: {
        if(!pausado){
            trace()<<"TimeoutBeacon Nodo: "<<SELF_MAC_ADDRESS;
            lostBeacons++;
            //Limpa o Buffer da App
            if (!TXBuffer.empty()) {
                limparBufferAplicacao();
            } else {
                mensagensPerdidas++;
            }
            //cMessage *msg = new cMessage("BEACON",BEACON);
            //handleRadioControlMessage(msg);

            if (lostBeacons >= maxLostBeacons) {
                trace() << "Lost synchronisation with PAN " << associatedPAN;
                setMacState(MAC_STATE_SETUP);
                associatedPAN = -1;
                desyncTimeStart = getClock();
                disconnectedFromPAN_node();
                std::vector<int>::iterator iter;
                 int i=0;
                 for (iter = nodosAssociados.begin();iter != nodosAssociados.end(); iter++) {
                     if(nodosAssociados[i] == SELF_MAC_ADDRESS){
                         nodosAssociados.erase(iter);
                         break;
                     }
                     i++;
                 }

                if (currentPacket)
                    clearCurrentPacket("No PAN");
            } else if (associatedPAN != -1) {
                trace() << "Missed beacon from PAN " << associatedPAN
                        << ", will wake up to receive next beacon in "
                        << beaconInterval * symbolLen - guardTime * 15 << " seconds";
                beaconsPerdidos++;
                cout << "Beacon Perdido NODO: " << SELF_MAC_ADDRESS << "\n";
                setMacState(MAC_STATE_SLEEP);
                toRadioLayer(createRadioCommand(SET_STATE, SLEEP));
                setTimer(FRAME_START, beaconInterval * symbolLen - guardTime * 15);
            }
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
        cout << "Sou o nodo: "<< SELF_MAC_ADDRESS << "\n";
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
    case GTS_RETRANS: {
        if(!pausado){
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


            cout << "GTSend: "<< GTSend <<"\n";
            cout << "GTSstart: "<< GTSend <<"\n";
            cout << "GTSlength: "<< GTSend <<"\n";
            cout << "Sou o nodo: "<< SELF_MAC_ADDRESS <<endl;






            /*Basic802154Packet *packetRetrans = new Basic802154Packet(
                    "Retransmissao", MAC_LAYER_PACKET);
            packetRetrans->setPANid(SELF_MAC_ADDRESS);
            packetRetrans->setMac802154PacketType(MAC_802154_DATA_PACKET);
            packetRetrans->setSrcID(SELF_MAC_ADDRESS);
            packetRetrans->setSource(SELF_MAC_ADDRESS);
            //packetRetrans->setSeqNum(seqNum++);
            packetRetrans->setDstID(0);
            packetRetrans->setByteLength(COMMAND_PKT_SIZE);
            packetRetrans->setRetransmissao(true);
            retransmitir(packetRetrans);

            if(useNetworkCoding){
               std::map<int, MessagesNeighborhood*>::iterator iter;
                for (iter = neigmapNodosEscutados.begin(); iter != neigmapNodosEscutados.end(); iter++) {
                    if (iter->first == SELF_MAC_ADDRESS){
                        MessagesNeighborhood *neig = iter->second;
                        neigmapNodosEscutadosRefinamento[iter->first] = neig;
                    }
                }
                retransmissaoNetworkCoding(packetRetrans);
                relayNetworkCoding++;

           }

             transmitPacket(packetRetrans);*/
            retransmissionGTS();
            if(useCoopAux){
                setTimer(SLEEP_START, phyDelaySleep2Tx + GTSlength);
            }

             if(!useNetworkCoding){
                 qntidadeVezesCooperou = qntidadeVezesCooperou + 1;

                 //limpa o buffer da aplicacao antes de dormir
                 //setTimer(CLEAR_BUFFER_APP, GTSlength);
                 // set a timer to go to sleep after this GTS slot ends

                 setTimer(SLEEP_START, phyDelaySleep2Tx + GTSlength);
             }
        }
        break;
    }

    case GTS_SECOND_RETRANS: {
        if(!pausado){
            if (macState == MAC_STATE_SLEEP) {
                toRadioLayer(createRadioCommand(SET_STATE, RX));
            }
            setMacState(MAC_STATE_GTS);

            if(useNetworkCoding){
                secondRetrans = true;
                cout << "Sou o nodo: "<< SELF_MAC_ADDRESS <<endl;
                GTSend = second_GTSendRetrans;
                GTSstart = second_GTSstartRetrans;
                GTSlength = second_GTSlengthRetrans;
                retransmissionGTS();
                qntidadeVezesCooperou = qntidadeVezesCooperou + 1;
                //relayNetworkCoding++;

               //limpa o buffer da aplicacao antes de dormir
               //setTimer(CLEAR_BUFFER_APP, GTSlength);
               // set a timer to go to sleep after this GTS slot ends

               //setTimer(SLEEP_START, phyDelaySleep2Tx + GTSlength);
            }
        }

        break;
    }

    case GTS_THIRD_RETRANS: {
            if(!pausado){
                if (macState == MAC_STATE_SLEEP) {
                    toRadioLayer(createRadioCommand(SET_STATE, RX));
                }
                setMacState(MAC_STATE_GTS);

                if(useNetworkCoding){
                    secondRetrans = true;
                    cout << "Sou o nodo: "<< SELF_MAC_ADDRESS <<endl;
                    GTSend = third_GTSendRetrans;
                    GTSstart = third_GTSstartRetrans;
                    GTSlength = third_GTSlengthRetrans;
                    retransmissionGTS();
                    qntidadeVezesCooperou = qntidadeVezesCooperou + 1;
                    //relayNetworkCoding++;

                   //limpa o buffer da aplicacao antes de dormir
                   //setTimer(CLEAR_BUFFER_APP, GTSlength);
                   // set a timer to go to sleep after this GTS slot ends

                   setTimer(SLEEP_START, phyDelaySleep2Tx + GTSlength);
                }
            }

            break;
        }

    case START_GACK:{
        if (macState == MAC_STATE_SLEEP) {
            toRadioLayer(createRadioCommand(SET_STATE, RX));
        }
        //setMacState(MAC_STATE_GTS);

        // we delay transmission attempt by the time required by radio to wake up
        // note that GTS_START timer was scheduled exactly phyDelaySleep2Tx seconds
        // earlier than the actual start time of GTS slot
        //setTimer(ATTEMPT_TX, phyDelaySleep2Tx);

        limparBufferAplicacao();

        //Aqui montar a msg que o cood vai enviar com o gack
        Basic802154Packet *packetGACK = new Basic802154Packet(
                     "GroupACK", MAC_LAYER_PACKET);
        packetGACK->setDstID(BROADCAST_MAC_ADDRESS);
        packetGACK->setPANid(SELF_MAC_ADDRESS);
        packetGACK->setMac802154PacketType(MAC_802154_GACK);
        packetGACK->setSrcID(SELF_MAC_ADDRESS);
        packetGACK->setSource(SELF_MAC_ADDRESS);
        packetGACK->setByteLength(BASE_BEACON_PKT_SIZE + packetGACK->getGackArraySize());
        preencherListaGack(packetGACK);



        //transmitPacket(packetGACK);
        setMacState(MAC_STATE_CAP);
       toRadioLayer(packetGACK);
       toRadioLayer(createRadioCommand(SET_STATE, TX));
       setTimer(ATTEMPT_TX, TX_TIME(packetGACK->getByteLength()));




        //toRadioLayer(createRadioCommand(SET_STATE, RX));
        //setMacState(MAC_STATE_CAP);

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
//Este método monta o pacote de retransmissão e envia
void Basic802154:: retransmissionGTS(){
    Basic802154Packet *packetRetrans = new Basic802154Packet(
             "Retransmissao", MAC_LAYER_PACKET);
     packetRetrans->setPANid(SELF_MAC_ADDRESS);
     packetRetrans->setMac802154PacketType(MAC_802154_DATA_PACKET);
     packetRetrans->setSrcID(SELF_MAC_ADDRESS);
     packetRetrans->setSource(SELF_MAC_ADDRESS);
     //packetRetrans->setSeqNum(seqNum++);
     packetRetrans->setDstID(0);
     packetRetrans->setByteLength(COMMAND_PKT_SIZE);
     packetRetrans->setRetransmissao(true);
     retransmitir(packetRetrans);

     if(useNetworkCoding){
         /*Aqui retiro da lista de escutados os outros nodos que são cooperantes*/
         std::map<int, MessagesNeighborhood*>::iterator ite;
         for (ite = neigmapNodosEscutados.begin(); ite != neigmapNodosEscutados.end(); ite++ ) {
             for(int i = 0; i< numeroDeCoop; i++){
                 if ((ite->first == vizinhosCoop[i]) && (vizinhosCoop[i]!= SELF_MAC_ADDRESS )){
                     ite = neigmapNodosEscutados.erase(ite);
                     ite--;
                     break;
                 }
             }
         }
         retransmissaoNetworkCoding(packetRetrans);
         relayNetworkCoding++;

    }

      transmitPacket(packetRetrans);

}



/*Este método chama a codificação, insere a mensagem codificada no pacote de retransmissão e seta os coeficientes utilizados na retransmissão*/

void Basic802154:: retransmissaoNetworkCoding(Basic802154Packet *packetRetrans){
    uint8_t byte = 0;
   if(useCoopAux){
       selectMsgNetworkCodingAux();
   }else{
       selectMsgNetworkCoding();
   }

   parseToVector();
   codificador->encode_messages(SELF_MAC_ADDRESS);
   for(int i=0;i<MSG_SIZE;i++){
       byte = codificador->msg_array[i];
       packetRetrans->setPayload(i,byte);
   }
   for(int i=1;i<numhosts;i++){
       packetRetrans->setCoeficiente(i,codificador->coeficientes[i]);

   }

   if(useNetworkCoding){
       neigmapNodosEscutadosRefinamento.clear();
        //neigmapNodosEscutados.clear();
    }

}

/*Metodo verifica as mensagens que o coordenador não recebeu
 * e que o cooperante escutou e armazena as msg na estrutura nodesCanRecover*/
void Basic802154:: msgNotReceived(){
    int numMsgLost = nodesNotReceived.size();
    std::map<int, MessagesNeighborhood*>::iterator iter;
    numberRecover = 0;
    nodesCanRecover.clear();

    for (iter = neigmapNodosEscutados.begin(); iter != neigmapNodosEscutados.end(); iter++) {
        for(int i = 1; i < numMsgLost; i++){
            if(iter->first == nodesNotReceived[i]){
                numberRecover++;

                MessagesNeighborhood *neig = iter->second;
                nodesCanRecover[iter->first] = neig;
                //iter = neigmapNodosEscutados.erase(iter);
                //iter--;
                break;
            }
        }
    }


}

/*Este método seleciona apenas três mensagens das escutadas para enviar*/
void Basic802154::preSelectMsgNetworkCoding(){
    std::map<int, vector<int>>::iterator iterNeighborhood; // iterar em matrizVizinhos
    std::map<int, MessagesNeighborhood*>::iterator iter; // iterar em nodesCanRecover
    vector<int> listaVizinhos;
    bool notRecover=true;

    otherRecover.clear();

    for (iter = nodesCanRecover.begin(); iter != nodesCanRecover.end(); iter++) {
        notRecover = true;
        for(int i = 0;i < numeroDeCoop; i++){
            if(vizinhosCoop[i] != SELF_MAC_ADDRESS){
                iterNeighborhood = matrizVizinhos.find(vizinhosCoop[i]);
                if (iterNeighborhood != matrizVizinhos.end()) { //existe na matriz
                    listaVizinhos = iterNeighborhood->second;
                    for(int j = 0; j < (int)listaVizinhos.size();j++){
                        if(iter->first == listaVizinhos[j]){
                            MessagesNeighborhood *neig = iter->second;
                            otherRecover[iter->first] = neig;
                            notRecover = false;
                        }
                    }
                }
            }
        }
        if(notRecover){
            MessagesNeighborhood *neig = iter->second;
            neigmapNodosEscutadosSelecionados[iter->first] = neig;
        }
    }


}
/*
 * Este método considera que o coordenador selecionou
 * os cooperantes principais e auxiliares para ajudar
 * os principais.
 *
 * Aqui cada cooperante faz a intersecção das suas msgs escutadas com
 * as que o coordenador perdeu e com as dos seus auxiliares. Salva o
 * resultado em neigmapNodosEscutadosRefinamento.
 * */
void Basic802154::selectMsgCoopAux(){
    /*Deixa em uma estrutura as msgs escutadas e que o coord não recebeu*/
    /*Verificar as msg em comum entre o Coop Principal e os coop Aux*/

    std::map<int, vector<int>>::iterator iterNeighborhood1;
    std::map<int, vector<int>>::iterator iterNeighborhood2;
    std::map<int, MessagesNeighborhood*>::iterator iter;

    vector<int> listaVizinhos1;
    vector<int> listaVizinhos2;
    vector<int> listaAux;
    int numMsg;
    bool exist1 = false;
    bool exist2 = false;
    if(cooperador){
        listaAux = coopAuxPerNode;
    }else{
        if(cooperanteAuxiliar || cooperanteAuxiliarAuxiliar){
            listaAux = coopHelped;
        }
    }
    nodesCanRecoverInCommon.clear();
    listaVizinhos1.clear();
    listaVizinhos2.clear();
    msgNotReceived();// armazena o resultado em nodesCanRecover
    numMsg = nodesCanRecover.size();
    if(numMsg > 0){
        for (iter = nodesCanRecover.begin(); iter != nodesCanRecover.end(); iter++) {
            //for(int i = 0; i < (int) listaAux.size(); i++){
            if(listaAux.size()==2){ // significa que há dois auxiliares
                iterNeighborhood1 = matrizVizinhos.find(listaAux[0]);
                iterNeighborhood2 = matrizVizinhos.find(listaAux[1]);
                if (iterNeighborhood1 != matrizVizinhos.end()){
                    if (iterNeighborhood2 != matrizVizinhos.end()){
                        listaVizinhos1 = iterNeighborhood1->second;
                        listaVizinhos2 = iterNeighborhood2->second;
                        exist1 = false;
                        exist2 = false;
                        exist1 = std::find(listaVizinhos1.begin(), listaVizinhos1.end(), iter->first) != listaVizinhos1.end();
                        exist2 = std::find(listaVizinhos2.begin(), listaVizinhos2.end(), iter->first) != listaVizinhos2.end();
                        if(exist1 && exist2){
                            MessagesNeighborhood *neig = iter->second;
                            nodesCanRecoverInCommon[iter->first] = neig;
                        }
                    }
                }

            }else{
                if(listaAux.size() == 1){// Há apenas um auxiliar
                    iterNeighborhood1 = matrizVizinhos.find(listaAux[0]);
                    if (iterNeighborhood1 != matrizVizinhos.end()) {
                        listaVizinhos1 = iterNeighborhood1->second;
                        if (std::find(listaVizinhos1.begin(), listaVizinhos1.end(), iter->first) != listaVizinhos1.end()){
                            MessagesNeighborhood *neig = iter->second;
                            nodesCanRecoverInCommon[iter->first] = neig;
                        }
                    }
                }
            }
       }
    }


}

/*
 * coop envia: A + B + C
 * Aux1 envia: B + C
 * Aux2 envia: proprioAux + C
 * */
void Basic802154::selectMsgNetworkCodingAux(){
    std::map<int, MessagesNeighborhood*>::iterator iter;
    std::map<int, MessagesNeighborhood*>::iterator iterNodes;
    int numMsg = 0;
    selectMsgCoopAux();

    neigmapNodosEscutadosRefinamento.clear();
    if(cooperador){
        numMsg = 0;
        if(nodesCanRecoverInCommon.size()>0){
            for (iter = nodesCanRecoverInCommon.begin(); iter != nodesCanRecoverInCommon.end(); iter++) {
                MessagesNeighborhood *neig = iter->second;
                neigmapNodosEscutadosRefinamento[iter->first] = neig;
                //iter = nodesCanRecoverInCommon.erase(iter);
                //iter--;
                numMsg++;
                if(numMsg == 3){// significa que já há três msg prontas para codificar
                    break;
                }
            }
        }

    }else{
        if(cooperanteAuxiliar){
            //inicia o for já na segunda msg
            if(nodesCanRecoverInCommon.size() > 1){// considerando que quero a segunda
            numMsg = 0;
            iter = nodesCanRecoverInCommon.begin();
                for (++iter; iter != nodesCanRecoverInCommon.end(); iter++) {
                    MessagesNeighborhood *neig = iter->second;
                    neigmapNodosEscutadosRefinamento[iter->first] = neig;
                    //iter = nodesCanRecoverInCommon.erase(iter);
                    //iter--;
                    numMsg++;
                    if(numMsg == 2){// significa que já há duas msg prontas para codificar
                        break;
                    }
                }
            }

        }else{
            if(cooperanteAuxiliarAuxiliar){
                // Insere a msg do proprio auxiliar
                for (iterNodes = neigmapNodosEscutados.begin(); iterNodes != neigmapNodosEscutados.end(); iterNodes++) {
                        if(iterNodes->first == SELF_MAC_ADDRESS){
                            MessagesNeighborhood *neig = iterNodes->second;
                            neigmapNodosEscutadosRefinamento[iterNodes->first] = neig;
                            break;
                        }
                    }
                // inicia o for na terceira msg
                numMsg = 0;
                if(nodesCanRecoverInCommon.size()>2){// considerando que quero a terceira msg
                    iter = ++nodesCanRecoverInCommon.begin();
                    for (++iter; iter != nodesCanRecoverInCommon.end(); iter++) {
                           MessagesNeighborhood *neig = iter->second;
                           neigmapNodosEscutadosRefinamento[iter->first] = neig;
                           //iter = nodesCanRecoverInCommon.erase(iter);
                           //iter--;
                           numMsg++;
                           if(numMsg == 1){// significa que já há duas msg prontas para codificar
                               break;
                           }
                       }
                }else{
                    for (iter = nodesCanRecoverInCommon.begin(); iter != nodesCanRecoverInCommon.end(); iter++) {
                       MessagesNeighborhood *neig = iter->second;
                       neigmapNodosEscutadosRefinamento[iter->first] = neig;
                   }

                }
            }

        }
    }
}


/*Este método seleciona mensagens das escutadas para enviar*/
void Basic802154::selectMsgNetworkCoding(){
    std::map<int, MessagesNeighborhood*>::iterator iter;
    std::map<int, MessagesNeighborhood*>::iterator iterNodes;
    int random_pos = 0;
    srand(time(NULL));
    int numMsg, numMsgOther;
    int k,j;
    numMsg = 0;
    numMsgOther = 0;

    if(!secondRetrans){
            msgNotReceived();
            preSelectMsgNetworkCoding();
    }

    neigmapNodosEscutadosRefinamento.clear();
    /*Aqui coloca a mensagem do próprio nodo para codificar*/
    for (iterNodes = neigmapNodosEscutados.begin(); iterNodes != neigmapNodosEscutados.end(); iterNodes++) {
        if(iterNodes->first == SELF_MAC_ADDRESS){
            MessagesNeighborhood *neig = iterNodes->second;
            neigmapNodosEscutadosRefinamento[iterNodes->first] = neig;
            break;
        }
    }
    numMsg = neigmapNodosEscutadosSelecionados.size();
    if(numMsg > 0){
        var_msg_perNode = var_msg_perNode + numMsg;
        /*Aqui seleciona uma mensagem que o coordenador não recebeu para codificar
         * Se não funcionar adequadamente posso selecionar uma msg que o coordenador já tem*/
        for (iter = neigmapNodosEscutadosSelecionados.begin(); iter != neigmapNodosEscutadosSelecionados.end(); iter++) {
            MessagesNeighborhood *neig = iter->second;
            neigmapNodosEscutadosRefinamento[iter->first] = neig;
            iter = neigmapNodosEscutadosSelecionados.erase(iter);
            break;
        }
        /*Aqui pega aleatoriamente uma mensagem escutada por mais nodos para codificar*/
        numMsgOther = otherRecover.size();
        if(numMsgOther>0){
            var_msg_otherNodes = var_msg_otherNodes + numMsgOther;
            bool escutada = false;
            while(!escutada){
            random_pos = (rand() % numhosts) + 1;
            iter = otherRecover.find(random_pos);
                if(iter !=otherRecover.end()){
                    MessagesNeighborhood *neig = iter->second;
                    neigmapNodosEscutadosRefinamento[iter->first] = neig;
                    iter =  otherRecover.erase(iter);
                    escutada = true;
                }
            }
       }
    }else{
        if(nodesCanRecover.size()>0){
            k = 0;
            while (k!=3){
                random_pos = (rand() % numhosts) + 1;
                cout<<"sou o nodo: " << SELF_MAC_ADDRESS <<"\n";
                iter = nodesCanRecover.find(random_pos);
                 if(iter !=nodesCanRecover.end()){
                     MessagesNeighborhood *neig = iter->second;
                     neigmapNodosEscutadosRefinamento[iter->first] = neig;
                     iter =  nodesCanRecover.erase(iter);
                     k++;
                    if((int)nodesCanRecover.size() <= k){
                        k=3;
                    }
                }
            }
        }else
        if(neigmapNodosEscutados.size()>0){
            k = 0;
            while (k!=3){
                random_pos = (rand() % numhosts) + 1;
                iter = neigmapNodosEscutados.find(random_pos);
                if(iter !=neigmapNodosEscutados.end()){
                     MessagesNeighborhood *neig = iter->second;
                     neigmapNodosEscutadosRefinamento[iter->first] = neig;
                     k++;
                     if((int)neigmapNodosEscutados.size() <= k){
                         k=3;
                     }
                }
             }
        }
    }













  /*  Fazer uma cópia da estrutura que armazena as msg  para retransmitir
     * selecionar duas msg de forma aleatoria e mais a do próprio nodo e
     * inserir essa nova estrutura no parse vector.
    std::map<int, MessagesNeighborhood*>::iterator iter;
    int numberNeighborRec = 0;
    int numberNeighborCoop = 0;
    int random_pos = 0, i = 0, j;
    numberNeighborCoop = neigmapNodosEscutados.size();
    numberNeighborRec = nodesCanRecover.size();
    if(numberNeighborRec >= 2){// verifica se vou poder selecionar mensagens para codificar
        Seleciona uma mensagem da lista que o coordenadore não recebeu
        while (i<=1){
         random_pos = (rand() % numberNeighborRec) + 1;
         j = 1;
         for (iter = nodesCanRecover.begin(); iter != nodesCanRecover.end(); iter++) {
             if(j==random_pos){
                 MessagesNeighborhood *neig = iter->second;
                 neigmapNodosEscutadosRefinamento[iter->first] = neig;
                 neigmapNodosEnviados[iter->first] = neig;
                 nodesCanRecover.erase(iter);
                 i++;
                 break;
             }
         }

        }
//PAREI AQUI, AQUI VER COMO FICARIA SE FOR USAR A MATRIZ DE ADJACENCIA EM CADA COOP

        Seleciona uma mensagem da lista que o coordenador conhece
        while (i<2){
            random_pos = (rand() % numberNeighborCoop) + 1;
            j = 1;
            cout<<"sou o nodo: " << SELF_MAC_ADDRESS <<"\n";
            for (iter = neigmapNodosEscutados.begin(); iter != neigmapNodosEscutados.end(); iter++) {
                if((j==random_pos) && (iter->first!= SELF_MAC_ADDRESS)){
                    MessagesNeighborhood *neig = iter->second;
                    neigmapNodosEscutadosRefinamento[iter->first] = neig;
                    //neigmapNodosEscutados.erase(iter);

                    i++;
                    break;
                }
                j++;
            }
        }
    }else{
        for (iter = neigmapNodosEscutados.begin(); iter != neigmapNodosEscutados.end(); iter++) {
            if(iter->first == SELF_MAC_ADDRESS){
                MessagesNeighborhood *neig = iter->second;
                nodesCanRecover[iter->first] = neig;
            }
        }
        neigmapNodosEscutadosRefinamento =  nodesCanRecover;
    }*/

}



// método Ríad: insere no pacote as informações utilizadas para realizar a seleção dos cooperantes
void Basic802154::preencherDados(Basic802154Packet *macPacket) {
    trace() << "Vizinhos Enviados Ao Coord: "<<neigmap.size() ;
    trace() << "Eu sou o: " << SELF_MAC_ADDRESS;
    if (neigmap.size() > 0) {

        //Aqui seria o local para realizar a comparação entre as duas listas, vizinhança atual (neigmap) e a antiga (vizinhosAntigos)

        unsigned i = neigmap.size();
        std::map<int, Neighborhood*>::iterator iter;

        macPacket->setVizinhosOuNodosCooperantesArraySize(i);
        macPacket->setSomaSinais(somaDeSinais);
        macPacket->setEnergy(resMgrModule->getRemainingEnergy()/ resMgrModule->getInitialEnergy()); // Considera a energia restante no nodo
        //macPacket->setEnergy(resMgrModule->getSpentEnergy()); //Considera a energia gasta
        //cout<<"Energia Gasta: "<< resMgrModule->getSpentEnergy()<< "  Nodo: "<< SELF_MAC_ADDRESS<<"\n";
        //cout<<"Energia rstante: "<< resMgrModule->getRemainingEnergy()<< "  Nodo: "<< SELF_MAC_ADDRESS<<"\n";
        //limpar a vizinhança antiga  Aqui
        vizinhosAntigos.clear();
        i = 0;
        for (iter = neigmap.begin(); iter != neigmap.end(); iter++) {
            Neighborhood *nodo = iter->second;
            trace() << "Vizinho["<<i<<"]: " << nodo->nodeId;
            vizinhosAntigos.push_back(nodo->nodeId);
            macPacket->setVizinhosOuNodosCooperantes(i, nodo->nodeId);
            i++;
        }

        //limpar a vizinhança atual Aqui
       //neigmap.clear();
    }

}

/* A packet is received from upper layer (Network) */
void Basic802154::fromNetworkLayer(cPacket * pkt, int dstMacAddress) {
    Basic802154Packet *macPacket = new Basic802154Packet(
            "802.15.4 MAC data packet", MAC_LAYER_PACKET);

     //modificação Ríad
    if (userelay) {
        preencherDados(macPacket);
    }
    if(useNetworkCoding){
        int idNode = SELF_MAC_ADDRESS;
        int cont = 0;
        macPacket->setPayloadArraySize(MSG_SIZE);
        cont = idNode;
        for(int i = 0; i<MSG_SIZE;i++){
            macPacket->setPayload(i,cont);
            cont++;
        }
        macPacket->setCoeficienteArraySize(numhosts);
        for(int i = 0; i<numhosts;i++){
            macPacket->setCoeficiente(i,0);
        }
    }
    /*
     * Aqui verificar e já deve pausar (variavel bool sinalizada no evento de pause),
     * se já for, preencher uma variável e enviar junto da mensagem.
     *
     */
    if(redeDinamica){
        if(eventoPauseATivado){
            //cout<< "Nodo" << SELF_MAC_ADDRESS << "Vai pausar \n";
            macPacket->setPauseNodo(true);
            pausaEnviada = true;
        }else{
            macPacket->setPauseNodo(false);
        }

    }
    encapsulatePacket(macPacket, pkt);

    macPacket->setSrcID(SELF_MAC_ADDRESS); //if connected to PAN, would have a short MAC address assigned,
                                           //but we are not using short addresses in this model
    macPacket->setDstID(dstMacAddress);
    macPacket->setMac802154PacketType(MAC_802154_DATA_PACKET);
    macPacket->setSeqNum(seqNum++);

    /*
     * Se o nodo que estiver enviando um pacote for cooperante, este precisa guardar o
     * seu pacote na matriz de escutados, para depois codificar e retransmitir junto com os dos vizinhos
     * */

    if(useNetworkCoding){
        if((cooperador) || (cooperanteAuxiliar) || (cooperanteAuxiliarAuxiliar)){
            Basic802154Packet* framedup = macPacket->dup();
            MessagesNeighborhood *neig = new MessagesNeighborhood();
            neig->setFrameTransmission(framedup);
            neigmapNodosEscutados[SELF_MAC_ADDRESS] = neig;
            neigmapNodosEscutadosRefinamento[SELF_MAC_ADDRESS] = neig;

        }
    }

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
        declareOutput("Quantidade de Vezes que Cooperou");
        collectOutput("Quantidade de Vezes que Cooperou", "", qntidadeVezesCooperou);

        declareOutput("Total de Beacons Perdidos");
        collectOutput("Total de Beacons Perdidos", "", beaconsPerdidos);

        declareOutput("Mensagens Perdidas");
        collectOutput("Mensagens Perdidas", "", mensagensPerdidas);

        //declareOutput("Msg's enviadas");
        //collectOutput("Msg's enviadas", "", msgEnviadas);

        if(useNetworkCoding){
               declareOutput("Numero de Retransmissões Codificadas por Relay");
               collectOutput("Numero de Retransmissões Codificadas por Relay", "", relayNetworkCoding);

               /*Parametro só para teste*/
               declareOutput("Numero de Mensagens a ser codificadas unicamente por um relay");
               collectOutput("Numero de Mensagens a ser codificadas unicamente por um relay", "", var_msg_perNode);
               /*Parametro só para teste*/
               declareOutput("Numero de Mensagens a ser codificadas por outros relays");
               collectOutput("Numero de Mensagens a ser codificadas por outros relays", "", var_msg_otherNodes);

        }


    } else {
        double fimSim = 0;
        if(userelay){
            int num = 1;
            declareOutput("Pacotes Escutados Transmissao");

            for(int i = 1; i < (int)pacotesEscutadosT.size(); i++){
                collectOutput("Pacotes Escutados Transmissao", num, "Pacotes", pacotesEscutadosT[i]);
                //trace()<<"Coord recebeu pacote do nodo: "<<i<< "- qnt pacotes: "<<pacotesEscutadosT[i];
                num++;

            }

            int num2 = 1;
            double media = 0;
            declareOutput("Tempo Duracao da Selecao de Coop");
            for(int i = 0; i < (int)times.size(); i++){
                collectOutput("Tempo Duracao da Selecao de Coop", num2, "Tempo", times[i]);
                media = media + times[i];
                //trace()<<"Coord recebeu pacote do nodo: "<<i<< "- qnt pacotes: "<<pacotesEscutadosT[i];
                num2++;

            }
            declareOutput("Media de Tempo Duracao da Selecao de Coop");
            collectOutput("Media de Tempo Duracao da Selecao de Coop", "", media/times.size());

            contabilizarMsgRetransmissores();//Suelen

            int cont = 1;
            declareOutput("Mensagens Individuais retransmitidas");

            for(int i = 1; i < (int)msgRecuperadas.size(); i++){
                collectOutput("Mensagens Individuais retransmitidas", cont, "Mensagens", msgRecuperadas[i]);
                trace()<<"nodo: "<<i<<"recuperou "<< msgRecuperadas[i];
                cont++;

            }
             declareOutput("Overhead de retransmissoes");
            collectOutput("Overhead de retransmissoes", "", retransmissoesRepetidas.size());

            declareOutput("Numero de Cooperantes Selecionados");
            collectOutput("Numero de Cooperantes Selecionados", "", numCoopSel);


        }

        declareOutput("Beacons sent");
        collectOutput("Beacons sent", "", sentBeacons);

        //Suelen
        declareOutput("Msg's Recebidas");
        collectOutput("Msg's Recebidas", "", msgRecebidas);

        // Mensagens que foram recebidas por retransmissão, mesmo que não tenha sido útil
        declareOutput("Msg's Recebidas Retransmissao");
        collectOutput("Msg's Recebidas Retransmissao", "", msgRtrans);

        // numero de retransmissões em que alguma mensagem foi recuperada
        declareOutput("Retransmissoes Uteis");
        collectOutput("Retransmissoes Uteis", "", retransmissoesEfetivas);

        // numero de retransmissões em que nenhuma mensagem foi recuperada
        declareOutput("Retransmissoes Nao Uteis");
        collectOutput("Retransmissoes Nao Uteis", "", retransmissoesNaoEfetivas);

        declareOutput("Msg's Recuperadas");
        collectOutput("Msg's Recuperadas", "", mensagensRecuperadas);

        declareOutput("Numero de Selecoes Realizadas");
        collectOutput("Numero de Selecoes Realizadas", "", numSelRealizadas);

        if(useNetworkCoding){
            declareOutput("Numero de Mensagens recebidas Codificadas");
            collectOutput("Numero de Mensagens recebidas Codificadas", "", retransCoded);

            /*Número de retransmissão codificadas que não foram decodificadas de imediato*/
            declareOutput("Numero de Mensagens recebidas Codificadas nao recuperadas");
            collectOutput("Numero de Mensagens recebidas Codificadas nao recuperadas", "", var_msg_notRecover);

            /* Número total de mensagens que foram recuperadas pela codificação */
            declareOutput("Numero de Mensagens extraidas da codificacao com sucesso");
            collectOutput("Numero de Mensagens extraidas da codificacao com sucesso", "", num_msg_decod_sucess);



            int cont = 1;
            declareOutput("Mensagens Individuais Recuperadas Codificacao");

            for(int i = 1; i < numhosts; i++){
                collectOutput("Mensagens Individuais Recuperadas Codificacao", cont, "Mensagens", recoverPerNode[i]);
                trace()<<"nodo: "<<i<<"recuperou "<< recoverPerNode[i];
                cont++;

            }


        }

        fimSim = SIMTIME_DBL(simTime() - inicioSim);

        declareOutput("Tempo simulacao");
        collectOutput("Tempo simulacao", "", fimSim);



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
//void Basic802154::selecionaNodosSmartNumVizinhos(
//        Basic802154Packet *beaconPacket) {
//
//    std::string fileName("prob" + std::to_string(numeroDoProblema) + ".mod");
//    char *cstr = new char[fileName.length() + 1];
//    strcpy(cstr, fileName.c_str());
//
//    std::ofstream out(cstr);
//
//    std::map<int, Neighborhood*>::iterator iterNeighborhood;
//
//    out << "min:";
//    bool primeiro = true;
//    if (neigmap.size() > 0) {
//        for (iterNeighborhood = neigmap.begin();
//                iterNeighborhood != neigmap.end(); iterNeighborhood++) {
//            Neighborhood *nodo = iterNeighborhood->second;
//            if (primeiro) {
//                primeiro = false;
//                out << beta3 * nodo->numeroDevizinhos << "*x" << nodo->nodeId;
//            } else {
//                out << "+" << beta3 * nodo->numeroDevizinhos << "* x"
//                        << nodo->nodeId;
//            }
//
//        }
//        out << ";\n";
//
//        for (iterNeighborhood = neigmap.begin();
//                iterNeighborhood != neigmap.end(); iterNeighborhood++) {
//            Neighborhood *nodo = iterNeighborhood->second;
//
//            if (nodo->numeroDevizinhos > 0) {
//                primeiro = true;
//                int i, nodosConectados = 0;
//                for (i = 0; i < nodo->numeroDevizinhos; i++) {
//
//                    if (neigmap.find(nodo->vizinhos[i]) == neigmap.end()) {
//                        adicionarNodoSolto(nodo->nodeId, nodo->vizinhos[i]);
//                    } else {
//                        nodosConectados++;
//                        if (primeiro) {
//                            out << "C" << nodo->nodeId << ":x" << nodo->nodeId
//                                    << "+ x" << nodo->vizinhos[i];
//                            primeiro = false;
//                        } else {
//                            out << "+x" << nodo->vizinhos[i];
//                        }
//                    }
//                }
//                if (!primeiro) {
//                    out << ">=1;\n";
//                }
//            }
//        }
//        out << "\n";
//        std::map<int, vector<int>*>::iterator iter;
//
//        for (iter = listaDeNodosSoltos.begin();
//                iter != listaDeNodosSoltos.end(); iter++) {
//
//            vector<int>::iterator v = iter->second->begin();
//
//            out << "C" << iter->first << ":";
//            primeiro = true;
//            while (iter->second->end() != v) {
//                if (primeiro) {
//                    out << "x" << *v;
//                    primeiro = false;
//                } else {
//                    out << "+x" << *v;
//                }
//                v++;
//            }
//            out << ">=1;\n";
//        }
//        out << "\n";
//
//        primeiro = true;
//        int i = 0;
//        for (iterNeighborhood = neigmap.begin();
//                iterNeighborhood != neigmap.end(); iterNeighborhood++) {
//            Neighborhood *nodo = iterNeighborhood->second;
//            out << "bin x" << nodo->nodeId << ";" << "\n";
//            i++;
//
//        }
//
//        out.close();
//
//        lprec *lp;
//
//        char *lpName = "prob";
//
//        lp = read_LP(cstr, 2, lpName);
//
//        if (lp == NULL) {
//            fprintf(stderr, "Unable to read model\n");
//        } else {
//
//            solve(lp);
//            print_solution(lp, 1);
//            REAL resultado_lp[i];
//            get_variables(lp, resultado_lp);
//            int j = 0;
//            primeiro = true;
//            //limpando lista de colaboradores
//            nodosColaboradores.clear();
//            //cout<<"Selecao de Cooperantes\n";
//            for (iterNeighborhood = neigmap.begin();
//                    iterNeighborhood != neigmap.end(); iterNeighborhood++) {
//                Neighborhood *nodo = iterNeighborhood->second;
//
//                if (resultado_lp[j] == 1) {
//                    nodosColaboradores.push_back(nodo->nodeId);
//                    //cout<<"- : "<<nodo->nodeId <<"\n";
//                }
//                j++;
//
//            }
//        }
//        numeroDoProblema++;
//    }
//}


//Pegar hora
const std::string currentDateTime() {
   char            fmt[64], buf[64];
   struct timeval  tv;
   struct tm       *tm;

   gettimeofday(&tv, NULL);
   tm = localtime(&tv.tv_sec);
   //strftime(fmt, sizeof fmt, "%Y-%m-%d--%H:%M:%S.%%06u", tm);
   strftime(fmt, sizeof fmt, "%H:%M:%S.%%06u", tm);
   snprintf(buf, sizeof buf, fmt, tv.tv_usec);
   return buf;
}
void Basic802154::tratarDivisao(Neighborhood *nodo){
    double energy, somaRssi, rssi, txSucesso;
    int numeroDevizinhos;
    if(nodo->energy == 0){
        cout<<"entrei na restricao\n";
        nodo->energy = 0.1;
    }
    if(nodo->rssi == 0){
        nodo->rssi= 0.1;
    }
    if(nodo->somaRssi == 0){
        nodo->somaRssi= 0.1;
    }
    if(nodo->txSucesso == 0){
        nodo->txSucesso= 0.1;
    }


}
void Basic802154:: selectCoopAux(){
    /* Preciso saber quem são os cooperantes. (nodosColaboradores)*
     * Para cada cooperante preciso saber quem são os seus vizinhos;
     * Para cada vizinho do cooperante que comunique com o coordenador, verifico a lista de vizinhos deste nodo
     * procurando se ele escutou algum nodo que o coordenador perdeu a mensagem;
     * Salvo os dois vizinhos do Coop que mais tenha escutado mensagens perdidas.
     *
     */
    bool exist = false;
    bool existCoop = false;
    std::map<int, Neighborhood*>::iterator iterNeighborhood;
    std::map<int, Neighborhood*>::iterator iter;
    Neighborhood *nodo;
    Neighborhood *nodovizinho;
    int numvizinhos = 0;
    std::vector<int> GACK_msgPerdidas;
    int numMsgCanRecover = 0;
    int nodeCanRecover = 0;
    int auxNumMsgCanRecover = 0;
    int auxNodeCanRecover = 0;

    int auxNumMsgEmComum = 0;
    int numMsgEmComum = 0;
    int auxNodeMsgEmComum = 0;
    int nodeMsgEmComum = 0;

    int numMsgCoop = 0;
    int coopAux1 = 0;
    int coopAux2 = 0;



    std::vector<int> msgCanRecover;
    std::vector<int> auxMsgCanRecover;
    std::vector<int> auxMsgEmComum;
    std::vector<int> msgEmComum;
    std::vector<int> coopAux;

    GACK_msgPerdidas.clear();
    msgCanRecover.clear();
    auxMsgCanRecover.clear();
    auxMsgEmComum.clear();
    msgEmComum.clear();
    coopAux.clear();
    nodosColaboradoresAuxiliares.clear();
    //Armazenos as mensagens que o coord perdeu no vetor GACK_msgPerdidas
    for(int i = 1; i < (int) GACK.size();i++){
        if(GACK[i] == 0){
            GACK_msgPerdidas.push_back(i);
        }
    }

    for(int i = 0;i < (int)nodosColaboradores.size();i++){
        cout<<"Nodo Colaborador: "<< nodosColaboradores[i]<<endl;
        iterNeighborhood = neigmap.find(nodosColaboradores[i]);
        if(iterNeighborhood != neigmap.end()){
            nodo = iterNeighborhood->second;
            numvizinhos = nodo->vizinhos.size();
            if((numvizinhos > 0) && (GACK_msgPerdidas.size()>0)){
                for (int j = 0; j < numvizinhos; j++) {
                    exist = false;
                    existCoop = false;
                    existCoop = std::find(nodosColaboradores.begin(), nodosColaboradores.end(), nodo->vizinhos[j]) != nodosColaboradores.end();
                    exist = std::find(nodosColaboradoresAuxiliares.begin(), nodosColaboradoresAuxiliares.end(), nodo->vizinhos[j]) != nodosColaboradoresAuxiliares.end();
                    if((!exist) && (!existCoop)){
                        auxNumMsgCanRecover = 0;
                        auxMsgCanRecover.clear();
                        auxNodeCanRecover = 0;
                        cout<<"vizinho colaborador: "<< nodo->vizinhos[j]<<endl;
                        // verifica se cada vizinho comunica com o coordenador
                        iter = neigmap.find(nodo->vizinhos[j]);
                        if(iter!=neigmap.end()){
                            nodovizinho = iter->second;
                            for(int m = 0; m < (int)nodovizinho->vizinhos.size();m++){
                                cout<<"vizinho do vizinho do coop: "<< nodovizinho->vizinhos[m]<<endl;
                                for(int n = 0; n < numvizinhos; n++){
                                    cout<<"vizinho do coop: "<< nodo->vizinhos[n]<<endl;
                                    for(int k = 0; k < (int)GACK_msgPerdidas.size();k++){
                                        cout<<"msg perdida: "<< GACK_msgPerdidas[k]<<endl;
                                        /*Aqui salvo as msgs que o cooperante tem em comum com os vizinhos*/
                                        if((nodovizinho->vizinhos[m] == nodo->vizinhos[n]) && (nodovizinho->vizinhos[m] == GACK_msgPerdidas[k])){
                                            cout<<"O vizinho do coop e o coop escutaram a mesma msg perdida"<<endl;
                                            auxNumMsgCanRecover++;
                                            auxMsgCanRecover.push_back(GACK_msgPerdidas[k]);
                                            auxNodeCanRecover = nodo->vizinhos[j];
                                            break;

                                        }
                                    }
                                }
                            }
                            if(auxNumMsgCanRecover > numMsgEmComum){
                            //Aqui faço a intersecção das msgs em comum com um segundo auxiliar
                                for (int p = j +1; p < numvizinhos; p++) {
                                        auxNumMsgEmComum = 0;
                                        auxMsgEmComum.clear();
                                        auxNodeMsgEmComum = 0;
                                        exist = false;
                                        existCoop = false;
                                        existCoop = std::find(nodosColaboradores.begin(), nodosColaboradores.end(), nodo->vizinhos[p]) != nodosColaboradores.end();
                                        exist = std::find(nodosColaboradoresAuxiliares.begin(), nodosColaboradoresAuxiliares.end(), nodo->vizinhos[p]) != nodosColaboradoresAuxiliares.end();
                                        if((!exist) && (!existCoop)){
                                            iter = neigmap.find(nodo->vizinhos[p]);
                                            if(iter!=neigmap.end()){
                                                nodovizinho = iter->second;
                                                for(int o = 0;o < auxNumMsgCanRecover;o++){
                                                    for(int m = 0; m < (int)nodovizinho->vizinhos.size();m++){
                                                        if(auxMsgCanRecover[o] == nodovizinho->vizinhos[m]){
                                                            auxNumMsgEmComum++;
                                                            auxMsgEmComum.push_back(auxMsgCanRecover[o]);
                                                            auxNodeMsgEmComum = nodo->vizinhos[p];
                                                            break;
                                                        }
                                                    }
                                                }
                                                if(auxNumMsgEmComum > numMsgEmComum){// no fim o "nodeMsgEmComum" resulta no nodo que tem mais em comum como primeiro auxiliar e o cooperante
                                                    numMsgEmComum = auxNumMsgEmComum;
                                                    msgEmComum = auxMsgEmComum;
                                                    nodeMsgEmComum = auxNodeMsgEmComum;


                                                    auxNumMsgEmComum = 0;
                                                    auxMsgEmComum.clear();
                                                    auxNodeMsgEmComum = 0;
                                                }
                                          }
                                    }
                                }
                            }

                    }
                        if(numMsgEmComum > numMsgCoop){
                            numMsgCoop = numMsgEmComum;
                            coopAux1 = auxNodeCanRecover;
                            coopAux2 = nodeMsgEmComum;
                        }
                }
            }

        }
    }
        //coopAux.push_back(coopAux1);
        //coopAux.push_back(coopAux2);
        nodosColaboradoresAuxiliares.push_back(coopAux1);
        nodosColaboradoresAuxiliares.push_back(coopAux2);
        numMsgEmComum = 0;
        numMsgCoop = 0;



}
    for(int i=0;i<nodosColaboradoresAuxiliares.size();i++){
        cout<<"Auxiliares: "<<nodosColaboradoresAuxiliares[i]<<endl;
    }
}








// método correto
//método que monta e resolve o probelma de otimização e escreve um arquivo .mod
void Basic802154::selecionaNodosSmart(Basic802154Packet *beaconPacket) {
    if (neigmap.size() > 0) {
        double result;
        double energyRemaining;
        int alteradoVizinho = 0;
        /*struct timeval  tv = { 0 };
        gettimeofday(&tv, NULL);
        double mill = (tv.tv_usec) / 1000 ; // Para pegar milisegundos*/
        string tempo = currentDateTime();

        std::string fileName("prob" + std::to_string(numeroDoProblema) + "-" +tempo + ".mod");
        //trace()<<"prob"<<std::to_string(numeroDoProblema)<<"-"<<tempo <<".mod";
        //std::string fileName("prob" + std::to_string(numeroDoProblema) + "- TIME" + __TIME__+":"+std::to_string(mill) + ".mod");
        //std::string fileName("prob" + std::to_string(numeroDoProblema) + ".mod");
        char *cstr = new char[fileName.length() + 1];
        strcpy(cstr, fileName.c_str());
        std::ofstream out(cstr);

        std::map<int, Neighborhood*>::iterator iterNeighborhood;

        out << "min:";
        bool primeiro = true;
    //if (neigmap.size() > 0) {
        for (iterNeighborhood = neigmap.begin();
                iterNeighborhood != neigmap.end(); iterNeighborhood++) {
            Neighborhood *nodo = iterNeighborhood->second;

            //nodo->energy=0;
            if(associatedDevices[nodo->nodeId] == true){
                cout<<"Nodo "<<  nodo->nodeId<<"\n";
                result = 0;
                tratarDivisao(nodo);
                if (primeiro) {
                    primeiro = false;
                    if(nodo->numeroDevizinhos == 0){
                          alteradoVizinho = 1;
                          nodo->numeroDevizinhos= 1;
                    }
                    //trace() << "Energia: " << nodo->energy << " RSSI: "
                      //      << nodo->somaRssi << " Vizinhos: "
                        //    << nodo->numeroDevizinhos << "Taxa de Sucesso: "
                          //  << nodo->txSucesso << " ID: " << nodo->nodeId;
                    //result = ((beta1 / nodo->energy) + (beta2 / nodo->somaRssi)+ (beta3 / nodo->numeroDevizinhos)+ (beta4 / nodo->txSucesso));
                    result = (beta1 / nodo->energy);
                    //trace()<<"Result: "<<result << "*x"<< nodo->nodeId;

                    /*out
                            << ((beta1 / nodo->energy) + (beta2 / nodo->somaRssi)
                                    + (beta3 / nodo->numeroDevizinhos)
                                    + (beta4 / nodo->txSucesso)) << "*x"
                            << nodo->nodeId;*/

                    out << (beta1 / nodo->energy)<< "*x" << nodo->nodeId;

                    if(alteradoVizinho == 1){
                        nodo->numeroDevizinhos = 0;
                        alteradoVizinho = 0;
                    }

                } else {
                    if(nodo->numeroDevizinhos == 0){
                      alteradoVizinho = 1;
                      nodo->numeroDevizinhos= 1;
                    }
                    //trace() << "Energia: " << nodo->energy << " RSSI: "
                      //      << nodo->somaRssi << " Vizinhos: "
                        //    << nodo->numeroDevizinhos << "Taxa de Sucesso: "
                          //  << nodo->txSucesso << " ID: " << nodo->nodeId;
                    //result = ((beta1 / nodo->energy) + (beta2 / nodo->somaRssi)+ (beta3 / nodo->numeroDevizinhos)+ (beta4 / nodo->txSucesso));
                    result = (beta1 / nodo->energy);
                    //trace()<<"Result: "<<result << "*x"<< nodo->nodeId;

                    /*out << "+"
                            << ((beta1 / nodo->energy) + (beta2 / nodo->somaRssi)
                                    + (beta3 / nodo->numeroDevizinhos)
                                    + (beta4 / nodo->txSucesso)) << "* x"
                            << nodo->nodeId;*/

                    out << "+" << (beta1 / nodo->energy) << "* x" << nodo->nodeId;

                    if(alteradoVizinho == 1){
                        nodo->numeroDevizinhos = 0;
                        alteradoVizinho = 0;
                    }
                }
              }

            }
            out << ";\n";
            // restricoes dos nodos que se veem
            for (iterNeighborhood = neigmap.begin();
                    iterNeighborhood != neigmap.end(); iterNeighborhood++) {
                Neighborhood *nodo = iterNeighborhood->second;


              if(associatedDevices[nodo->nodeId] == true){
                    if (nodo->numeroDevizinhos > 0) {
                        primeiro = true;
                        int i, nodosConectados = 0;
                        for (i = 0; i < nodo->numeroDevizinhos; i++) {

                            if (neigmap.find(nodo->vizinhos[i]) == neigmap.end()) {
                                adicionarNodoSolto(nodo->nodeId, nodo->vizinhos[i]);
                                //TESTE
                                /*if (primeiro) {
                                    out << "ConectCoordX" << nodo->nodeId << ":x" << nodo->nodeId
                                            << "+ x" << nodo->vizinhos[i];
                                    primeiro = false;
                                }else {
                                    out << "+x" << nodo->vizinhos[i];
                                }*/
                            } else {
                                nodosConectados++;
                                if (primeiro) {
                                    out << "ConectCoordX" << nodo->nodeId << ":x" << nodo->nodeId
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
            }
            out << "\n";
            std::map<int, vector<int>*>::iterator iter;
            // nodos que só são enchergados por um nodo
            for (iter = listaDeNodosSoltos.begin();
                    iter != listaDeNodosSoltos.end(); iter++) {

                vector<int>::iterator v = iter->second->begin();

                out << "NotConetCoordX" << iter->first << ":";
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
            // mostra quem sao as variaveis binarias
            for (iterNeighborhood = neigmap.begin();
                    iterNeighborhood != neigmap.end(); iterNeighborhood++) {
                Neighborhood *nodo = iterNeighborhood->second;

                out << "bin x" << nodo->nodeId << ";" << "\n";
                i++;
            }

            out.close();
            listaDeNodosSoltos.clear();// Limpa a lista de nodos que o coordenador não escuta

            lprec *lp;

            char *lpName = "prob";

            lp = read_LP(cstr, 2, lpName);

            if (lp == NULL) {
                fprintf(stderr, "Unable to read model\n");
            } else {

                solve(lp);
                print_solution(lp, 1);

                cout << "-----------------------------------neigmap.size " << neigmap.size() << "\n";
                REAL resultado_lp[i];
                cout << "----------------------REAL resultado_lp cria" << "\n";
                get_variables(lp, resultado_lp);
                cout << "----------get_variables" << "\n";
                int j = 0;
                primeiro = true;

                cout<<"neigmap.size(): "<< neigmap.size()<<"\n";
                cout<<"i: "<< i<<"\n";
               /* for(int u=0; u < i;u++){
                    cout<<"resultado_lp["<<u<<"]: "<< resultado_lp[u]<<"\n";
                }*/
                cout << "---------- inicializando as variáveis \n";

                cout << nodosColaboradores.empty();
                nodosColaboradores.clear();
                cout << "---------- limpando os nodosColaboradores \n";
                for (iterNeighborhood = neigmap.begin();
                        iterNeighborhood != neigmap.end(); iterNeighborhood++) {
                    cout << "----------começando o for " << j << "\n";
                    Neighborhood *nodo = iterNeighborhood->second;
                    cout << "----------Nodo " << nodo->nodeId << "\n";
                    if (resultado_lp[j] == 1) {
                        nodosColaboradores.push_back(nodo->nodeId);
                        cout << "----------Colaborador" << nodo->nodeId << "\n";
                    }
                    j++;

                }
                numeroDoProblema++;

               /* for(int j=0;j< (int) nodosColaboradores.size();j++){
                    cout<<"Colaborador: "<< nodosColaboradores[j] <<"\n";
                    for(int l = 1; l< numhosts;l++){
                        if(nodosColaboradores[j] == l){
                            qntidadeVezesCooperou[l] = qntidadeVezesCooperou[l] + selecao + 1;
                            cout<<"Quantidade que vezes nodo"<<l<< "Colaborou: "<< qntidadeVezesCooperou[l]  <<"\n";
                           break;
                         }
                     }
                }*/


            }
       //}
    }
}

void Basic802154:: algGenetic(){
    srand(time(NULL));
    //cout<< "tempExecInicio "<< tempExecInicio <<endl;
    std::map<int, Neighborhood*> neigmapAux;
    static AlgoritmoGenetico *ag;
    //Neighborhood* neigh;
    int numLinhas = 0;
    int numColunas = 0;
    //int TAM_POPULACAO = 500;
    int TAM_POPULACAO = 1000;
    double TAXA_MIN_MUTACAO = 0.1;
    //int NUM_GERACOES = 500;
    int NUM_GERACOES = 1000;
    double beta1 = 0.5;
    neigmapAux.clear();


    std::map<int,Neighborhood*>::iterator iterNeighborhood;
    for(int i = 1; i < numhosts; i++){
        iterNeighborhood = neigmap.find(i);
        if(iterNeighborhood != neigmap.end()){
            neigmapAux[i] = iterNeighborhood->second;
        }else{
            Neighborhood* neigh = new Neighborhood();
            neigh->nodeId = i;
            neigh->energy = 0;
            neigh->somaRssi = 0;
            neigh-> rssi = 0;
            neigh->vizinhos.clear();
            neigh->numeroDevizinhos = neigh->vizinhos.size();
            neigmapAux[i] = neigh;
        }
    }

    numColunas = neigmapAux.size();
    numLinhas = neigmapAux.size();
    ag = new AlgoritmoGenetico(numLinhas, numColunas, TAM_POPULACAO, TAXA_MIN_MUTACAO, NUM_GERACOES);

    std::map<int,Neighborhood*>::iterator it;
    Neighborhood *nodosEscutados; //= new Neighborhood();
    double custo = 0;

    for(int i = 0; i < numColunas; i++){
        // cout << "debug it:" << it->second.nodeId << endl
        it = neigmapAux.find(i+1);
        if(it != neigmapAux.end()){
            nodosEscutados = it->second;

            if(nodosEscutados->energy > 0){
                custo = (beta1/nodosEscutados->energy);
                ag->addCusto(i, custo);
                for(int j = 0; j < (int) nodosEscutados->vizinhos.size(); j++){
                    ag->addDados(i, nodosEscutados->vizinhos[j]);
                }
            }else{
                custo = 100.5;
                ag->addCusto(i, custo);
                ag->addDados(i, 0);
            }
        }
    }

    ag->executar();
    nodosColaboradores.clear();
    for(int k=0; k < (int) ag->nodoscooperantes.size();k++){
        nodosColaboradores.push_back(ag->nodoscooperantes[k]);
    }

}

//Monta a matri de adjacencia a partir do neighmap Para a Heuristica Gulosa
void Basic802154::vizinhanca(){
    std::map<int, Neighborhood*>::iterator iterNeighborhood;
    int aux;
    //int matrizAdj[numhosts][numhosts];
    //int matrizAdj[numhosts][numhosts];
    int idNodo = 0;
    int aux2 = 0;
    int posi = 0;
    finiteSet.clear();

    limparMatrizAdjacencia();

    for (iterNeighborhood = neigmap.begin();
            iterNeighborhood != neigmap.end(); iterNeighborhood++) {
        cout << "----------começando o for vizinhos  NEIGMAP NODO: "<< iterNeighborhood->first << "\n";
        Neighborhood *nodo = iterNeighborhood->second;
        cout << " Nodo: " << nodo->nodeId << "\n";

        //atribuição que escuta o coordenador e se escuta;
        matrizAdj[nodo->nodeId][0] = 1;
        matrizAdj[nodo->nodeId][nodo->nodeId] = 1;
        matrizAdj[0][nodo->nodeId] = 1; // se esta no neigmap o coord escuta;

        //Matriz de adjacencia sendo preenchida em map
        vizinhosAdj[nodo->nodeId] = nodo->vizinhos;

        // ordena o vetor de vizinhos de cada nodo
        for (int i = 0; i < nodo->vizinhos.size(); i++){
                for (int j = 0; j < nodo->vizinhos.size(); j++){
                    if (nodo->vizinhos[i] < nodo->vizinhos[j]){
                        /*aqui acontece a troca, do maior cara
                        vaia para a direita e o menor para a esquerda*/
                        aux = nodo->vizinhos[i];
                        nodo->vizinhos[i] = nodo->vizinhos[j];
                        nodo->vizinhos[j] = aux;
                    }
                }
            }
        //vai preenchendo a matriz de adjacencia
        for(int k = 0; k < nodo->vizinhos.size(); k++){
            aux2 = 0;
                for(int v = 1; v < numhosts; v++){
                    posi = v;
                    if(nodo->vizinhos[k] == v){
                       aux2 = 1;
                       printf("entrei\n");
                       matrizAdj[nodo->nodeId][posi] = 1;
                       printf(" matrizAdj[%d][%d]=%d\t", nodo->nodeId,posi, matrizAdj[nodo->nodeId][posi]);
                       break;
                    }
                }

            }

        //Print de verificação
        //for(int i = 0; i < nodo->vizinhos.size(); i++){
        //    cout << "vizinhos[" << i << "]:" << nodo->vizinhos[i]<< "\n" ;
        //}

    }
    // Print Matriz adjacencia 0 e 1's
    //for (int i = 0; i < numhosts; i++){
        //for (int j = 0; j < numhosts; j++){
           //printf(" matrizAdj[%d][%d]=%d\t", i,j, matrizAdj[i][j]);
        //}
        //printf("\n");
    //}

    // verifica os nodos que estão associados e monta o conjunto do Universo (finiteSet)
    for(int i = 0; i < numhosts; i++){
          printf(" associatedDevices[%d]: %d\n", i, associatedDevices[i]);

          if(associatedDevices[i] == true){
              finiteSet.push_back(i);
          }


    }
    //Print do conjunto de nodos que precisam ser cobertos, ou seja, todos os nodos associados na rede
    for(int i = 0; i < finiteSet.size(); i++){
       printf(" finiteSet[%d]: %d\n", i, finiteSet[i]);
    }

    //Chamada da função de heuristica gulosa.
    heuristicGreedy();

}

void Basic802154:: heuristicGreedy(){
    std::map<int, vector<int>>::iterator iterSubConjuntosF;
    std::vector<int> auxiliarVizinhos; // nodos cobertos
    std::vector<int> contador; // nodos cobertos
    solutionSet.clear();
    auxiliarSet.clear();
    contador.clear();
    auxiliarVizinhos.clear();
    int max = 0;
    int posicao = 0;

    //Faz uma cópia do conjunto original para um auxiliar
    for(int i = 0; i < finiteSet.size(); i++){
        auxiliarSet.push_back(finiteSet[i]);

    }
    //Limpa o cont
    //for(int i = 0; i < numhosts; i++){
        //contador.push_back(0);

    //}

    // enquanto estiver nodos não cobertos continue
    while(!auxiliarSet.empty()){
            auxiliarVizinhos.clear();
            contador.clear();
            for(int i = 0; i < numhosts; i++){
                    contador.push_back(0);

             }

            // verifica qual nodos possui o maior numero de vizinhos ainda não coberto
            for (iterSubConjuntosF = vizinhosAdj.begin();
                    iterSubConjuntosF != vizinhosAdj.end(); iterSubConjuntosF++) {
                    cout << "----------começando o for vizinhosAdj nodo "<< iterSubConjuntosF->first <<"\n";
                    std::vector<int> sub = iterSubConjuntosF->second;

                    for(int i=0; i < sub.size(); i++){
                        for (int k=0; k < auxiliarSet.size(); k++){
                            if (sub[i] == auxiliarSet[k]){ // verifica se o elemento auxiliarSet[k] e vizinho do nodo em analise (linha)
                                  //cont[iterSubConjuntosF->first]++; // conta o numero de nodos cobertos
                                  contador[iterSubConjuntosF->first]++;
                                  break;
                            }
                        }

                    }
            }
            // verifica a posição (id) do nodo com mais vizinhos ainda não cobertos em auxiliarSet, finaliza e deu.
            max = 0;
            posicao = -1;
            for(int m = 0; m < numhosts; m++){
                if(contador[m]>max){
                   max=contador[m];
                   posicao = m;
               }
            }

            // se não há elementos que cubram todos os nodos obrigo a sair do while
            if(max == 0){
                auxiliarSet.clear();
            }

            if(!auxiliarSet.empty()){
                    // Copia em um auxiliar os nodos cobertos pelo coop selecionado
                    for (iterSubConjuntosF = vizinhosAdj.begin();
                                iterSubConjuntosF != vizinhosAdj.end(); iterSubConjuntosF++) {
                                cout << "----------começando o for vizinhos do Nodo: "<< iterSubConjuntosF->first << "\n";
                                std::vector<int> sub = iterSubConjuntosF->second;

                                if(iterSubConjuntosF->first == posicao){
                                    auxiliarVizinhos = sub;
                                    solutionSet.push_back(iterSubConjuntosF->first); // adiciono o id do cooperante na solucao
                                    break;
                                 }
                     }

                    // apaga os elementos que foram cobertos pelo cooperante selecionado
                    for(int i = 0; i < auxiliarSet.size(); i++){
                        for(int k = 0; k < auxiliarVizinhos.size(); k++){
                            if(auxiliarSet[i] == auxiliarVizinhos[k]){
                                auxiliarSet.erase(auxiliarSet.begin() + i);
                            }
                         }

                    }
            }
    }

    nodosColaboradores.clear();
    for(int i = 0; i< solutionSet.size();i++){
        nodosColaboradores.push_back(solutionSet[i]);
        cout << "----------Colaborador" << nodosColaboradores[i] << "\n";
    }

}

// Apaga os elementos da matriz
void Basic802154::limparMatrizAdjacencia(){

    for (int i = 0; i < numhosts; i++){
          for (int j = 0; j < numhosts; j++){
              matrizAdj[i][j]=0;
          }
      }

}
void Basic802154::matrizVizinhancaNodos(Basic802154Packet * pkt){
    unsigned vizinhos = pkt->getVizinhosOuNodosCooperantesArraySize();
    int i;
    std::vector<int> vizinhanca;
    if(vizinhos > 0){
        vizinhanca.clear();
        for (i = 0; i < (int)vizinhos; i++) {
            vizinhanca.push_back(pkt->getVizinhosOuNodosCooperantes(i));
        }
        matrizVizinhos[pkt->getSrcID()] = vizinhanca;
    }

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
    cout << "Eu sou o: " << SELF_MAC_ADDRESS << "\n";
    unsigned vizinhos = pkt->getVizinhosOuNodosCooperantesArraySize();
    if (pkt->getDadosVizinhoArraySize() == 0) { //verifico se é retransmissao
        //if (rssi > limiteRSSI && !oportunista) {
        if (rssi > limiteRSSI) {
            if (iterNeighborhood == neigmap.end()) {
                nodo = new Neighborhood();
                nodo->nodeId = pkt->getSrcID();
                nodo->rssi = (rssi / MAX_RSSI);
//                trace()<<"NAN rssi: "<<rssi;
//                if(isnan(nodo->rssi)){
//                    trace()<<"DEU NAN rssi: "<<rssi;
//                }
//                trace()<<"SOMASINAIS ANTES.  "<<somaDeSinais;
                somaDeSinais = somaDeSinais + nodo->rssi;
//                trace()<<"SOMASINAIS DEPOIS. > "<<somaDeSinais;
                nodo->energy = pkt->getEnergy();
                nodo->numeroDevizinhos = vizinhos;
                nodo->somaRssi = pkt->getSomaSinais(); //atulizar soma de rssi
//                trace()<<"SomaRSSI> "<<nodo->somaRssi;
//                if(isnan(nodo->somaRssi)){
//                  trace()<<"DEU NAN somaRSSI: "<<nodo->somaRssi;
//                }
                nodo->txSucesso = taxaDeSucesso(pkt->getSrcID(), 1);

                if (isPANCoordinator && vizinhos > 0) {
                    //atualizar lista de vinhos
                    int i;
                    for (i = 0; i < vizinhos; i++) {
                        nodo->vizinhos.push_back(
                                pkt->getVizinhosOuNodosCooperantes(i));
                    }
                }
                neigmap[pkt->getSrcID()] = nodo;
            } else {            //se o nodo já está na lista atualiza os dados
                nodo = iterNeighborhood->second;
                somaDeSinais = somaDeSinais - nodo->rssi;
                nodo->rssi = (rssi / MAX_RSSI);
//                trace()<<"NAN rssi: "<<rssi;
//                if(isnan(nodo->rssi)){
//                   trace()<<"DEU NAN rssi: "<<rssi;
//                }
//                trace()<<"SOMASINAIS ANTES. "<<somaDeSinais;

                somaDeSinais = somaDeSinais + nodo->rssi;

//                trace()<<"SOMASINAIS DEPOIS. "<<somaDeSinais;
                nodo->energy = pkt->getEnergy();
                nodo->numeroDevizinhos = vizinhos;
                nodo->somaRssi = pkt->getSomaSinais();
//                trace()<<"SomaRSSI> "<<nodo->somaRssi;
//                if(isnan(nodo->somaRssi)){
//                    trace()<<"DEU NAN somaRSSI: "<<nodo->somaRssi;
//                }
                nodo->txSucesso = taxaDeSucesso(pkt->getSrcID(), 1);
                if (isPANCoordinator && vizinhos > 0) {
                    //atualizar lista de vinhos
                    nodo->vizinhos.clear();
                    int i;
                    for (i = 0; i < vizinhos; i++) {
                        nodo->vizinhos.push_back(
                                pkt->getVizinhosOuNodosCooperantes(i));
                    }
                }

            }
            if(!isPANCoordinator){
                matrizVizinhancaNodos(pkt);
            }
        }/*else{// se for a tecnica oportunista não preciso verificar a qualidade da comunicação
            if(oportunista){
            if (iterNeighborhood == neigmap.end()) {
                            nodo = new Neighborhood();
                            nodo->nodeId = pkt->getSrcID();
                            nodo->rssi = (rssi / MAX_RSSI);
            //                trace()<<"NAN rssi: "<<rssi;
            //                if(isnan(nodo->rssi)){
            //                    trace()<<"DEU NAN rssi: "<<rssi;
            //                }
            //                trace()<<"SOMASINAIS ANTES.  "<<somaDeSinais;
                            somaDeSinais = somaDeSinais + nodo->rssi;
            //                trace()<<"SOMASINAIS DEPOIS. > "<<somaDeSinais;
                            nodo->energy = pkt->getEnergy();
                            nodo->numeroDevizinhos = vizinhos;
                            nodo->somaRssi = pkt->getSomaSinais(); //atulizar soma de rssi
            //                trace()<<"SomaRSSI> "<<nodo->somaRssi;
            //                if(isnan(nodo->somaRssi)){
            //                  trace()<<"DEU NAN somaRSSI: "<<nodo->somaRssi;
            //                }
                            nodo->txSucesso = taxaDeSucesso(pkt->getSrcID(), 1);

                            if (isPANCoordinator && vizinhos > 0) {
                                //atualizar lista de vinhos
                                int i;
                                for (i = 0; i < vizinhos; i++) {
                                    nodo->vizinhos.push_back(
                                            pkt->getVizinhosOuNodosCooperantes(i));
                                }
                            }
                            neigmap[pkt->getSrcID()] = nodo;
                        } else {            //se o nodo já está na lista atualiza os dados
                            nodo = iterNeighborhood->second;
                            somaDeSinais = somaDeSinais - nodo->rssi;
                            nodo->rssi = (rssi / MAX_RSSI);
            //                trace()<<"NAN rssi: "<<rssi;
            //                if(isnan(nodo->rssi)){
            //                   trace()<<"DEU NAN rssi: "<<rssi;
            //                }
            //                trace()<<"SOMASINAIS ANTES. "<<somaDeSinais;

                            somaDeSinais = somaDeSinais + nodo->rssi;

            //                trace()<<"SOMASINAIS DEPOIS. "<<somaDeSinais;
                            nodo->energy = pkt->getEnergy();
                            nodo->numeroDevizinhos = vizinhos;
                            nodo->somaRssi = pkt->getSomaSinais();
            //                trace()<<"SomaRSSI> "<<nodo->somaRssi;
            //                if(isnan(nodo->somaRssi)){
            //                    trace()<<"DEU NAN somaRSSI: "<<nodo->somaRssi;
            //                }
                            nodo->txSucesso = taxaDeSucesso(pkt->getSrcID(), 1);
                            if (isPANCoordinator && vizinhos > 0) {
                                //atualizar lista de vinhos
                                nodo->vizinhos.clear();
                                int i;
                                for (i = 0; i < vizinhos; i++) {
                                    nodo->vizinhos.push_back(
                                            pkt->getVizinhosOuNodosCooperantes(i));
                                }
                            }

                        }
        }
     }*/
    }
}
double Basic802154::taxaDeSucesso(int id, int recebidas) {
    double SR, SampleSR;
    /*SampleSR = (qntMsgEnviada/qntMsgRecebida)*/
    SampleSR = 1 / recebidas;
    for (int i = 1; i <= numhosts - 1; i++) {
        if (i == id) {
            historicoTaxaDeSucesso[i].id = id;
            historicoTaxaDeSucesso[i].taxaDeSucesso = (1 - alphaSucess)
                    * historicoTaxaDeSucesso[i].taxaDeSucesso
                    + (alphaSucess * SampleSR);
            SR = historicoTaxaDeSucesso[i].taxaDeSucesso;
            break;
        }
    }
    return SR;

}

// método Ríad
//
void Basic802154::souNodoCooperante(Basic802154Packet * pkt) {
    int i = 0;
    cooperador = false;
    coopAuxPerNode.clear();

    if(!useCoopAux){
        while (pkt->getVizinhosOuNodosCooperantesArraySize() > i) {

            if (pkt->getVizinhosOuNodosCooperantes(i) == self) {
                cooperador = true;
                break;
            }
            i++;
        }
    }
    if(useCoopAux){
        int numeroCooperante;
        numeroCooperante = (int)pkt->getVizinhosOuNodosCooperantesArraySize();
        int a = 0;
        int b = 2;
        int j;
        cout<< "numeroCooperante: "<< numeroCooperante << endl;
        if(numeroCooperante > 0){
            for(i = 0; i< numeroCooperante;i++){
                if (pkt->getVizinhosOuNodosCooperantes(i) == self) {
                    cooperador = true;
                    cout<< "A: "<< a << endl;
                    cout<< "B: "<< b << endl;
                    if((int)pkt->getCoopAuxiliaresArraySize() > 0){
                        for(j = a; j < b; j++){
                            coopAuxPerNode.push_back(pkt->getCoopAuxiliares(j));
                        }
                    }
                    break;

                }
                a = b;
                b = b + 2;
            }
        }
    }


}
void Basic802154::souNodoCooperanteAuxiliar(Basic802154Packet * pkt) {
    //unsigned int i = 0;
    cooperanteAuxiliar = false;
    cooperanteAuxiliarAuxiliar = false;
    coopHelped.clear();

    /*while (pkt->getCoopAuxiliaresArraySize() > i) {

        if (pkt->getCoopAuxiliares(i) == self) {
            cooperanteAuxiliar = true;
            break;
        }
        i++;
    }*/
    int numeroCooperante = (int)pkt->getVizinhosOuNodosCooperantesArraySize();
    int numeroCooperanteAuxiliar = (int)pkt->getCoopAuxiliaresArraySize();
    int a = 0;
    int b = 2;
    int j,i;
    int identifierAux = 0;
    cout<< "numeroCooperanteAuxiliar: "<< numeroCooperanteAuxiliar << endl;
    cout<< "numeroCooperante: "<< numeroCooperante << endl;
    if((numeroCooperanteAuxiliar > 0) && (numeroCooperante >0)){
        for(j = 0; j < numeroCooperante;j++){
            identifierAux = 0;
            cout<< "A: "<< a << endl;
            cout<< "B: "<< b << endl;
            for(i = a; i < b; i++){
                identifierAux++;
                if (pkt->getCoopAuxiliares(i) == self) {
                    if(identifierAux == 2){//significa que é o segundo auxiliar, é o cooperanteAuxiliarAuxiliar
                        cooperanteAuxiliarAuxiliar = true;
                    }else{
                        if(identifierAux == 1){ //É o primeiro auxiliar
                            cooperanteAuxiliar = true;
                        }
                    }
                    coopHelped.push_back(pkt->getVizinhosOuNodosCooperantes(j));
                    if(cooperanteAuxiliarAuxiliar){//Aqui guarda o outro auxiliar
                        coopHelped.push_back(pkt->getCoopAuxiliares(i-1));
                    }else{//Aqui guarda o outro auxiliar
                        coopHelped.push_back(pkt->getCoopAuxiliares(i+1));
                    }
                    break;
                }
            }
            a = b;
            b = b + 2;
        }
    }



}

/*
 * Esse método armazena as mensagens que o coodenador escutou e que chegaram por retransmissão
 *
 * */
void Basic802154::listarNodosEscutadosRetransmissaoNetworkCoding(Basic802154Packet *rcvPacket){
        int recuperadas = 0;
        Basic802154Packet* framedup = rcvPacket->dup();
        /*unsigned short*/ uint8_t coeficiente = 0;

        cout<<"Coordenador recebeu retrans do nodo: " <<framedup->getSrcID()<<endl;

        for(int i = 1;i < numhosts;i++){
            coeficiente = framedup->getCoeficiente(i);
            matrix_coeficiente[framedup->getSrcID()][i] = coeficiente;
        }
        trace()<< "matrix_coeficiente" << endl;
        for(int i = 0;i < numhosts;i++){
            for(int j = 1;j<numhosts;j++){
                trace()<< "["<<i<<"]["<<j<<"] = "<< matrix_coeficiente[i][j];
             }
            trace()<< endl;
        }

        trace()<< "Lista Coord Escutou" << endl;
        trace()<< "Numero de vizinhos Cood" <<neigmapNodosEscutados.size() <<endl;
        for(int i = 0; i< (int)neigmapNodosEscutados.size();i++){
            trace()<< "["<<i<<"] = "<<neigmapNodosEscutados[i];
        }
        codificador->n_equations++;
        for(int i=0; i< MSG_SIZE;i++){
            matrix_combination[framedup->getSrcID()][i] = framedup->getPayload(i);
        }

        MessagesNeighborhood *neig = neigmapNodosEscutados[framedup->getSrcID()];
        neig->setFrameRetransmission(framedup);
        parseMatrix();
        primeiraRetransCod = true;
        //recuperadas = sucessoMsgCodRecebida;
        num_msg_decod_sucess = num_msg_decod_sucess + sucessoMsgCodRecebida;
        //sucessoMsgCodRecebida = codificador->solve_system(sucessoMsgCodRecebida);
        sucessoMsgCodRecebida = codificador->solveSystem(sucessoMsgCodRecebida);
        if(sucessoMsgCodRecebida == 0){
            var_msg_notRecover++;
        }
        cout << " sucessoMsgCodRecebida retrans = " << sucessoMsgCodRecebida << endl;
        //recuperadas = sucessoMsgCodRecebida - recuperadas;
        //trace()<< "Mensagens Recuperadas" <<recuperadas<<endl;
        for(int i = 1;i < numhosts;i++){
            recoverPerNode[i] = codificador->recoverPerNode[i];
        }

}
/*
 * Esse método armazena as mensagens que o coordenador e o nodo cooperante escutaram e que chegaram por transmissão direta
 *
 * */

void Basic802154::listarNodosEscutadosTransmissaoNetworkCoding(Basic802154Packet *rcvPacket){

    if(isPANCoordinator){
        Basic802154Packet* framedup = rcvPacket->dup();
        if(rcvPacket->getRetransmissao() == false){
            for(int i =0; i< MSG_SIZE;i++){
                buffer_msg[framedup->getSrcID()][i] = framedup->getPayload(i);
            }

            MessagesNeighborhood *neig = new MessagesNeighborhood();
            neig->setFrameTransmission(framedup);
            neigmapNodosEscutados[framedup->getSrcID()] = neig;
            codificador->received[framedup->getSrcID()] = 1;
            GACK[framedup->getSrcID()] = 1;
            sucessoMsgDirRecebida++;
            cout << " sucessoMsgCodRecebida trans = " << sucessoMsgDirRecebida << endl;
        }
    }else{
        if((cooperador) || (cooperanteAuxiliar) || (cooperanteAuxiliarAuxiliar)){
            cout<< "Sou o nodo: " << self << endl;
            if (rcvPacket->getMac802154PacketType() == MAC_802154_DATA_PACKET && rcvPacket->getRetransmissao() == false){
                Basic802154Packet* framedup = rcvPacket->dup();
                MessagesNeighborhood *neig = new MessagesNeighborhood();
                neig->setFrameTransmission(framedup);
                neigmapNodosEscutados[framedup->getSrcID()] = neig;

            }


        }
   }
}





// método Ríad
void Basic802154::listarNodosEscutados(Basic802154Packet *rcvPacket,
        double rssi) {
    int repetido = 0;
    if (isPANCoordinator) {
        cout << "Sou o nodo:" << SELF_MAC_ADDRESS << " número de sequencia: "
                << rcvPacket->getSeqNum() << " Nodo que enviou: "
                << rcvPacket->getSrcID() << "\n";
    }
    if (rcvPacket->getDadosVizinhoArraySize() == 0 && rcvPacket->getRetransmissao() == false
            && rcvPacket->getSrcID() != 0 ) { // evita que retransmissoes sejam retransmitidas novamente

        for (int i = 0; i < (int) nodosEscutados.size(); i++) {
            if (nodosEscutados[i].idMens == rcvPacket->getSeqNum()
                    && nodosEscutados[i].idNodo == rcvPacket->getSrcID()) {
                repetido = 1;
                break;
            }
        }
        if (repetido == 0 && rssi >= limiteRSSI) { // nodos comuns só escutam seus vizinhos se o rssi for maior que -87Dbm
            MENSAGENS_ESCUTADAS escutados;
            escutados.idMens = rcvPacket->getSeqNum();
            escutados.idNodo = rcvPacket->getSrcID();
            nodosEscutados.push_back(escutados); // insere nos nodos escutados
            //cout<<"Inserindo Escutado: "<< nodosEscutados.front() <<"\n";
            if(isPANCoordinator){
                pacotesEscutadosT[rcvPacket->getSrcID()] = pacotesEscutadosT[rcvPacket->getSrcID()] + 1;
            }
            if(useNetworkCoding){
                //irá armazenar a mensagem (transmissão) escutada se for coordenador ou cooperante
                listarNodosEscutadosTransmissaoNetworkCoding(rcvPacket);
            }
        }

    }
    if(useNetworkCoding){
        if(rcvPacket->getRetransmissao() == true){
            if (isPANCoordinator) {
                retransCoded++; //Número de mensagens codificadas recebidas
                // irá armazenar a retransmissão escutada se for coordenador
                listarNodosEscutadosRetransmissaoNetworkCoding(rcvPacket);
            }

        }
  }
}
//Suelen Este método armazena as retransmissoes por beacon interval
void Basic802154::armazenaRetransmissoes(Basic802154Packet *rcvPacket) {

    if (historicoDeCooperacao.find(rcvPacket->getSrcID())
            != historicoDeCooperacao.end()) {

        map<int, vector<MENSAGENS_ESCUTADAS>*>::iterator iter;
        iter = historicoDeCooperacao.find(rcvPacket->getSrcID());
        vector<MENSAGENS_ESCUTADAS>* v = iter->second;
        for (int i = 0; i < (int) rcvPacket->getDadosVizinhoArraySize(); i++) {
            MENSAGENS_ESCUTADAS escutados;
            escutados.idMens = rcvPacket->getDadosVizinho(i).idMens;
            escutados.idNodo = rcvPacket->getDadosVizinho(i).idNodo;
            v->push_back(escutados);
        }

    } else {
        vector<MENSAGENS_ESCUTADAS> *vetor = new vector<MENSAGENS_ESCUTADAS>;
        for (int i = 0; i < (int) rcvPacket->getDadosVizinhoArraySize(); i++) {
            MENSAGENS_ESCUTADAS escutados;
            escutados.idMens = rcvPacket->getDadosVizinho(i).idMens;
            escutados.idNodo = rcvPacket->getDadosVizinho(i).idNodo;
            vetor->push_back(escutados);
            historicoDeCooperacao[rcvPacket->getSrcID()] = vetor;
        }

    }

}

void Basic802154::verificarRetransmissao(Basic802154Packet *rcvPacket, double rssi) {
    int i = 0, j = 0, repetido = 0;
    int utilidadeRetransmissao = 0;

        vector<MENSAGENS_ESCUTADAS> *vetor = new vector<MENSAGENS_ESCUTADAS>;
        if (rcvPacket->getRetransmissao() == true) {
            msgRtrans++;
        //}

        if (rcvPacket->getDadosVizinhoArraySize() > 0) {
            if(rssi > limiteRSSI){
                cout << "Numer de escutados: " << rcvPacket->getDadosVizinhoArraySize()
                        << "\n";
                trace()<< "Numer de escutadosCoopSemCood: " << rcvPacket->getDadosVizinhoArraySize();
                /** Salvando todas as cooperações para excluir cooperantes que estão repetindo as mensagens**/
                armazenaRetransmissoes(rcvPacket);

                if (historicoDeSucesso.find(rcvPacket->getSrcID())
                        != historicoDeSucesso.end()) {

                    for (int c = 0; c < (int) nodosEscutados.size(); c++) {
                        cout << "nodosEscutados[" << c << "].idNodo: "
                                << nodosEscutados[c].idNodo << "\n";
                        cout << "nodosEscutados[" << c << "].idMens: "
                                << nodosEscutados[c].idMens << "\n";
                        trace() << "nodosEscutados[" << c << "].idNodo: " << nodosEscutados[c].idNodo ;
                        trace() << "nodosEscutados[" << c << "].idMens: " << nodosEscutados[c].idMens;
                    }
                    /*for (i = 0; i < (int) rcvPacket->getDadosVizinhoArraySize(); i++) {
                        trace() << "getDadosVizinho[" << i << "].idNodo: "<< rcvPacket->getDadosVizinho(i).idNodo ;
                        trace() << "getDadosVizinho([" << i << "].idMens: "<< rcvPacket->getDadosVizinho(i).idMens;
                    }*/
                    map<int, vector<MENSAGENS_ESCUTADAS>*>::iterator iter;
                    iter = historicoDeSucesso.find(rcvPacket->getSrcID());
                    vector<MENSAGENS_ESCUTADAS>* v = iter->second;

                    for (i = 0; i < (int) rcvPacket->getDadosVizinhoArraySize(); i++) {
                        repetido = 0;
                        for (j = 0; j < (int) nodosEscutados.size(); j++) {
                            //if(nodosEscutados[j].idMens == rcvPacket->getDadosVizinho(i)){
                            if (nodosEscutados[j].idMens
                                    == rcvPacket->getDadosVizinho(i).idMens
                                    && nodosEscutados[j].idNodo
                                            == rcvPacket->getDadosVizinho(i).idNodo) {
                                repetido = 1;
                                MENSAGENS_ESCUTADAS_REPETIDAS repetidos;
                                repetidos.idMens = rcvPacket->getDadosVizinho(i).idMens;;
                                repetidos.idNodo = rcvPacket->getDadosVizinho(i).idNodo;
                                repetidos.idRetransmissor =  rcvPacket->getSrcID();
                                retransmissoesRepetidas.push_back(repetidos);
                                break;
                            }
                        }
                        if (repetido == 0) {
                            //cooperacoesDoBeacon[rcvPacket->getDadosVizinho(i).idNodo] = true;
                            MENSAGENS_ESCUTADAS escutados;
                            escutados.idMens = rcvPacket->getDadosVizinho(i).idMens;
                            escutados.idNodo = rcvPacket->getDadosVizinho(i).idNodo;
                            v->push_back(escutados);
                            historicoDeSucessoBeacon[rcvPacket->getSrcID()] = v;
                            nodosEscutados.push_back(escutados); // Se a msg que veio da cooperação não havia sido escutada agoa foi, por isso add aqui
                            utilidadeCoop++;
                            utilidadeRetransmissao++;
                            trace()<<"inseri em sucesso o nodo: "<<escutados.idNodo << " e a msg: "<< escutados.idMens<< " Quem escutou foi: "<<rcvPacket->getSrcID();



                        }
                    }
                } else {
                    for (int c = 0; c < (int) nodosEscutados.size(); c++) {
                        cout << "nodosEscutados[" << c << "].idNodo: "<< nodosEscutados[c].idNodo << "\n";
                        cout << "nodosEscutados[" << c << "].idMens: "<< nodosEscutados[c].idMens << "\n";
                        trace() << "nodosEscutados[" << c << "].idNodo: " << nodosEscutados[c].idNodo ;
                        trace() << "nodosEscutados[" << c << "].idMens: " << nodosEscutados[c].idMens;
                    }
                    for (i = 0; i < (int) rcvPacket->getDadosVizinhoArraySize(); i++) {
                        repetido = 0;
                        for (j = 0; j < (int) nodosEscutados.size(); j++) {
                            if (nodosEscutados[j].idMens
                                    == rcvPacket->getDadosVizinho(i).idMens
                                    && nodosEscutados[j].idNodo
                                            == rcvPacket->getDadosVizinho(i).idNodo) {
                                repetido = 1;
                                MENSAGENS_ESCUTADAS_REPETIDAS repetidos;
                                repetidos.idMens = rcvPacket->getDadosVizinho(i).idMens;
                                repetidos.idNodo = rcvPacket->getDadosVizinho(i).idNodo;
                                repetidos.idRetransmissor =  rcvPacket->getSrcID();
                                retransmissoesRepetidas.push_back(repetidos);
                                break;
                            }
                        }
                        if (repetido == 0) {
                            //cooperacoesDoBeacon[rcvPacket->getDadosVizinho(i).idNodo] = true;
                            MENSAGENS_ESCUTADAS escutados;
                            escutados.idMens = rcvPacket->getDadosVizinho(i).idMens;
                            escutados.idNodo = rcvPacket->getDadosVizinho(i).idNodo;
                            vetor->push_back(escutados);
                            nodosEscutados.push_back(escutados); // Se a msg que veio da cooperação não havia sido escutada agoa foi, por isso add aqui
                            utilidadeCoop++;
                            utilidadeRetransmissao++;
                            historicoDeSucesso[rcvPacket->getSrcID()] = vetor;
                            historicoDeSucessoBeacon[rcvPacket->getSrcID()] = vetor;
                            trace()<<"inseri em sucesso o nodo: "<<escutados.idNodo << " e a msg: "<< escutados.idMens<< " Quem escutou foi: "<<rcvPacket->getSrcID();
                        }
                    }
                    //historicoDeSucesso[rcvPacket->getSrcID()] = vetor;
                }
            }
            }
            if (utilidadeRetransmissao != 0) {
                mensagensRecuperadas = mensagensRecuperadas
                        + utilidadeRetransmissao;
                retransmissoesEfetivas++;
                //cout<<"Retransmissões Uteis: "<< retransmissoesEfetivas<< "\n";
            } else {
                retransmissoesNaoEfetivas++;
                //cout<<"Retransmissões que não foram Uteis: "<< retransmissoesNaoEfetivas<< "\n";
            }


            cout << "O nodo retransmitiu " << (int) rcvPacket->getDadosVizinhoArraySize() << " Mensagens e " << utilidadeRetransmissao << " Foram uteis.\n";
            //cout<<"Até esta retransmissão este cooperante retransmitu "<<utilidadeCoop <<" Mensagens uteis.\n";
            cout << "Retransmissões Uteis: " << retransmissoesEfetivas << "\n";
            trace() << "Retransmissões Uteis: " << retransmissoesEfetivas;
            cout << "Retransmissões que não foram Uteis: " << retransmissoesNaoEfetivas << "\n";
            trace() << "Retransmissões que não foram Uteis: " << retransmissoesNaoEfetivas;

        }


}
//Suelen contabiliza as mensagens recuperadas por nodo
void Basic802154::contabilizarMsgRetransmissores() {
    std::map<int, vector<MENSAGENS_ESCUTADAS>*>::iterator iter;
    vector<MENSAGENS_ESCUTADAS>::iterator i;


    for (iter = historicoDeSucesso.begin();iter != historicoDeSucesso.end(); iter++) {
        vector<MENSAGENS_ESCUTADAS> *nodo = iter->second;
        cout << "Cooperante: " << iter->first << "\n";
        trace() << "Cooperante: " << iter->first;
        for (i = (iter->second)->begin(); i != (iter->second)->end(); i++) {
            cout << "Nodo: " << i->idNodo << "\n";
            cout << "Mens: " << i->idMens << "\n";
            trace() << "Hist-Nodo: " << i->idNodo;
            trace() << "Mens: " << i->idMens;

                for(int j = 0; j < numhosts;j++){
                    if(i->idNodo == j){
                        msgRecuperadas[j] = msgRecuperadas[j] + 1;
                       trace()<<"recuperadas: "<<msgRecuperadas[j];
                        break;
                    }

                }


        }
    }

}
/*
//Suelen contabiliza as mensagens recuperadas por nodo em um beacon - Está correto, mas não estou usando no momento
void Basic802154::contabilizarRetransmissoes() {
    std::map<int, vector<MENSAGENS_ESCUTADAS>*>::iterator iter;
    vector<MENSAGENS_ESCUTADAS>::iterator i;
    qntdMsgRecebCoop = 0;


    for (iter = historicoDeSucessoBeacon.begin();iter != historicoDeSucessoBeacon.end(); iter++) {
        vector<MENSAGENS_ESCUTADAS> *nodo = iter->second;
        cout << "Cooperante: " << iter->first << "\n";
        trace() << "Cooperante: " << iter->first;
        for (i = (iter->second)->begin(); i != (iter->second)->end(); i++) {
            cout << "Nodo: " << i->idNodo << "\n";
            cout << "Mens: " << i->idMens << "\n";
            trace() << "Hist-Nodo: " << i->idNodo;
            trace() << "Mens: " << i->idMens;

                for(int j = 0; j < numhosts;j++){
                    if(i->idNodo == j){
                        msgRecuperadasBeacon[j] = msgRecuperadasBeacon[j] + 1;
                        trace()<<"recuperadas: "<<msgRecuperadasBeacon[j];
                        qntdMsgRecebCoop = qntdMsgRecebCoop + msgRecuperadasBeacon[j];
                        break;
                    }

                }


        }
    }

}*/





//Suelen Verifica os nodos que retransmitiram todas as mensagens repetidas
void Basic802154::verificaRetransmissoesRepetidas() {
    std::map<int, vector<MENSAGENS_ESCUTADAS>*>::iterator iter;
    std::map<int, vector<MENSAGENS_ESCUTADAS>*>::iterator iterProx;
    vector<MENSAGENS_ESCUTADAS>::iterator i;
    vector<MENSAGENS_ESCUTADAS>::iterator j;

    for (iter = historicoDeCooperacao.begin();
            iter != historicoDeCooperacao.end(); iter++) {
        vector<MENSAGENS_ESCUTADAS> *nodo = iter->second;
        cout << "Cooperante: " << iter->first << "\n";
        for (i = (iter->second)->begin(); i != (iter->second)->end(); i++) {
            cout << "valor 1: " << i->idNodo << "\n";
            cout << "Mens: " << i->idMens << "\n";

            for (iterProx = iter++; iterProx != historicoDeCooperacao.end();
                    iterProx++) {
                vector<MENSAGENS_ESCUTADAS> *nodoProx = iterProx->second;
                for (j = (iterProx->second)->begin();
                        j != (iterProx->second)->end(); j++) {
                    cout << "valor 2: " << j->idNodo << "\n";
                    if (i->idNodo == j->idNodo && i->idMens == j->idMens) {
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
Basic802154Packet *Basic802154::pausarNodo(int PANid){
    Basic802154Packet *result = new Basic802154Packet("PAN pause node",
                MAC_LAYER_PACKET);
        result->setDstID(PANid);
        result->setPANid(PANid);
        result->setMac802154PacketType(MAC_802154_PAUSE_PACKET);
        result->setSrcID(SELF_MAC_ADDRESS);
        result->setByteLength(COMMAND_PKT_SIZE);
        return result;

}

Basic802154Packet *Basic802154::restartNodo(int PANid){
    Basic802154Packet *result = new Basic802154Packet("PAN restart node",
                MAC_LAYER_PACKET);
        result->setDstID(PANid);
        result->setPANid(PANid);
        result->setMac802154PacketType(MAC_802154_RESTART_PACKET);
        result->setSrcID(SELF_MAC_ADDRESS);
        result->setByteLength(COMMAND_PKT_SIZE);
        return result;

}



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
    //cout << "tipo de Pacote: " << rcvPacket->getMac802154PacketType() << endl;
    if (!rcvPacket) {
        return;
    }
    cout<<"sou o nodo: "<<self<<endl;
    if(pausado){
        //Colocar o nodo para dormir e deletar o pacote
        //toRadioLayer(createRadioCommand(SET_STATE, SLEEP));
        cout << "Sou o nodo: " << SELF_MAC_ADDRESS << " Estou pausado e Meu radio Está ligado\n";
        trace() << "Sou o nodo: " << SELF_MAC_ADDRESS << " Estou pausado e Meu radio Está ligado";
        //delete pkt;
        return;
    }else{
        if (userelay) {
            if (rcvPacket->getMac802154PacketType() == MAC_802154_DATA_PACKET /*|| (rcvPacket->getMac802154PacketType() == MAC_802154_PAUSE_PACKET)*/) { // Coloquei esse if pq quero guardar as inf apenas dos pacotes de dados
                AtualizarVizinhaca(rcvPacket, rssi); // insere o nodo que enviou o pacote como vizinho

                if (isPANCoordinator) {
                    listarNodosEscutados(rcvPacket, rssi); // insere o nodo que enviou o pacote como escutado
                    verificarRetransmissao(rcvPacket,rssi);
                }

                if ((cooperador) ||(cooperanteAuxiliar)|| (cooperanteAuxiliarAuxiliar)) { // essa variavel é setada ao ouvir o beacon
                    cout<<"sou o nodo: "<<self<<endl;
                    listarNodosEscutados(rcvPacket, rssi); // insere o nodo que enviou o pacote como escutado

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

            if (isPANCoordinator)
                break;          //PAN coordinators ignore beacons from other PANs
            if (associatedPAN != -1 && associatedPAN != rcvPacket->getPANid())
                break;          //Ignore, if associated to another PAN


            trace()<<"Recebi beacon "<< SELF_MAC_ADDRESS;
            nodosEscutados.clear();
            if(useNetworkCoding){
                neigmapNodosEscutados.clear();
                neigmapNodosEscutadosRefinamento.clear();
                neigmapNodosEnviados.clear();
                neigmapNodosEscutadosSelecionados.clear();
                vizinhosCoop.clear();
                secondRetrans = false;
                GACK.clear();
                nodesNotReceived.clear();
                if(useCoopAux){
                    souNodoCooperanteAuxiliar(rcvPacket);
                }

                for(int i = 0; i< numhosts; i++){
                 GACK.push_back(0);
                }


            }


            //Modificação Ríad
            if (userelay) {
                tempoDeBeacon = rcvPacket->getTempoBeacon();
                atualizarVizinhanca = rcvPacket->getTempAtualizVizinhanca();
                idBeacon = rcvPacket->getIdBeacon();//Verificará se ja é o id de ficar escutando os vizinhos
                souNodoCooperante(rcvPacket);
            }
            recvBeacons++;

            //cancel beacon timeout message (if present)
            cancelTimer(BEACON_TIMEOUT);

            //LimparBuffer
            cout << "Sou o nodo: " << SELF_MAC_ADDRESS << "\n";

            // Se recebeu o beacon, já solicitou pausa mas não teve resposta solicita novamente
            /*if(pausaEnviada){
                receiveBeacon_node(rcvPacket);
                attemptTransmission("CAP started");

            }else{*/

                        //cout<<"tamanho Buffer: "<<TXBuffer.size()<<"\n";
                        limparBufferAplicacao();
                //        // Gerar Mensagem da aplicação
                //        RadioControlMessage *msg1 = new RadioControlMessage("BEACON_CHEGADA",BEACON_CHEGADA1);
                //        msg1->setRadioControlMessageKind(BEACON_CHEGADA1);
                //        handleRadioControlMessage(msg1);

                        //this node is connected to this PAN (or will try to connect), update frame parameters
                        double offset = TX_TIME(rcvPacket->getByteLength());
                        //trace()<<"offset: "<< offset;
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
                        //delayRadio = 0;

                        //Suelen
                        if (userelay) {
                            primeiraRetrans = 0;
                            irDormir = 0;
                            inicioGTSRetrans = rcvPacket->getSlotInicioRetrans();
                            firstCoop = false;



                            //SUELEN
                            cout << "Beacon Recebido no Nodo:" << SELF_MAC_ADDRESS << "\n";
                            numeroDeCoop = 0;
                            numeroDeCoop = rcvPacket->getVizinhosOuNodosCooperantesArraySize();
                            if(useNetworkCoding){
                                thirdCoop = true;
                                for(int i = 0;i < (int)rcvPacket->getVizinhosOuNodosCooperantesArraySize();i++){
                                    vizinhosCoop.push_back(rcvPacket->getVizinhosOuNodosCooperantes(i));
                                }
                            }

                            cout << "Este Beacon informa que tem " << numeroDeCoop
                                    << " Nodos Cooperantes\n";
                            if ((int) rcvPacket->getGTSlistArraySize() > 0
                                    && rcvPacket->getVizinhosOuNodosCooperantesArraySize()
                                            > 0) {
                                //trace()<<"primeiraRetrans: "<<((rcvPacket->getGTSlist(inicioGTSRetrans).start - 1)* baseSlotDuration * (1 << frameOrder)* symbolLen) + getClock() - phyDelaySleep2Tx - offset;
                                // seta o tempo de inicio das retransmissões
                                primeiraRetrans =
                                        ((rcvPacket->getGTSlist(inicioGTSRetrans).start - 1)
                                                * baseSlotDuration * (1 << frameOrder)
                                                * symbolLen) + getClock() - phyDelaySleep2Tx - offset;
                            }
                        }

                        //cout<<"Devia ser Associado A: "<< rcvPacket->getPANid()<<"\n";
                        //cout<<"Associado A: "<< associatedPAN<<"\n";
                        for (int i = 0; i < (int) rcvPacket->getGTSlistArraySize(); i++) {
                            if (rcvPacket->getGTSlist(i).owner == SELF_MAC_ADDRESS
                                    && associatedPAN == rcvPacket->getPANid()) {

                                //Suelen obs:antes era só o conteudo do else
                                if (rcvPacket->getGTSlist(i).retransmissor) {

                                    if(useCoopAux){
                                        GTSstartRetrans = (rcvPacket->getGTSlist(i).start - 1)
                                                * baseSlotDuration * (1 << frameOrder) * symbolLen;
                                        GTSendRetrans = GTSstartRetrans
                                                + rcvPacket->getGTSlist(i).length * baseSlotDuration
                                                        * (1 << frameOrder) * symbolLen;
                                        GTSlengthRetrans = GTSendRetrans - GTSstartRetrans;

                                        cout << "Nodo " << rcvPacket->getGTSlist(i).owner
                                            << " Usa o GTS slot from "
                                            << getClock() + GTSstartRetrans << " to "
                                            << getClock() + GTSendRetrans << " length "
                                            << GTSlengthRetrans << "\n";

                                    // set GTS timer phyDelaySleep2Tx seconds earlier as radio can be sleeping
                                        setTimer(GTS_RETRANS,
                                            GTSstartRetrans - phyDelaySleep2Tx - offset);

                                    }else{

                                        if(firstCoop){
                                            cout << "Tamanho Lista: "
                                                    << (int) rcvPacket->getGTSlistArraySize()
                                                    << "Lista GTS[" << i << "]: "
                                                    << rcvPacket->getGTSlist(i).owner
                                                    << " Cooperante\n";
                                            GTSstartRetrans = (rcvPacket->getGTSlist(i).start - 1)
                                                    * baseSlotDuration * (1 << frameOrder) * symbolLen;
                                            GTSendRetrans = GTSstartRetrans
                                                    + rcvPacket->getGTSlist(i).length * baseSlotDuration
                                                            * (1 << frameOrder) * symbolLen;
                                            GTSlengthRetrans = GTSendRetrans - GTSstartRetrans;



                                            //delayRadio = offset;

                                            //trace()<<"tempo Atual: " << simTime();
                                            //trace()<<"getClok(): " << getClock();
                                            //trace()<<"GTSstartRetrans: " << GTSstartRetrans;
                                            //trace()<<"GTSendRetrans: " << GTSendRetrans;
                                            //trace()<<"GTSlengthRetrans: " << GTSlengthRetrans;


                                            //trace() << "GTS slot from " << getClock() + GTSstartRetrans
                                              //      << " to " << getClock() + GTSendRetrans
                                                //    << " length " << GTSlengthRetrans;
                                            cout << "Nodo " << rcvPacket->getGTSlist(i).owner
                                                    << " Usa o GTS slot from "
                                                    << getClock() + GTSstartRetrans << " to "
                                                    << getClock() + GTSendRetrans << " length "
                                                    << GTSlengthRetrans << "\n";

                                            // set GTS timer phyDelaySleep2Tx seconds earlier as radio can be sleeping
                                            setTimer(GTS_RETRANS,
                                                    GTSstartRetrans - phyDelaySleep2Tx - offset);
                                        }else{
                                            if(thirdCoop){
                                                thirdCoop = false;
                                                third_GTSstartRetrans = (rcvPacket->getGTSlist(i).start - 1)
                                                        * baseSlotDuration * (1 << frameOrder) * symbolLen;
                                                third_GTSendRetrans = third_GTSstartRetrans
                                                        + rcvPacket->getGTSlist(i).length * baseSlotDuration
                                                                * (1 << frameOrder) * symbolLen;
                                                third_GTSlengthRetrans = third_GTSendRetrans - third_GTSstartRetrans;

                                                cout << "Tamanho Lista: "
                                                    << (int) rcvPacket->getGTSlistArraySize()
                                                    << "Lista GTS[" << i << "]: "
                                                    << rcvPacket->getGTSlist(i).owner
                                                    << " Cooperante\n";

                                                cout << "Nodo " << rcvPacket->getGTSlist(i).owner
                                                                << " Usa o GTS slot from "
                                                                << getClock() + third_GTSstartRetrans << " to "
                                                                << getClock() + third_GTSendRetrans << " length "
                                                                << third_GTSlengthRetrans << "\n";

                                                setTimer(GTS_THIRD_RETRANS, third_GTSstartRetrans - phyDelaySleep2Tx - offset);

                                            }
                                            else{
                                                firstCoop = true;
                                                second_GTSstartRetrans = (rcvPacket->getGTSlist(i).start - 1)
                                                        * baseSlotDuration * (1 << frameOrder) * symbolLen;
                                                second_GTSendRetrans = second_GTSstartRetrans
                                                        + rcvPacket->getGTSlist(i).length * baseSlotDuration
                                                                * (1 << frameOrder) * symbolLen;
                                                second_GTSlengthRetrans = second_GTSendRetrans - second_GTSstartRetrans;

                                                cout << "Tamanho Lista: "
                                                    << (int) rcvPacket->getGTSlistArraySize()
                                                    << "Lista GTS[" << i << "]: "
                                                    << rcvPacket->getGTSlist(i).owner
                                                    << " Cooperante\n";

                                                cout << "Nodo " << rcvPacket->getGTSlist(i).owner
                                                                << " Usa o GTS slot from "
                                                                << getClock() + second_GTSstartRetrans << " to "
                                                                << getClock() + second_GTSendRetrans << " length "
                                                                << second_GTSlengthRetrans << "\n";

                                                setTimer(GTS_SECOND_RETRANS, second_GTSstartRetrans - phyDelaySleep2Tx - offset);
                                            }
                                        }
                                    }

                                } else {
                                    cout << "Tamanho Lista: "
                                            << (int) rcvPacket->getGTSlistArraySize()
                                            << "Lista GTS[" << i << "]: "
                                            << rcvPacket->getGTSlist(i).owner << "\n";
                                    GTSstart = (rcvPacket->getGTSlist(i).start - 1)
                                            * baseSlotDuration * (1 << frameOrder) * symbolLen;
                                    GTSend = GTSstart
                                            + rcvPacket->getGTSlist(i).length * baseSlotDuration
                                                    * (1 << frameOrder) * symbolLen;
                                    GTSlength = GTSend - GTSstart;

                                    //trace()<<"tempo Atual: " << simTime();
                                    //trace()<<"getClok(): " << getClock();
                                    //trace()<<"GTSstart: " << GTSstart;
                                    //trace()<<"GTSend: " << GTSend;
                                    //trace()<<"GTSlength: " << GTSlength;

                                    limparBufferAplicacao();
                                    // Gerar Mensagem da aplicação
                                    RadioControlMessage *msg1 = new RadioControlMessage(
                                            "BEACON_CHEGADA", BEACON_CHEGADA1);
                                    msg1->setRadioControlMessageKind(BEACON_CHEGADA1);
                                    handleRadioControlMessage(msg1);

                                    //trace() << "GTS slot from " << getClock() + GTSstart
                                      //      << " to " << getClock() + GTSend << " length "
                                        //    << GTSlength;
                                    cout << "Nodo " << rcvPacket->getGTSlist(i).owner
                                            << " Usa o GTS slot from " << getClock() + GTSstart
                                            << " to " << getClock() + GTSend << " length "
                                            << GTSlength << "\n";

                                }

                            }
                        }
                        //Suelen
                        if (userelay) {
                            if (associatedPAN == rcvPacket->getPANid()) {
                                if ((int) rcvPacket->getGTSlistArraySize() > 0) {
                                    irDormir = (((rcvPacket->getGTSlist(0).start - 1)
                                            * baseSlotDuration * (1 << frameOrder) * symbolLen)
                                            + (rcvPacket->getGTSlist(0).length
                                                    * baseSlotDuration * (1 << frameOrder)
                                                    * symbolLen) + getClock());
                                }
                            }

                        }

                        if (associatedPAN != rcvPacket->getPANid()) {
                            setTimer(BACK_TO_SETUP, CAPend - offset);
                        }
                        setMacState(MAC_STATE_CAP);
                        if (associatedPAN == rcvPacket->getPANid()) {
                            if (GTSstart != CAPend) {
                                //SUELEN
                                //COMENTEI PQ NESTE COMANDO SÓ ACORDA EM SEU MOMENTO D TRANSMISSAO
                                // set timer to sleep after CAP, unless GTS slots starts right after
                                //setTimer(SLEEP_START, CAPend - offset);
                            }
                            if (GTSstart > 0) {
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

                        if (associatedPAN == rcvPacket->getPANid() && GTSstart > 0) {
                            if((int) rcvPacket->getGTSlistArraySize()>0){
                                trace()<<"idBeacon FromRadio: "<<idBeacon;
                                setTimer(SLEEP_START, 0);
                                //Verificação para acordar ao iniciar as transmissoes se for a técnica smart e estiver um beacon antes de realizar a selecao
                                //if(/*tempoDeBeacon >= selecao -2 &&*/ smart){ // -2 pq ele preenche a lista de vizinhos ao receber o beacon, assim no 2º beacon fica escutando no 3 envia qm escutou e no 4 o coor seleciona os coop
                                    //setTimer(WAKE_UP_START, CAPend - phyDelaySleep2Tx - offset);
                                //}else{// acorda apenas antes de transmitir
                                    //setTimer(WAKE_UP_START, GTSstart - phyDelaySleep2Tx - offset);
                                //}

                                //Essa isso antes de alterar para as outras técnicas serem diferentes da smart
                                //setTimer(WAKE_UP_START, CAPend - phyDelaySleep2Tx - offset);


                                //Se for cooperante irá escutar todos os vizinhos
                               /* if(cooperador){// Se o nodo for cooperante acorda no inicio para escutar os vizinhos
                                    setTimer(WAKE_UP_START, CAPend - phyDelaySleep2Tx - offset);
                                }else{
                                    //Alteração para que na smart os nodos nao escutem a vizinhança o tempo todo
                                    if(smart && idBeacon < 5){ //período de configuração da rede, escuta os vizinhos nos primeiros 5 beacons
                                       setTimer(WAKE_UP_START, CAPend - phyDelaySleep2Tx - offset);
                                    }else{
                                        // se não for cooperante verificará se é o momento de atualizar a vizinhança ou não
                                       if(idBeacon == atualizarVizinhanca && smart){ // verifica se ja está no periodo de escutar os vizinhos
                                           //atualizarVizinhanca = idBeacon + 5;
                                           setTimer(WAKE_UP_START, CAPend - phyDelaySleep2Tx - offset);

                                       }else{// acorda apenas antes de transmitir (não fica escutando os vizinhos)
                                           setTimer(WAKE_UP_START, GTSstart - phyDelaySleep2Tx - offset);
                                       }
                                    }
                                }*/

//                                //Alteração para que na smart os nodos nao escutem a vizinhança o tempo todo
//                                  if(!cooperador && (smart || smartPeriodic) && idBeacon < 5){ //período de configuração da rede, escuta os vizinhos nos primeiros 5 beacons
//                                     setTimer(WAKE_UP_START, CAPend - phyDelaySleep2Tx - offset);
//                                  }
//                                  //Se for cooperante irá escutar todos os vizinhos
//                                  if((smart || smartPeriodic) && cooperador){// Se o nodo for cooperante acorda no inicio para escutar os vizinhos
//                                      setTimer(WAKE_UP_START, CAPend - phyDelaySleep2Tx - offset);
//                                  }else{
//                                      // se não for cooperante verificará se é o momento de atualizar a vizinhança ou não
//                                     if(idBeacon == atualizarVizinhanca && (smart || smartPeriodic)){ // verifica se ja está no periodo de escutar os vizinhos
//                                         //atualizarVizinhanca = idBeacon + 5;
//                                         setTimer(WAKE_UP_START, CAPend - phyDelaySleep2Tx - offset);
//
//                                     }else{// acorda apenas antes de transmitir (não fica escutando os vizinhos)
//                                         setTimer(WAKE_UP_START, GTSstart - phyDelaySleep2Tx - offset);
//                                     }
//                                  }

                                //Alteração para que na smart os nodos nao escutem a vizinhança o tempo todo
                                  if((smart || smartPeriodic) && idBeacon < 5){ //período de configuração da rede, escuta os vizinhos nos primeiros 5 beacons
                                     setTimer(WAKE_UP_START, CAPend - phyDelaySleep2Tx - offset);
                                  }else{
                                      //Se for cooperante irá escutar todos os vizinhos
                                      //if((smart || smartPeriodic) && cooperador){// Se o nodo for cooperante acorda no inicio para escutar os vizinhos
                                      if((cooperador) || (cooperanteAuxiliar) || (cooperanteAuxiliarAuxiliar)){//Se for cooperante irá escutar todos os vizinhos
                                          setTimer(WAKE_UP_START, CAPend - phyDelaySleep2Tx - offset);
                                      }else{
                                          if((smart || smartPeriodic) && !cooperador){
                                              // se não for cooperante verificará se é o momento de atualizar a vizinhança ou não
                                              if(atualizarVizinhanca){ // verifica se ja está no periodo de escutar os vizinhos
                                                  setTimer(WAKE_UP_START, CAPend - phyDelaySleep2Tx - offset);

                                              }else{// acorda apenas antes de transmitir (não fica escutando os vizinhos)
                                                  setTimer(WAKE_UP_START, GTSstart - phyDelaySleep2Tx - offset);
                                              }
                                          }else{ // se não for smart: acorda apenas antes de transmitir (não fica escutando os vizinhos)
                                              setTimer(WAKE_UP_START, GTSstart - phyDelaySleep2Tx - offset);
                                          }
                                      }
                               }
                            }
                        }
            //}

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
                cout << "Nodo " << rcvPacket->getSrcID() << " Associado \n";
                // update associatedDevices and reply with an ACK
                associatedDevices[rcvPacket->getSrcID()] = true;
                Basic802154Packet *ackPacket = new Basic802154Packet(
                        "PAN associate response", MAC_LAYER_PACKET);
                ackPacket->setPANid(SELF_MAC_ADDRESS);
                ackPacket->setMac802154PacketType(MAC_802154_ACK_PACKET);
                ackPacket->setDstID(rcvPacket->getSrcID());
                ackPacket->setByteLength(ACK_PKT_SIZE);

                //int repetido = 0;
                //Insere na lista de associados se ainda nao estiver---Quando implementar a desassociação isso não será necessario
                /*for (int k = 0; k < (int)  nodosAssociados.size(); k++) {
                   if ( nodosAssociados[k] == rcvPacket->getSrcID()) {
                       repetido = 1;
                       break;
                   }

                }
                if(repetido == 0){*/
                    nodosAssociados.push_back(rcvPacket->getSrcID());
                    nchildren++;
                    nchildrenActual++;
               // }

                //nodosAssociados.push_back(rcvPacket->getSrcID());

                //Conta como filho!!!
                //nchildren++;

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

        /*case MAC_802154_CONFIRMATION_PAUSE:{
            if(isPanCoordinator){
               break;
            }

            trace()<<"Sou o nodo["<<SELF_MAC_ADDRESS <<"] Coordenador já sabe que Vou Pausar";
            setMacState(MAC_STATE_SETUP);
            associatedPAN = -1;
            desyncTimeStart = getClock();
            disconnectedFromPAN_node();
            toRadioLayer(createRadioCommand(SET_STATE, SLEEP));
            setMacState(MAC_STATE_SLEEP);
            pausado = true;
            //resMgrModule->destroyNode();

            break;


        }*/

        case MAC_802154_PAUSE_PACKET: {//Suelen -- coordenador recebe a mensagem do nodo sinalizando que ira pausar
            cout << "Eu sou o: " << SELF_MAC_ADDRESS << "\n";
            cout <<"Recebi o pacote do nodo: " << rcvPacket->getSrcID() << "Sinalizando que ele irá pausar \n";

            if (isPANCoordinator) {
                  nchildrenActual--;
                  nchildren --;
                  int t=0;
                  std::vector<int>::iterator iter;
                   for (iter = nodosAssociados.begin();iter != nodosAssociados.end(); iter++) {
                       if(nodosAssociados[t] == rcvPacket->getSrcID()){
                           nodosAssociados.erase(iter);
                           break;
                       }
                       t++;
                   }
                   associatedDevices[rcvPacket->getSrcID()] = false;

                   // --------- Considero que na msg vem junto a sinalização de pause -------
                   if (isNotDuplicatePacket(rcvPacket)) {
                       dataReceived_hub(rcvPacket);
                       toNetworkLayer(decapsulatePacket(rcvPacket));
                   }
                   // Suelen -- Variavel utilizada para determinar o numero de coop
                   if (userelay) {
                          if (rcvPacket->getRetransmissao() == false) {
                               msgRecebidas++;
                               numdadosrecebidosnogtstransmissao++;

                           }

                           // se o nodo que solicitou pausa era cooperante é necessário uma nova selecao
                           if(redeDinamica && smart){
                               if(!processoSelecao){
                                  for(int i = 0; i < nodosColaboradores.size(); i++){
                                      cout << "nodosColaboradores" << nodosColaboradores[i] << "\n";
                                      if(nodosColaboradores[i] == rcvPacket->getSrcID()){
                                          coopPause = true;
                                          break;
                                      }
                                  }
                               }
                           }
                    }








                   // reply with an ACK
                   Basic802154Packet *ackPacket = new Basic802154Packet("PAN confirmation pause",
                           MAC_LAYER_PACKET);
                   ackPacket->setPANid(SELF_MAC_ADDRESS);
                   ackPacket->setMac802154PacketType(MAC_802154_ACK_PACKET);
                   ackPacket->setDstID(rcvPacket->getSrcID());
                   ackPacket->setByteLength(ACK_PKT_SIZE);

                   toRadioLayer(ackPacket);
                   toRadioLayer(createRadioCommand(SET_STATE, TX));
                   setTimer(ATTEMPT_TX, TX_TIME(ACK_PKT_SIZE));
            }
            break;
        }

        case MAC_802154_GACK:{
           cout <<"Sou o nodo: " << SELF_MAC_ADDRESS <<endl;
           cout <<"Pacote: " << rcvPacket->getMac802154PacketType() <<endl;
           nodesNotReceived.clear();
           if(useNetworkCoding){
                    if (cooperador || cooperanteAuxiliar || cooperanteAuxiliarAuxiliar) {
                            for(int i = 0; i < rcvPacket->getGackArraySize();i++){
                                cout<<"Lista GACK: " << rcvPacket->getGack(i)<< endl;
                                if(rcvPacket->getGack(i) == 0){
                                    cout << "Coordenador não recebeu a mensagem do nodo: " << i <<endl;
                                    nodesNotReceived.push_back(i);
                                }
                            }

                    }
                }
           break;
        }


            // data frame
        case MAC_802154_DATA_PACKET: {
            cout << "Eu sou o: " << SELF_MAC_ADDRESS << "\n";
            if ((rssi > limiteRSSI) || (rcvPacket->getPauseNodo())) { // Só irá receber a mensagem se o RSSI for bom

                if (isNotDuplicatePacket(rcvPacket)) {
                    dataReceived_hub(rcvPacket);
                    toNetworkLayer(decapsulatePacket(rcvPacket));
                } else {
                    //trace() << "Packet [" << rcvPacket->getName() << "] from node "<< rcvPacket->getSrcID() << " is a duplicate";
                    //cout<<"Duplicado\n";
                }
                // Suelen -- Variavel utilizada para determinar o numero de coop
                if (userelay) {
                    if (isPANCoordinator) {

                        if (rcvPacket->getRetransmissao() == false) {
                            msgRecebidas++;
                            numdadosrecebidosnogtstransmissao++;

                        }
                    }
               }
               if (isPANCoordinator) {
                    if (rcvPacket->getPauseNodo()){
                          nchildrenActual--;
                          nchildren --;
                          int t=0;
                          std::vector<int>::iterator iter;
                           for (iter = nodosAssociados.begin();iter != nodosAssociados.end(); iter++) {
                               if(nodosAssociados[t] == rcvPacket->getSrcID()){
                                   nodosAssociados.erase(iter);
                                   break;
                               }
                               t++;
                           }
                           associatedDevices[rcvPacket->getSrcID()] = false;

                                   // se o nodo que solicitou pausa era cooperante é necessário uma nova selecao
                                   if(redeDinamica && smart){
                                       if(!processoSelecao){
                                          for(int i = 0; i < nodosColaboradores.size(); i++){
                                              cout << "nodosColaboradores" << nodosColaboradores[i] << "\n";
                                              if(nodosColaboradores[i] == rcvPacket->getSrcID()){
                                                  coopPause = true;
                                                  break;
                                              }
                                          }
                                       }
                                  }


                           // reply with an ACK
                           Basic802154Packet *ackPacket = new Basic802154Packet("PAN confirmation pause",
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

    //received an ack while waiting for a response to Pause packet
    case MAC_802154_PAUSE_PACKET:{// suelen
        if (currentPacket->getSeqNum() == rcvPacket->getSeqNum()) {
            trace() << "Pause packet successfully transmitted to "
                    << rcvPacket->getSrcID() << ", local clock " << getClock();
            cout << "Pause packet successfully transmitted to "
                    << rcvPacket->getSrcID() << ", local clock " << getClock()
                    << "\n";

            trace()<<"Sou o nodo["<<SELF_MAC_ADDRESS <<"] Coordenador já sabe que Vou Pausar";
            setMacState(MAC_STATE_SETUP);
            associatedPAN = -1;
            desyncTimeStart = getClock();
            disconnectedFromPAN_node();
            toRadioLayer(createRadioCommand(SET_STATE, SLEEP));
            setMacState(MAC_STATE_SLEEP);
            pausado = true;
            pausaEnviada = false;
            neigmap.clear();// limpa seus vizinhos, para tentar evitar que seja selecionado como cooperante
            //resMgrModule->destroyNode();

            clearCurrentPacket("Success", true);
        }
        break;

    }
        //received an ack while waiting for a response to data packet
    case MAC_802154_DATA_PACKET: {
        if (currentPacket->getSeqNum() == rcvPacket->getSeqNum()) {
            string aux = rcvPacket->getName();
            cout<< "currentPacket->getName(): " <<currentPacket->getName() <<"\n";
            cout<< "rcvPacket->getName(): " <<rcvPacket->getName() <<"\n";
            //if(rcvPacket->getName() == "PAN confirmation pause"){
              if (aux.compare("PAN confirmation pause") == 0){
                trace()<<"Sou o nodo["<<SELF_MAC_ADDRESS <<"] Coordenador já sabe que Vou Pausar";
                setMacState(MAC_STATE_SETUP);
                associatedPAN = -1;
                desyncTimeStart = getClock();
                disconnectedFromPAN_node();
                toRadioLayer(createRadioCommand(SET_STATE, SLEEP));
                setMacState(MAC_STATE_SLEEP);
                pausado = true;
                pausaEnviada = false;
                neigmap.clear();// limpa seus vizinhos, para tentar evitar que seja selecionado como cooperante
            }

            trace() << "Data packet successfully transmitted to "
                    << rcvPacket->getSrcID() << ", local clock " << getClock();
            cout << "Data packet successfully transmitted to "
                    << rcvPacket->getSrcID() << ", local clock " << getClock()
                    << "\n";
            clearCurrentPacket("Success", true);
            aux = "";
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
    if (currentPacket) {
        // Contabiliza mensagens perdidas por timeout de ACK
        if (descr == "ACK timeout"
                && currentPacket->getMac802154PacketType() == 1003) {
            mensagensPerdidas++;
            //limparBufferAplicacao();
            trace() << "Mensagens Perdidas: " << mensagensPerdidas;
            cout << "Mensagens Perdidas: " << mensagensPerdidas << "\n";
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
            trace() << "Transmitting Nodo [" << currentPacket->getSrcID()
                    << "] in GTS Transmitting [" << currentPacket->getName()
                    << "] Id msg: "<< currentPacket->getSeqNum();
            cout << "Nodo [" << currentPacket->getSrcID()
                    << "] Transmitindo no GTS\n";

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
        for (int i = 0; i < (int) nodosEscutados.size(); i++) {

            nextPacket->setDadosVizinho(i, nodosEscutados[i]);

            cout << "Escutado vou retransmitir id: " << nodosEscutados[i].idNodo << "\n";
            cout << "Escutado mens: " << nodosEscutados[i].idMens << "\n";
            trace()<<"Escutado vou retransmitir id: " << nodosEscutados[i].idNodo;
            trace()<<"Escutado mens: " << nodosEscutados[i].idMens;
            //i++;
        }
    }
    if(useNetworkCoding){
        nextPacket->setPayloadArraySize(MSG_SIZE);
        for(int i = 0; i<MSG_SIZE;i++){
            nextPacket->setPayload(i,0);
        }
        nextPacket->setCoeficienteArraySize(numhosts);
        for(int i = 0; i<numhosts;i++){
            nextPacket->setCoeficiente(i,0);
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
        cout<<"currentFrameStart + GTSend: "<<currentFrameStart + GTSend<<endl;
        cout<<"txEndTime: "<<txEndTime<<endl;
        if(!isPANCoordinator){
            if (currentFrameStart + GTSend < txEndTime)
                //transmission will not fit in GTS
                allowTx = 0;
        }
    }

    if (allowTx) {
        if (currentPacket->getMac802154PacketType() == MAC_802154_DATA_PACKET) {
            if (macState == MAC_STATE_CAP) {
                collectOutput("pkt TX state breakdown", "Contention");
            } else {
                collectOutput("pkt TX state breakdown", "Contention-free");

            }
        }
        //decrement retry counter, set transmission end timer and modify mac and radio states.
        currentPacketRetries--;
        trace() << "Nodo " << currentPacket->getSrcID() << " |Transmitting ["
                << currentPacket->getName() << "] Id msg: "<< currentPacket->getSeqNum()<<" now, remaining attempts "
                << currentPacketRetries;
        //cout << "Nodo [" << currentPacket->getSrcID()<< "] Transmitindo no GTS\n";
        cout << "Transmitting [" << currentPacket->getName()
                << "] now, remaining attempts " << currentPacketRetries << "\n";


        setTimer(currentPacketResponse > 0 ? ACK_TIMEOUT : ATTEMPT_TX, txTime);
        toRadioLayer(currentPacket->dup());
        toRadioLayer(createRadioCommand(SET_STATE, TX));// realiza a transmissão
    } else {
        //transmission not allowed
        //trace() << "txTime " << txTime << " CAP:"<< (currentFrameStart + CAPend - getClock()) << " GTS:"<< (currentFrameStart + GTSend - getClock());
        trace() << "Transmission of [" << currentPacket->getName()
                << "] stopped, not enough time";
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
    //Aqui verifica se o nodo solocicitou pausa, se pausaEnviada = true, significa que não teve resp, então solicita novamente
    if(pausar){
           if(pausaEnviada){
                setMacState(MAC_STATE_CAP);
                transmitPacket(pausarNodo(0));
                //toRadioLayer(pausarNodo(0));// passo o id do nodo coordenador
                trace() << "Node: "<< SELF_MAC_ADDRESS <<"Transmitting [PAN pause node]";
                cout << "Node: "<< SELF_MAC_ADDRESS <<"Transmitting [PAN pause node] \n";

                //toRadioLayer(createRadioCommand(SET_STATE, TX));

             }
     }

    if (getAssociatedPAN() == -1){
        transmitPacket(newConnectionRequest(beacon->getPANid()));
    }
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
void Basic802154::calculaNumNodosCooperantes() {
    numeronodoscooperantes = 0;
    //taxa de perda
    SamLoss = numhosts - numdadosrecebidosnogtstransmissao - 1;

    std::map<int, int>::iterator iter;
    int msgEnviadasTotal = 0;

    vectortaxaperda.recordWithTimestamp(simTime(), SamLoss);

    EV << "#####numdadosrecebidosnogtstransmissao:"
              << numdadosrecebidosnogtstransmissao << "#####SamLoss:" << SamLoss
              << endl;
    cout << "#####numdadosrecebidosnogtstransmissao:"
            << numdadosrecebidosnogtstransmissao << "#####SamLoss:" << SamLoss
            << "\n";

    //reinciando a variável que contabiliza a quantidade de msg que chegaram no período de transmissão.
    numdadosrecebidosnogtstransmissao = 0;

    if (alpha > limiaralpha)
        alpha = alpha - 0.1;
    if (betha > limiarbetha)
        betha = betha - 0.1;

    EstLoss = (1 - alpha) * EstLoss + alpha * SamLoss;

    DevLoss = (1 - betha) * DevLoss + betha * fabs(SamLoss - EstLoss);
    //abs(SamLoss - EstLoss);

    EV << " alpha " << alpha << " betha " << betha << "DevLoss:" << DevLoss
              << "EstLoss:" << EstLoss << endl;

    numeronodoscooperantes = (int) (ceil(ganho * EstLoss + DevLoss));

    EV << "[Conferencia] numeronodoscooperantes:" << numeronodoscooperantes
              << endl;
    cout << "[Conferencia] numeronodoscooperantes:" << numeronodoscooperantes
            << "\n";

}
void Basic802154::limparBufferAplicacao() {
    while (!TXBuffer.empty()) {
        cout << "Limpando Buffer: " << TXBuffer.front();
        cancelAndDelete(TXBuffer.front()); // cancela todos os eventos do omnet
        TXBuffer.pop();
        mensagensPerdidas++;
        cout << "Mens Perdidas Por limpeza de buffer: " << mensagensPerdidas
                << " Nodo: " << SELF_MAC_ADDRESS << "\n";
        //trace() << "Mens Perdidas Por limpeza de buffer: " << mensagensPerdidas
                //<< " Nodo: " << SELF_MAC_ADDRESS << "\n";
    }

}

void Basic802154::selecaoCoopAleatoria(Basic802154Packet *beaconPacket) {
    int numCoop = 0, qntNodosCoordEscuta = 0, qntNodosCoordNaoEscuta = 0, coop = 0;
    int limiteCoop = 0;
    int repetido=0;
    int novoLimit=0;

    if(neigmap.size()>0){
        qntNodosCoordEscuta = neigmap.size();
        qntNodosCoordNaoEscuta = (numhosts - 1) - neigmap.size();

        cout << "Quantdad de Nodos que o Cood Escuta: " << qntNodosCoordEscuta<< "\n";
        cout << "Coord Nao escuta: " << qntNodosCoordNaoEscuta << "\n";
        limiteCoop = min(min(qntNodosCoordEscuta, qntNodosCoordNaoEscuta),numCoopMax);// Limitar em 40 como o número máximo de cooperantes
        numCoop = rand() % (limiteCoop) + 1; // seleciona de 1 ao min(escutaCoord,naoEscutaCoord);
        cout << "numCoop: " << numCoop << "\n";
        trace() << "Comunicacao direta: " << neigmap.size();
        trace() << "Comunicacao Falha: " << qntNodosCoordNaoEscuta;
        trace() << "Seleção de cooperantes:";
        trace() << "numCoop: " << numCoop << "\n";

        int j = 0;
        std::map<int, Neighborhood*>::iterator iterNeighborhood;
        nodosColaboradores.clear();
        while (j != numCoop) {
            coop = rand() % (numhosts - 1) + 1;
            repetido=0;
            for (j = 0; j < (int) nodosColaboradores.size(); j++) {
                if (nodosColaboradores[j] == coop) {
                    repetido = 1;
                    break;
                }

            }
            if(repetido==0){
            for (iterNeighborhood = neigmap.begin();
                    iterNeighborhood != neigmap.end(); iterNeighborhood++) {
                Neighborhood *nodo = iterNeighborhood->second;

                if (nodo->nodeId == coop) {
                    nodosColaboradores.push_back(nodo->nodeId);
                    cout << "Selecionado: " << coop << "\n";
                    j++;
                    break;
                }

            }
            }

        }
    }
}
void Basic802154::selecaoCompletamenteAleatoria(Basic802154Packet *beaconPacket){
    trace()<<"Entrei para selecionar coop";
    int numCoop = 0, j = 0, coop = 0, repetido=0, maxCoop = 0, nodosRede = 0, associado = 1;
    std::vector<int>::iterator iter;
    nodosColaboradores.clear();

    //nodosRede = numhosts-1;
    nodosRede = nodosAssociados.size();
    trace() << "Nodos Rede: " << numhosts-1;
    trace()<<"Nodos Associados: " << nodosAssociados.size();

    if(numCoopMax > (nodosRede)){
        maxCoop = nodosRede;
    }else{
        maxCoop = numCoopMax;
    }
    // número de cooperantes a serem selecionados
    numCoop = rand() % (maxCoop) + 1;
    cout << "NumCoop: " << numCoop << "\n";
    trace()<<"NumCoop: " << numCoop;
    /*for(int i = 0;i <nodosAssociados.size() ; i++){
        trace()<<"Associado: "<<nodosAssociados[i];
    }*/


//    while (j != numCoop) {
//           coop = rand() % (numhosts-1) + 1;
//           repetido=0;
//
//           //tava comentado esse bloco
//            associado = 1;
//            for (int i = 0;i < (int) nodosAssociados.size() ; i++) {
//               if(nodosAssociados[i] == coop){
//                   associado = 0;
//                   break;
//               }
//           }
//            if(associado!=0){
//                trace()<<"nao associado: "<<coop;
//
//            }
//         if(associado == 0){// tava comentado aq
//               for (int k = 0; k < (int) nodosColaboradores.size(); k++) {
//                   if (nodosColaboradores[k] == coop) {
//                       repetido = 1;
//                       break;
//                   }
//
//               }
//               if(repetido == 0){
//                     nodosColaboradores.push_back(coop);
//                     cout << "Selecionado: " << coop << "\n";
//                     trace()<<"Selecionado: " << coop;
//                     j++;
//               }
//           }// comentar aqui se comentar os blocos de cima
//
//
//     }

    while (j != numCoop) {
               coop = rand() % (numhosts-1) + 1;
               repetido=0;

               //tava comentado esse bloco
                /*associado = 1;
                for (int i = 0;i < (int) nodosAssociados.size() ; i++) {
                   if(nodosAssociados[i] == coop){
                       associado = 0;
                       break;
                   }
               }
                if(associado!=0){
                    trace()<<"nao associado: "<<coop;

                }
             if(associado == 0){*/// tava comentado aq
                   for (int k = 0; k < (int) nodosColaboradores.size(); k++) {
                       if (nodosColaboradores[k] == coop) {
                           repetido = 1;
                           break;
                       }

                   }
                   if(repetido == 0){
                         nodosColaboradores.push_back(coop);
                         cout << "Selecionado: " << coop << "\n";
                         trace()<<"Selecionado: " << coop;
                         j++;
                   }
               //}// comentar aqui se comentar os blocos de cima


         }

}



//Tecnica utilizada na metodologia do Odilson
void Basic802154::selecaoOportunista(Basic802154Packet *beaconPacket){
    double snrMedio = 0.0;
    calculaNumNodosCooperantes();

    cout<< "Num Coop: "<<numeronodoscooperantes<<"\n";

    possiveisCooperantes.clear();
    nodosColaboradores.clear();
    std::map<int, Neighborhood*>::iterator iterNeighborhood;

    // Obtem a soma de todos os RSSI dos nodos que comunicaram com o coordenador
    for (iterNeighborhood = neigmap.begin();
                     iterNeighborhood != neigmap.end(); iterNeighborhood++) {
                 Neighborhood *nodo = iterNeighborhood->second;

                 snrMedio +=  nodo->rssi;
     }
    // Faz a media dos RSSI
    snrMedio = snrMedio/neigmap.size();

    //Insere na lista de possiveis cooperantes os nodos que apresentarem o RSSI maior que a media
    for (iterNeighborhood = neigmap.begin();
                         iterNeighborhood != neigmap.end(); iterNeighborhood++) {
                     Neighborhood *nodo = iterNeighborhood->second;

           if(nodo->rssi > snrMedio){
               possiveisCooperantes.push_back(*nodo);
           }
     }
    if(numeronodoscooperantes > numCoopMax){
        numeronodoscooperantes = numCoopMax;
    }
    //Verifico se existe possiveis cooperantes para suprir a demanda
     if((int) possiveisCooperantes.size() > numeronodoscooperantes){
         ordenaPossiveisCoop();
         for(int i = 0; i < numeronodoscooperantes;i++){
             nodosColaboradores.push_back(possiveisCooperantes[i].nodeId);
         }

     }else{// Se não há tantos possiveis coop quanto a demanda necessita coloco todos os que estão habilitados a serem
         if((int) possiveisCooperantes.size() > 0){
             for(int i = 0; i < (int) possiveisCooperantes.size();i++){
                  nodosColaboradores.push_back(possiveisCooperantes[i].nodeId);
             }

         }
     }


}
void Basic802154::ordenaPossiveisCoop(){
    Neighborhood aux;
   /* cout<<"possiveisCooperantes ANTES\n";
    for (int i = 0; i < (int) possiveisCooperantes.size(); i++){
        cout<<"possiveisCooperantes["<<i<<"] ID: "<< possiveisCooperantes[i].nodeId<<"\n";
        cout<<"possiveisCooperantes["<<i<<"] RSSI: "<< possiveisCooperantes[i].rssi<<"\n";
    }*/


    for (int i = 0; i < (int) possiveisCooperantes.size(); i++){
        for (int j = 0; j < (int) possiveisCooperantes.size(); j++){
            if (possiveisCooperantes[i].rssi > possiveisCooperantes[j].rssi){
                aux = possiveisCooperantes[i];
                possiveisCooperantes[i] = possiveisCooperantes[j];
                possiveisCooperantes[j] = aux;
            }
        }
    }

    /*cout<<"possiveisCooperantes Depois\n";
        for (int i = 0; i < (int) possiveisCooperantes.size(); i++){
            cout<<"possiveisCooperantes["<<i<<"] ID: "<< possiveisCooperantes[i].nodeId<<"\n";
            cout<<"possiveisCooperantes["<<i<<"] RSSI: "<< possiveisCooperantes[i].rssi<<"\n";
        }*/

}

/*#############################################################################################
                                             Codificação
#############################################################################################*/

void Basic802154::inicializeMetric(){
    relayNetworkCoding = 0;
    for(int i = 0; i< numhosts; i++){
           codificador->recoverPerNode[i] = 0;
           recoverPerNode[i] = 0;
       }
}

int Basic802154::parseToVector(){

    //codificador->mapacodificador = neigmap;
    codificador->mapacodificador.clear();
    //codificador->mapacodificador = neigmapNodosEscutados;
    codificador->mapacodificador = neigmapNodosEscutadosRefinamento;
}
void Basic802154::parseMatrix(){

    for(int i = 0;i<numhosts; i++){
        for(int j = 0;j<numhosts;j++){
            codificador->matrix[i][j] = matrix_coeficiente[i][j];
        }
    }

    for(int i = 0;i<numhosts; i++){
        for(int j = 0;j<MSG_SIZE;j++){
            codificador->combination1[i][j] = matrix_combination[i][j];
        }
    }

    if(!primeiraRetransCod){
        for(int i = 0;i<numhosts; i++){
            for(int j = 0;j<MSG_SIZE;j++){
                codificador->buffer_msg[i][j] = buffer_msg[i][j];
            }
        }
    }

}


void Basic802154::inicializaMatriz(){

    primeiraRetransCod = false;
    for(int i=0;i<numhosts;i++){
        for(int j=0;j<numhosts;j++){
            matrix_coeficiente[i][j] = 0;
        }
    }

    for(int i=0;i<numhosts;i++){
        for(int j=0;j<numhosts;j++){
            codificador->matrix[i][j] = 1;
        }
    }

    for(int i=0;i<numhosts;i++){
        for(int j=0;j<MSG_SIZE;j++){
            matrix_combination[i][j] = 0;
        }
    }

    for(int i=0;i<numhosts;i++){
        for(int j=0;j<MSG_SIZE;j++){
            codificador->combination1[i][j] = 0;
        }
    }

    for (int i = 1; i < numhosts; i++)
        codificador->received[i] = 0;

    for(int i = 0;i<numhosts; i++){
        for(int j = 0;j<MSG_SIZE;j++){
            codificador->buffer_msg[i][j] = 0;
        }
    }


    for(int i = 0;i<numhosts; i++){
        for(int j = 0;j<MSG_SIZE;j++){
            buffer_msg[i][j] = 0;
        }
    }

}
