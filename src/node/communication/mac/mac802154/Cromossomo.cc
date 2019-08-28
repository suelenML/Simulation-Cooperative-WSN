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

#include "Cromossomo.h"
#include "Util.h"


Cromossomo::Cromossomo(){
        colunas.clear();
        custoTotal = 0;
}

Cromossomo::Cromossomo(vector<int>  &colunas, map<int, vector<int>> &listaColuna, map<int, vector<int>> &listaLinha,  vector<double> &listaCusto){
    Cromossomo();
    qtdColunaCobreLinha = std::vector<int>(listaLinha.size());
        for(int i = 0; i < (int)colunas.size(); i++){
            addColuna(colunas[i], listaCusto[colunas[i]], listaColuna);
        }
}

void Cromossomo::addColuna(int coluna, double custo, map <int, vector<int>> &listaColuna){
        if (std::find(colunas.begin(), colunas.end(), coluna) != colunas.end()){
            return;
        }
        colunas.push_back(coluna);
        custoTotal += custo;

        std::map<int,vector<int>>::iterator it;
        it = listaColuna.find(coluna);
        std::vector<int> aux = it->second;
        for(int i = 0; i < (int)aux.size();i++){
            qtdColunaCobreLinha[aux[i]]++;
        }
}

void Cromossomo::removeColuna(int coluna, vector<double> &listaCusto){
        colunas.erase(colunas.begin()+(coluna-1));
        custoTotal = custoTotal - listaCusto[coluna];
}

std::vector<int> Cromossomo::getColunas()   {
        return colunas;
}

double Cromossomo::getCustoTotal()  {
        return custoTotal;
}

void Cromossomo::gerarIndividuo(map <int, vector<int>> &listaLinha, map <int, vector<int>> &listaColuna,  vector<double> &listaCusto){
        std::vector<int> linhasDescobertas;
        std::vector<int> conjuntoColuna;
        conjuntoColuna.reserve(listaLinha.size());
        qtdColunaCobreLinha.clear();

        std::map<int,vector<int>>::iterator it;
        std::vector<int> aux;
        std::map<int,vector<int>>::iterator it2;
        std::vector<int> aux2;

        for (int i = 0; i < (int)listaLinha.size(); i++){
            linhasDescobertas.push_back(i);
        }

        qtdColunaCobreLinha = std::vector<int>(listaLinha.size());

        while (!linhasDescobertas.empty()){
            aux.clear();
            conjuntoColuna.clear();
            aux2.clear();
            int random_pos = getRandomInt(linhasDescobertas.size());
            int linha = linhasDescobertas[random_pos];

            it = listaLinha.find(linha);

            if(it != listaLinha.end()){

                aux = it->second;
                for(int i = 0; i < (int)aux.size();i++){ // ver se tenho que iniciar no zero ou no 1
                    conjuntoColuna.push_back(aux[i]);
                }


                int menorColuna = colunaMinimizaCusto(conjuntoColuna, linhasDescobertas, listaColuna, listaCusto);

                this->addColuna(menorColuna, listaCusto[menorColuna], listaColuna);
                //linhasDescobertas.removeAll(listaColuna[menorColuna]);
                it2 = listaColuna.find(menorColuna);
                aux2 = it2->second;
                removeAll(linhasDescobertas, aux2);
            }else{
                if((int)linhasDescobertas.size() == 1){
                    linhasDescobertas.erase (linhasDescobertas.begin());
                }
            }
        }


}

void Cromossomo::removeAll(vector<int> &linhasDescobertas, vector<int> &aux2){

    if(linhasDescobertas[0] == 0){
        linhasDescobertas.erase (linhasDescobertas.begin());
    }


    for(int i = 0; i < (int)aux2.size();i++){
            for(int j = 0; j < (int)linhasDescobertas.size(); j++){
                if(aux2[i] == linhasDescobertas[j]){
                    linhasDescobertas.erase (linhasDescobertas.begin()+j);
                    break;
                }
            }

    }

}
int Cromossomo::colunaMinimizaCusto(std::vector<int> &conjuntoColuna, std::vector<int> &linhasDescobertas, map <int, vector<int>> &listaColuna, vector<double> &listaCusto){
        double menor = std::numeric_limits<double>::max();
        std::map<int,vector<int>>::iterator it;
        int menorColuna = -1;

        for (int i = 0; i < (int)conjuntoColuna.size(); i++){ // ver se tenho que iniciar no zero ou no 1
            int coluna = conjuntoColuna[i];
            double custo = listaCusto[coluna];

            it = listaColuna.find(coluna);
            std::vector<int> aux = it->second; // declarar fora dop for
            int intersecao_size = Util::intersecao(linhasDescobertas, aux).size();
            if ((custo / intersecao_size) < menor){
                menor = custo / intersecao_size;
                menorColuna = coluna;
            }
        }
        return menorColuna;
}

void Cromossomo::eliminaRedundancia(map <int, vector<int>> &listaColuna, vector<double> &listaCusto){
            std::map<int,vector<int>>::iterator it2;
            std::vector<int> aux2;

            std::vector<int> T;
            T = std::vector<int>(this->colunas);
            while (!T.empty()){
                int random_pos = getRandomInt(T.size());
                int coluna = T[random_pos];
                T.erase(T.begin() + random_pos);

                it2 = listaColuna.find(coluna);
                aux2 = it2->second;

                if (isRedundante(aux2)){
                    removeColuna(coluna, listaCusto);

                    for(int i = 0; i < (int)aux2.size();i++){ // ver se tenho que iniciar no zero ou no 1
                        qtdColunaCobreLinha[aux2[i]]--;
                    }
                }
            }
}


bool Cromossomo::isRedundante(std::vector<int> &conjuntoLinha){
    for(int i = 0; i < (int)conjuntoLinha.size();i++){
        if (qtdColunaCobreLinha[conjuntoLinha[i]] < 2){
            return false;
        }
    }

    return true;
}

Cromossomo::~Cromossomo() {
    // TODO Auto-generated destructor stub
}

