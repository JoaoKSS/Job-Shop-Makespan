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
- **Geração de Tabela:** Processa múltiplas instâncias em lote e exporta relatórios completos para Excel (`generate_results.py`).

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

### Gerar Relatório Completo (Excel)
Para processar todas as instâncias da pasta e exportar a tabela comparativa (Makespan obtido vs Best Known Solution):
```bash
python3 generate_results.py
```
*(Nota: Certifique-se de ter a biblioteca `openpyxl` instalada: `python3 -m pip install openpyxl`)*

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
