#include "Cromossomo.h"
#include "Util.h"


Cromossomo::Cromossomo(){
		colunas.clear();
		custoTotal = 0;
}

Cromossomo::Cromossomo(vector<int>  &colunas, map<int, vector<int>> &listaColuna, map<int, vector<int>> &listaLinha,  vector<double> &listaCusto){
	Cromossomo();
	int tamVector = 0;
	//qtdColunaCobreLinha = std::vector<int>(listaLinha.size());
	qtdColunaCobreLinha = std::vector<int>(listaColuna.size());
	tamVector = (int)colunas.size();
	    for(int i = 0; i < tamVector; i++){
			addColuna(colunas[i], listaCusto[colunas[i]], listaColuna);
		}
}

void Cromossomo::addColuna(int coluna, double custo, map <int, vector<int>> &listaColuna){
		if (std::find(colunas.begin(), colunas.end(), coluna) != colunas.end()){
			return;
		}
		colunas.push_back(coluna);
		custoTotal += custo;
		vector<int> aux;
		aux.clear();

		std::map<int,vector<int>>::iterator it;
		it = listaColuna.find(coluna);
		if(it != listaColuna.end()){
		    aux = it->second;
            for(int i = 0; i < (int)aux.size();i++){
                qtdColunaCobreLinha[aux[i]]++;
            }
        }
}

void Cromossomo::removeColuna(int coluna, vector<double> &listaCusto){
        std::vector<int>::iterator  it;
        int pos = 0;
        it = find(colunas.begin(),colunas.end(), coluna);
        if(it!=colunas.end()){
            pos = it - colunas.begin();
            colunas.erase(it);
            custoTotal = custoTotal - listaCusto[pos];
        }

}

std::vector<int> Cromossomo::getColunas()	{
		return colunas;
}

double Cromossomo::getCustoTotal()	{
		return custoTotal;
}

void Cromossomo::gerarIndividuo(map <int, vector<int>> &listaLinha, map <int, vector<int>> &listaColuna,  vector<double> &listaCusto){
		std::vector<int> linhasDescobertas;
		std::vector<int> conjuntoColuna;
		//conjuntoColuna.reserve(200000);
		qtdColunaCobreLinha.clear();
		linhasDescobertas.clear();

        std::map<int,vector<int>>::iterator it;
        std::vector<int> aux;
        std::map<int,vector<int>>::iterator it2;
        std::vector<int> aux2;

		for (int i = 0; i < (int)listaColuna.size(); i++){//varro todos os elementos (listaColuna contém todos os elementos)
		    it = listaLinha.find(i);// pesquiso por quais colunas cada elemento é coberto
		    if(it != listaLinha.end()){
		        linhasDescobertas.push_back(it->first);
            }
		}

		//qtdColunaCobreLinha = std::vector<int>(listaLinha.size());
		qtdColunaCobreLinha = std::vector<int>(listaColuna.size());

		while (!linhasDescobertas.empty()){
            aux.clear();
            conjuntoColuna.clear();
            aux2.clear();
			//int random_pos = getRandomInt(linhasDescobertas.size());
			//srand(time(NULL));
            int random_pos = rand() % linhasDescobertas.size();

			int linha = linhasDescobertas[random_pos];

			it = listaLinha.find(linha);

			if(it != listaLinha.end()){

                aux = it->second;
                for(int i = 0; i < (int)aux.size();i++){
                    conjuntoColuna.push_back(aux[i]);
                }


                int menorColuna = colunaMinimizaCusto(conjuntoColuna, linhasDescobertas, listaColuna, listaCusto);

                this->addColuna(menorColuna, listaCusto[menorColuna], listaColuna);
                //linhasDescobertas.removeAll(listaColuna[menorColuna]);
                it2 = listaColuna.find(menorColuna);
                if(it2 != listaColuna.end()){
                    aux2 = it2->second;
                    removeAll(linhasDescobertas, aux2);
                }

			}

			else{
                if((int)linhasDescobertas.size() == 1){
                    linhasDescobertas.erase(linhasDescobertas.begin());
                }
			}

		}


}

void Cromossomo::removeAll(vector<int> &linhasDescobertas, vector<int> &aux2){

    /*if(linhasDescobertas[0] == 0){
        linhasDescobertas.erase (linhasDescobertas.begin());
    } */


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

		for (int i = 0; i < (int)conjuntoColuna.size(); i++){
			int coluna = conjuntoColuna[i];
			double custo = listaCusto[coluna];

			it = listaColuna.find(coluna);
			if(it != listaColuna.end()){
                std::vector<int> aux = it->second; // declarar fora dop for
                int intersecao_size = Util::intersecao(linhasDescobertas, aux).size();
                if ((custo / intersecao_size) < menor){
                    menor = custo / intersecao_size;
                    menorColuna = coluna;
                }
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
			    aux2.clear();
				//int random_pos = getRandomInt(T.size());
				//srand(time(NULL));
                int random_pos = rand() % T.size();

				int coluna = T[random_pos];
				T.erase(T.begin() + random_pos);

				it2 = listaColuna.find(coluna);
				if(it2 != listaColuna.end()){
				    aux2 = it2->second;
				    if(aux2.size() > 0){
				        if (isRedundante(aux2)){
                            removeColuna(coluna, listaCusto);

                            for(int i = 0; i < (int)aux2.size();i++){ // ver se tenho que iniciar no zero ou no 1
                                qtdColunaCobreLinha[aux2[i]]--;
                            }
                        }
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
