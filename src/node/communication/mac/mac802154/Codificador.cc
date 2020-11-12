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

#include "Codificador.h"

//Número de nodos maximo (incluindo o coordenador).
#define N_NODES 101
#define MSG_SIZE 127
#define NO_COEFFICIENT        0x0000
#define MULTIPLE_COEFFICIENTS 0xFFFF



Define_Module(Codificador);

Codificador::Codificador() {
    // TODO Auto-generated constructor stub

}

Codificador::~Codificador() {
    // TODO Auto-generated destructor stub
}



/****************************************************************************************
                            FUNÇÕES DE CODIFICAÇÃO A 8 BITS
****************************************************************************************/
/*
*   Multiplicação
*/
uint8_t Codificador::mult(uint8_t a, uint8_t b) //multiplicacao de 2 número num corpo de 8 bits
{
    int8_t i;
    int16_t  result = 0;

    for (i = 0; i < 8; i++)
        if ((a >> i) & 1) result ^= ((int16_t ) b) << i;

    for (i = 6; i >= 0; i--)
        if (result & (0x100 << i)) result ^= (0x1A9 << i);

    return result;
}

/*
*   Inverso multiplicativo
*/
unsigned char Codificador::inv(unsigned char a) //inverso  do mult
{
    char i, j;
    unsigned short result, r[8], p;

    if (a == 0) return 0;

    for (i = 0; i < 8; i++)
    {
        r[i] = ((unsigned short) a) << i;
        for (j = 6; j >= 0; j--)
            if (r[i] & (0x100 << j)) r[i] ^= (0x1A9 << j);
    }

    for (result = 0; result < 0x100; result++)
    {
        p = 0;
        for (i = 0; i < 8; i++)
            if (result & (1 << i)) p ^= r[i];

        if (p == 1) return result;
    }

    return 0;
}

/****************************************************************************************
                        FUNÇÕES PARA DECIFRAR AS MENSAGENS CODIFICADAS
****************************************************************************************/


/*
*   Troca duas linhas das matrizes de coeficientes (matrix) e da matriz de mensagens recebidas com codificação (combination1).
*/
void Codificador::swapLines( uint8_t c1, uint8_t c2){

    uint8_t n, b, e1, e2;

    for (n = 1; n < N_NODES; n++){
        e1 = matrix[c1][n];
        e2 = matrix[c2][n];

        matrix[c1][n] = e2;
        matrix[c2][n] = e1;
    }

    for (b = 0; b < MSG_SIZE; b++){
        e1 = combination1[c1][b];
        e2 = combination1[c2][b];

        combination1[c1][b] = e2;
        combination1[c2][b] = e1;
    }
}

