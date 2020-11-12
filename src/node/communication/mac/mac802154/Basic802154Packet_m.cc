//
// Generated file, do not edit! Created by nedtool 4.6 from src/node/communication/mac/mac802154/Basic802154Packet.msg.
//

// Disable warnings about unused variables, empty switch stmts, etc:
#ifdef _MSC_VER
#  pragma warning(disable:4101)
#  pragma warning(disable:4065)
#endif

#include <iostream>
#include <sstream>
#include "Basic802154Packet_m.h"

USING_NAMESPACE


// Another default rule (prevents compiler from choosing base class' doPacking())
template<typename T>
void doPacking(cCommBuffer *, T& t) {
    throw cRuntimeError("Parsim error: no doPacking() function for type %s or its base class (check .msg and _m.cc/h files!)",opp_typename(typeid(t)));
}

template<typename T>
void doUnpacking(cCommBuffer *, T& t) {
    throw cRuntimeError("Parsim error: no doUnpacking() function for type %s or its base class (check .msg and _m.cc/h files!)",opp_typename(typeid(t)));
}




// Template rule for outputting std::vector<T> types
template<typename T, typename A>
inline std::ostream& operator<<(std::ostream& out, const std::vector<T,A>& vec)
{
    out.put('{');
    for(typename std::vector<T,A>::const_iterator it = vec.begin(); it != vec.end(); ++it)
    {
        if (it != vec.begin()) {
            out.put(','); out.put(' ');
        }
        out << *it;
    }
    out.put('}');
    
    char buf[32];
    sprintf(buf, " (size=%u)", (unsigned int)vec.size());
    out.write(buf, strlen(buf));
    return out;
}

// Template rule which fires if a struct or class doesn't have operator<<
template<typename T>
inline std::ostream& operator<<(std::ostream& out,const T&) {return out;}

EXECUTE_ON_STARTUP(
    cEnum *e = cEnum::find("Mac802154Packet_type");
    if (!e) enums.getInstance()->add(e = new cEnum("Mac802154Packet_type"));
    e->insert(MAC_802154_BEACON_PACKET, "MAC_802154_BEACON_PACKET");
    e->insert(MAC_802154_ASSOCIATE_PACKET, "MAC_802154_ASSOCIATE_PACKET");
    e->insert(MAC_802154_DATA_PACKET, "MAC_802154_DATA_PACKET");
    e->insert(MAC_802154_ACK_PACKET, "MAC_802154_ACK_PACKET");
    e->insert(MAC_802154_GTS_REQUEST_PACKET, "MAC_802154_GTS_REQUEST_PACKET");
    e->insert(MAC_802154_PAUSE_PACKET, "MAC_802154_PAUSE_PACKET");
    e->insert(MAC_802154_RESTART_PACKET, "MAC_802154_RESTART_PACKET");
    e->insert(MAC_802154_GACK, "MAC_802154_GACK");
);

Basic802154GTSspec::Basic802154GTSspec()
{
    owner = 0;
    start = 0;
    length = 0;
    retransmissor = 0;
}

void doPacking(cCommBuffer *b, Basic802154GTSspec& a)
{
    doPacking(b,a.owner);
    doPacking(b,a.start);
    doPacking(b,a.length);
    doPacking(b,a.retransmissor);
}

void doUnpacking(cCommBuffer *b, Basic802154GTSspec& a)
{
    doUnpacking(b,a.owner);
    doUnpacking(b,a.start);
    doUnpacking(b,a.length);
    doUnpacking(b,a.retransmissor);
}

class Basic802154GTSspecDescriptor : public cClassDescriptor
{
  public:
    Basic802154GTSspecDescriptor();
    virtual ~Basic802154GTSspecDescriptor();

    virtual bool doesSupport(cObject *obj) const;
    virtual const char *getProperty(const char *propertyname) const;
    virtual int getFieldCount(void *object) const;
    virtual const char *getFieldName(void *object, int field) const;
    virtual int findField(void *object, const char *fieldName) const;
    virtual unsigned int getFieldTypeFlags(void *object, int field) const;
    virtual const char *getFieldTypeString(void *object, int field) const;
    virtual const char *getFieldProperty(void *object, int field, const char *propertyname) const;
    virtual int getArraySize(void *object, int field) const;

    virtual std::string getFieldAsString(void *object, int field, int i) const;
    virtual bool setFieldAsString(void *object, int field, int i, const char *value) const;

    virtual const char *getFieldStructName(void *object, int field) const;
    virtual void *getFieldStructPointer(void *object, int field, int i) const;
};

Register_ClassDescriptor(Basic802154GTSspecDescriptor);

Basic802154GTSspecDescriptor::Basic802154GTSspecDescriptor() : cClassDescriptor("Basic802154GTSspec", "")
{
}

Basic802154GTSspecDescriptor::~Basic802154GTSspecDescriptor()
{
}

bool Basic802154GTSspecDescriptor::doesSupport(cObject *obj) const
{
    return dynamic_cast<Basic802154GTSspec *>(obj)!=NULL;
}

const char *Basic802154GTSspecDescriptor::getProperty(const char *propertyname) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    return basedesc ? basedesc->getProperty(propertyname) : NULL;
}

int Basic802154GTSspecDescriptor::getFieldCount(void *object) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    return basedesc ? 4+basedesc->getFieldCount(object) : 4;
}

unsigned int Basic802154GTSspecDescriptor::getFieldTypeFlags(void *object, int field) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldTypeFlags(object, field);
        field -= basedesc->getFieldCount(object);
    }
    static unsigned int fieldTypeFlags[] = {
        FD_ISEDITABLE,
        FD_ISEDITABLE,
        FD_ISEDITABLE,
        FD_ISEDITABLE,
    };
    return (field>=0 && field<4) ? fieldTypeFlags[field] : 0;
}

const char *Basic802154GTSspecDescriptor::getFieldName(void *object, int field) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldName(object, field);
        field -= basedesc->getFieldCount(object);
    }
    static const char *fieldNames[] = {
        "owner",
        "start",
        "length",
        "retransmissor",
    };
    return (field>=0 && field<4) ? fieldNames[field] : NULL;
}

int Basic802154GTSspecDescriptor::findField(void *object, const char *fieldName) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    int base = basedesc ? basedesc->getFieldCount(object) : 0;
    if (fieldName[0]=='o' && strcmp(fieldName, "owner")==0) return base+0;
    if (fieldName[0]=='s' && strcmp(fieldName, "start")==0) return base+1;
    if (fieldName[0]=='l' && strcmp(fieldName, "length")==0) return base+2;
    if (fieldName[0]=='r' && strcmp(fieldName, "retransmissor")==0) return base+3;
    return basedesc ? basedesc->findField(object, fieldName) : -1;
}

const char *Basic802154GTSspecDescriptor::getFieldTypeString(void *object, int field) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldTypeString(object, field);
        field -= basedesc->getFieldCount(object);
    }
    static const char *fieldTypeStrings[] = {
        "int",
        "int",
        "int",
        "bool",
    };
    return (field>=0 && field<4) ? fieldTypeStrings[field] : NULL;
}

const char *Basic802154GTSspecDescriptor::getFieldProperty(void *object, int field, const char *propertyname) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldProperty(object, field, propertyname);
        field -= basedesc->getFieldCount(object);
    }
    switch (field) {
        default: return NULL;
    }
}

int Basic802154GTSspecDescriptor::getArraySize(void *object, int field) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getArraySize(object, field);
        field -= basedesc->getFieldCount(object);
    }
    Basic802154GTSspec *pp = (Basic802154GTSspec *)object; (void)pp;
    switch (field) {
        default: return 0;
    }
}

std::string Basic802154GTSspecDescriptor::getFieldAsString(void *object, int field, int i) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldAsString(object,field,i);
        field -= basedesc->getFieldCount(object);
    }
    Basic802154GTSspec *pp = (Basic802154GTSspec *)object; (void)pp;
    switch (field) {
        case 0: return long2string(pp->owner);
        case 1: return long2string(pp->start);
        case 2: return long2string(pp->length);
        case 3: return bool2string(pp->retransmissor);
        default: return "";
    }
}

bool Basic802154GTSspecDescriptor::setFieldAsString(void *object, int field, int i, const char *value) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->setFieldAsString(object,field,i,value);
        field -= basedesc->getFieldCount(object);
    }
    Basic802154GTSspec *pp = (Basic802154GTSspec *)object; (void)pp;
    switch (field) {
        case 0: pp->owner = string2long(value); return true;
        case 1: pp->start = string2long(value); return true;
        case 2: pp->length = string2long(value); return true;
        case 3: pp->retransmissor = string2bool(value); return true;
        default: return false;
    }
}

