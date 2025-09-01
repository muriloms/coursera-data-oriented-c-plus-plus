// Arquivo: 02_arrays.cpp
// Tópico: Declaração e uso de arrays com std::array

#include <iostream>
#include <array>  // Necessário para std::array
using namespace std;

int main() {
    // ========== DECLARAÇÃO DE ARRAYS ==========
    
    // Array não inicializado (NÃO RECOMENDADO!)
    array<int, 4> meuArray1;  // 4 inteiros, valores indefinidos
    
    // Array com brace initialization (RECOMENDADO!)
    auto meuArray = array<int, 4>{1, 2, 3, 4};  // auto deduz o tipo
    
    // Array inicializado com zeros usando braces vazias
    auto arrayZeros = array<int, 5>{};  // todos elementos = 0
    
    // Array parcialmente inicializado
    auto arrayParcial = array<double, 6>{1.0, 2.0, 3.0}; 
    // Primeiros 3 elementos: 1.0, 2.0, 3.0
    // Últimos 3 elementos: 0.0 (default)
    
    // Compilador deduz tipo e tamanho
    auto a3 = array{10, 20, 30};  // array<int, 3>
    
    // ========== ACESSANDO ELEMENTOS ==========
    
    cout << "=== Acessando elementos por índice ===" << endl;
    cout << "meuArray[0] = " << meuArray[0] << endl;  // primeiro elemento
    cout << "meuArray[2] = " << meuArray[2] << endl;  // terceiro elemento
    
    // Modificando um elemento
    meuArray[1] = 99;
    cout << "Após meuArray[1] = 99: " << meuArray[1] << endl;
    
    // ========== ITERANDO SOBRE ARRAYS ==========
    
    cout << "\n=== Iteração com range-based for ===" << endl;
    cout << "meuArray: ";
    for (auto valor : meuArray) {  // valor copia cada elemento
        cout << valor << " ";
    }
    cout << endl;
    
    cout << "\nArrayParcial (note os zeros): ";
    for (auto valor : arrayParcial) {
        cout << valor << " ";
    }
    cout << endl;
    
    // Iteração com referência (para modificar)
    cout << "\n=== Multiplicando todos por 2 ===" << endl;
    for (auto& valor : meuArray) {  // & = referência
        valor *= 2;
    }
    
    cout << "meuArray após multiplicação: ";
    for (auto valor : meuArray) {
        cout << valor << " ";
    }
    cout << endl;
    
    // ========== TAMANHO DO ARRAY ==========
    cout << "\n=== Informações do array ===" << endl;
    cout << "Tamanho de meuArray: " << meuArray.size() << endl;
    cout << "Primeiro elemento: " << meuArray.front() << endl;
    cout << "Último elemento: " << meuArray.back() << endl;
    
    return 0;
}