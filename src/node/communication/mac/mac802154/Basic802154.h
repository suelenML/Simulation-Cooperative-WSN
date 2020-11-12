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

#ifndef BASIC_802154_H_
#define BASIC_802154_H_

using namespace std;
#include <bitset>
#include "AlgoritmoGenetico.h"
#include "Neighborhood.h"
#include "Neighbor.h"
#include "Codificador.h"
#include "MessagesNeighborhood.h"
extern "C" {
    #include <lpsolve/lp_lib.h>

}
#define MSG_SIZE 127
#define N_NODES 101

#define absLocal(x) ((x)<0 ? (-x) : (x))
/*
adding includes riad
*/


#include <map>
#include <vector>
#include <time.h>

#include "VirtualMac.h"
#include "Basic802154Packet_m.h"

#define ACK_PKT_SIZE 6
#define COMMAND_PKT_SIZE 10
#define GTS_SPEC_FIELD_SIZE 3
#define BASE_BEACON_PKT_SIZE 12

#define TX_TIME(x) (phyLayerOverhead + x)*1/(1000*phyDataRate/8.0)	//x are in BYTES

using namespace std;

enum MacStates {
	MAC_STATE_SETUP = 1000,
	MAC_STATE_SLEEP = 1001,
	MAC_STATE_CAP = 1002,
	MAC_STATE_GTS = 1003,
};

enum Mac802154Timers {
	PERFORM_CCA = 1,
	ATTEMPT_TX = 2,
	BEACON_TIMEOUT = 3,
	ACK_TIMEOUT = 4,
	GTS_START = 5,
	SLEEP_START = 6,
	FRAME_START = 7,
	BACK_TO_SETUP = 8,
	GTS_RETRANS = 9,
	WAKE_UP_START = 10,
	PAUSE_NODE = 11,
	RESTART_NODE = 12,
	GTS_SECOND_RETRANS = 13,
	START_GACK = 14,
    GTS_THIRD_RETRANS = 15,
};

enum macApp{
    BEACON_CHEGADA1 = 21,
};
//Suelen
struct  MENSAGENS_ESCUTADAS_REPETIDAS {
    short idNodo;
    short idMens;
    short idRetransmissor;

};
//Suelen
struct RETRANSMISSORES_REPETIDOS{
    short idNodo1;
    short idNodo2;

};

//Suelen
struct  TAXA_DE_SUCESSO{
    short id;
    double taxaDeSucesso;
};

//Estrutura para as retransmissões individuais
struct RETRANSMISSAO{
    simtime_t GTSstartRetrans;             // Absolute time of the start of GTS of retransmission period for current node
    simtime_t GTSendRetrans;               // Absolute time of the end of GTS of retransmission period for current node
    simtime_t GTSlengthRetrans;
    simtime_t phyDelaySleep2Tx;
    double offset;
};

struct RETRANS_GACK{
    vector<int> GACK;
    vector<int> idCoop;

    RETRANS_GACK():idCoop(N_NODES, 0) { }  // <- set them here by default
};

class Basic802154: public VirtualMac {
 private:
    /*--- A map from int value of state to its description (used in debug) ---*/
	map<int,string> stateDescr;

    /*--- A map for packet breakdown statistics ---*/
	map<string,int> packetBreak;

    /*--- General MAC variable ---*/
	simtime_t phyDelayForValidCS;		// delay for valid CS
	simtime_t phyDelaySleep2Tx;
	simtime_t phyDelayRx2Tx;
	int phyLayerOverhead;
	int phyBitsPerSymbol;
	double phyDataRate;

	map<int,bool> associatedDevices;	// map of assoicated devices (for PAN coordinator)

    /*--- 802154Mac state variables  ---*/
	int macState;		// current MAC state
	int CAPlength;		// duration of CAP interval (in number of superframe slots)
	int macBSN;			// beacon sequence number (unused)
	int lostBeacons;	// number of consequitive lost beacon packets
	int frameInterval;	// duration of active part of the frame (in symbols)
	int beaconInterval;	// duration of the whole frame (in symbols)
	int seqNum;			// sequence number for data packets