const char *Basic802154GTSspecDescriptor::getFieldStructName(void *object, int field) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldStructName(object, field);
        field -= basedesc->getFieldCount(object);
    }
    switch (field) {
        default: return NULL;
    };
}

void *Basic802154GTSspecDescriptor::getFieldStructPointer(void *object, int field, int i) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldStructPointer(object, field, i);
        field -= basedesc->getFieldCount(object);
    }
    Basic802154GTSspec *pp = (Basic802154GTSspec *)object; (void)pp;
    switch (field) {
        default: return NULL;
    }
}

MENSAGENS_ESCUTADAS::MENSAGENS_ESCUTADAS()
{
    idMens = 0;
    idNodo = 0;
}

void doPacking(cCommBuffer *b, MENSAGENS_ESCUTADAS& a)
{
    doPacking(b,a.idMens);
    doPacking(b,a.idNodo);
}

void doUnpacking(cCommBuffer *b, MENSAGENS_ESCUTADAS& a)
{
    doUnpacking(b,a.idMens);
    doUnpacking(b,a.idNodo);
}

class MENSAGENS_ESCUTADASDescriptor : public cClassDescriptor
{
  public:
    MENSAGENS_ESCUTADASDescriptor();
    virtual ~MENSAGENS_ESCUTADASDescriptor();

    virtual bool doesSupport(cObject *obj) const;
    virtual const char *getProperty(const char *propertyname) const;
    virtual int getFieldCount(void *object) const;
    virtual const char *getFieldName(void *object, int field) const;
    virtual int findField(void *object, const char *fieldName) const;
    virtual unsigned int getFieldTypeFlags(void *object, int field) const;
    virtual const char *getFieldTypeString(void *object, int field) const;
    virtual const char *getFieldProperty(void *object, int field, const char *propertyname) const;
    virtual int getArraySize(void *object, int field) const;

    virtual std::string getFieldAsString(void *object, int field, int i) const;
    virtual bool setFieldAsString(void *object, int field, int i, const char *value) const;

    virtual const char *getFieldStructName(void *object, int field) const;
    virtual void *getFieldStructPointer(void *object, int field, int i) const;
};

Register_ClassDescriptor(MENSAGENS_ESCUTADASDescriptor);

MENSAGENS_ESCUTADASDescriptor::MENSAGENS_ESCUTADASDescriptor() : cClassDescriptor("MENSAGENS_ESCUTADAS", "")
{
}

MENSAGENS_ESCUTADASDescriptor::~MENSAGENS_ESCUTADASDescriptor()
{
}

bool MENSAGENS_ESCUTADASDescriptor::doesSupport(cObject *obj) const
{
    return dynamic_cast<MENSAGENS_ESCUTADAS *>(obj)!=NULL;
}

const char *MENSAGENS_ESCUTADASDescriptor::getProperty(const char *propertyname) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    return basedesc ? basedesc->getProperty(propertyname) : NULL;
}

int MENSAGENS_ESCUTADASDescriptor::getFieldCount(void *object) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    return basedesc ? 2+basedesc->getFieldCount(object) : 2;
}

unsigned int MENSAGENS_ESCUTADASDescriptor::getFieldTypeFlags(void *object, int field) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldTypeFlags(object, field);
        field -= basedesc->getFieldCount(object);
    }
    static unsigned int fieldTypeFlags[] = {
        FD_ISEDITABLE,
        FD_ISEDITABLE,
    };
    return (field>=0 && field<2) ? fieldTypeFlags[field] : 0;
}

const char *MENSAGENS_ESCUTADASDescriptor::getFieldName(void *object, int field) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldName(object, field);
        field -= basedesc->getFieldCount(object);
    }
    static const char *fieldNames[] = {
        "idMens",
        "idNodo",
    };
    return (field>=0 && field<2) ? fieldNames[field] : NULL;
}

int MENSAGENS_ESCUTADASDescriptor::findField(void *object, const char *fieldName) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    int base = basedesc ? basedesc->getFieldCount(object) : 0;
    if (fieldName[0]=='i' && strcmp(fieldName, "idMens")==0) return base+0;
    if (fieldName[0]=='i' && strcmp(fieldName, "idNodo")==0) return base+1;
    return basedesc ? basedesc->findField(object, fieldName) : -1;
}

const char *MENSAGENS_ESCUTADASDescriptor::getFieldTypeString(void *object, int field) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldTypeString(object, field);
        field -= basedesc->getFieldCount(object);
    }
    static const char *fieldTypeStrings[] = {
        "short",
        "short",
    };
    return (field>=0 && field<2) ? fieldTypeStrings[field] : NULL;
}

const char *MENSAGENS_ESCUTADASDescriptor::getFieldProperty(void *object, int field, const char *propertyname) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldProperty(object, field, propertyname);
        field -= basedesc->getFieldCount(object);
    }
    switch (field) {
        default: return NULL;
    }
}

int MENSAGENS_ESCUTADASDescriptor::getArraySize(void *object, int field) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getArraySize(object, field);
        field -= basedesc->getFieldCount(object);
    }
    MENSAGENS_ESCUTADAS *pp = (MENSAGENS_ESCUTADAS *)object; (void)pp;
    switch (field) {
        default: return 0;
    }
}

std::string MENSAGENS_ESCUTADASDescriptor::getFieldAsString(void *object, int field, int i) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldAsString(object,field,i);
        field -= basedesc->getFieldCount(object);
    }
    MENSAGENS_ESCUTADAS *pp = (MENSAGENS_ESCUTADAS *)object; (void)pp;
    switch (field) {
        case 0: return long2string(pp->idMens);
        case 1: return long2string(pp->idNodo);
        default: return "";
    }
}

bool MENSAGENS_ESCUTADASDescriptor::setFieldAsString(void *object, int field, int i, const char *value) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->setFieldAsString(object,field,i,value);
        field -= basedesc->getFieldCount(object);
    }
    MENSAGENS_ESCUTADAS *pp = (MENSAGENS_ESCUTADAS *)object; (void)pp;
    switch (field) {
        case 0: pp->idMens = string2long(value); return true;
        case 1: pp->idNodo = string2long(value); return true;
        default: return false;
    }
}

const char *MENSAGENS_ESCUTADASDescriptor::getFieldStructName(void *object, int field) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldStructName(object, field);
        field -= basedesc->getFieldCount(object);
    }
    switch (field) {
        default: return NULL;
    };
}

void *MENSAGENS_ESCUTADASDescriptor::getFieldStructPointer(void *object, int field, int i) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldStructPointer(object, field, i);
        field -= basedesc->getFieldCount(object);
    }
    MENSAGENS_ESCUTADAS *pp = (MENSAGENS_ESCUTADAS *)object; (void)pp;
    switch (field) {
        default: return NULL;
    }
}

DADOS_RETRANS_GACK::DADOS_RETRANS_GACK()
{
    GACK = 0;
    idCoop = 0;
}

void doPacking(cCommBuffer *b, DADOS_RETRANS_GACK& a)
{
    doPacking(b,a.GACK);
    doPacking(b,a.idCoop);
}

void doUnpacking(cCommBuffer *b, DADOS_RETRANS_GACK& a)
{
    doUnpacking(b,a.GACK);
    doUnpacking(b,a.idCoop);
}

class DADOS_RETRANS_GACKDescriptor : public cClassDescriptor
{
  public:
    DADOS_RETRANS_GACKDescriptor();
    virtual ~DADOS_RETRANS_GACKDescriptor();

    virtual bool doesSupport(cObject *obj) const;
    virtual const char *getProperty(const char *propertyname) const;
    virtual int getFieldCount(void *object) const;
    virtual const char *getFieldName(void *object, int field) const;
    virtual int findField(void *object, const char *fieldName) const;
    virtual unsigned int getFieldTypeFlags(void *object, int field) const;
    virtual const char *getFieldTypeString(void *object, int field) const;
    virtual const char *getFieldProperty(void *object, int field, const char *propertyname) const;
    virtual int getArraySize(void *object, int field) const;

    virtual std::string getFieldAsString(void *object, int field, int i) const;
    virtual bool setFieldAsString(void *object, int field, int i, const char *value) const;

    virtual const char *getFieldStructName(void *object, int field) const;
    virtual void *getFieldStructPointer(void *object, int field, int i) const;
};

Register_ClassDescriptor(DADOS_RETRANS_GACKDescriptor);

DADOS_RETRANS_GACKDescriptor::DADOS_RETRANS_GACKDescriptor() : cClassDescriptor("DADOS_RETRANS_GACK", "")
{
}

DADOS_RETRANS_GACKDescriptor::~DADOS_RETRANS_GACKDescriptor()
{
}

bool DADOS_RETRANS_GACKDescriptor::doesSupport(cObject *obj) const
{
    return dynamic_cast<DADOS_RETRANS_GACK *>(obj)!=NULL;
}

