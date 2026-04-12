#include "JobShop.h"
#include <algorithm>
#include <fstream>
#include <iostream>
#include <queue>

using namespace std;

void GrafoDisjuntivo::init(int n) {
  num_operacoes = n;
  T.assign(n, 0);
  M.assign(n, 0);
  J.assign(n, 0);
  SucJ.assign(n, -1);
  AntJ.assign(n, -1);
  SucM.assign(n, -1);
  AntM.assign(n, -1);
  for (int i = 0; i < MAX_NODES; ++i)
    adj[i].clear();
}

void GrafoDisjuntivo::constroiAdjacencia() {
  for (int i = 0; i < num_operacoes; ++i) {
    adj[i].clear();
  }
  for (int i = 0; i < num_operacoes; ++i) {
    if (SucJ[i] != -1)
      adj[i].push_back(SucJ[i]);
    if (SucM[i] != -1)
      adj[i].push_back(SucM[i]);
  }
}

void lerInstancia(string filepath, GrafoDisjuntivo &g) {
  ifstream fin(filepath);
  if (!fin.is_open()) {
    cerr << "Erro ao abrir o arquivo: " << filepath << endl;
    return;
  }

  string palavra;
  int total_jobs = 0, total_maquinas = 0;

  // Ignora ate achar TotalJobs
  while (fin >> palavra) {
    if (palavra == "TotalJobs:") {
      fin >> total_jobs;
      fin >> palavra >> total_maquinas;
      break;
    }
  }

  g.num_jobs = total_jobs;
  g.num_maquinas = total_maquinas;
  g.init(total_jobs * total_maquinas);

  // Ignora "Costs"
  while (fin >> palavra) {
    if (palavra == "Costs:")
      break;
  }

  // Le a matriz de tempos
  for (int j = 0; j < total_jobs; ++j) {
    for (int m = 0; m < total_maquinas; ++m) {
      int id_op = j * total_maquinas + m;
      fin >> g.T[id_op];
      g.J[id_op] = j;
      g.M[id_op] = m;
    }
  }

  // Le as dependencias dos jobs
  for (int j = 0; j < total_jobs; ++j) {
    while (fin >> palavra) {
      if (palavra == "Job:")
        break;
    }
    int num_arestas;
    fin >> num_arestas;

    for (int e = 0; e < num_arestas; ++e) {
      int u, v;
      string seta;
      fin >> u >> seta >> v;

      // As maquinas servem de indices aqui para o proprio job
      int op_u = j * total_maquinas + u;
      int op_v = j * total_maquinas + v;

      g.SucJ[op_u] = op_v;
      g.AntJ[op_v] = op_u;
    }
  }

  fin.close();
}

bool caminhadaTopologica(const GrafoDisjuntivo &g, vector<int> &ordem) {
  ordem.clear();
  vector<int> grau_entrada(g.num_operacoes, 0);

  for (int i = 0; i < g.num_operacoes; ++i) {
    for (int v : g.adj[i]) {
      grau_entrada[v]++;
    }
  }

  queue<int> fila;
  for (int i = 0; i < g.num_operacoes; ++i) {
    if (grau_entrada[i] == 0) {
      fila.push(i);
    }
  }

  while (!fila.empty()) {
    int u = fila.front();
    fila.pop();
    ordem.push_back(u);

    for (int v : g.adj[u]) {
      grau_entrada[v]--;
      if (grau_entrada[v] == 0) {
        fila.push(v);
      }
    }
  }

  return (int)ordem.size() == g.num_operacoes;
}

