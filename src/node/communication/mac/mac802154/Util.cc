/*
 * Util.cpp
 *
 *  Created on: 22 de ago de 2019
 *      Author: Suelen
 */

#include "Util.h"

int getRandomInt(int n){
	Random *random;
	random = new Random();
	return random->nextInt(n);
}

double getRandomDouble(){
	Random *random;
	random = new Random();
	return random->nextDouble();
}

std::vector<int> Util::intersecao(std::vector<int> &lista1, std::vector<int> &lista2){
	std::vector<int> intersec;
	for (int i = 0; i < (int)lista1.size();i++){ // ver se tenho que iniciar no zero ou no 1
		if (std::find(lista2.begin(), lista2.end(), lista1[i]) != lista2.end()){
			intersec.push_back(lista1[i]);
		}
	}
	return intersec;
}

std::vector<int> Util::uniao(std::vector<int> &lista1, std::vector<int> &lista2){
	std::vector<int> set;

	for(int i = 0; i < (int)lista1.size(); i++){
		set.push_back(lista1[i]);
	}

	for(int i = 0; i < (int)lista2.size(); i++){
		set.push_back(lista2[i]);
	}

	return set;
}

Util::~Util() {
	// TODO Auto-generated destructor stub
}