const char *DADOS_RETRANS_GACKDescriptor::getProperty(const char *propertyname) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    return basedesc ? basedesc->getProperty(propertyname) : NULL;
}

int DADOS_RETRANS_GACKDescriptor::getFieldCount(void *object) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    return basedesc ? 2+basedesc->getFieldCount(object) : 2;
}

unsigned int DADOS_RETRANS_GACKDescriptor::getFieldTypeFlags(void *object, int field) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldTypeFlags(object, field);
        field -= basedesc->getFieldCount(object);
    }
    static unsigned int fieldTypeFlags[] = {
        FD_ISEDITABLE,
        FD_ISEDITABLE,
    };
    return (field>=0 && field<2) ? fieldTypeFlags[field] : 0;
}

const char *DADOS_RETRANS_GACKDescriptor::getFieldName(void *object, int field) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldName(object, field);
        field -= basedesc->getFieldCount(object);
    }
    static const char *fieldNames[] = {
        "GACK",
        "idCoop",
    };
    return (field>=0 && field<2) ? fieldNames[field] : NULL;
}

int DADOS_RETRANS_GACKDescriptor::findField(void *object, const char *fieldName) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    int base = basedesc ? basedesc->getFieldCount(object) : 0;
    if (fieldName[0]=='G' && strcmp(fieldName, "GACK")==0) return base+0;
    if (fieldName[0]=='i' && strcmp(fieldName, "idCoop")==0) return base+1;
    return basedesc ? basedesc->findField(object, fieldName) : -1;
}

const char *DADOS_RETRANS_GACKDescriptor::getFieldTypeString(void *object, int field) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldTypeString(object, field);
        field -= basedesc->getFieldCount(object);
    }
    static const char *fieldTypeStrings[] = {
        "int",
        "int",
    };
    return (field>=0 && field<2) ? fieldTypeStrings[field] : NULL;
}

const char *DADOS_RETRANS_GACKDescriptor::getFieldProperty(void *object, int field, const char *propertyname) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldProperty(object, field, propertyname);
        field -= basedesc->getFieldCount(object);
    }
    switch (field) {
        default: return NULL;
    }
}

int DADOS_RETRANS_GACKDescriptor::getArraySize(void *object, int field) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getArraySize(object, field);
        field -= basedesc->getFieldCount(object);
    }
    DADOS_RETRANS_GACK *pp = (DADOS_RETRANS_GACK *)object; (void)pp;
    switch (field) {
        default: return 0;
    }
}

std::string DADOS_RETRANS_GACKDescriptor::getFieldAsString(void *object, int field, int i) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldAsString(object,field,i);
        field -= basedesc->getFieldCount(object);
    }
    DADOS_RETRANS_GACK *pp = (DADOS_RETRANS_GACK *)object; (void)pp;
    switch (field) {
        case 0: return long2string(pp->GACK);
        case 1: return long2string(pp->idCoop);
        default: return "";
    }
}

bool DADOS_RETRANS_GACKDescriptor::setFieldAsString(void *object, int field, int i, const char *value) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->setFieldAsString(object,field,i,value);
        field -= basedesc->getFieldCount(object);
    }
    DADOS_RETRANS_GACK *pp = (DADOS_RETRANS_GACK *)object; (void)pp;
    switch (field) {
        case 0: pp->GACK = string2long(value); return true;
        case 1: pp->idCoop = string2long(value); return true;
        default: return false;
    }
}

const char *DADOS_RETRANS_GACKDescriptor::getFieldStructName(void *object, int field) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldStructName(object, field);
        field -= basedesc->getFieldCount(object);
    }
    switch (field) {
        default: return NULL;
    };
}

void *DADOS_RETRANS_GACKDescriptor::getFieldStructPointer(void *object, int field, int i) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldStructPointer(object, field, i);
        field -= basedesc->getFieldCount(object);
    }
    DADOS_RETRANS_GACK *pp = (DADOS_RETRANS_GACK *)object; (void)pp;
    switch (field) {
        default: return NULL;
    }
}

Register_Class(Basic802154Packet);

Basic802154Packet::Basic802154Packet(const char *name, int kind) : ::MacPacket(name,kind)
{
    this->Mac802154PacketType_var = 0;
    this->PANid_var = 0;
    this->srcID_var = 0;
    this->dstID_var = 0;
    this->seqNum_var = 0;
    this->pauseNodo_var = 0;
    this->beaconOrder_var = 0;
    this->frameOrder_var = 0;
    this->BSN_var = 0;
    this->CAPlength_var = 0;
    this->GTSlength_var = 0;
    GTSlist_arraysize = 0;
    this->GTSlist_var = 0;
    vizinhosOuNodosCooperantes_arraysize = 0;
    this->vizinhosOuNodosCooperantes_var = 0;
    this->somaSinais_var = 0;
    this->energy_var = 0;
    dadosVizinho_arraysize = 0;
    this->dadosVizinho_var = 0;
    this->slotInicioRetrans_var = 0;
    this->retransmissao_var = false;
    this->tempoBeacon_var = 0;
    this->tempAtualizVizinhanca_var = 0;
    this->primeiraSelecao_var = 0;
    this->idBeacon_var = 0;
    payload_arraysize = 0;
    this->payload_var = 0;
    coeficiente_arraysize = 0;
    this->coeficiente_var = 0;
    gack_arraysize = 0;
    this->gack_var = 0;
    coopAuxiliares_arraysize = 0;
    this->coopAuxiliares_var = 0;
    numSlotPerCoop_arraysize = 0;
    this->numSlotPerCoop_var = 0;
    this->idNodoRetransmitido_var = 0;
    vetBit_arraysize = 0;
    this->vetBit_var = 0;
    dadosRetrans_arraysize = 0;
    this->dadosRetrans_var = 0;
}

Basic802154Packet::Basic802154Packet(const Basic802154Packet& other) : ::MacPacket(other)
{
    GTSlist_arraysize = 0;
    this->GTSlist_var = 0;
    vizinhosOuNodosCooperantes_arraysize = 0;
    this->vizinhosOuNodosCooperantes_var = 0;
    dadosVizinho_arraysize = 0;
    this->dadosVizinho_var = 0;
    payload_arraysize = 0;
    this->payload_var = 0;
    coeficiente_arraysize = 0;
    this->coeficiente_var = 0;
    gack_arraysize = 0;
    this->gack_var = 0;
    coopAuxiliares_arraysize = 0;
    this->coopAuxiliares_var = 0;
    numSlotPerCoop_arraysize = 0;
    this->numSlotPerCoop_var = 0;
    vetBit_arraysize = 0;
    this->vetBit_var = 0;
    dadosRetrans_arraysize = 0;
    this->dadosRetrans_var = 0;
    copy(other);
}

Basic802154Packet::~Basic802154Packet()
{
    delete [] GTSlist_var;
    delete [] vizinhosOuNodosCooperantes_var;
    delete [] dadosVizinho_var;
    delete [] payload_var;
    delete [] coeficiente_var;
    delete [] gack_var;
    delete [] coopAuxiliares_var;
    delete [] numSlotPerCoop_var;
    delete [] vetBit_var;
    delete [] dadosRetrans_var;
}

Basic802154Packet& Basic802154Packet::operator=(const Basic802154Packet& other)
{
    if (this==&other) return *this;
    ::MacPacket::operator=(other);
    copy(other);
    return *this;
}

