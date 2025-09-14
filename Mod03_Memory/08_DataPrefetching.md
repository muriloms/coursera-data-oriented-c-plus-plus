# Data Prefetching e Acesso Linear - Material de Estudo Completo

## 🎯 A Grande Questão

Cache existe, mas qual é o **benefício real** em aplicações práticas? Vamos comparar acesso aleatório vs linear e descobrir como o prefetching transforma completamente a performance.

## 📐 Princípios de Localidade Revisitados

### Localidade Temporal
```
"Dados acessados uma vez serão acessados novamente em breve"
→ Mantenha no cache após primeiro uso
```

### Localidade Espacial
```
"Dados próximos ao acessado serão necessários em breve"
→ RAM entrega memory rows completos
→ Cache carrega cache lines inteiras (64 bytes)
→ CPU faz prefetch automático
```

## 🔮 Prefetching: A Mágica do Hardware

### Tipos de Prefetching

#### 1. Prefetching Sequencial
```cpp
// CPU detecta padrão: i, i+1, i+2...
for(int i = 0; i < size; i++) {
    process(data[i]);  // CPU já buscou data[i+1], data[i+2]...
}
```

#### 2. Prefetching com Stride
```cpp
// CPU detecta padrão: i, i+k, i+2k...
for(int i = 0; i < size; i += stride) {
    process(data[i]);  // CPU tenta prever próximo salto
}
```

## 📊 Resultados Experimentais: Linear vs Aleatório

### O Benchmark Modificado

```cpp
// Mesmo código, mas inicialização diferente:

// ANTES (Aleatório):
data[0] = 847;  data[1] = 123;  data[2] = 555; ...

// AGORA (Linear):
data[0] = 1;    data[1] = 2;    data[2] = 3; ...

// Loop continua igual:
for(size_t iter = 0; iter < n; iter++) {
    i = data[i];  // Agora segue ordem sequencial
    sum += i;
}
```

### Gráfico Comparativo

```
Tempo (ns)
   ↑
100│     Random ──────────────────────
   │            ╱
   │          ╱
 10│        ╱
   │      ╱
   │    ╱
  2│──────────── Linear (com prefetch!)
  1│
   └────────────────────────────────────→ Tamanho
    L1↑      L2↑        L3↑         RAM
```

### Análise dos Resultados

| Região | Random | Linear | Speedup |
|--------|--------|--------|---------|
| L1 (≤32KB) | 1.5 ns | 1.5 ns | 1x |
| L2 (≤1MB) | 3.4 ns | **1.5 ns** | 2.3x |
| L3 (≤37MB) | 21 ns | **1.5 ns** | 14x |
| RAM (>37MB) | 112 ns | **2 ns** | **56x!** |

## 🚀 Como o Prefetching Esconde Latência

### Mecanismo em L2

```
Timeline de execução:
─────────────────────────────────────────────
CPU:     [Processa CL0 (8 elementos)]
Prefetch:         [Busca CL1 de L2→L1]
─────────────────────────────────────────────
CPU:              [Processa CL1 (já em L1!)]
Prefetch:                  [Busca CL2 de L2→L1]
─────────────────────────────────────────────

Resultado: Sempre paga apenas latência L1!
```

### Pipeline de Prefetch

```
┌──────────┐  Prefetch  ┌──────────┐  Prefetch  ┌──────────┐
│   RAM    │ ────────→  │    L3    │ ────────→  │    L2    │
└──────────┘            └──────────┘            └──────────┘
                                                       ↓
                                                  Prefetch
                                                       ↓
┌──────────┐            ┌──────────┐            ┌──────────┐
│   CPU    │ ←────────  │    L1    │ ←────────  │ Pipeline │
└──────────┘   Usa      └──────────┘   Ready    └──────────┘
```

## 🔬 Experimento com Padding (Stride Access)

### Estrutura com Padding

```cpp
struct Element {
    int64_t next;              // 8 bytes úteis
    int64_t padding[NPAD];     // NPAD * 8 bytes desperdiçados
};
```

### Casos Testados

#### Caso 1: NPAD = 0 (Sem padding)
```
Memória: [next][next][next][next]...
Cache Line: |←── 8 elementos ──→|
Eficiência: 100%
```

#### Caso 2: NPAD = 7 (1 elemento por cache line)
```
Memória: [next|padding×7][next|padding×7]...
Cache Line: |←─ 1 elemento ─→|
Eficiência: 12.5%
```

#### Caso 3: NPAD = 15 (2 cache lines por elemento)
```
Memória: [next|padding×15         ]...
Cache Lines: |←─── elemento ───→|←─→|
Eficiência: 6.25%
```

### Resultados do Padding

```cpp
Performance por padding:
┌─────────┬──────────┬─────────────┬──────────┐
│ NPAD    │ Bytes/el │ L2 Latência │ RAM Lat. │
├─────────┼──────────┼─────────────┼──────────┤
│ 0       │ 8        │ 1.5 ns      │ 2 ns     │
│ 7       │ 64       │ 3.4 ns      │ 4 ns     │
│ 15      │ 128      │ 3.4 ns      │ 15 ns    │
└─────────┴──────────┴─────────────┴──────────┘
```

## ⚠️ Limitações do Prefetching

### 1. Limites de Page Boundary
```
Memory Page (4KB)
┌────────────────────┐┌────────────────────┐
│  Prefetch OK ✓     ││  Novo page ✗       │
└────────────────────┘└────────────────────┘
                      ↑
            Prefetcher para aqui!
```

### 2. TLB Pressure
```cpp
// Muitas páginas = Muitas traduções = TLB misses
Large stride → Mais páginas tocadas → TLB thrashing
```

