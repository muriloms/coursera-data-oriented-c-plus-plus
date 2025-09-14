# RAM vs CPU: A Batalha pelos Recursos - Material de Estudo Completo

## 🎯 O Dilema Fundamental

Você tem um processador capaz de 12.5 operações por ciclo com vetorização, mas ele fica esperando dados da RAM. Por outro lado, você tem bandwidth de memória de 144 GB/s, mas a CPU não consegue processar tão rápido. Como equilibrar esses recursos?

## 💻 O Experimento Base

### Código de Benchmark
```cpp
#include <vector>
#include <algorithm>

// Transformação linear simples
void linear_transform(std::vector<double>& data) {
    const double A = 2.5;
    const double B = 1.3;
    
    std::transform(data.begin(), data.end(), data.begin(),
        [=](double x) { return A * x + B; }
    );
}
```

**Operações por elemento:** 2 (1 multiplicação + 1 adição)

### Configuração de Compilação
```bash
g++ -O3 -mavx512f -ffast-math
```
- `-mavx512f`: Usa instruções AVX-512
- `-ffast-math`: Permite reordenamento algébrico

## 📊 Resultados Experimentais

### Performance Medida
```
Gráfico Log-Log:
ns/elemento
   ↑
0.64│                    ──────── RAM (memory-bound)
    │              ──────
    │        ──────
0.04│──────── L1 Cache (compute-bound)
    └────────────────────────────────→ Tamanho dos dados
     8KB    256KB    8MB     4GB
```

### Análise dos Extremos

| Localização | Tempo/elemento | Ops/ciclo | Limitante |
|-------------|---------------|-----------|-----------|
| L1 Cache (8KB) | 0.04 ns | 12.5 | CPU |
| RAM (4GB) | 0.64 ns | 1.56 | Memória |

## 🔬 Experimento: Aumentando a Complexidade Computacional

### Aproximação de Exponencial (Série de Taylor)
```cpp
// e^x ≈ 1 + x + x²/2! + x³/3! + x⁴/4! + ...
double exponential_approx(double x) {
    // Esquema de Horner para polinômio de ordem 4
    return 1.0 + x * (1.0 + x * (0.5 + x * (0.166667 + x * 0.041667)));
}
```

**Análise de Operações:**
- Ordem 4: 4 multiplicações + 4 adições = 8 operações
- Ordem n: 2n operações

### Resultados com Diferentes Intensidades Aritméticas

#### Dados em L1 Cache (8 KB)
```
Operações | Tempo (ns) | Aumento
----------|------------|----------
    2     |   0.04     |   1.0x
    4     |   0.07     |   1.75x
    6     |   0.11     |   2.75x
    8     |   0.14     |   3.5x

Comportamento: LINEAR - Dobra ops, dobra tempo
Estado: COMPUTE-BOUND
```

#### Dados em RAM (4 GB)
```
Operações | Tempo (ns) | Aumento
----------|------------|----------
    2     |   0.64     |   1.00x
    4     |   0.65     |   1.02x
    6     |   0.66     |   1.03x
    8     |   0.67     |   1.05x

Comportamento: QUASE CONSTANTE - Ops "grátis"
Estado: MEMORY-BOUND
```

## 🎭 Compute-Bound vs Memory-Bound

### Compute-Bound (Limitado pela CPU)
```
Características:
• Dados cabem em cache
• CPU é o gargalo
• Tempo cresce linearmente com operações
• Vetorização crucial

Visualização:
CPU: ████████████████ (100% uso)
MEM: ██░░░░░░░░░░░░░░ (12% uso)
```

### Memory-Bound (Limitado pela Memória)
```
Características:
• Dados grandes (não cabem em cache)
• Memória é o gargalo
• Operações extras são "grátis"
• Bandwidth é crucial

Visualização:
CPU: ██░░░░░░░░░░░░░░ (12% uso)
MEM: ████████████████ (100% uso)
```

## 📐 Intensidade Aritmética

### Definição
```
Intensidade Aritmética = Operações / Bytes transferidos
```

### Exemplos Práticos

```cpp
// Baixa intensidade (0.25 ops/byte)
void vector_add(double* a, double* b, double* c, int n) {
    for(int i = 0; i < n; i++) {
        c[i] = a[i] + b[i];  // 1 op, 24 bytes (2 reads + 1 write)
    }
}

// Média intensidade (2 ops/byte)
void transform(double* data, int n) {
    for(int i = 0; i < n; i++) {
        data[i] = 2.5 * data[i] + 1.3;  // 2 ops, 16 bytes (read + write)
    }
}

// Alta intensidade (16 ops/byte)
void polynomial(double* data, int n) {
    for(int i = 0; i < n; i++) {
        double x = data[i];
        // Polinômio de ordem 8
        data[i] = ((((((x*0.1 + 0.2)*x + 0.3)*x + 0.4)*x + 
                     0.5)*x + 0.6)*x + 0.7)*x + 0.8;
        // 16 ops, 16 bytes
    }
}
```

## 🔄 Roofline Model

### Visualização do Limite de Performance

```
Performance (GFLOPS)
   ↑
256│     ╱────────── Teto da CPU (compute-bound)
   │    ╱
128│   ╱
   │  ╱ 
64 │ ╱   Memory-bound | Compute-bound
   │╱                 |
32 │                  |
   └──────────────────┼────────────→ Intensidade Aritmética
                      2.0 ops/byte
                   Ponto de Equilíbrio
```

### Cálculo do Ponto de Equilíbrio
```
CPU Peak = 256 GFLOPS (exemplo)
Memory BW = 128 GB/s

Intensidade de Equilíbrio = 256 GFLOPS / 128 GB/s = 2 ops/byte

Se intensidade < 2: Memory-bound
Se intensidade > 2: Compute-bound
```

