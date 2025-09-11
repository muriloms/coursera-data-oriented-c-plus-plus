# Memória Cache - Material de Estudo Completo

## 🎯 O Problema Fundamental

A CPU opera em nanosegundos, mas a RAM demora centenas de ciclos para responder. Como resolver essa discrepância? A resposta: **memória cache** - um buffer intermediário menor, mas muito mais rápido.

## 🏗️ Arquitetura do Cache

### Visão Geral do Sistema

```
┌─────────┐    ┌─────────┐    ┌─────────┐    ┌─────────┐
│   CPU   │←──→│   L1    │←──→│   L2    │←──→│   L3    │←──→ RAM
│  Core   │    │  Cache  │    │  Cache  │    │  Cache  │
└─────────┘    └─────────┘    └─────────┘    └─────────┘
   ↑ Registradores (bytes)
      32 KB         1 MB         37 MB        394 GB
      1 ns         4 ns         12 ns        100 ns
```

### Como Funciona

```
CPU solicita dado no endereço 0x1234
                ↓
         L1 Cache tem?
         /          \
       SIM          NÃO (L1 miss)
        ↓            ↓
    Retorna      L2 Cache tem?
    (1 ns)       /          \
               SIM          NÃO (L2 miss)
                ↓            ↓
            Retorna      L3 Cache tem?
            (4 ns)       /          \
                       SIM          NÃO (L3 miss)
                        ↓            ↓
                    Retorna      Busca na RAM
                    (12 ns)      (100 ns)
```

## 📍 Princípios de Localidade

### 1. Localidade Temporal
**"Dados acessados recentemente serão acessados novamente em breve"**

```cpp
// Exemplo: variável usada repetidamente
for(int i = 0; i < 1000000; i++) {
    sum += array[i];  // 'sum' acessado 1 milhão de vezes
    count++;          // 'count' também
}
```

### 2. Localidade Espacial
**"Dados próximos ao acessado serão acessados em breve"**

```cpp
// Exemplo: processamento sequencial
int array[1000];
for(int i = 0; i < 1000; i++) {
    process(array[i]);  // Próximo: array[i+1]
}
```

## 📦 Cache Lines - A Unidade Fundamental

### O que é uma Cache Line?

Uma **cache line** é um bloco de dados transferido entre níveis de memória:
- **Tamanho típico:** 64 bytes
- **Capacidade:** 16 ints (4 bytes) ou 8 doubles (8 bytes)

```
Memória RAM:
┌──┬──┬──┬──┬──┬──┬──┬──┬──┬──┬──┬──┬──┬──┬──┬──┐
│00│01│02│03│04│05│06│07│08│09│0A│0B│0C│0D│0E│0F│...
└──┴──┴──┴──┴──┴──┴──┴──┴──┴──┴──┴──┴──┴──┴──┴──┘
            ↓ Acesso ao endereço 0x04
    ┌─────────────────────────────┐
    │ Cache line (64 bytes)       │
    │ Endereços 0x00 até 0x3F     │
    └─────────────────────────────┘
         Todos copiados para cache!
```

### Impacto na Performance

```cpp
// BOM - Aproveita cache line inteira
struct CompactData {
    float x, y, z, w;     // 16 bytes
    float vx, vy, vz, vw; // 16 bytes
    // Total: 32 bytes = meia cache line
};

// RUIM - Desperdiça cache line
struct SparseData {
    float value;        // 4 bytes
    char padding[60];   // 60 bytes desperdiçados
    // Total: 64 bytes, mas só 4 úteis!
};
```

## 🔍 Tipos de Cache e Associatividade

### Cache Totalmente Associativo
```
Qualquer bloco pode ir em qualquer linha
┌────────┬────────┬────────┬────────┐
│ Addr:X │ Addr:Y │ Addr:Z │ Addr:W │
│ Data   │ Data   │ Data   │ Data   │
└────────┴────────┴────────┴────────┘
✅ Flexível  ❌ Busca cara (O(n))
```

### Cache de Mapeamento Direto
```
Cada bloco tem posição fixa
Endereço % tamanho_cache = posição
✅ Busca rápida  ❌ Conflitos frequentes
```

