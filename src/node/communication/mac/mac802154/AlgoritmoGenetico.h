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

#ifndef ALGORITMOGENETICO_H_
#define ALGORITMOGENETICO_H_

using namespace std;

#include <map>
#include <vector>
#include <iostream>
#include <time.h>
#include <stdlib.h>
#include <math.h>
#include "Populacao.h"
#include "Util.h"
#include "Cromossomo.h"
#include "Basic802154.h"

class AlgoritmoGenetico {
public:
    int TAM_POPULACAO = 500;
    double TAXA_MIN_MUTACAO = 0.1;
    int NUM_GERACOES = 500;
    static const int QTD_TORNEIO = 2;

    std::map<int, vector<int>> listaLinha;
    std::map<int, vector<int>> listaColuna;
    std::vector<double> listaCusto;
    std::vector<int> nodoscooperantes;

    Populacao *populacao;

public:
    virtual ~AlgoritmoGenetico(){
        delete populacao;
    }

    AlgoritmoGenetico(int nLinha, int nColuna, int tam_populacao, double taxa_min_mutacao, int num_geracoes);

    virtual void addCusto(int coluna, double custo);

    virtual void addDados(int coluna, int linha);

    virtual void executar();

    // Seleção de individuos por torneio
    virtual Cromossomo *selecao();

    virtual Cromossomo *crossover();

    virtual void mutacao(Cromossomo *C);

    virtual double taxaMutacao(double custoMaisApto, double custoMenosApto);

};

#endif /* ALGORITMOGENETICO_H_ */
