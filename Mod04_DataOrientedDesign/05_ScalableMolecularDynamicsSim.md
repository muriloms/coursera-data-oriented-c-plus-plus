# Simulação de Dinâmica Molecular Escalável - Material de Estudo Completo

## 🎯 O Problema: Simulação N-Body

### Requisitos do Sistema
- Partículas rastreadas individualmente
- Mecânica Newtoniana com interação par-a-par
- Massas idênticas, espaço 2D
- Força proporcional ao inverso do quadrado da distância

### Física do Modelo

```
Força entre partículas i e j:
F_ij = k * (r_j - r_i) / |r_j - r_i|³

Onde:
- k: constante (positiva = repulsão, negativa = atração)
- r_i, r_j: vetores posição
- d_ij = r_j - r_i: vetor deslocamento
```

### Integração Numérica (Método de Euler)
```
a_i = Σ F_ij / m
v_i(t+Δt) = v_i(t) + a_i * Δt
r_i(t+Δt) = r_i(t) + v_i * Δt
```

## ❌ Implementação Ingênua

### Estrutura Básica
```cpp
struct Particle {
    Vec2 position;
    Vec2 velocity;
    int id;  // Para rastrear partículas
};

vector<Particle> particles;
```

### Cálculo de Força Ingênuo
```cpp
Vec2 computeAcceleration(int i, const vector<Particle>& particles) {
    Vec2 acceleration = {0, 0};
    
    for(int j = 0; j < particles.size(); j++) {
        if(i == j) continue;
        
        Vec2 d = particles[j].position - particles[i].position;
        float dist2 = d.x * d.x + d.y * d.y;
        float dist3 = dist2 * sqrt(dist2);
        
        acceleration += k * d / dist3;
    }
    
    return acceleration;
}

void simulateStep(vector<Particle>& particles, float dt) {
    vector<Vec2> accelerations(particles.size());
    
    // O(N²) - PROBLEMA!
    for(int i = 0; i < particles.size(); i++) {
        accelerations[i] = computeAcceleration(i, particles);
    }
    
    // Atualiza velocidades e posições
    for(int i = 0; i < particles.size(); i++) {
        particles[i].velocity += accelerations[i] * dt;
        particles[i].position += particles[i].velocity * dt;
    }
}
```

### Complexidade: O(N²)
```
N partículas → N×(N-1) interações
1,000 partículas = 999,000 cálculos
10,000 partículas = 99,990,000 cálculos!
100,000 partículas = 9,999,900,000 cálculos!!
```

## 🎯 Otimização com Cutoff

### Conceito
```
Força decai com 1/r²
→ Partículas distantes contribuem pouco
→ Ignorar além de distância dc (cutoff)
```

### Tentativa de Otimização (Ainda Ruim!)
```cpp
Vec2 computeAccelerationCutoff(int i, const vector<Particle>& particles, float dc) {
    Vec2 acceleration = {0, 0};
    float dc2 = dc * dc;
    
    for(int j = 0; j < particles.size(); j++) {
        if(i == j) continue;
        
        Vec2 d = particles[j].position - particles[i].position;
        float dist2 = d.x * d.x + d.y * d.y;
        
        if(dist2 < dc2) {  // Teste de cutoff
            float dist3 = dist2 * sqrt(dist2);
            acceleration += k * d / dist3;
        }
    }
    
    return acceleration;
}
```

### Por Que Ainda é Ruim?
```
AINDA O(N²) para verificar distâncias!
- Acessa todas as partículas
- Cache misses para partículas distantes
- Apenas economiza cálculo, não acesso à memória
```

## 💡 O Desafio Real

### Objetivos:
1. **Localidade espacial = Localidade na memória**
2. **Usar algoritmos C++ (paralelizável)**
3. **Overhead razoável**
4. **Escalabilidade real**

## 🔍 Análise do Padrão de Acesso

### Observação Chave
```
Partícula i interage apenas com vizinhas próximas (< dc)
→ Não precisa verificar TODAS as partículas
→ Precisa encontrar vizinhas RAPIDAMENTE
```

### Problema Fundamental
```cpp
// Layout linear não reflete proximidade espacial!
particles: [P0][P1][P2][P3][P4][P5]...

Espacialmente:
P0 próximo de P37, P92, P156...
P1 próximo de P5, P88, P203...

Memória não reflete espaço!
```

## 🎯 Soluções Possíveis

### 1. Spatial Hashing / Grid
```
Dividir espaço em células
Partículas na mesma célula = próximas na memória
Buscar apenas células vizinhas
```

### 2. Neighbor Lists
```
Pré-computar lista de vizinhos
Atualizar periodicamente
Trade-off: memória vs computação
```

### 3. Tree Structures (Barnes-Hut)
```
Quadtree/Octree para organizar espaço
Aproximar grupos distantes
O(N log N) ao invés de O(N²)
```

### 4. Sorting Espacial
```
Ordenar partículas por coordenada
Usar Z-order (Morton code)
Mantém proximidade espacial
```

## 📊 Comparação de Abordagens

| Método | Complexidade | Memória | Cache | Paralelização |
|--------|-------------|---------|--------|--------------|
| Naive | O(N²) | O(N) | Péssimo | Difícil |
| Grid | O(N) médio | O(N+G) | Bom | Fácil |
| Neighbor List | O(N) | O(N×k) | Ótimo | Médio |
| Tree | O(N log N) | O(N) | Médio | Difícil |
| Sort | O(N log N) | O(N) | Bom | Médio |

## 💻 Preview: Solução com Grid Espacial

```cpp
// Estrutura de grid
struct SpatialGrid {
    float cell_size;  // >= cutoff distance
    int grid_width, grid_height;
    vector<vector<int>> cells;  // Índices de partículas por célula
    
    int getCellIndex(Vec2 pos) {
        int x = pos.x / cell_size;
        int y = pos.y / cell_size;
        return y * grid_width + x;
    }
    
    void rebuild(const vector<Particle>& particles) {
        // Limpa grid
        for(auto& cell : cells) cell.clear();
        
        // Adiciona partículas às células
        for(int i = 0; i < particles.size(); i++) {
            int cell = getCellIndex(particles[i].position);
            cells[cell].push_back(i);
        }
    }
    
    vector<int> getNeighbors(Vec2 pos) {
        vector<int> neighbors;
        int cx = pos.x / cell_size;
        int cy = pos.y / cell_size;
        
        // Verifica 9 células vizinhas (2D)
        for(int dy = -1; dy <= 1; dy++) {
            for(int dx = -1; dx <= 1; dx++) {
                int nx = cx + dx;
                int ny = cy + dy;
                if(nx >= 0 && nx < grid_width && 
                   ny >= 0 && ny < grid_height) {
                    int cell = ny * grid_width + nx;
                    neighbors.insert(neighbors.end(),
                                   cells[cell].begin(),
                                   cells[cell].end());
                }
            }
        }
        return neighbors;
    }
};
```

## 🎓 Lições para Data-Oriented Design

1. **O(N²) não escala** - Sempre busque algoritmos melhores
2. **Localidade espacial ≠ Localidade na memória** - Precisa estrutura auxiliar
3. **Trade-offs são inevitáveis** - Memória vs Computação
4. **Cache efficiency importa mais que complexidade** - O(N) com cache ruim pode perder para O(N log N) cache-friendly

---

**Próximo Vídeo:** Implementação completa da solução otimizada com análise de performance!