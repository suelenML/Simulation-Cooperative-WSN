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

#ifndef RANDOM_H_
#define RANDOM_H_

#include "boost/random.hpp"
#include "boost/generator_iterator.hpp"
#include <ctime>
using namespace std;

class Random {

       private:
          boost::random::mt19937 rng;

       public:
          explicit Random();
          explicit Random(unsigned seed);

          void seed(unsigned seed);
          unsigned nextInt(unsigned max);

          int nextInt(int min, int max);
          double nextDouble();

          float nextFloat();
          bool nextBool();

};

#endif /* RANDOM_H_ */
