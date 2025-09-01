// Arquivo: 05_arrays_vs_vectors.cpp
// Tópico: Quando usar arrays vs vectors

#include <iostream>
#include <array>
#include <vector>
#include <chrono>  // para medir tempo
using namespace std;

int main() {
    cout << "=== ARRAYS vs VECTORS - Comparação ===" << endl;
    cout << "======================================" << endl;
    
    // ========== ARRAYS: Tamanho fixo em tempo de compilação ==========
    
    cout << "\n--- ARRAYS (std::array) ---" << endl;
    cout << "✓ Tamanho FIXO definido em COMPILAÇÃO" << endl;
    cout << "✓ Alocado na STACK (rápido)" << endl;
    cout << "✓ Ideal para pequenos conjuntos de dados" << endl;
    cout << "✓ Overhead mínimo de memória" << endl;
    
    // O tamanho DEVE ser conhecido em tempo de compilação
    constexpr int tamanhoArray = 5;  // constexpr = constante de compilação
    auto arr = array<int, tamanhoArray>{10, 20, 30, 40, 50};
    
    // int n = 5;
    // array<int, n> arr2;  // ERRO! n não é constante de compilação
    
    cout << "\nExemplo array: ";
    for (auto val : arr) cout << val << " ";
    cout << "\nTamanho: " << arr.size() << " (fixo)" << endl;
    
    // ========== VECTORS: Tamanho dinâmico ==========
    
    cout << "\n--- VECTORS (std::vector) ---" << endl;
    cout << "✓ Tamanho DINÂMICO definido em EXECUÇÃO" << endl;
    cout << "✓ Alocado no HEAP (mais flexível)" << endl;
    cout << "✓ Ideal para grandes conjuntos de dados" << endl;
    cout << "✓ Pode crescer/diminuir durante execução" << endl;
    
    // Tamanho pode ser definido em tempo de execução
    cout << "\nDigite o tamanho do vetor: ";
    int n;
    cin >> n;
    
    auto vec = vector<int>(n);  // n definido pelo usuário!
    for (int i = 0; i < n; i++) {
        vec[i] = (i + 1) * 100;
    }
    
    cout << "Vetor criado: ";
    for (auto val : vec) cout << val << " ";
    cout << "\nTamanho inicial: " << vec.size() << endl;
    
    // Vetor pode crescer
    vec.push_back(999);
    cout << "Após push_back(999), tamanho: " << vec.size() << endl;
    
    // ========== EXEMPLO PRÁTICO DE USO ==========
    
    cout << "\n=== QUANDO USAR CADA UM ===" << endl;
    
    // ARRAY: Coordenadas 3D (sempre 3 valores)
    auto coordenada3D = array<double, 3>{1.0, 2.5, 3.7};
    cout << "\nArray para coordenada 3D (sempre 3 valores): ";
    cout << "(" << coordenada3D[0] << ", " 
         << coordenada3D[1] << ", " 
         << coordenada3D[2] << ")" << endl;
    
    // ARRAY: Dias da semana (sempre 7)
    auto diasSemana = array<string, 7>{
        "Dom", "Seg", "Ter", "Qua", "Qui", "Sex", "Sab"
    };
    
    // VECTOR: Lista de estudantes (quantidade variável)
    vector<string> estudantes;
    estudantes.push_back("Ana");
    estudantes.push_back("Bruno");
    estudantes.push_back("Carlos");
    // Pode adicionar mais estudantes conforme necessário
    
    cout << "\nVector para lista de estudantes (quantidade variável): ";
    for (const auto& nome : estudantes) {
        cout << nome << " ";
    }
    cout << "\n(Pode adicionar/remover estudantes)" << endl;
    
    // ========== PERFORMANCE BÁSICA ==========
    
    cout << "\n=== TESTE DE PERFORMANCE ===" << endl;
    const int TESTE_SIZE = 1000000;
    
    // Vector
    auto start = chrono::high_resolution_clock::now();
    vector<int> vecTeste(TESTE_SIZE);
    for (int i = 0; i < TESTE_SIZE; i++) {
        vecTeste[i] = i;
    }
    auto end = chrono::high_resolution_clock::now();
    auto durationVec = chrono::duration_cast<chrono::microseconds>(end - start);
    
    cout << "Vector (" << TESTE_SIZE << " elementos): " 
         << durationVec.count() << " microsegundos" << endl;
    
    // ========== RESUMO ==========
    
    cout << "\n=== RESUMO ===" << endl;
    cout << "USE ARRAY quando:" << endl;
    cout << "  • Tamanho é conhecido e fixo" << endl;
    cout << "  • Dataset pequeno (< 1000 elementos)" << endl;
    cout << "  • Performance crítica" << endl;
    cout << "  • Ex: coordenadas, matrizes pequenas, buffers fixos" << endl;
    
    cout << "\nUSE VECTOR quando:" << endl;
    cout << "  • Tamanho pode variar" << endl;
    cout << "  • Dataset grande" << endl;
    cout << "  • Precisa adicionar/remover elementos" << endl;
    cout << "  • Ex: listas dinâmicas, dados de arquivos, resultados de cálculos" << endl;
    
    return 0;
}