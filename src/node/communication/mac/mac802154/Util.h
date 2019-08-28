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

#ifndef UTIL_H_
#define UTIL_H_

#include <vector>
#include <algorithm>
#include <set>

using namespace std;
#include "Random.h"

class Util {

public:

    Util();
    virtual ~Util();

    static std::vector<int> intersecao(std::vector<int> &lista1, std::vector<int> &lista2);

    static std::vector<int> uniao(std::vector<int> &lista1, std::vector<int> &lista2);
};
int getRandomInt(int n);

double getRandomDouble();


#endif /* UTIL_H_ */
