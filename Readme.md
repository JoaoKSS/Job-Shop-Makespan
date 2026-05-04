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

```bash
./jobshop Instancias/JSP/nome_do_arquivo.psi > resultado.txt && python3 visualizar.py Instancias/JSP/nome_do_arquivo.psi resultado.txt
```

## exemplo de saída

Instancia carregada: 3 Jobs, 5 Maquinas.<br>
Caminhada topologica realizada com sucesso. 15 vertices iterados<br>
Makespan (Comprimento do caminho maximo): 63<br>
Caminho maximo (Operacoes [Job, Maquina]): [0,0] -> [0,1] -> [0,2] -> [0,3] -> [1,3] -> [1,4] -> [2,4]<br>
Escalonamento:<br>
[0,0] inicio=0 fim=4<br>
[0,1] inicio=4 fim=7<br>
[0,2] inicio=7 fim=12<br>
[0,3] inicio=12 fim=27<br>
[0,4] inicio=27 fim=33<br>
[1,0] inicio=4 fim=14<br>
[1,1] inicio=14 fim=15<br>
[1,2] inicio=15 fim=26<br>
[1,3] inicio=27 fim=41<br>
[1,4] inicio=41 fim=54<br>
[2,0] inicio=14 fim=21<br>
[2,1] inicio=21 fim=29<br>
[2,2] inicio=29 fim=31<br>
[2,3] inicio=41 fim=53<br>
[2,4] inicio=54 fim=63<br>