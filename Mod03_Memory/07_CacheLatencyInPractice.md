# Benchmark de Cache - Material de Estudo Completo

## 🎯 Objetivo do Benchmark

Medir empiricamente o impacto da hierarquia de cache em código real, usando padrões de acesso que violam os princípios de localidade para revelar as verdadeiras latências de cada nível.

## 🔬 Design do Experimento

### Estrutura de Dados

```cpp
std::vector<int64_t> data;  // Vector de inteiros de 64 bits
// Tamanho n varia para testar diferentes níveis de cache
```

### Algoritmo de Traversal Indexado

```cpp
// Cada elemento aponta para o próximo índice
// Exemplo visual:
Index:  [0] [1] [2] [3] [4] [5] [6] [7]
Value:  [3] [5] [6] [2] [0] [4] [7] [1]

Ordem de travessia: 0→3→2→6→7→1→5→4→0
```

### Código do Benchmark

```cpp
int64_t sum = 0;
int64_t i = 0;

// Loop principal - percorre todos elementos
for(size_t iter = 0; iter < n; iter++) {
    i = data[i];     // Próximo índice vem do valor atual
    sum += i;        // Operação simples para evitar otimização
}
```

## 📊 Resultados Experimentais

### Medições Principais

| Tamanho do Vector | Tempo por Elemento | Observação |
|-------------------|-------------------|------------|
| 2,048 elementos (16 KB) | 1.51 ns | Cabe em L1 |
| 4 bilhões elementos (32 GB) | 118 ns | Excede todos caches |

**Diferença:** 78x mais lento para dados grandes!

## 📈 Análise Completa por Tamanho

### Gráfico de Performance (Log-Log)

```
Tempo (ns)
   ↑
118│                                    ────── RAM
   │                              ──────
   │                        ──────
 21│                  ────── L3 Cache
   │            ──────
3.4│      ────── L2 Cache
1.5│────── L1 Cache
   └────────────────────────────────────→ Tamanho (bytes)
    2¹¹   2¹⁵(32KB) 2²⁰(1MB)  2²⁵(32MB)  2³²(4GB)
```

### Pontos de Transição Identificados

```
┌─────────────────────────────────────────────┐
│  Região L1 (≤32 KB)                        │
│  • Latência: 1.5 ns                        │
│  • Todos dados cabem em L1                 │
│  • Performance constante                    │
├─────────────────────────────────────────────┤
│  Região L2 (32 KB - 1 MB)                  │
│  • Latência: ~3.4 ns                       │
│  • Dados excedem L1, cabem em L2           │
│  • Pequena variação por compartilhamento   │
├─────────────────────────────────────────────┤
│  Região L3 (1 MB - 37 MB)                  │
│  • Latência: ~21 ns                        │
│  • Dados excedem L2, cabem em L3           │
│  • Compartilhado entre cores               │
├─────────────────────────────────────────────┤
│  Região RAM (>37 MB)                       │
│  • Latência: ~112 ns                       │
│  • Cache totalmente ineficaz               │
│  • Cada acesso vai até a RAM               │
└─────────────────────────────────────────────┘
```

## 🔍 Análise Detalhada das Latências

### Latência Efetiva vs Raw

```cpp
// Latência medida = Latência raw + Overhead do programa
L1 Efetiva: 1.5 ns = 1.0 ns (raw) + 0.5 ns (overhead)

// Overhead inclui:
// - Decodificação de instruções
// - Cálculo de endereços
// - Operação aritmética (sum)
```

### Conversão para Ciclos de CPU

```
CPU @ 2 GHz (normal):
• 1 ciclo = 0.5 ns
• L1 = 3 ciclos
• L2 = 7 ciclos
• L3 = 42 ciclos
• RAM = 224 ciclos

CPU @ 4 GHz (turbo):
• 1 ciclo = 0.25 ns
• L1 = 6 ciclos
• L2 = 14 ciclos
• L3 = 84 ciclos
• RAM = 448 ciclos
```

## 💡 Por Que Acesso Aleatório é Tão Ruim?

### Violação de Localidade

```cpp
// Padrão aleatório - Cache não consegue prever
data[0] → data[847] → data[123] → data[999] → ...

// Cada acesso:
// 1. Miss em L1 (busca em L2)
// 2. Miss em L2 (busca em L3)
// 3. Miss em L3 (busca em RAM)
// 4. Carrega cache line inteira (64 bytes)
// 5. Usa apenas 8 bytes
// 6. Desperdiça 56 bytes!
```

### Comparação com Acesso Sequencial (Próximo Vídeo)

```cpp
// Sequencial - Cache funciona perfeitamente
data[0] → data[1] → data[2] → data[3] → ...

// Cache line de 64 bytes = 8 elementos int64_t
// Amortiza latência sobre 8 acessos!
```

## 💻 Implementação Completa do Benchmark

