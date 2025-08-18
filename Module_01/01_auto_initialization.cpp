// Arquivo: 01_auto_initialization.cpp
// Tópico: Inicialização com auto e tipos fundamentais

#include <iostream>
using namespace std;

int main() {
    // ========== INICIALIZAÇÃO COM AUTO ==========
    // O compilador deduz o tipo da variável baseado no valor inicial
    
    // Inteiro
    auto idade = 25;  // tipo deduzido: int
    
    // Float - note o sufixo 'f' para especificar float
    auto altura = 1.75f;  // tipo deduzido: float
    
    // Double - sem sufixo, números decimais são double por padrão
    auto peso = 70.5;  // tipo deduzido: double
    
    // ========== BRACE INITIALIZATION COM TIPOS FUNDAMENTAIS ==========
    // Previne conversões automáticas indesejadas
    
    int numero1{42};      // OK
    double numero2{3.14}; // OK
    
    // int numero3{3.14};  // ERRO! Previne narrowing conversion
    // Esta linha causaria erro de compilação porque 3.14 não é int
    
    // Inicialização com zero usando braces vazias
    int contador{};     // inicializado com 0
    double saldo{};     // inicializado com 0.0
    
    cout << "=== Valores inicializados com auto ===" << endl;
    cout << "idade: " << idade << " (int)" << endl;
    cout << "altura: " << altura << " (float)" << endl;
    cout << "peso: " << peso << " (double)" << endl;
    
    cout << "\n=== Valores com brace initialization ===" << endl;
    cout << "numero1: " << numero1 << endl;
    cout << "numero2: " << numero2 << endl;
    cout << "contador (inicializado vazio): " << contador << endl;
    cout << "saldo (inicializado vazio): " << saldo << endl;
    
    return 0;
}