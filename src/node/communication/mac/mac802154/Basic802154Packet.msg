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

cplusplus {{
#include "MacPacket_m.h"
}}

class MacPacket;

enum Mac802154Packet_type {
	MAC_802154_BEACON_PACKET = 1001;
	MAC_802154_ASSOCIATE_PACKET = 1002;
	MAC_802154_DATA_PACKET = 1003;
	MAC_802154_ACK_PACKET = 1004;
	MAC_802154_GTS_REQUEST_PACKET = 1005;
	MAC_802154_PAUSE_PACKET = 1006;
	MAC_802154_RESTART_PACKET = 1007;
	MAC_802154_GACK = 1008;

}

struct Basic802154GTSspec {
	int owner;
	int start;
	int length;
	bool retransmissor;
}
struct MENSAGENS_ESCUTADAS {
	short idMens;
	short idNodo;
	
}

packet Basic802154Packet extends MacPacket {
	int Mac802154PacketType enum (Mac802154Packet_type);
	int PANid;
	int srcID;
	int dstID;
	int seqNum;
	bool pauseNodo; // Variavel para sinalizar pause

	// those fields belong to beacon packet (MAC_802154_BEACON_PACKET)
	int beaconOrder;
	int frameOrder;
	int BSN;
	int CAPlength;
	int GTSlength;
	Basic802154GTSspec GTSlist[];
	//modificações Ríad
	short vizinhosOuNodosCooperantes[];//quando enviado pelo coordenador será a lista de colaboradores selecionado e quando enviado pelos outros nodos será a lista de vizinhos
	double somaSinais;
	double energy;
	//int dadosVizinho[];//identificação dos vizinhos, dados retransmitidos
	MENSAGENS_ESCUTADAS dadosVizinho[];
	short slotInicioRetrans;// posição do GTS em que inicia a retransmisso
	bool retransmissao = false; 
	//short numMsgEnviadas;
	short tempoBeacon;
	bool tempAtualizVizinhanca;
	short primeiraSelecao;
	short idBeacon;
	
	//codificacao
	unsigned short payload[];
	unsigned short coeficiente[];
	unsigned short gack[];
	//int coeficiente[];
	short coopAuxiliares[];
	
}

