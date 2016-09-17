/*
 * Neighbor.h
 *
 *  Created on: Aug 19, 2016
 *      Author: riad
 */

#ifndef NEIGHBOR_H_
#define NEIGHBOR_H_

class Neighbor {
public:
    Neighbor();
    virtual ~Neighbor();

    int nodeId;
    double rssi;
    double energy;
};

#endif /* NEIGHBOR_H_ */

