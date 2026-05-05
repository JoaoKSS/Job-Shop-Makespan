# 🏭 Job-Shop-Makespan

![Badge](https://img.shields.io/badge/C%2B%2B-17-blue?style=flat&logo=c%2B%2B)
![Badge](https://img.shields.io/badge/Python-3.x-blue?style=flat&logo=python)
![Badge](https://img.shields.io/badge/Otimiza%C3%A7%C3%A3o-Busca%20Local%20(N3)-orange?style=flat)
![Badge](https://img.shields.io/badge/Status-Conclu%C3%ADdo-success?style=flat)

Este projeto implementa um otimizador heurístico para o clássico **Job Shop Scheduling Problem (JSP)**. O algoritmo utiliza **Caminhada Topológica (Algoritmo de Kahn)** para validação de grafos acíclicos, **Programação Dinâmica** para cálculo do Caminho Crítico e Makespan, e uma **Busca Local (Hill Climbing com Vizinhança N3)** para otimizar as soluções encontradas.

---

## 🚀 Funcionalidades

- **Resolução de Instâncias:** Lê arquivos padrão Taillard/OR-Library (`.psi`).
- **Geração de Gráficos:** Cria gráficos de Gantt interativos utilizando Python (`visualizar.py`).
- **Geração de Relatórios em Lote:** Processa todas as instâncias de forma ultrarrápida nativamente em C++ (`generate_results.cpp`), exportando a tabela comparativa para `.csv`.

---

## 🛠️ Como Compilar e Executar (Linux)

### 1. Compilação
Utilize o `make` para compilar o projeto em C++ e gerar o arquivo binário executável.
```bash
make
```

### 2. Executar uma Instância Única
```bash
./jobshop Instancias/JSP/nome_do_arquivo.psi
```

### 3. Limpar Arquivos de Compilação
```bash
make clean
```

---

## 📊 Visualização e Relatórios

### Gerar Gráfico de Gantt
Para rodar a instância, salvar a saída e já abrir o gráfico de Gantt animado em HTML no seu navegador:
```bash
./jobshop Instancias/JSP/nome_do_arquivo.psi > resultado.txt && python3 visualizar.py Instancias/JSP/nome_do_arquivo.psi resultado.txt
```

<div align="center">
  <img src="imagens/Job%20Shop%20Visualizer.png" alt="Exemplo do Job Shop Visualizer" width="800"/>
</div>

### Gerar Relatório Completo em Lote (CSV)
Para processar todas as 80 instâncias da pasta de forma ultrarrápida e exportar a tabela comparativa (Makespan obtido vs Best Known Solution) para o arquivo `resultados_ta01_ta80.csv`:
```bash
make
./generate_results
```

*(Opcional: Se desejar utilizar o script em Python para gerar o `.xlsx` via openpyxl, use: `python3 generate_results.py`)*

---

## 📄 Exemplo de Saída

Ao rodar uma instância simples pelo terminal, o programa retorna a extração do tempo máximo e o escalonamento final:

```text
Instancia carregada: 3 Jobs, 5 Maquinas.
Caminhada topologica realizada com sucesso. 15 vertices iterados
Makespan (Comprimento do caminho maximo): 63
Caminho maximo (Operacoes [Job, Maquina]): [0,0] -> [0,1] -> [0,2] -> [0,3] -> [1,3] -> [1,4] -> [2,4]
Escalonamento:
[0,0] inicio=0 fim=4
[0,1] inicio=4 fim=7
[0,2] inicio=7 fim=12
[0,3] inicio=12 fim=27
[0,4] inicio=27 fim=33
[1,0] inicio=4 fim=14
[1,1] inicio=14 fim=15
[1,2] inicio=15 fim=26
[1,3] inicio=27 fim=41
[1,4] inicio=41 fim=54
[2,0] inicio=14 fim=21
[2,1] inicio=21 fim=29
[2,2] inicio=29 fim=31
[2,3] inicio=41 fim=53
[2,4] inicio=54 fim=63
```
