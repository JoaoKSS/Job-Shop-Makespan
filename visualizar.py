import sys
import os
import re
import webbrowser

def extrair_dados_psi(caminho_psi):
    """Lê o arquivo .psi extraindo tempos E a ordem de precedência de cada Job."""
    try:
        with open(caminho_psi, 'r') as f:
            linhas = f.readlines()
        
        num_jobs = 0
        num_maquinas = 0
        tempos = []
        
        for linha in linhas:
            if "TotalJobs:" in linha:
                partes = linha.split()
                num_jobs = int(partes[1])
                num_maquinas = int(partes[3])
                break

        # Lê a matriz de custos
        start_costs = False
        for linha in linhas:
            if "Costs:" in linha:
                start_costs = True
                continue
            if start_costs and len(tempos) < num_jobs:
                valores = [int(x) for x in linha.split() if x.lstrip('-').isdigit()]
                if valores: tempos.append(valores)
        
        # Lê as dependências de cada Job (seção "Job: N")
        # Constrói a sequência de máquinas de cada job
        # SucJ[m] = próxima máquina após m, para cada job
        job_deps = []  # lista de dicts: job_deps[j] = {maquina_u: maquina_v, ...}
        
        i = 0
        while i < len(linhas):
            if linhas[i].strip().startswith("Job:"):
                deps = {}
                i += 1
                while i < len(linhas) and not linhas[i].strip().startswith("Job:") and not linhas[i].strip().startswith("#"):
                    match = re.match(r'\s*(\d+)\s*->\s*(\d+)', linhas[i].strip())
                    if match:
                        u = int(match.group(1))
                        v = int(match.group(2))
                        deps[u] = v
                    i += 1
                job_deps.append(deps)
            else:
                i += 1
        
        # Gera a sequência ordenada de máquinas para cada job
        job_ordem = []
        for j in range(num_jobs):
            if j < len(job_deps):
                deps = job_deps[j]
                # Encontra a primeira máquina (a que não é destino de ninguém)
                destinos = set(deps.values())
                origens = set(deps.keys())
                todas = origens | destinos
                primeira = None
                for m in todas:
                    if m not in destinos:
                        primeira = m
                        break
                
                if primeira is None:
                    # Fallback: usa ordem 0, 1, 2...
                    job_ordem.append(list(range(num_maquinas)))
                    continue
                
                # Percorre a cadeia de dependências
                ordem = [primeira]
                atual = primeira
                while atual in deps:
                    atual = deps[atual]
                    ordem.append(atual)
                
                job_ordem.append(ordem)
            else:
                job_ordem.append(list(range(num_maquinas)))
        
        return num_jobs, num_maquinas, tempos, job_ordem
    except Exception as e:
        print(f"Erro ao ler .psi: {e}")
        import traceback
        traceback.print_exc()
        return None

def extrair_dados_resultado(caminho_res):
    try:
        with open(caminho_res, 'r') as f:
            conteudo = f.read()
        
        makespan = None
        caminho_critico = []
        tempo_exec = "N/A"
        
        m_match = re.search(r"Makespan.*: (\d+)", conteudo)
        if m_match: makespan = int(m_match.group(1))
            
        p_match = re.search(r"Caminho maximo.*: (.*)", conteudo)
        if p_match:
            partes = p_match.group(1).split("->")
            for p in partes:
                coord = re.findall(r"(\d+)", p)
                if len(coord) >= 2:
                    caminho_critico.append({'job': int(coord[0]), 'machine': int(coord[1])})
        
        t_match = re.search(r"Tempo de Execucao: ([\d\.eE\-\+]+)", conteudo)
        if t_match: tempo_exec = f"{t_match.group(1)}s"
                    
        return makespan, caminho_critico, tempo_exec
    except Exception as e:
        print(f"Erro ao ler resultado: {e}")
        return None, [], "N/A"

