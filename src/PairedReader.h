/*
 * PairedReader.h
 *
 *  Created on: Aug 26, 2020
 *      Author: gentzian
 */

#ifndef PAIREDREADER_H_
#define PAIREDREADER_H_

#include <string>
#include <vector>
#include "ZipFastqReader.h"

using namespace std;

class PairedReader {
public:
	PairedReader(string fastqgz_1, string fastqgz_2);
	std::vector<std::pair<fq_read*,fq_read*>>* next_reads(int max_size);
	virtual ~PairedReader();
private:
	ZipFastqReader Reader1;
	ZipFastqReader Reader2;
};

#endif /* PAIREDREADER_H_ */
