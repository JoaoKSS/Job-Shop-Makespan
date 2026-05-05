#include "JobShop.h"
#include <iostream>
#include <fstream>
#include <map>
#include <iomanip>
#include <string>

using namespace std;

int main() {
    map<string, int> best_known_solutions = {
        {"ta01", 1231}, {"ta02", 1244}, {"ta03", 1218}, {"ta04", 1175}, {"ta05", 1224},
        {"ta06", 1238}, {"ta07", 1227}, {"ta08", 1217}, {"ta09", 1274}, {"ta10", 1241},
        {"ta11", 1357}, {"ta12", 1367}, {"ta13", 1342}, {"ta14", 1345}, {"ta15", 1339},
        {"ta16", 1360}, {"ta17", 1462}, {"ta18", 1396}, {"ta19", 1332}, {"ta20", 1348},
        {"ta21", 1642}, {"ta22", 1600}, {"ta23", 1557}, {"ta24", 1644}, {"ta25", 1595},
        {"ta26", 1643}, {"ta27", 1680}, {"ta28", 1603}, {"ta29", 1625}, {"ta30", 1584},
        {"ta31", 1764}, {"ta32", 1784}, {"ta33", 1791}, {"ta34", 1829}, {"ta35", 2007},
        {"ta36", 1819}, {"ta37", 1771}, {"ta38", 1673}, {"ta39", 1795}, {"ta40", 1669},
        {"ta41", 2005}, {"ta42", 1937}, {"ta43", 1846}, {"ta44", 1979}, {"ta45", 2000},
        {"ta46", 2004}, {"ta47", 1889}, {"ta48", 1937}, {"ta49", 1961}, {"ta50", 1923},
        {"ta51", 2760}, {"ta52", 2756}, {"ta53", 2717}, {"ta54", 2839}, {"ta55", 2679},
        {"ta56", 2781}, {"ta57", 2943}, {"ta58", 2885}, {"ta59", 2655}, {"ta60", 2723},
        {"ta61", 2868}, {"ta62", 2869}, {"ta63", 2755}, {"ta64", 2702}, {"ta65", 2725},
        {"ta66", 2845}, {"ta67", 2825}, {"ta68", 2784}, {"ta69", 3071}, {"ta70", 2995},
        {"ta71", 5464}, {"ta72", 5181}, {"ta73", 5568}, {"ta74", 5339}, {"ta75", 5392},
        {"ta76", 5342}, {"ta77", 5436}, {"ta78", 5394}, {"ta79", 5358}, {"ta80", 5183}
    };

    ofstream out("resultados_ta01_ta80.csv");
    if (!out) {
        cerr << "Erro ao criar arquivo de resultados." << endl;
        return 1;
    }

    out << "Instancia,Makespan Obtido,Best Known Solution,Gap,Gap (%)" << endl;

    for (int i = 1; i <= 80; i++) {
        string instance_key = "ta";
        if (i < 10) instance_key += "0";
        instance_key += to_string(i);

        string filename = "Instancias/JSP/" + instance_key + "Jsp.psi";
        string instance_name = instance_key + "Jsp.psi";

        GrafoDisjuntivo g;
        lerInstancia(filename, g);

        if (g.num_operacoes == 0) {
            cout << "Processando " << instance_name << "... Erro" << endl;
            out << instance_name << ",Erro,,," << endl;
            continue;
        }

        resolveHeuristica(g);
        buscaLocal(g);

        vector<int> ordem;
        if (!caminhadaTopologica(g, ordem)) {
            cout << "Processando " << instance_name << "... Erro: Ciclo gerado" << endl;
            out << instance_name << ",Ciclo,,," << endl;
            continue;
        }

        vector<int> caminho_critico;
        int makespan = caminhoMaximo(g, ordem, caminho_critico);
        
        int bks = best_known_solutions[instance_key];
        int gap = makespan - bks;
        double gap_pct = (double)gap / bks * 100.0;

        cout << "Processando " << instance_name << "... Makespan: " << makespan << ", BKS: " << bks << ", Gap: " << gap << endl;

        out << instance_name << "," << makespan << "," << bks << "," << gap << ",";
        out << fixed << setprecision(2) << gap_pct << "%" << endl;
    }

    out.close();
    cout << "\nResultados salvos em: /home/joao-kennedy/Documentos/Algoritmos Para Grafos/resultados_ta01_ta80.csv" << endl;
    return 0;
}
