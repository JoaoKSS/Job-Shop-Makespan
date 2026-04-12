#ifndef JOBSHOP_H
#define JOBSHOP_H

#include <vector>
#include <string>

using namespace std;

const int MAX_NODES = 2000; // Limite para instâncias (100 jobs x 20 máquinas)

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

    // Lista de Adjacência
    vector<int> adj[MAX_NODES];
    
    void init(int n);
    void constroiAdjacencia();
};

// Funções principais
void lerInstancia(string filepath, GrafoDisjuntivo& g);
bool caminhadaTopologica(const GrafoDisjuntivo& g, vector<int>& ordem);
int caminhoMaximo(const GrafoDisjuntivo& g, const vector<int>& ordem_topologica, vector<int>& caminho);
void resolveHeuristica(GrafoDisjuntivo& g);
void buscaLocal(GrafoDisjuntivo& g);

#endif