void Basic802154Packet::copy(const Basic802154Packet& other)
{
    this->Mac802154PacketType_var = other.Mac802154PacketType_var;
    this->PANid_var = other.PANid_var;
    this->srcID_var = other.srcID_var;
    this->dstID_var = other.dstID_var;
    this->seqNum_var = other.seqNum_var;
    this->pauseNodo_var = other.pauseNodo_var;
    this->beaconOrder_var = other.beaconOrder_var;
    this->frameOrder_var = other.frameOrder_var;
    this->BSN_var = other.BSN_var;
    this->CAPlength_var = other.CAPlength_var;
    this->GTSlength_var = other.GTSlength_var;
    delete [] this->GTSlist_var;
    this->GTSlist_var = (other.GTSlist_arraysize==0) ? NULL : new Basic802154GTSspec[other.GTSlist_arraysize];
    GTSlist_arraysize = other.GTSlist_arraysize;
    for (unsigned int i=0; i<GTSlist_arraysize; i++)
        this->GTSlist_var[i] = other.GTSlist_var[i];
    delete [] this->vizinhosOuNodosCooperantes_var;
    this->vizinhosOuNodosCooperantes_var = (other.vizinhosOuNodosCooperantes_arraysize==0) ? NULL : new short[other.vizinhosOuNodosCooperantes_arraysize];
    vizinhosOuNodosCooperantes_arraysize = other.vizinhosOuNodosCooperantes_arraysize;
    for (unsigned int i=0; i<vizinhosOuNodosCooperantes_arraysize; i++)
        this->vizinhosOuNodosCooperantes_var[i] = other.vizinhosOuNodosCooperantes_var[i];
    this->somaSinais_var = other.somaSinais_var;
    this->energy_var = other.energy_var;
    delete [] this->dadosVizinho_var;
    this->dadosVizinho_var = (other.dadosVizinho_arraysize==0) ? NULL : new MENSAGENS_ESCUTADAS[other.dadosVizinho_arraysize];
    dadosVizinho_arraysize = other.dadosVizinho_arraysize;
    for (unsigned int i=0; i<dadosVizinho_arraysize; i++)
        this->dadosVizinho_var[i] = other.dadosVizinho_var[i];
    this->slotInicioRetrans_var = other.slotInicioRetrans_var;
    this->retransmissao_var = other.retransmissao_var;
    this->tempoBeacon_var = other.tempoBeacon_var;
    this->tempAtualizVizinhanca_var = other.tempAtualizVizinhanca_var;
    this->primeiraSelecao_var = other.primeiraSelecao_var;
    this->idBeacon_var = other.idBeacon_var;
    delete [] this->payload_var;
    this->payload_var = (other.payload_arraysize==0) ? NULL : new unsigned short[other.payload_arraysize];
    payload_arraysize = other.payload_arraysize;
    for (unsigned int i=0; i<payload_arraysize; i++)
        this->payload_var[i] = other.payload_var[i];
    delete [] this->coeficiente_var;
    this->coeficiente_var = (other.coeficiente_arraysize==0) ? NULL : new unsigned short[other.coeficiente_arraysize];
    coeficiente_arraysize = other.coeficiente_arraysize;
    for (unsigned int i=0; i<coeficiente_arraysize; i++)
        this->coeficiente_var[i] = other.coeficiente_var[i];
    delete [] this->gack_var;
    this->gack_var = (other.gack_arraysize==0) ? NULL : new unsigned short[other.gack_arraysize];
    gack_arraysize = other.gack_arraysize;
    for (unsigned int i=0; i<gack_arraysize; i++)
        this->gack_var[i] = other.gack_var[i];
    delete [] this->coopAuxiliares_var;
    this->coopAuxiliares_var = (other.coopAuxiliares_arraysize==0) ? NULL : new short[other.coopAuxiliares_arraysize];
    coopAuxiliares_arraysize = other.coopAuxiliares_arraysize;
    for (unsigned int i=0; i<coopAuxiliares_arraysize; i++)
        this->coopAuxiliares_var[i] = other.coopAuxiliares_var[i];
    delete [] this->numSlotPerCoop_var;
    this->numSlotPerCoop_var = (other.numSlotPerCoop_arraysize==0) ? NULL : new short[other.numSlotPerCoop_arraysize];
    numSlotPerCoop_arraysize = other.numSlotPerCoop_arraysize;
    for (unsigned int i=0; i<numSlotPerCoop_arraysize; i++)
        this->numSlotPerCoop_var[i] = other.numSlotPerCoop_var[i];
    this->idNodoRetransmitido_var = other.idNodoRetransmitido_var;
    delete [] this->vetBit_var;
    this->vetBit_var = (other.vetBit_arraysize==0) ? NULL : new uint64_t[other.vetBit_arraysize];
    vetBit_arraysize = other.vetBit_arraysize;
    for (unsigned int i=0; i<vetBit_arraysize; i++)
        this->vetBit_var[i] = other.vetBit_var[i];
    delete [] this->dadosRetrans_var;
    this->dadosRetrans_var = (other.dadosRetrans_arraysize==0) ? NULL : new DADOS_RETRANS_GACK[other.dadosRetrans_arraysize];
    dadosRetrans_arraysize = other.dadosRetrans_arraysize;
    for (unsigned int i=0; i<dadosRetrans_arraysize; i++)
        this->dadosRetrans_var[i] = other.dadosRetrans_var[i];
}

void Basic802154Packet::parsimPack(cCommBuffer *b)
{
    ::MacPacket::parsimPack(b);
    doPacking(b,this->Mac802154PacketType_var);
    doPacking(b,this->PANid_var);
    doPacking(b,this->srcID_var);
    doPacking(b,this->dstID_var);
    doPacking(b,this->seqNum_var);
    doPacking(b,this->pauseNodo_var);
    doPacking(b,this->beaconOrder_var);
    doPacking(b,this->frameOrder_var);
    doPacking(b,this->BSN_var);
    doPacking(b,this->CAPlength_var);
    doPacking(b,this->GTSlength_var);
    b->pack(GTSlist_arraysize);
    doPacking(b,this->GTSlist_var,GTSlist_arraysize);
    b->pack(vizinhosOuNodosCooperantes_arraysize);
    doPacking(b,this->vizinhosOuNodosCooperantes_var,vizinhosOuNodosCooperantes_arraysize);
    doPacking(b,this->somaSinais_var);
    doPacking(b,this->energy_var);
    b->pack(dadosVizinho_arraysize);
    doPacking(b,this->dadosVizinho_var,dadosVizinho_arraysize);
    doPacking(b,this->slotInicioRetrans_var);
    doPacking(b,this->retransmissao_var);
    doPacking(b,this->tempoBeacon_var);
    doPacking(b,this->tempAtualizVizinhanca_var);
    doPacking(b,this->primeiraSelecao_var);
    doPacking(b,this->idBeacon_var);
    b->pack(payload_arraysize);
    doPacking(b,this->payload_var,payload_arraysize);
    b->pack(coeficiente_arraysize);
    doPacking(b,this->coeficiente_var,coeficiente_arraysize);
    b->pack(gack_arraysize);
    doPacking(b,this->gack_var,gack_arraysize);
    b->pack(coopAuxiliares_arraysize);
    doPacking(b,this->coopAuxiliares_var,coopAuxiliares_arraysize);
    b->pack(numSlotPerCoop_arraysize);
    doPacking(b,this->numSlotPerCoop_var,numSlotPerCoop_arraysize);
    doPacking(b,this->idNodoRetransmitido_var);
    b->pack(vetBit_arraysize);
    doPacking(b,this->vetBit_var,vetBit_arraysize);
    b->pack(dadosRetrans_arraysize);
    doPacking(b,this->dadosRetrans_var,dadosRetrans_arraysize);
}

void Basic802154Packet::parsimUnpack(cCommBuffer *b)
{
    ::MacPacket::parsimUnpack(b);
    doUnpacking(b,this->Mac802154PacketType_var);
    doUnpacking(b,this->PANid_var);
    doUnpacking(b,this->srcID_var);
    doUnpacking(b,this->dstID_var);
    doUnpacking(b,this->seqNum_var);
    doUnpacking(b,this->pauseNodo_var);
    doUnpacking(b,this->beaconOrder_var);
    doUnpacking(b,this->frameOrder_var);
    doUnpacking(b,this->BSN_var);
    doUnpacking(b,this->CAPlength_var);
    doUnpacking(b,this->GTSlength_var);
    delete [] this->GTSlist_var;
    b->unpack(GTSlist_arraysize);
    if (GTSlist_arraysize==0) {
        this->GTSlist_var = 0;
    } else {
        this->GTSlist_var = new Basic802154GTSspec[GTSlist_arraysize];
        doUnpacking(b,this->GTSlist_var,GTSlist_arraysize);
    }
    delete [] this->vizinhosOuNodosCooperantes_var;
    b->unpack(vizinhosOuNodosCooperantes_arraysize);
    if (vizinhosOuNodosCooperantes_arraysize==0) {
        this->vizinhosOuNodosCooperantes_var = 0;
    } else {
        this->vizinhosOuNodosCooperantes_var = new short[vizinhosOuNodosCooperantes_arraysize];
        doUnpacking(b,this->vizinhosOuNodosCooperantes_var,vizinhosOuNodosCooperantes_arraysize);
    }
    doUnpacking(b,this->somaSinais_var);
    doUnpacking(b,this->energy_var);
    delete [] this->dadosVizinho_var;
    b->unpack(dadosVizinho_arraysize);
    if (dadosVizinho_arraysize==0) {
        this->dadosVizinho_var = 0;
    } else {
        this->dadosVizinho_var = new MENSAGENS_ESCUTADAS[dadosVizinho_arraysize];
        doUnpacking(b,this->dadosVizinho_var,dadosVizinho_arraysize);
    }
    doUnpacking(b,this->slotInicioRetrans_var);
    doUnpacking(b,this->retransmissao_var);
    doUnpacking(b,this->tempoBeacon_var);
    doUnpacking(b,this->tempAtualizVizinhanca_var);
    doUnpacking(b,this->primeiraSelecao_var);
    doUnpacking(b,this->idBeacon_var);
    delete [] this->payload_var;
    b->unpack(payload_arraysize);
    if (payload_arraysize==0) {
        this->payload_var = 0;
    } else {
        this->payload_var = new unsigned short[payload_arraysize];
        doUnpacking(b,this->payload_var,payload_arraysize);
    }
    delete [] this->coeficiente_var;
    b->unpack(coeficiente_arraysize);
    if (coeficiente_arraysize==0) {
        this->coeficiente_var = 0;
    } else {
        this->coeficiente_var = new unsigned short[coeficiente_arraysize];
        doUnpacking(b,this->coeficiente_var,coeficiente_arraysize);
    }
    delete [] this->gack_var;
    b->unpack(gack_arraysize);
    if (gack_arraysize==0) {
        this->gack_var = 0;
    } else {
        this->gack_var = new unsigned short[gack_arraysize];
        doUnpacking(b,this->gack_var,gack_arraysize);
    }
    delete [] this->coopAuxiliares_var;
    b->unpack(coopAuxiliares_arraysize);
    if (coopAuxiliares_arraysize==0) {
        this->coopAuxiliares_var = 0;
    } else {
        this->coopAuxiliares_var = new short[coopAuxiliares_arraysize];
        doUnpacking(b,this->coopAuxiliares_var,coopAuxiliares_arraysize);
    }
    delete [] this->numSlotPerCoop_var;
    b->unpack(numSlotPerCoop_arraysize);
    if (numSlotPerCoop_arraysize==0) {
        this->numSlotPerCoop_var = 0;
    } else {
        this->numSlotPerCoop_var = new short[numSlotPerCoop_arraysize];
        doUnpacking(b,this->numSlotPerCoop_var,numSlotPerCoop_arraysize);
    }
    doUnpacking(b,this->idNodoRetransmitido_var);
    delete [] this->vetBit_var;
    b->unpack(vetBit_arraysize);
    if (vetBit_arraysize==0) {
        this->vetBit_var = 0;
    } else {
        this->vetBit_var = new uint64_t[vetBit_arraysize];
        doUnpacking(b,this->vetBit_var,vetBit_arraysize);
    }
    delete [] this->dadosRetrans_var;
    b->unpack(dadosRetrans_arraysize);
    if (dadosRetrans_arraysize==0) {
        this->dadosRetrans_var = 0;
    } else {
        this->dadosRetrans_var = new DADOS_RETRANS_GACK[dadosRetrans_arraysize];
        doUnpacking(b,this->dadosRetrans_var,dadosRetrans_arraysize);
    }
}