	int NB, CW, BE;		// CSMA-CA algorithm parameters (Number of Backoffs, 
						// Contention Window length and Backoff Exponent)

	double currentPacketLimit;	// maximum delay limit for current packet
	int currentPacketRetries;	// number of retries left for next packet to be sent
	bool currentPacketGtsOnly;	// transmission of current packet can be done in GTS only
	bool currentPacketSuccess; 	// outcome of transmission for current packet
	string currentPacketHistory;// history of transmission attempts for current packet

	simtime_t currentPacketResponse;// Duration of timeout for receiving a reply after sending a packet
	simtime_t ackWaitDuration;		// Duration of timeout for receiving an ACK
	simtime_t symbolLen;			// Duration of transmittion of a single symbol
	simtime_t guardTime;

	// packet statistics
	int sentBeacons;
	int recvBeacons;
	int packetoverflow;
	
	// syncronisation statistics
	simtime_t desyncTime;
	simtime_t desyncTimeStart;

    /*--- 802154Mac packet pointers (sometimes packet is created not immediately before sending) ---*/
	Basic802154Packet *beaconPacket;
	Basic802154Packet *currentPacket;

	void fromNetworkLayer(cPacket *, int);
	void fromRadioLayer(cPacket *, double, double);

	void readIniFileParameters(void);
	void setMacState(int newState);
	void handleAckPacket(Basic802154Packet *);
	void performCSMACA();
	void attemptTransmission(const char *);
	void transmitCurrentPacket();
	void collectPacketHistory(const char * s);
	void clearCurrentPacket(const char * s = NULL, bool success = false);

     /*--- The .ned file's parameters ---*/
	bool printStateTransitions;
	bool isPANCoordinator;
	bool isFFD;
	bool batteryLifeExtention;
	bool enableSlottedCSMA;

	int macMinBE;
	int macMaxBE;
	int macMaxCSMABackoffs;
	int macMaxFrameRetries;
	int maxLostBeacons;
	int minCAPLength;
	int unitBackoffPeriod;
	int baseSlotDuration;
	int numSuperframeSlots;
	int baseSuperframeDuration;
	int beaconOrder;
	int frameOrder;

	simtime_t CAPend;				// Absolute time of end of CAP period for current frame
	simtime_t currentFrameStart;	// Absolute recorded start time of the current frame
	simtime_t GTSstart;				// Absolute time of the start of GTS period for current node
	simtime_t GTSend;				// Absolute time of the end of GTS period for current node
	simtime_t GTSlength;			// length of the GTS period for current node

	int associatedPAN;	// ID of current PAN (-1 if not associated)
    
    //Número máximo de nodes a serem associados ao PAN
    int maxChild;
    int nchildren; //guarda o número de nodes associados


    /* ------------SUELEN----------------*/
    //Conta o número de beacons perdidos
    int beaconsPerdidos;
    simtime_t GTSstartRetrans;             // Absolute time of the start of GTS of retransmission period for current node
    simtime_t GTSendRetrans;               // Absolute time of the end of GTS of retransmission period for current node
    simtime_t GTSlengthRetrans;            // length of the GTS of retransmission period for current node
    short inicioGTSRetrans;
    simtime_t primeiraRetrans;
    simtime_t irDormir;
    bool firstCoop;
    bool thirdCoop;
    double delayRadio;
    simtime_t second_GTSstartRetrans;
    simtime_t second_GTSendRetrans;
    simtime_t second_GTSlengthRetrans;
    simtime_t start_gack; // sinaliza o tempo para o coordenador enviar o GACK;
    simtime_t third_GTSstartRetrans;
    simtime_t third_GTSendRetrans;
    simtime_t third_GTSlengthRetrans;






  protected:

