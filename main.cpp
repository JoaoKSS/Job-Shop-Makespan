#include "JobShop.h"
#include <iostream>
#include <chrono>

using namespace std;
using namespace std::chrono;

int main(int argc, char* argv[]) {
    if (argc < 2) {
        cout << "Uso: " << argv[0] << " <arquivo_instancia.psi>" << endl;
        return 1;
    }

    GrafoDisjuntivo g;
    lerInstancia(argv[1], g);

    if (g.num_operacoes == 0) {
        cout << "Falha ao ler instancia ou instancia vazia." << endl;
        return 1;
    }

    cout << "Instancia carregada: " << g.num_jobs << " Jobs, " << g.num_maquinas << " Maquinas." << endl;

    auto time_start = high_resolution_clock::now();

    resolveHeuristica(g);

    vector<int> ordem;
    if (!caminhadaTopologica(g, ordem)) {
        cout << "A heuristica gerou um ciclo! Escalonamento invalido." << endl;
        return 1;
    }

    buscaLocal(g); // Tenta otimizar a solucao
    caminhadaTopologica(g, ordem); // Refaz a caminhada topologica

    auto time_end = high_resolution_clock::now();
    duration<double> tempo_decorrido = time_end - time_start;

    cout << "Caminhada topologica realizada com sucesso. Vertices iterados: " << ordem.size() << endl;

    vector<int> caminho_critico;
    int makespan = caminhoMaximo(g, ordem, caminho_critico);

    cout << "Makespan (Comprimento do caminho maximo): " << makespan << endl;
    cout << "Caminho maximo (Operacoes [Job, Maquina]): ";
    for (int i = 0; i < (int)caminho_critico.size(); ++i) {
        int u = caminho_critico[i];
        cout << "[" << g.J[u] << "," << g.M[u] << "]" << (i == (int)caminho_critico.size()-1 ? "" : " -> ");
    }
    cout << endl;
    cout << "Tempo de Execucao: " << tempo_decorrido.count() << " segundos" << endl;

    //escalonamento
    int n = g.num_operacoes;
    vector<int> tempo_inicio(n, 0);
    for (int u : ordem) {
        for (int v : g.adj[u]) {
            if (tempo_inicio[u] + g.T[u] > tempo_inicio[v]) {
                tempo_inicio[v] = tempo_inicio[u] + g.T[u];
            }
        }
    }

    cout << "Escalonamento:" << endl;
    for (int i = 0; i < n; ++i) {
        cout << "[" << g.J[i] << "," << g.M[i] << "]"
             << " inicio=" << tempo_inicio[i]
             << " fim=" << tempo_inicio[i] + g.T[i] << endl;
    }

    return 0;
}