int Basic802154Packet::getMac802154PacketType() const
{
    return Mac802154PacketType_var;
}

void Basic802154Packet::setMac802154PacketType(int Mac802154PacketType)
{
    this->Mac802154PacketType_var = Mac802154PacketType;
}

int Basic802154Packet::getPANid() const
{
    return PANid_var;
}

void Basic802154Packet::setPANid(int PANid)
{
    this->PANid_var = PANid;
}

int Basic802154Packet::getSrcID() const
{
    return srcID_var;
}

void Basic802154Packet::setSrcID(int srcID)
{
    this->srcID_var = srcID;
}

int Basic802154Packet::getDstID() const
{
    return dstID_var;
}

void Basic802154Packet::setDstID(int dstID)
{
    this->dstID_var = dstID;
}

int Basic802154Packet::getSeqNum() const
{
    return seqNum_var;
}

void Basic802154Packet::setSeqNum(int seqNum)
{
    this->seqNum_var = seqNum;
}

bool Basic802154Packet::getPauseNodo() const
{
    return pauseNodo_var;
}

void Basic802154Packet::setPauseNodo(bool pauseNodo)
{
    this->pauseNodo_var = pauseNodo;
}

int Basic802154Packet::getBeaconOrder() const
{
    return beaconOrder_var;
}

void Basic802154Packet::setBeaconOrder(int beaconOrder)
{
    this->beaconOrder_var = beaconOrder;
}

int Basic802154Packet::getFrameOrder() const
{
    return frameOrder_var;
}

void Basic802154Packet::setFrameOrder(int frameOrder)
{
    this->frameOrder_var = frameOrder;
}

int Basic802154Packet::getBSN() const
{
    return BSN_var;
}

void Basic802154Packet::setBSN(int BSN)
{
    this->BSN_var = BSN;
}

int Basic802154Packet::getCAPlength() const
{
    return CAPlength_var;
}

void Basic802154Packet::setCAPlength(int CAPlength)
{
    this->CAPlength_var = CAPlength;
}

int Basic802154Packet::getGTSlength() const
{
    return GTSlength_var;
}

void Basic802154Packet::setGTSlength(int GTSlength)
{
    this->GTSlength_var = GTSlength;
}

void Basic802154Packet::setGTSlistArraySize(unsigned int size)
{
    Basic802154GTSspec *GTSlist_var2 = (size==0) ? NULL : new Basic802154GTSspec[size];
    unsigned int sz = GTSlist_arraysize < size ? GTSlist_arraysize : size;
    for (unsigned int i=0; i<sz; i++)
        GTSlist_var2[i] = this->GTSlist_var[i];
    GTSlist_arraysize = size;
    delete [] this->GTSlist_var;
    this->GTSlist_var = GTSlist_var2;
}

unsigned int Basic802154Packet::getGTSlistArraySize() const
{
    return GTSlist_arraysize;
}

Basic802154GTSspec& Basic802154Packet::getGTSlist(unsigned int k)
{
    if (k>=GTSlist_arraysize) throw cRuntimeError("Array of size %d indexed by %d", GTSlist_arraysize, k);
    return GTSlist_var[k];
}

void Basic802154Packet::setGTSlist(unsigned int k, const Basic802154GTSspec& GTSlist)
{
    if (k>=GTSlist_arraysize) throw cRuntimeError("Array of size %d indexed by %d", GTSlist_arraysize, k);
    this->GTSlist_var[k] = GTSlist;
}

void Basic802154Packet::setVizinhosOuNodosCooperantesArraySize(unsigned int size)
{
    short *vizinhosOuNodosCooperantes_var2 = (size==0) ? NULL : new short[size];
    unsigned int sz = vizinhosOuNodosCooperantes_arraysize < size ? vizinhosOuNodosCooperantes_arraysize : size;
    for (unsigned int i=0; i<sz; i++)
        vizinhosOuNodosCooperantes_var2[i] = this->vizinhosOuNodosCooperantes_var[i];
    for (unsigned int i=sz; i<size; i++)
        vizinhosOuNodosCooperantes_var2[i] = 0;
    vizinhosOuNodosCooperantes_arraysize = size;
    delete [] this->vizinhosOuNodosCooperantes_var;
    this->vizinhosOuNodosCooperantes_var = vizinhosOuNodosCooperantes_var2;
}

unsigned int Basic802154Packet::getVizinhosOuNodosCooperantesArraySize() const
{
    return vizinhosOuNodosCooperantes_arraysize;
}

short Basic802154Packet::getVizinhosOuNodosCooperantes(unsigned int k) const
{
    if (k>=vizinhosOuNodosCooperantes_arraysize) throw cRuntimeError("Array of size %d indexed by %d", vizinhosOuNodosCooperantes_arraysize, k);
    return vizinhosOuNodosCooperantes_var[k];
}

void Basic802154Packet::setVizinhosOuNodosCooperantes(unsigned int k, short vizinhosOuNodosCooperantes)
{
    if (k>=vizinhosOuNodosCooperantes_arraysize) throw cRuntimeError("Array of size %d indexed by %d", vizinhosOuNodosCooperantes_arraysize, k);
    this->vizinhosOuNodosCooperantes_var[k] = vizinhosOuNodosCooperantes;
}

double Basic802154Packet::getSomaSinais() const
{
    return somaSinais_var;
}

void Basic802154Packet::setSomaSinais(double somaSinais)
{
    this->somaSinais_var = somaSinais;
}

double Basic802154Packet::getEnergy() const
{
    return energy_var;
}

void Basic802154Packet::setEnergy(double energy)
{
    this->energy_var = energy;
}

void Basic802154Packet::setDadosVizinhoArraySize(unsigned int size)
{
    MENSAGENS_ESCUTADAS *dadosVizinho_var2 = (size==0) ? NULL : new MENSAGENS_ESCUTADAS[size];
    unsigned int sz = dadosVizinho_arraysize < size ? dadosVizinho_arraysize : size;
    for (unsigned int i=0; i<sz; i++)
        dadosVizinho_var2[i] = this->dadosVizinho_var[i];
    dadosVizinho_arraysize = size;
    delete [] this->dadosVizinho_var;
    this->dadosVizinho_var = dadosVizinho_var2;
}

unsigned int Basic802154Packet::getDadosVizinhoArraySize() const
{
    return dadosVizinho_arraysize;
}

