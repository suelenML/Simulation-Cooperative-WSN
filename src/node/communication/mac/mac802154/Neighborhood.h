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

#ifndef NEIGHBORHOOD_H_
#define NEIGHBORHOOD_H_
#include <omnetpp.h>



class Neighborhood {
public:
    Neighborhood();
    virtual ~Neighborhood();


    int nodeId;
    std::vector<int> vizinhos;//só deve ser preenchido pelo nodo coordenador
    double energy;
    double somaRssi;
    double rssi;
    int numeroDevizinhos=0;
    double txSucesso;
    double txSucessoVizinhanca;

};



#endif /* NEIGHBORHOOD_H_ */
