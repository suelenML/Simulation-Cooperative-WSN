#include "AlgoritmoGenetico.h"

AlgoritmoGenetico::AlgoritmoGenetico(int nLinha, int nColuna, int tam_populacao, double taxa_min_mutacao, int num_geracoes){
		TAM_POPULACAO = tam_populacao;
		TAXA_MIN_MUTACAO = taxa_min_mutacao;
		NUM_GERACOES = num_geracoes;
		listaColuna.clear();
		listaLinha.clear();
		listaCusto.clear();
		// listaCusto.reserve(200000);
		for(int i = 1; i <= nColuna; i++){
			listaCusto.push_back(0);
		}

		populacao = new Populacao(TAM_POPULACAO);

}


void AlgoritmoGenetico::addCusto(int coluna, double custo)
	{
		this->listaCusto[coluna] = custo;
	}

void AlgoritmoGenetico::addDados(int coluna, int linha)
{
    std::vector<int> auxListaColuna;
    std::vector<int> auxListaLinha;
	linha--;

	std::map<int,vector<int>>::iterator itListaColuna;
	itListaColuna = listaColuna.find(coluna);

	std::map<int,vector<int>>::iterator itListaLinha;
	itListaLinha = listaLinha.find(linha);


	if(linha == -1){
        auxListaColuna.clear();
        listaColuna[coluna] = auxListaColuna;
	}else{
        if(itListaColuna == listaColuna.end()){
            auxListaColuna.clear();
            auxListaColuna.push_back(linha);
            listaColuna[coluna] = auxListaColuna;


        }else{
            auxListaColuna.clear();
            auxListaColuna = listaColuna[coluna];
            auxListaColuna.push_back(linha);
            listaColuna[coluna] = auxListaColuna;
        }

        if(itListaLinha == listaLinha.end()){
            auxListaLinha.clear();
            auxListaLinha.push_back(coluna);
            listaLinha[linha] = auxListaLinha;
        }else{
            auxListaLinha.clear();
            auxListaLinha = listaLinha[linha];
            auxListaLinha.push_back(coluna);
            listaLinha[linha] = auxListaLinha;
        }
	}

}

void AlgoritmoGenetico::executar(){
	populacao->gerarPopulacaoInicial(listaLinha, listaColuna, listaCusto);
	int i = 0;
	while (i < NUM_GERACOES){
		Cromossomo *filho = crossover();

		Cromossomo *maisApto = populacao->maisApto();
		Cromossomo *menosApto = populacao->menosApto();

		double random_double = getRandomDouble();
		if (random_double < taxaMutacao(maisApto->getCustoTotal(), menosApto->getCustoTotal()))	{
			mutacao(filho);
		}

		if (filho->getCustoTotal() < menosApto->getCustoTotal()){
			populacao->atualizar(filho);
			i = 0;
		}
		else{
			i++;
		}
		cout << "i = " << i << endl;
	}
	cout << "MELHOR SOLUCAO:" << endl;

	cout << "Colunas:" << endl;
	for(int i = 0; i < (int)populacao->maisApto()->getColunas().size();i++){
		cout << "Coluna[" <<i<<"]: "<<populacao->maisApto()->getColunas()[i]<<"\n";
	}

	cout << "CUSTO: " << populacao->maisApto()->getCustoTotal() << endl;

}

Cromossomo *AlgoritmoGenetico::selecao(){
		Cromossomo *c = nullptr;
		for (int i = 0; i < QTD_TORNEIO; i++){
			//int random_pos = getRandomInt(TAM_POPULACAO);
			srand(time(NULL));
			int random_pos = rand() % TAM_POPULACAO;
			Cromossomo *ran = populacao->getPopulacao()[random_pos];
			if (c == nullptr || ran->getCustoTotal() < c->getCustoTotal())	{
				c = ran;
			}
		}
		return c;
	}

Cromossomo *AlgoritmoGenetico::crossover(){
	Cromossomo *pai_x = selecao();
	Cromossomo *pai_y = selecao();

	while (pai_x == pai_y){
		pai_y = selecao();
	}
	std::vector<int> paiX;
	std::vector<int> paiY;
	for(int j = 0;j< (int)pai_x->getColunas().size();j++){
        paiX.push_back(0);
	}

	for(int i = 0; i < (int)pai_y->getColunas().size();i++){
        paiY.push_back(0);
	}

	for(int i = 0; i < (int)pai_x->getColunas().size();i++){
		paiX[i] = pai_x->getColunas()[i];
	}

	for(int i = 0; i < (int)pai_y->getColunas().size();i++){
		paiY[i] = pai_y->getColunas()[i];
	}
	std::vector<int> uniao = Util::uniao(paiX, paiY);
	Cromossomo *filho = new Cromossomo(uniao, listaColuna, listaLinha, listaCusto);
	filho->eliminaRedundancia(listaColuna, listaCusto);


	return filho;
}

void AlgoritmoGenetico::mutacao(Cromossomo *C){
	double random_double = getRandomDouble();
	int n = (int)(random_double * C->getColunas().size());
	for (int i = 0; i < n; i++){
		//int random_col = getRandomInt(listaColuna.size());
		srand(time(NULL));
		int random_col = rand() % listaColuna.size();
		C->addColuna(random_col, listaCusto[random_col], listaColuna);
	}
	C->eliminaRedundancia(listaColuna, listaCusto);
}

double AlgoritmoGenetico::taxaMutacao(double custoMaisApto, double custoMenosApto){
		double taxa = TAXA_MIN_MUTACAO;
		taxa = taxa / (1 - exp((-custoMenosApto - custoMaisApto) / custoMenosApto));
		return taxa;
}
