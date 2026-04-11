# Job-Shop-Makespan

## comandos utilizados linux

```bash
make
```

```bash
./jobshop Instancias/JSP/nome_do_arquivo.psi
```
```bash
make clean
```

## exemplo de saída

Instancia carregada: n Jobs, m Maquinas.
Caminhada topologica realizada com sucesso. Vertices iterados: n * m
Makespan (Comprimento do caminho maximo): valor
Caminho maximo (Operacoes [Job, Maquina]): [3,5] -> [5,5] -> [12,5] -> [1,5] -> [1,7] -> [1,14] -> [8,14] -> [6,14] -> [5,14] -> [7,14] -> [4,14] -> [4,12] -> [4,5] -> [4,1] -> [9,1] -> [12,1] -> [10,1] -> [11,1] -> [1,1] -> [3,1] -> [3,3] -> [4,3] -> [8,3]