### 3. Stride Detection Limits
```cpp
// Funciona bem:
stride = 1, 2, 4, 8, 16...

// Problemático:
stride = 17, 23, 31...  // Padrões irregulares
stride > 2KB            // Muito grande
```

## 💻 Código Completo do Benchmark

```cpp
#include <iostream>
#include <vector>
#include <chrono>
#include <iomanip>

template<int NPAD>
class StrideBenchmark {
private:
    struct Element {
        int64_t next;
        int64_t padding[NPAD];
    };
    
    std::vector<Element> data;
    size_t num_elements;
    
public:
    StrideBenchmark(size_t total_bytes) {
        num_elements = total_bytes / sizeof(Element);
        data.resize(num_elements);
        
        // Inicialização linear
        for(size_t i = 0; i < num_elements - 1; i++) {
            data[i].next = i + 1;
        }
        data[num_elements - 1].next = 0;  // Fecha ciclo
    }
    
    double measureLatency() {
        using Clock = std::chrono::high_resolution_clock;
        
        // Warm-up
        int64_t idx = 0;
        for(size_t i = 0; i < num_elements; i++) {
            idx = data[idx].next;
        }
        
        // Medição
        auto start = Clock::now();
        
        int64_t sum = 0;
        idx = 0;
        const size_t iterations = 1000;
        
        for(size_t iter = 0; iter < iterations; iter++) {
            for(size_t i = 0; i < num_elements; i++) {
                idx = data[idx].next;
                sum += idx;
            }
        }
        
        auto end = Clock::now();
        
        // Evita otimização
        if(sum == 0) std::cout << "";
        
        auto ns = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start);
        return ns.count() / (double)(iterations * num_elements);
    }
    
    static void runComparison() {
        std::cout << "=== Comparação: Linear vs Random vs Stride ===\n\n";
        std::cout << "Tamanho\t\tLinear\tStride-8\tStride-16\n";
        std::cout << "───────\t\t──────\t────────\t─────────\n";
        
        std::vector<size_t> sizes = {
            16 * 1024,      // L1
            512 * 1024,     // L2
            8 * 1024 * 1024,  // L3
            128 * 1024 * 1024  // RAM
        };
        
        for(size_t size : sizes) {
            // Linear (NPAD=0)
            StrideBenchmark<0> linear(size);
            double t_linear = linear.measureLatency();
            
            // Stride de 64 bytes (NPAD=7)
            StrideBenchmark<7> stride8(size);
            double t_stride8 = stride8.measureLatency();
            
            // Stride de 128 bytes (NPAD=15)
            StrideBenchmark<15> stride16(size);
            double t_stride16 = stride16.measureLatency();
            
            // Output
            if(size < 1024*1024) {
                std::cout << size/1024 << " KB\t\t";
            } else {
                std::cout << size/(1024*1024) << " MB\t\t";
            }
            
            std::cout << std::fixed << std::setprecision(2)
                      << t_linear << "\t"
                      << t_stride8 << "\t\t"
                      << t_stride16 << "\n";
        }
    }
};

int main() {
    StrideBenchmark<0>::runComparison();
    return 0;
}
```

## 🎯 Otimizações Práticas

### 1. Structure of Arrays (SoA) vs Array of Structures (AoS)

```cpp
// RUIM - AoS com acesso parcial
struct Particle_AoS {
    float x, y, z;
    float vx, vy, vz;
    float mass;
    int id;
};
Particle_AoS particles[1000];

// Se só precisa posições:
for(int i = 0; i < 1000; i++) {
    process(particles[i].x);  // Desperdiça 87.5% da cache line!
}

// BOM - SoA
struct Particles_SoA {
    float x[1000], y[1000], z[1000];
    float vx[1000], vy[1000], vz[1000];
    float mass[1000];
    int id[1000];
};

// Acesso eficiente:
for(int i = 0; i < 1000; i++) {
    process(particles.x[i]);  // 100% da cache line útil!
}
```

### 2. Loop Fusion
```cpp
// RUIM - Múltiplas passadas
for(int i = 0; i < N; i++) a[i] *= 2;
for(int i = 0; i < N; i++) b[i] += a[i];
for(int i = 0; i < N; i++) c[i] = b[i] * b[i];

// BOM - Uma passada
for(int i = 0; i < N; i++) {
    a[i] *= 2;
    b[i] += a[i];
    c[i] = b[i] * b[i];
}
```

## 📊 Impacto no Mundo Real

### Aplicações Típicas

| Aplicação | Padrão de Acesso | Speedup com Prefetch |
|-----------|------------------|---------------------|
| Processamento de Imagem | Linear | 10-50x |
| Árvores Binárias | Aleatório | 1-2x |
| Multiplicação de Matrizes | Stride | 5-20x |
| Grafos | Irregular | 1-3x |
| Arrays/Vectors | Sequencial | 20-100x |

## 🎓 Conclusões Fundamentais

1. **Acesso linear é 50x mais rápido que aleatório**
2. **Prefetching esconde latência completamente até L3**
3. **Padding desperdiça bandwidth e quebra prefetching**
4. **Estruturas de dados determinam performance**
5. **Hardware moderno assume localidade**

## 💡 Regra de Ouro

> "Organize seus dados para acesso previsível e contíguo.  
> O hardware fará o resto do trabalho por você!"

## 🚀 Próximos Passos

Este conhecimento é fundamental para:
- **Data-Oriented Design** (próximo módulo)
- **SIMD/Vectorização** (próxima aula)
- **Programação paralela eficiente**
- **Otimização de aplicações reais**

---

💭 **Insight Final:** A diferença entre acesso linear e aleatório não é incremental - é transformacional. Um simples reordenamento de dados pode fazer seu código rodar 50x mais rápido sem mudar uma única linha de lógica!