/*
*   Multiplica a linha c1 das matrizes de coeficientes (matrix) e da matriz de mensagens recebidas com codificação (combination1)
*   pelo fator f, e subtrai o resultado
*   da linha c2, sobrescrevendo-a.
*/
void Codificador:: combineLines(uint8_t c1, uint8_t c2, uint8_t f){
    uint8_t n, b, e1, e2, auxMul, auxSub;

    for (n = 1; n < N_NODES; n++){
        e1 = matrix[c1][n];
        e2 = matrix[c2][n];

        auxMul = mult(f, e1);
        auxSub = e2 ^ auxMul;
        e2 = auxSub;
        matrix[c2][n] = e2;
    }

    for (b = 0; b < MSG_SIZE; b++){
        e1 = combination1[c1][b];
        e2 = combination1[c2][b];

        auxMul = mult(f, e1);
        auxSub = e2 ^ auxMul;
        e2 = auxSub;
        combination1[c2][b] = e2;
    }
}
/*
*   Combina as linhas da matriz de coeficientes (matrix) e da matriz de mensagens recebidas com codificação (combination1)
*   que estão abaixo de cp, de forma a zerar a coluna n.
*/
void Codificador:: clearColumn(uint8_t cp, uint8_t n){

    uint8_t c, kpinv, k, f;

    kpinv = inv(matrix[cp][n]);

    for (c = cp + 1; c < N_NODES; c++){
        k = matrix[c][n];

        if (k != 0)
        {
            f = mult(kpinv, k);
            combineLines(cp, c, f);
        }
    }
}
/*
*   Retira da matriz de coeficientes (matrix) e da matriz de mensagens recebidas com codificação (combination1)
*   as informações sobre a mensagem de um nodo.
*/
void Codificador::subtractMsg(int idNode){

    uint8_t c, k, b, m, r, aux = 0;

        for (c = 1; c < N_NODES; c++)
        {
            k = matrix[c][idNode];
            if (k != 0){
                for (b = 0; b < MSG_SIZE; b++){
                    m = buffer_msg[idNode][b];
                    r = combination1[c][b];
                    aux = mult(k,m);
                    r =  r ^ aux;
                    combination1[c][b] = r;
                }
                matrix[c][idNode] = 0;

            }
        }
}
/*
*   Remove das matrizes de coeficientes (matrix) e da matriz de mensagens recebidas com codificação (combination1)
*   as informações sobre as mensagens já conhecidas.
*/
void Codificador::removeKnownElements(){
    uint8_t n;

        for (n = 1; n < N_NODES; n++)
        {
            if (received[n]){
                subtractMsg(n);
            }
        }
}
/*
*   Faz o escalonamento da matriz de coeficiente.
*/
void Codificador::reduceMatrix(){
    uint8_t cp = 1;
    uint8_t n, c, k;

    for (n = 1; n < N_NODES; n++){
        k = matrix[cp][n];

        if (k == 0){
            for (c = cp + 1; c < N_NODES; c++){
                k = matrix[c][n];

                if (k != 0){
                    swapLines(c, cp);
                    break;
                }
            }
        }

        if (k != 0){
            clearColumn(cp, n);
            cp++;
        }
    }
}
/*
*   Retorna a coluna em que está o único coeficiente de uma linha da matriz de coeficiente.
*   Caso não encontre nenhum, retorna NO_COEFFICIENT. Caso encontre mais de um,
*   retorna MULTIPLE_COEFFICIENTS.
*/
int Codificador:: getCoefficientCol(uint8_t c){
    uint16_t u = NO_COEFFICIENT;
    uint8_t  n, k;

    for (n = 1; n < N_NODES; n++){
        k = matrix[c][n];

        if (k != 0){
            if (u == NO_COEFFICIENT) u = n;
            else return MULTIPLE_COEFFICIENTS;
        }
    }

    return u;
}
/*
*   Faz a retrossubstituição a partir da matriz escalonada, obtendo as mensagens
*   faltantes que forem possíveis de ser recuperadas. Retorna o número de
*   mensagens decodificadas com sucesso.
*/
int Codificador:: substituteBack(){
    // matrix = matriz que armazena os coeficientes
    // buffer_msg = matriz que armazena as mensagens que chegaram sem codificação
    //combination1 = matriz que armazena as mensagems codificadas

    uint8_t  count = 0;
    uint8_t  c, b, r, m, kinv, auxConv;
    uint16_t n;
    int auxPrint = 0;

    for (c = N_NODES-1; c > 0; c--)
    {
        n = getCoefficientCol(c);

        if      (n == MULTIPLE_COEFFICIENTS) break;
        else if (n != NO_COEFFICIENT){
            auxConv = (uint8_t) n;
            kinv = inv(matrix[c][auxConv]);
            std::cout<< "kinv: " << (int)kinv <<"\n";

            for (b = 0; b < MSG_SIZE; b++){
                r = combination1[c][b];
                m = mult(r,kinv);
                buffer_msg[auxConv][b] = m;

                auxPrint = (int)m;
                std::cout<< "buffer_msg["<< (int)auxConv<<"]["<<(int)b<< "]: " << auxPrint  <<"\n";
            }
            received[n] = 1;
            // recoverPerNode[auxConv]: armazena a quantidade de mensagens recuperadas de determinado nodo, no geral da simulação
            recoverPerNode[auxConv]++;
            subtractMsg((uint8_t) n);
            count++;
        }
    }
    for(int rec = 0 ;rec < MSG_SIZE;rec++){
        //std::cout<< buffer_msg[auxConv][rec] << "\n";
        std::cout<< "buffer_msg["<< (int)auxConv<<"]["<<(int)rec<< "]: " << (int)buffer_msg[auxConv][rec]  <<"\n";
    }

    return count;
}

/*Resolve a codificação*/
int Codificador::solveSystem(int sucesso){
    int recuperadas = 0;

    removeKnownElements();
    reduceMatrix();
    recuperadas = substituteBack();
    //return recuperadas + sucesso;
    return recuperadas;

}

