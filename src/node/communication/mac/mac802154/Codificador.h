//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with this program.  If not, see http://www.gnu.org/licenses/.
// 

#include  "Neighborhood.h"
#include "MessagesNeighborhood.h"
#include <string.h>
#include <stdint.h>

#ifndef CODIFICADOR_H_
#define CODIFICADOR_H_


class Codificador : public cSimpleModule{
public:
    Codificador();
    virtual ~Codificador();


    unsigned char mult(unsigned char a, unsigned char b);
    unsigned char inv(unsigned char a);
    void encode_messages(int);

    void removeKnownElements();
    void swapLines( uint8_t c1, uint8_t c2);
    void combineLines(uint8_t c1, uint8_t c2, uint8_t f);
    void clearColumn(uint8_t cp, uint8_t n);
    void subtractMsg(int idNode);
    void reduceMatrix();
    int getCoefficientCol(uint8_t c);
    int substituteBack();
    int solveSystem(int sucesso);



   std::map<int, MessagesNeighborhood*> mapacodificador;

   uint8_t buffer_msg[21][127];/*matriz de armazenamento de msg sem codificação*/
   uint8_t matrix[21][21];/*matriz de coeficiente*/
   /*unsigned short*/ uint8_t combination1[21][127];/*mensagens retransmitidas recebidas (codificadas)*/
   uint8_t msg_array[127]; /*matriz que armazena as mensagens codificadas */
   uint8_t coeficientes[21];
   //int coeficientes[21];
   unsigned short received[21]; /* vetor que sinaliza de quem o coordenador escutou*/
   int n_equations;
   short rec_motes[21];
   int controle_rec_motes;

};

#endif /* CODIFICADOR_H_ */
