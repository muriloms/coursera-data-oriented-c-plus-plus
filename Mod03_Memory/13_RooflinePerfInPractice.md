# Modelo Roofline na Prática - Material de Estudo Completo

## 🎯 Usando o Roofline para Otimização Real

O modelo Roofline não é apenas teórico - é uma ferramenta prática que guia suas decisões de otimização. Cada ponto no gráfico representa uma versão do seu código, e você pode mover esse ponto através de diferentes estratégias.

## 📍 Movendo o Ponto no Gráfico

### Seu Código como um Ponto
```
Performance (GFLOPS)
   ↑
   │      ╱═══════ Teto Computacional
   │    ╱
   │  ╱     ← Para cima: Melhor performance
   │╱       ← Para direita: Maior intensidade
   │• ← Seu código atual
   └────────────────→ Intensidade Aritmética
```

**Objetivo:** Mover o ponto para cima (melhor performance)

## 🔧 Cenário 1: Compute-Bound

### Diagnóstico
```
Seu código está aqui:
   │═══════════ Teto CPU
   │     • ← Limitado por computação
   │   ╱
   │ ╱
   └────────────→
```

### Estratégias de Otimização

#### 1. Otimização do Compilador
```bash
# Testar diferentes flags
-O2 vs -O3
-march=native
-ffast-math
-funroll-loops

# Testar diferentes compiladores
gcc vs clang vs icc
```

#### 2. Vetorização
```cpp
// Antes - compilador pode não vetorizar
for(int i = 0; i < n; i++) {
    if(data[i] > 0) result[i] = sqrt(data[i]);
}

// Depois - vetorizável
for(int i = 0; i < n; i++) {
    float mask = (data[i] > 0) ? 1.0f : 0.0f;
    result[i] = mask * sqrt(fabs(data[i]) + !mask);
}
```

#### 3. Usar Bibliotecas Otimizadas
```cpp
// Ao invés de implementar você mesmo:
for(int i = 0; i < n; i++)
    for(int j = 0; j < n; j++)
        for(int k = 0; k < n; k++)
            C[i][j] += A[i][k] * B[k][j];

// Use bibliotecas otimizadas:
cblas_dgemm(CblasRowMajor, CblasNoTrans, CblasNoTrans,
            n, n, n, 1.0, A, n, B, n, 0.0, C, n);
```

## 🔧 Cenário 2: Memory-Bound (Saturando Bandwidth)

### Diagnóstico
```
   │      ╱═══════
   │    •  ← Na linha de bandwidth
   │  ╱      (saturando memória)
   │╱
   └────────────→
```

### Única Solução: Aumentar Intensidade Aritmética

#### Estratégia: Loop Fusion
```cpp
// Antes - 3 passadas pela memória
for(int i = 0; i < n; i++) a[i] *= 2;
for(int i = 0; i < n; i++) b[i] = a[i] + 1;
for(int i = 0; i < n; i++) c[i] = b[i] * b[i];

// Depois - 1 passada
for(int i = 0; i < n; i++) {
    a[i] *= 2;
    b[i] = a[i] + 1;
    c[i] = b[i] * b[i];
}
```

#### Estratégia: Recomputação vs Armazenamento
```cpp
// Antes - armazena valores intermediários
for(int i = 0; i < n; i++) temp[i] = expensive_function(data[i]);
for(int i = 0; i < n; i++) result[i] = process(temp[i]);

// Depois - recomputa quando necessário
for(int i = 0; i < n; i++) {
    double val = expensive_function(data[i]);
    result[i] = process(val);
}
```

## 🔧 Cenário 3: Memory-Bound (Não Saturando) - MAIS COMUM!

### Diagnóstico
```
   │      ╱═══════
   │    ╱
   │  • ← Abaixo da linha
   │╱      (desperdiçando bandwidth)
   └────────────→
```

### Múltiplas Oportunidades de Otimização

#### 1. Melhorar Padrão de Acesso
```cpp
// Ruim - stride grande
for(int i = 0; i < n; i++)
    for(int j = 0; j < n; j++)
        sum += matrix[j][i];  // Acesso por coluna

// Bom - acesso contíguo
for(int i = 0; i < n; i++)
    for(int j = 0; j < n; j++)
        sum += matrix[i][j];  // Acesso por linha
```

#### 2. Estruturas de Dados Compactas
```cpp
// Ruim - desperdício de cache
struct Particle {
    double x, y, z;        // 24 bytes
    char name[100];        // 100 bytes desperdiçados
    int id;                // 4 bytes
    char padding[28];      // Alinhamento
};  // Total: 156 bytes por partícula

// Bom - compacto
struct CompactParticle {
    float x, y, z;         // 12 bytes
    int id;                // 4 bytes
};  // Total: 16 bytes (10x menor!)
```

#### 3. Cache Blocking
```cpp
// Ruim - muitos cache misses
for(int i = 0; i < n; i++)
    for(int j = 0; j < n; j++)
        for(int k = 0; k < n; k++)
            C[i][j] += A[i][k] * B[k][j];

// Bom - processa blocos que cabem em cache
int block = 64;
for(int ii = 0; ii < n; ii += block)
    for(int jj = 0; jj < n; jj += block)
        for(int kk = 0; kk < n; kk += block)
            for(int i = ii; i < min(ii+block,n); i++)
                for(int j = jj; j < min(jj+block,n); j++)
                    for(int k = kk; k < min(kk+block,n); k++)
                        C[i][j] += A[i][k] * B[k][j];
```