/*
int Codificador::solve_system(int sucesso)//resolve o escalonamento
{

    // buffer_msg = estrutura: linha: endereco do nodo,coluna: os bytes da msg.

    short i, j, k, notrec_mote, cont, row;
    //Define a listagem de vizinhanca (mensagens recebidas) de determinado nodo cooperante. É uma lista de bits baseada na posicão da transmissão, se o
    //o nodo recebeu a primeira mensagem da tranmissão seta o bit para 1, se não, seta para zero.

    //RETIRA DA MATRIZ OS ELEMENTOS JÁ CONHECIDOS
    //for (j = 1; j <= N_MOTES; j++)

    for (j = 1; j < N_MOTES; j++)
    {
        if (received[j]){
            //EV << "[Conferencia] retirando elemento conhecido da matriz: " << j << endl;
            for (i = 0; i < n_equations; i++)
                if (matrix[i][j]>0)
                {
                    for (k = 0; k < MSG_SIZE; k++)
                        combination1[i][k] ^= mult(matrix[i][j], buffer_msg[j][k]); //matrix[i][j] = coeficientes: linha = coeficiente, coluna = endereco do nodo.
                    matrix[i][j] = 0;
                }
        }
    }


    //FAZ O ESCALONAMENTO DA MATRIZ
    for (j = 1, row = 0; j < N_MOTES; j++)
    {
        i = row;
        if (matrix[i][j]>0)
        {
            clear_column(row, j);
            row++;
        }
        else
        {
            for (i++; i < n_equations; i++)
                if (matrix[i][j]>0)
                {
                    swap_line(row, i);
                    clear_column(row, j);
                    row++;
                    break;
                }
        }
    }
    int rec_mote_index = 0;
    controle_rec_motes = 0;
    //ENCONTRA AS INCÓGNITAS POSSÍVEIS DE DECIFRAR
    for (i = n_equations - 1; i >= 0; i--)
    {
        cont = 0;
        for (j = 1; j < N_MOTES; j++)
            if (matrix[i][j]>0)
            {
                cont++;
                notrec_mote = j;
            }

        if (cont == 0) n_equations--;
        else if (cont == 1)         //RESOLVE A INCÓGNITA ENCONTRADA
        {
            for (k = 0; k < MSG_SIZE; k++) buffer_msg[notrec_mote][k] = mult(inv(matrix[i][notrec_mote]), combination1[i][k]);
            //EV << "[Conferencia] mensagem decodificada : " << notrec_mote << endl;
            //Imprime a mensagem recuperada

            std::cout<<"Mensagem recuperada do nodo";
            std::cout<< notrec_mote;
            for(int m = 0; m < MSG_SIZE;m++){
                std::cout<<"buffer_msg["<<notrec_mote<<"]["<<m<<"]: "<< buffer_msg[notrec_mote][m]<< endl;
            }

            received[notrec_mote] = 1;
            rec_motes[rec_mote_index] = notrec_mote;
            rec_mote_index++;
            controle_rec_motes++;
            n_equations--;
            sucesso++;  // Aqui deve-se contabilizar o sucesso na decodificacao das MSG.

            for (i = 0; i < n_equations; i++)
                if (matrix[i][notrec_mote]>0)
                {
                    for (k = 0; k < MSG_SIZE; k++){
                        combination1[i][k] ^= mult(matrix[i][notrec_mote], buffer_msg[notrec_mote][k]);
                        std::cout<<"combination1["<<i<<"]["<<k<<"]: "<< buffer_msg[i][k]<< endl;
                    }
                    matrix[i][notrec_mote] = 0;
                }
        }
        else break;
    }
    return sucesso;
}
*/

/****************************************************************************************
                CODIFICAÇÃO E ENVIO DE RETRANSMISSÃO
****************************************************************************************/

void Codificador::encode_messages(int idCooperante) //codifica, constroi e transmite mensagens
{
    // primeiro: para cada vizinho no mapa, codificar a mesg byte a byte com a funcao mult.
    // Os parametros sao id do nodo cooperante + endereco do vizinho.
    //Cada vizinho tera a sua msg codificada no formato de um vetor de byte.
    //segundo: fazer o xor de todas as msgs codificadas e retornar esse valor.


    for(int i=0;i<MSG_SIZE;i++){
        msg_array[i] = 0;
    }

    for(int i=0;i<N_NODES;i++){
         coeficientes[i] = 0;
    }

    uint8_t coef,r;
    uint16_t byte;
    coef = 0;
    r = 0;
    std::map<int, MessagesNeighborhood*>::iterator iter1;
          if(mapacodificador.size() > 0){
              for( iter1 = mapacodificador.begin(); iter1 != mapacodificador.end(); iter1++ ){
                   MessagesNeighborhood *neig = iter1->second;

                   if(neig->frametrans->getPayloadArraySize() > 0){

                       Basic802154Packet *frame = neig->frametrans;
                       coef = (iter1->first + idCooperante) % (N_NODES-1); // tem que colocar o módulo
                       coeficientes[iter1->first] = coef;


                       std::cout<<"Mensagem antes da codi"<<endl;
                       for(int i = 0; i < MSG_SIZE; i++){
                           std::cout<< (int)frame->getPayload(i)<<endl;
                       }

                       /*
                        * Pega byte a byte da mensagem, codifica com o coeficiente,
                        * depois codifica com as mensagens existentes e assim uma única
                        * mensagem de retransmissão é enviada.
                        * */
                       for(int i = 0; i < MSG_SIZE; i++){
                           byte = frame->getPayload(i);
                           r = msg_array[i];
                           byte = mult(coef, byte);
                           msg_array[i] = r ^ byte;
                           std::cout<<(int)msg_array[i] <<endl;
                       }

                       /*std::cout<<"Mensagem depois da codi"<<endl;
                       for(int i = 0; i < MSG_SIZE; i++){
                            std::cout<<msg_array[i] <<endl;
                       }*/
                   }

              }
          }
}
