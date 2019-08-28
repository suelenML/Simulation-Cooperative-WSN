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

#ifndef POPULACAO_H_
#define POPULACAO_H_

using namespace std;

#include <map>
#include <vector>
#include "Cromossomo.h"

class Populacao {
public:
    virtual ~Populacao();
    Populacao(int tam_populacao);

    virtual std::vector<Cromossomo*> getPopulacao();
    virtual void  gerarPopulacaoInicial(map <int, vector<int>> &listaLinha, map <int, vector<int>> &listaColuna, vector<double> &listaCusto);
    virtual void atualizar(Cromossomo *novo);
    virtual Cromossomo *maisApto();
    virtual Cromossomo *menosApto();

    int tam_populacao;
    std::vector<Cromossomo*> populacao;

    int posMaisApto = 0;
    int posMenosApto = 0;

    void classifica(int index);

};

#endif /* POPULACAO_H_ */
