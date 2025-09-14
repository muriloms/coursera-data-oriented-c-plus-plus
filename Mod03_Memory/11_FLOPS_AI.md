# FLOPS e Intensidade Aritmética - Material de Estudo Completo

## 📏 Definições Fundamentais

### FLOP - Floating Point Operation
**Definição:** Uma operação aritmética executada em número de ponto flutuante (single ou double precision)

**O que conta como 1 FLOP:**
- ✅ Adição
- ✅ Subtração  
- ✅ Multiplicação
- ❌ Divisão (geralmente mais lenta, múltiplos ciclos)

### Convenções de Notação (Confusas mas Importantes!)

```
flop    = 1 operação (singular)
flops   = múltiplas operações (plural)
FLOPS   = operações por segundo (taxa/rate)
```

**Cuidado com a confusão:**
- "The code does 1000 flops" → 1000 operações
- "The code runs at 1000 FLOPS" → 1000 operações/segundo

## 📊 Unidades de Medida de Performance

### Hierarquia de Unidades
```
FLOPS     = FLoating-point Operations Per Second
GFLOPS    = 10⁹ FLOPS (bilhões)
TFLOPS    = 10¹² FLOPS (trilhões)
PFLOPS    = 10¹⁵ FLOPS (quatrilhões)
```

### Notações Alternativas (menos comuns)
```
flop/s    = mais lógico mas menos usado
GFlop/s   = variação encontrada
```

## 🧮 Cálculos de Performance

### Exemplo 1: Dados em L1 Cache
```
Operação: 2 flops por elemento
Tempo: 0.04 ns por elemento

Performance = 2 flops / 0.04 ns
            = 2 flops / (0.04 × 10⁻⁹ s)
            = 50 × 10⁹ FLOPS
            = 50 GFLOPS
```

### Exemplo 2: Dados em RAM
```
2 operações por elemento:
Performance = 2 / 0.64 ns = 3.2 GFLOPS

6 operações por elemento:
Performance = 6 / 0.64 ns = 9.4 GFLOPS

8 operações por elemento:
Performance = 8 / 0.64 ns = 12.5 GFLOPS
```

## 🎯 Intensidade Aritmética

### Definição Formal
```
Intensidade Aritmética = FLOPs executados / Bytes transferidos
```

### Exemplos Práticos

#### Transformação Linear (y = ax + b)
```cpp
for(int i = 0; i < n; i++) {
    data[i] = 2.5 * data[i] + 1.3;
}

Análise:
- FLOPs: 2 (1 multiplicação + 1 adição)
- Bytes: 16 (8 bytes read + 8 bytes write)
- Intensidade: 2/16 = 0.125 flops/byte
```

#### Produto Escalar
```cpp
double dot_product(double* a, double* b, int n) {
    double sum = 0;
    for(int i = 0; i < n; i++) {
        sum += a[i] * b[i];  // 2 flops
    }
    return sum;
}

Análise:
- FLOPs: 2n
- Bytes: 16n (dois arrays lidos)
- Intensidade: 2n/16n = 0.125 flops/byte
```

#### Multiplicação de Matrizes
```cpp
// C = A × B
for(int i = 0; i < n; i++) {
    for(int j = 0; j < n; j++) {
        for(int k = 0; k < n; k++) {
            C[i][j] += A[i][k] * B[k][j];  // 2 flops
        }
    }
}

Análise:
- FLOPs: 2n³
- Bytes: 8n² × 3 (três matrizes)
- Intensidade: 2n³/(24n²) = n/12 flops/byte
```

## 📈 Relação Performance vs Intensidade

### Gráfico de Performance
```
GFLOPS
   ↑
50 │████ L1 Cache (compute-bound)
   │████
40 │████
   │████
30 │████
   │
20 │
   │
10 │                    ████████████ RAM (memory-bound)
   │          ████████████████████████
 3 │████████████████████████████████████
   └────────────────────────────────────→ Intensidade
    0.25    0.5     1.0     2.0    4.0  flops/byte
```

## 💻 Código Completo: Medindo FLOPS

