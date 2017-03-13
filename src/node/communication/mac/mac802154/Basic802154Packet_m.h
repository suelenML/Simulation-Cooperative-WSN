//
// Generated file, do not edit! Created by nedtool 4.6 from src/node/communication/mac/mac802154/Basic802154Packet.msg.
//

#ifndef _BASIC802154PACKET_M_H_
#define _BASIC802154PACKET_M_H_

#include <omnetpp.h>

// nedtool version check
#define MSGC_VERSION 0x0406
#if (MSGC_VERSION!=OMNETPP_VERSION)
#    error Version mismatch! Probably this file was generated by an earlier version of nedtool: 'make clean' should help.
#endif



// cplusplus {{
#include "MacPacket_m.h"
// }}

/**
 * Enum generated from <tt>src/node/communication/mac/mac802154/Basic802154Packet.msg:19</tt> by nedtool.
 * <pre>
 * enum Mac802154Packet_type
 * {
 * 
 *     MAC_802154_BEACON_PACKET = 1001;
 *     MAC_802154_ASSOCIATE_PACKET = 1002;
 *     MAC_802154_DATA_PACKET = 1003;
 *     MAC_802154_ACK_PACKET = 1004;
 *     MAC_802154_GTS_REQUEST_PACKET = 1005;
 * }
 * </pre>
 */
enum Mac802154Packet_type {
    MAC_802154_BEACON_PACKET = 1001,
    MAC_802154_ASSOCIATE_PACKET = 1002,
    MAC_802154_DATA_PACKET = 1003,
    MAC_802154_ACK_PACKET = 1004,
    MAC_802154_GTS_REQUEST_PACKET = 1005
};

/**
 * Struct generated from src/node/communication/mac/mac802154/Basic802154Packet.msg:27 by nedtool.
 */
struct Basic802154GTSspec
{
    Basic802154GTSspec();
    int owner;
    int start;
    int length;
    bool retransmissor;
};

void doPacking(cCommBuffer *b, Basic802154GTSspec& a);
void doUnpacking(cCommBuffer *b, Basic802154GTSspec& a);

/**
 * Struct generated from src/node/communication/mac/mac802154/Basic802154Packet.msg:33 by nedtool.
 */
struct MENSAGENS_ESCUTADAS
{
    MENSAGENS_ESCUTADAS();
    short idMens;
    short idNodo;
};

void doPacking(cCommBuffer *b, MENSAGENS_ESCUTADAS& a);
void doUnpacking(cCommBuffer *b, MENSAGENS_ESCUTADAS& a);

/**
 * Class generated from <tt>src/node/communication/mac/mac802154/Basic802154Packet.msg:39</tt> by nedtool.
 * <pre>
 * packet Basic802154Packet extends MacPacket
 * {
 *     int Mac802154PacketType @enum(Mac802154Packet_type);
 *     int PANid;
 *     int srcID;
 *     int dstID;
 *     int seqNum;
 * 
 *     // those fields belong to beacon packet (MAC_802154_BEACON_PACKET)
 *     int beaconOrder;
 *     int frameOrder;
 *     int BSN;
 *     int CAPlength;
 *     int GTSlength;
 *     Basic802154GTSspec GTSlist[];
 *     //modificações Ríad
 *     short vizinhosOuNodosCooperantes[];//quando enviado pelo coordenador será a lista de colaboradores selecionado e quando enviado pelos outros nodos será a lista de vizinhos
 *     double somaSinais;
 *     double energy;
 *     //int dadosVizinho[];//identificação dos vizinhos, dados retransmitidos
 *     MENSAGENS_ESCUTADAS dadosVizinho[];
 *     short slotInicioRetrans;// posição do GTS em que inicia a retransmisso
 *     bool retransmissao = false;
 *     //short numMsgEnviadas;
 *     short tempoBeacon;
 * }
 * </pre>
 */
