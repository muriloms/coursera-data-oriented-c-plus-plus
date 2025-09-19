# Apresentação do Módulo: Algoritmos Paralelos - Material de Estudo Completo

## 📈 A Evolução Histórica da Performance

### O Gráfico Revelador (1995-2020)

```
Performance (log)
   ↑
1000│                           ╱─── Performance estagna
    │                      ╱────
100 │                 ╱────
    │            ╱────  
10  │       ╱────              ← 2004: Ponto de inflexão
    │  ╱────
1   │──
    └────────────────────────────→ Ano
     1995    2004    2010    2020

Clock (GHz)
4   │         ────────────────── Clock estagna em ~3-4 GHz
2   │    ╱────
1   │╱────
    └────────────────────────────→
```

### Análise por Período

#### Era 1: 1995-2004 (Era Dourada)
- **Crescimento:** 40x em 10 anos
- **Motor:** Aumento exponencial do clock
- **Benefício:** Programas ficavam automaticamente mais rápidos

#### Era 2: 2004-2020 (Era Multi-Core)
- **Crescimento:** 4x em 16 anos
- **Limitação:** Clock estagnado por limites físicos
- **Solução:** Múltiplos cores ao invés de clock mais rápido

## 🔬 Por Que o Clock Parou?

### Limites Físicos
```
Potência = Capacitância × Voltagem² × Frequência

Aumentar frequência → Mais calor
Diminuir transistores → Mais vazamento de corrente
Resultado: "Power Wall" em ~4 GHz
```

## 🎯 A Solução: Paralelismo

### De Single-Core para Multi-Core

```
2004: 1 core @ 3.6 GHz = 100% performance
2024: 8 cores @ 3.6 GHz = 800% performance (teórico)
      
Mas na prática...
Programa tradicional usa apenas 1 core = 100% performance!
```

### Exemplo: CPU AMD (8 Cores)
```
┌─────────────────────────────┐
│ ┌────┐ ┌────┐ ┌────┐ ┌────┐│
│ │Core│ │Core│ │Core│ │Core││
│ │ 0  │ │ 1  │ │ 2  │ │ 3  ││
│ └────┘ └────┘ └────┘ └────┘│
│ ┌────┐ ┌────┐ ┌────┐ ┌────┐│
│ │Core│ │Core│ │Core│ │Core││
│ │ 4  │ │ 5  │ │ 6  │ │ 7  ││
│ └────┘ └────┘ └────┘ └────┘│
└─────────────────────────────┘
```

### GPUs: Paralelismo Extremo
```
CPU: 8-64 cores poderosos
GPU: 1000-10000 cores simples

Filosofia diferente:
CPU = Poucos cavalos de corrida
GPU = Rebanho de ovelhas
```

## 💡 O Problema do Paralelismo

### Uso Típico Multi-Core (Ineficiente para Performance)
```
Core 0: Decodificar vídeo
Core 1: Rede social
Core 2: Antivírus
Core 3: [idle]
Core 4: [idle]
Core 5: [idle]
Core 6: [idle]
Core 7: [idle]

Sua aplicação científica: Usando apenas Core 0!
```

### O Que Precisamos
```
Aplicação única usando TODOS os cores:
Core 0: Processar dados[0:1000]
Core 1: Processar dados[1000:2000]
Core 2: Processar dados[2000:3000]
Core 3: Processar dados[3000:4000]
...
Speedup: ~8x!
```

## 🛠️ A Promessa dos Algoritmos Paralelos C++

### Antes (Sequencial)
```cpp
// Usa apenas 1 core
transform(data.begin(), data.end(), result.begin(),
          [](float x) { return expensive_computation(x); });
```

### Depois (Paralelo)
```cpp
// Usa TODOS os cores automaticamente!
transform(execution::par, 
          data.begin(), data.end(), result.begin(),
          [](float x) { return expensive_computation(x); });
```

**Uma mudança, speedup potencial de 8x!**

## 📚 O Que Vamos Aprender

### 1. Algoritmos Paralelos C++17/20
- `execution::par` - Paralelo em CPU
- `execution::par_unseq` - Paralelo + vetorizado
- `execution::unseq` - Vetorizado (SIMD)

### 2. Execução em GPU
- Mesmos algoritmos, hardware diferente
- Quando usar CPU vs GPU
- Pitfalls e otimizações

### 3. Problemas Comuns
- Race conditions
- False sharing
- Load balancing
- Sincronização excessiva

## ⚠️ Armadilhas do Paralelismo

### 1. Nem Tudo Paraleliza
```cpp
// Sequencial por natureza
for(int i = 1; i < n; i++) {
    data[i] = data[i-1] * 2;  // Depende do anterior!
}
```

### 2. Overhead Pode Dominar
```cpp
// Muito pequeno para paralelizar
parallel_for(0, 10, [](int i) {
    result[i] = i * 2;  // Overhead > benefício
});
```

### 3. Contenção de Memória
```cpp
// Todos os cores brigando pela mesma cache line
parallel_for(0, num_cores, [&sum](int i) {
    sum += data[i];  // Race condition + false sharing!
});
```

## 🎯 Quando o Paralelismo Vale a Pena

### Características Ideais:
1. **Granularidade grossa** - Muito trabalho por thread
2. **Independência** - Tarefas não se comunicam
3. **Load balance** - Trabalho dividido igualmente
4. **Data locality** - Cada core trabalha com seus dados

### Exemplos Perfeitos:
- Processamento de imagem (pixels independentes)
- Monte Carlo (simulações independentes)
- Transformações de array (map, reduce)
- Álgebra linear (multiplicação de matrizes)

## 📊 Expectativas Realistas

### Lei de Amdahl
```
Speedup = 1 / (S + P/N)

S = Fração sequencial
P = Fração paralela
N = Número de cores

Exemplo: 95% paralelo, 8 cores
Speedup = 1 / (0.05 + 0.95/8) = 5.9x (não 8x!)
```

## 🚀 Preview do Módulo

### O Que Vem Por Aí:
1. **Fundamentos** - Como funciona o paralelismo
2. **STL Paralelo** - Algoritmos prontos
3. **GPU Computing** - Milhares de cores
4. **Sincronização** - Evitando bugs
5. **Performance** - Medindo e otimizando
6. **Casos reais** - Aplicações científicas

---

**Mensagem Principal:** A era do "almoço grátis" acabou em 2004. Agora, para ganhar performance, você precisa abraçar o paralelismo. Este módulo mostra como fazer isso de forma eficiente e correta com C++ moderno!