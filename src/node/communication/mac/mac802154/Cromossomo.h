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

#ifndef CROMOSSOMO_H_
#define CROMOSSOMO_H_


using namespace std;

#include <map>
#include <vector>
#include <algorithm>
#include "Util.h"

class Cromossomo {
public:
    Cromossomo();
    Cromossomo(vector<int>  &colunas, map <int, vector<int>> &listaColuna, map <int, vector<int>> &listaLinha,  vector<double> &listaCusto);
    virtual ~Cromossomo();
    virtual void addColuna(int coluna, double custo, map <int, vector<int>> &listaColuna);
    virtual void removeColuna(int coluna, vector<double> &listaCusto);
    virtual std::vector<int> getColunas();
    virtual double getCustoTotal();
    virtual void gerarIndividuo(map <int, vector<int>> &listaLinha, map <int, vector<int>> &listaColuna,  vector<double> &listaCusto);
    virtual void eliminaRedundancia(map <int, vector<int>> &listaColuna, vector<double> &listaCusto);
    virtual void removeAll(vector<int> &linhasDescobertas, vector<int> &aux2);

    std::vector<int>  colunas;
    double custoTotal = 0;
    std::vector<int> qtdColunaCobreLinha;
    static int colunaMinimizaCusto(std::vector<int> &conjuntoColuna, std::vector<int> &linhasDescobertas, map <int, vector<int>> &listaColuna, vector<double> &listaCusto);
    virtual bool isRedundante(std::vector<int> &conjuntoLinha);// verifica se os elementos de uma linha X sao cobertos por mais de uma coluna
};

#endif /* CROMOSSOMO_H_ */
