
#include <thread>
#include "PairedReader.h"
#include "FastqReader.h"
#include "BoostZipReader.h"

PairedReader::PairedReader(string fastqgz_1, string fastqgz_2) {

	if (fastqgz_1.length() > 3){
		string suffix = fastqgz_1.substr(fastqgz_1.length()-3);
		if (suffix.compare(".gz") == 0){
			// use zip reader
			Reader1 = new BoostZipReader(fastqgz_1);

		}
		else{
			// use fastq reader
			Reader1 = new FastqReader(fastqgz_1);
		}
	}
	if (fastqgz_2.length() > 3){
		string suffix = fastqgz_2.substr(fastqgz_2.length()-3);
		if (suffix.compare(".gz") == 0){
			// use zip reader
			Reader2 = new BoostZipReader(fastqgz_2);
		}
		else{
			// use fastq reader
			Reader2 = new FastqReader(fastqgz_2);
		}
	}
}

std::vector<std::pair<fq_read*, fq_read*>>* PairedReader::next_reads(
		size_t max_size) {
	std::vector<std::pair<fq_read*, fq_read*>> *pairs = new std::vector<std::pair<fq_read*, fq_read*>>();
	//fq_read* pairs = new fq_read[max_size*2+1];

	fq_read *read1, *read2;
	for (size_t i = 0; i < max_size; i++) {
		read1 = Reader1->next_read();
		read2 = Reader2->next_read();
		if (read1 != NULL && read2 != NULL) {
			pairs->push_back(std::pair<fq_read*, fq_read*>(read1, read2));
		}
		else{
			delete read1;
			delete read2;
		}
	}
	return pairs;
}

void fill_reads(IFastqReader* reader, size_t max_size, fq_read ***reads) {
	for (size_t i = 0; i < max_size; i++) {
		fq_read *tmp = reader->next_read();
		if (tmp != NULL){
			(*reads)[i] = tmp;
		}
		else{
			(*reads)[i] = NULL;
			delete tmp;
			break;
		}
	}
}

std::vector<std::pair<fq_read*, fq_read*>>* PairedReader::next_reads2(
		size_t max_size, int reading_threads) {
	std::vector<std::pair<fq_read*, fq_read*>> *pairs = new std::vector<std::pair<fq_read*, fq_read*>>();

	fq_read** reads1 = new fq_read*[max_size];
	fq_read** reads2 = new fq_read*[max_size];

	if (reading_threads == 2){
		std::thread to1(fill_reads,  Reader1, max_size, &reads1);
		std::thread to2(fill_reads,  Reader2, max_size, &reads2);
		to1.join();
		to2.join();
	}
	else{
		fill_reads(Reader1, max_size, &reads1);
		fill_reads(Reader2, max_size, &reads2);
	}

	for(size_t i = 0; i < max_size; i++){
		if(reads1[i] != NULL && reads2[i] != NULL){
			pairs->push_back(std::pair<fq_read*, fq_read*>(reads1[i], reads2[i]));
		}
		else{
			break;
		}
	}
	delete[] reads1;
	delete[] reads2;

	return pairs;
}

PairedReader::~PairedReader() {
	delete Reader1;
	delete Reader2;
}