def extrair_escalonamento(caminho_res):
    """Lê o bloco 'Escalonamento:' do resultado.txt com os tempos reais do C++."""
    try:
        with open(caminho_res, 'r') as f:
            conteudo = f.read()

        operacoes = []
        em_bloco = False
        for linha in conteudo.splitlines():
            if linha.strip() == "Escalonamento:":
                em_bloco = True
                continue
            if em_bloco:
                # Formato: [job,maquina] inicio=X fim=Y
                m = re.match(r'\[(\d+),(\d+)\]\s+inicio=(\d+)\s+fim=(\d+)', linha.strip())
                if m:
                    operacoes.append({
                        'job': int(m.group(1)),
                        'machine': int(m.group(2)),
                        'inicio': int(m.group(3)),
                        'fim': int(m.group(4)),
                        'duracao': int(m.group(4)) - int(m.group(3)),
                    })
        return operacoes
    except Exception as e:
        print(f"Erro ao ler escalonamento: {e}")
        return []

def gerar_visualizacao(num_jobs, num_maquinas, makespan_real, caminho_real, escalonamento, tempo_exec, filename):
    """Gera o HTML usando os tempos reais exportados pelo C++."""

    # Marca quais operações estão no caminho crítico
    criticos = {(c['job'], c['machine']) for c in caminho_real}
    for op in escalonamento:
        op['critico'] = (op['job'], op['machine']) in criticos

    makespan_final = makespan_real if makespan_real else max((op['fim'] for op in escalonamento), default=0)

    html_template = f"""
    <!DOCTYPE html>
    <html lang="pt-br">
    <head>
        <meta charset="UTF-8">
        <title>JSP Visualizer - {filename}</title>
        <style>
            :root {{ --primary: #455a64; --primary-dark: #263238; --critical: #d32f2f; --bg: #eceff1; --text: #37474f; }}
            body {{ font-family: 'Inter', sans-serif; background: var(--bg); margin: 0; padding: 20px; color: var(--text); }}
            .container {{ width: 98%; max-width: 1900px; margin: 0 auto; background: white; padding: 25px; border-radius: 16px; box-shadow: 0 15px 35px rgba(0,0,0,0.05); }}
            header {{ border-bottom: 1px solid #eee; margin-bottom: 20px; padding-bottom: 10px; }}
            h1 {{ margin: 0; font-size: 1.6em; color: var(--primary-dark); }}
            .info-chips {{ display: flex; gap: 10px; margin-bottom: 20px; flex-wrap: wrap; }}
            .chip {{ padding: 8px 16px; border-radius: 20px; font-size: 0.85em; font-weight: 600; display: flex; align-items: center; gap: 8px; border: 1px solid #e2e8f0; }}
            .chip-file {{ background: #f8fafc; color: #475569; }}
            .chip-dim {{ background: #f0f9ff; color: #0369a1; }}
            .chip-time {{ background: #fffbeb; color: #b45309; border-color: #fde68a; }}
            .chip-makespan {{ background: #ecfdf5; color: #047857; border-color: #a7f3d0; }}
            .path-box {{ background: #fff5f5; border: 1px solid #fee2e2; padding: 15px; border-radius: 8px; margin-bottom: 20px; }}
            .chart-container {{ width: 100%; overflow-x: auto; background: white; padding: 10px 0; }}
            .gantt-chart {{ position: relative; border-left: 2px solid #cfd8dc; border-bottom: 2px solid #cfd8dc; padding: 5px 0; }}
            
            .legend {{ display: flex; flex-wrap: wrap; gap: 15px; margin-top: 25px; justify-content: center; font-size: 0.8em; border-top: 1px solid #eee; padding-top: 15px; }}
            .legend-item {{ display: flex; align-items: center; gap: 6px; font-weight: 600; color: #64748b; }}
            .color-box {{ width: 14px; height: 14px; border-radius: 3px; }}

            .job-row {{ height: 48px; display: flex; align-items: center; border-bottom: 1px solid #f1f5f9; position: relative; }}
            .job-label {{ width: 80px; min-width: 80px; font-weight: 700; position: sticky; left: 0; background: white; z-index: 20; color: #94a3b8; font-size: 0.8em; text-align: center; }}

            /* Eixo do Tempo */
            .time-axis {{ height: 30px; position: relative; border-top: 2px solid #cfd8dc; margin-top: 10px; margin-left: 80px; }}
            .time-mark {{ position: absolute; top: 0; transform: translateX(-50%); font-size: 0.7em; color: #64748b; padding-top: 8px; font-weight: 600; }}
            .time-mark::before {{ content: ''; position: absolute; top: -2px; left: 50%; width: 2px; height: 8px; background: #cfd8dc; }}

            /* Cursor de Tempo */
            #timeCursor {{ position: absolute; top: 0; bottom: 0; width: 2px; background: rgba(211, 47, 47, 0.5); pointer-events: none; display: none; z-index: 50; }}
            #timeLabel {{ position: absolute; background: #000; color: #fff; padding: 4px 8px; border-radius: 4px; font-size: 0.7em; pointer-events: none; display: none; z-index: 60; transform: translateX(-50%); }}

            @keyframes fadeIn {{ from {{ opacity: 0; }} to {{ opacity: 1; }} }}
            .op-bar {{ 
                position: absolute; height: 34px; border-radius: 6px; display: flex; align-items: center; justify-content: center; 
                color: white; font-size: 0.75em; font-weight: 700; box-shadow: 0 2px 4px rgba(0,0,0,0.1); 
                transition: transform 0.2s; cursor: crosshair; animation: fadeIn 0.5s ease-in;
                background-image: linear-gradient(to bottom, rgba(255,255,255,0.1), rgba(0,0,0,0.05));
            }}
            .op-bar:hover {{ transform: scaleY(1.1); z-index: 100 !important; }}
            .op-bar.critical {{ border: 2px solid #ff0000; box-shadow: 0 0 12px rgba(255,0,0,0.5); z-index: 10; }}
        </style>
    </head>
    <body>
        <div class="container">
            <header>
                <h1>Job Shop Visualizer</h1>
            </header>
            <div class="info-chips">
                <div class="chip chip-file">📄 Instância: {filename}</div>
                <div class="chip chip-dim">⚙️ {num_jobs} Jobs x {num_maquinas} Máquinas</div>
                <div class="chip chip-time">⏱️ Execução: {tempo_exec}</div>
                <div class="chip chip-makespan">🏆 Makespan: {makespan_final}</div>
            </div>
            <div class="path-box">
                <div style="font-weight: bold; color: var(--critical); font-size: 0.8em; text-transform: uppercase; margin-bottom: 5px;">Caminho Crítico [maquina, Trabalho]:</div>
                <div style="font-family: monospace; font-size: 0.9em; color: #b91c1c;">{" → ".join([f"[{c['machine']},{c['job']}]" for c in caminho_real])}</div>
            </div>
            <div class="chart-container" id="chartScroll">
                <div class="gantt-chart" id="ganttChart" style="width: {makespan_final * 15 + 100}px;">
                    <div id="timeCursor"></div>
                    <div id="timeLabel">0</div>
                    {gerar_rows_html(escalonamento, num_jobs)}
                </div>
                <div class="time-axis" id="timeAxis" style="width: {makespan_final * 15 + 100}px;">
                    {gerar_axis_html(makespan_final)}
                </div>
            </div>
            <div class="legend">
                {gerar_legend_html(num_maquinas)}
            </div>
        </div>
        <script>
            const chartScroll = document.getElementById('chartScroll');
            const ganttChart = document.getElementById('ganttChart');
            const timeCursor = document.getElementById('timeCursor');
            const timeLabel = document.getElementById('timeLabel');
            const scale = 15;

            chartScroll.addEventListener('mousemove', (e) => {{
                const rect = ganttChart.getBoundingClientRect();
                const x = e.clientX - rect.left;
                const maxX = {makespan_final} * scale + 85;
                
                if (x >= 85 && x <= maxX) {{
                    timeCursor.style.display = 'block';
                    timeLabel.style.display = 'block';
                    timeCursor.style.left = x + 'px';
                    timeLabel.style.left = x + 'px';
                    timeLabel.style.top = (e.clientY - rect.top - 30) + 'px';
                    timeLabel.innerText = Math.round((x - 85) / scale);
                }} else {{
                    timeCursor.style.display = 'none';
                    timeLabel.style.display = 'none';
                }}
            }});
            chartScroll.addEventListener('mouseleave', () => {{
                timeCursor.style.display = 'none';
                timeLabel.style.display = 'none';
            }});
        </script>
    </body>
    </html>
    """
    
    with open("visualizacao.html", "w") as f:
        f.write(html_template)
    webbrowser.open('file://' + os.path.realpath("visualizacao.html"))

