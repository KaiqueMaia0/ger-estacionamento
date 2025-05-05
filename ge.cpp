/*Trabalho de Estrutura de  Dados - Gerenciamento de um estacionamento
Discentes:Kaique Maia, Marcos Vinicus, Tacio Moreira e David Ryan
*/

#include <iostream>
#include <queue>
#include <deque>
#include <list>
#include <vector>
#include <fstream>
#include <string>
#include <algorithm>
using namespace std;

string toUpper(const string& str) {
    string result = str;
    transform(result.begin(), result.end(), result.begin(), ::toupper);
    return result;
}


enum TipoCarro { COMUM = 0, PRIORITARIO, EMERGENCIA, SERVICO };

struct Carro {
    string placa;
    TipoCarro tipo;
    int vagaOcupada = -1;
};

string tipoToStr(TipoCarro tipo) {
    switch(tipo) {
        case COMUM: return "Comum";
        case PRIORITARIO: return "Prioritário";
        case EMERGENCIA: return "Emergência";
        case SERVICO: return "Serviço";
        default: return "Desconhecido";
    }
}

void gravarHistorico(const string& acao) {
    ofstream file("historico.txt", ios::app);
    if (file.is_open()) {
        file << acao << endl;
        file.close();
    }
}

//com prioridade
void inserirFilaEntrada(queue<Carro>& filaComum, deque<Carro>& filaPrioritaria, Carro carro) {
    if (carro.tipo == EMERGENCIA || carro.tipo == PRIORITARIO)
        filaPrioritaria.push_back(carro);
    else
        filaComum.push(carro);
    gravarHistorico("Veículo " + carro.placa + " (" + tipoToStr(carro.tipo) + ") inserido na fila de entrada.");
}

//inserir se houver vaga
bool moverParaEstacionamento(queue<Carro>& filaComum, deque<Carro>& filaPrioritaria, list<Carro>& estacionamento, vector<bool>& vagas) {
    int vagaDisponivel = -1;
    for (int i = 0; i < vagas.size(); i++) {
        if (!vagas[i]) {
            vagaDisponivel = i;
            break;
        }
    }
    if (vagaDisponivel == -1) {
        cout << "Estacionamento cheio!\n";
        return false;
    }

    Carro carro;
    if (!filaPrioritaria.empty()) {
        carro = filaPrioritaria.front();
        filaPrioritaria.pop_front();
    } else if (!filaComum.empty()) {
        carro = filaComum.front();
        filaComum.pop();
    } else {
        cout << "Nenhum carro na fila de entrada.\n";
        return false;
    }
    
    carro.vagaOcupada = vagaDisponivel;
    estacionamento.push_back(carro);
    vagas[vagaDisponivel] = true;
    cout << "Carro " << carro.placa << " estacionado na vaga " << vagaDisponivel << ".\n";
    gravarHistorico("Veículo " + carro.placa + " estacionado.");
    return true;
}

    bool placasIguais(string a, string b) {
        transform(a.begin(), a.end(), a.begin(), ::toupper);
        transform(b.begin(), b.end(), b.begin(), ::toupper);
        return a == b;
    }

//move do estacionamento para fila de saída
bool moverParaSaida(list<Carro>& estacionamento, deque<Carro>& filaSaida, vector<bool>& vagas, const string& placa, bool saidaFrente) {
    for (auto it = estacionamento.begin(); it != estacionamento.end(); ++it) {
        if (placasIguais(it->placa, placa)){
            Carro carro = *it;

            int vagaLiberada = it->vagaOcupada;
            if (vagaLiberada >= 0 && vagaLiberada < vagas.size()) {
                vagas[vagaLiberada] = false;
            }

            if (saidaFrente)
                filaSaida.push_front(carro);
            else
                filaSaida.push_back(carro);

            estacionamento.erase(it);

            cout << "Carro " << placa << " movido para fila de saída.\n";
            gravarHistorico("Veículo " + carro.placa + " movido para saída (" + (saidaFrente ? "frente" : "fundo") + ").");
            return true;
        }
    }

    cout << "Carro não encontrado no estacionamento.\n";
    return false;
}

void listarFilaEntrada(queue<Carro> filaComum, deque<Carro> filaPrioritaria) {
    cout << "\nFila Prioritária:\n";
    for (auto& carro : filaPrioritaria)
        cout << "- " << carro.placa << " (" << tipoToStr(carro.tipo) << ")\n";

    cout << "\nFila Comum:\n";
    while (!filaComum.empty()) {
        Carro c = filaComum.front();
        cout << "- " << c.placa << " (" << tipoToStr(c.tipo) << ")\n";
        filaComum.pop();
    }
}

void listarEstacionamento(list<Carro> estacionamento) {
    cout << "\nEstacionamento:\n";
    for (auto& carro : estacionamento)
        cout << "- " << carro.placa << " (" << tipoToStr(carro.tipo) << ")\n";
}

void listarFilaSaida(deque<Carro> filaSaida) {
    cout << "\nFila de Saída:\n";
    for (auto& carro : filaSaida)
        cout << "- " << carro.placa << " (" << tipoToStr(carro.tipo) << ")\n";
}

int main() {
    queue<Carro> filaComum;
    deque<Carro> filaPrioritaria;
    deque<Carro> filaSaida;
    list<Carro> estacionamento;
    int capacidade = 5;
    vector<bool> vagas(capacidade, false);
    int opcao;

    do {
        cout << "\n=== MENU ===\n";
        cout << "1. Inserir carro na fila de entrada\n";
        cout << "2. Mover carro da fila para estacionamento\n";
        cout << "3. Mover carro do estacionamento para fila de saída\n";
        cout << "4. Listar veículos\n";
        cout << "5. Sair\n";
        cout << "Escolha: ";
        cin >> opcao;
        cin.ignore();
    
        switch (opcao) {
            case 1: {
                Carro carro;
                int tipo;
                cout << "Placa: ";
                getline(cin, carro.placa);
                cout << "Tipo (0=Comum, 1=Prioritário, 2=Emergência, 3=Serviço): ";
                cin >> tipo;
                carro.tipo = static_cast<TipoCarro>(tipo);
                inserirFilaEntrada(filaComum, filaPrioritaria, carro);
                break;
            }
    
            case 2:
                moverParaEstacionamento(filaComum, filaPrioritaria, estacionamento, vagas);
                break;
    
            case 3: {
                string placa;
                int portao;
                cout << "Placa do carro a sair: ";
                cin>>placa;
                cout << "Portão de saída (0=Frente, 1=Fundo): ";
                cin >> portao;
                moverParaSaida(estacionamento, filaSaida, vagas, placa, portao == 0);
                break;
            }
    
            case 4:
                listarFilaEntrada(filaComum, filaPrioritaria);
                listarEstacionamento(estacionamento);
                listarFilaSaida(filaSaida);
                break;
    
            case 5:
                cout << "Encerrando o programa...\n";
                break;
    
            default:
                cout << "Opção inválida. Tente novamente.\n";
                break;
        }
    } while (opcao != 5);

    return 0;
}