	/*--- Interface functions of the 802.15.4 MAC ---*/
	Basic802154Packet *newConnectionRequest(int);
	Basic802154Packet *newGtsRequest(int,int);
	Basic802154Packet *getCurrentPacket() { return currentPacket; }
	void transmitPacket(Basic802154Packet *pkt, int retries = 0, bool GTSonly = false, double delay = 0);
	int getCurrentMacState() { return macState; }
	int getAssociatedPAN() { return associatedPAN; }
	Basic802154Packet *pausarNodo(int); //Suelen
	Basic802154Packet *restartNodo(int); // Suelen

	/*--- Virtual interface functions can be overwritten by a decision module ---*/
	virtual void startup();
	virtual void timerFiredCallback(int);
	virtual void finishSpecific();
	
	/*--- HUB-specific desicions ---*/
	virtual void dataReceived_hub(Basic802154Packet *) {}
	virtual int gtsRequest_hub(Basic802154Packet *) { return 0; }			//default: always reject
    virtual int associationRequest_hub(Basic802154Packet *);	//default: always accept
	virtual void prepareBeacon_hub(Basic802154Packet *) {}

	/*----Suelen----*/
	virtual void remover(Basic802154Packet *, int) {}

	/*--- Node-specific desicions ---*/
	virtual void connectedToPAN_node() {}
	virtual void disconnectedFromPAN_node() {}
	virtual void assignedGTS_node(int) {}
	virtual void receiveBeacon_node(Basic802154Packet *);
	virtual void startedGTS_node() {}

	/*--- General desicions ---*/
	virtual bool acceptNewPacket(Basic802154Packet *);
	virtual void transmissionOutcome(Basic802154Packet *, bool, string);


	/*----riad----*/
	    double MAX_RSSI = -87.0; //-100;
	    double EstLoss; // usei
	    double DevLoss; // usei
	    double SamLoss;
	    int numhosts; // usei

	    int numeronodoscooperantes; // usei
	    int numeronodoscooperantespassado;
	    int numdadosrecebidosnogtstransmissao; // Para deteminar o numero de cooperantes (usei)


	    cOutVector vectormsgsuccs;
	    cOutVector vectortaxaperda; // usei

	    int sucessomsgrecebida;
	    double limiaralpha; //usei
	    double limiarbetha; // usei
	    double alpha; // usei
	    double betha; // usei
	    double ganho; // usei
	    int contadordivtemp;
	    int histnumnodoscoop;

	    double beta1;
	    double beta2;
	    double beta3;
	    double beta4;

	    //dados que eu preciso
	    double somaDeSinais = 0.0;
	    int totalDevizinhos = 0;
	    double meuRSSI = 0.0;

	    int numeroDoProblema=0;

	    bool cooperador=false;


	    int numeroDeCooperacoes=0;
	    int tempoDeBeacon=0;
	    int selecao=0;
	    short idBeacon; // guarda o id do beacon corrente
	    //short atualizarVizinhanca;// sinaliza o momento em que os nodos irao atualizar a vizinhanca
	    bool atualizarVizinhanca;