MENSAGENS_ESCUTADAS& Basic802154Packet::getDadosVizinho(unsigned int k)
{
    if (k>=dadosVizinho_arraysize) throw cRuntimeError("Array of size %d indexed by %d", dadosVizinho_arraysize, k);
    return dadosVizinho_var[k];
}

void Basic802154Packet::setDadosVizinho(unsigned int k, const MENSAGENS_ESCUTADAS& dadosVizinho)
{
    if (k>=dadosVizinho_arraysize) throw cRuntimeError("Array of size %d indexed by %d", dadosVizinho_arraysize, k);
    this->dadosVizinho_var[k] = dadosVizinho;
}

short Basic802154Packet::getSlotInicioRetrans() const
{
    return slotInicioRetrans_var;
}

void Basic802154Packet::setSlotInicioRetrans(short slotInicioRetrans)
{
    this->slotInicioRetrans_var = slotInicioRetrans;
}

bool Basic802154Packet::getRetransmissao() const
{
    return retransmissao_var;
}

void Basic802154Packet::setRetransmissao(bool retransmissao)
{
    this->retransmissao_var = retransmissao;
}

short Basic802154Packet::getTempoBeacon() const
{
    return tempoBeacon_var;
}

void Basic802154Packet::setTempoBeacon(short tempoBeacon)
{
    this->tempoBeacon_var = tempoBeacon;
}

bool Basic802154Packet::getTempAtualizVizinhanca() const
{
    return tempAtualizVizinhanca_var;
}

void Basic802154Packet::setTempAtualizVizinhanca(bool tempAtualizVizinhanca)
{
    this->tempAtualizVizinhanca_var = tempAtualizVizinhanca;
}

short Basic802154Packet::getPrimeiraSelecao() const
{
    return primeiraSelecao_var;
}

void Basic802154Packet::setPrimeiraSelecao(short primeiraSelecao)
{
    this->primeiraSelecao_var = primeiraSelecao;
}

short Basic802154Packet::getIdBeacon() const
{
    return idBeacon_var;
}

void Basic802154Packet::setIdBeacon(short idBeacon)
{
    this->idBeacon_var = idBeacon;
}

void Basic802154Packet::setPayloadArraySize(unsigned int size)
{
    unsigned short *payload_var2 = (size==0) ? NULL : new unsigned short[size];
    unsigned int sz = payload_arraysize < size ? payload_arraysize : size;
    for (unsigned int i=0; i<sz; i++)
        payload_var2[i] = this->payload_var[i];
    for (unsigned int i=sz; i<size; i++)
        payload_var2[i] = 0;
    payload_arraysize = size;
    delete [] this->payload_var;
    this->payload_var = payload_var2;
}

unsigned int Basic802154Packet::getPayloadArraySize() const
{
    return payload_arraysize;
}

unsigned short Basic802154Packet::getPayload(unsigned int k) const
{
    if (k>=payload_arraysize) throw cRuntimeError("Array of size %d indexed by %d", payload_arraysize, k);
    return payload_var[k];
}

void Basic802154Packet::setPayload(unsigned int k, unsigned short payload)
{
    if (k>=payload_arraysize) throw cRuntimeError("Array of size %d indexed by %d", payload_arraysize, k);
    this->payload_var[k] = payload;
}

void Basic802154Packet::setCoeficienteArraySize(unsigned int size)
{
    unsigned short *coeficiente_var2 = (size==0) ? NULL : new unsigned short[size];
    unsigned int sz = coeficiente_arraysize < size ? coeficiente_arraysize : size;
    for (unsigned int i=0; i<sz; i++)
        coeficiente_var2[i] = this->coeficiente_var[i];
    for (unsigned int i=sz; i<size; i++)
        coeficiente_var2[i] = 0;
    coeficiente_arraysize = size;
    delete [] this->coeficiente_var;
    this->coeficiente_var = coeficiente_var2;
}

unsigned int Basic802154Packet::getCoeficienteArraySize() const
{
    return coeficiente_arraysize;
}

unsigned short Basic802154Packet::getCoeficiente(unsigned int k) const
{
    if (k>=coeficiente_arraysize) throw cRuntimeError("Array of size %d indexed by %d", coeficiente_arraysize, k);
    return coeficiente_var[k];
}

void Basic802154Packet::setCoeficiente(unsigned int k, unsigned short coeficiente)
{
    if (k>=coeficiente_arraysize) throw cRuntimeError("Array of size %d indexed by %d", coeficiente_arraysize, k);
    this->coeficiente_var[k] = coeficiente;
}

void Basic802154Packet::setGackArraySize(unsigned int size)
{
    unsigned short *gack_var2 = (size==0) ? NULL : new unsigned short[size];
    unsigned int sz = gack_arraysize < size ? gack_arraysize : size;
    for (unsigned int i=0; i<sz; i++)
        gack_var2[i] = this->gack_var[i];
    for (unsigned int i=sz; i<size; i++)
        gack_var2[i] = 0;
    gack_arraysize = size;
    delete [] this->gack_var;
    this->gack_var = gack_var2;
}

unsigned int Basic802154Packet::getGackArraySize() const
{
    return gack_arraysize;
}

unsigned short Basic802154Packet::getGack(unsigned int k) const
{
    if (k>=gack_arraysize) throw cRuntimeError("Array of size %d indexed by %d", gack_arraysize, k);
    return gack_var[k];
}

void Basic802154Packet::setGack(unsigned int k, unsigned short gack)
{
    if (k>=gack_arraysize) throw cRuntimeError("Array of size %d indexed by %d", gack_arraysize, k);
    this->gack_var[k] = gack;
}

void Basic802154Packet::setCoopAuxiliaresArraySize(unsigned int size)
{
    short *coopAuxiliares_var2 = (size==0) ? NULL : new short[size];
    unsigned int sz = coopAuxiliares_arraysize < size ? coopAuxiliares_arraysize : size;
    for (unsigned int i=0; i<sz; i++)
        coopAuxiliares_var2[i] = this->coopAuxiliares_var[i];
    for (unsigned int i=sz; i<size; i++)
        coopAuxiliares_var2[i] = 0;
    coopAuxiliares_arraysize = size;
    delete [] this->coopAuxiliares_var;
    this->coopAuxiliares_var = coopAuxiliares_var2;
}

unsigned int Basic802154Packet::getCoopAuxiliaresArraySize() const
{
    return coopAuxiliares_arraysize;
}

short Basic802154Packet::getCoopAuxiliares(unsigned int k) const
{
    if (k>=coopAuxiliares_arraysize) throw cRuntimeError("Array of size %d indexed by %d", coopAuxiliares_arraysize, k);
    return coopAuxiliares_var[k];
}

void Basic802154Packet::setCoopAuxiliares(unsigned int k, short coopAuxiliares)
{
    if (k>=coopAuxiliares_arraysize) throw cRuntimeError("Array of size %d indexed by %d", coopAuxiliares_arraysize, k);
    this->coopAuxiliares_var[k] = coopAuxiliares;
}

void Basic802154Packet::setNumSlotPerCoopArraySize(unsigned int size)
{
    short *numSlotPerCoop_var2 = (size==0) ? NULL : new short[size];
    unsigned int sz = numSlotPerCoop_arraysize < size ? numSlotPerCoop_arraysize : size;
    for (unsigned int i=0; i<sz; i++)
        numSlotPerCoop_var2[i] = this->numSlotPerCoop_var[i];
    for (unsigned int i=sz; i<size; i++)
        numSlotPerCoop_var2[i] = 0;
    numSlotPerCoop_arraysize = size;
    delete [] this->numSlotPerCoop_var;
    this->numSlotPerCoop_var = numSlotPerCoop_var2;
}

unsigned int Basic802154Packet::getNumSlotPerCoopArraySize() const
{
    return numSlotPerCoop_arraysize;
}

short Basic802154Packet::getNumSlotPerCoop(unsigned int k) const
{
    if (k>=numSlotPerCoop_arraysize) throw cRuntimeError("Array of size %d indexed by %d", numSlotPerCoop_arraysize, k);
    return numSlotPerCoop_var[k];
}

void Basic802154Packet::setNumSlotPerCoop(unsigned int k, short numSlotPerCoop)
{
    if (k>=numSlotPerCoop_arraysize) throw cRuntimeError("Array of size %d indexed by %d", numSlotPerCoop_arraysize, k);
    this->numSlotPerCoop_var[k] = numSlotPerCoop;
}

short Basic802154Packet::getIdNodoRetransmitido() const
{
    return idNodoRetransmitido_var;
}

void Basic802154Packet::setIdNodoRetransmitido(short idNodoRetransmitido)
{
    this->idNodoRetransmitido_var = idNodoRetransmitido;
}

