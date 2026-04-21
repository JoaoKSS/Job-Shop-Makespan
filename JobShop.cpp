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
  grau_entrada.assign(n, 0);
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

bool caminhadaTopologica(GrafoDisjuntivo &g, vector<int> &ordem) {
  ordem.clear();
  
  // Reinicializa grau_entrada
  fill(g.grau_entrada.begin(), g.grau_entrada.end(), 0);

  for (int i = 0; i < g.num_operacoes; ++i) {
    for (int v : g.adj[i]) {
      g.grau_entrada[v]++;
    }
  }

  queue<int> fila;
  for (int i = 0; i < g.num_operacoes; ++i) {
    if (g.grau_entrada[i] == 0) {
      fila.push(i);
    }
  }

  while (!fila.empty()) {
    int u = fila.front();
    fila.pop();
    ordem.push_back(u);

    for (int v : g.adj[u]) {
      g.grau_entrada[v]--;
      if (g.grau_entrada[v] == 0) {
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

/**
 * @brief Gera uma solução inicial factível para o problema de Job Shop.
 * * Utiliza uma heurística construtiva gulosa baseada em uma fila de prioridade 
 * simples (FIFO) para orientar as arestas disjuntivas das máquinas.
 * * @details
 * A fila começa com a 1ª etapa de cada Job. Empates são 
 * desempatados estritamente pela ordem de leitura do arquivo (Job 0, Job 1...).
 * A primeira operação da fila ganha a máquina que exige. 
 * Se a máquina estiver ocupada, ela entra na fila daquela máquina, gerando 
 * as restrições (SucM e AntM).
 * Assim que uma operação é agendada, a próxima etapa daquele 
 * mesmo Job é destrancada e enviada para o final da fila geral.
 */
void resolveHeuristica(GrafoDisjuntivo &g) {

queue<int> operacoes_disponiveis;

// Encontra a primeira operacao de cada job e coloca na fila de disponíveis
  for (int i = 0; i < g.num_operacoes; ++i) {
    if (g.AntJ[i] == -1) {
      operacoes_disponiveis.push(i);
    }
  }

  // guarda quem é o último da fila de espera em cada máquina
  vector<int> ultima_na_maquina(g.num_maquinas, -1);

  while (!operacoes_disponiveis.empty()) {
    // Pega a primeira disponivel e agenda na maquina dela
    int op_atual = operacoes_disponiveis.front();
    operacoes_disponiveis.pop();

    int maquina_necessaria = g.M[op_atual];
    if (ultima_na_maquina[maquina_necessaria] != -1) { // se já tem uma operação agendada pra essa máquina
      // a operação atual entra atrás dela
      int ant = ultima_na_maquina[maquina_necessaria];
      g.SucM[ant] = op_atual;
      g.AntM[op_atual] = ant;
    }

    // a atual agora é a última operação agendada para essa máquina
    ultima_na_maquina[maquina_necessaria] = op_atual;
    // libera o próximo da linha do job, se existir
    int proxima_op_job = g.SucJ[op_atual];
    if (proxima_op_job != -1) {
      operacoes_disponiveis.push(proxima_op_job);
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

    // Vizinhança em Blocos
    vector<pair<int, int>> pares_validos;
    int n_crit = caminho_critico.size();
    int i = 0;
    while (i < n_crit - 1) {
      int u = caminho_critico[i];
      int v = caminho_critico[i + 1];

      // Testa se inicia bloco de maquina
      if (g.M[u] == g.M[v] && g.SucM[u] == v) {
        int start = i;
        int end = i + 1;
        // Expande o bloco enquanto as proximas tarefas da linha critica forem da mesma maquina
        while (end + 1 < n_crit && g.M[caminho_critico[end]] == g.M[caminho_critico[end + 1]] && g.SucM[caminho_critico[end]] == caminho_critico[end + 1]) {
          end++;
        }

        if (end - start == 1) {
          // Bloco com 2 tarefas swap
          pares_validos.push_back({caminho_critico[start], caminho_critico[end]});
        } else {
          // Bloco com mais tarefas usa apenas os extremos
          pares_validos.push_back({caminho_critico[start], caminho_critico[start + 1]});
          pares_validos.push_back({caminho_critico[end - 1], caminho_critico[end]});
        }
        i = end; // Pula para fim do bloco
      } else {
        i++;
      }
    }

    // Tenta processar apenas as sub-areas com real potencial
    for (auto par : pares_validos) {
      int u = par.first;
      int v = par.second;

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