### Cache Set-Associativo (Híbrido)
```
Múltiplos conjuntos, cada um parcialmente associativo
Melhor compromisso entre flexibilidade e velocidade
```

## 📊 Hierarquia de Cache Moderna

### Sistema Típico Intel (Exemplo do Curso)

| Nível | Tamanho | Latência | Compartilhamento | Características |
|-------|---------|----------|------------------|-----------------|
| **L1-D** | 32 KB | ~1 ns | Por core | Dados |
| **L1-I** | 32 KB | ~1 ns | Por core | Instruções |
| **L2** | 1 MB | ~4 ns | Por core | Unificado |
| **L3** | 37 MB | ~12 ns | Todos cores | Last Level Cache |
| **RAM** | 394 GB | ~100 ns | Sistema | Memória principal |

## ✍️ Operações de Escrita e Cache Dirty

### Fluxo de Escrita

```cpp
// CPU escreve em endereço que está em L1
data[i] = 42;
```

```
1. Escrita no L1 Cache
   ┌──────────────┐
   │ L1: valor=42 │ ← Marcado como "dirty"
   └──────────────┘
   
2. RAM ainda tem valor antigo
   ┌──────────────┐
   │ RAM: valor=0 │ ← Desatualizado temporariamente
   └──────────────┘
   
3. Write-back quando necessário
   - Quando linha é removida do cache
   - Quando explicitamente sincronizado
```

### Cascata de Write-backs

```
L1 Miss → Precisa evict linha dirty
         ↓
    Write-back para L2
         ↓
    L2 cheio? → Evict linha L2
         ↓
    Write-back para L3
         ↓
    L3 cheio? → Evict linha L3
         ↓
    Write-back para RAM
    
Resultado: Um simples read pode causar múltiplos writes!
```

## 💻 Código Exemplo: Impacto do Cache

```cpp
#include <iostream>
#include <vector>
#include <chrono>
#include <cstring>

class CacheDemo {
private:
    using Clock = std::chrono::high_resolution_clock;
    static constexpr size_t CACHE_LINE_SIZE = 64;
    
public:
    // Demonstra false sharing
    struct BadAlignment {
        alignas(64) int counter1;  // Thread 1
        int counter2;              // Thread 2 - mesma cache line!
    };
    
    struct GoodAlignment {
        alignas(64) int counter1;  // Thread 1
        alignas(64) int counter2;  // Thread 2 - cache lines diferentes
    };
    
    // Demonstra travessia de matriz
    static void row_major_traversal(int** matrix, int size) {
        int sum = 0;
        for(int i = 0; i < size; i++) {
            for(int j = 0; j < size; j++) {
                sum += matrix[i][j];  // Acesso sequencial na memória
            }
        }
    }
    
    static void column_major_traversal(int** matrix, int size) {
        int sum = 0;
        for(int j = 0; j < size; j++) {
            for(int i = 0; i < size; i++) {
                sum += matrix[i][j];  // Pula entre linhas - cache misses!
            }
        }
    }
    
    // Demonstra cache blocking
    static void matrix_multiply_naive(float** A, float** B, float** C, int n) {
        for(int i = 0; i < n; i++) {
            for(int j = 0; j < n; j++) {
                for(int k = 0; k < n; k++) {
                    C[i][j] += A[i][k] * B[k][j];
                }
            }
        }
    }
    
    static void matrix_multiply_blocked(float** A, float** B, float** C, int n) {
        const int BLOCK = 64;  // Ajustado para cache L1
        
        for(int ii = 0; ii < n; ii += BLOCK) {
            for(int jj = 0; jj < n; jj += BLOCK) {
                for(int kk = 0; kk < n; kk += BLOCK) {
                    // Mini multiplicação que cabe em cache
                    for(int i = ii; i < std::min(ii + BLOCK, n); i++) {
                        for(int j = jj; j < std::min(jj + BLOCK, n); j++) {
                            for(int k = kk; k < std::min(kk + BLOCK, n); k++) {
                                C[i][j] += A[i][k] * B[k][j];
                            }
                        }
                    }
                }
            }
        }
    }
};

// Exemplo de uso eficiente de cache
class CacheFriendlyContainer {
private:
    struct Node {
        int data[15];        // 60 bytes de dados úteis
        Node* next;          // 8 bytes ponteiro
        // Total: 68 bytes, mas alinhado em 64 (cache line)
    } __attribute__((packed, aligned(64)));
    
    std::vector<Node> nodes;  // Alocação contígua
    
public:
    void process_all() {
        // Percorre sequencialmente - cache friendly
        for(auto& node : nodes) {
            for(int i = 0; i < 15; i++) {
                node.data[i] *= 2;
            }
        }
    }
};
```