class Basic802154Packet : public ::MacPacket
{
  protected:
    int Mac802154PacketType_var;
    int PANid_var;
    int srcID_var;
    int dstID_var;
    int seqNum_var;
    int beaconOrder_var;
    int frameOrder_var;
    int BSN_var;
    int CAPlength_var;
    int GTSlength_var;
    Basic802154GTSspec *GTSlist_var; // array ptr
    unsigned int GTSlist_arraysize;
    short *vizinhosOuNodosCooperantes_var; // array ptr
    unsigned int vizinhosOuNodosCooperantes_arraysize;
    double somaSinais_var;
    double energy_var;
    MENSAGENS_ESCUTADAS *dadosVizinho_var; // array ptr
    unsigned int dadosVizinho_arraysize;
    short slotInicioRetrans_var;
    bool retransmissao_var;
    short tempoBeacon_var;

  private:
    void copy(const Basic802154Packet& other);

  protected:
    // protected and unimplemented operator==(), to prevent accidental usage
    bool operator==(const Basic802154Packet&);

  public:
    Basic802154Packet(const char *name=NULL, int kind=0);
    Basic802154Packet(const Basic802154Packet& other);
    virtual ~Basic802154Packet();
    Basic802154Packet& operator=(const Basic802154Packet& other);
    virtual Basic802154Packet *dup() const {return new Basic802154Packet(*this);}
    virtual void parsimPack(cCommBuffer *b);
    virtual void parsimUnpack(cCommBuffer *b);

    // field getter/setter methods
    virtual int getMac802154PacketType() const;
    virtual void setMac802154PacketType(int Mac802154PacketType);
    virtual int getPANid() const;
    virtual void setPANid(int PANid);
    virtual int getSrcID() const;
    virtual void setSrcID(int srcID);
    virtual int getDstID() const;
    virtual void setDstID(int dstID);
    virtual int getSeqNum() const;
    virtual void setSeqNum(int seqNum);
    virtual int getBeaconOrder() const;
    virtual void setBeaconOrder(int beaconOrder);
    virtual int getFrameOrder() const;
    virtual void setFrameOrder(int frameOrder);
    virtual int getBSN() const;
    virtual void setBSN(int BSN);
    virtual int getCAPlength() const;
    virtual void setCAPlength(int CAPlength);
    virtual int getGTSlength() const;
    virtual void setGTSlength(int GTSlength);
    virtual void setGTSlistArraySize(unsigned int size);
    virtual unsigned int getGTSlistArraySize() const;
    virtual Basic802154GTSspec& getGTSlist(unsigned int k);
    virtual const Basic802154GTSspec& getGTSlist(unsigned int k) const {return const_cast<Basic802154Packet*>(this)->getGTSlist(k);}
    virtual void setGTSlist(unsigned int k, const Basic802154GTSspec& GTSlist);
    virtual void setVizinhosOuNodosCooperantesArraySize(unsigned int size);
    virtual unsigned int getVizinhosOuNodosCooperantesArraySize() const;
    virtual short getVizinhosOuNodosCooperantes(unsigned int k) const;
    virtual void setVizinhosOuNodosCooperantes(unsigned int k, short vizinhosOuNodosCooperantes);
    virtual double getSomaSinais() const;
    virtual void setSomaSinais(double somaSinais);
    virtual double getEnergy() const;
    virtual void setEnergy(double energy);
    virtual void setDadosVizinhoArraySize(unsigned int size);
    virtual unsigned int getDadosVizinhoArraySize() const;
    virtual MENSAGENS_ESCUTADAS& getDadosVizinho(unsigned int k);
    virtual const MENSAGENS_ESCUTADAS& getDadosVizinho(unsigned int k) const {return const_cast<Basic802154Packet*>(this)->getDadosVizinho(k);}
    virtual void setDadosVizinho(unsigned int k, const MENSAGENS_ESCUTADAS& dadosVizinho);
    virtual short getSlotInicioRetrans() const;
    virtual void setSlotInicioRetrans(short slotInicioRetrans);
    virtual bool getRetransmissao() const;
    virtual void setRetransmissao(bool retransmissao);
    virtual short getTempoBeacon() const;
    virtual void setTempoBeacon(short tempoBeacon);
};

inline void doPacking(cCommBuffer *b, Basic802154Packet& obj) {obj.parsimPack(b);}
inline void doUnpacking(cCommBuffer *b, Basic802154Packet& obj) {obj.parsimUnpack(b);}


#endif // ifndef _BASIC802154PACKET_M_H_