	    //void contabilizarMensagens();
	    void listarNodosEscutados(Basic802154Packet *rcvPacket, double rssi);
	    void souNodoCooperante(Basic802154Packet * pkt);
	    void preencherDados(Basic802154Packet *macPacket);
	    void calculaNumNodosCooperantes();
	    void selecionaNodosSmart(Basic802154Packet *beaconPacket);
        void selecionaNodosSmartNumVizinhos(Basic802154Packet *beaconPacket);
	    void AtualizarVizinhaca(Basic802154Packet * pkt, double rssi);
	    void adicionarNodoSolto(int pai, int filho);
	    void retransmitir(Basic802154Packet *nextPacket);
	    void verificarRetransmissao(Basic802154Packet *rcvPacket, double rssi);
	    void enviarNodosCooperantes(Basic802154Packet *beaconPacket);
	    void verificaRetransmissoesRepetidas();//Suelen
	    void armazenaRetransmissoes(Basic802154Packet *rcvPacket);//Suelen
	    void limparBufferAplicacao();//Suelen
	    double taxaDeSucesso(int id, int recebidas); //Suelen
	    void selecaoCoopAleatoria(Basic802154Packet *beaconPacket);//Suelen
	    void contabilizarMsgRetransmissores(); //Suelen
	    void contabilizarRetransmissoes(); //Suelen
	    void tratarDivisao(Neighborhood *nodo);//Suelen
	    void selecaoOportunista(Basic802154Packet *beaconPacket);//Suelen
	    void selecaoCompletamenteAleatoria(Basic802154Packet *beaconPacket);//Suelen
	    void ordenaPossiveisCoop();//Suelen
	    void vizinhanca(); //Suelen
	    void limparMatrizAdjacencia();//Suelen
	    void heuristicGreedy(); // Suelen
	    void algGenetic(); //Suelen
	    /*Codificacao*/
	    int parseToVector();
	    void parseMatrix();
	    void inicializaMatriz();
	    void listarNodosEscutadosRetransmissaoNetworkCoding(Basic802154Packet *rcvPacket);
	    void listarNodosEscutadosTransmissaoNetworkCoding(Basic802154Packet *rcvPacket);
	    void retransmissaoNetworkCoding(Basic802154Packet *packetRetrans);
	    void inicializeMetric();
	    void selectMsgNetworkCoding();
	    void retransmissionGTS();
	    void preencherListaGack(Basic802154Packet *beaconPacket);
	    void msgNotReceived();
	    void matrizVizinhancaNodos(Basic802154Packet * pkt);
	    void preSelectMsgNetworkCoding();
	    void selectCoopAux();
	    void enviarCooperantesAuxiliares(Basic802154Packet *beaconPacket);
	    void souNodoCooperanteAuxiliar(Basic802154Packet * pkt);
	    void selectMsgCoopAux();
	    void selectMsgNetworkCodingAux();

	    /*Metodos para a retransmissão sem codificação*/
	    void  definirNumeroSlotsCooperantes(Basic802154Packet *beaconPacket);
	    void definirnumSlotsDisponiveis(Basic802154Packet *beaconPacket);
	    void retransmissaoIndividual(Basic802154Packet *packetRetrans);
	    void montarVetorDeBit(int addr);
	    void setMsgPerCoop(Basic802154Packet *beaconPacket);
	    int pop_cnt_64(uint64_t i);
	    void inserirNumeroSlotsBeacon(Basic802154Packet *beaconPacket);



	    int retransmissoesEfetivas=0; // que o coordenador escutou apenas por retransmissão
	    int retransmissoesNaoEfetivas=0; // que o coordenador escutou na transmissão direta e que foi por retransmissão também
	    vector<int> nodosColaboradores;//aqui serão armazenados os ids dos nodos colaboradores.
	    //std::vector<int> nodosEscutados;
	    std::vector<MENSAGENS_ESCUTADAS> nodosEscutados;
	    std::map<int,bool> cooperacoesDoBeacon; // guarda as msg's que chegaram por cooperaçao
	    //std::map<int, vector<int>*> historicoDeCooperacao; // pelo que entendi salva o id do nodo e um vetor com os nodos que ele escutou
	    std::map<int, vector<MENSAGENS_ESCUTADAS>*> historicoDeCooperacao; // pelo que entendi salva o id do nodo e um vetor com os nodos que ele escutou
	    std::map<int, vector<int>*> listaDeNodosSoltos;//são nodos que não conseguiram trocar ms com o coordenador
	    std::map<int, Neighborhood*> neigmap; // são os nodos que cada nodo possiu como vizinho
	    vector<int> pacotesEscutadosT;
	    vector<int> vizinhosAntigos;