void Basic802154Packet::setVetBitArraySize(unsigned int size)
{
    uint64_t *vetBit_var2 = (size==0) ? NULL : new uint64_t[size];
    unsigned int sz = vetBit_arraysize < size ? vetBit_arraysize : size;
    for (unsigned int i=0; i<sz; i++)
        vetBit_var2[i] = this->vetBit_var[i];
    for (unsigned int i=sz; i<size; i++)
        vetBit_var2[i] = 0;
    vetBit_arraysize = size;
    delete [] this->vetBit_var;
    this->vetBit_var = vetBit_var2;
}

unsigned int Basic802154Packet::getVetBitArraySize() const
{
    return vetBit_arraysize;
}

uint64_t Basic802154Packet::getVetBit(unsigned int k) const
{
    if (k>=vetBit_arraysize) throw cRuntimeError("Array of size %d indexed by %d", vetBit_arraysize, k);
    return vetBit_var[k];
}

void Basic802154Packet::setVetBit(unsigned int k, uint64_t vetBit)
{
    if (k>=vetBit_arraysize) throw cRuntimeError("Array of size %d indexed by %d", vetBit_arraysize, k);
    this->vetBit_var[k] = vetBit;
}

void Basic802154Packet::setDadosRetransArraySize(unsigned int size)
{
    DADOS_RETRANS_GACK *dadosRetrans_var2 = (size==0) ? NULL : new DADOS_RETRANS_GACK[size];
    unsigned int sz = dadosRetrans_arraysize < size ? dadosRetrans_arraysize : size;
    for (unsigned int i=0; i<sz; i++)
        dadosRetrans_var2[i] = this->dadosRetrans_var[i];
    dadosRetrans_arraysize = size;
    delete [] this->dadosRetrans_var;
    this->dadosRetrans_var = dadosRetrans_var2;
}

unsigned int Basic802154Packet::getDadosRetransArraySize() const
{
    return dadosRetrans_arraysize;
}

DADOS_RETRANS_GACK& Basic802154Packet::getDadosRetrans(unsigned int k)
{
    if (k>=dadosRetrans_arraysize) throw cRuntimeError("Array of size %d indexed by %d", dadosRetrans_arraysize, k);
    return dadosRetrans_var[k];
}

void Basic802154Packet::setDadosRetrans(unsigned int k, const DADOS_RETRANS_GACK& dadosRetrans)
{
    if (k>=dadosRetrans_arraysize) throw cRuntimeError("Array of size %d indexed by %d", dadosRetrans_arraysize, k);
    this->dadosRetrans_var[k] = dadosRetrans;
}

class Basic802154PacketDescriptor : public cClassDescriptor
{
  public:
    Basic802154PacketDescriptor();
    virtual ~Basic802154PacketDescriptor();

    virtual bool doesSupport(cObject *obj) const;
    virtual const char *getProperty(const char *propertyname) const;
    virtual int getFieldCount(void *object) const;
    virtual const char *getFieldName(void *object, int field) const;
    virtual int findField(void *object, const char *fieldName) const;
    virtual unsigned int getFieldTypeFlags(void *object, int field) const;
    virtual const char *getFieldTypeString(void *object, int field) const;
    virtual const char *getFieldProperty(void *object, int field, const char *propertyname) const;
    virtual int getArraySize(void *object, int field) const;

    virtual std::string getFieldAsString(void *object, int field, int i) const;
    virtual bool setFieldAsString(void *object, int field, int i, const char *value) const;

    virtual const char *getFieldStructName(void *object, int field) const;
    virtual void *getFieldStructPointer(void *object, int field, int i) const;
};

Register_ClassDescriptor(Basic802154PacketDescriptor);

Basic802154PacketDescriptor::Basic802154PacketDescriptor() : cClassDescriptor("Basic802154Packet", "MacPacket")
{
}

Basic802154PacketDescriptor::~Basic802154PacketDescriptor()
{
}

bool Basic802154PacketDescriptor::doesSupport(cObject *obj) const
{
    return dynamic_cast<Basic802154Packet *>(obj)!=NULL;
}

const char *Basic802154PacketDescriptor::getProperty(const char *propertyname) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    return basedesc ? basedesc->getProperty(propertyname) : NULL;
}

int Basic802154PacketDescriptor::getFieldCount(void *object) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    return basedesc ? 30+basedesc->getFieldCount(object) : 30;
}

unsigned int Basic802154PacketDescriptor::getFieldTypeFlags(void *object, int field) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldTypeFlags(object, field);
        field -= basedesc->getFieldCount(object);
    }
    static unsigned int fieldTypeFlags[] = {
        FD_ISEDITABLE,
        FD_ISEDITABLE,
        FD_ISEDITABLE,
        FD_ISEDITABLE,
        FD_ISEDITABLE,
        FD_ISEDITABLE,
        FD_ISEDITABLE,
        FD_ISEDITABLE,
        FD_ISEDITABLE,
        FD_ISEDITABLE,
        FD_ISEDITABLE,
        FD_ISARRAY | FD_ISCOMPOUND,
        FD_ISARRAY | FD_ISEDITABLE,
        FD_ISEDITABLE,
        FD_ISEDITABLE,
        FD_ISARRAY | FD_ISCOMPOUND,
        FD_ISEDITABLE,
        FD_ISEDITABLE,
        FD_ISEDITABLE,
        FD_ISEDITABLE,
        FD_ISEDITABLE,
        FD_ISEDITABLE,
        FD_ISARRAY | FD_ISEDITABLE,
        FD_ISARRAY | FD_ISEDITABLE,
        FD_ISARRAY | FD_ISEDITABLE,
        FD_ISARRAY | FD_ISEDITABLE,
        FD_ISARRAY | FD_ISEDITABLE,
        FD_ISEDITABLE,
        FD_ISARRAY | FD_ISEDITABLE,
        FD_ISARRAY | FD_ISCOMPOUND,
    };
    return (field>=0 && field<30) ? fieldTypeFlags[field] : 0;
}

const char *Basic802154PacketDescriptor::getFieldName(void *object, int field) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldName(object, field);
        field -= basedesc->getFieldCount(object);
    }
    static const char *fieldNames[] = {
        "Mac802154PacketType",
        "PANid",
        "srcID",
        "dstID",
        "seqNum",
        "pauseNodo",
        "beaconOrder",
        "frameOrder",
        "BSN",
        "CAPlength",
        "GTSlength",
        "GTSlist",
        "vizinhosOuNodosCooperantes",
        "somaSinais",
        "energy",
        "dadosVizinho",
        "slotInicioRetrans",
        "retransmissao",
        "tempoBeacon",
        "tempAtualizVizinhanca",
        "primeiraSelecao",
        "idBeacon",
        "payload",
        "coeficiente",
        "gack",
        "coopAuxiliares",
        "numSlotPerCoop",
        "idNodoRetransmitido",
        "vetBit",
        "dadosRetrans",
    };
    return (field>=0 && field<30) ? fieldNames[field] : NULL;
}

int Basic802154PacketDescriptor::findField(void *object, const char *fieldName) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    int base = basedesc ? basedesc->getFieldCount(object) : 0;
    if (fieldName[0]=='M' && strcmp(fieldName, "Mac802154PacketType")==0) return base+0;
    if (fieldName[0]=='P' && strcmp(fieldName, "PANid")==0) return base+1;
    if (fieldName[0]=='s' && strcmp(fieldName, "srcID")==0) return base+2;
    if (fieldName[0]=='d' && strcmp(fieldName, "dstID")==0) return base+3;
    if (fieldName[0]=='s' && strcmp(fieldName, "seqNum")==0) return base+4;
    if (fieldName[0]=='p' && strcmp(fieldName, "pauseNodo")==0) return base+5;
    if (fieldName[0]=='b' && strcmp(fieldName, "beaconOrder")==0) return base+6;
    if (fieldName[0]=='f' && strcmp(fieldName, "frameOrder")==0) return base+7;
    if (fieldName[0]=='B' && strcmp(fieldName, "BSN")==0) return base+8;
    if (fieldName[0]=='C' && strcmp(fieldName, "CAPlength")==0) return base+9;
    if (fieldName[0]=='G' && strcmp(fieldName, "GTSlength")==0) return base+10;
    if (fieldName[0]=='G' && strcmp(fieldName, "GTSlist")==0) return base+11;
    if (fieldName[0]=='v' && strcmp(fieldName, "vizinhosOuNodosCooperantes")==0) return base+12;
    if (fieldName[0]=='s' && strcmp(fieldName, "somaSinais")==0) return base+13;
    if (fieldName[0]=='e' && strcmp(fieldName, "energy")==0) return base+14;
    if (fieldName[0]=='d' && strcmp(fieldName, "dadosVizinho")==0) return base+15;
    if (fieldName[0]=='s' && strcmp(fieldName, "slotInicioRetrans")==0) return base+16;
    if (fieldName[0]=='r' && strcmp(fieldName, "retransmissao")==0) return base+17;
    if (fieldName[0]=='t' && strcmp(fieldName, "tempoBeacon")==0) return base+18;
    if (fieldName[0]=='t' && strcmp(fieldName, "tempAtualizVizinhanca")==0) return base+19;
    if (fieldName[0]=='p' && strcmp(fieldName, "primeiraSelecao")==0) return base+20;
    if (fieldName[0]=='i' && strcmp(fieldName, "idBeacon")==0) return base+21;
    if (fieldName[0]=='p' && strcmp(fieldName, "payload")==0) return base+22;
    if (fieldName[0]=='c' && strcmp(fieldName, "coeficiente")==0) return base+23;
    if (fieldName[0]=='g' && strcmp(fieldName, "gack")==0) return base+24;
    if (fieldName[0]=='c' && strcmp(fieldName, "coopAuxiliares")==0) return base+25;
    if (fieldName[0]=='n' && strcmp(fieldName, "numSlotPerCoop")==0) return base+26;
    if (fieldName[0]=='i' && strcmp(fieldName, "idNodoRetransmitido")==0) return base+27;
    if (fieldName[0]=='v' && strcmp(fieldName, "vetBit")==0) return base+28;
    if (fieldName[0]=='d' && strcmp(fieldName, "dadosRetrans")==0) return base+29;
    return basedesc ? basedesc->findField(object, fieldName) : -1;
}