```cpp
#include <iostream>
#include <vector>
#include <chrono>
#include <random>
#include <algorithm>
#include <cmath>

class CacheBenchmark {
private:
    std::vector<int64_t> data;
    size_t size;
    
    // Inicializa padrão de acesso aleatório
    void initializeRandomPattern() {
        // Cria permutação aleatória
        for(size_t i = 0; i < size; i++) {
            data[i] = i;
        }
        
        std::random_device rd;
        std::mt19937 gen(rd());
        std::shuffle(data.begin(), data.end(), gen);
        
        // Garante ciclo completo
        // Encontra ciclos e conecta
        std::vector<bool> visited(size, false);
        size_t current = 0;
        size_t start = 0;
        
        for(size_t count = 0; count < size; count++) {
            visited[current] = true;
            size_t next = data[current];
            
            if(visited[next]) {
                // Encontrou fim de ciclo
                // Procura próximo não visitado
                size_t unvisited = 0;
                while(unvisited < size && visited[unvisited]) {
                    unvisited++;
                }
                
                if(unvisited < size) {
                    data[current] = unvisited;
                    current = unvisited;
                } else {
                    // Todos visitados, fecha ciclo
                    data[current] = start;
                    break;
                }
            } else {
                current = next;
            }
        }
    }
    
public:
    CacheBenchmark(size_t bytes) {
        size = bytes / sizeof(int64_t);
        data.resize(size);
        initializeRandomPattern();
    }
    
    double measureLatency(size_t iterations) {
        using Clock = std::chrono::high_resolution_clock;
        
        // Warm-up
        int64_t dummy = 0;
        int64_t idx = 0;
        for(size_t i = 0; i < size; i++) {
            idx = data[idx];
            dummy += idx;
        }
        
        // Medição real
        auto start = Clock::now();
        
        int64_t sum = 0;
        idx = 0;
        for(size_t iter = 0; iter < iterations; iter++) {
            for(size_t i = 0; i < size; i++) {
                idx = data[idx];
                sum += idx;
            }
        }
        
        auto end = Clock::now();
        
        // Evita otimização
        if(sum == dummy) std::cout << "";
        
        auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start);
        return duration.count() / (double)(iterations * size);
    }
};

int main() {
    std::cout << "=== Cache Latency Benchmark ===\n\n";
    std::cout << "Size (KB)\tLatency (ns)\tCache Level\n";
    std::cout << "─────────\t────────────\t───────────\n";
    
    // Testa diferentes tamanhos
    std::vector<size_t> sizes;
    
    // L1 range: 2KB até 32KB
    for(int kb = 2; kb <= 32; kb *= 2) {
        sizes.push_back(kb * 1024);
    }
    
    // L2 range: 64KB até 1MB
    for(int kb = 64; kb <= 1024; kb *= 2) {
        sizes.push_back(kb * 1024);
    }
    
    // L3 range: 2MB até 32MB
    for(int mb = 2; mb <= 32; mb *= 2) {
        sizes.push_back(mb * 1024 * 1024);
    }
    
    // RAM: 64MB até 512MB
    for(int mb = 64; mb <= 512; mb *= 2) {
        sizes.push_back(mb * 1024 * 1024);
    }
    
    for(size_t size : sizes) {
        CacheBenchmark bench(size);
        
        // Ajusta iterações baseado no tamanho
        size_t iterations = std::max(size_t(1), size_t(100000000) / size);
        
        double latency = bench.measureLatency(iterations);
        
        // Determina nível de cache
        std::string level;
        if(size <= 32 * 1024) level = "L1";
        else if(size <= 1024 * 1024) level = "L2";
        else if(size <= 37 * 1024 * 1024) level = "L3";
        else level = "RAM";
        
        // Output formatado
        if(size < 1024 * 1024) {
            std::cout << size / 1024 << " KB\t\t";
        } else {
            std::cout << size / (1024 * 1024) << " MB\t\t";
        }
        
        std::cout << std::fixed << std::setprecision(2) 
                  << latency << "\t\t" << level << "\n";
    }
    
    return 0;
}
```

## 📊 Insights do Benchmark

### 1. Cache é Tudo-ou-Nada para Acesso Aleatório
- Se dados cabem totalmente: latência mínima
- Se não cabem: latência máxima
- Não há meio termo com padrão aleatório

### 2. Overhead do Programa é Mensurável
- Raw latency ≠ Effective latency
- Overhead ~0.5 ns para operações básicas
- Importante considerar em otimizações

### 3. Hierarquia Claramente Visível
- Transições nítidas entre níveis
- Cada nível ~3-5x mais lento que anterior
- RAM é 75x mais lenta que L1!

## 🎓 Lições Aprendidas

### Para Performance Crítica:
1. **Mantenha working set pequeno** - Caiba em L1/L2
2. **Evite acesso aleatório** - Destruidor de cache
3. **Meça, não assuma** - Hardware varia
4. **Entenda seus limites** - 32KB, 1MB, 37MB

### Referência Importante:
> "What Every Programmer Should Know About Memory" - Ulrich Drepper (2007)
> Artigo fundamental que inspirou este benchmark

## 🔍 Exercícios Práticos

### Exercício 1: Previsão
Qual será a latência para um vector de 500 KB?

### Exercício 2: Modificação
Modifique o código para medir latência de escrita vs leitura.

### Exercício 3: Comparação
Execute o benchmark em sua máquina e compare com os resultados apresentados.

## 🚀 Próximos Passos

No próximo vídeo, veremos o mesmo benchmark com **acesso sequencial** - a diferença será dramática e revelará o verdadeiro poder do cache quando usado corretamente!

---

💭 **Reflexão Final:** Este benchmark prova empiricamente que ignorar a hierarquia de cache pode tornar seu código 75x mais lento. Não é teoria - é realidade mensurável em qualquer computador moderno!