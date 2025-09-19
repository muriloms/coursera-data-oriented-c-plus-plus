# Solução Orientada a Dados - Material de Estudo Completo

## 🎯 A Estratégia: Grid Espacial com Ordenação

### Conceito Central
Manter partículas próximas no espaço também próximas na memória usando um grid espacial e ordenação.

## 📐 Design do Grid

### Configuração do Grid
```
Tamanho da célula = dc (distância de cutoff)
→ Círculo de raio dc cobre no máximo 9 células (3×3)
```

### Visualização
```
Grid Espacial:
┌───┬───┬───┬───┬───┐
│ 0 │ 1 │ 2 │ 3 │ 4 │
├───┼───┼───┼───┼───┤
│ 5 │ 6 │ 7 │ 8 │ 9 │
├───┼───┼───┼───┼───┤
│10 │11 │12 │13 │14 │
└───┴───┴───┴───┴───┘

Busca de vizinhos da célula 6:
┌───┬───┬───┐
│ 1 │ 2 │ 3 │  ← Verificar estas
├───┼───┼───┤     9 células apenas!
│ 5 │ 6 │ 7 │
├───┼───┼───┤
│ 9 │10 │11 │
└───┴───┴───┘
```

## 🔄 Algoritmo Completo

### Passo 1: Calcular Índice Linear
```cpp
int getGridIndex(Vec2 position, float cell_size, int grid_width) {
    int gx = position.x / cell_size;
    int gy = position.y / cell_size;
    return gy * grid_width + gx;
}
```

### Passo 2: Ordenar Partículas por Grid Index
```cpp
// Antes da ordenação (aleatório):
particles: [P(cell=8)][P(cell=2)][P(cell=6)][P(cell=2)][P(cell=0)]...

// Aplicar sort:
sort(particles.begin(), particles.end(),
     [&](const Particle& a, const Particle& b) {
         return getGridIndex(a.position) < getGridIndex(b.position);
     });

// Depois da ordenação (agrupado por célula):
particles: [P(cell=0)][P(cell=2)][P(cell=2)][P(cell=6)][P(cell=8)]...
```

### Passo 3: Construir Diretório
```cpp
vector<int> buildDirectory(const vector<Particle>& particles, int num_cells) {
    vector<int> directory(num_cells + 1, particles.size());
    
    for(int i = 0; i < particles.size(); i++) {
        int cell = getGridIndex(particles[i].position);
        if(directory[cell] == particles.size()) {
            directory[cell] = i;  // Marca início da célula
        }
    }
    
    // Propaga valores para células vazias
    for(int i = num_cells - 1; i >= 0; i--) {
        if(directory[i] == particles.size()) {
            directory[i] = directory[i + 1];
        }
    }
    
    return directory;
}
```

### Exemplo de Diretório
```
Particles vector (índice do grid em cinza):
[0][0][2][2][2][6][6][8][8][8]
 0  1  2  3  4  5  6  7  8  9

Directory vector:
Cell 0: início=0, fim=2  → partículas [0,2)
Cell 1: início=2, fim=2  → vazio
Cell 2: início=2, fim=5  → partículas [2,5)
Cell 3: início=5, fim=5  → vazio
Cell 4: início=5, fim=5  → vazio
Cell 5: início=5, fim=5  → vazio
Cell 6: início=5, fim=7  → partículas [5,7)
Cell 7: início=7, fim=7  → vazio
Cell 8: início=7, fim=10 → partículas [7,10)
```

### Passo 4: Encontrar Vizinhos
```cpp
vector<int> getNeighborIndices(int cell, const vector<int>& directory, 
                                int grid_width, int grid_height) {
    vector<int> neighbors;
    
    int cx = cell % grid_width;
    int cy = cell / grid_width;
    
    // Percorre grid 3×3 ao redor
    for(int dy = -1; dy <= 1; dy++) {
        for(int dx = -1; dx <= 1; dx++) {
            int nx = cx + dx;
            int ny = cy + dy;
            
            // Verifica limites
            if(nx >= 0 && nx < grid_width && 
               ny >= 0 && ny < grid_height) {
                int neighbor_cell = ny * grid_width + nx;
                
                // Adiciona índices desta célula
                int start = directory[neighbor_cell];
                int end = directory[neighbor_cell + 1];
                
                for(int i = start; i < end; i++) {
                    neighbors.push_back(i);
                }
            }
        }
    }
    
    return neighbors;
}
```

## 💻 Implementação Completa

