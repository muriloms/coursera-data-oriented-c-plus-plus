# Modelo Baseado em Agentes Complexo - Material de Estudo Completo

## 🎯 O Desafio: Modelar um Cruzamento de Trânsito

Vamos modelar uma interseção complexa para otimizar semáforos, considerando:
- Diferentes tipos de entidades (carros, pedestres, ônibus, táxis)
- Regras heterogêneas (faixas de ônibus, faixas de pedestres)
- Física dos veículos
- Comportamento humano (distração, propensão ao risco)
- Performance (muitas simulações necessárias)

## 🏗️ Abordagem Orientada a Objetos (Tradicional)

### Hierarquia de Classes

```cpp
// Classe base abstrata
struct TrafficEntity {
    Vec3 position;
    Vec3 target_position;
    float speed;
    
    // Método virtual puro - deve ser sobrescrito
    virtual Vec3 update(vector<TrafficEntity*> neighbors) = 0;
};
```

### Diagrama UML Completo

```
                TrafficEntity
                (abstract)
                ├─ position
                ├─ target_position
                ├─ speed
                └─ update() = 0
                     ↑
        ┌────────────┼────────────┐
        ↑                         ↑
    Vehicle                   Pedestrian
    (abstract)                    ├─ inherits Person
    ├─ driver*→Person            └─ update()
    ├─ space_occupied
    ├─ acceleration
    ├─ deceleration
    └─ update() = 0
         ↑
    ┌────┼────┬──────┐
    ↑         ↑      ↑
   Car      Bus    Taxi
   └update() └update() └update()
   
   Person
   ├─ attention_score
   └─ recklessness_score
```

### Implementação OOP

```cpp
// Pessoa (comportamento humano)
struct Person {
    float attention_score;    // 0-1 (baixo = distraído)
    float recklessness_score; // 0-1 (alto = arriscado)
};

// Entidade de tráfego abstrata
struct TrafficEntity {
    Vec3 position;
    Vec3 target_position;
    float speed;
    
    virtual ~TrafficEntity() = default;
    virtual Vec3 update(const vector<TrafficEntity*>& neighbors) = 0;
};

// Veículo abstrato
struct Vehicle : public TrafficEntity {
    Person* driver;        // Ponteiro para motorista
    float space_occupied;  // Espaço no lane
    float acceleration;
    float deceleration;
    
    // Ainda abstrato - filhos implementam update()
};

// Implementações concretas
struct Car : public Vehicle {
    Vec3 update(const vector<TrafficEntity*>& neighbors) override {
        // Lógica específica do carro
        Vec3 new_velocity = calculateCarBehavior(neighbors);
        
        // Considera comportamento do motorista
        if(driver->attention_score < 0.5) {
            // Reação atrasada
            new_velocity *= 0.8;
        }
        
        if(driver->recklessness_score > 0.7) {
            // Freia tarde
            new_velocity *= 1.2;
        }
        
        return new_velocity;
    }
};

struct Bus : public Vehicle {
    Vec3 update(const vector<TrafficEntity*>& neighbors) override {
        // Lógica específica do ônibus
        // - Pode usar faixa exclusiva
        // - Para em pontos específicos
        // - Mais conservador
        return calculateBusBehavior(neighbors);
    }
};

struct Taxi : public Vehicle {
    Vec3 update(const vector<TrafficEntity*>& neighbors) override {
        // Lógica do táxi
        // - Mais agressivo
        // - Para em qualquer lugar
        return calculateTaxiBehavior(neighbors);
    }
};

// Pedestre herda de ambos
struct Pedestrian : public TrafficEntity, public Person {
    Vec3 update(const vector<TrafficEntity*>& neighbors) override {
        // Física mais simples
        // - Para instantaneamente
        // - Usa calçadas e faixas
        return calculatePedestrianBehavior(neighbors);
    }
};
```

### Simulação Principal (OOP)