```cpp
#include <iostream>
#include <vector>
#include <chrono>
#include <iomanip>

class FLOPSBenchmark {
private:
    using Clock = std::chrono::high_resolution_clock;
    
    // Diferentes intensidades aritméticas
    template<int OPS>
    static double compute(double x) {
        double result = x;
        for(int i = 0; i < OPS/2; i++) {
            result = result * 1.01 + 0.01;  // 2 flops
        }
        return result;
    }
    
public:
    static void measure_flops() {
        std::cout << "=== Medição de FLOPS e Intensidade Aritmética ===\n\n";
        
        // Diferentes tamanhos para testar cache/RAM
        std::vector<size_t> sizes = {
            1024,        // 8 KB - L1
            131072,      // 1 MB - L2
            536870912    // 4 GB - RAM
        };
        
        std::vector<std::string> labels = {"L1", "L2", "RAM"};
        
        for(size_t idx = 0; idx < sizes.size(); idx++) {
            size_t n = sizes[idx] / sizeof(double);
            std::vector<double> data(n, 1.5);
            
            std::cout << labels[idx] << " (" << sizes[idx]/1024 << " KB):\n";
            std::cout << "Ops  | Time(ns) | GFLOPS | Intensity | State\n";
            std::cout << "-----|----------|--------|-----------|-------\n";
            
            // Testa diferentes números de operações
            for(int ops : {2, 4, 8, 16, 32}) {
                auto start = Clock::now();
                
                // Executa benchmark
                for(auto& x : data) {
                    if(ops == 2) x = compute<2>(x);
                    else if(ops == 4) x = compute<4>(x);
                    else if(ops == 8) x = compute<8>(x);
                    else if(ops == 16) x = compute<16>(x);
                    else x = compute<32>(x);
                }
                
                auto end = Clock::now();
                
                // Calcula métricas
                double seconds = std::chrono::duration<double>(end - start).count();
                double ns_per_elem = seconds * 1e9 / n;
                double gflops = (ops * n) / (seconds * 1e9);
                double intensity = ops / 16.0;  // 16 bytes por elemento (read+write)
                
                // Determina se é compute ou memory bound
                std::string state = (idx == 0 && ops > 8) ? "Compute" : 
                                   (idx == 2 && ops < 16) ? "Memory" : "Mixed";
                
                std::cout << std::setw(4) << ops << " | "
                         << std::fixed << std::setprecision(3) 
                         << std::setw(8) << ns_per_elem << " | "
                         << std::setw(6) << gflops << " | "
                         << std::setw(9) << intensity << " | "
                         << state << "\n";
            }
            std::cout << "\n";
        }
    }
    
    static void theoretical_limits() {
        std::cout << "=== Limites Teóricos do Hardware ===\n\n";
        
        // Exemplo: CPU moderna
        double cpu_freq = 3.5;  // GHz
        int cores = 8;
        int simd_width = 8;  // AVX-512 doubles
        int fma_units = 2;   // Unidades FMA por core
        
        double peak_flops = cpu_freq * cores * simd_width * fma_units * 2;
        // ×2 porque FMA = 2 operações
        
        std::cout << "CPU: " << cpu_freq << " GHz × " 
                  << cores << " cores × "
                  << simd_width << " SIMD × "
                  << fma_units << " FMA units × 2 ops\n";
        std::cout << "Peak Performance: " << peak_flops << " GFLOPS\n\n";
        
        // Bandwidth de memória
        double memory_bw = 128;  // GB/s
        
        std::cout << "Memory Bandwidth: " << memory_bw << " GB/s\n";
        std::cout << "Balance Point: " << peak_flops/memory_bw 
                  << " flops/byte\n\n";
        
        // Tabela de intensidades típicas
        std::cout << "Operação           | Intensidade | Limitante\n";
        std::cout << "-------------------|-------------|----------\n";
        std::cout << "Vector Add (y=x+y) | 0.17        | Memory\n";
        std::cout << "DAXPY (y=ax+y)     | 0.25        | Memory\n";
        std::cout << "Dot Product        | 0.25        | Memory\n";
        std::cout << "Matrix-Vector      | 1.0         | Memory\n";
        std::cout << "FFT                | 2-5         | Mixed\n";
        std::cout << "Matrix-Matrix      | n/4         | Compute\n";
    }
};

int main() {
    FLOPSBenchmark::measure_flops();
    FLOPSBenchmark::theoretical_limits();
    return 0;
}
```

## 🎯 Determinando o Limitante

### Memory-Bound
```
Características:
• Performance não melhora com mais operações
• GFLOPS baixo comparado ao pico teórico
• Intensidade < 1-2 flops/byte
• Adicionar operações é "grátis"
```

### Compute-Bound
```
Características:
• Performance piora linearmente com mais operações
• GFLOPS próximo ao pico teórico
• Intensidade > 2-4 flops/byte
• CPU está saturada
```

## 📊 Performance Típica de Hardware Moderno

| Componente | Performance Típica |
|------------|-------------------|
| Core i9 (1 core) | 50-100 GFLOPS |
| Core i9 (todos cores) | 400-800 GFLOPS |
| RTX 3090 | 35 TFLOPS |
| A100 GPU | 19.5 TFLOPS (FP64) |
| Supercomputador TOP500 #1 | >1 EFLOPS |

## 🔑 Conceitos-Chave para Memorizar

1. **FLOP:** Uma operação aritmética em ponto flutuante
2. **FLOPS:** Taxa de operações por segundo (performance)
3. **Intensidade Aritmética:** FLOPs/byte transferido
4. **Memory-Bound:** Limitado por bandwidth de memória
5. **Compute-Bound:** Limitado por poder computacional

## 💡 Aplicação Prática

### Como Melhorar Performance:

**Se Memory-Bound:**
- Aumentar intensidade aritmética
- Melhorar reuso de cache
- Usar algoritmos cache-oblivious
- Comprimir dados

**Se Compute-Bound:**
- Vetorização (SIMD)
- Paralelização (threads)
- Algoritmos mais eficientes
- GPUs para alta intensidade

---

**Insight Final:** FLOPS é uma métrica fundamental, mas sozinha não conta toda a história. A relação entre poder computacional e bandwidth de memória, expressa pela intensidade aritmética, é que determina se seu código está aproveitando o hardware eficientemente. Entender essa balança é essencial para otimização real!