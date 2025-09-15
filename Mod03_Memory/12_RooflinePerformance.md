# Roofline Performance Model - Material de Estudo Completo

## 🏗️ O Modelo Roofline: Visualizando Limites de Hardware

O modelo Roofline é uma ferramenta visual poderosa que mostra os limites físicos do hardware e ajuda a identificar se um programa está limitado por computação ou memória.

## 📊 Construindo o Gráfico Roofline

### Configuração do Experimento
```cpp
// Tamanho fixo: 4 GB de dados
std::vector<double> data(536870912);  // 4 GB

// Variamos apenas a intensidade aritmética
for(auto& x : data) {
    x = compute_with_N_operations(x);  // N = 2, 4, 8, 16, 32, 80...
}
```

### Eixos do Gráfico
```
Performance (GFLOPS) - Log
   ↑
1000│                    ╱═══════ Teto Computacional
    │                  ╱
100 │                ╱
    │              ╱
10  │            ╱ ← Transição
    │          ╱
1   │  ══════╱ ← Teto de Memória
    │
    └────────────────────────────→ Intensidade Aritmética (flops/byte) - Log
     0.1    0.25   1.0    10     100
```

## 📈 Resultados Experimentais - Single Core

### Dados Medidos
```
Intensidade | GFLOPS | Limitante
------------|--------|----------
0.25        | 3.2    | Memory
0.50        | 6.4    | Memory
1.00        | 12.8   | Memory
2.00        | 25.6   | Memory
5.00        | 37.0   | Transição
10.00       | 37.0   | Compute
```

### Análise dos Regimes

#### Regime Memory-Bound (Esquerda)
```
Performance = β × Intensidade

Onde β = Memory Bandwidth (GB/s)

Exemplo:
β = 12.8 GB/s (single core medido)
Intensidade = 0.25 flops/byte
Performance = 12.8 × 0.25 = 3.2 GFLOPS
```

#### Regime Compute-Bound (Direita)
```
Performance = Constante = Peak FLOPS

Não importa a intensidade, 
performance é limitada pela CPU
```

## 🔍 Comparando com Limites Teóricos

### Bandwidth de Memória

```
Especificação do Hardware: 144 GB/s (todos os cores)
STREAM Benchmark (1 core): 18 GB/s
Nossa medição (1 core): 12.8 GB/s

Gap: 8x entre pico e single core!
```

### Performance Computacional

```
LINPACK (1 core): 100+ GFLOPS
Nossa medição: 37 GFLOPS

Gap: ~3x do pico
```

## 🚀 Performance com Paralelização (24 Cores)

### Resultados Multi-Core
```
Memory-Bound:
Single: 3.2 GFLOPS → Multi: 25.6 GFLOPS (8x speedup)
Limitado por bandwidth compartilhado!

Compute-Bound:
Single: 37 GFLOPS → Multi: 536 GFLOPS (14.5x speedup)
Mais próximo do speedup linear
```

### Gráfico Roofline Completo

```
GFLOPS (Log)
   ↑
1000│                    ═════════ LINPACK Peak (1000)
    │                  ╱
536 │                ╱ ← Nossa performance (multi-core)
    │              ╱
100 │            ╱═══════ Single Core Peak
    │          ╱
37  │        ╱ ← Nossa performance (single)
    │      ╱
25.6│    ╱ ← Multi-core memory bound
    │  ╱
3.2 │╱ ← Single memory bound
    └────────────────────────────→ Intensidade (Log)
     0.25    1.0    5.0    10
```

## 💻 Implementação do Modelo Roofline