## 💻 Código Completo: Medindo Compute vs Memory Bound

```cpp
#include <iostream>
#include <vector>
#include <chrono>
#include <cmath>

class ResourceBalance {
private:
    using Clock = std::chrono::high_resolution_clock;
    
    // Diferentes intensidades aritméticas
    static double compute_light(double x) {
        return 2.5 * x + 1.3;  // 2 ops
    }
    
    static double compute_medium(double x) {
        // Polinômio ordem 4 - 8 ops
        return 1.0 + x * (1.0 + x * (0.5 + 
               x * (0.166667 + x * 0.041667)));
    }
    
    static double compute_heavy(double x) {
        // Polinômio ordem 8 - 16 ops
        double x2 = x * x;
        double x4 = x2 * x2;
        double x8 = x4 * x4;
        return 1.0 + x + x2*0.5 + x*x2*0.166667 + 
               x4*0.041667 + x*x4*0.008333 + 
               x2*x4*0.001389 + x*x2*x4*0.000198 + 
               x8*0.0000248;
    }
    
public:
    static void benchmark_intensity() {
        std::cout << "=== Compute-Bound vs Memory-Bound ===\n\n";
        
        // Teste com diferentes tamanhos
        std::vector<size_t> sizes = {
            1024,        // 8 KB - L1 Cache
            131072,      // 1 MB - L2 Cache  
            4194304,     // 32 MB - L3 Cache
            536870912    // 4 GB - RAM
        };
        
        std::vector<std::string> labels = {
            "L1 (8KB)", "L2 (1MB)", "L3 (32MB)", "RAM (4GB)"
        };
        
        for(size_t i = 0; i < sizes.size(); i++) {
            size_t n = sizes[i] / sizeof(double);
            std::vector<double> data(n, 1.5);
            
            std::cout << labels[i] << ":\n";
            
            // Light computation (2 ops)
            auto start = Clock::now();
            for(auto& x : data) x = compute_light(x);
            auto end = Clock::now();
            auto t_light = std::chrono::duration<double>(end - start).count();
            
            // Medium computation (8 ops)
            std::fill(data.begin(), data.end(), 1.5);
            start = Clock::now();
            for(auto& x : data) x = compute_medium(x);
            end = Clock::now();
            auto t_medium = std::chrono::duration<double>(end - start).count();
            
            // Heavy computation (16 ops)
            std::fill(data.begin(), data.end(), 1.5);
            start = Clock::now();
            for(auto& x : data) x = compute_heavy(x);
            end = Clock::now();
            auto t_heavy = std::chrono::duration<double>(end - start).count();
            
            // Análise
            double speedup_med = t_light / t_medium;
            double speedup_heavy = t_light / t_heavy;
            
            std::cout << "  2 ops:  " << t_light * 1e9 / n << " ns/elem\n";
            std::cout << "  8 ops:  " << t_medium * 1e9 / n << " ns/elem";
            std::cout << " (slowdown: " << t_medium/t_light << "x)\n";
            std::cout << "  16 ops: " << t_heavy * 1e9 / n << " ns/elem";
            std::cout << " (slowdown: " << t_heavy/t_light << "x)\n";
            
            // Diagnóstico
            if(t_heavy/t_light > 6) {
                std::cout << "  → COMPUTE-BOUND\n";
            } else if(t_heavy/t_light < 2) {
                std::cout << "  → MEMORY-BOUND\n";
            } else {
                std::cout << "  → BALANCED\n";
            }
            std::cout << "\n";
        }
    }
};

int main() {
    ResourceBalance::benchmark_intensity();
    return 0;
}
```

## 🎯 Estratégias de Otimização

### Para Código Memory-Bound
1. **Aumentar intensidade aritmética** - Mais computação por byte
2. **Melhorar localidade** - Reduzir cache misses
3. **Usar prefetching** - Esconder latência
4. **Comprimir dados** - Menos bytes para transferir

### Para Código Compute-Bound
1. **Vetorização SIMD** - Múltiplas operações por ciclo
2. **Paralelização** - Usar múltiplos cores
3. **Algoritmos mais eficientes** - Menos operações
4. **Usar bibliotecas otimizadas** - MKL, BLAS, etc.

## 📊 Aplicações Reais

| Aplicação | Típico Estado | Intensidade |
|-----------|---------------|-------------|
| BLAS Level 1 (vector ops) | Memory-bound | 0.25 |
| BLAS Level 2 (matrix-vector) | Memory-bound | 1-2 |
| BLAS Level 3 (matrix-matrix) | Compute-bound | 10-100 |
| FFT | Balanced | 2-5 |
| Stencil computations | Memory-bound | 0.5-2 |
| Monte Carlo | Compute-bound | 10+ |

## 🎓 Conceitos-Chave

1. **Compute-Bound:** CPU é o limitante, tempo escala com operações
2. **Memory-Bound:** Memória é o limitante, operações extras são "grátis"
3. **Intensidade Aritmética:** Ops/byte determina o limitante
4. **Roofline Model:** Visualiza limites de performance
5. **Balance Point:** Onde CPU e memória estão equilibrados

## 💡 Regra de Ouro

> "Identifique se seu código é compute-bound ou memory-bound.  
> Então otimize o recurso que é o gargalo.  
> Não adianta vetorizar código memory-bound  
> ou melhorar cache em código compute-bound!"

---

**Reflexão Final:** Performance não é sobre fazer a CPU trabalhar mais rápido ou a memória transferir mais dados - é sobre equilibrar esses recursos para que nenhum fique esperando pelo outro. O segredo está em entender qual recurso é seu limitante e ajustar a intensidade aritmética do seu algoritmo adequadamente.