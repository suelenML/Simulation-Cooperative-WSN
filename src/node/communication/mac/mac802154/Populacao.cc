/*
 * Populacao.cpp
 *
 *  Created on: 22 de ago de 2019
 *      Author: Suelen
 */

#include "Populacao.h"


Populacao::Populacao(int tam_populacao){
    this->tam_populacao = tam_populacao;
    populacao = std::vector<Cromossomo*>(tam_populacao);
    posMaisApto = -1;
    posMenosApto = -1;
}

std::vector<Cromossomo*> Populacao::getPopulacao(){
        return populacao;
}

void Populacao:: gerarPopulacaoInicial(map <int, vector<int>> &listaLinha, map <int, vector<int>> &listaColuna, vector<double> &listaCusto){
    for (int i = 0; i < tam_populacao; i++){
        Cromossomo *c = new Cromossomo();
        c->gerarIndividuo(listaLinha, listaColuna, listaCusto);
        c->eliminaRedundancia(listaColuna, listaCusto);
        populacao[i] = c;
        classifica(i);

        //delete c; // testar essa parte
    }
}

void Populacao::classifica(int index){
        Cromossomo *c = populacao[index];
        if (posMaisApto == -1 || c->getCustoTotal() < populacao[posMaisApto]->getCustoTotal()){
            posMaisApto = index;
        }
        if (posMenosApto == -1 || c->getCustoTotal() > populacao[posMenosApto]->getCustoTotal()){
            posMenosApto = index;
        }
}

void Populacao::atualizar(Cromossomo *novo){
        populacao[posMenosApto] = novo;
        for (int i = 0; i < (int)populacao.size(); i++){
            classifica(i);
        }
}

Cromossomo *Populacao::maisApto()   {
        return populacao[posMaisApto];
}

Cromossomo *Populacao::menosApto()  {
        return populacao[posMenosApto];
}

Populacao::~Populacao() {
    // TODO Auto-generated destructor stub
}
