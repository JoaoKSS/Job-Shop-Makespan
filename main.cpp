#include "JobShop.h"
#include <iostream>

using namespace std;

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

    resolveHeuristica(g); // Gera o agendamento simples

    vector<int> ordem;
    if (!caminhadaTopologica(g, ordem)) {
        cout << "A heuristica gerou um ciclo! Escalonamento invalido." << endl;
        return 1;
    }

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

    return 0;
}
