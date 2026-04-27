#ifndef JOBSHOP_H
#define JOBSHOP_H

#include <vector>
#include <string>

using namespace std;

struct GrafoDisjuntivo {
    int num_operacoes;
    int num_jobs;
    int num_maquinas;
    
    // Arrays da Instância
    vector<int> T;
    vector<int> M;
    vector<int> J;
    vector<int> SucJ;
    vector<int> AntJ;
    
    // Arrays da Construção da Solução
    vector<int> SucM;
    vector<int> AntM;

    // Grau de entrada
    vector<int> grau_entrada;

    // Lista de Adjacência
    vector<vector<int>> adj;

    void init(int n);
    void constroiAdjacencia();
};

// Funções principais
void lerInstancia(string filepath, GrafoDisjuntivo &g);
bool caminhadaTopologica(GrafoDisjuntivo &g, vector<int> &ordem);
int caminhoMaximo(const GrafoDisjuntivo &g, const vector<int> &ordem_topologica, vector<int> &caminho);
void resolveHeuristica(GrafoDisjuntivo &g);
void buscaLocal(GrafoDisjuntivo &g);
#endif