## 💻 Código Prático: Aplicando Otimizações

```cpp
#include <iostream>
#include <vector>
#include <chrono>

class RooflineOptimization {
public:
    // Versão inicial - não otimizada
    static double baseline(std::vector<double>& data) {
        double sum = 0;
        for(size_t i = 0; i < data.size(); i++) {
            sum += data[i];
        }
        return sum;
    }
    
    // Otimização 1: Loop unrolling
    static double unrolled(std::vector<double>& data) {
        double sum0 = 0, sum1 = 0, sum2 = 0, sum3 = 0;
        size_t i = 0;
        
        for(; i <= data.size() - 4; i += 4) {
            sum0 += data[i];
            sum1 += data[i+1];
            sum2 += data[i+2];
            sum3 += data[i+3];
        }
        
        for(; i < data.size(); i++) {
            sum0 += data[i];
        }
        
        return sum0 + sum1 + sum2 + sum3;
    }
    
    // Otimização 2: Aumentar intensidade
    static double higher_intensity(std::vector<double>& data) {
        double sum = 0;
        for(size_t i = 0; i < data.size(); i++) {
            double x = data[i];
            // Mais operações por elemento carregado
            double x2 = x * x;
            double x3 = x2 * x;
            sum += x + x2 * 0.5 + x3 * 0.333;
        }
        return sum;
    }
    
    // Otimização 3: Cache blocking
    static double blocked(std::vector<double>& data) {
        const size_t BLOCK = 8192; // Ajustado para L1
        double sum = 0;
        
        for(size_t block = 0; block < data.size(); block += BLOCK) {
            size_t end = std::min(block + BLOCK, data.size());
            for(size_t i = block; i < end; i++) {
                sum += data[i];
            }
        }
        return sum;
    }
    
    static void compare_optimizations() {
        const size_t SIZE = 100'000'000;
        std::vector<double> data(SIZE, 1.0);
        
        std::cout << "=== Comparação de Otimizações ===\n\n";
        
        auto benchmark = [&](auto func, const char* name) {
            auto start = std::chrono::high_resolution_clock::now();
            double result = func(data);
            auto end = std::chrono::high_resolution_clock::now();
            
            double ms = std::chrono::duration<double, std::milli>(end - start).count();
            double gflops = SIZE / (ms * 1e6);
            
            std::cout << name << ":\n";
            std::cout << "  Tempo: " << ms << " ms\n";
            std::cout << "  Performance: " << gflops << " GFLOPS\n";
            std::cout << "  Resultado: " << result << "\n\n";
            
            return ms;
        };
        
        double baseline_time = benchmark(baseline, "Baseline");
        double unrolled_time = benchmark(unrolled, "Unrolled");
        double intensity_time = benchmark(higher_intensity, "Higher Intensity");
        double blocked_time = benchmark(blocked, "Blocked");
        
        std::cout << "Speedups:\n";
        std::cout << "  Unrolled: " << baseline_time/unrolled_time << "x\n";
        std::cout << "  Intensity: " << baseline_time/intensity_time << "x\n";
        std::cout << "  Blocked: " << baseline_time/blocked_time << "x\n";
    }
};
```

## 📊 Árvore de Decisão para Otimização

```
Onde está seu ponto no Roofline?
│
├─ Abaixo de qualquer linha?
│  └─ Problema de implementação
│     ├─ Verificar padrões de acesso
│     ├─ Verificar alinhamento
│     └─ Verificar cache misses
│
├─ Na linha de memória?
│  ├─ Saturando bandwidth?
│  │  └─ Aumentar intensidade aritmética
│  └─ Não saturando?
│     ├─ Melhorar localidade
│     ├─ Prefetching
│     └─ Estruturas compactas
│
└─ Na linha de computação?
   ├─ Vetorização
   ├─ Paralelização
   └─ Bibliotecas otimizadas
```

## 🎯 Checklist de Otimização Prática

### Para Memory-Bound
- [ ] Medir bandwidth atual vs teórico
- [ ] Verificar padrão de acesso (sequencial?)
- [ ] Implementar cache blocking
- [ ] Considerar AoS vs SoA
- [ ] Loop fusion onde possível
- [ ] Prefetching manual se necessário

### Para Compute-Bound
- [ ] Verificar flags de compilação
- [ ] Confirmar vetorização (-fopt-info-vec)
- [ ] Loop unrolling
- [ ] Considerar intrinsics
- [ ] Avaliar bibliotecas (MKL, BLAS)
- [ ] Paralelização (OpenMP, threads)

## 🔑 Mensagens Principais

1. **O Roofline é um guia, não um destino** - Use para identificar oportunidades
2. **Medir sempre** - Não assuma onde está o problema
3. **Otimização iterativa** - Cada mudança move o ponto
4. **Conhecer os limites** - Nem sempre dá para melhorar mais
5. **Escolher batalhas** - Foque onde há maior ganho potencial

## 💡 Conclusão: O Caminho para Performance

```
Estado Atual → Diagnóstico → Estratégia → Nova Medição
     ↑                                          ↓
     └──────────── Iterar até satisfeito ←─────┘
```

O Roofline Model transforma otimização de "tentativa e erro" em um processo sistemático e científico. Cada decisão é informada por onde você está e para onde pode ir.

---

**Próximo Módulo:** Data-Oriented Design - Como organizar seus dados para maximizar performance desde o início, ao invés de otimizar depois!