def gerar_rows_html(ops, num_jobs):
    rows = ""
    for j in range(num_jobs):
        rows += f'<div class="job-row"><div class="job-label">Job {j}</div>'
        job_ops = [o for o in ops if o['job'] == j]
        for op in job_ops:
            crit_class = " critical" if op['critico'] else ""
            cor = ["#3498db", "#9b59b6", "#f1c40f", "#2ecc71", "#e67e22", "#e74c3c", "#1abc9c", "#34495e", "#d35400", "#c0392b", "#27ae60", "#2980b9", "#8e44ad", "#f39c12", "#7f8c8d"][op['machine'] % 15]
            title = f"M{op['machine']} | Duração: {op['duracao']} | Início: {op['inicio']} | Fim: {op['fim']}"
            rows += f'<div class="op-bar{crit_class}" style="left: {op["inicio"] * 15 + 85}px; width: {op["duracao"] * 15}px; background-color: {cor};" title="{title}">'
            rows += f'M{op["machine"]}({op["duracao"]})</div>'
        rows += '</div>'
    return rows

def gerar_axis_html(makespan):
    marks = ""
    if makespan <= 100: step = 5
    elif makespan <= 200: step = 10
    elif makespan <= 500: step = 50
    elif makespan <= 2000: step = 100
    else: step = 200
    for t in range(0, makespan + 1, step):
        marks += f'<div class="time-mark" style="left: {t * 15 + 5}px;">{t}</div>'
    return marks

