// Arquivo: 04_vectors_2d.cpp
// Tópico: Vetores 2D - vetor de vetores (matrizes)

#include <iostream>
#include <vector>
using namespace std;

int main() {
    // ========== DECLARAÇÃO DE VETORES 2D ==========
    
    int n = 3;  // Tamanho da matriz (n x n)
    
    // Matriz 3x3 inicializada com zeros
    // vector<vector<double>> é o tipo: vetor de vetores de double
    auto w = vector<vector<double>>(n, vector<double>(n, 0.0));
    //                               ^                 ^
    //                               |                 |
    //                      n linhas (vetores)    cada linha: n doubles = 0.0
    
    // Matriz com valores específicos usando brace initialization
    auto matriz1 = vector<vector<int>>{
        {1, 2, 3},
        {4, 5, 6},
        {7, 8, 9}
    };
    
    // Matriz não quadrada (2x4)
    auto matriz2 = vector<vector<double>>(2, vector<double>(4, 1.5));
    
    // ========== ACESSANDO ELEMENTOS ==========
    
    cout << "=== Acessando elementos da matriz ===" << endl;
    
    // Modificando elementos
    w[0][0] = 1.1;  // linha 0, coluna 0
    w[1][1] = 2.2;  // linha 1, coluna 1
    w[2][2] = 3.3;  // linha 2, coluna 2
    
    cout << "Matriz w (diagonal modificada):" << endl;
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            cout << w[i][j] << "\t";
        }
        cout << endl;
    }
    
    // ========== ITERANDO COM RANGE-BASED FOR ==========
    
    cout << "\n=== Matriz1 usando range-based for ===" << endl;
    for (const auto& linha : matriz1) {  // cada linha é um vetor
        for (auto elemento : linha) {
            cout << elemento << "\t";
        }
        cout << endl;
    }
    
    // ========== MATRIZ DINÂMICA ==========
    
    cout << "\n=== Construindo matriz dinamicamente ===" << endl;
    vector<vector<int>> matrizDinamica;
    
    // Adicionando linhas
    for (int i = 0; i < 3; i++) {
        vector<int> linha;
        for (int j = 0; j < 4; j++) {
            linha.push_back((i + 1) * 10 + j);
        }
        matrizDinamica.push_back(linha);
    }
    
    cout << "Matriz dinâmica:" << endl;
    for (const auto& linha : matrizDinamica) {
        for (auto elemento : linha) {
            cout << elemento << "\t";
        }
        cout << endl;
    }
    
    // ========== INFORMAÇÕES DA MATRIZ ==========
    
    cout << "\n=== Informações das matrizes ===" << endl;
    cout << "matriz1:" << endl;
    cout << "  Número de linhas: " << matriz1.size() << endl;
    cout << "  Número de colunas: " << matriz1[0].size() << endl;
    
    cout << "\nmatriz2:" << endl;
    cout << "  Número de linhas: " << matriz2.size() << endl;
    cout << "  Número de colunas: " << matriz2[0].size() << endl;
    
    // ========== EXEMPLO PRÁTICO: TABELA DE MULTIPLICAÇÃO ==========
    
    cout << "\n=== Tabela de Multiplicação 5x5 ===" << endl;
    int tam = 5;
    auto tabela = vector<vector<int>>(tam, vector<int>(tam));
    
    // Preenchendo a tabela
    for (int i = 0; i < tam; i++) {
        for (int j = 0; j < tam; j++) {
            tabela[i][j] = (i + 1) * (j + 1);
        }
    }
    
    // Exibindo a tabela
    cout << "   ";
    for (int j = 1; j <= tam; j++) cout << j << "\t";
    cout << "\n   ";
    for (int j = 1; j <= tam; j++) cout << "---\t";
    cout << endl;
    
    for (int i = 0; i < tam; i++) {
        cout << i + 1 << " | ";
        for (int j = 0; j < tam; j++) {
            cout << tabela[i][j] << "\t";
        }
        cout << endl;
    }
    
    return 0;
}