int caminhoMaximo(const GrafoDisjuntivo &g, const vector<int> &ordem_topologica,
                  vector<int> &caminho) {
  int n = g.num_operacoes;
  vector<int> tempo_inicio(n, 0);
  vector<int> pai(n, -1);

  for (int u : ordem_topologica) {
    for (int v : g.adj[u]) {
      if (tempo_inicio[u] + g.T[u] > tempo_inicio[v]) {
        tempo_inicio[v] = tempo_inicio[u] + g.T[u];
        pai[v] = u;
      }
    }
  }

  int makespan = 0;
  int ultimo_no = -1;

  for (int i = 0; i < n; ++i) {
    int tempo_conclusao = tempo_inicio[i] + g.T[i];
    if (tempo_conclusao > makespan) {
      makespan = tempo_conclusao;
      ultimo_no = i;
    }
  }

  caminho.clear();
  int atual = ultimo_no;
  while (atual != -1) {
    caminho.push_back(atual);
    atual = pai[atual];
  }
  reverse(caminho.begin(), caminho.end());

  return makespan;
}

// heuristica gulosa para definir uma solucao viavel
void resolveHeuristica(GrafoDisjuntivo &g) {
  // Monta listas de operacoes por maquina
  vector<vector<int>> ops_maquina(g.num_maquinas);
  for (int i = 0; i < g.num_operacoes; ++i) {
    ops_maquina[g.M[i]].push_back(i);
  }

  vector<int> prontos;
  vector<int> ops_restantes_job(
      g.num_jobs, 0); // indice da proxima operacao a rodar de cada job
  vector<int> primeira_op_job(g.num_jobs, -1);

  // encontra a primeira operacao de cada job
  for (int i = 0; i < g.num_operacoes; ++i) {
    if (g.AntJ[i] == -1) {
      primeira_op_job[g.J[i]] = i;
    }
  }

  for (int j = 0; j < g.num_jobs; ++j) {
    if (primeira_op_job[j] != -1)
      prontos.push_back(primeira_op_job[j]);
  }

  vector<int> ultima_na_maquina(g.num_maquinas, -1);

  while (!prontos.empty()) {
    // Pega a primeira disponivel e agenda na maquina dela
    int u = prontos.front();
    prontos.erase(prontos.begin());

    int m = g.M[u];
    if (ultima_na_maquina[m] != -1) {
      int ant = ultima_na_maquina[m];
      g.SucM[ant] = u;
      g.AntM[u] = ant;
    }
    ultima_na_maquina[m] = u;

    // Coloca o sucessor do job como pronto
    if (g.SucJ[u] != -1) {
      prontos.push_back(g.SucJ[u]);
    }
  }

  g.constroiAdjacencia();
}

// Busca Local baseada em Hill Climbing
void buscaLocal(GrafoDisjuntivo &g) {
  bool melhorou = true;
  while (melhorou) {
    melhorou = false;

    vector<int> ordem;
    if (!caminhadaTopologica(g, ordem))
      break;

    vector<int> caminho_critico;
    int melhor_makespan = caminhoMaximo(g, ordem, caminho_critico);

    for (int i = 0; i < (int)caminho_critico.size() - 1; ++i) {
      int u = caminho_critico[i];
      int v = caminho_critico[i + 1];

      if (g.M[u] == g.M[v] && g.SucM[u] == v) {
        
        // Salva estado antigo
        int ant_u = g.AntM[u];
        int suc_v = g.SucM[v];

        // Swap
        g.SucM[u] = suc_v;
        g.AntM[u] = v;
        g.SucM[v] = u;
        g.AntM[v] = ant_u;

        if (ant_u != -1) g.SucM[ant_u] = v;
        if (suc_v != -1) g.AntM[suc_v] = u;

        g.constroiAdjacencia();

        vector<int> nova_ordem;
        if (caminhadaTopologica(g, nova_ordem)) {
          vector<int> novo_caminho;
          int novo_makespan = caminhoMaximo(g, nova_ordem, novo_caminho);

          // Funcao objetivo de Minimizacao
          if (novo_makespan < melhor_makespan) {
            melhorou = true;
            break; // Aceita a melhora e reinicia a busca
          }
        }

        // Reverte as mudancas se piorou ou se causou ciclo
        g.SucM[u] = v;
        g.AntM[u] = ant_u;
        g.SucM[v] = suc_v;
        g.AntM[v] = u;

        if (ant_u != -1) g.SucM[ant_u] = u;
        if (suc_v != -1) g.AntM[suc_v] = v;

        g.constroiAdjacencia();
      }
    }
  }
}