```cpp
struct Particle {
    Vec2 position;
    Vec2 velocity;
    int id;
};

class SpatialGridSimulation {
private:
    vector<Particle> particles;
    vector<int> directory;
    float cell_size;
    int grid_width, grid_height;
    int num_cells;
    
    int getGridIndex(Vec2 pos) const {
        int gx = clamp(int(pos.x / cell_size), 0, grid_width - 1);
        int gy = clamp(int(pos.y / cell_size), 0, grid_height - 1);
        return gy * grid_width + gx;
    }
    
public:
    SpatialGridSimulation(float world_width, float world_height, float cutoff) 
        : cell_size(cutoff) {
        grid_width = ceil(world_width / cell_size);
        grid_height = ceil(world_height / cell_size);
        num_cells = grid_width * grid_height;
        directory.resize(num_cells + 1);
    }
    
    void simulateStep(float dt, float k, float dc) {
        // 1. Ordenar por célula do grid
        sort(particles.begin(), particles.end(),
             [this](const Particle& a, const Particle& b) {
                 return getGridIndex(a.position) < getGridIndex(b.position);
             });
        
        // 2. Construir diretório
        fill(directory.begin(), directory.end(), particles.size());
        for(int i = 0; i < particles.size(); i++) {
            int cell = getGridIndex(particles[i].position);
            if(directory[cell] == particles.size()) {
                directory[cell] = i;
            }
        }
        
        // 3. Calcular forças célula por célula
        vector<Vec2> accelerations(particles.size(), {0, 0});
        
        for(int cell = 0; cell < num_cells; cell++) {
            int start = directory[cell];
            int end = directory[cell + 1];
            
            if(start >= end) continue;  // Célula vazia
            
            // Para cada partícula nesta célula
            for(int i = start; i < end; i++) {
                Vec2 acc = {0, 0};
                
                // Verificar células vizinhas
                int cx = cell % grid_width;
                int cy = cell / grid_width;
                
                for(int dy = -1; dy <= 1; dy++) {
                    for(int dx = -1; dx <= 1; dx++) {
                        int nx = cx + dx;
                        int ny = cy + dy;
                        
                        if(nx >= 0 && nx < grid_width && 
                           ny >= 0 && ny < grid_height) {
                            int ncell = ny * grid_width + nx;
                            int nstart = directory[ncell];
                            int nend = directory[ncell + 1];
                            
                            // Calcular força com partículas desta célula
                            for(int j = nstart; j < nend; j++) {
                                if(i == j) continue;
                                
                                Vec2 d = particles[j].position - 
                                        particles[i].position;
                                float dist2 = d.x * d.x + d.y * d.y;
                                
                                if(dist2 < dc * dc && dist2 > 0) {
                                    float dist3 = dist2 * sqrt(dist2);
                                    acc += k * d / dist3;
                                }
                            }
                        }
                    }
                }
                
                accelerations[i] = acc;
            }
        }
        
        // 4. Atualizar velocidades e posições
        for(int i = 0; i < particles.size(); i++) {
            particles[i].velocity += accelerations[i] * dt;
            particles[i].position += particles[i].velocity * dt;
        }
    }
};
```

## 📊 Análise de Complexidade

### Comparação
| Operação | Naive | Grid Espacial |
|----------|-------|---------------|
| Busca vizinhos | O(N) | O(k) médio |
| Total por partícula | O(N) | O(k) |
| Total geral | O(N²) | O(N·k) |
| Ordenação | - | O(N log N)* |

*Mas vetor já está quase ordenado → O(N) na prática!

### Performance Real
```
N = 10,000 partículas
Densidade média = 10 partículas/célula

Naive: 100,000,000 comparações
Grid: 10,000 × 90 = 900,000 comparações
Speedup: ~111x!
```

## 🎯 Vantagens da Solução

1. **Cache Efficient** - Partículas vizinhas contíguas
2. **Paralelizável** - Células independentes
3. **Usa STL** - sort() otimizado
4. **Escalável** - O(N) com densidade constante
5. **Simples** - Sem estruturas complexas

## ⚠️ Detalhes de Implementação

### Condições de Contorno
```cpp
// Opção 1: Walls (reflexão)
if(pos.x < 0 || pos.x > world_width) {
    velocity.x = -velocity.x;
}

// Opção 2: Periodic (wrap-around)
pos.x = fmod(pos.x + world_width, world_width);

// Opção 3: Open (partículas podem sair)
// Apenas ignorar
```

### Otimizações Adicionais
1. **Neighbor lists** - Cachear vizinhos por várias iterações
2. **Verlet lists** - Buffer de segurança para reduzir rebuilds
3. **Multi-threading** - Processar células em paralelo
4. **SIMD** - Vetorizar cálculos de força

---

**Conclusão:** Esta solução transforma um problema O(N²) intratável em O(N) escalável, mantendo excelente localidade de memória e compatibilidade com paralelização moderna!