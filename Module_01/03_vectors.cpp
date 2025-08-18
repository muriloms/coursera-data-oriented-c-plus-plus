// Arquivo: 03_vectors.cpp
// Tópico: Vetores - containers dinâmicos para grandes datasets

#include <iostream>
#include <vector>
using namespace std;

int main() {
    // ========== DECLARAÇÃO DE VETORES ==========
    
    // Vetor com brace initialization
    auto v1 = vector<double>{1.0, 2.0, 3.0};  // 3 doubles
    
    // Vetor com tamanho definido em tempo de execução
    int n = 10;
    auto v2 = vector<double>(n, 1.0);  // 10 doubles, todos = 1.0
    
    // Vetor vazio
    vector<int> v3;  // tamanho 0, cresce dinamicamente
    
    // Vetor com tamanho mas sem valor inicial específico
    vector<int> v4(5);  // 5 inteiros inicializados com 0
    
    // ========== OPERAÇÕES COM VETORES ==========
    
    cout << "=== Vetor v1 inicial ===" << endl;
    cout << "v1: ";
    for (auto valor : v1) {
        cout << valor << " ";
    }
    cout << "\nTamanho: " << v1.size() << endl;
    
    // Adicionando elementos
    v1.push_back(4.0);  // adiciona ao final
    v1.push_back(5.0);
    
    cout << "\n=== Após push_back ===" << endl;
    cout << "v1: ";
    for (auto valor : v1) {
        cout << valor << " ";
    }
    cout << "\nNovo tamanho: " << v1.size() << endl;
    
    // Acessando elementos
    cout << "\n=== Acessando elementos ===" << endl;
    cout << "v1[0] = " << v1[0] << endl;
    cout << "v1.at(2) = " << v1.at(2) << endl;  // com verificação de limites
    cout << "Primeiro: " << v1.front() << endl;
    cout << "Último: " << v1.back() << endl;
    
    // Modificando elementos
    v1[1] = 99.9;
    cout << "\nApós v1[1] = 99.9: ";
    for (auto valor : v1) {
        cout << valor << " ";
    }
    cout << endl;
    
    // ========== VETOR DINÂMICO ==========
    
    cout << "\n=== Vetor dinâmico v3 ===" << endl;
    cout << "Tamanho inicial de v3: " << v3.size() << endl;
    
    // Adicionando elementos dinamicamente
    for (int i = 1; i <= 5; i++) {
        v3.push_back(i * 10);
    }
    
    cout << "v3 após inserções: ";
    for (auto valor : v3) {
        cout << valor << " ";
    }
    cout << "\nTamanho final: " << v3.size() << endl;
    
    // Capacidade vs Tamanho
    cout << "\n=== Capacidade vs Tamanho ===" << endl;
    cout << "v3.size() = " << v3.size() << endl;
    cout << "v3.capacity() = " << v3.capacity() << " (memória alocada)" << endl;
    
    // Limpando o vetor
    v3.clear();
    cout << "\nApós clear():" << endl;
    cout << "v3.size() = " << v3.size() << endl;
    cout << "v3.capacity() = " << v3.capacity() << " (memória ainda alocada)" << endl;
    
    return 0;
}