	    /*----------------SUELEN------------------*/
	    bool redeDinamica;
	    double limiteRSSI;
	    bool userelay;
	    bool smart;
	    bool smartPeriodic;
	    bool aleatoria;
	    bool oportunista;
	    bool completamenteAleatoria;
	    int utilidadeCoop;
	    int primeiraSelecao;
	    int msgEnviadas;
	    int msgRecebidas;
	    int msgRtrans;
	    int beaconsRecebidos;
	    int mensagensRecuperadas;
	    int mensagensPerdidas;
	    std::vector<MENSAGENS_ESCUTADAS_REPETIDAS> retransmissoesRepetidas;
	    //std::vector<RETRANSMISSORES_REPETIDOS> retransmissoresDuplicados;
	    std::map<int, vector<MENSAGENS_ESCUTADAS>*> historicoDeSucesso; // armazena as mensagens de cooperação que o coordenador não havia escutado diretamente
	    int qntidadeVezesCooperou;
	    int numCoopMax;
	    bool pausar;// para silenciar o nodo
	    double pausarNoTempo;// para silenciar o nodo
	    double retornarNoTempo;
	    bool pausado; // para silenciar nodo
	    bool eventoPauseATivado;
	    int tempConfig; //numero de BI para configuracao da Rede
	    bool tempAtualizVizinhanca;
	    bool processoSelecao; // sinaliza que neste momento já esta em processo de selecao
	    bool coopPause; // sinaliza que um coop saiu da area do coordenador
	    int numSelRealizadas; // numero de seleções que foram realizadas
        //int tempAtualizVizinhanca;
        std::map<int, vector<MENSAGENS_ESCUTADAS>*> historicoDeSucessoBeacon; // armazena as mensagens de cooperação que o coordenador não havia escutado diretamente a cada beacon
        int limitBISelecao; // limita o intervalo de seleção, para não crescer muito

        /*Variáveis taxa de sucesso Metodo Smart*/
        int qntdMsgRecebCoop; // quantidade de mensagens que o coordenador recebeu por meio de cooperações
        float qntdMsgEscutCood; // quantidade de mensagens que o coordenador recebeu diretamente
        float txSucessSmart; // armzana o resultado da tx de sucesso das msg's recebidas no coordenador (diretas e com cooperação)
        float txSucessSmartAnterior; // armzana o resultado da tx de sucesso das msg's recebidas no coordenador (diretas e com cooperação) no BI anterior
        std::vector<int> msgRecuperadasBeacon;  // armazena a quantidade de msg que chegaram por cooperação de cada nodo origem no intervalo de um beacon
        int nchildrenActual;
        int nchildrenAntigos;//nº de nodos associados antes o envio de um novo beacon;
        bool pausaEnviada; // variavel de controle para saber se o nodo solicitou a a pausa e não recebeu resposta (ACK)

	    /*Variáveis taxa de sucesso Metodo Odilson*/
	    double alphaSucess;
	    std::vector<TAXA_DE_SUCESSO> historicoTaxaDeSucesso;
	    std::vector<int> nodosAssociados;
	    std::vector<int> msgRecuperadas; // armazena a quantidade de msg que chegaram por cooperação de cada nodo origem

	    /*Metodo Oportunista*/
	    std::vector<Neighborhood> possiveisCooperantes;


	    /*Variaveis auxiliares para a heuristica*/
	    std::vector<int> finiteSet; // conjunto de todos os nodos associados na rede
	    std::vector<int> solutionSet; // conjunto resultante do algoritmo (cooperantes escolhidos)
	    std::vector<int> auxiliarSet; // auxilia na resolução do algoritmo guloso
	    int **matrizAdj; // matriz de adjacencia
	    std::map<int, vector<int>> vizinhosAdj; //matriz de adjacencia
	    clock_t tempExecInicio;
	    clock_t tempExecFim;
	    double tempSelecao;
	    simtime_t inicioExec; // variavel de tempo (omnet) de inicio do algoritmo de selecao de coop
	    simtime_t fimExec;  // variavel de tempo (omnet) de finalizacao do algoritmo de selecao de coop
	    simtime_t tempTotalSelec;
	    simtime_t inicioSim;
	    std::vector<double> times;