```cpp
#include <iostream>
#include <vector>
#include <chrono>
#include <cmath>
#include <algorithm>

class RooflineModel {
private:
    static constexpr size_t DATA_SIZE = 536870912; // 4 GB
    
    // Função com intensidade variável
    template<int OPS>
    static double compute_intensive(double x) {
        double result = x;
        for(int i = 0; i < OPS/2; i++) {
            result = result * 1.001 + 0.001; // 2 flops
        }
        return result;
    }
    
public:
    static void build_roofline() {
        std::cout << "=== Modelo Roofline ===\n\n";
        
        // Medições de referência
        double peak_bandwidth = measure_bandwidth();
        double peak_compute = measure_compute();
        
        std::cout << "Limites do Hardware:\n";
        std::cout << "Peak Bandwidth: " << peak_bandwidth << " GB/s\n";
        std::cout << "Peak Compute: " << peak_compute << " GFLOPS\n\n";
        
        // Ponto de equilíbrio
        double balance_point = peak_compute / peak_bandwidth;
        std::cout << "Balance Point: " << balance_point << " flops/byte\n\n";
        
        // Teste diferentes intensidades
        std::vector<int> operations = {2, 4, 8, 16, 32, 64, 128};
        
        std::cout << "Ops | Intensity | GFLOPS | Efficiency | Regime\n";
        std::cout << "----|-----------|--------|------------|--------\n";
        
        size_t n = DATA_SIZE / sizeof(double);
        std::vector<double> data(n, 1.5);
        
        for(int ops : operations) {
            auto start = std::chrono::high_resolution_clock::now();
            
            // Executa computação
            for(auto& x : data) {
                if(ops == 2) x = compute_intensive<2>(x);
                else if(ops == 4) x = compute_intensive<4>(x);
                else if(ops == 8) x = compute_intensive<8>(x);
                else if(ops == 16) x = compute_intensive<16>(x);
                else if(ops == 32) x = compute_intensive<32>(x);
                else if(ops == 64) x = compute_intensive<64>(x);
                else x = compute_intensive<128>(x);
            }
            
            auto end = std::chrono::high_resolution_clock::now();
            
            // Calcula métricas
            double seconds = std::chrono::duration<double>(end - start).count();
            double gflops = (ops * n) / (seconds * 1e9);
            double intensity = ops / 16.0; // read + write
            
            // Determina regime
            double expected_memory = peak_bandwidth * intensity;
            double expected_compute = peak_compute;
            double expected = std::min(expected_memory, expected_compute);
            double efficiency = gflops / expected * 100;
            
            std::string regime = (intensity < balance_point) ? "Memory" : "Compute";
            
            std::cout << std::setw(3) << ops << " | "
                     << std::fixed << std::setprecision(3)
                     << std::setw(9) << intensity << " | "
                     << std::setw(6) << gflops << " | "
                     << std::setw(10) << efficiency << "% | "
                     << regime << "\n";
        }
        
        // Plota ASCII Roofline
        plot_roofline(peak_bandwidth, peak_compute);
    }
    
private:
    static double measure_bandwidth() {
        size_t n = DATA_SIZE / sizeof(double);
        std::vector<double> a(n, 1.0);
        std::vector<double> b(n, 2.0);
        
        auto start = std::chrono::high_resolution_clock::now();
        
        // STREAM Copy
        for(size_t i = 0; i < n; i++) {
            a[i] = b[i];
        }
        
        auto end = std::chrono::high_resolution_clock::now();
        double seconds = std::chrono::duration<double>(end - start).count();
        
        return (2 * DATA_SIZE) / (seconds * 1e9); // GB/s
    }
    
    static double measure_compute() {
        size_t n = 10000000;
        std::vector<double> data(n, 1.5);
        
        auto start = std::chrono::high_resolution_clock::now();
        
        // Compute intensive
        for(auto& x : data) {
            for(int i = 0; i < 100; i++) {
                x = x * 1.0001 + 0.0001; // 200 flops total
            }
        }
        
        auto end = std::chrono::high_resolution_clock::now();
        double seconds = std::chrono::duration<double>(end - start).count();
        
        return (200 * n) / (seconds * 1e9); // GFLOPS
    }
    
    static void plot_roofline(double bw, double peak) {
        std::cout << "\n=== Gráfico Roofline (ASCII) ===\n\n";
        std::cout << "GFLOPS\n";
        std::cout << "  ↑\n";
        
        double balance = peak / bw;
        
        for(int row = 10; row >= 0; row--) {
            double gflops = std::pow(2, row);
            std::cout << std::setw(4) << (int)gflops << "│";
            
            for(int col = 0; col < 40; col++) {
                double intensity = std::pow(2, (col - 10) / 3.0);
                double roofline = std::min(bw * intensity, peak);
                
                if(std::abs(roofline - gflops) < gflops * 0.2) {
                    if(intensity < balance) std::cout << "╱";
                    else std::cout << "═";
                } else {
                    std::cout << " ";
                }
            }
            std::cout << "\n";
        }
        
        std::cout << "    └";
        for(int i = 0; i < 40; i++) std::cout << "─";
        std::cout << "→ Intensidade (flops/byte)\n";
        std::cout << "     0.1        1.0        10       100\n";
    }
};

int main() {
    RooflineModel::build_roofline();
    return 0;
}
```

## 🎯 Interpretando o Modelo Roofline

### Zonas do Gráfico

1. **Zona Memory-Bound (Esquerda)**
   - Performance cresce linearmente com intensidade
   - Limitada por bandwidth
   - Adicionar operações é "grátis"

2. **Ponto de Equilíbrio**
   - Intensidade = Peak FLOPS / Peak Bandwidth
   - Transição entre regimes

3. **Zona Compute-Bound (Direita)**
   - Performance constante
   - Limitada por CPU
   - Precisa otimizar computação

## 📊 Aplicações Práticas do Roofline

### Diagnóstico de Performance

```
Sua aplicação está:
- Muito abaixo da linha? → Problema de implementação
- Na linha memory-bound? → Otimize acesso à memória
- Na linha compute-bound? → Otimize computação
- No joelho da curva? → Bem balanceada
```

### Guia de Otimização

| Posição no Roofline | Estratégia de Otimização |
|---------------------|-------------------------|
| Memory-bound baixo | Cache blocking, prefetching |
| Memory-bound na linha | Aumentar intensidade aritmética |
| Compute-bound baixo | Vetorização, loop unrolling |
| Compute-bound na linha | Paralelização, GPU |

## 🔑 Lições Principais

1. **Hardware tem limites físicos** - Não dá para superar o roofline
2. **Paralelização tem limites** - Memory bandwidth é compartilhado
3. **Balance point é crucial** - Determina estratégia de otimização
4. **Gap com pico é normal** - 50% do pico já é excelente
5. **Simplicidade funciona** - Código simples chegou a 50% do pico

## 💡 Conclusões Práticas

### Para Memory-Bound:
- Speedup máximo = Peak BW / Single Core BW (~8x)
- Paralelização limitada por bandwidth compartilhado
- Foco: aumentar reuso de dados

### Para Compute-Bound:
- Speedup pode ser quase linear com cores
- Vetorização essencial
- Foco: usar todas unidades funcionais

---

**Reflexão Final:** O modelo Roofline não é apenas uma ferramenta de análise - é um mapa que mostra exatamente onde seu código está e para onde pode ir. Entender se você está limitado por memória ou computação é o primeiro passo para otimização efetiva. Lembre-se: você não pode quebrar as leis da física, mas pode chegar bem perto do limite!