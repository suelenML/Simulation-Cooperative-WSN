/*
 * Random.cpp
 *
 *  Created on: 22 de ago de 2019
 *      Author: Suelen
 */

#include "Random.h"

Random::Random() : rng(static_cast<int>(std::time(0))) {}

Random::Random(unsigned seed) : rng(seed) {}

void Random::seed(unsigned seed)
{
   rng.seed(seed);
}

unsigned Random::nextInt(unsigned range)
{
   if (range == 0)
      return 0;

   boost::uniform_int<unsigned> interval(0,range-1);
   boost::variate_generator<boost::mt19937&, boost::uniform_int<unsigned> >
      die(rng, interval);
   return die();
}

int Random::nextInt(int min, int max)
{
   boost::uniform_int<> interval(min,max);
   boost::variate_generator<boost::mt19937&, boost::uniform_int<> >
      die(rng, interval);
   return die();
}

double Random::nextDouble()
{
   boost::uniform_real<> realInterval(0,1);
   boost::variate_generator<boost::mt19937&, boost::uniform_real<> >
      realRandom(rng, realInterval);
   return realRandom();
}

float Random::nextFloat()
{
   boost::uniform_real<float> realInterval(0,1);
   boost::variate_generator<boost::mt19937&, boost::uniform_real<float> >
      realRandom(rng, realInterval);
   return realRandom();
}

bool Random::nextBool()
{
   return nextInt(100) % 2 == 0;
}


