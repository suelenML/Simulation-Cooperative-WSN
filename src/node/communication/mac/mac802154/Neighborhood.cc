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

#include "Neighborhood.h"

Neighborhood::Neighborhood() {
    nodeId = 0;
    vizinhos.clear();
    energy = 0;
    somaRssi = 0;
    rssi = 0;
    numeroDevizinhos = 0;
    txSucesso = 0;
    txSucessoVizinhanca = 0;


}


Neighborhood::~Neighborhood() {
    // TODO Auto-generated destructor stub
}

