#include "stdafx.h"
#include <iostream>
#include <cstdarg> // Обязательно для va_list

using namespace std;
using namespace Error;

FST::RELATION::RELATION(unsigned char c, short ns)
	: symbol(c), nnode(ns)
{ }

FST::NODE::NODE()
	: n_relation(0), relations(NULL)
{ }

// !!! ИСПРАВЛЕННЫЙ КОНСТРУКТОР !!!
FST::NODE::NODE(short n, RELATION rel, ...)
	: n_relation(n)
{
	relations = new RELATION[n];
	relations[0] = rel; // Первый аргумент берем явно

	va_list args;
	va_start(args, rel); // Начинаем читать то, что после rel
	for (short i = 1; i < n; ++i) {
		relations[i] = va_arg(args, RELATION);
	}
	va_end(args);
}

FST::FST::FST(unsigned char* s, short ns, NODE n, ...)
	: string(s), position(-1), nstates(ns)
{
	nodes = new NODE[ns];
	nodes[0] = n; // Первый узел явно

	va_list args;
	va_start(args, n);
	for (int i = 1; i < ns; ++i)
		nodes[i] = va_arg(args, NODE);
	va_end(args);

	rstates = new short[nstates];
	memset(rstates, 0xff, sizeof(short) * nstates);
	rstates[0] = 0;
}

bool step(FST::FST& fst, short*& rstates) {
	bool rc = false;
	std::swap(rstates, fst.rstates);
	
	for (int k = 0; k < fst.nstates; k++) fst.rstates[k] = -1;

	for (short i = 0; i < fst.nstates; ++i) {
		if (rstates[i] == fst.position) {
			for (short j = 0; j < fst.nodes[i].n_relation; ++j) {
				if (fst.nodes[i].relations[j].symbol == fst.string[fst.position]) {
					fst.rstates[fst.nodes[i].relations[j].nnode] = fst.position + 1;
					rc = true;
				}
			}
		}
	}
	return rc;
}

bool FST::execute(FST& fst) {
	short* rstates = new short[fst.nstates];
	short lstring = (short)strlen((const char*)fst.string);
	bool rc = true;

	memset(rstates, 0xff, sizeof(short) * fst.nstates);

	for (short i = 0; i < lstring && rc; ++i) {
		fst.position++;
		rc = step(fst, rstates);
	}
	delete[] rstates;

	return rc ? (fst.rstates[fst.nstates - 1] == lstring) : rc;
}