def gerar_legend_html(num_maquinas):
    items = ""
    cores = ["#3498db", "#9b59b6", "#f1c40f", "#2ecc71", "#e67e22", "#e74c3c", "#1abc9c", "#34495e", "#d35400", "#c0392b", "#27ae60", "#2980b9", "#8e44ad", "#f39c12", "#7f8c8d"]
    for m in range(num_maquinas):
        cor = cores[m % 15]
        items += f'<div class="legend-item"><div class="color-box" style="background-color: {cor}"></div> M{m}</div>'
    return items

if __name__ == "__main__":
    if len(sys.argv) < 3:
        print("Uso: python3 visualizar.py <instancia.psi> <resultado.txt>")
    else:
        dados_psi = extrair_dados_psi(sys.argv[1])
        if dados_psi:
            nj, nm, _t, _jo = dados_psi
            mk, cp, tx = extrair_dados_resultado(sys.argv[2])
            esc = extrair_escalonamento(sys.argv[2])
            if not esc:
                print("Aviso: bloco 'Escalonamento:' nao encontrado no resultado.txt. Recompile e rode o C++ novamente.")
            else:
                gerar_visualizacao(nj, nm, mk, cp, esc, tx, os.path.basename(sys.argv[1]))
                print("Visualizacao gerada com sucesso!")
        else:
            print("Nao foi possivel carregar a instancia.")