const char *Basic802154PacketDescriptor::getFieldTypeString(void *object, int field) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldTypeString(object, field);
        field -= basedesc->getFieldCount(object);
    }
    static const char *fieldTypeStrings[] = {
        "int",
        "int",
        "int",
        "int",
        "int",
        "bool",
        "int",
        "int",
        "int",
        "int",
        "int",
        "Basic802154GTSspec",
        "short",
        "double",
        "double",
        "MENSAGENS_ESCUTADAS",
        "short",
        "bool",
        "short",
        "bool",
        "short",
        "short",
        "unsigned short",
        "unsigned short",
        "unsigned short",
        "short",
        "short",
        "short",
        "uint64_t",
        "DADOS_RETRANS_GACK",
    };
    return (field>=0 && field<30) ? fieldTypeStrings[field] : NULL;
}

const char *Basic802154PacketDescriptor::getFieldProperty(void *object, int field, const char *propertyname) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldProperty(object, field, propertyname);
        field -= basedesc->getFieldCount(object);
    }
    switch (field) {
        case 0:
            if (!strcmp(propertyname,"enum")) return "Mac802154Packet_type";
            return NULL;
        default: return NULL;
    }
}

int Basic802154PacketDescriptor::getArraySize(void *object, int field) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getArraySize(object, field);
        field -= basedesc->getFieldCount(object);
    }
    Basic802154Packet *pp = (Basic802154Packet *)object; (void)pp;
    switch (field) {
        case 11: return pp->getGTSlistArraySize();
        case 12: return pp->getVizinhosOuNodosCooperantesArraySize();
        case 15: return pp->getDadosVizinhoArraySize();
        case 22: return pp->getPayloadArraySize();
        case 23: return pp->getCoeficienteArraySize();
        case 24: return pp->getGackArraySize();
        case 25: return pp->getCoopAuxiliaresArraySize();
        case 26: return pp->getNumSlotPerCoopArraySize();
        case 28: return pp->getVetBitArraySize();
        case 29: return pp->getDadosRetransArraySize();
        default: return 0;
    }
}

std::string Basic802154PacketDescriptor::getFieldAsString(void *object, int field, int i) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldAsString(object,field,i);
        field -= basedesc->getFieldCount(object);
    }
    Basic802154Packet *pp = (Basic802154Packet *)object; (void)pp;
    switch (field) {
        case 0: return long2string(pp->getMac802154PacketType());
        case 1: return long2string(pp->getPANid());
        case 2: return long2string(pp->getSrcID());
        case 3: return long2string(pp->getDstID());
        case 4: return long2string(pp->getSeqNum());
        case 5: return bool2string(pp->getPauseNodo());
        case 6: return long2string(pp->getBeaconOrder());
        case 7: return long2string(pp->getFrameOrder());
        case 8: return long2string(pp->getBSN());
        case 9: return long2string(pp->getCAPlength());
        case 10: return long2string(pp->getGTSlength());
        case 11: {std::stringstream out; out << pp->getGTSlist(i); return out.str();}
        case 12: return long2string(pp->getVizinhosOuNodosCooperantes(i));
        case 13: return double2string(pp->getSomaSinais());
        case 14: return double2string(pp->getEnergy());
        case 15: {std::stringstream out; out << pp->getDadosVizinho(i); return out.str();}
        case 16: return long2string(pp->getSlotInicioRetrans());
        case 17: return bool2string(pp->getRetransmissao());
        case 18: return long2string(pp->getTempoBeacon());
        case 19: return bool2string(pp->getTempAtualizVizinhanca());
        case 20: return long2string(pp->getPrimeiraSelecao());
        case 21: return long2string(pp->getIdBeacon());
        case 22: return ulong2string(pp->getPayload(i));
        case 23: return ulong2string(pp->getCoeficiente(i));
        case 24: return ulong2string(pp->getGack(i));
        case 25: return long2string(pp->getCoopAuxiliares(i));
        case 26: return long2string(pp->getNumSlotPerCoop(i));
        case 27: return long2string(pp->getIdNodoRetransmitido());
        case 28: return uint642string(pp->getVetBit(i));
        case 29: {std::stringstream out; out << pp->getDadosRetrans(i); return out.str();}
        default: return "";
    }
}

bool Basic802154PacketDescriptor::setFieldAsString(void *object, int field, int i, const char *value) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->setFieldAsString(object,field,i,value);
        field -= basedesc->getFieldCount(object);
    }
    Basic802154Packet *pp = (Basic802154Packet *)object; (void)pp;
    switch (field) {
        case 0: pp->setMac802154PacketType(string2long(value)); return true;
        case 1: pp->setPANid(string2long(value)); return true;
        case 2: pp->setSrcID(string2long(value)); return true;
        case 3: pp->setDstID(string2long(value)); return true;
        case 4: pp->setSeqNum(string2long(value)); return true;
        case 5: pp->setPauseNodo(string2bool(value)); return true;
        case 6: pp->setBeaconOrder(string2long(value)); return true;
        case 7: pp->setFrameOrder(string2long(value)); return true;
        case 8: pp->setBSN(string2long(value)); return true;
        case 9: pp->setCAPlength(string2long(value)); return true;
        case 10: pp->setGTSlength(string2long(value)); return true;
        case 12: pp->setVizinhosOuNodosCooperantes(i,string2long(value)); return true;
        case 13: pp->setSomaSinais(string2double(value)); return true;
        case 14: pp->setEnergy(string2double(value)); return true;
        case 16: pp->setSlotInicioRetrans(string2long(value)); return true;
        case 17: pp->setRetransmissao(string2bool(value)); return true;
        case 18: pp->setTempoBeacon(string2long(value)); return true;
        case 19: pp->setTempAtualizVizinhanca(string2bool(value)); return true;
        case 20: pp->setPrimeiraSelecao(string2long(value)); return true;
        case 21: pp->setIdBeacon(string2long(value)); return true;
        case 22: pp->setPayload(i,string2ulong(value)); return true;
        case 23: pp->setCoeficiente(i,string2ulong(value)); return true;
        case 24: pp->setGack(i,string2ulong(value)); return true;
        case 25: pp->setCoopAuxiliares(i,string2long(value)); return true;
        case 26: pp->setNumSlotPerCoop(i,string2long(value)); return true;
        case 27: pp->setIdNodoRetransmitido(string2long(value)); return true;
        case 28: pp->setVetBit(i,string2uint64(value)); return true;
        default: return false;
    }
}

const char *Basic802154PacketDescriptor::getFieldStructName(void *object, int field) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldStructName(object, field);
        field -= basedesc->getFieldCount(object);
    }
    switch (field) {
        case 11: return opp_typename(typeid(Basic802154GTSspec));
        case 15: return opp_typename(typeid(MENSAGENS_ESCUTADAS));
        case 29: return opp_typename(typeid(DADOS_RETRANS_GACK));
        default: return NULL;
    };
}

void *Basic802154PacketDescriptor::getFieldStructPointer(void *object, int field, int i) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldStructPointer(object, field, i);
        field -= basedesc->getFieldCount(object);
    }
    Basic802154Packet *pp = (Basic802154Packet *)object; (void)pp;
    switch (field) {
        case 11: return (void *)(&pp->getGTSlist(i)); break;
        case 15: return (void *)(&pp->getDadosVizinho(i)); break;
        case 29: return (void *)(&pp->getDadosRetrans(i)); break;
        default: return NULL;
    }
}