```cpp
class TrafficSimulation {
private:
    vector<TrafficEntity*> entities;  // Polimorfismo via ponteiros
    
public:
    void addEntity(TrafficEntity* entity) {
        entities.push_back(entity);
    }
    
    void simulateStep() {
        // Atualiza cada entidade
        for(auto* entity : entities) {
            Vec3 new_velocity = entity->update(entities);
            entity->position += new_velocity * dt;
        }
    }
    
    void run(int steps) {
        for(int i = 0; i < steps; i++) {
            simulateStep();
        }
    }
};
```

## ⚠️ Problemas da Abordagem OOP

### 1. Layout de Memória Fragmentado

```
RAM (caótico):
┌────┐     ┌────┐     ┌──────┐     ┌────┐
│Car1│ ... │Bus1│ ... │Person│ ... │Car2│
└────┘     └────┘     └──────┘     └────┘
  ↑          ↑           ↑            ↑
Ponteiros apontando para todo lugar!

Cache misses constantes!
```

### 2. Overhead de Virtualização

```cpp
// Cada chamada virtual:
entity->update(neighbors);  
// ↓
// 1. Carrega ponteiro para objeto
// 2. Carrega vtable pointer
// 3. Busca função na vtable
// 4. Chama função indiretamente
// = 3-4 acessos à memória extras!
```

### 3. Indireções Múltiplas

```cpp
// Para acessar recklessness de um motorista de carro:
entities[i]->driver->recklessness_score;
//        ↑       ↑
//   Indireção 1  Indireção 2
```

### 4. Perda de Localidade

```cpp
// Processando entidades:
for(auto* entity : entities) {
    entity->update();  // Cada uma em lugar diferente da RAM
}
```

## 📊 Análise de Performance Esperada

### Métricas OOP
```
Operação                | Cache Misses | Tempo
------------------------|--------------|--------
Update all entities     | Alto (90%)   | 100ms
Check collisions        | Alto (85%)   | 80ms
Apply physics           | Alto (80%)   | 60ms
Total frame time        | -            | 240ms
FPS                     | -            | 4.2
```

### Problemas Identificados:
1. **Virtual calls:** ~20% do tempo
2. **Cache misses:** ~60% do tempo
3. **Indireções:** ~15% do tempo
4. **Útil:** Apenas 5% do tempo!

## 🤔 Por Que OOP Falha Aqui?

### OOP Otimiza para:
- ✅ Modularidade
- ✅ Reusabilidade
- ✅ Encapsulamento
- ✅ Extensibilidade

### Mas Sacrifica:
- ❌ Localidade de dados
- ❌ Acesso sequencial
- ❌ Cache efficiency
- ❌ Performance

## 💡 Conflito Fundamental

```
OOP: "Agrupe dados com comportamento"
     ↓
     Objetos espalhados na memória
     
DOD: "Agrupe dados por padrão de acesso"
     ↓
     Dados contíguos e eficientes
```

## 🎓 Lições do Modelo OOP

### Vantagens:
1. **Código intuitivo** - Modela o mundo real
2. **Fácil extensão** - Adicionar novo tipo é trivial
3. **Encapsulamento** - Cada objeto gerencia seu estado

### Desvantagens:
1. **Performance ruim** - Cache unfriendly
2. **Muitas indireções** - Ponteiros everywhere
3. **Overhead de virtualização** - Vtables custosas
4. **Fragmentação de memória** - Dados espalhados

## 🔄 Preparando para Transformação

### Perguntas para Data-Oriented Design:

1. **Quais dados são acessados juntos?**
   - Posições de todas entidades (colisão)
   - Velocidades de todas entidades (física)

2. **Quais operações são mais frequentes?**
   - Update de posição (todos frames)
   - Detecção de colisão (todos frames)
   - Mudança de comportamento (raro)

3. **Onde está o hot path?**
   - 90% do tempo: update + colisão
   - 10% do tempo: lógica específica

---

**Próximo Vídeo:** Vamos transformar este design OOP em Data-Oriented Design e ver melhorias de 10-100x em performance!