## 🎯 Otimizações Cache-Aware

### 1. Loop Tiling/Blocking
```cpp
// Processa dados em blocos que cabem em cache
void process_large_array(float* data, size_t size) {
    const size_t L1_SIZE = 32 * 1024 / sizeof(float);
    
    for(size_t block = 0; block < size; block += L1_SIZE) {
        size_t end = std::min(block + L1_SIZE, size);
        
        // Tudo cabe em L1
        for(size_t i = block; i < end; i++) {
            data[i] = complex_computation(data[i]);
        }
    }
}
```

### 2. Padding para Evitar Conflitos
```cpp
// Evita que arrays mapeiem para mesmas linhas de cache
class OptimizedMatrix {
    static constexpr int ROWS = 1024;
    static constexpr int COLS = 1024;
    static constexpr int PAD = 1;  // Evita power-of-2
    
    float data[ROWS][COLS + PAD];  // Padding previne conflitos
};
```

### 3. Prefetching
```cpp
void process_with_prefetch(int* data, size_t size) {
    for(size_t i = 0; i < size; i++) {
        // Dica para CPU trazer próximos dados
        __builtin_prefetch(&data[i + 8], 0, 3);
        
        // Processa dado atual
        result[i] = heavy_computation(data[i]);
    }
}
```

## 📈 Métricas de Performance

### Como Medir Cache Performance

```bash
# Linux - usando perf
perf stat -e cache-references,cache-misses ./program

# Métricas importantes:
# - Cache hit ratio: (hits / (hits + misses)) * 100
# - Miss penalty: ciclos perdidos por miss
# - Bandwidth utilization: bytes/segundo usado vs disponível
```

## 🎓 Conceitos-Chave

1. **Cache Line:** Unidade de transferência (64 bytes)
2. **Cache Miss:** Dado não encontrado, busca nível acima
3. **Cache Hit:** Dado encontrado, acesso rápido
4. **Dirty Line:** Modificada mas não sincronizada
5. **Eviction:** Remoção de linha para abrir espaço
6. **Write-back:** Sincronização de dados dirty
7. **Localidade:** Temporal e espacial

## 💡 Dicas Práticas

### DO's ✅
- Acesse dados sequencialmente
- Agrupe dados relacionados
- Use estruturas de tamanho power-of-2 com cuidado
- Processe dados em blocos cache-sized
- Alinhe estruturas críticas em cache lines

### DON'Ts ❌
- Evite stride patterns grandes
- Não desperdice espaço em cache lines
- Evite false sharing em código paralelo
- Não ignore localidade de referência

## 🔍 Exercícios

### Exercício 1: Cache Line Analysis
Quantas cache lines são necessárias para:
```cpp
struct Data {
    double values[100];  // 800 bytes
    int flags[50];       // 200 bytes
};
```

### Exercício 2: Otimização
Reescreva para melhor uso de cache:
```cpp
for(int i = 0; i < 1000; i++) {
    a[i] = b[i * 100] + c[i * 100];
}
```

### Exercício 3: False Sharing
Identifique e corrija o problema:
```cpp
struct Counters {
    int thread1_count;
    int thread2_count;
};
```

## 🎯 Conclusão

O cache é fundamental para performance moderna. A diferença entre código cache-friendly e cache-unfriendly pode ser 10x, 100x ou mais! 

**Lembre-se:** 
- Cache não é mágica - funciona com padrões previsíveis
- Localidade é a chave - temporal e espacial
- Hierarquia existe por uma razão - use cada nível eficientemente
- Medir é essencial - não adivinhe, profile!

---

💭 **Reflexão Final:** Um programador que entende cache pode fazer um Core i3 superar um Core i9 mal utilizado. O hardware moderno é incrivelmente poderoso, mas só entrega essa potência para quem sabe usá-lo!
