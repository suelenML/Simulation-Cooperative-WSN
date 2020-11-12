//****************************************************************************
//*  Copyright: National ICT Australia,  2007 - 2012                         *
//*  Developed at the ATP lab, Networked Systems research theme              *
//*  Author(s): Athanassios Boulis, Yuriy Tselishchev                        *
//*  This file is distributed under the terms in the attached LICENSE file.  *
//*  If you do not find this file, copies can be found by writing to:        *
//*                                                                          *
//*      NICTA, Locked Bag 9013, Alexandria, NSW 1435, Australia             *
//*      Attention:  License Inquiry.                                        *
//*                                                                          *  
//****************************************************************************/

package node.communication.mac.mac802154;

simple Basic802154 like node.communication.mac.iMac {
 parameters:
    //debug parameters
	bool collectTraceInfo = default (false);
	bool printStateTransitions = default (false);

	//mac layer parameters
	int macMaxPacketSize = default (142);	//128 (Maximum payload for 802.15.4) + 14 (Data packet overhead)
	int macPacketOverhead = default (14);
	int macBufferSize = default (32);

	bool enableSlottedCSMA = default (true);
	bool isFFD = default (false);
	bool isPANCoordinator = default (false);
	bool batteryLifeExtention = default (false);

	int frameOrder = default (4);
	int beaconOrder = default (6);
	int unitBackoffPeriod = default (20);
	int baseSlotDuration = default (60);

	int numSuperframeSlots = default (16);
	int macMinBE = default (5);
	int macMaxBE = default (7);
	int macMaxCSMABackoffs = default (4);
	int macMaxFrameRetries = default (2);
	int maxLostBeacons = default (4);
	int minCAPLength = default (440);

	// parameters dependent on physical layer
	// some are essential and are not defined as default
	double phyDelayForValidCS = default (0.128);
	double phyDataRate;
	double phyDelaySleep2Tx = default (0.2); //in ms
	double phyDelayRx2Tx = default (0.02);	//in ms
	int phyFrameOverhead = default (6);
	int phyBitsPerSymbol; 

	//reception guard time, in ms
	double guardTime = default (1);

    //Número máximo de filhos a serem associados
    int maxChild = default (255); // maximo permitido por default
	
	//Suelen 
	bool userelay = default(false);
	bool smart = default(false);
	bool smartPeriodic = default(false);
	bool aleatoria = default(false);
	bool oportunista = default(false);
	bool completamenteAleatoria = default(false);
	int selecao = default(10);
	int tempConfig = default(5);
	int tempAtualizVizinhanca = default(3);
	bool redeDinamica = default(false);
	
	// Para determinar o numero de cooperantes
	double limiaralpha = default(0.2);
    double limiabetha = default(0.2);
    double betha = default (0.4);
    double alpha = default (0.4);
    double ganho = default (2.0);
    
    //Para determinar a taxa de sucesso
    double alphaSucess = default(0.3);
    
    //Para determinar quais nodos serao coop
    double beta1 = default(-1.0);
	double beta2 = default(-1.5);
	double beta3 = default(-2.0);
	double beta4 = default(-1.0);
	
	
	//Codificação
	bool useNetworkCoding = default(false);
	bool useCoopAux = default(false); // se a rede terá cooperantes auxiliares para a codificação
    bool useGACK = default(false); // o coordenador enviará um GACK após receber as transmissões diretas
    bool useThreeRetransPerCoop = default(false); // cada cooperante enviará três msg de retransmissão codificada
    bool useRetransIndependent = default(false); // Cada cooperante enviará uma msg de retransmissão por vez
	
	
	
	
	
 gates:
	output toNetworkModule;
	output toRadioModule;
	input fromNetworkModule;
	input fromRadioModule;
	input fromCommModuleResourceMgr;
}