	    /*Codificação */
	    bool useNetworkCoding;
	    bool useCoopAux;
	    bool useGACK;
	    bool useThreeRetransPerCoop;
	    Codificador *codificador;
	    int sucessoMsgCodRecebida;
	    int num_msg_decod_sucess;
	    int sucessoMsgDirRecebida;
	    int retransCoded;
	    /*unsigned char*/ uint8_t buffer_msg[N_NODES][MSG_SIZE];//matriz de armazenamento de msg
	    /*unsigned short*/ uint8_t matrix_coeficiente[N_NODES][N_NODES]; // numero de nodos X numero de vizinhos
	    /*unsigned short*/ uint8_t matrix_combination[N_NODES][MSG_SIZE];
	    // neigmapNodosEscutados representa as mensagens que o nodo escutou dos vizinhos
	    bool primeiraRetransCod;
	    std::map<int, MessagesNeighborhood*> neigmapNodosEscutados; // to pensando em essa estrutura substituir o vetor de nodosEscutados
	    int recoverPerNode[N_NODES]; /*Número de mensagens recuperadas por nodo*/
	    int relayNetworkCoding;/*Número de mensagens retransmitidas por relay (using network coding)*/
	    std::map<int, MessagesNeighborhood*> neigmapNodosEscutadosRefinamento; // Essa estrutura armazena só três msg que serão codificadas
	    std::vector<int> vizinhosCoop; //cada cooperante armazenará a lista de coop para não retransmitir msg de outro relay.
        int numeroDeCoop;
        std::vector<int> GACK; //vetor que o coordenador enviará antes da coop, avisando quais msg ele precisa
        std::vector<int> nodesNotReceived; // vetor para cada nodo saber as msg que o coord perdeu
        int numberRecover;
        std::map<int, MessagesNeighborhood*> nodesCanRecover; //mensagem que o cooperante pode recuperar dentre as que o coord não escutou
        std::map<int, MessagesNeighborhood*> neigmapNodosEnviados;
        std::map<int, vector<int>> matrizVizinhos;// mantem uma matriz de adjacencia da rede
        std::map<int, MessagesNeighborhood*> otherRecover; // guarda as mensagem que possui, mas que outros cooperantes também possuem
        std::map<int, MessagesNeighborhood*> neigmapNodosEscutadosSelecionados; // mensagens que foram selecionadas para a codificação
        bool secondRetrans; // sinaliza se é a segunda retransmissao
        int var_msg_perNode;
        int var_msg_otherNodes;
        int var_msg_notRecover;
        int numCoopSel;// é so para saber o numero total de cooperantes selecionados
        int numMSGSolicitadas; // é so para saber qntas mensagens o coordenador precisa recuperar
        vector<int> nodosColaboradoresAuxiliares; // Coordenadore guarda quem são os coop auxiliares
        bool cooperanteAuxiliar; // sinaliza que o nodo é um coop auxiliar
        bool cooperanteAuxiliarAuxiliar;
        vector<int> coopAuxPerNode; // cada nodo cooperante guarda quem saõ seus auxiliares
        vector<int> coopHelped; // cada cooperante auxiliar guarda qual cooperante principal ele ajudará
        std::map<int, MessagesNeighborhood*> nodesCanRecoverInCommon; // mensagens que o cooperante e os auxiliares conseguem recuperar em comum


        /*Variaveis para a retransmissao sem coodificação*/
        vector<int> numSlotsPerCoop;
        bool useRetransIndependent;
        vector<RETRANSMISSAO> timersRetrans;
        RETRANSMISSAO retransAtual;
        MessagesNeighborhood *msgRetrans;
        bool primeiraRetransIndepend;
        uint64_t vetBit[2];
        uint64_t bitmapNeigh[N_NODES][2];
        vector<int> GACKRetrans;
        RETRANS_GACK informRetrans;






};

#endif	//BASIC_802154
